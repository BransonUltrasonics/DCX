// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/stdlib/stdlib.h_v   1.0   18 Aug 2014 09:16:50   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares stdlib.h functions so that we will not have to use
// libraries provided by the compiler. Their overhead is unknown and may
// interfere with our system design. ASM generated by the compiler is good
// enough so we don't need to go bit-twiddling with these functions.

#ifndef STDLIB_H_
#define STDLIB_H_

#include "ArchDefs.h"

#define NULL (0)

extern "C"
{
void InitMemoryAllocation(int before, int after, int fill, int test);
void * calloc(size_t num, size_t size);
void * malloc(size_t size);
void * Memrealloc(void * ptr, size_t size, char * file, int line);
void free(void * ptr);

int atoi(const char * string);
int xtoi(const char * hexNumber);
int otoi(const char * hexNumber);

void * MallocAlignedNoCache(size_t size, int align);
void * CallocAlignedNoCache(size_t number, size_t size, int align);
void FreeAlignedNoCache(void * ptr);
void * mallocNoInit(size_t size);
void ShowMemory();
}

#endif
