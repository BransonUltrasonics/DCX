/*
 * FlashAMD.cpp
 *
 *  Created on: Nov 29, 2010
 *      Author: atali
 */

#include "FlashAMD.h"
#include "assert.h"

FlashAMD::FlashAMD(char * flashName, unsigned int baseAddress, int flashId,
		int flashSize, int bits) :
	Flash(flashName, baseAddress, flashId, flashSize, bits)
{
}

void FlashAMD::WaitFor(unsigned int address, unsigned int data)
{
	for (;;)
	{
		unsigned int read = Read(address);
		if (read == data)
			break;
	}
}

bool FlashAMD::Identify()
{
	return (ReadFlashType() == ((unsigned int)type));
}

bool FlashAMD::Program(void * flash, void * data, int length, FlashCallback cb)
{
	unsigned int wptr = (unsigned int) flash;
	int unitSize = wordAccess ? 2 : 1;
	unsigned int rptr = (unsigned int) data;
	int progress = 0;
	int maxLength = length;
	while (length > 0)
	{
		if (cb)
			cb(progress, maxLength);
		Write(base + 0x555 * unitSize, 0xAA);
		Write(base + 0x2AA * unitSize, 0x55);
		Write(base + 0x555 * unitSize, ProgramByte);
		unsigned int writeData;
		if (wordAccess)
			writeData = *((unsigned short *) rptr);
		else
			writeData = *((unsigned char *) rptr); // TODO byte access
		Write(wptr, writeData);
		WaitFor(wptr, writeData);
		length -= unitSize;
		wptr += unitSize;
		rptr += unitSize;
		progress += unitSize;
	}
	if (cb)
		cb(maxLength, maxLength);
	Write(base + 0x555 * unitSize, 0xAA);
	Write(base + 0x2AA * unitSize, 0x55);
	Write(base, ReadArray);
	return true;
}

bool FlashAMD::Erase(void * flash, int length, FlashCallback cb)
{
	unsigned int wptr = (unsigned int) flash;
	int unitSize = wordAccess ? 2 : 1;
	int progress = 0;
	int maxLength = length;
	while (length > 0)
	{
		if (cb)
			cb(progress, maxLength);
		int sectorSize = GetSectorSize(wptr - base);
		if (sectorSize == 0)
			break;
		Write(base + 0x555 * unitSize, 0xAA);
		Write(base + 0x2AA * unitSize, 0x55);
		Write(base + 0x555 * unitSize, EraseSetup);
		Write(base + 0x555 * unitSize, 0xAA);
		Write(base + 0x2AA * unitSize, 0x55);
		Write(wptr, EraseBlock);
		WaitFor(wptr, 0xFFFF);
		length -= sectorSize;
		wptr += sectorSize;
		progress += sectorSize;
	}
	if (cb)
		cb(maxLength, maxLength);
	Write(base + 0x555 * unitSize, 0xAA);
	Write(base + 0x2AA * unitSize, 0x55);
	Write(base, ReadArray);
	return true;
}

void FlashAMD::UnlockChip()
{
}

void FlashAMD::LockChip()
{
}

void FlashAMD::Unlock(void * flash, int length)
{
	ASSERT(length > 0);
}

void FlashAMD::Lock(void * flash, int length)
{
	ASSERT(length > 0);
}

int FlashAMD::GetSectorSize(void * flash)
{
	unsigned int wptr = (unsigned int) flash;
	int sectorSize = GetSectorSize(wptr - base);
	return sectorSize;
}

unsigned int FlashAMD::ReadFlashType()
{
	int unitSize = wordAccess ? 2 : 1;
	Write(base + 0x555 * unitSize, 0xAA);
	Write(base + 0x2AA * unitSize, 0x55);
	Write(base + 0x555 * unitSize, AutoSelect);
	unsigned int type = Read(base) << 16;
	type |= Read(base + 2);
	Write(base + 0x555 * unitSize, 0xAA);
	Write(base + 0x2AA * unitSize, 0x55);
	Write(base, ReadArray);
	return type;
}
