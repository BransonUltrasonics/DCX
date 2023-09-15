// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Devices/MCP3208.cpp_v   1.3   15 Jul 2011 09:41:18   pdwivedi  $
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

MCP3208::MCP3208(SPI *spi, int cs, int refmV)
{
	this->spi = spi;
	this->cs = cs;
	this->refmV = refmV;
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

	// Fill the command now
	data[0] = 0 | (1 << 2) // start bit
			| (1 << 1) // single channel
			| ((ch & 0x4) >> 2); // msb of channel id
	data[1] = (ch & 3) << 6; // last 2 lsb of channel id
	data[2] = 0; // don't care

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
	return value >> 1;// * refmV / 4095;
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
			mV[i] = ((transfer[1] & 0xFFF) >> 1);//((transfer[1] & 0xFFF) * refmV) / 4095;
			
		}
	}
	q.UnLock();
}
