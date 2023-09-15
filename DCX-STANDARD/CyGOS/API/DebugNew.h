// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/API/DebugNew.h_v   1.0   29 Oct 2010 10:38:24   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares debugging versions of operator new. This header is
// optional. If not included, memory allocation will still work, but diagnostics
// information about what file allocated the block at what line will not be
// available.

#ifndef DEBUGNEW_H_
#define DEBUGNEW_H_

#include "stdlib.h"

// CYGOS_USE_DEBUGNEW is defined in Makefile for CyGOS and application (two
// places). The big reason for this is to eliminate debugging code in final
// project releases by commenting out a line in Makefile instead of chasing
// all sources for this file being included.
#ifdef CYGOS_USE_DEBUGNEW

void * operator new(size_t size, char * file, int line);
void * operator new[](size_t size, char * file, int line);

#define DEBUG_NEW new(__FILE__, __LINE__)
#define new DEBUG_NEW

#endif

#endif
