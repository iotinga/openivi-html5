// Class representing a single channel to be read from CAN bus.
// SPDX-FileCopyrightText: 2023 Davide Rondini <info@drondini.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "canchannel.h"

CANChannel::CANChannel(const char* inputName)
{
	name.assign(inputName);
	can_id = 0xFFFFFFFF;
	offset = 0.0;
	gain = 0.0;
	bitlength = 0;
	startbit = 0;
	startbyte = 0;
	bytelength = 0;
	need_mask = false;
	valid = false;
}

CANChannel::CANChannel(const CANChannel& other)
{
	name = other.name;
	can_id = other.can_id;
	offset = other.offset;
	gain = other.gain;
	bitlength = other.bitlength;
	startbit = other.startbit;
	startbyte = other.startbyte;
	bytelength = other.startbyte;
	need_mask = other.need_mask;
	valid = other.valid;
}

CANChannel::~CANChannel()
{
	name.clear();
}

CANChannel& CANChannel::operator=(const CANChannel& other)
{
	name.assign(other.name);
	can_id = other.can_id;
	offset = other.offset;
	gain = other.gain;
	bitlength = other.bitlength;
	startbit = other.startbit;
	startbyte = other.startbyte;
	bytelength = other.startbyte;
	need_mask = other.need_mask;
	valid = other.valid;
	return *this;
}

const char * CANChannel::GetName()
{
	return name.c_str();
}

canid_t CANChannel::GetCAN_ID()
{
	return can_id;
}

uint32_t CANChannel::GetStartBit()
{
	return startbit;
}

uint32_t CANChannel::GetBitLength()
{
	return bitlength;
}

double CANChannel::GetGain()
{
	return gain;
}

double CANChannel::GetOffset()
{
	return offset;
}

void CANChannel::SetCANParameters(canid_t id, uint32_t canStart, uint32_t canLength, uint32_t maxCanBytes)
{
	can_id = id;
	startbit = canStart;
	bitlength = canLength;

	// NEEDS UPDATE: this only works for multiple of 8 bit position and size
	need_mask = !(((startbit % 8) == 0) && ((bitlength % 8) == 0));
	startbyte = startbit >> 3;
	bytelength = bitlength >> 3;
	// Avoid overflow from CAN frame data
	valid = (startbyte + bytelength <= maxCanBytes);
}

void CANChannel::SetCalibrationParameters(double channelGain, double channelOffset)
{
	gain = channelGain;
	offset = channelOffset;
}

double CANChannel::GetCalibratedValue(uint32_t raw)
{
	return gain*((double) raw) + offset;
}

uint32_t CANChannel::GetValueFromCANFrame(const canfd_frame* inputFrame, CAN_Data_Endianess endianess)
{
	uint32_t result = 0;
	if (valid) {
		if (need_mask) {
			// Not yet implemented!
		} else {
			if (bytelength == 1) {
				result = (uint32_t) inputFrame->data[startbyte];
			} else {
				if (bytelength == 2) {
					if (endianess == CAN_ENDIAN_BIG) {
						result = (uint32_t) inputFrame->data[startbyte + 1];
						result += (uint32_t) inputFrame->data[startbyte] << 8;
					} else {
						result = (uint32_t) inputFrame->data[startbyte];
						result += (uint32_t) inputFrame->data[startbyte + 1] << 8;
					}
				} else if (bytelength == 4) {
					if (endianess == CAN_ENDIAN_BIG) {
						result = (uint32_t) inputFrame->data[startbyte + 3];
						result += (uint32_t) inputFrame->data[startbyte + 2] << 8;
						result += (uint32_t) inputFrame->data[startbyte + 1] << 16;
						result += (uint32_t) inputFrame->data[startbyte] << 24;
					} else {
						result = (uint32_t) inputFrame->data[startbyte];
						result += (uint32_t) inputFrame->data[startbyte + 1] << 8;
						result += (uint32_t) inputFrame->data[startbyte + 2] << 16;
						result += (uint32_t) inputFrame->data[startbyte + 3] << 24;
					}
				}
			}
		}
	}

	return result;
}
