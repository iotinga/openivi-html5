/*
 * Abstract class representing a CAN bus interface.
 *
 * SPDX-FileCopyrightText: 2023 Davide Rondini <info@drondini.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef CANINTERFACE_H
#define CANINTERFACE_H

#include <cstdint>

/* Define basic CAN frame structure: if linux, use kernel definition,
 * otherwise define it in the same way. As in Linux code, this is ready
 * to support also CAN-FD, even though at the moment it is not supported. */
#ifdef __linux__
#include <linux/can.h>
#else
#error SocketCAN is only supported under Linux OS
#endif

#define CAN_PAYLOAD_SIZE 	CANFD_MAX_DLEN
#define CAN_HEADER_SIZE		(sizeof(canfd_frame) - CAN_PAYLOAD_SIZE)

typedef enum CAN_Frame_Format_Enum {
	CAN_FRAME_11,
	CAN_FRAME_29
} CAN_Frame_Format;

typedef enum CAN_Bus_Mode_Enum {
	CAN_MODE_NORMAL = 0,
	CAN_MODE_SILENT = 1,
	CAN_MODE_OFF = 2,
	CAN_MODE_BUILTIN_OUT_LITTLE = 3,
	CAN_MODE_BUILTIN_OUT_BIG = 4
} CAN_Bus_Mode;

typedef enum CommRetCode_Enum {
	CRET_OK 									= 0,
	CRET_ERROR 								= 1,
	CRET_NOT_IMPLEMENTED			= 2,
	CRET_READ_ERROR						= 3,
	CRET_WRITE_ERROR					= 4,
	CRET_INVALID_DEVICE				= 5,
	CRET_NO_ACCESS						= 6,
	CRET_NOTHING_TO_READ			= 7,
	CRET_DEVICE_ERROR					= 8,
	CRET_DEVICE_INACTIVE			= 9,
	CRET_TOO_MANY_DATA				= 10,
	CRET_DISCONNECT						= 11,
	CRET_INVALID_DATA					= 12,
	CRET_INVALID_DATA_FORMAT	= 13,
	CRET_SIZE_MISMATCH				= 14,
	CRET_BUSY_WAIT						= 15,
	CRET_PARTIAL_READ					= 16,
	CRET_COMPLETE_READ				= 17
} CommRetCode;

typedef struct CANSetup_Struct {
	uint32_t baudrate;								// CAN baud rate
	CAN_Frame_Format frame_format;		// Standard or extended frame format
	CAN_Bus_Mode bus_mode;						// Bus mode: normal, silent, off
	bool enableCAN_FD;								// CAN-FD enabled
	int timeout_ms;										// Read Timeout [ms]
	struct can_filter *filterIDs;			// IDs to be filtered: it is an array of filters, no filter if set to NULL
	uint32_t filterCount;							// Number of filters set
} CANSetup;

class CANInterface
{
	public:
    CANInterface();
    virtual ~CANInterface();
		
		virtual bool IsEnabled() = 0;
		virtual CommRetCode Open(const char* unixDevName, void* options) = 0;
		virtual CommRetCode Close() = 0;
		
		virtual CommRetCode ReadFrame(canfd_frame* outFrame) = 0;
		virtual CommRetCode WriteFrame(const canfd_frame* inFrame) = 0;
		
	protected:
		// struct canfd_frame	frame;
		CANSetup canOptions;
};

#endif // CANINTERFACE_H
