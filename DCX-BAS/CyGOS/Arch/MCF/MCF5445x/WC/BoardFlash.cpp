// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Arch/MCF/MCF5445x/WC/BoardFlash.cpp_v   1.2   26 Oct 2012 06:18:00   rjamloki  $
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
#include "FlashM29W128G.h"

/*
Flash * Board::GetFlash()
{
	return new FlashM29W320ET((int) __FLASH_BASE, 16);
}
*/

Flash * Board::GetFlash()
{
	return new FlashM29W128GL((int) __FLASH_BASE, 8);
}

