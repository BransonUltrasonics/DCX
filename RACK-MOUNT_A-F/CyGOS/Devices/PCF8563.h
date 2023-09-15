// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Devices/PCF8563.h_v   1.0   09 Jun 2015 09:13:44   tayars  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements PCF8563 RTC chip driver.

#ifndef PCF8563_H_
#define PCF8563_H_
#include "I2CDevice.h"

class PCF8563: public I2CDevice
{
public:
	PCF8563(I2C * i2c, int raddress, int waddress);
	virtual ~PCF8563();
	int Read(int address, int MaxRetries);
	void Write(int address, int data);
	void Write(int address, const unsigned char * data, int length);
	void Read(int address, unsigned char * data, int &length, int MaxRetries);
	int readAddress;
	int writeAddress;
};

#endif /* PCF8563_H_ */
