// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Devices/ADC.h_v   1.0   29 Oct 2010 10:39:16   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This file implements generic ADC driver.

#ifndef ADC_H_
#define ADC_H_

class ADC
{
public:
	/**
	 * Read single channel in mV.
	 * @param ch Channel number, range depends on the device.
	 */
	virtual int ReadInput(int ch) = 0;
	/**
	 * Read specified channels in mV.
	 * @param ch Up to 32 bits, set if read, clear if skip the channel.
	 * @param mV Array of results in mV. All channel values are placed into
	 * their indexed locations, if channel is skipped the index is skipped as
	 * well.
	 */
	virtual void ReadInputs(int ch, int * mV) = 0;
};

#endif
