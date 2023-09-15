// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Net/LAN8701.cpp_v   1.0   19 Apr 2011 10:59:02   PDwivedi  $
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
	if (ReadRegister(miiPHYIDR1) != 0x0007)
		return false;
	if ((ReadRegister(miiPHYIDR2) & 0xFFF0) != 0xC0F0)
		return false;
	return true;
}

LAN8701::~LAN8701()
{
}

