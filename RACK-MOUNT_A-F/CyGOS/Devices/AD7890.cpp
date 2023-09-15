// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Devices/AD7890.cpp_v   1.0   24 Jul 2015 09:16:32   sesharma  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements AD7890 driver.

#include "AD7890.h"
#include "SPI.h"

#define AD7890ADDR_MASK 0x7
#define CONV(x) (x << 12)
#define STDBY(x) (x << 11)
#define AD7890BAUD 1000000

AD7890::AD7890(SPI * spi, int cs, int ctar) {
	// TODO Auto-generated constructor stub
	this->spi = spi;
	this->cs = 1 << cs;
	this->ctar = ctar;
	printf("\n %s cs %d ctar %d ", __FUNCTION__, cs , ctar);
	spi->EnableSPI(false, 16, AD7890BAUD, true, false, ctar);
}

AD7890::~AD7890() {

}

/*
 * Reads the channel value in raw for the channel specified in argument ch and returns the same.
 */
int AD7890::ReadInput(int ch) {
	int value;
	uint16 msg = 0;
	SpiLock q(spi);
	q.Lock(-1);
	WriteCR(ch);
	spi->SingleTransfer(1, &msg, cs, false, ctar);
	q.UnLock();
	value = (int) msg & 0xFFF;
	return value;
}

/*
 * Prepares the conversion command for the channel specified in argument address.
 */
void AD7890::WriteCR(int address) {
	uint16 msg;
	msg = 0 | ((address & AD7890ADDR_MASK) << 13) | CONV(1) | STDBY(0);
	spi->SingleTransfer(1, &msg, cs, false, ctar);
}

/*
 * Reads the channel 0 to channel passed in ch argument and copies to RawVal
 */
void AD7890::ReadInputs(int ch, int * RawVal)
{
	int ChNum;
	uint16 data;
	uint16 TransferResults[9];//device has 8 channels. first transfer is dummy
	SpiLock q(spi);
	q.Lock(-1);
	for (ChNum = 0; ((ChNum <= (ch + 1)) && (ChNum <= 8)); ChNum++)
	{
		data = 0 | ((ChNum & AD7890ADDR_MASK) << 13) | CONV(1) | STDBY(0);
		spi->SingleTransfer(1, &data, cs, false, ctar);
		TransferResults[ChNum] = (data & 0xFFF);
	}
	q.UnLock();
	for (ChNum = 0; ((ChNum <= ch) && (ChNum < 8)); ChNum++)
		RawVal[ChNum] =  TransferResults[ChNum + 1];
}
