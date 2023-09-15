// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/stdlib/StdLib.cpp_v   1.0   18 Aug 2014 09:16:50   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements stdlib.h functions.

#include "stdlib.h"

int atoi(const char *string)
{
	int result = 0;
	while (*string)
	{
		int ch = *string++;
		if ((ch >= '0') && (ch <= '9'))
		{
			result *= 10;
			result += ch - '0';
		}
		else
			break;
	}
	return result;
}

int xtoi(const char * hexNumber)
{
	int result = 0;
	for (int i = 0; hexNumber[i] > 0; i++)
	{
		result <<= 4;
		int c = hexNumber[i];
		if ((c >= '0') && (c <= '9'))
			result |= c & 0x0F;
		else if ((c >= 'A') && (c <= 'F'))
			result |= c - 'A' + 10;
		else if ((c >= 'a') && (c <= 'f'))
			result |= c - 'a' + 10;
		else
			break;
	}
	return result;
}

int otoi(const char * octNumber)
{
	int result = 0;
	for (int i = 0; octNumber[i] > 0; i++)
	{
		result <<= 3;
		int c = octNumber[i];
		result |= c & 0x0F;
	}
	return result;
}
