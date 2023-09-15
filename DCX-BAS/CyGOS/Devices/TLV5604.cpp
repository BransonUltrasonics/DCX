// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Devices/TLV5604.cpp_v   1.8   24 Aug 2013 17:06:44   amaurya  $
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
	this->cs = (1 << cs);
	this->refABmV = refABmV;
	this->refCDmV = refCDmV;
}

TLV5604::~TLV5604()
{
}

/*   Purpose:
*	Read voltage on the specified channel. Routine executes with interrupts enabled
* and while busy-waiting(non –interrupt mode).
*   Entry condition:
*	ch- DAC channel to write..
*	val- scaled value in milivolt (max 4000 for WC application. Based on the amplifier circuit
*                                       at channel output , Application/Product requirement of analog
*	output range,and analog refernce value ).
*/
void TLV5604::WriteOutput(int ch, int mV)
{
	ASSERT(mV >= 0);
	uint16 data = 0;
	int bits = 0;
	/*The 16-bit data word for the TLV5604 consists of two parts:
	 * Control bits (D15 . . . D12)
	 * New DAC value (D11 . . . D0)
	 * D15 D14 D13 D12 D11 D10 D9 D8 D7 D6 D5 D4 D3 D2 D1 D0
	 * A1  A0  PWR SPD New DAC       value(10 bits)     X X
	 * X: don’t care
	 * SPD: Speed control bit. 1 -fast mode 0 - slow mode
	 * PWR: Power control bit. 1 - power down 0 - normal operation
	 * //DAC channel selection
		A1 A0 DAC
		0 0   A
		0 1   B
		1 0   C
		1 1   D
	 * */

	// select the output
	data |= (ch & 0x3) << 14;
	// select the value now
	switch (ch)
	{
	case DacOutputA:
	case DacOutputB:
//		ASSERT(mV <= refABmV * 2);
//		bits = (mV * 1023) / refABmV / 2;
		if(!(mV <= refABmV * 2)){
			CyGOS::WriteSystemEvent(ASSERT_ERROR, __LINE__, (('T' << 24) | ('L' << 16) | ('V' << 8) | ('5')), mV, true);//ASSERT(0);//ASSERT(mV <= refABmV * 2);
			mV = refABmV * 2;
		}
		bits = (mV * 1023) / refABmV / 2;
		break;
	case DacOutputC:
	case DacOutputD:
		if(!(mV <= refCDmV * 2)){
			CyGOS::WriteSystemEvent(ASSERT_ERROR, __LINE__, (('T' << 24) | ('L' << 16) | ('V' << 8) | ('5')), mV, true);//ASSERT(0);//ASSERT(mV <= refABmV * 2);
			mV = refABmV * 2;
		}
		bits = (mV * 1023) / refABmV / 2;
//		ASSERT(mV <= refCDmV * 2);
//		bits = (mV * 1023) / refCDmV / 2;
		break;
	default:
		CRASH("Unknown DAC output!");
	}
	data |= (bits & 0x3FF) << 2;

//	SpiLock q(spi);
//	q.Lock(-1);
	spi->EnableSPI(false, 16, 12500000, true, false);
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
	//spi->EnableSPI(false, 16, 20000000, true, false);
	spi->EnableSPI(false, 16, 12500000, true, false);
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
				if(!(mV[i] <= refABmV * 2)){
					CyGOS::WriteSystemEvent(ASSERT_ERROR, __LINE__, (('T' << 24) | ('L' << 16) | ('V' << 8) | ('5')), mV[i], true);//ASSERT(0);//ASSERT(mV[i] <= refABmV * 2);
					mV[i] = refABmV * 2;
				}
				bits = (mV[i] * 1023) / refABmV / 2;
				break;
			case DacOutputC:
			case DacOutputD:
				if(!(mV[i] <= refCDmV * 2)){
					CyGOS::WriteSystemEvent(ASSERT_ERROR, __LINE__, (('T' << 24) | ('L' << 16) | ('V' << 8) | ('5')), mV[i], true);//ASSERT(0);//ASSERT(mV[i] <= refABmV * 2);
					mV[i] = refCDmV * 2;
				}
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
