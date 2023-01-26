// Thread that reads data from CAN bus and make it available to GUI.
// SPDX-FileCopyrightText: 2023 Davide Rondini <info@drondini.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef CANREADER_H
#define CANREADER_H

#include <clustercandata.h>
#include <QThread>
// #include <QSharedDataPointer>

// class CanReaderData;

/**
 * @todo write docs
 */
class CanReader : public QThread
{
public:
    /**
     * Default constructor
     */
    CanReader();

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
    // QSharedDataPointer<CanReaderData> d;
};

// Q_DECLARE_TYPEINFO(CanReader, Q_MOVABLE_TYPE);

#endif // CANREADER_H
