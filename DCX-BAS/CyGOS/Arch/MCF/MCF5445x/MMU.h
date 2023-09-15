// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Arch/MCF/MCF5445x/MMU.h_v   1.3   04 Sep 2013 09:21:50   rjamloki  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This object encapsulates CPU-specific MMU initialization.

#ifndef MMU_H_
#define MMU_H_

#include "CpuDefs.h"

struct MMU_Regs {
	uint32 CR;
	uint32 OR;
	uint32 SR;
	uint32 unused0C;
	uint32 AR;
	uint32 TR;
	uint32 DR;
};

class MMU {
public:
	static void Init();
	static void AccessError(void * frame);
	static void Map(uint32 phys);
	static int UnMap(int virt_addr, int itlb);
	static void RemapCodeData(bool writable, bool Bootable);
protected:
	static void Map(int virt_addr, int phys_addr, int itlb, int asid,
			int shared_global, int size_code, int cache_mode, int super_prot,
			int rwx, int locked);
	static uint8 mapped[];
};

#endif
