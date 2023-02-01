// Structure for CAN data to be transfered to digital cluster.
// SPDX-FileCopyrightText: 2023 Davide Rondini <info@drondini.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef CAN_DATA_BUFFER_H
#define CAN_DATA_BUFFER_H

#include <cstdint>
#include <set>
#include <canchannel.h>

typedef struct ClusterCANData_Struct
{
	std::set<canid_t> can_filter_set;
	CANChannel* speed_ch;
	CANChannel* rpm_ch;
	CANChannel* vbat_ch;
	uint32_t speed_raw;
	uint32_t rpm_raw;
	uint32_t vbat_raw;
} ClusterCANData;

#endif
