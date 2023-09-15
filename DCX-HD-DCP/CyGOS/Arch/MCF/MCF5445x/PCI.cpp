// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/Arch/MCF/MCF5445x/PCI.cpp_v   1.0   29 Oct 2010 10:38:36   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This object implements PCI support.

#include "PCI.h"
#include "MMU.h"
#include "CpuDefs.h"
#include "stdio.h"
#include "assert.h"

// MCF5445x to PCI memory mappings:
//
//	WIN         MCF5445x                    PCI            TYPE
//	---         --------                    ---            ----
//	[0] 0xA0000000 -> 0xAFFFFFFF  0xA0000000 -> 0xAFFFFFFF  MEM 256M
//	[1] 0xB0000000 -> 0xB7FFFFFF  0xB0000000 -> 0xB7FFFFFF  MEM 128M
//	[2] 0xB8000000 -> 0xBFFFFFFF  0xB8000000 -> 0xBFFFFFFF  IO  128M

#define PCI_MEM_BASE		0xA0000000
#define PCI_MEM_SIZE		0x10000000
#define PCI_IO_BASE			0xB0000000
#define PCI_IO_SIZE			0x08000000
#define PCI_CONFIG_BASE		0xB8000000
#define PCI_CONFIG_SIZE		0x08000000

volatile PCI_Regs * PCI::regs;

enum PciSlot
{
	PciCPU = 16, PciJ14 = 17, PciJ15 = 18, PciJ16 = 19, PciJ17 = 20,
};

extern "C" unsigned int htoil(unsigned int value)
{
	return ((value >> 24) & 0x000000FF) | ((value >> 8) & 0x0000FF00) | ((value
			<< 8) & 0x00FF0000) | ((value << 24) & 0xFF000000);
}

extern "C" unsigned int itohl(unsigned int value)
{
	return htoil(value);
}

extern "C" unsigned int htois(unsigned int value)
{
	return ((value >> 8) & 0x000000FF) | ((value << 8) & 0x0000FF00);
}

extern "C" unsigned int itohs(unsigned int value)
{
	return htois(value);
}

int ReadConfig(int bus, int dev, int fn, int reg, int len)
{
	ASSERT(bus >= 0x00);
	ASSERT(dev >= 0x00);
	ASSERT(reg >= 0x00);
	ASSERT(bus <= 0xFF);
	ASSERT(dev <= 0x1F);
	ASSERT(reg <= 0x3F);

	uint32 addr = MCF_PCI_PCICAR_BUSNUM(bus) | MCF_PCI_PCICAR_DEVNUM(dev)
			| MCF_PCI_PCICAR_FUNCNUM(fn) | MCF_PCI_PCICAR_DWORD(reg)
			| MCF_PCI_PCICAR_E;

	MCF_PCI_PCICAR = addr;
	__asm__ __volatile__("nop");

	uint32 value;
	switch (len)
	{
	case 1:
		value = *(volatile uint8 *) (PCI_CONFIG_BASE + (reg & 3));
		break;
	case 2:
		value = htois(*(volatile uint16 *) (PCI_CONFIG_BASE + (reg & 2)));
		break;
	case 4:
		value = htoil(*(volatile uint32 *) (PCI_CONFIG_BASE));
		break;
	}

	MCF_PCI_PCICAR = ~MCF_PCI_PCICAR_E;
	__asm__ __volatile__("nop");

	return value;
}

void WriteConfig(int bus, int dev, int fn, int reg, int len, uint32 value)
{
	ASSERT(bus >= 0x00);
	ASSERT(dev >= 0x00);
	ASSERT(reg >= 0x00);
	ASSERT(bus <= 0xFF);
	ASSERT(dev <= 0x1F);
	ASSERT(reg <= 0x3F);

	uint32 addr = MCF_PCI_PCICAR_BUSNUM(bus) | MCF_PCI_PCICAR_DEVNUM(dev)
			| MCF_PCI_PCICAR_FUNCNUM(fn) | MCF_PCI_PCICAR_DWORD(reg)
			| MCF_PCI_PCICAR_E;

	MCF_PCI_PCICAR = addr;
	__asm__ __volatile__("nop");

	switch (len)
	{
	case 1:
		*(vuint8 *) (PCI_CONFIG_BASE + (reg & 3)) = value;
		break;
	case 2:
		*(vuint16 *) (PCI_CONFIG_BASE + (reg & 2)) = htois(value);
		break;
	case 4:
		*(vuint32 *) (PCI_CONFIG_BASE) = htoil(value);
		break;
	}

	MCF_PCI_PCICAR = ~MCF_PCI_PCICAR_E;
	__asm__ __volatile__("nop");
}

void PCI::Init()
{
	MMU::Map(0xB8000000);
	regs = (PCI_Regs *) 0xFC0A8000;
	// Arbitration controller setup:
	// INTMPRI		Internal master priority high
	// EXTMPRI		External master priority - all 4 high
	// INTMINTEN	Internal master interrupt enable
	// EXTMINTEN	External master interrupt enable - all 4 high
	MCF_PCI_PACR = MCF_PCI_PACR_INTMPRI | MCF_PCI_PACR_EXTMPRI(0x0F)
			| MCF_PCI_PACR_INTMINTEN | MCF_PCI_PACR_EXTMINTEN(0x0F);
	// PCI pin assignment regs - enable all requests and grants
	MCF_PAD_PAR_PCI = MCF_PAD_PAR_PCI_PAR_GNT0 | MCF_PAD_PAR_PCI_PAR_GNT1
			| MCF_PAD_PAR_PCI_PAR_GNT2 | MCF_PAD_PAR_PCI_PAR_GNT3_GNT3
			| MCF_PAD_PAR_PCI_PAR_REQ0 | MCF_PAD_PAR_PCI_PAR_REQ1
			| MCF_PAD_PAR_PCI_PAR_REQ2 | MCF_PAD_PAR_PCI_PAR_REQ3_REQ3;
	// PCI target control register 1:
	// P			Prefetch reads
	// WCT			Write combine timer, 8 PCI clocks
	MCF_PCI_PCITCR1 = MCF_PCI_PCITCR1_P | MCF_PCI_PCITCR1_WCT(8);
	// PCI memory address, starting at A0000000, 256MB
	MCF_PCI_PCIIW0BTAR = 0xA00FA000;
	// PCI memory address, starting at B0000000, 128MB
	MCF_PCI_PCIIW1BTAR = 0xB007B000;
	// PCI IO address, starting at B8000000, 128MB
	MCF_PCI_PCIIW2BTAR = 0xB807B800;
	// Memory/IO window control
	// Enable all 3 windows
	// First 2 windows - memory, no read line or multiple
	// Last window - I/O
	MCF_PCI_PCIIWCR = MCF_PCI_PCIIWCR_W0C_E | MCF_PCI_PCIIWCR_W0C_MEMREAD
			| MCF_PCI_PCIIWCR_W1C_E | MCF_PCI_PCIIWCR_W1C_MEMREAD
			| MCF_PCI_PCIIWCR_W2C_E | MCF_PCI_PCIIWCR_W2C_IO;
	// PCI initiator control register
	// No retry error enable, no initiator abort enable, no target abort enable
	// PCI retries or master latency timeout - maximum
	MCF_PCI_PCIICR = MCF_PCI_PCIICR_MAXRETRY(0xFF);
	// PCI Command Register
	// MEMORY WRITE AND INVALIDATE command enabled
	// Bus mastering enabled
	// Memory access enabled
	// No I/O access enabled since M68K has no separate I/O space
	MCF_PCI_PCISCR = MCF_PCI_PCISCR_MW | MCF_PCI_PCISCR_B | MCF_PCI_PCISCR_M;
	// Configuration register 1
	// Cache line size 8 DWORDs
	// Latency timer - maximum, hardware ignores lowest 3 bits
	MCF_PCI_PCICR1 = MCF_PCI_PCICR1_CLS(8) | MCF_PCI_PCICR1_LTMR(0xFF);
	// Configuration register 2
	// Nothing special to do
	MCF_PCI_PCICR2 = 0;
	// PCI target control register 2:
	// disable all BAR
	MCF_PCI_PCITCR2 = 0;
	// Global Control and Status Register:
	// clear reset bit
	// don't enable any interrupts for now
	MCF_PCI_PCIGSCR = 0; // MCF_PCI_PCIGSCR_SEE | MCF_PCI_PCIGSCR_PEE;
}

void PCI::PrintDevices()
{
	uint32 id = regs->PCIIDR;
	printf("PCI: Vendor %04X, device %04X\n", id & 0xFFFF, id >> 16);
	id = regs->PCICCRIR;
	printf("PCI: Class %06X, rev %02X\n", id >> 24, id & 0xFF);

	uint32 data;
	data = ReadConfig(0, PciJ14, 0, 0, 4);
	printf("PCI configuration data from J14: Vendor %04X, device %04X\n", data
			& 0xFFFF, data >> 16);
	if (data != 0xFFFFFFFF)
		PrintDeviceHeader(0, PciJ14);
	data = ReadConfig(0, PciJ15, 0, 0, 4);
	printf("PCI configuration data from J15: Vendor %04X, device %04X\n", data
			& 0xFFFF, data >> 16);
	if (data != 0xFFFFFFFF)
		PrintDeviceHeader(0, PciJ15);
	data = ReadConfig(0, PciJ16, 0, 0, 4);
	printf("PCI configuration data from J16: Vendor %04X, device %04X\n", data
			& 0xFFFF, data >> 16);
	if (data != 0xFFFFFFFF)
		PrintDeviceHeader(0, PciJ16);
	data = ReadConfig(0, PciJ17, 0, 0, 4);
	printf("PCI configuration data from J17: Vendor %04X, device %04X\n", data
			& 0xFFFF, data >> 16);
	if (data != 0xFFFFFFFF)
		PrintDeviceHeader(0, PciJ17);
}

void PCI::PrintDeviceHeader(int bus, int device)
{
	uint32 data;
	for (int i = 0; i < 0x40; i++)
	{
		data = ReadConfig(bus, device, 0, i, 4);
		printf("%08X ", data);
		if ((i & 7) == 7)
			printf("\n");
	}
	data = ReadConfig(bus, device, 0, 1, 4);
	uint32 status = data >> 16;
	printf("Status: %04X\n", status);
	printf("\tParity error is%s detected\n", status & 0x8000 ? "" : " not");
	printf("\tSystem error is%s signalled\n", status & 0x4000 ? "" : " not");
	printf("\tMaster abort is%s received\n", status & 0x2000 ? "" : " not");
	printf("\tTarget abort is%s received\n", status & 0x1000 ? "" : " not");
	printf("\tTarget abort is%s signalled\n", status & 0x0800 ? "" : " not");
	char * timing[4] =
	{ "fast", "medium", "slow", "reserved" };
	printf("\tDevice select timing: %s\n", timing[(status >> 9) & 3]);
	printf("\tData parity is%s reported\n", status & 0x0100 ? "" : " not");
	printf("\tFast back-to-back%s capable\n", status & 0x0080 ? "" : " not");
	printf("\tUDF%s supported\n", status & 0x0040 ? "" : " not");
	printf("\t66MHz%s capable\n", status & 0x0020 ? "" : " not");
	data = ReadConfig(bus, device, 0, 2, 4);
	printf("Class and revision: %08X\n", data);
	char * classCode[] =
	{ "Pre-2.0", "Mass storage controller", "Network controller",
			"Display controller", "Multimedia device", "Memory controller",
			"Bridge device", "Simple comm controller",
			"Base system peripheral", "Input device", "Docking station",
			"Processor", "Serial bus controller" };
	printf("\tClass: %02X - %s\n", data >> 24, classCode[data >> 24]);
	printf("\tSubclass: %02X\n", (data >> 16) & 0xFF);
	printf("\tProgramming interface: %02X\n", (data >> 8) & 0xFF);
	printf("\tRevision: %02X\n", data & 0xFF);
	data = ReadConfig(bus, device, 0, 2, 4);
	printf("Header type, etc: %08X\n", data);
	printf("\t%s function device\n", data & 0x00800000 ? "Multi" : "Single");
	printf("\tHeader type: %02X\n", (data >> 16) & 0x7F);
	printf("Address space requirements:\n");
	for (int i = 0; i < 6; i++)
	{
		WriteConfig(bus, device, 0, i + 4, 4, 0xFFFFFFFF);
		data = ReadConfig(bus, device, 0, i + 4, 4);
		if (!data)
		{
			if (i == 0)
				printf("\tNo base address registers implemented\n");
			break;
		}
		printf("\tBase register %d: %08X\n", i, data);
		printf("\t\t%s space\n", data & 1 ? "I/O" : "Memory");
		int mask;
		if (data & 1)
		{
			// I/O
			mask = 0xFFFFFFFC;
		}
		else
		{
			// Memory
			mask = 0xFFFFFFF0;
			printf("\t\t%srefetchable\n", data & 8 ? "P" : "Not p");
			char * type[] =
			{ "Anywhere 32-bit", "Below 1MB", "Anywhere 64-bit", "Reserved" };
			printf("\t\tMemory type: %s\n", type[(data >> 1) & 3]);
		}
		mask &= data;
		mask = ~mask;
		mask++;
		if (mask <= 0x3FF)
			printf("\t\tAddress space size: %d bytes\n", mask);
		else if (mask <= 0xFFFFF)
			printf("\t\tAddress space size: %d K bytes\n", mask >> 10);
		else
			printf("\t\tAddress space size: %d M bytes\n", mask >> 20);
	}
	data = ReadConfig(bus, device, 0, 10, 4);
	printf("CardBus CIS Pointer: %08X\n", data);
	data = ReadConfig(bus, device, 0, 11, 4);
	printf("Subsystem and vendor ID: %08X\n", data);
	data = ReadConfig(bus, device, 0, 12, 4);
	printf("Expansion ROM base address: %08X\n", data);
	data = ReadConfig(bus, device, 0, 15, 4);
	printf("Interrupts etc: %08X\n", data);
	printf("\tMaximum latency: %02X\n", (data >> 24) & 0xFF);
	printf("\tMinimum grant time: %02X\n", (data >> 16) & 0xFF);
	if (((data >> 8) & 0xFF) == 0)
		printf("\tNo interrupts\n");
	else
		printf("\tInterrupt pin: %02X INT%c\n", (data >> 8) & 0xFF,
				((data >> 8) & 0xFF) + '@');
	printf("\tInterrupt line register: IRQ%d\n", (data >> 0) & 0xFF);
}
