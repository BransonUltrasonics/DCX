// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Devices/DAC.h_v   1.1   24 Jul 2015 08:55:06   sesharma  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This file implements generic DAC driver.

#ifndef DAC_H_
#define DAC_H_

class DAC
{
public:
	/**
	 * Write into single channel in raw
	 * @param ch Channel number, range depends on the device.
	 * @param raw Output value in raw.
	 */
	virtual void WriteOutput(int ch, int raw) = 0;
	/**
	 * Write into specified channels in raw.
	 * @param ch Up to 32 bits, set if write, clear if skip the channel.
	 * @param RawVal Array of values in raw. All channel values are placed into
	 * their indexed locations, if channel is skipped the index is skipped as
	 * well.
	 */
	virtual void WriteOutputs(int ch, int * RawVal) = 0;
};

#endif
