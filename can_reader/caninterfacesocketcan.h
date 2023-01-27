/*
 * Abstract class representing a CAN bus interface.
 *
 * SPDX-FileCopyrightText: 2023 Davide Rondini <info@drondini.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef CANINTERFACESOCKETCAN_H
#define CANINTERFACESOCKETCAN_H

#include "caninterface.h"

#ifdef __linux__
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/utsname.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <linux/sockios.h>
#include <linux/net_tstamp.h>
#endif

class CANInterfaceSocketCAN : public CANInterface
{
	public:
    CANInterfaceSocketCAN();
    virtual ~CANInterfaceSocketCAN();
		
		virtual bool IsEnabled();
		virtual CommRetCode Open(const char* unixDevName, void* options);
		virtual CommRetCode Close();
		
    virtual CommRetCode ReadFrame(canfd_frame* outFrame);
    virtual CommRetCode WriteFrame(const canfd_frame* inFrame);
		
	protected:
		
		int socket_fd; // file descriptor for socket
		struct sockaddr_can addr;
		struct ifreq ifr;
		ssize_t numBytes;
		uint8_t frameMTU;
		fd_set fdSetSelect;
		
		// Structures for using recvmsg() syscall instead of read()
		char ctrlmsg[CMSG_SPACE(sizeof(struct timeval)) + CMSG_SPACE(sizeof(__u32))];
		struct iovec iov;
		struct msghdr msg;
		struct cmsghdr *cmsg;
		struct timeval tv;
		struct timespec* ts;
		struct timeval timeOut, *timeOutRef;
};

#endif // CANINTERFACESOCKETCAN_H
