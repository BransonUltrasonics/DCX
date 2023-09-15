// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Arch/MCF/MCF5445x/WC/BoardNet.cpp_v   1.4   16 Jan 2013 05:47:10   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This object encapsulates board-specific network initialization.

#include "Board.h"
#include "LAN8701.h"
#include "KSZ8863RLL.h"
#include "assert.h"

bool Board::rmii = false;
Ethernet * Board::eth[2];

MII * Board::GetMII(int channel)
{
	switch (channel)
	{
	case 0:
		return new LAN8701(0, 0x00);
	case 1:
	
	   return new KSZ8863RLL(1, 0x00);
		//TODO: Verify this
		///switch working as MII device??
		//return new LAN8701(1, 0x00);
	default:
		CRASH("Bad MII channel");
	}
	return 0;
}
