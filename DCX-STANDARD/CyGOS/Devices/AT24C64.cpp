// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Devices/AT24C64.cpp_v   1.0   10 Nov 2010 09:31:08   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements AT24C64 I2C chip driver.

#include "AT24C64.h"
#include "DebugNew.h"
#include "assert.h"
#include "RTOS.h"

extern int I2CRetries;

AT24C64::AT24C64(I2C * i2c, int address) :
	I2CDevice(i2c, address)
{
	ASSERT((address & 0xF0) == 0xA0);
}

AT24C64::~AT24C64()
{
}

/**
 * Read: Read single byte data from I2C device. Multiple bytes are read, up to maxLength.
 * @param addr Device address on I2C bus.
 * @param MaxRetries number of retries.
 */
int AT24C64::Read(int address, int MaxRetries)
{
	unsigned char ch;
	I2CDevice::Read(address, 2, &ch, 1,MaxRetries);
	return ch;
}

/**
 * Read: Read data from I2C device. Multiple bytes are read, up to maxLength.
 * @param addr Device address on I2C bus.
 *  @param data Buffer where to place the read data.
 * @param length.
 * @param MaxRetries number of retries.
 */
void AT24C64::Read(int addr, unsigned char * data, int &length, int MaxRetries)
{
	int pageSize = 32;
	I2CRetries = 0;
	int templen = length;
	while (templen > 0)
	{
		// This many bytes are left in the currently addressed page.
		int pageLeft = pageSize - (addr & (pageSize - 1));
		if (pageLeft > templen)
			pageLeft = templen;
		
		if(I2CDevice::Read(addr, 2, data, pageLeft,MaxRetries) < 0)
		{
			length = -1;
			return;
		}	
		RTOS::DelayMs(2);
		templen -= pageLeft;
		addr += pageLeft;
		data += pageLeft;
	}	
}

/**
 * Write: Write single byte data to an I2C device.
 * @param Addr Device address on I2C bus.
 * @param length Data length to write.
 */
void AT24C64::Write(int address, int data)
{
	unsigned char ch = data;
	I2CDevice::Write(address, 2, &ch, 1);
	RTOS::DelayMs(5);
}

/**
 * Write: Write data to an I2C device.
 * @param Addr Device address on I2C bus.
 * @param data Buffer containing data to send to the device.
 * @param length Data length to write.
 */
void AT24C64::Write(int addr, const unsigned char * data, int length)
{
	int pageSize = 32;
	while (length > 0)
	{
		// This many bytes are left in the currently addressed page.
		int pageLeft = pageSize - (addr & (pageSize - 1));
		if (pageLeft > length)
			pageLeft = length;
		I2CDevice::Write(addr, 2, data, pageLeft);
		RTOS::DelayMs(5);
		length -= pageLeft;
		addr += pageLeft;
		data += pageLeft;
	}
}

