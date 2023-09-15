/*
 * FlashAMD.h
 *
 *  Created on: Nov 29, 2010
 *      Author: atali
 */

#ifndef FLASHAMD_H_
#define FLASHAMD_H_

#include "Flash.h"

class FlashAMD: public Flash
{
protected:
	enum Command
	{
		AutoSelect = 0x90,
		EraseSetup = 0x80,
		EraseBlock = 0x30,
		ProgramByte = 0xA0,
		ReadArray = 0xF0
	};
public:
	FlashAMD(char * flashName, unsigned int baseAddress, int flashId,
			int flashSize, int bits);
	virtual bool Program(void * flash, void * data, int length,
			FlashCallback cb = 0);
	virtual bool Erase(void * flash, int length, FlashCallback cb = 0);
	unsigned int ReadFlashType();
protected:
	virtual int GetSectorSize(void * flash);
	virtual int GetSectorSize(int offset) = 0;
	void WaitFor(unsigned int address, unsigned int data);
	virtual void UnlockChip();
	virtual void LockChip();
	virtual void Unlock(void * flash, int length);
	virtual void Lock(void * flash, int length);
	bool Identify();
};

#endif
