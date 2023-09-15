// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/API/trace.h_v   1.0   21 Mar 2012 12:25:54   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares convenience macros to print a variable name and value
// in most frequently used formats. Both polling and regular printf() macros
// are defined.

#ifndef TRACE_H_
#define TRACE_H_

#include "stdio.h"

#define TRACE_C(var)	printf(#var " = %c\n", var)
#define TRACE_D(var)	printf(#var " = %d\n", var)
#define TRACE_S(var)	printf(#var " = %s\n", var)
#define TRACE_8X(var)	printf(#var " = %08X\n", var)
#define TRACE_8LX(var)	printf(#var " = %08lX\n", var)

#define PTRACE_C(var)	pprintf(#var " = %c\n", var)
#define PTRACE_D(var)	pprintf(#var " = %d\n", var)
#define PTRACE_S(var)	pprintf(#var " = %s\n", var)
#define PTRACE_8X(var)	pprintf(#var " = %08X\n", var)
#define PTRACE_8LX(var)	pprintf(#var " = %08lX\n", var)

#define TRACE(var,format)	printf(#var " = %" #format "\n", var)
#define PTRACE(var,format)	pprintf(#var " = %" #format "\n", var)

#endif
