// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Devices/PCF8563.cpp_v   1.4   08 Oct 2013 05:56:46   rjamloki  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements PCF8563 I2C RTC chip driver.

#include "PCF8563.h"
#include "DebugNew.h"
#include "assert.h"
#include "RTOS.h"

//extern int I2CRetries;


/**
 * Constructor:
 * @param i2c The driver instance the device is connected to
 * @param raddress: I2C address to be used while reading from device
 * @param waddress: I2C address to be used while writing to device
 */
PCF8563::PCF8563(I2C * i2c, int raddress, int waddress) :
	I2CDevice(i2c, 0)
{
	pprintf("%s",__FUNCTION__);
	ASSERT((raddress & 0xF0) == 0xA0);
	ASSERT((waddress & 0xF0) == 0xA0);
	readAddress = raddress;
	writeAddress = waddress;
}


PCF8563::~PCF8563()
{

}

/**
 * Read: Read single byte data from I2C device. Multiple bytes are read, up to maxLength.
 * @param addr Device address on I2C bus.
 * @param MaxRetries number of retries.
 */
int PCF8563::Read(int address, int MaxRetries)
{
	I2CDevice::address = readAddress;
	unsigned char ch;
	I2CDevice::Read(address, 1, &ch, 1,MaxRetries);
	return ch;
}

/**
 * Read: Read data from I2C device. Multiple bytes are read, up to maxLength.
 * @param addr Device address on I2C bus.
 *  @param data Buffer where to place the read data.
 * @param length.
 * @param MaxRetries number of retries.
 */
void PCF8563::Read(int addr, unsigned char * data, int &length, int MaxRetries)
{

	I2CDevice::address = readAddress;//change slave address during read
//	I2CRetries = 0;
	if(I2CDevice::Read(addr, 1, data, length, MaxRetries) < 0)
	{
		length = -1;
		return;
	}

}

/**
 * Write: Write single byte data to an I2C device.
 * @param Addr Device address on I2C bus.
 * @param length Data length to write.
 */
void PCF8563::Write(int address, int data)
{
	I2CDevice::address = writeAddress;
	unsigned char ch = data;
	I2CDevice::Write(address, 1, &ch, 1);
}

/**
 * Write: Write data to an I2C device.
 * @param Addr Device address on I2C bus.
 * @param data Buffer containing data to send to the device.
 * @param length Data length to write.
 */
void PCF8563::Write(int addr, const unsigned char * data, int length)
{
	I2CDevice::address = writeAddress;//change slave address during write
	I2CDevice::Write(addr, 1, data, length);
}

