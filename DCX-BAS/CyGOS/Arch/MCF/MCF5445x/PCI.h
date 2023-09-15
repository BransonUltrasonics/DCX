// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Arch/MCF/MCF5445x/PCI.h_v   1.0   21 Mar 2012 12:31:08   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This object implements PCI support.

#ifndef PCI_H_
#define PCI_H_

#include "ArchDefs.h"

struct PCI_Regs
{
public:
	uint32 PCIIDR;
	uint32 PCISCR;
	uint32 PCICCRIR;
	uint32 PCICR1;
	uint32 PCIBAR0;
	uint32 PCIBAR1;
	uint32 PCIBAR2;
	uint32 PCIBAR3;
	uint32 PCIBAR4;
	uint32 PCIBAR5;
	uint32 PCICCPR;
	uint32 PCISID;
	uint32 PCIERBAR;
	uint32 PCICPR;
	uint32 PCICPR2;
private:
	uint32 unused40[8];
public:
	uint32 PCIGSCR;
	uint32 PCITBATR0_alias;
	uint32 PCITBATR1_alias;
	uint32 PCITCR1;
	uint32 PCIIW0BTAR;
	uint32 PCIIW1BTAR;
	uint32 PCIIW2BTAR;
private:
	uint32 unused7C;
public:
	uint32 PCIIWCR;
	uint32 PCIIICR;
	uint32 PCIIISR;
	uint32 PCITCR2;
	uint32 PCITBATR0;
	uint32 PCITBATR1;
	uint32 PCITBATR2;
	uint32 PCITBATR3;
	uint32 PCITBATR4;
	uint32 PCITBATR5;
	uint32 PCIINTR;
private:
	uint32 unusedAC[];
public:
	uint32 PCICAR;
private:
	uint32 unusedFC;
};

class PCI
{
public:
	static void Init();
	static void PrintDevices();
protected:
	static void PrintDeviceHeader(int bus, int device);
	static volatile PCI_Regs * regs;
};

#endif
