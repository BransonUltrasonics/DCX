// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Arch/MCF/MCF5445x/Board.h_v   1.0   24 Jul 2015 09:18:20   sesharma  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This object encapsulates board-specific initialization.

#ifndef BOARD_H_
#define BOARD_H_

#include "stdlib.h"

class Ethernet;
class I2C;

// These definitions do not belong into CPU definitions file because they are
// board-specific, a board may or may not have BBRAM...

extern unsigned char __FLASH_ROM[];
extern unsigned char __FLASH_BASE[];
extern unsigned char __FLASH_SIZE[];
extern unsigned char __FLASH_END[];
extern unsigned char __TEXT_START[];
extern unsigned char __TEXT_END[];
extern unsigned char __FLASH_RAM[];
extern unsigned char __USER_FLASH_SIZE[];//TODO: Test it

extern int __SERIAL_NUMBER;
extern unsigned char __ETHERNET_MAC[6];
extern unsigned char __ETHERNET_MAC0[6];
extern unsigned char __ETHERNET_MAC1[6];

#define	PRINTF_INPUT_SIZE	  25000
#define PRINTF_OUTPUT_SIZE	  25000


class MII;
class Flash;
class SPI;
typedef void (*TimerCallback)();

/**
 * Class Board: Class implements board-specific functionality. All functions
 * and data members are static, so no instance of this class is necessary.
 */
class Board
{
public:
	static void * AllocAlignedNoCache(size_t size, int align);
	static void FreeAlignedNoCache(void * ptr);
	static void Init(bool cache);
	static void UpdateLEDs(unsigned char * state);
	static void SetupSlowCS8bit(int cs, int wait, volatile void * addr);
	static MII * GetMII(int channel);
	static Flash * GetFlash();
	static SPI * GetSPI(int instance, int irqlevel, int irqprio);
	static bool rmii[];
	//Timers
	static void InstallTimer(int usInterval, TimerCallback callback,int instance);
	static void StartTimer(int instance);
	static void StopTimer(int instance);
	static void RemoveTimer(int instance);
	const static int spiNum;
	const static int i2cNum;
	const static int profileTimerNum;
	//
	//RTC
	static int GetRTCTime();
	static I2C * GetI2C(int instance, int level, int priority, int clk);
	static Ethernet * eth[];
	static int EthCount();

	static void InitProfilerTimer();
	// Inline for better performance in the profiler
	static int GetProfilerTimerCounter();



protected:
	static SPI * spi[];
	static I2C * i2c[];
	static MII * MIIs[];
	static int dmaTimers;
};

#endif
