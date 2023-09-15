// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Devices/TLV5604.cpp_v   1.1   15 Jul 2011 09:41:30   pdwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This file implements TLV5604 driver.

#include "TLV5604.h"
#include "SPI.h"
#include "assert.h"

TLV5604::TLV5604(SPI * spi, int cs, int refABmV, int refCDmV)
{
	this->spi = spi;
	this->cs = cs;
	this->refABmV = refABmV;
	this->refCDmV = refCDmV;
}

TLV5604::~TLV5604()
{
}

void TLV5604::WriteOutput(int ch, int mV)
{
	ASSERT(mV >= 0);
	uint16 data = 0;
	int bits = 0;

	// select the output
	data |= (ch & 0x3) << 14;
	// select the value now
	switch (ch)
	{
	case DacOutputA:
	case DacOutputB:
		ASSERT(mV <= refABmV * 2);
		bits = (mV * 1023) / refABmV / 2;
		break;
	case DacOutputC:
	case DacOutputD:
		ASSERT(mV <= refCDmV * 2);
		bits = (mV * 1023) / refCDmV / 2;
		break;
	default:
		CRASH("Unknown DAC output!");
	}
	data |= (bits & 0x3FF) << 2;

//	SpiLock q(spi);
//	q.Lock(-1);
	spi->EnableSPI(false, 16, 1250000, true, false);
	spi->SingleTransfer(1, &data, cs, false);
}

/**
 * Write into specified channels in mV.
 * @param ch Up to 32 bits, set if write, clear if skip the channel.
 * @param mV Array of values in mV. All channel values are placed into
 * their indexed locations, if channel is skipped the index is skipped as
 * well.
 */
void TLV5604::WriteOutputs(int ch, int * mV)
{
	ASSERT((ch & 0xFFFFFFF0) == 0);
	SpiLock q(spi);
	q.Lock(-1);
	spi->EnableSPI(false, 16, 1250000, true, false);
	for (int i = 0; i < 4; i++)
	{
		if (ch & (1 << i))
		{
			// select the output
			uint16 data = i << 14;
			// select the value now
			int bits = 0;
			switch (i)
			{
			case DacOutputA:
			case DacOutputB:
				ASSERT(mV[i] <= refABmV * 2);
				bits = (mV[i] * 1023) / refABmV / 2;
				break;
			case DacOutputC:
			case DacOutputD:
				ASSERT(mV[i] <= refCDmV * 2);
				bits = (mV[i] * 1023) / refCDmV / 2;
				break;
			default:
				CRASH("Unknown DAC output!");
			}
			data |= (bits & 0x3FF) << 2;
			spi->LongWordTransfer(sizeof(data), (uint8 *) &data, 16, cs);
		}
	}
	q.UnLock();
}
