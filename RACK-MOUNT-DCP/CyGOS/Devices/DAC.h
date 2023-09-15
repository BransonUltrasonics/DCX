// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Devices/DAC.h_v   1.0   18 Aug 2014 09:16:18   amaurya  $
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
	 * Write into single channel in mV.
	 * @param ch Channel number, range depends on the device.
	 * @param mV Output value in mV.
	 */
	virtual void WriteOutput(int ch, int mV) = 0;
	/**
	 * Write into specified channels in mV.
	 * @param ch Up to 32 bits, set if write, clear if skip the channel.
	 * @param mV Array of values in mV. All channel values are placed into
	 * their indexed locations, if channel is skipped the index is skipped as
	 * well.
	 */
	virtual void WriteOutputs(int ch, int * mV) = 0;
};

#endif
