// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Arch/MCF/MCF5445x/DCPRM/BoardNet.cpp_v   1.0   18 Aug 2014 09:15:42   amaurya  $
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


bool Board::rmii = true;
Ethernet * Board::eth[1];

MII * Board::GetMII(int channel)
{
	switch (channel)
	{
	case 0:
		return new KSZ8863RLL(0, 0x00);

	default:
		CRASH("Bad MII channel");
	}
	return 0;
}
