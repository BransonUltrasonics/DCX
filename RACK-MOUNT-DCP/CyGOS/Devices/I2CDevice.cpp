// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Devices/I2CDevice.cpp_v   1.0   18 Aug 2014 09:16:18   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements base class for I2C chip driver.

#include "I2CDevice.h"
#include "DebugNew.h"
extern int I2CRetries;
I2CDevice::I2CDevice(I2C * i2c, int addr)
{
	interface = i2c;
	address = addr;
}

I2CDevice::~I2CDevice()
{
}

/**
 * Read: Read data from I2C device. Multiple bytes are read, up to maxLength.
 * @param i2cAddr Device address on I2C bus.
 * @param i2cAddrLen
 * @param data Buffer where to place the read data.
 * @param dataLen Buffer length.
 * @param MaxRetries number of retries.
 */
int I2CDevice::Read(int i2cAddr, int i2cAddrLen, unsigned char * buf, int len, int MaxRetries)
{
	if (!Acquire())
		return -1;
	I2CRetries = 0;
	interface->Read(address, i2cAddr, i2cAddrLen, buf, len, MaxRetries);
	Release();
	return len;
}

/**
 * Write: Write data to an I2C device.
 * @param i2cAddr Device address on I2C bus.
 * @param i2cAddrLen 
 * @param data Buffer containing data to send to the device.
 * @param dataLen Data length to write.
 */
int I2CDevice::Write(int i2cAddr, int i2cAddrLen, const unsigned char * buf,
		int len)
{
	if (!Acquire())
		return -1;
	interface->Write(address, i2cAddr, i2cAddrLen, buf, len);
	Release();
	return len;
}
