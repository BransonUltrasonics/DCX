// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Devices/24AA025E48.h_v   1.0   21 Mar 2012 12:35:30   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements 24AA025E48 I2C chip driver.

#ifndef I2C_24AA025E48_H_
#define I2C_24AA025E48_H_

#include "I2CDevice.h"
class EUI_24AA025E48: public I2CDevice
{
public:
	EUI_24AA025E48(I2C * i2c, int address);
	virtual ~EUI_24AA025E48();
	int Read(int address, int MaxRetries);
	void Write(int address, int data);
	void Write(int address, const unsigned char * data, int length);
	void Read(int address, unsigned char * data, int &length, int MaxRetries);
};

#endif /* 24AA025E48_H_ */
