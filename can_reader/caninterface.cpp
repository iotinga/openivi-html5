/*
 * Abstract class representing a CAN bus interface.
 *
 * SPDX-FileCopyrightText: 2023 Davide Rondini <info@drondini.com>
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <cstring>
#include "caninterface.h"

CANInterface::CANInterface()
{
	// Default options
	canOptions.baudrate = 500000;
	canOptions.frame_format = CAN_FRAME_11;
	canOptions.enableCAN_FD = false;
	canOptions.timeout_ms = 5000;
	canOptions.filterIDs = NULL;
	canOptions.filterCount = 0;
}

CANInterface::~CANInterface()
{
	if ((canOptions.filterCount > 0) && (canOptions.filterIDs != NULL)) {
		delete [] canOptions.filterIDs;
		canOptions.filterIDs = NULL;
	}
}
