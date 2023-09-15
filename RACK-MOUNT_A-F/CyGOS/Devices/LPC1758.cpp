// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Devices/LPC1758.cpp_v   1.0   24 Jul 2015 09:16:32   sesharma  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements LPC1758 driver.

#include "LPC1758.h"
#include "SPI.h"
#include "assert.h"

LPC1758::LPC1758(SPI *spi, int cs, int ctar)
{
	this->spi = spi;
	this->cs = 1 << cs;
	this->ctar = ctar;
	spi->EnableSPI(false, 16, 1000000, false, false, ctar);
	printf("\n %s cs %d ctar %d ", __FUNCTION__, cs , ctar);
}

LPC1758::~LPC1758()
{
}

/*Purpose:This function is used to transfer the pressure and flow settings to the CPU3 board on
  AES board. Pressure and flow settings are transferred to device in one 16 bit frame. Lower 8 bits
  of received frame contains the actual flow settings.
Entry Condition:
WriteVal: Lower 8 bits shall contain the pressure settings(Max 100 PSI). Bit 8 to 15 shall contain
 the flow settings(Max 100 percent).
Exit Condition:
Function returns the actual flow settings in percentage.
*/
int LPC1758::WriteReadVal(int WriteVal)
{
	uint16 data;
	int value = 0;
	data = (WriteVal & 0xFFFF);
	// Enable the transfer and fetch the values
	SpiLock q(spi);
	q.Lock(-1);
	spi->SingleTransfer(1, &data, cs, false, ctar);
	q.UnLock();
	value = data;
	return (value & 0xFF);
}


