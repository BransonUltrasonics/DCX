// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Arch/MCF/MCF5445x/MMU.cpp_v   1.0   18 Aug 2014 09:15:36   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This object encapsulates CPU-specific MMU initialization.

#include "MMU.h"
#include "assert.h"
#include "trace.h"
#include "Task.h"
#include "CYGOS.h"

extern char __MMU_BAR[];

/**
 * mapped: Array of bits that are set for each 16-MByte region when it is mapped
 * into processor memory space. The tracking is necessary so that we don't
 * overload MMU page tables with repeated entries when peripheral chip selects
 * are configured within 16M block but their sizes are smaller, so multiple
 * devices can use their chip selects within one 16M mapping region.
 */
uint8 MMU::mapped[256 / 8];

enum Bits {
	DR_SZ16M = 0x00000300,
	DR_IC = 0x00000000,
	DR_DCB = 0x00000040,
	DR_DCNP = 0x00000080,
	DR_R = 0x00000010,
	DR_W = 0x00000008,
	DR_X = 0x00000004,
	DR_LK = 0x00000002,
	OR_STLB = 0x00000100,
	OR_CA = 0x00000080,
	OR_CNL = 0x00000040,
	OR_ITLB = 0x00000010,
	OR_DTLB = 0x00000000,
	OR_ADR = 0x00000008,
	OR_READ = 0x00000004,
	OR_ACC = 0x00000002,
	OR_UAA = 0x00000001,
	TR_SG = 0x00000002,
	TR_V = 0x00000001,
	SR_HIT = 0x00000002,
	CR_EN = 0x00000001,
};

void MMU::Map(int virt_addr, int phys_addr, int itlb, int asid,
		int shared_global, int size_code, int cache_mode, int super_prot,
		int rwx, int locked) {
	volatile MMU_Regs * regs = (MMU_Regs *) __MMU_BAR;
	// Set up search of TLB.
	regs->AR = virt_addr + 1;
	// Search.
	regs->OR = OR_STLB + OR_ADR + itlb;
	// Set up tag value.
	regs->TR = virt_addr + asid + shared_global + TR_V;
	// Set up data value.
	regs->DR = phys_addr + size_code + cache_mode + super_prot + rwx + locked;
	// Save it.
	regs->OR = OR_ACC + OR_UAA + itlb;
}

void MMU::Map(uint32 phys) {
	int region = phys >> 24;
	int byte = region >> 3;
	int bit = region & 7;
	if ((mapped[byte] & (1 << bit)) == 0) {
		Map(phys, phys, OR_DTLB, 0, TR_SG, DR_SZ16M, DR_DCNP, 0, DR_R + DR_W,
				DR_LK);
		mapped[byte] |= 1 << bit;
	}
}

static inline void WriteMMUBAR(volatile void * value) {
	char * ptr = (char *) value;
	ptr++;
	asm volatile ("movec %0,%%mmubar": : "d"(ptr));
}

static inline void WriteASID(int asid) {
	asm volatile ("movec %0,%%asid": : "d"(asid));
}

/**
 * MMU initialization routine. MMU is mapped so that memory for code is always
 * at cacheable SDRAM area, and data is initially mapped into the same range.
 * Both mappings are locked. Code should not need unlocked mapping tables
 * because there are 32 table entries and we only need 16 16MB regions each.
 * Data mapping is initially into cacheable area and locked, everything else
 * will be mapped dynamically as needed. Also, we'll initially map interrupt
 * vectors area not locked, and peripherals area non-cached and locked.
 */
void MMU::Init() {
	volatile MMU_Regs * regs = (MMU_Regs *) __MMU_BAR;
	WriteMMUBAR(regs);
	regs->OR = OR_CA;
	regs->OR = OR_CA + OR_ITLB;
	WriteASID(0);
	// First, code mapping. All mapped entries are locked because we only have
	// 16 entries for RAM code and 1 entry for ROM code. This leaves 15 more
	// entries for anything we could think of.
	int phys = 0x40000000;
	int virt = 0x50000000;
	for (int i = 0; i < 8; i++) {
		if (i == 0) {
			Map(virt, phys, OR_ITLB, 0, TR_SG, DR_SZ16M, DR_IC, 0, DR_R + DR_X,
					DR_LK);
			Map(virt, phys, OR_DTLB, 0, TR_SG, DR_SZ16M, DR_DCB, 0, DR_R + DR_W,
					0);
		} else {
			Map(virt, phys, OR_DTLB, 0, TR_SG, DR_SZ16M, DR_DCB, 0, DR_R + DR_W,
					DR_LK);
		}
		virt += 0x01000000;
		phys += 0x01000000;
	}
	// Grant access to flash in it's place, instructions cached, data not cached
	Map(0, 0, OR_ITLB, 0, TR_SG, DR_SZ16M, DR_IC, 0, DR_R + DR_W + DR_X, DR_LK);
	Map(0, 0, OR_DTLB, 0, TR_SG, DR_SZ16M, DR_DCNP, 0, DR_R + DR_W + DR_X, DR_LK);
	// Last but not least - internal peripherals, data only, locked, not cached
	phys = 0xFC000000;
	for (int i = 0; i < 4; i++) {
		Map(phys, phys, OR_DTLB, 0, TR_SG, DR_SZ16M, DR_DCNP, 0, DR_R + DR_W,
				DR_LK);
		phys += 0x01000000;
	}
	// Interrupt vector table for flash to RAM copying, locked because of
	// unmapping
	Map(0x40000000, 0x40000000, OR_DTLB, 0, TR_SG, DR_SZ16M, DR_DCNP, 0,
			DR_R + DR_W, DR_LK);
	regs->CR = CR_EN;
	asm("nop");
}

/**
 * This function is executed when CPU executes access error exception and FS
 * field shows the error was related to MMU TLB miss.
 */
void MMU::AccessError(void * frame) {
	bool write = false;
	bool data = false;
	bool extension = false;
	volatile MMU_Regs * regs = (MMU_Regs *) __MMU_BAR;
	unsigned int mmuar = regs->AR;
	CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)(mmuar >> 16) , (unsigned short)mmuar, 0, true);

	PTRACE_8X(mmuar);
	pprintf("\n\n");

	switch (MCF5XXX_RD_SF_FS(frame)) {
	case 9:
		CRASH("Attempted write to write-protected space");
	case 5:
		CRASH("Instruction access fault");
		break;
	case 6:
		CRASH("Extension word access fault");
		extension = true;
		break;
	case 10:
		if((mmuar >= 0x41000000) && (mmuar <= 0x4FFFFFFF)){
			pprintf("\n maapping in data write fault due to addr %08X", mmuar);
			Map((mmuar & 0x4f000000), (mmuar & 0x4f000000), OR_DTLB, 0, TR_SG, DR_SZ16M, DR_DCNP, 0,
						DR_R + DR_W, 0);
			asm("nop");
			return;
		}
		CRASH("Data write access fault");
		data = true;
		write = true;
		break;
	case 14:
		if((mmuar >= 0x41000000) && (mmuar <= 0x4FFFFFFF)){
			pprintf("\n maapping in data read fault due to addr %08X", mmuar);
			Map((mmuar & 0x4f000000), (mmuar & 0x4f000000), OR_DTLB, 0, TR_SG, DR_SZ16M, DR_DCNP, 0,
						DR_R + DR_W, 0);
			asm("nop");
			return;
		}
		CRASH("Data read access fault");
		data = true;
		break;
	default:
		CRASH("AccessError");
	}
}

int MMU::UnMap(int virt_addr, int itlb) {
	volatile MMU_Regs * regs = (MMU_Regs *) __MMU_BAR;
	regs->AR = virt_addr + 1;
	regs->OR = OR_STLB + OR_ADR + itlb;
	int sr = 0;
	if (regs->SR & SR_HIT) {
		regs->OR = OR_READ + OR_ACC + itlb;
		sr = regs->SR;
		regs->TR = 0;
		regs->DR = 0;
		regs->OR = OR_ACC + OR_UAA + itlb;
	}
	regs->OR = OR_CNL;
	return sr;
}

/**
 * Remap code area as data that can be written or can be read-only. Initially
 * it is mapped read-write in the boot loader, so that code can be copied into
 * RAM. Then in boot loader Board::Init code is mapped read-only. When boot
 * loader loads the app, it is stored into the flash. Since SDRAM is already
 * initialized, MMU is not initialized separately, their initialization is
 * executed from same code. CPU::Init goes directly into MemoryInit for the app.
 * So, this code is executed again by the boot loader right before the flash
 * app is executed, to make code area in RAM is read-write again. Shared code
 * in MemoryInit will make code area read-only again after new code is copied
 * over.
 */
//void MMU::RemapCodeData(bool writable) {
//	int phys = 0x40000000;
//	int virt = 0x50000000;
//	UnMap(virt, OR_DTLB);
//	asm("nop");
//	Map(virt, phys, OR_DTLB, 0, TR_SG, DR_SZ16M, DR_DCB, 0, DR_R +
//			(writable ? DR_W : 0), 0);
//	asm("nop");
//}
void MMU::RemapCodeData(bool writable,bool Bootable) {
	 int phys = 0x40000000;
	 int virt = 0x50000000;
	 UnMap(virt, OR_DTLB);
	 asm("nop");
	 Map(virt, phys, OR_DTLB, 0, TR_SG, DR_SZ16M, DR_DCB, 0, DR_R +
	   (writable ? DR_W : 0),(Bootable ? 0 : DR_LK));
	 Map(0x41000000, 0x41000000, OR_DTLB, 0, TR_SG, DR_SZ16M, DR_DCNP, 0,
		DR_R + DR_W, 0);
	 asm("nop");
}
