// Thread that reads data from CAN bus and make it available to GUI.
// SPDX-FileCopyrightText: 2023 Davide Rondini <info@drondini.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "canreader.h"

CanReader::CanReader(ClusterCANData* outputData, QMutex* outputDataMutex)
    : canData(outputData), dataMutex(outputDataMutex)
{
    /* canBus pointer is of abstract type CANInterface,
     * but currently there is only one interface implemented,
     * so go straight to allocation. In the future, multiple
     * CAN devices may be implemented. */
    canBus = (CANInterface*) new CANInterfaceSocketCAN;
}

CanReader::~CanReader()
{
    delete canBus;
}

void CanReader::run()
{
    while (true) {
        // Check for interruption if requested
        if (QThread::currentThread()->isInterruptionRequested()) {
            break;
        }
        // DEBUG increase values as counters, then sleep for a while
        dataMutex->lock();
        canData->rpm_raw = (canData->rpm_raw + 10) % 10000;
        canData->tps_raw = (canData->tps_raw + 1) % 100;
        dataMutex->unlock();
        usleep(100000);
    }
}
