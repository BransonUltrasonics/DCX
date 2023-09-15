// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/API/CRC.cpp_v   1.0   29 Oct 2010 10:38:22   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements functions to calculate CRC codes.

#include "CRC.h"
#include "assert.h"
#include "stdio.h"
#include "DebugNew.h"

/**
 * CalcCrc7_09: Calculate 7-bit CRC with polynomial 0x09.
 * @param data Data to calculate CRC from.
 * @param length Data length in bytes.
 * @return unsigned int 7-bit CRC value.
 */
unsigned int CalcCrc7_09(const void * data, int length)
{
	unsigned int crc = 0;
	const unsigned char * buf = (const unsigned char *) data;
	while (length-- > 0)
	{
		unsigned char byte = *buf++;
		for (int b = 8; b > 0; b--)
		{
			int bit = (byte & 0x80) ? 1 : 0;
			byte <<= 1;
			crc <<= 1;
			crc |= bit;
			if (crc & 0x80)
			{
				// a trick - we need to clear overflow bit 80 and we need to
				// flip some other bits according to polynomial 09 - we can do
				// both at same time by flipping both polynomial and overflow
				// bits using constant 89
				crc ^= 0x89;
			}
		}
	}
	for (int b = 7; b > 0; b--)
	{
		int bit = 0;
		crc <<= 1;
		crc |= bit;
		if (crc & 0x80)
		{
			crc ^= 0x89;
		}
	}
	return crc;
}

/**
 * CalcCrc16_1021: Calculate 16-bit CRC with polynomial 0x1021.
 * @param data Data to calculate CRC from.
 * @param length Data length in bytes.
 * @return unsigned int 16-bit CRC value.
 */
unsigned int CalcCrc16_1021(const void * data, int length)
{
	unsigned int crc = 0;
	const unsigned char * buf = (const unsigned char *) data;
	while (length-- > 0)
	{
		unsigned char byte = *buf++;
		for (int b = 8; b > 0; b--)
		{
			int bit = (byte & 0x80) ? 1 : 0;
			byte <<= 1;
			crc <<= 1;
			crc |= bit;
			if (crc & 0x10000)
			{
				// a trick - we need to clear overflow bit 10000 and we need to
				// flip some other bits according to polynomial 1021 - we can do
				// both at same time by flipping both polynomial and overflow
				// bits using constant 11021
				crc ^= 0x11021;
			}
		}
	}
	for (int b = 16; b > 0; b--)
	{
		int bit = 0;
		crc <<= 1;
		crc |= bit;
		if (crc & 0x10000)
		{
			crc ^= 0x11021;
		}
	}
	return crc;
}

unsigned int CalcCrc32_EDB88320(const void * data, int length)
{
	unsigned int crc = 0xFFFFFFFF;
	const unsigned char * buf = (const unsigned char *) data;
	for (int i = 0; i < length; i++)
	{
		unsigned int byte = *buf++;
		for (int j = 0; j < 8; j++)
		{
			if ((byte & 0x01) ^ (crc & 0x01))
			{
				crc >>= 1;
				crc = crc ^ 0xEDB88320;
			}
			else
			{
				crc >>= 1;
			}
			byte >>= 1;
		}
	}
	return crc;
}
