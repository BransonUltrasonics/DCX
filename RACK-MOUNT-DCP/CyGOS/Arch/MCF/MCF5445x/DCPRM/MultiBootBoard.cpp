// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Arch/MCF/MCF5445x/DCPRM/MultiBootBoard.cpp_v   1.0   18 Aug 2014 09:15:42   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3 MultiBoot loader.
//
// This module provides board-specific functions for common MultiBoot tasks.
//
// TODO Everything is copied from M54455EVB, only flash and SDRAM settings are verified. The rest must be checked.

#include "SerialBootTask.h"
#include "Board.h"
#include "FlashM29W320E.h"
#include "LED.h"
#include "RTOS.h"
#include "CPU.h"
#include "MultiBootCpu.h"
#include "string.h"
#include "MMU.h"

extern char __USER_FLASH_ROM[];
extern char __USER_FLASH_RAM[];

void SerialBootTask::Init(void * romBase)
{
	romBase = 0;
//	int flashBase = (int) romBase;
	list.AddTail(Board::GetFlash());
}

void SerialBootTask::PrintData()
{
	printf("Ethernet MAC address: ");
	for (int i = 0; i < 6; i++)
	{
		if (i != 0)
			printf(":");
		printf("%02X", __ETHERNET_MAC0[i]);
	}
	printf("\n");
}

void SerialBootTask::PrintPrompts()
{
	if (memcmp(__ETHERNET_MAC0, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) == 0)
		printf("<M>\t\t\tEnter board MAC address\n");
}

unsigned short * SerialBootTask::FlashAddress(unsigned short * ramAddress)
{
	return (unsigned short *) ((((int) ramAddress) & 0x00FFFFFF) | 0x00000000);
}

bool SerialBootTask::CheckPrompts(int ch)
{
	switch (ch)
	{
	case 'M':
		if (memcmp(__ETHERNET_MAC0, "\xFF\xFF\xFF\xFF\xFF\xFF", 6) == 0)
		{
			EnterMacAddress(__ETHERNET_MAC0);
			return true;
		}
		break;
	}
	return false;
}

void SerialBootTask::ExecuteFlash()
{
	MMU::RemapCodeData(true,true);
	DisableInterrupts();
	unsigned long * flash = (unsigned long *) __USER_FLASH_ROM;
	// Each image has following entry point locations:
	// flash[0] - Application start from RAM (address)
	// flash[1] - Application start from flash (address)
	flash += 1;
	void (*p)() = (void (*)()) (*flash);
	pprintf("Transferring to address %p\n", p);
	p();
}

void SerialBootTask::MoveImageToSdram()
{
	MMU::RemapCodeData(true,true);
	// All local variables should be static so that when memcpy  copies SDRAM
	// contents around, the variables will not overlap the new program image.
	// Variables allocated on stack could be anywhere, including the new code
	// area.
	RTOS::Stop();
	printf("\nCopying RAM image to right place...\n");
	// just in case, reset the flash.
	MultiBootCpu::WriteProtectCs();
	bootImageFlash = (unsigned int) __USER_FLASH_RAM;
	static unsigned long * flash = (unsigned long *) __USER_FLASH_RAM;
	memcpy((void *) bootImageFlash, bootImageBuffer, bootImageLength);
	// Each image has following entry point locations:
	// flash[0] - Application start from RAM (address)
	// flash[1] - Application start from flash (address)
	void (*p)() = (void (*)()) (*flash);
	printf("Transferring to address %p\n", p);
	p();
}
