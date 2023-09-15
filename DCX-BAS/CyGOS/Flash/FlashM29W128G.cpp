#include "FlashM29W128G.h"
#include "assert.h"
#define SECTOR_SIZE_BYTE 0x20000
/*
 * FlashM29W128G.cpp
 *
 *  Created on: Aug 9, 2012
 *      Author: RJamloki
 */

FlashM29W128GL::FlashM29W128GL(unsigned int base, int bits) :
	FlashAMD("M29W128GL", base, 0x207E2100, 0x1000000, bits)
{
	//refer to data sheet for flash ID 0x207E2100
}

int FlashM29W128GL::GetSectorSize(int offset)
{
	ASSERT(wordAccess == false);
	if(offset < 0 || offset > 0x1000000)
		return 0;

	return SECTOR_SIZE_BYTE;
}
/*
 * Function programs the flash and works for 8 bit access only
 */
bool FlashM29W128GL::Program(void * flash, void * data, int length, FlashCallback cb)
{
	unsigned int wptr = (unsigned int) flash;
	unsigned int rptr = (unsigned int) data;
	int progress = 0;
	int maxLength = length;
	unsigned int writeData;
	ASSERT(wordAccess == false);
	while (length > 0)
	{
		if (cb)
			cb(progress, maxLength);

		Write(base + 0xAAA , 0xAA);
		Write(base + 0x555 , 0x55);
		Write(base + 0xAAA , ProgramByte);
		writeData = *((unsigned char *) rptr);
		Write(wptr, writeData);
		WaitFor(wptr, writeData);
		length -= 1;
		wptr += 1;
		rptr += 1;
		progress += 1;
	}
	if (cb)
		cb(maxLength, maxLength);

	Write(base + 0xAAA , 0xAA);
	Write(base + 0x555 , 0x55);
	Write(base, ReadArray);
	return true;
}

/*
 * Function erases the flash and works for 8 bit access only
 */
bool FlashM29W128GL::Erase(void * flash, int length, FlashCallback cb)
{
	unsigned int wptr = (unsigned int) flash;
	int progress = 0;
	int maxLength = length;
	ASSERT(wordAccess == false);
	while (length > 0)
	{
		if (cb)
			cb(progress, maxLength);
		int sectorSize = GetSectorSize(wptr - base);
		if (sectorSize == 0)
			break;
		Write(base + 0xAAA , 0xAA);
		Write(base + 0x555 , 0x55);
		Write(base + 0xAAA , EraseSetup);
		Write(base + 0xAAA , 0xAA);
		Write(base + 0x555 , 0x55);
		Write(wptr, EraseBlock);
		WaitFor(wptr, 0xFF);
		length -= sectorSize;
		wptr += sectorSize;
		progress += sectorSize;
	}
	if (cb)
		cb(maxLength, maxLength);

	Write(base + 0xAAA , 0xAA);
	Write(base + 0x555 , 0x55);
	Write(base, ReadArray);
	return true;
}


/*
 * Function reads the predefined ID of chip
 * coded in Flash itself by manufacturer.
 * Function supports only 8 bit access
 * to the flash.
 */
unsigned int FlashM29W128GL::ReadFlashType()
{
	unsigned int type;
	ASSERT(wordAccess == false);
	Write(base + 0xAAA , 0xAA);
	Write(base + 0x555 , 0x55);
	Write(base + 0xAAA , AutoSelect);
	type = Read(base) << 24;
	type |= Read(base + 1) << 16;
	type |= Read(base + 2) << 8;
	type |= Read(base + 3);
	Write(base + 0xAAA , 0xAA);
	Write(base + 0x555 , 0x55);
	Write(base, ReadArray);
	return type;
}


