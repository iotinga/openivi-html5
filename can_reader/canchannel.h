// Class representing a single channel to be read from CAN bus.
// SPDX-FileCopyrightText: 2023 Davide Rondini <info@drondini.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef CANCHANNEL_H
#define CANCHANNEL_H

#include <cstdint>
#include <string>
#include <linux/can.h>
#include <caninterface.h>

/**
 * @todo write docs
 */
class CANChannel
{
  public:
    /**
     * Default constructor
     */
    CANChannel(const char* inputName);

    /**
     * Copy constructor
     *
     * @param other TODO
     */
    CANChannel(const CANChannel& other);

    /**
     * Destructor
     */
    ~CANChannel();

    /**
     * Assignment operator
     *
     * @param other TODO
     * @return TODO
     */
    CANChannel& operator=(const CANChannel& other);

    /* Get methods */

    /** Get name */
    const char* GetName();

    /** Get linear calibration gain. */
    double GetGain();

    /** Get linear calibration offset. */
    double GetOffset();

    /** Get CAN ID where channel is included. */
    canid_t GetCAN_ID();

    /** Get start bit of channel inside CAN frame data. */
    uint32_t GetStartBit();

    /** Get bit length of channel inside CAN frame data. */
    uint32_t GetBitLength();

    /* Set methods for initalization */

    /* Set CAN information for channel. */
    void SetCANParameters(canid_t id, uint32_t canStart, uint32_t canLength);

    /* set linear calibration parameters */
    void SetCalibrationParameters(double channelGain, double channelOffset);

    /* Get calibrated value from raw integer. */
    double GetCalibratedValue(uint32_t raw);

    /* Extract raw value fron CAN frame data */
    uint32_t GetValueFromCANFrame(const canfd_frame* inputFrame, CAN_Data_Endianess endianess);

  protected:
    canid_t can_id;
    double offset;
    double gain;
    uint32_t bitlength;
    uint32_t startbit;
    uint32_t bytelength;
    uint32_t startbyte;
    bool need_mask;
    std::string name;
};

#endif // CANCHANNEL_H
