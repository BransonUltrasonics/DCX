// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Devices/I2CDevice.h_v   1.0   10 Nov 2010 09:31:10   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements base class for I2C chip driver.

#ifndef I2CDEVICE_H_
#define I2CDEVICE_H_

#include "I2C.h"

class I2CDevice
{
public:
	I2CDevice(I2C * i2c, int addr);
	virtual ~I2CDevice();
	int Read(int i2cAddr, int i2cAddrLen, unsigned char * buf, int len, int MaxRetries);
	int Write(int i2cAddr, int i2cAddrLen, const unsigned char * buf, int len);
	int WriteReg(int i2cAddr, int data)
	{
		unsigned char ch = data;
		return Write(i2cAddr, 1, &ch, 1);
	}
	int ReadReg(int i2cAddr)
	{
		unsigned char ch;
		Read(i2cAddr, 1, &ch, 1,5);
		return ch;
	}
protected:
	bool Acquire()
	{
		return interface->Acquire();
	}
	void Release()
	{
		interface->Release();
	}
	int address;
	I2C * interface;
};

#endif
