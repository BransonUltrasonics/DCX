// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/RTOS/Debug.cpp_v   1.0   09 Jun 2015 09:14:00   tayars  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements the simplistic debugger to use after the crash.

// TODO Implement '?' to show available commands.

#include "Debug.h"
#include "Terminal.h"
#include "CPU.h"
#include "RTOS.h"
#include "stdio.h"
#include "DebugNew.h"

uint8 * Debug::stack;
Debug * Debug::debug;

void Debug::Init()
{
	debug = new Debug(0x10000);
}

Debug::Debug(int stackSize)
{
	stack = new uint8[stackSize];
}

void Debug::Poll()
{
	if (!Terminal::Peek())
		return;
	Terminal::In();
	CPU::SwitchStack(stack);
	printf("\x0C" "Debugger activated.\n");
	debug->Run();
}

void Debug::Run()
{
	for (;;)
	{
		ReadLine();
		printf("Command line: '%s'\n", command);
		// command letter to lower case
		switch (command[0])
		{
		case 'r':
		case 'R':
		{
			char * ptr = command + 1;
			int addr = HexToInt(ptr, 4);
			addr &= 0xFFFFFFFC;
			int length = HexToInt(ptr, 2);
			length += 3;
			length &= 0xFFFFFFFC;
			ReadMemory(addr, length);
		}
			break;
		case 'T':
			RTOS::PrintStatus(PrintTasks);
			break;
		case 'M':
			ShowMemory();
			break;
		case 'Z':
			CPU::Restart();
			break;
		}
	}
}

void Debug::ReadMemory(int addr, int length)
{
	int maxLength = 32;
	while (length > 0)
	{
		int len = length > maxLength ? maxLength : length;
		printf("S3%02X%08X", len + 5, addr);
		int checksum = len + 5;
		checksum += addr >> 24;
		checksum += addr >> 16;
		checksum += addr >> 8;
		checksum += addr >> 0;
		unsigned char * address = (unsigned char *) addr;
		for (int i = 0; i < len; i++)
		{
			int ch = *address++;
			checksum += ch;
			printf("%02X", ch);
		}
		checksum &= 0xFF;
		checksum ^= 0xFF;
		printf("%02X\n", checksum);
		length -= len;
		addr += len;
	}
}

void Debug::ReadLine()
{
	unsigned int index = 0;
	bool echo = false;
	for (;;)
	{
		int ch = Terminal::In();
		if (index == 0)
			echo = ch >= 'a';
		if (echo)
		{
			if (ch == '\n')
				Terminal::Out('\r');
			Terminal::Out(ch);
		}
		if (ch == '\b')
		{
			if (index > 0)
				index--;
		}
		else if ((ch == '\r') || (ch == '\n'))
		{
			command[index] = 0;
			break;
		}
		else
		{
			if (index < (sizeof(command) - 1))
				command[index++] = ch;
		}
	}
}

unsigned int Debug::HexToInt(char * & ptr, int length)
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
		if (ch >= 'a')
			ch -= ' ';
		if (ch <= '9')
			result |= ch & 0xF;
		else
			result |= (ch - 'A' + 10) & 0xF;
	}
	return result;
}
