// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Arch/MCF/MCF5445x/FEC.cpp_v   1.0   21 Mar 2012 12:29:36   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements architecture-specific FEC object.

#include "FEC.h"
#include "MII.h"
#include "CPU.h"
#include "string.h"
#include "Board.h"
#include "assert.h"

/**
 * fecs: Pointer to the FEC objects, used for interrupt handling support.
 */
FEC * FEC::fecs[2] =
{ 0, 0 };

FEC::FEC(int chan, MII * mi, bool rmii, unsigned char * macAddr) :
	FecBase(0xFC030000 + 0x4000 * chan, macAddr /*chan ? __ETHERNET_MAC1
			: __ETHERNET_MAC08*/)
{
	ASSERT(chan == 0 || chan == 1);
	// For 256-pin packages clock is either 180 or 240 and we have only one FEC
	if ((__SYSTEM_CLOCK == 180) || (__SYSTEM_CLOCK == 240))
		ASSERT(chan == 0);
	mii = mi;
	channel = chan;
	//	//int par= /*MCF_GPIO_PAR_FECI2C/MCF_PAD_PAR_FECI2C;*/
	//	//	par &= 0x0F; // keep the I2C bits unaffected
	//	//	par |= MCF_GPIO_PAR_FECI2C_PAR_MDC_EMDC; // MDC is a peripheral pin
	//	//	par |= MCF_GPIO_PAR_FECI2C_PAR_MDIO_EMDIO; // MDIO is a peripheral pin
	//	//	MCF_PAD_PAR_FECI2C = par;
	//	//	par = MCF_GPIO_PAR_FEC_PAR_FEC_7W_FEC | MCF_GPIO_PAR_FEC_PAR_FEC_MII_FEC;
	//	//	MCF_GPIO_PAR_FEC = par;
	mode = rmii ? FecModeRMII : FecModeMII;
	switch (channel)
	{
	case 0:
		MCF_PAD_PAR_FEC = (MCF_PAD_PAR_FEC & 0xF0)
				| (rmii ? MCF_PAD_PAR_FEC_PAR_FEC0_RMII_GPIO
						: MCF_PAD_PAR_FEC_PAR_FEC0_MII);
		for (int i = intFec0TransmitFrame; i <= intFec0BabblingReceive; i++)
		{
			CPU::SetupHandler(IntHandler0, (CpuInterrupt) i, 4, 0, StackSize);
		}
		break;
	case 1:
		MCF_PAD_PAR_FEC = (MCF_PAD_PAR_FEC & 0x0F)
				| (rmii ? MCF_PAD_PAR_FEC_PAR_FEC1_RMII_GPIO
						: MCF_PAD_PAR_FEC_PAR_FEC1_MII);
		for (int i = intFec1TransmitFrame; i <= intFec1BabblingReceive; i++)
		{
			CPU::SetupHandler(IntHandler1, (CpuInterrupt) i, 4, 0, StackSize);
		}
		break;
	default:
		CRASH("Bad FEC channel number");
	}
	fecs[channel] = this;
	ASSERT(mii->Verify());
}

FEC::~FEC()
{
}

/**
 * IntHandler: Interrupt handler for all FEC. Function executes interrupt
 * handlers for specific objects.
 * @param level Interrupt level.
 * @param frame Pointer to exception stack frame.
 */
void FEC::IntHandler0(int vector, void * frame)
{
	fecs[0]->IntHandler();
	return;
	ASSERT(frame);
	ASSERT(vector);
}

/**
 * IntHandler: Interrupt handler for all FEC. Function executes interrupt
 * handlers for specific objects.
 * @param level Interrupt level.
 * @param frame Pointer to exception stack frame.
 */
void FEC::IntHandler1(int vector, void * frame)
{
	fecs[1]->IntHandler();
   return;
   ASSERT(frame);
   ASSERT(vector);

}
