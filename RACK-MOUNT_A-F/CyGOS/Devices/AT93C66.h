// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Devices/AT93C66.h_v   1.0   24 Jul 2015 09:16:32   sesharma  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements AT93C66 driver.
#ifndef CAT93C66_H_
#define CAT93C66_H_

#include "SPI.h"

class AT93C66
{
public:
	AT93C66(SPI * spi, int cs, int ctar);
	virtual ~AT93C66();
	void Write(uint16 address, uint16 data);
	uint16 Read(uint16 address);
	void WriteEnable(bool enable);
protected:
	//below functions for testing only. Not required by Application
	void Erase(uint16 address);
	void EraseAll();
	void WriteAll(uint16 data);
	SPI * spi;
	int cs;
	int ctar;
};
#endif
