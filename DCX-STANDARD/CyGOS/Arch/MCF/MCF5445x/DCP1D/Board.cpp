// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Arch/MCF/MCF5445x/DCP1D/Board.cpp_v   1.5   07 Sep 2011 17:23:02   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This object encapsulates board-specific initialization.
//
// TODO Everything is copied from M54455EVB, only flash and SDRAM settings are verified. The rest must be checked.

#include "Board.h"
#include "Terminal.h"
#include "Cache.h"
#include "CPUdefs.h"
#include "Exception.h"
#include "CPUdefs.h"
#include "assert.h"
#include "DebugNew.h"
#include "Debug.h"
#include "LED.h"
#include "DSPI.h"
#include "stdio.h"
#include "PIT.h"
//#include "RTC.h"
#include "I2C.h"

extern int __SYSTEM_CLOCK;

SPI * Board::spi[1];
I2C * Board::i2c[1];
/**
 * Init: Initialize the board-specific hardware.
 * <ol>
 * <li>Set up CS3 memory region.
 * <li>Initialize the terminal port.
 * <li>Initialize the cache for the SDRAM area.
 * <li>Initialize exception handling.
 * <li>Initialize LEDs.
 * <li>When all done, go to main().
 * </ol>
 * @param cache When true, cache must be initialized. This routine is used by
 * MultiBoot and by application code. Application should not touch the cache
 * because MultiBoot code already initialized it.
 */
void Board::Init(bool cache)
{
	MCF_FBCS_CSMR(0) = (MCF_FBCS_CSMR(0) & 0x0000FFFF) | MCF_FBCS_CSMR_BAM_4M;
	MCF_CLOCK_PSR |= (0x10 | MCF_CLOCK_PSR_LOLRE);//0x10;
	Terminal::Init();
	if (cache)
	{
		extern char __SDRAM_BASE[];
		int sdram = (int) __SDRAM_BASE;
		int mask = MCF5XXX_ACR_AM_128M;
		Cache::Init(sdram, mask);
	}
	Exception::Init();
	Debug::Init();
	LED::Init();
}

/**
 * UpdateLEDs: Update All LEDs to specified state. LED class does not have any
 * board-specific code to affect any LED outputs, so Board class provides this
 * functionality.
 * @param state LED state buffer.
 */
void Board::UpdateLEDs(unsigned char * state)
{
	printf("No LEDs %d",(int)state);

}

/**
 * AllocAlignedNoCache: Allocate memory block at least the specified size in
 * non-cacheable area of main memory.<p>
 * Because of alignment requirements, each block can have some unused memory in
 * the beginning of memory block. This amount of memory is random for each
 * memory block. So, for deallocating the memory, allocated memory block must
 * have a pointer through which it can be deallocated. For consistency, this
 * true malloc pointer is stored right before the memory pointer that is
 * returned by this function. This way, Free can extract the true malloc pointer
 * using the provided aligned pointer, and free memory using it.
 * @param size Size fo the memory block to allocate.
 * @param align Memory region alignment. If anything less than 16 is passed in
 * the parameter, 16 will be used. Parameter is also verified to be a power of
 * two.
 * @return void * A pointer to the aligned non-cacheable area of specified size.
 */
void * Board::AllocAlignedNoCache(size_t size, int align)
{
	// If requested alignment is more than 16 bytes (USB for example), the code
	// in malloc() will fail because alignment buffers are not handled properly
	// in ShowMemory code.
	ASSERT(align <= 16); // TODO Fix alignment more than 16 bytes!
	// value and-ed with (value-1) gives 0 only if value has 0 or 1 bits set
	bool powerOfTwo = (align & (align - 1)) == 0;
	ASSERT(powerOfTwo);
	if (align < 16)
		align = 16;
	int allocSize = size + align; // first, alignment extra buffer
	allocSize += 16; // extra safety net in the beginning - CPU cache line size
	allocSize += 16; // extra safety net in the end - CPU cache line size
	// allocate the memory, but don't initialize the data part as malloc() would
	// do
	unsigned char * alloc = (unsigned char *) mallocNoInit(allocSize);
	unsigned char * ptr = alloc + allocSize; // this is the end of allocated buffer
	ptr -= 16; // this is the end of the allocated block
	ptr -= size; // this is the beginning of the allocated block
	// the block is over-allocated by align+16, so here we can just clear lower
	// bits of the pointer and get the aligned result
	ptr = (unsigned char *) (((int) ptr) & ~(align - 1));
	// Now ptr is aligned, and this pointer will be used to reference the
	// allocated memory when we want to free it. We need to store the true
	// allocated pointer value.
	((unsigned char **) ptr)[-1] = alloc;
	return ptr - 0x10000000; // shift of 256MB between cache and no-cache regions
}

/**
 * FreeAlignedNoCache: Free a block of memory that was allocated as aligned
 * non-cacheable memory. Function verifies that correct pointer is passed to
 * be deallocated, by checking the pointer shift value that was added to move
 * the pointer out of cache range when buffer was allocated.
 * @param ptr Pointer returned by AllocAlignedNoCache function call.
 */
void Board::FreeAlignedNoCache(void * ptr)
{
	//unsigned int ptrValue = (unsigned int) ptr;
	//	ASSERT(ptrValue & 0x01000000);
	unsigned char * cp = (unsigned char *) ptr;
	cp += 0x10000000; // shift of 256MB between cache and no-cache regions
	void * alloc = ((unsigned char **) cp)[-1];
	free(alloc);
}

void Board::SetupSlowCS8bit(int cs, int wait, volatile void * addr)
{
	CRASH("SetupSlowCS8bit");
	MCF_FBCS_CSAR(cs) = MCF_FBCS_CSAR_BA((int) addr);
	uint32 reg = 0;
	reg |= MCF_FBCS_CSCR_PS_8;
	// setup time - chip select low on second clock after address is set
	reg |= MCF_FBCS_CSCR_ASET(2);
	// auto-acknowledged
	reg |= MCF_FBCS_CSCR_AA;
	// wait states
	reg |= MCF_FBCS_CSCR_WS(wait);
	reg |= MCF_FBCS_CSCR_RDAH(1);
	reg |= MCF_FBCS_CSCR_WRAH(1);
	reg |= MCF_FBCS_CSCR_SWSEN;
	MCF_FBCS_CSCR(cs) = reg;
	//	| MCF_FBCS_CSCR_RDAH(3) | MCF_FBCS_CSCR_WRAH(3)
	reg = MCF_FBCS_CSMR_BAM_2M; // this is 2MB chip select address space
	// and the chip select is valid
	MCF_FBCS_CSMR(cs) = reg | MCF_FBCS_CSMR_V;
}

SPI * Board::GetSPI(int instance, int irqlevel, int irqprio)
{
	ASSERT(instance == 0);
	if (!spi[instance])
		spi[instance] = new DSPI(instance, irqlevel, irqprio);
	return spi[instance];
}

void Board::InstallTimer(int usInterval, TimerCallback callback, int instance)
{
	PIT * timerPtr = new PIT(instance, (usInterval * __SYSTEM_CLOCK), 1, 1,
			callback, 256);
	printf("Timer %d",(int)timerPtr);
}

void Board::StartTimer(int instance)
{
	PIT::StartInt(instance);
}

void Board::StopTimer(int instance)
{
	PIT::StopInt(instance);
}

void Board::InitRTC()
{
	//	RTC * rtcPtr = new RTC;
	//	rtcPtr->Init(0, 0);
}
int Board::GetRTCTime()
{
	return 0;//RTC::Get_time();
}

I2C * Board::GetI2C(int instance, int level, int priority)
{
	ASSERT(instance == 0);
	if (!i2c[instance])
		i2c[instance] = new I2C(instance, 0, 5, 5);
	return i2c[instance];
	level = 0;
	priority = 0;
}
