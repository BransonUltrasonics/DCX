// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Arch/MCF/MCF5445x/DCPRM/BoardFlash.cpp_v   1.0   18 Aug 2014 09:15:40   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This object encapsulates board-specific flash memory initialization.

#include "Board.h"
#include "FlashM29W320E.h"

Flash * Board::GetFlash()
{
	return new FlashM29W320ET((int) __FLASH_BASE, 16);
}
