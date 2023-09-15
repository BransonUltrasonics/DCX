// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Devices/MCP3208.cpp_v   1.0   18 Aug 2014 09:16:20   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This file implements MCP3208 driver.
/*
 *  MCP3208.cpp
 *
 *  Created on: Feb 26, 2013
 *      Author: amaurya
 */
#include "MCP3208.h"
#include "SPI.h"
#include "assert.h"

#define SINGLEENDED 1
#define DIFFERENTIALENDED 0
#define START_BITBOS (1 << 2)
#define INPUT_CONFIGURATION(x) (x << 1)
#define MSB_CHANNELID_BITPOS(ch) ((ch & 0x04) >> 2)
#define LSBS_CHANNELID_BITPOS(ch) ((ch & 3) << 6)

MCP3208::MCP3208(SPI *spi, int cs, int refmV)
{
	this->spi = spi;
	this->cs = 1 << cs;
	this->refmV = refmV;
	ActualAdcRes = ADC_RESOLUTION_12BIT;
    ConfigAdcRes = ADC_RESOLUTION_11BIT;
    ShiftVal = ActualAdcRes - ConfigAdcRes;
}

MCP3208::~MCP3208()
{
}

/**
 * Read voltage on the specified channel. Routine executes with interrupts
 * enabled and while busy-waiting. Execution time is 25 bit times plus one
 * more for processing.
 * @param ch ADC channel to read
 * @return Voltage sensed on channel in mV
 */
int MCP3208::ReadInput(int ch)
{
	ASSERT((ch >= 0) && (ch <= 7));

	uint16 data[3];
	uint32 value = 0;

	//Fill the command now
	//refer to data sheet for preparing conversion start command
	data[0] = 0 | (START_BITBOS)
			|  INPUT_CONFIGURATION(SINGLEENDED)
			|	MSB_CHANNELID_BITPOS(ch);

	data[1] = LSBS_CHANNELID_BITPOS(ch);
	data[2] = 0;//don't care bits

	// Enable the transfer and fetch the values
//	SpiLock q(spi);
//	q.Lock(-1);
	if (refmV == 5000)
		spi->EnableSPI(false, 8, 2000000, false, false);
	else
		spi->EnableSPI(false, 8, 1000000, false, false);
	spi->SingleTransfer(3, data, cs, false);
//	q.UnLock();

	// returned value is 12 bits
	value = 0 | ((data[1] & 0xFF) << 8) | ((data[2] & 0xFF) << 0);
	value &= 0xFFF;
	// convert it into mV
	return value >> ShiftVal;// * refmV / 4095;
}

/**
 * Read specified channels in mV. Routine uses interrupt-driven SPI driver.
 * Execution time is 25 bit times plus one more for processing, per channel.
 * @param mask Up to 32 bits, set if read, clear if skip the channel.
 * @param data Array of results in mV. All channel values are placed into
 * their indexed locations, if channel is skipped the index is skipped as
 * well.
 */
void MCP3208::ReadInputs(int ch, int * mV)
{
	ASSERT((ch & 0xFFFFFF00) == 0);
	SpiLock q(spi);
	q.Lock(-1);
	if (refmV == 5000)
		spi->EnableSPI(false, 12, 2000000, false, false);
	else
		spi->EnableSPI(false, 12, 1000000, false, false);
	for (int i = 0; i < 8; i++)
	{
		if (ch & (1 << i))
		{
			uint16 transfer[2];
			transfer[0] = 0x060 | (i << 2); // start, single-ended, channel
			transfer[1] = 0; // dummy bits to finish the transfer
			spi->LongWordTransfer(sizeof(transfer), (uint8 *) transfer, 16, cs);
			mV[i] = ((transfer[1] & 0xFFF) >> ShiftVal);//((transfer[1] & 0xFFF) * refmV) / 4095;
			
		}
	}
	q.UnLock();
}
