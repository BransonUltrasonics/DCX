// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/MultiBoot/SerialBootTask.cpp_v   1.1   16 Aug 2011 08:44:54   pdwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3 MultiBoot loader.
//
// This module implements the serial port boot loader task.

#include "SerialBootTask.h"
#include "MultiBootCpu.h"
#include "CyGOS.h"
#include "RTOS.h"
#include "CPU.h"
#include "Board.h"
#include "stdio.h"
#include "string.h"

void CyGOS::CreateSerialBootTask(int & priority)
{
	RTOS::Create(new SerialBootTask(), priority++);
}

extern char __USER_FLASH_SIZE[];
extern char __USER_FLASH_RAM[];
extern char __USER_FLASH_ROM[];

unsigned char * SerialBootTask::bootImageBuffer;
unsigned int SerialBootTask::bootImageLength;
unsigned int SerialBootTask::bootImageFlash;

SerialBootTask::SerialBootTask() :
	Task("Serial", 1024)
{
}

void SerialBootTask::Run()
{
	MultiBootCpu::Init();
	Init(__FLASH_ROM);
	IdentifyFlash();
	printf("Board serial number: %d\n", __SERIAL_NUMBER);
	PrintData();
	printf("<1> or <Enter>\t\tSerial download to flash\n");
	printf("<2> or <Esc>\t\tSerial download to SDRAM\n");
	if (__SERIAL_NUMBER == (int)0xFFFFFFFF)
		printf("<N>\t\t\tEnter board serial number\n");
	PrintPrompts();
	int iMax = 30;
	int ch;
	for (int i = 0; i < iMax; i++)
	{
		ch = getcharWait(MsToTicks(50));
		printf(".");
		if (ch != -1)
			break;
	}
	if (CheckPrompts(ch))
		CPU::Restart();
	switch (ch)
	{
	case '1':
	case '\r':
	case '\n':
		SerialDownload(true);
		break;
	case '2':
	case '\x1B':
		SerialDownload(false);
		break;
	case 'N':
		if (__SERIAL_NUMBER == (int)0xFFFFFFFF)
		{
			EnterSerialNumber();
			CPU::Restart();
		}
		break;
	}
	DelayMs(100);
	RTOS::Stop();
	MultiBootCpu::WriteProtectCs();
	printf("\nExecuting old image in flash memory\n");
	ExecuteFlash();
}

void SerialBootTask::EnterSerialNumber()
{
	char serAsc[20];
	printf("\nEnter serial number: ");
	gets(serAsc);
	int serial = atoi(serAsc);
	printf("\nBoard serial number: %d\n", serial);
	printf("Store this serial number? [Y/N]");
	int ch = getchar();
	unsigned short * wptr = (unsigned short *) &__SERIAL_NUMBER;
	wptr = FlashAddress(wptr);
	if (ch == 'Y')
	{
		ASSERT(flash);
		DelayMs(100);
		flash->UnlockChip();
		bool program = flash->Program(wptr, &serial, sizeof(serial));
		ASSERT(program);
		flash->LockChip();
		printf("Done\n");
	}
	else
	{
		printf("Aborted\n");
	}
	DelayMs(1000);
}

void SerialBootTask::EnterMacAddress(unsigned char * macAddress)
{
	char macAsc[20];
	printf("\nEnter MAC address: ");
	gets(macAsc);
	char * ptr = macAsc;
	unsigned char macBin[6];
	for (int i = 0; i < 6; i++)
		macBin[i] = HexToInt(ptr, 1);
	printf("\nEthernet MAC address: ");
	for (int i = 0; i < 6; i++)
	{
		if (i != 0)
			printf(":");
		printf("%02X", macBin[i]);
	}
	printf("\n");
	printf("Store this MAC address? [Y/N]");
	int ch = getchar();
	unsigned short * wptr = (unsigned short *) macAddress;
	wptr = FlashAddress(wptr);
	if (ch == 'Y')
	{
		ASSERT(flash);
		flash->UnlockChip();
		DelayMs(100);
		bool program = flash->Program(wptr, macBin, sizeof(macBin));
		ASSERT(program);
		flash->LockChip();
		printf("Done\n");
	}
	else
	{
		printf("Aborted\n");
	}
}

unsigned int SerialBootTask::HexToInt(char * & ptr, int length)
{
	unsigned int result = 0;
	int i;
	char ch;
	length *= 2;
	// special case - '0' is 1 character
	if (length == 0)
		length = 1;
	for (i = 0; i < length; i++)
	{
		result <<= 4;
		ch = *ptr++;
		if (ch <= '9')
			result |= ch & 0xF;
		else
		{
			ch |= ' ';
			result |= (ch - 'a' + 10) & 0xF;
		}
	}
	return result;
}

void SerialBootTask::IdentifyFlash()
{
	flash = 0;
	POSITION pos = list.GetHeadPosition();
	while (pos)
	{
		flash = list.GetNext(pos);
		if (flash->Identify())
			break;
	}
	if (!flash)
		CRASH("Flash memory chip not recognized");
	printf("Flash on board: %s at base address %08X\n", flash->GetName(),
			flash->GetBase());
	DelayMs(100);
}

void SerialBootTask::SerialDownload(bool toFlash)
{
	printf("\nReceiving HEX file through serial port...\n");
	if (toFlash)
	{
		IdentifyFlash();
	}
	else
	{
		printf("Downloading image into SDRAM\n");
		// Download into SDRAM will require the receive memory buffer to be
		// past the SDRAM app load address, so here, just in case, we allocate
		// a dummy buffer so that the HEX download buffer is in higher memory
		// addresses. This way, when HEX is downloaded, we can copy it into
		// the right place without worrying that buffer overlaps the final
		// load area.
		volatile uint8 * dummy = new uint8[(int) __USER_FLASH_SIZE];
		dummy[0] = 0;
	}
	printf("Initializing download buffer... ");
	DelayMs(100);
	int size = (int) __USER_FLASH_SIZE;
	uint8 * buf = new uint8[size];
	bootImageBuffer = buf;
	bootImageLength = 0;
	printf("Done, waiting for the HEX file...\n");
	while (SerialReceiveHexRecord(buf))
	{
		printf(".");
	}
	printf("Downloaded image size: %X\n", bootImageLength);
	DelayMs(100);
	if (toFlash)
	{
		BurnImageToFlash();
	}
	else
	{
		MoveImageToSdram();
	}
}

bool SerialBootTask::SerialReceiveHexRecord(unsigned char * buf)
{
	char line[100];
	int i = 0;
	int ch = ' ';

	while (ch <= ' ') // skip leftover line feeds
	{
		ch = getchar();
	}
	line[i++] = ch;
	while (ch > ' ')
	{
		ch = getchar();
		line[i++] = ch;
		if (i >= 100)
		{
			printf("\nMissed end-of-line somewhere!\n");
			Suspend();
		}
	}
	// i points past the last char, last char is CR or LF, replace with 0
	line[i - 1] = 0;
	char * ptr = &line[2];
	int length = HexToInt(ptr, 1);
	if ((line[0] == 'S') && (line[1] == '0'))
	{
		ptr += 4; // skip empty 0000
		length -= 3; // and ignore checksum in this length as well
		printf("Receiving '");
		for (int j = 0; j < length; j++)
		{
			printf("%c", HexToInt(ptr, 1));
		}
		printf("'\n");
		return true;
	}
	if ((line[0] == 'S') && (line[1] == '7'))
	{
		printf("\nDone\n");
		return false;
	}
	if ((line[0] == 'S') && (line[1] == '3'))
	{
		int checksum = length;
		length -= 5;
		int addr = HexToInt(ptr, 4);
		checksum += addr >> 24;
		checksum += addr >> 16;
		checksum += addr >> 8;
		checksum += addr >> 0;
		addr -= (int) __USER_FLASH_RAM;
		if ((addr < 0) || (addr >= (int) __USER_FLASH_SIZE))
		{
			printf("\nAddress %08X is out of range\n", addr
					+ (int) __USER_FLASH_RAM);
			Suspend();
		}
		// fill the gaps
		while (addr > (int)bootImageLength)
			buf[bootImageLength++] = 0xFF;
		for (int i = 0; i < length; i++)
		{
			ch = HexToInt(ptr, 1);
			buf[addr++] = ch;
			checksum += ch;
		}
		int chk = HexToInt(ptr, 1);
		checksum += chk;
		checksum &= 0xFF;
		if (checksum != 0xFF)
		{
			printf("\n\nChecksum %02X error at %d\n", checksum, addr);
			Suspend();
		}
		if ((int)bootImageLength < addr)
			bootImageLength = addr;
		return true;
	}
	printf("Bad input '%s'\n", line);
	return true;
}

void SerialBootTask::BurnImageToFlash()
{
	RTOS::Stop();
	printf("Burning image to flash...\n");
	ASSERT(flash);
	bootImageFlash = (unsigned int) __USER_FLASH_ROM;
	printf("Boot image: address %08X, size %08X (%8d)\n",(unsigned int) __USER_FLASH_ROM,
			bootImageLength, bootImageLength);
	flash->Unlock(__USER_FLASH_ROM, bootImageLength);
	if (!flash->Erase(__USER_FLASH_ROM, bootImageLength))
		CRASH("Flash erase error");
	if (!flash->Program(__USER_FLASH_ROM, bootImageBuffer, bootImageLength))
		CRASH("Flash programming error");
	flash->Lock(__USER_FLASH_ROM, bootImageLength);
	MultiBootCpu::WriteProtectCs();
	printf("\nDone\nExecuting new image in flash...\n");
	ExecuteFlash();
}
