// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Arch/MCF/MCF5445x/Cache.h_v   1.0   21 Mar 2012 12:28:30   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This object encapsulates CPU-specific cache initialization.

#ifndef CACHE_H_
#define CACHE_H_

class Cache
{
public:
	static void Init(int sdram, int mask);
protected:
	/**
	 * WriteCACR: Write a value into cache control register.
	 * @param value Value to write.
	 */
	static inline void WriteCACR(int value)
	{
		asm volatile ("movec %0,%%cacr": : "d"(value));
		asm(" nop");
	}
	/**
	 * WriteACR0: Write a value into cache access control register 0.
	 * @param value Value to write.
	 */
	static inline void WriteACR0(int value)
	{
		asm volatile ("movec %0,%%acr0": : "d"(value));
		asm(" nop");
	}
	/**
	 * WriteACR1: Write a value into cache access control register 1.
	 * @param value Value to write.
	 */
	static inline void WriteACR1(int value)
	{
		asm volatile ("movec %0,%%acr1": : "d"(value));
		asm(" nop");
	}
	/**
	 * WriteACR2: Write a value into cache access control register 2.
	 * @param value Value to write.
	 */
	static inline void WriteACR2(int value)
	{
		asm volatile ("movec %0,%%acr2": : "d"(value));
		asm(" nop");
	}
	/**
	 * WriteACR3: Write a value into cache access control register 3.
	 * @param value Value to write.
	 */
	static inline void WriteACR3(int value)
	{
		asm volatile ("movec %0,%%acr3": : "d"(value));
		asm(" nop");
	}

};

#endif
