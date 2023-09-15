// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Net/LAN8701.h_v   1.0   19 Apr 2011 10:59:06   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares the SMSC LAN8701 MII object.

#ifndef LAN8701_H_
#define LAN8701_H_

#include "MII.h"

class LAN8701: public MII
{
	enum DeviceMiiRegisters
	{
		miiSiliconRev = 16,
		miiModeControlStatus = 17,
		miiSpecialModes = 18,
		miiSymbolErrCounter = 26,
		miiControlStatusIndication = 27,
		miiSpecialInternalTestabilityControls = 28,
		miiInterruptSource = 29,
		miiInterruptMask = 30,
		miiPhySpecialControlStatus = 31,
	};
public:
	LAN8701(int chan, int addr);
	virtual ~LAN8701();
	virtual bool Verify();
};


#endif /* LAN8701_H_ */
