// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Devices/TLV5604.h_v   1.1   24 Jul 2015 08:55:06   sesharma  $
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
	TLV5604(SPI * spi, int cs, int ctar);
	virtual ~TLV5604();
	void WriteOutput(int ch, int RawVal);
	void WriteOutputs(int ch, int * RawVal);
protected:
	SPI * spi;
	int cs;
	int ctar;
};

#endif
