// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Arch/MCF/MCF5445x/WC/Board.h_v   1.5.1.1   Mar 06 2017 16:38:14   EGUAJARDO  $
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
//extern unsigned char __BBRAM_START[];
//extern unsigned char __BBRAM_END[];
//extern unsigned char __BBRAM_RAM_START[];
//extern unsigned char __BBRAM_RAM_END[];

extern unsigned char __FLASH_ROM[];
extern unsigned char __FLASH_BASE[];
extern unsigned char __FLASH_SIZE[];
extern unsigned char __FLASH_END[];
extern unsigned char __TEXT_START[];
extern unsigned char __TEXT_END[];
extern unsigned char __FLASH_RAM[];

extern int __SERIAL_NUMBER;
extern unsigned char __ETHERNET_MAC[6];
extern unsigned char __ETHERNET_MAC0[6];
extern unsigned char __ETHERNET_MAC1[6];

#define	PRINTF_INPUT_SIZE	  25000
#define PRINTF_OUTPUT_SIZE	  25000
#define CHECK_FOR_AA13_RESISTOR3 (MCF_EPORT_EPPDR & MCF_EPORT_EPPDR_EPPD3)  //to read the pin I'm interested in. EPPDR reflects the current state of the pin
#define CHECK_FOR_AB14_RESISTOR4 (MCF_EPORT_EPPDR & MCF_EPORT_EPPDR_EPPD4)  //to read the pin I'm interested in. EPPDR reflects the current state of the pin
#define CONFIGURE_IRQ3PIN_FOR_SENSING_VOLTAGE_LEVEL    0xFF3F
#define CONFIGURE_IRQ4PIN_FOR_SENSING_VOLTAGE_LEVEL    0xFCFF

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
	static bool rmii;
	//Timers
	static void InstallTimer(int usInterval, TimerCallback callback,
			int instance);
	static void StartTimer(int instance);
	static void StopTimer(int instance);
	//RTC
	static void InitRTC();
	static int GetRTCTime();
	static I2C * GetI2C(int instance, int level, int priority);
	static Ethernet * eth[];
	static int EthCount()
	{
		return 2;
	}
	static void InitProfilerTimer(int instance);
	static void InitDMATimer(int instance);
	// Inline for better performance in the profiler
	inline static int GetProfilerTimerCounter(int instance)
	{
		return MCF_DTIM_DTCN(instance);
	}
protected:
	static SPI * spi[1];
	static I2C * i2c[1];
	static int dmaTimers;
private:
    static void SetupforSensingVoltageonAA13andAB14(void);
    static void CheckforVoltageSignalonAA13Resistor3(void);
    static void CheckforVoltageSignalonAB14Resistor4(void);
    static void EnableWatchDog(void);
    static bool VoltageonResistor3Flag; //Assuming Resistor3 is NOT grounded (+3.3V is Detected, WD IS NOT Installed)
    static bool VoltageonResistor4Flag; //Assuming Resistor4 is NOT grounded (+3.3V is Detected, WD IS NOT Installed)
};

#endif
