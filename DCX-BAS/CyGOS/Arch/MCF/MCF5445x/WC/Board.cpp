// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Arch/MCF/MCF5445x/WC/Board.cpp_v   1.10.1.1   Mar 06 2017 16:15:30   EGUAJARDO  $
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
#include "MMU.h"
#include "I2C.h"
#include "TaskManager.h"

extern int __SYSTEM_CLOCK;

SPI * Board::spi[1];
I2C * Board::i2c[1];
int Board::dmaTimers = 0;
bool Board::VoltageonResistor3Flag = TRUE; //Assuming Resistor3 is NOT grounded (+3.3V is Detected, WD IS NOT Installed)
bool Board::VoltageonResistor4Flag = TRUE; //Assuming Resistor4 is NOT grounded (+3.3V is Detected, WD IS NOT Installed)
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
/**********************************************************
 *
 * Configuring Registers and disabling interruptions
 *
 *********************************************************/
void Board::SetupforSensingVoltageonAA13andAB14(void)
{
    //Eport data direction register settings
    MCF_EPORT_EPDDR &= ~(MCF_EPORT_EPDDR_EPDD3); // Configure as input. Setting any bit in a PDDR_x register configures the corresponding port pin as an output
    //Eport interrupt enable register settings
    MCF_EPORT_EPIER &= ~(MCF_EPORT_EPIER_EPIE3); // Disabling interrupts. If a bit in EPIER is set, EPORT generates an interrupt
    MCF_EPORT_EPPAR &= CONFIGURE_IRQ3PIN_FOR_SENSING_VOLTAGE_LEVEL; //This will prepare the pin for sensing voltage level

    //Eport data direction register settings
    MCF_EPORT_EPDDR &= ~(MCF_EPORT_EPDDR_EPDD4); // Configure as input. Setting any bit in a PDDR_x register configures the corresponding port pin as an output
    //Eport interrupt enable register settings
    MCF_EPORT_EPIER &= ~(MCF_EPORT_EPIER_EPIE4); // Disabling interrupts. If a bit in EPIER is set, EPORT generates an interrupt
    MCF_EPORT_EPPAR &= CONFIGURE_IRQ4PIN_FOR_SENSING_VOLTAGE_LEVEL; //This will prepare the pin for sensing voltage level
}


/*********************************************************
 *
 *             Verifying Voltage on R3
 *
 ********************************************************/
void Board::CheckforVoltageSignalonAA13Resistor3(void)
{
   if(CHECK_FOR_AA13_RESISTOR3 == FALSE){      // Reading the pin I'm interested in
      printf("\n\n 0V Detected on Resistor3");
      VoltageonResistor3Flag = FALSE;      // Set to FALSE because Resistor3 is Grounded (NO +3.3V is Detected)
   }
}


/*********************************************************
 *
 *             Verifying Voltage on R4
 *
 *********************************************************/
void Board::CheckforVoltageSignalonAB14Resistor4(void)
{
   if(CHECK_FOR_AB14_RESISTOR4 == FALSE){     // Reading the pin I'm interested in
      printf("\n\n 0V Detected on Resistor4");
      VoltageonResistor4Flag = FALSE;     // Set to FALSE because Resistor4 is Grounded (NO +3.3V is Detected)
   }
}


/**********************************************************
 *
 *                Enable Watch Dog
 *
 **********************************************************/
void Board::EnableWatchDog(void)
{
   MCF_PAD_PAR_TIMER |= (MCF_PAD_PAR_TIMER_PAR_T3IN(MCF_PAD_PAR_TIMER_PAR_T0IN_T0OUT));
   MCF_DTIM_DTMR(3) = MCF_DTIM_DTMR_CLK(1)| MCF_DTIM_DTMR_PS(__SYSTEM_CLOCK - 1) //1MHz
      | MCF_DTIM_DTMR_OM | MCF_DTIM_DTMR_FRR; //clock source 120 Mhz/Toggle output/Restart

   MCF_DTIM_DTXMR(3) |= MCF_DTIM_DTXMR_HALTED;//stop counting when core halted/Increment by one
   MCF_DTIM_DTRR(3) = 500; //(2 Khz)
   MCF_DTIM_DTCN(3) = 0;
   MCF_DTIM_DTMR(3) |= MCF_DTIM_DTMR_RST;
}

void Board::Init(bool cache)
{
	MMU::RemapCodeData(false,cache);
	MCF_FBCS_CSMR(0) = (MCF_FBCS_CSMR(0) & 0x0000FFFF) | MCF_FBCS_CSMR_BAM_16M;
	MCF_CLOCK_PSR |= (0x10 | MCF_CLOCK_PSR_LOLRE);//0x10;
	Terminal::Init();
	pprintf("\n Board Init");
	//pprintf("Board init with cache %s", cache?"enabled":"disabled");
	//cache = TRUE;
	if (cache)
	{
		SetupforSensingVoltageonAA13andAB14();                         //Configuring Registers and disabling interruptions REQWATHCODG
		CheckforVoltageSignalonAA13Resistor3();                        //Verifying Voltage on R3	REQWATHCODG
		CheckforVoltageSignalonAB14Resistor4();                        //Verifying Voltage on R4	REQWATHCODG

		if(VoltageonResistor3Flag == FALSE && VoltageonResistor4Flag == TRUE){ //Watch Dog is installed	REQWATHCODG
			EnableWatchDog();		//REQWATHCODG
		}
		//ASSERT(0);
		extern char __SDRAM_BASE[];
		int sdram = (int) __SDRAM_BASE;
		int mask = MCF5XXX_ACR_AM_128M;
		//pprintf("\n Cache Init");
		Cache::Init(sdram, mask);
		pprintf("\n Cache Init Done");
	}
	// If running the app, initialize profiler DMA timer. The cost of DTIM
	// running at all times is low, so here we can just let it run forever
	// even if we don't need it.
	else
	{
		TaskManager::InitTiming();
		pprintf("\n TaskManager InitTiming Done");
	}

	Exception::Init();
	pprintf("\n Exception Init Done");
	Debug::Init();
	pprintf("\n Debug Init Done");
	LED::Init();
	pprintf("\n Board Init Done");
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
	//pprintf("\n %s 1", __FUNCTION__);
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
	//pprintf("\n %s 2 %08X %08X %08X %08X", __FUNCTION__, alloc, ptr, ptr - 0x10000000, ptr - 0x0F000000);
	return ptr - 0x10000000; // shift of 256MB between cache and no-cache regions: Didn't work with 240MB??
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
	pprintf("Timer %d,addr %x ",(int)instance,timerPtr);
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
	//Init On Chip RTC
}
int Board::GetRTCTime()
{
	return 0;
}

I2C * Board::GetI2C(int instance, int level, int priority)
{
	//This board with MCF54452 chip only has
	//only one I2C hardware instance
	//The only valid instance number is 0
	//Assert if instance is not valid.
	ASSERT(instance == 0);
	if (!i2c[instance])
		//All i2c chips by are running at 400KHZ
		i2c[instance] = new I2C(instance, 0, level, priority);
	return i2c[instance];
}

void Board::InitDMATimer(int instance)
{
//	DisableInterrupts();
   ASSERT((instance >=0 && instance <=3));
   MCF_DTIM_DTMR(instance) |= MCF_DTIM_DTMR_CLK(1);//125/1
   MCF_DTIM_DTMR(instance) |= MCF_DTIM_DTMR_PS(124);//125/125
   MCF_DTIM_DTMR(instance) &= ~MCF_DTIM_DTMR_RST;
   MCF_DTIM_DTMR(instance) |= MCF_DTIM_DTMR_RST;
}

/**
 * Initialize the DMA timer for task manager execution profiling. The function
 * must be called only once, if called from boot loader it did not start ticking
 * after being initialized in the application.
 */
void Board::InitProfilerTimer(int instance)
{
	CriticalSection s;
	ASSERT((instance >= 0 && instance <= 3));
	ASSERT((dmaTimers & (1 << instance)) == 0);
	MCF_DTIM_DTMR(instance) = MCF_DTIM_DTMR_CLK(1)
			| MCF_DTIM_DTMR_PS(__SYSTEM_CLOCK - 1);
	MCF_DTIM_DTXMR(instance) = 0;
	MCF_DTIM_DTCN(instance) = 0;
	MCF_DTIM_DTMR(instance) |= MCF_DTIM_DTMR_RST;
	dmaTimers |= 1 << instance;
}
