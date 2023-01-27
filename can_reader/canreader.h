// Thread that reads data from CAN bus and make it available to GUI.
// SPDX-FileCopyrightText: 2023 Davide Rondini <info@drondini.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef CANREADER_H
#define CANREADER_H

#include <clustercandata.h>
#include <QThread>
#include <QMutex>

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
    CanReader(ClusterCANData* outputData, QMutex* outputDataMutex);

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
};

// Q_DECLARE_TYPEINFO(CanReader, Q_MOVABLE_TYPE);

#endif // CANREADER_H
