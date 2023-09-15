// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Devices/AT24C64.h_v   1.0   09 Jun 2015 09:13:46   tayars  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements AT24C64 I2C chip driver.

#ifndef AT24C64_H_
#define AT24C64_H_

#include "I2CDevice.h"

class AT24C64: public I2CDevice
{
public:
	AT24C64(I2C * i2c, int address);
	virtual ~AT24C64();
	int Read(int address, int MaxRetries);
	void Write(int address, int data);
	void Write(int address, const unsigned char * data, int length);
	void Read(int address, unsigned char * data, int &length, int MaxRetries);
};

#endif
