/*
 * Abstract class representing a CAN bus interface.
 *
 * SPDX-FileCopyrightText: 2023 Davide Rondini <info@drondini.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "caninterfacesocketcan.h"
#include <cstdio>
#include <cstring>

CANInterfaceSocketCAN::CANInterfaceSocketCAN() : CANInterface()
{
	socket_fd = -1;
	numBytes = 0;
	frameMTU = CAN_MTU;
	memset(&addr, 0x00, sizeof(addr));
	/* these settings are static and can be held out of the hot path */
	msg.msg_name = &addr;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = &ctrlmsg;
}

CANInterfaceSocketCAN::~CANInterfaceSocketCAN()
{

}

bool CANInterfaceSocketCAN::IsEnabled()
{
	return true;
}

CommRetCode CANInterfaceSocketCAN::Open(const char* unixDevName, void* options)
{
	/* if socket is already opened, do nothing. */
	if (socket_fd >= 0) {
		return CRET_OK;
	}
	if (options) {
		CANSetup* optionsIn = (CANSetup*) options;
		canOptions.baudrate = optionsIn->baudrate;
		canOptions.frame_format = optionsIn->frame_format;
		canOptions.enableCAN_FD = optionsIn->enableCAN_FD;
		canOptions.timeout_ms = optionsIn->timeout_ms;
		if ((optionsIn->filterCount > 0) && (optionsIn->filterIDs != NULL)) {
			if (canOptions.filterIDs != NULL) {
				delete [] canOptions.filterIDs;
				canOptions.filterIDs = NULL;
			}
			canOptions.filterCount = optionsIn->filterCount;
			canOptions.filterIDs = new struct can_filter[canOptions.filterCount];
			memcpy(canOptions.filterIDs, optionsIn->filterIDs, canOptions.filterCount*sizeof(struct can_filter));
		} else {
			canOptions.filterIDs = NULL;
			canOptions.filterCount = 0;
		}
		frameMTU = canOptions.enableCAN_FD ? CANFD_MTU : CAN_MTU;
	}
	// First of all, check that the interface is activated, looking at /sys/class/net/canX/operstate
	char canStatusFileName[256];
	char canStatus[] = {'\0', '\0', '\0'};
	const int canfd_on = 1;
	int timestamp_on = 1;
	timestamp_on = SOF_TIMESTAMPING_RX_HARDWARE | SOF_TIMESTAMPING_RAW_HARDWARE | SOF_TIMESTAMPING_SYS_HARDWARE | SOF_TIMESTAMPING_SOFTWARE | SOF_TIMESTAMPING_RX_SOFTWARE;
	// Set timeout
	timeOut.tv_sec = canOptions.timeout_ms/1000;
	timeOut.tv_usec = (canOptions.timeout_ms % 1000)*1000;
	memset(canStatusFileName, '\0', 256);
	snprintf(canStatusFileName, 256, "/sys/class/net/%s/operstate", unixDevName);
	FILE* canStatusFile = fopen(canStatusFileName, "rt");
	if (canStatusFile) {
		size_t readBytes = fread(canStatus, 1, 2, canStatusFile);
		// Close file in any case, it is no longer used
		fclose(canStatusFile);
		if (readBytes != 2) {
			return CRET_DEVICE_ERROR;
		} else {
			if (canStatus[0] == 'u') {
				// interface is up, proceed to open it
				socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
				if (socket_fd != -1) {
					memset(&ifr.ifr_name, 0, sizeof(ifr.ifr_name));
					strcpy(ifr.ifr_name, unixDevName);

					addr.can_family = AF_CAN;
					if (ioctl(socket_fd, SIOCGIFINDEX, &ifr) < 0) {
						return CRET_DEVICE_ERROR;
					}
					addr.can_ifindex = ifr.ifr_ifindex;
					
					/* Try to enable CAN-FD frames. */
					setsockopt(socket_fd, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd_on, sizeof(canfd_on));
					
					// Enable reading of hardware timestamps if available.
					// if (setsockopt(socket_fd, SOL_SOCKET, SO_TIMESTAMP,
					if (setsockopt(socket_fd, SOL_SOCKET, SO_TIMESTAMPING, &timestamp_on, sizeof(timestamp_on)) < 0) {
						return CRET_DEVICE_ERROR;
					}
					
					/* If set into CAN setup, apply ID filters */
					if ((canOptions.filterCount > 0) && (canOptions.filterIDs != NULL)) {
						setsockopt(socket_fd, SOL_CAN_RAW, CAN_RAW_FILTER, canOptions.filterIDs, canOptions.filterCount*sizeof(struct can_filter));
					}
					
					int bindRes = bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr));
					if (bindRes == 0) {
						return CRET_OK;
					} else {
						return CRET_DEVICE_ERROR;
					}
				} else {
					return CRET_DEVICE_ERROR;
				}
			} else {
				// interface is down, return error code
				return CRET_DEVICE_INACTIVE;
			}
		}
	} else {
		return CRET_INVALID_DEVICE;
	}
	// Should never arrive here
	return CRET_INVALID_DEVICE;
}

CommRetCode CANInterfaceSocketCAN::Close()
{
	if (socket_fd >= 0) {
		close(socket_fd);
		socket_fd = -1;
	}
	return CRET_OK;
}

CommRetCode CANInterfaceSocketCAN::ReadFrame(canfd_frame* outFrame)
{
	int ret = 0;
	iov.iov_base = outFrame;
	FD_ZERO(&fdSetSelect);
	FD_SET(socket_fd, &fdSetSelect);
	// Set timeout
	timeOut.tv_sec = canOptions.timeout_ms/1000;
	timeOut.tv_usec = (canOptions.timeout_ms % 1000)*1000;
	timeOutRef = &timeOut;
	
	ret = select(socket_fd + 1, &fdSetSelect, NULL, NULL, timeOutRef);
	if (ret < 0) {
		// perror("select");
		return CRET_READ_ERROR;
	} else if (ret == 0) {
		return CRET_NOTHING_TO_READ;
	}

	if (FD_ISSET(socket_fd, &fdSetSelect)) {
		/* these settings may be modified by recvmsg() */
		iov.iov_len = sizeof(*outFrame);
		msg.msg_namelen = sizeof(addr);
		msg.msg_controllen = sizeof(ctrlmsg);  
		msg.msg_flags = 0;

		numBytes = recvmsg(socket_fd, &msg, 0);

		if (numBytes < 0) {
			return CRET_READ_ERROR;
		}
		
		return CRET_OK;
	}

	return CRET_ERROR;
}

CommRetCode CANInterfaceSocketCAN::WriteFrame(const canfd_frame* inFrame)
{
	numBytes = write(socket_fd, (const void*) inFrame, frameMTU);

	if (numBytes < 0) {
		return CRET_WRITE_ERROR;
	}
	
	return CRET_OK;
}
