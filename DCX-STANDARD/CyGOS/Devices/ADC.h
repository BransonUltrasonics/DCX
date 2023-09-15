// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Devices/ADC.h_v   1.2   09 Sep 2013 08:58:04   rjamloki  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This file implements generic ADC driver.
/*
 * ADC.h
 *
 *  Created on: Feb 26, 2013
 *      Author: amaurya
 */
#ifndef ADC_H_
#define ADC_H_
#include "stdio.h"
enum ADC_RESOLUTION
{
	ADC_RESOLUTION_10BIT = 10,
	ADC_RESOLUTION_11BIT,
	ADC_RESOLUTION_12BIT,
	ADC_RESOLUTION_13BIT,
	ADC_RESOLUTION_14BIT,
	ADC_RESOLUTION_15BIT,
	ADC_RESOLUTION_16BIT,
};

class ADC
{
protected:
	ADC_RESOLUTION ActualAdcRes, ConfigAdcRes;
	int ShiftVal;
public:
	ADC()
	{
		ShiftVal = 0;
	}
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
	virtual void ReadInputsThroughSequencing(int * Val){
	   pprintf("\n %s Not Implemented", __FUNCTION__);
	}
	virtual void EnableSequencing(int MaxChNum){
	   pprintf("\n Sequencing Not Implemented");
	}
	virtual int SetResolution(ADC_RESOLUTION Res){
		//pprintf("\n %s Not Implemented",__FUNCTION__);
		return ConfigAdcRes;
	}
};

#endif
