// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Arch/MCF/MCF5445x/CPU.h_v   1.0   29 Oct 2010 10:38:32   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This file declares objects for peripheral devices built into the CPU.

#ifndef CPU_H_
#define CPU_H_

#include "Exception.h"

class PIT;

/**
 * enum CpuInterrupt: Interrupt vector values as defined for the CPU from
 * interrupt controller perspective. All vectors defined in this enum are in
 * range 0...127. 64 must be added to get the real interrupt vector number.
 */
enum CpuInterrupt
{
	intUnused00,
	intEPFR1,
	intEPFR2,
	intEPFR3,
	intEPFR4,
	intEPFR5,
	intEPFR6,
	intEPFR7,
	intEDMA0,
	intEDMA1,
	intEDMA2,
	intEDMA3,
	intEDMA4,
	intEDMA5,
	intEDMA6,
	intEDMA7,
	intEDMA8,
	intEDMA9,
	intEDMA10,
	intEDMA11,
	intEDMA12,
	intEDMA13,
	intEDMA14,
	intEDMA15,
	intEDMAError,
	intUART0 = 26,
	intUART1,
	intUART2,
	intUnused29,
	intI2C,
	intDSPI,
	intDTimer0,
	intDTimer1,
	intDTimer2,
	intDTimer3,
	intFec0TransmitFrame,
	intFec0TransmitBuffer,
	intFec0TxFifoUnderrun,
	intFec0CollisionRetryLimit,
	intFec0ReceiveFrame,
	intFec0ReceiveBuffer,
	intFec0Mii,
	intFec0LateCollision,
	intFec0HeartbeatError,
	intFec0GracefulStop,
	intFec0EthernetBusError,
	intFec0BabblingTransmit,
	intFec0BabblingReceive,
	intFec1TransmitFrame,
	intFec1TransmitBuffer,
	intFec1TxFifoUnderrun,
	intFec1CollisionRetryLimit,
	intFec1ReceiveFrame,
	intFec1ReceiveBuffer,
	intFec1Mii,
	intFec1LateCollision,
	intFec1HeartbeatError,
	intFec1GracefulStop,
	intFec1EthernetBusError,
	intFec1BabblingTransmit,
	intFec1BabblingReceive,
	intCoreBusError,
	intRTC,
	intDSPIEndOfQueue = 33 + 64,
	intDSPITransmitFIFOFill,
	intDSPITransferComplete,
	intDSPITransmitFIFOUnderFlow,
	intDSPIReceiveFIFONotEmpty,
	intDSPIReceiveFIFOOverFlow,
	intDSPIrecOverFlowORTransmitUnderFlow,
	intRNG,
	intPIT0 = 43 + 64,
	intPIT1,
	intPIT2,
	intPIT3,
	intUSBOTG,
	intUnused48Plus64,
	intSSI,
};

enum CpuInterruptMode
{
	CpuIntModeLevelLow = 0x00,
	CpuIntModeEdgeRising = 0x01,
	CpuIntModeEdgeFalling = 0x02,
	CpuIntModeEdgeBoth = 0x03
};

inline int IntVector(CpuInterrupt i)
{
	return i + 64;
}

/**
 * class CPU: Class implements CPU-specific functions. All functions and data
 * members are static, so no instance of this type is necessary.
 */
class CPU
{
public:
	static void Restart();
	static void SetupCS8bit(int cs, int wait, int width, volatile void * addr);
	static void SetupHandler(ExceptionHandler handler, CpuInterrupt vector,
			int irqlevel, int irqprio, int stackSize);
	static void InstallRtosTimer(int usec, void(*handler)(), int stackSize);
	static void InstallTaskSwitchRoutine(void(*TaskSwitch)());
	static void InitExternalInterrupt(ExceptionHandler handler, int intr,
			int prio, int stackSize, CpuInterruptMode mode, bool enable);
	static void EnableExternalInterrupt(int intr);
	static void DisableExternalInterrupt(int intr);
	static void ClearExternalInterrupt(int intr);
	static void SwitchStack(void * newStack);
	/**
	 * GetStackExtra: CPU-specific minimum overhead that must be allocated for
	 * each task, in addition to estimated work stack size. The extra includes
	 * space for all CPU registers, function stack frame and some minimum extra
	 * space (16 entries).
	 * @return int Extra space to allocate to each task stack, in long words.
	 */
	inline static int GetStackExtra()
	{
		// D0...D7, A0...A6, 4 for stack frame
		// and 16 (arbitrary) minimum for something productive
		int stackExtra = 8 + 7 + 4 + 16;
		return stackExtra;
	}
	/**
	 * ForceTaskSwitch: CPU-specific way to force RTOS task switch from user
	 * task code.
	 */
	inline static void ForceTaskSwitch()
	{
		asm("trap #15");
	}
	static void DelayMs(int ms);
	static void DelayUs(int us);

	static const int ResetD0;
	static const int ResetD1;

protected:
	/**
	 * WriteCACR: Write a value into cache control register.
	 * @param value Value to write.
	 */
	static inline void WriteCACR(int value)
	{
		asm volatile ("movec %0,%%cacr": : "d"(value));
	}
	/**
	 * WriteACR0: Write a value into cache access control register 0.
	 * @param value Value to write.
	 */
	static inline void WriteACR0(int value)
	{
		asm volatile ("movec %0,%%acr0": : "d"(value));
	}
	/**
	 * WriteACR1: Write a value into cache access control register 1.
	 * @param value Value to write.
	 */
	static inline void WriteACR1(int value)
	{
		asm volatile ("movec %0,%%acr1": : "d"(value));
	}
private:
	static void Init();

	static PIT * rtosPit;
};

#endif
