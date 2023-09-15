// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Arch/MCF/MCF5445x/MultiBootCpu.cpp_v   1.0   09 Jun 2015 09:13:30   tayars  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3 MultiBoot loader.
//
// This module implements MultiBootCpu object. The object implements CPU-
// specific parts of MultiBoot loader.

#include "MultiBootCpu.h"
#include "CpuDefs.h"

/**
 * Init: Execute CPU-specific initialization for the MultiBoot. For MCF5329
 * processors this function disables the watchdog.
 */
void MultiBootCpu::Init()
{
	MCF_SCM_CWCR = 0;
}

void MultiBootCpu::WriteProtectCs()
{
	MCF_FBCS0_CSCR = MCF_FBCS0_CSCR | MCF_FBCS_CSMR_WP;
}
