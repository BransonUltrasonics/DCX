// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Devices/24AA025E48.cpp_v   1.1   24 Jul 2015 08:55:04   sesharma  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements 24AA025E48 I2C chip driver.

#include "24AA025E48.h"
#include "DebugNew.h"
#include "assert.h"
#include "RTOS.h"

#define PAGE_SIZE_24AA025E48 16

EUI_24AA025E48::EUI_24AA025E48(I2C * i2c, int address) :
	I2CDevice(i2c, address)
{
	ASSERT((address & 0xF0) == 0xA0);
}

EUI_24AA025E48::~EUI_24AA025E48()
{
}

/**
 * Read: Read single byte data from I2C device.
 * @param addr Device address on I2C bus.
 * @param MaxRetries number of retries.
 */
int EUI_24AA025E48::Read(int address, int MaxRetries)
{
	int RetVal;
	unsigned char ch;
	I2CDevice::Read(address, 1, &ch, 1,MaxRetries);
	RetVal = ch;
	return RetVal;
}

/**
 * Read: Read data from I2C device. Multiple bytes are read, up to length argument.
 * @param addr Device address on I2C bus.
 *  @param data Buffer where to place the read data.
 * @param length.
 * @param MaxRetries number of retries.
 */
void EUI_24AA025E48::Read(int addr, unsigned char * data, int &length, int MaxRetries)
{
		I2CDevice::Read(addr, 1, data, length, MaxRetries);
	
}

/**
 * Write: Write single byte data to an I2C device.
 * @param Addr Device address on I2C bus.
 * @param length Data length to write.
 */
void EUI_24AA025E48::Write(int address, int data)
{
	I2CDevice::Write(address, 1, (unsigned char *)&data, 1);
}

/**
 * Write: Write data to an I2C device.
 * @param Addr Device address on I2C bus.
 * @param data Buffer containing data to send to the device.
 * @param length Data length to write.
 */
void EUI_24AA025E48::Write(int addr, const unsigned char * data, int length)
{
	while (length > 0)
	{
		// This many bytes are left in the currently addressed page.
		int pageLeft = PAGE_SIZE_24AA025E48 - (addr & (PAGE_SIZE_24AA025E48 - 1));
		if (pageLeft > length)
			pageLeft = length;
		I2CDevice::Write(addr, 1, data, pageLeft);
		length -= pageLeft;
		addr += pageLeft;
		data += pageLeft;
	}
}

