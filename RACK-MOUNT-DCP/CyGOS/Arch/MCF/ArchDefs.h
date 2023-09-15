// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Arch/MCF/ArchDefs.h_v   1.0   18 Aug 2014 09:15:26   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares CPU architecture-specific data types and routines.

#ifndef ARCHDEFS_H_
#define ARCHDEFS_H_

#include "CpuDefs.h"

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

#define INT_MAX 0x7FFFFFFF

/**
 * size_t: 'Size of an object' type. Defined as unsigned long, this is enough
 * for any 32-bit architecture. Coldfire G++ expects this to be unsigned long.
 */
typedef unsigned long size_t;

/**
 * SR: Data type for CPU status register.
 */
typedef short SR;

SR EnterCritical();
void ExitCritical(SR sr);
void DisableInterrupts();
void EnableInterrupts();

extern "C"
{

inline unsigned long ntohl(unsigned long value)
{
	return value;
}

inline unsigned short ntohs(unsigned short value)
{
	return value;
}

inline unsigned long htonl(unsigned long value)
{
	return value;
}

inline unsigned short htons(unsigned short value)
{
	return value;
}

}

/**
 * class CriticalSection: Critical code section handler. Critical section of
 * code begins when the object is constructed, and ends when this object goes
 * out of scope. Object is useful when there are multiple return points in the
 * function, for example.
 */
class CriticalSection
{
public:
	/**
	 * CriticalSection: Begin a critical section of code. Disable interrupts.
	 * Current interrupt state is saved before disabling.
	 */
	CriticalSection()
	{
		sr = EnterCritical();
	}
	/*
	 * ~CriticalSection: End a critical section of code. Restore interrupt
	 * status to what it was before critical section was entered.
	 */
	~CriticalSection()
	{
		ExitCritical(sr);
	}
protected:
	/**
	 * SR: CPU status register containing interrupt enable/disable status.
	 */
	SR sr;
};

/**
 * Mis-aligned short integer. Data is stored in host byte order. Alignment of
 * this integer can be anything even if CPU does not support the requested
 * alignment. Data type is mostly used in LWIP code.
 */
class MisAlignedShort
{
public:
	void operator =(uint16 newValue)
	{
		value = newValue;
	}
	operator uint16()
	{
		return value;
	}
private:
	uint16 value;
};

/**
 * Mis-aligned long integer. Data is stored in host byte order. Alignment of
 * this integer can be anything even if CPU does not support the requested
 * alignment. Data type is mostly used in LWIP code.
 */
class MisAlignedLong
{
public:
	void operator =(uint32 newValue)
	{
		value = newValue;
	}
	operator uint32()
	{
		return value;
	}
private:
	uint32 value;
};

/**
 * Mis-aligned short integer. Data is stored in network byte order. Alignment of
 * this integer can be anything even if CPU does not support the requested
 * alignment. Data type should be used in structures that are transferred over
 * the network.
 */
class NetShort
{
public:
	void operator =(uint16 newValue)
	{
		value = newValue;
	}
	operator uint16()
	{
		return value;
	}
private:
	uint16 value;

};

/**
 * Mis-aligned long integer. Data is stored in network byte order. Alignment of
 * this integer can be anything even if CPU does not support the requested
 * alignment. Data type should be used in structures that are transferred over
 * the network.
 */
class NetLong
{
public:
	void operator =(uint32 newValue)
	{
		value = newValue;
	}
	operator uint32()
	{
		return value;
	}
private:
	uint32 value;
};

#endif
