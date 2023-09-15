// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Devices/MCP3208.h_v   1.0   21 Mar 2012 12:36:14   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This file implements MCP3208 driver.

#ifndef MCP3208_H_
#define MCP3208_H_

#include "ADC.h"

class SPI;

/**
 * 	Selects the ADC output
 */
enum MCP3208AdcInput
{
	AdcInputCh0,
	AdcInputCh1,
	AdcInputCh2,
	AdcInputCh3,
	AdcInputCh4,
	AdcInputCh5,
	AdcInputCh6,
	AdcInputCh7,
};

class MCP3208: public ADC
{
public:
	MCP3208(SPI *spi, int cs, int refmV);
	virtual ~MCP3208();
	int ReadInput(int ch);
	void ReadInputs(int ch, int * mV);
protected:
	SPI * spi;
	int cs;
	int refmV;
};

#endif
