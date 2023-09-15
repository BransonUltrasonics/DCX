// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Net/LAN8701.cpp_v   1.0   18 Aug 2014 09:16:34   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements the SMSC LAN8700 MII object.

#include "LAN8701.h"
#include "assert.h"
#include "DebugNew.h"
#include "stdio.h"

LAN8701::LAN8701(int chan, int addr) :
	MII(chan, addr, 0)
{
}

/**
 * Verfifies the prescence of MII.
 */
bool LAN8701::Verify()
{
	int Val = 0;
	Val = ReadRegister(miiPHYIDR1);
	if (Val != 0x0007){
		pprintf("\n miiPHYIDR1 = %04X", Val);
		//return false;
	}
	Val = ReadRegister(miiPHYIDR2);
	if ((Val & 0xFFF0) != 0xC0F0){
		pprintf("\n miiPHYIDR2 = %04X", Val);
		//return false;
	}
	return true;
}

LAN8701::~LAN8701()
{
}

int LAN8701::WriteSMIRegister(int reg, int value)
{
	int SMIFRAME16 = 0;
	int SMIFRAME32 = 0;
	// Clear the MII interrupt bit
	MCF_FEC_EIR(channel) = MCF_FEC_EIR_MII;
	// Write to the MII Management Frame Register to kick-off the MII write
/*	MCF_FEC_MMFR(channel) = MCF_FEC_MMFR_ST_01 | MCF_FEC_MMFR_OP_WRITE
			| MCF_FEC_MMFR_PA(address) | MCF_FEC_MMFR_RA(reg)
			| MCF_FEC_MMFR_TA_10 | MCF_FEC_MMFR_DATA(value);*/
	 SMIFRAME16 = (1 << 14) | (1 << 12) | ((address & 0x1F) << 7) | (reg << 2);

	SMIFRAME32 = (SMIFRAME16 << 16) | value;
	MCF_FEC_MMFR(channel)  = SMIFRAME32;
	pprintf("\n SMIFRAME32 %x",SMIFRAME32);
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
bool LAN8701::ReadLinkStatus()
{
	bool returnVal = false;
	returnVal = ((ReadRegister(miiBMSR) & BMSR_LINK_STATUS) == BMSR_LINK_STATUS);
	return returnVal;
}

bool LAN8701::GetLinkStatus(){
	return 0;
}
