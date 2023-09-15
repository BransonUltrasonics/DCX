// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Arch/MCF/MCF5445x/DCP1D/BoardNet.cpp_v   1.0   19 Apr 2011 11:04:12   PDwivedi  $
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
#include "assert.h"

bool Board::rmii = false;
Ethernet * Board::eth[1];

MII * Board::GetMII(int channel)
{
	switch (channel)
	{
	case 0:
		return new LAN8701(0, 0x00);
	default:
		CRASH("Bad MII channel");
	}
}
