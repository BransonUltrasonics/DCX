// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Flash/Flash.cpp_v   1.0   21 Mar 2012 12:37:26   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3 MultiBoot loader.
//
// This module implements the base flash chip class.

#include "Flash.h"

FlashAccess * Flash::access;

FlashAccess::FlashAccess(int bits)
{
	wordAccess = bits == 16;
}

void FlashAccess::Write(unsigned int address, unsigned int data)
{
	if (wordAccess)
	{
		unsigned short * ptr = (unsigned short *) address;
		*ptr = data;
	}
	else
	{
		unsigned char * ptr = (unsigned char *) address;
		*ptr = data;
	}
}

unsigned int FlashAccess::Read(unsigned int address)
{
	if (wordAccess)
	{
		unsigned short * ptr = (unsigned short *) address;
		return *ptr;
	}
	else
	{
		unsigned char * ptr = (unsigned char *) address;
		return *ptr;
	}
}

Flash::Flash(char * flashName, unsigned int baseAddress, int flashId,
		int flashSize, int bits)
{
	name = flashName;
	base = baseAddress;
	type = flashId;
	size = flashSize;
	wordAccess = bits == 16;
	if (!access)
		SetAccess(new FlashAccess(bits));
}
