// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Devices/TLV5604.h_v   1.0   18 Aug 2014 09:16:20   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This file implements TLV5604 driver.

#ifndef TLV5604_H_
#define TLV5604_H_

#include "DAC.h"

class SPI;

enum TLV5604DacOutput
{
	DacOutputA, DacOutputB, DacOutputC, DacOutputD,
};

class TLV5604: public DAC
{
public:
	TLV5604(SPI * spi, int cs, int refABmV, int refCDmV);
	virtual ~TLV5604();
	void WriteOutput(int ch, int mV);
	void WriteOutputs(int ch, int * mV);
protected:
	SPI * spi;
	int cs;
	int refABmV;
	int refCDmV;
};

#endif
