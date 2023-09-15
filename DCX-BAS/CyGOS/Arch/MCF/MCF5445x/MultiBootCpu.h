// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Arch/MCF/MCF5445x/MultiBootCpu.h_v   1.0   21 Mar 2012 12:30:56   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3 MultiBoot loader.
//
// This module declares MultiBootCpu object. The object implements CPU-specific
// parts of MultiBoot loader.

#ifndef MULTIBOOTCPU_H_
#define MULTIBOOTCPU_H_

/**
 * class MultiBootCpu: Object implements MultiBoot CPU-specific code.
 */
class MultiBootCpu
{
public:
	static void Init();
	static void WriteProtectCs();
};

#endif
