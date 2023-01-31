// Thread that reads data from CAN bus and make it available to GUI.
// SPDX-FileCopyrightText: 2023 Davide Rondini <info@drondini.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef CANREADER_H
#define CANREADER_H

#include <clustercandata.h>
#include <QThread>
#include <QMutex>

#include <string>
#include <clustercandata.h>
#include <caninterfacesocketcan.h>

/**
 * @brief Thread that continuously reads CAN bus data
 */
class CanReader : public QThread
{
public:
    /**
     * Default constructor
     */
    CanReader(const char* busName, CANSetup* busSetup, ClusterCANData* outputData, QMutex* outputDataMutex);

    /**
     * Destructor
     */
    ~CanReader();



protected:
    /**
     * @todo write docs
     *
     * @return TODO
     */
    virtual void run();

private:
    ClusterCANData* canData;
    QMutex* dataMutex;

    // CAN bus interface
    CANInterface* canBus;
    CANSetup portSetup;
    std::string canBusName;
};

// Q_DECLARE_TYPEINFO(CanReader, Q_MOVABLE_TYPE);

#endif // CANREADER_H
