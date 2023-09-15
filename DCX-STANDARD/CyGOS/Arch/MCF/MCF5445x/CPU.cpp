// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Arch/MCF/MCF5445x/CPU.cpp_v   1.2   01 Sep 2011 13:09:00   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This file implements objects for peripheral devices built into the CPU.

// FIXME Flash burning on M54455EVB
// Sometimes flash burning algorithm runs through erase/program very fast and
// the flash contents are not changed. Once this happens, power cycle is needed,
// or boot loader serial download may also help. This shows up on M54455EVB,
// not sure about other platforms.

#include "CPU.h"
#include "CPUdefs.h"
#include "ArchDefs.h"
#include "PIT.h"
#include "assert.h"
#include "DebugNew.h"
#include "MMU.h"

/**
 * rtosPit: Programmable interrupt timer that is used for RTOS task switching.
 */
PIT * CPU::rtosPit;

/**
 * ResetD0: D0 register value right after reset.
 * <table border>
 * <tr><th>PF<td>31..24<td>11001111<td>Processor family - ColdFire</tr>
 * <tr><th>VER<td>23..20<td>0011<td>ColdFire core V3</tr>
 * <td><th>REV<td>19..16<td>0000<td>Core revision</tr>
 * <td><th>MAC<td>15<td>0<td>MAC is not present</tr>
 * <td><th>DIV<td>14<td>1<td>Hardware divider is present</tr>
 * <td><th>EMAC<td>13<td>1<td>EMAC is present</tr>
 * <td><th>FPU<td>12<td>0<td>FPU is not present</tr>
 * <td><th>MMU<td>11<td>0<td>MMU is not present</tr>
 * <td><th><td>10..8<td>000<td>Reserved</tr>
 * <td><th>ISA<td>7..4<td>1000<td>Core implements ISA_A+</tr>
 * <td><th>DEBUG<td>3..0<td>1001<td>Core uses DEBUG_B+ module</tr>
 * </table>
 */
const int CPU::ResetD0 = 0;
/**
 * ResetD1: D1 register value right after reset.
 * <table border>
 * <tr><th>CLSZ<td>31..30<td>00<td>Cache line size is 16 bytes</tr>
 * <tr><th>CCAS<td>29..28<td>00<td>Configurable cache associativity -
 * four-way</tr>
 * <td><th><td>17..16<td>0..0<td>Reserved</tr>
 * <td><th>MBSZ<td>15..14<td>00<td>32-bit datapath</tr>
 * <td><th>UCAS<td>13..12<td>00<td>Unified cache associativity - four-way</tr>
 * <td><th>UCSZ<td>11..8<td>0110<td>Unified cache size 16K</tr>
 * <td><th>SRAMSZ<td>7..4<td>0111<td>SRAM bank size 32K</tr>
 * <td><th><td>3..0<td>0000<td>Reserved</tr>
 * </table>
 */
const int CPU::ResetD1 = 0;

/**
 * SetupCS8bit: Set up the chip select signal for 8-bit access. Routine assumes
 * that requested chip select pin is available in the CPU package.
 * @param cs Chip select pin number.
 * @param wait Wait states.
 * @param width Chip select width.
 * @param addr Memory area address to access through this chip select signal.
 * Memory area is always assumed to be 2MB.
 */
// FIXME Rename SetupCS8bit, it's not only 8-bit.
void CPU::SetupCS8bit(int cs, int wait, int width, volatile void * addr)
{
	MCF_FBCS_CSAR(cs) = MCF_FBCS_CSAR_BA((int) addr);
	uint32 reg = 0;
	switch (width)
	{
	case 8:
		reg |= MCF_FBCS_CSCR_PS_8;
		break;
	case 16:
		reg |= MCF_FBCS_CSCR_PS_16;
		break;
	case 32:
		reg |= MCF_FBCS_CSCR_PS_32;
		break;
	default:
		ASSERT(0);
	}
	// setup time - chip select low on second clock after address is set
	reg |= MCF_FBCS_CSCR_ASET(1);
	// auto-acknowledged
	reg |= MCF_FBCS_CSCR_AA;
	// wait states
	reg |= MCF_FBCS_CSCR_WS(wait);
	MCF_FBCS_CSCR(cs) = reg;
	//	| MCF_FBCS_CSCR_RDAH(3) | MCF_FBCS_CSCR_WRAH(3)
	reg = MCF_FBCS_CSMR_BAM_2M; // this is 2MB chip select address space
	// and the chip select is valid
	MCF_FBCS_CSMR(cs) = reg | MCF_FBCS_CSMR_V;
	MMU::Map((int) addr);
}

/**
 * SetupHandler: Set up interrupt handler. Function only processes interrupt
 * handlers for internal I/O (vectors 64...127. Procedure is following:
 * <ol>
 * <li>Sanity check:
 * <ol>
 * <li>Interrupt level is 1...6. Can't use 7 because level 7 interrupts seem
 * to get through even if interrupt mask is set to 7.
 * <li>Interrupt priority is in valid range 0...7.
 * <li>Interrupt level and priority pair is not used yet.
 * </ol>
 * <li>Flag the interrupt level and priority pair being used.
 * <li>Set exception handler in the RAM table to the provided handler
 * function. Function must be regular C function (or static member function).
 * It will be called with all interrupts disabled.
 * <li>Set up interrupt controller registers.
 * </ol>
 * @param handler Interrupt handler function.
 * @param vector Interrupt vector. Vector number must be in range 0...127.
 * @param irqlevel Interrupt level, 1...6.
 * @param prio Interrupt priority for CPUs that support the feature.
 */
void CPU::SetupHandler(ExceptionHandler handler, CpuInterrupt vector,
		int irqlevel, int prio, int stackSize)
{
	//Disable compiler warning
	prio = 0;
	// sanity check first
	ASSERT((vector >= 0) && (vector <= 127));
	// level 7 is only allowed for external interrupt because who
	// knows what is attached to the pin...
	ASSERT((irqlevel >= 1) && (irqlevel <= 7));
	if (irqlevel == 7)
		ASSERT(vector == intEPFR7);
	Exception::Set(IntVector(vector), handler, stackSize);
	int intc = (vector >= 64) ? 1 : 0;
	int v = vector & 0x3F;
	if (intc)
		MCF_INTC1_ICR(v) = irqlevel;
	else
		MCF_INTC0_ICR(v) = irqlevel;

	if (v < 32) // low vectors
		MCF_INTC_IMRL(intc) &= ~(1 << v);
	else
		MCF_INTC_IMRH(intc) &= ~(1 << (v - 32));
}

/**
 * InstallTaskSwitchRoutine: RTOS support - install the provided task switch
 * routine. This code is CPU-specific.
 * @param TaskSwitch Routine to call to switch tasks from user code (mainly
 * RTOS::Schedule). Routine must be implemented using interrupt handler stack
 * frame and entry/exit code.
 */
void CPU::InstallTaskSwitchRoutine(void(*TaskSwitch)())
{
	__VECTOR_RAM[32 + 15] = (long) TaskSwitch;
}

/**
 * InstallRtosTimer: RTOS support - configure the programmable interrupt timer
 * and set it up to call the specified function in specified time intervals.
 * Function uses timer 0 to generate RTOS timer interrupts.
 * @param usec Microseconds between timer ticks.
 * @param handler The function to call on each timer tick. Function must be
 * implemented using regular C function stack frame and entry/exit code.
 */
void CPU::InstallRtosTimer(int usec, void(*handler)(), int stackSize)
{
	rtosPit = new PIT(0, usec * __SYSTEM_CLOCK, 6, 7, handler, stackSize);
}

/**
 * InitExternalInterrupt: Initialize external interrupt handling for the CPU.
 * @param handler Interrupt handler function.
 * @param intr Interrupt number, 1...7.
 * @param prio Interrupt priority, ignored for MCF532x series.
 * @param mode Interrupt mode, level or edge sensitive.
 * @param enable Enable the interrupt immediately if true.
 */
void CPU::InitExternalInterrupt(ExceptionHandler handler, int intr, int prio,
		int stackSize, CpuInterruptMode mode, bool enable)
{
	ASSERT((intr >= intEPFR1) && (intr <= intEPFR7));
	SetupHandler(handler, (CpuInterrupt) intr, intr, prio, stackSize);
	// Set the pin assignment register
	int reg = MCF_EPORT_EPPAR;
	int mask = 3 << (intr * 2);
	reg &= ~mask;
	reg |= mode << (intr * 2);
	MCF_EPORT_EPPAR = reg;
	// Set the pin direction - 0 = input.
	mask = 1 << intr;
	MCF_EPORT_EPDDR &= ~mask;
	// Enable interrupts initially if requested.
	if (enable)
		MCF_EPORT_EPIER |= mask;
}

/**
 * EnableExternalInterrupt: Enable external interrupt using the current setup.
 * @param intr Interrupt number, 1...7.
 */
void CPU::EnableExternalInterrupt(int intr)
{
	ASSERT((intr >= intEPFR1) && (intr <= intEPFR7));
	int mask = 1 << intr;
	MCF_EPORT_EPIER |= mask;
}

/**
 * DisableExternalInterrupt: Disable external interrupt.
 * @param intr Interrupt number, 1...7.
 */
void CPU::DisableExternalInterrupt(int intr)
{
	ASSERT((intr >= intEPFR1) && (intr <= intEPFR7));
	int mask = 1 << intr;
	MCF_EPORT_EPIER &= ~mask;
	ClearExternalInterrupt(intr);
}

/**
 * ClearExternalInterrupt: Clear external interrupt triggered flag.
 * @param intr Interrupt number, 1...7.
 */
void CPU::ClearExternalInterrupt(int intr)
{
	ASSERT((intr >= intEPFR1) && (intr <= intEPFR7));
	int mask = 1 << intr;
	MCF_EPORT_EPFR = mask;
}

/**
 * DelayMs: Delay the CPU by specified number of milliseconds. Delay is about
 * 99% accurate. Routine uses busy-loop waiting so it should be used when RTOS
 * is not running.
 * @param delay Delay in milliseconds.
 */
void CPU::DelayMs(int ms)
{
	int loops = ms * __SYSTEM_CLOCK * 261;
	for (volatile int i = 0; i < loops; i++)
	{
	}
}

/**
 * DelayUs: Delay the CPU by specified number of microseconds. Delay is about
 * 95% accurate. Routine uses busy-loop waiting so it should be used when RTOS
 * is not running, or when precise microsecond-granular delays are needed.
 * Routine does not disable interrupts, so to provide more precise timing,
 * interrupts should be disabled by the caller when this routine is used.
 * @param delay Delay in microseconds.
 */
void CPU::DelayUs(int us)
{
	int loops = (us * __SYSTEM_CLOCK) / 4;
	for (volatile int i = 0; i < loops; i++)
	{
	}
}

void CPU::Restart()
{
//	DisableInterrupts();
//	MCF_RCM_RCR = 0x80;
}
