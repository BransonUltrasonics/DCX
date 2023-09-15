// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Arch/MCF/MCF5445x/MII.cpp_v   1.1   05 Aug 2011 10:19:38   pdwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements the base MII object.

#include "MII.h"
#include "CPUdefs.h"
#include "stdio.h"
#include "DebugNew.h"

MII::MII(int chan, int addr, int mdcFreqMHz)
{
	channel = chan;
	address = addr;
	int divider;
	if (mdcFreqMHz == 0) // default = 2.5MHz standard value for MDC frequency
	{
		divider = (__SYSTEM_CLOCK + 4) / 5;
	}
	else
	{
		for (divider = 1; divider < 64; divider++)
		{
			int freqMHz = __SYSTEM_CLOCK / 2 / divider;
			if (freqMHz <= mdcFreqMHz)
				break;
		}
	}
	MCF_FEC_MSCR(channel) = MCF_FEC_MSCR_MII_SPEED(divider);
	if (channel == 0)
		MCF_PAD_PAR_FECI2C |= 0x0050;
	else
		MCF_PAD_PAR_FECI2C |= 0x0F00;
}

MII::~MII()
{
}

void MII::PrintMiiRegisters(bool bits, bool all)
{
	for (int i = 0; i < 0x20; i++)
	{
		int reg = ReadRegister(i);
		printf("%02X %04X ", i, reg);
		if ((i & 7) == 7)
			printf("\n");
	}
	if (all)
		PrintPartSpecificMiiRegisters(bits);
}

void MII::PrintPartSpecificMiiRegisters(bool bits)
{
	pprintf("%b", bits);
}

/**
 * ReadRegister: Read a single MII register value. Function polls for FEC MII
 * interrupt and clears the interrupt bit in interrupt status register. This
 * interrupt should be disabled through the FEC interrupt mask register.
 * @param reg Register number, 0x00..0x1F.
 * @return int 16-bit unsigned value read from the register, or -1 on register
 * read timeout.
 */
int MII::ReadRegister(int reg)
{
	// Clear the MII interrupt bit
	MCF_FEC_EIR(channel) = MCF_FEC_EIR_MII;
	// Write to the MII Management Frame Register to kick-off the MII read
	MCF_FEC_MMFR(channel) = MCF_FEC_MMFR_ST_01 | MCF_FEC_MMFR_OP_READ
			| MCF_FEC_MMFR_PA(address) | MCF_FEC_MMFR_RA(reg)
			| MCF_FEC_MMFR_TA_10;
	// Poll for the MII interrupt (interrupt should be masked)
	for (int timeout = 0; timeout < cfgMiiTimeout; timeout++)
	{
		if (MCF_FEC_EIR(channel) & MCF_FEC_EIR_MII)
			break;
	}
	if ((MCF_FEC_EIR(channel) & MCF_FEC_EIR_MII) == 0) // timeout, no interrupt?
		return -1;
	// Clear the MII interrupt bit
	MCF_FEC_EIR(channel) = MCF_FEC_EIR_MII;
	return MCF_FEC_MMFR(channel) & 0x0000FFFF;
}

/**
 * WriteRegister: Write a value into MII register. Function polls for FEC MII
 * interrupt and clears the interrupt bit in interrupt status register. This
 * interrupt should be disabled through the FEC interrupt mask register.
 * @param reg Register number, 0x00..0x1F.
 * @param value 16-bit unsigned value to write into the register.
 * @return int 0 if no errors, -1 on register write timeout.
 */
int MII::WriteRegister(int reg, int value)
{
	// Clear the MII interrupt bit
	MCF_FEC_EIR(channel) = MCF_FEC_EIR_MII;
	// Write to the MII Management Frame Register to kick-off the MII write
	MCF_FEC_MMFR(channel) = MCF_FEC_MMFR_ST_01 | MCF_FEC_MMFR_OP_WRITE
			| MCF_FEC_MMFR_PA(address) | MCF_FEC_MMFR_RA(reg)
			| MCF_FEC_MMFR_TA_10 | MCF_FEC_MMFR_DATA(value);
	// Poll for the MII interrupt (interrupt should be masked)
	for (int timeout = 0; timeout < cfgMiiTimeout; timeout++)
	{
		if (MCF_FEC_EIR(channel) & MCF_FEC_EIR_MII)
			break;
	}
	if ((MCF_FEC_EIR(channel) & MCF_FEC_EIR_MII) == 0) // timeout, no interrupt?
		return -1;
	// Clear the MII interrupt bit
	MCF_FEC_EIR(channel) = MCF_FEC_EIR_MII;
	return 0;
}
