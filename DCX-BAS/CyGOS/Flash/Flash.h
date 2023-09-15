// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Flash/Flash.h_v   1.0   21 Mar 2012 12:37:32   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3 MultiBoot loader.
//
// This module implements the base flash chip class.

#ifndef FLASH_H_
#define FLASH_H_

class FlashAccess
{
public:
	FlashAccess(int bits);
	virtual void Write(unsigned int address, unsigned int data);
	virtual unsigned int Read(unsigned int address);
protected:
	bool wordAccess;
};

#ifdef FUTURE
/**
 * CFI Flash QRY data structure. Structure is stored in the flash and is
 * accessed through command 0x90 (Read Device Information). Each byte in this
 * structure is read using a single access starting at address 0x10 in the
 * flash. If flash uses 16-bit data bus, structure starts at short[0x10], if
 * flash uses 8-bit data bus, structure starts at char[0x10]. All fields use
 * Intel byte ordering regardless of manufacturer.
 */
struct QRY
{
	enum AlgoID
	{
		AlgoNull = 0x0000,
		AlgoIntelSharp = 0x0001,
		AlgoAmdFujitsu = 0x0002,
		AlgoIntelStandard = 0x0003,
		AlgoWinbond = 0x0006,
		AlgoMitsubishiStandard = 0x0100,
		AlgoMitsubishiExtended = 0x0101,
		AlgoSST = 0x0102,
		AlgoIntelPerformanceCode = 0x0200,
		AlgoIntelDataCommandSet = 0x0210,
	};
	enum DevInterface
	{
		Async8bitOnly = 0x0000,
		Async16bitOnly = 0x0001,
		Async8or16bit = 0x0002,
		Async32bitOnly = 0x0003,
		Async16or32bit = 0x0004,
	};
	/**
	 * String 'QRY'.
	 */
	char qry[3];
	/**
	 * Primary algorithm command set and control interface ID code.
	 */
	char priAlgoID[2];
	/**
	 * Address for primary algorithm extended query table, or 0.
	 */
	char priAlgoQry[2];
	/**
	 * Alternate algorithm command set and control interface ID code or 0.
	 */
	char altAlgoID[2];
	/**
	 * Address for secondary algorithm extended query table, or 0.
	 */
	char secAlgoQry[2];
	/**
	 * Minimum supply VCC, upper nibble volts, lower nibble 100mv increments.
	 */
	char vccMin;
	/**
	 * Maximum supply VCC, upper nibble volts, lower nibble 100mv increments.
	 */
	char vccMax;
	/**
	 * Minimum program VPP, upper nibble volts, lower nibble 100mv increments.
	 */
	char vppMin;
	/**
	 * Maximum program VPP, upper nibble volts, lower nibble 100mv increments.
	 */
	char vppMax;
	/**
	 * Single byte/word/dword program time, 2^N microseconds, N=0 - not
	 * supported.
	 */
	char typicalSingleProgramTime;
	/**
	 * Multiple byte/word/dword program time, 2^N microseconds, N=0 - not
	 * supported.
	 */
	char typicalMultiProgramTime;
	/**
	 * Single block erase time, 2^N milliseconds, N=0 - not supported.
	 */
	char typicalBlockEraseTime;
	/**
	 * Whole chip erase time, 2^N milliseconds, N=0 - not supported.
	 */
	char typicalFullChipEraseTime;
	/**
	 * Single byte/word/dword program time, 2^N times typical, N=0 - not
	 * supported.
	 */
	char maximumSingleProgramTime;
	/**
	 * Multiple byte/word/dword program time, 2^N times typical, N=0 - not
	 * supported.
	 */
	char maximumMultiProgramTime;
	/**
	 * Single block erase time, 2^N times typical, N=0 - not supported.
	 */
	char maximumBlockEraseTime;
	/**
	 * Whole chip erase time, 2^N times typical, N=0 - not supported.
	 */
	char maximumFullChipEraseTime;
	/**
	 * Device size, 2^N bytes.
	 */
	char deviceSize;
	/**
	 * Flash Device Interface Code description. Values in DevInterface enum.
	 */
	char interfaceCode;
	/**
	 * Maximum multi-byte program size, 2^N bytes.
	 */
	char maxMultiByteProgram;
	/**
	 * Number of Erase Block Regions.
	 */
	char eraseBlockRegionCount;
	char eraseBlockRegion[4];
};

struct Algorithm
{
	/**
	 * 'PRI' or 'ALT'
	 */
	char priOrAlt[3];
	/**
	 * Major and minor version numbers in ASCII.
	 */
	char asciiVersion[2];
};
#endif

typedef void (*FlashCallback)(int progress, int limit);

class Flash
{
public:
	virtual bool Identify() = 0;
	char * GetName() const
	{
		return name;
	}
	unsigned int GetBase() const
	{
		return base;
	}
	unsigned int GetSize() const
	{
		return size;
	}
	static void SetAccess(FlashAccess * acc)
	{
		access = acc;
	}
	void Write(unsigned int address, unsigned int data)
	{
		access->Write(address, data);
	}
	unsigned int Read(unsigned int address)
	{
		return access->Read(address);
	}
	virtual bool Program(void * flash, void * data, int length,
			FlashCallback cb = 0) = 0;
	virtual bool Erase(void * flash, int length, FlashCallback cb = 0) = 0;
	virtual void Unlock(void * flash, int length) = 0;
	virtual void UnlockChip() = 0;
	virtual void Lock(void * flash, int length) = 0;
	virtual void LockChip() = 0;
	virtual int GetSectorSize(void * flash) = 0;
	static FlashAccess * access;
protected:
	Flash(char * flashName, unsigned int baseAddress, int flashId,
			int flashSize, int bits);
	char * name;
	int type;
	unsigned int base;
	int size;
	bool wordAccess;
};

#endif
