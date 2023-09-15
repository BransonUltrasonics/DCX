// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/stdlib/assert.h_v   1.0   21 Mar 2012 12:49:04   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares ASSERT() macro. By default the macro checks the
// condition, and if false, crashes with the message. NDEBUG must be explicitly
// defined if checking macros must not generate any test code.

#ifndef ASSERT_H_
#define ASSERT_H_

void AssertFailed(char * expr, char * file, int line) __attribute__((noreturn));
void SystemCrash(char * expr, char * file, int line) __attribute__((noreturn));

#ifndef NDEBUG

#define ASSERT(expr) \
   if (!(expr)) AssertFailed(#expr, __FILE__, __LINE__)

#define CRASH(text) \
	SystemCrash(#text, __FILE__, __LINE__)

#else
#define ASSERT(expr)
#define CRASH(expr)
#endif

#endif
