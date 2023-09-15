/*
 * KSZ8863RLL.cpp
 *
 *  Created on: Aug 13, 2012
 *      Author: RJamloki
 */
#include "KSZ8863RLL.h"
#include "assert.h"
#include "DebugNew.h"
#include "stdio.h"

#define PHY_ADDR_1 0x01
#define PHY_ADDR_2 0x02

KSZ8863RLL::KSZ8863RLL(int chan, int addr) :
	MII(chan, addr, 0)
{
   PHYName = "KSZ8863RLL";
}

/**
 * Verifies the presence of MII.
 */
bool KSZ8863RLL::Verify()
{
	address = PHY_ADDR_1;
	if (ReadRegister(miiPHYIDR1) != 0x0022)
		return false;
	if (ReadRegister(miiPHYIDR2) != 0x1430)
		return false;
	address = PHY_ADDR_2;
	if (ReadRegister(miiPHYIDR1) != 0x0022)
		return false;
	if (ReadRegister(miiPHYIDR2) != 0x1430)
		return false;
	pprintf("Writing to reg 198 for phy1");
	address = PHY_ADDR_1;
	WriteSMIRegister(198, 0x0F);
	pprintf("Reading Reg 198 = %02x",ReadSMIRegister(198));
	pprintf("Writing to reg 198 for phy2");
	address = PHY_ADDR_2;
	WriteSMIRegister(198, 0x0F);
	pprintf("Reading Reg 198 = %02x",ReadSMIRegister(198));
	WriteSMIRegister(0x04, ReadSMIRegister(0x04) & 0xDF);
	WriteSMIRegister(0x12, ReadSMIRegister(0x12) & 0xE7);
	WriteSMIRegister(0x22, ReadSMIRegister(0x22) & 0xE7);
	WriteSMIRegister(0x32, ReadSMIRegister(0x32) & 0xE7);
	return true;
}


bool KSZ8863RLL::ReadLinkStatus()
{
	bool ReturnValue = false;
	address = PHY_ADDR_1;
	if((ReadRegister(miiBMSR) & BMSR_LINK_STATUS) == BMSR_LINK_STATUS)
		ReturnValue = true;
	else{
		address = PHY_ADDR_2;
		if((ReadRegister(miiBMSR) & BMSR_LINK_STATUS) == BMSR_LINK_STATUS)
			ReturnValue = true;
	}
	return ReturnValue;
}

bool KSZ8863RLL::GetLinkStatus(){
	return 0;
}

KSZ8863RLL::~KSZ8863RLL()
{
}
