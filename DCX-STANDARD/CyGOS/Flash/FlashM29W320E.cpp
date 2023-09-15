/*
 * FlashM29W320E.cpp
 *
 *  Created on: Nov 29, 2010
 *      Author: atali
 */

#include "FlashM29W320E.h"
#include "assert.h"

FlashM29W320EB::FlashM29W320EB(unsigned int base, int bits) :
	FlashAMD("M29W320EB", base, 0x00202257, 0x400000, bits)
{
}

int FlashM29W320EB::GetSectorSize(int offset)
{
	ASSERT(wordAccess);
	// Data sheet is defined in words, but offset is in bytes, so divide by 2
	offset /= 2;
	if (offset < 0x8000)
	{
		int size = 0x1000;
		if (offset & (size - 1))
			return 0;
		return size * 2;
	}
	int size = 0x8000;
	if (offset & (size - 1))
		return 0;
	return size * 2;
}

FlashM29W320ET::FlashM29W320ET(unsigned int base, int bits) :
	FlashAMD("M29W320ET", base, 0x00202256, 0x400000, bits)
{
}

int FlashM29W320ET::GetSectorSize(int offset)
{
	ASSERT(wordAccess);
	// Data sheet is defined in words, but offset is in bytes, so divide by 2
	offset /= 2;
	if (offset >= 0x1F8000)
	{
		int size = 0x1000;
		if (offset & (size - 1))
			return 0;
		return size * 2;
	}
	int size = 0x8000;
	if (offset & (size - 1))
		return 0;
	return size * 2;
}
