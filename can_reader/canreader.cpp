// Thread that reads data from CAN bus and make it available to GUI.
// SPDX-FileCopyrightText: 2023 Davide Rondini <info@drondini.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "canreader.h"
#include <QSharedData>

class CanReaderData : public QSharedData
{
public:
};

CanReader::CanReader()
    // : d(new CanReaderData())
{

}

CanReader::~CanReader()
{

}

void CanReader::run()
{

}
