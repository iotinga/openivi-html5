// Thread that reads data from CAN bus and make it available to GUI.
// SPDX-FileCopyrightText: 2023 Davide Rondini <info@drondini.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "canreader.h"

CanReader::CanReader(const char* busName, CANSetup* busSetup, ClusterCANData* outputData, QMutex* outputDataMutex)
    : canData(outputData), dataMutex(outputDataMutex)
{
    if (busName != NULL) {
        if (strlen(busName) > 0) {
            canBusName.assign(busName);
        } else {
            canBusName.assign("can0"); // fallback to can0
        }
    } else {
        canBusName.assign("can0"); // fallback to can0
    }
    if (busSetup != NULL) {
        /* Setup read from settings file. */
        portSetup.baudrate = busSetup->baudrate;
        portSetup.bus_mode = busSetup->bus_mode;
        portSetup.enableCAN_FD = busSetup->enableCAN_FD;
        portSetup.frame_format = busSetup->frame_format;
        portSetup.timeout_ms = busSetup->timeout_ms;
        portSetup.endianess = busSetup->endianess;
    } else {
        /* Default port setup. */
        portSetup.baudrate = 500000;
        portSetup.bus_mode = CAN_MODE_NORMAL;
        portSetup.enableCAN_FD = false;
        portSetup.frame_format = CAN_FRAME_11;
        portSetup.timeout_ms = 1000;
        portSetup.endianess = CAN_ENDIAN_BIG;
    }
    portSetup.filterCount = 0;
    portSetup.filterIDs = NULL;

    // Prepare the filter list to get only CAN IDs of interest
    size_t filterNumber = canData->can_filter_set.size();
    if (filterNumber > 0) {
        portSetup.filterCount = filterNumber;
        portSetup.filterIDs = new can_filter[filterNumber];
        int flIdx = 0;
        for (std::set<canid_t>::iterator fl = canData->can_filter_set.begin(); fl != canData->can_filter_set.end(); ++fl) {
           portSetup.filterIDs[flIdx].can_id = *fl;
           portSetup.filterIDs[flIdx].can_mask = 0x7FF;
           flIdx++;
        }
    }

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
    CommRetCode cRet = CRET_ERROR;
    struct canfd_frame canFrame;
    memset(&canFrame, 0x00, sizeof(struct canfd_frame));
    /* Open CAN interface */
    if (canBus != NULL) {
        cRet = canBus->Open(canBusName.c_str(), &portSetup);
        if (cRet == CRET_OK) {
            while (true) {
                // Check for interruption if requested
                if (QThread::currentThread()->isInterruptionRequested()) {
                    break;
                }
                // Read a CAN Frame
                // (wait until a frame is read or timeout_ms is reached)
                cRet = canBus->ReadFrame(&canFrame);
                if (cRet == CRET_OK) {
                    dataMutex->lock();
                    if (canData->rpm_ch) {
                        if (canFrame.can_id == canData->rpm_ch->GetCAN_ID()) {
                            canData->rpm_raw = canData->rpm_ch->GetValueFromCANFrame(&canFrame, portSetup.endianess);
                        }
                    }
                    if (canData->speed_ch) {
                        if (canFrame.can_id == canData->speed_ch->GetCAN_ID()) {
                            canData->speed_raw = canData->speed_ch->GetValueFromCANFrame(&canFrame, portSetup.endianess);
                        }
                    }
                    if (canData->vbat_ch) {
                        if (canFrame.can_id == canData->vbat_ch->GetCAN_ID()) {
                            canData->vbat_raw = canData->vbat_ch->GetValueFromCANFrame(&canFrame, portSetup.endianess);
                        }
                    }
                    if (canData->gear_ch) {
                        if (canFrame.can_id == canData->gear_ch->GetCAN_ID()) {
                            canData->gear_raw = canData->gear_ch->GetValueFromCANFrame(&canFrame, portSetup.endianess);
                        }
                    }
                    dataMutex->unlock();
                } else {
                    if (cRet != CRET_NOTHING_TO_READ) {
                        usleep(100000);
                    }
                }
            }
            /* Close CAN interface before exiting */
            canBus->Close();
        } else {
            // Cannot open interface
        }
    }
}
