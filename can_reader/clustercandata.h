// Structure for CAN data to be transfered to digital cluster.
// SPDX-FileCopyrightText: 2023 Davide Rondini <info@drondini.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef CAN_DATA_BUFFER_H
#define CAN_DATA_BUFFER_H

#include <cstdint>

typedef struct ClusterCANData_Struct
{
	uint32_t tps_raw;
	uint32_t rpm_raw;
} ClusterCANData;

#endif
