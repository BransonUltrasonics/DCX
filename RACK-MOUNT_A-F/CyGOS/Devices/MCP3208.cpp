// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Devices/MCP3208.cpp_v   1.1   24 Jul 2015 08:55:06   sesharma  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This file implements MCP3208 driver.

#include "MCP3208.h"
#include "SPI.h"
#include "assert.h"

#define SINGLEENDED 1
#define DIFFERENTIALENDED 0
#define START_BITBOS (1 << 2)
#define INPUT_CONFIGURATION(x) (x << 1)
#define MSB_CHANNELID_BITPOS(ch) ((ch & 0x04) >> 2)
#define LSBS_CHANNELID_BITPOS(ch) ((ch & 3) << 6)

MCP3208::MCP3208(SPI *spi, int cs, int ctar)
{
	this->spi = spi;
	this->cs = 1 << cs;
	this->ctar = ctar;
	ActualAdcRes = ADC_RESOLUTION_12BIT;
    ConfigAdcRes = ADC_RESOLUTION_12BIT;
    ShiftVal = ActualAdcRes - ConfigAdcRes;
	spi->EnableSPI(false, 12, 1000000, false, false, ctar);
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

	uint16 data[2];
	uint32 value = 0;

	//Fill the command now
	//refer to data sheet for preparing conversion start command
	data[0] = 0 | (((START_BITBOS)
			|  INPUT_CONFIGURATION(SINGLEENDED)
			|	MSB_CHANNELID_BITPOS(ch)) << 4) | (ch << 2);

	data[1] = 0;
	// Enable the transfer and fetch the values
	SpiLock q(spi);
	q.Lock(-1);
	spi->SingleTransfer(2, data, cs, false, ctar);
	q.UnLock();
	// returned value is 12 bits
	value = ((data[1] & 0xFFF) >> ShiftVal);
	return value;
}

/**
 * Read specified channels in mV. Routine uses interrupt-driven SPI driver.
 * Execution time is 25 bit times plus one more for processing, per channel.
 * @param mask Up to 32 bits, set if read, clear if skip the channel.
 * @param data Array of results in mV. All channel values are placed into
 * their indexed locations, if channel is skipped the index is skipped as
 * well.
 */
void MCP3208::ReadInputs(int ch, int * RawVal)
{
	ASSERT((ch & 0xFFFFFF00) == 0);
	SpiLock q(spi);
	q.Lock(-1);
	for (int i = 0; i < 8; i++)
	{
		if (ch & (1 << i))
		{
			uint16 transfer[2];
			transfer[0] = 0x060 | (i << 2); // start, single-ended, channel
			transfer[1] = 0; // dummy bits to finish the transfer
			spi->LongWordTransfer(sizeof(transfer), (uint8 *) transfer, 16, cs, ctar);
			RawVal[i] = (((transfer[1] & 0xFFF)) >> ShiftVal);
		}
	}
	q.UnLock();
}

int MCP3208::SetResolution(ADC_RESOLUTION Res)
{
	if((Res >= ADC_RESOLUTION_10BIT) && ((Res <= ADC_RESOLUTION_12BIT))){
		ConfigAdcRes = Res;
		ShiftVal =  ActualAdcRes - ConfigAdcRes;
	}
	return ConfigAdcRes;
}
