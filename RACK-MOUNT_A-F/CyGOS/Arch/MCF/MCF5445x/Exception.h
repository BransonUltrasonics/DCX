// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Arch/MCF/MCF5445x/Exception.h_v   1.1   24 Jul 2015 08:58:18   sesharma  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares CPU family-specific exception handling objects and
// definitions.

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include "CpuDefs.h"

extern unsigned int CrashBuffer[4131];
extern char TaskName[24];
extern unsigned int CrashValid;
extern unsigned int CurrStackPtr;
extern unsigned int Stackbegin;
extern unsigned int StackEnd;
extern unsigned int CrashBuffPrintLen;
extern unsigned int MMUAR;
/**
 * ExceptionHandler: A type defined for pointers to functions to handle
 * exceptions and interrupts.
 * @param vector Interrupt vector number that is handled by this function.
 * @param frame Pointer to interrupt stack frame.
 */
typedef void (*ExceptionHandler)(int vector, void * frame);

/**
 * Class Exception: Class implements all functionality required for interrupt
 * and exception handling. All functions and data fields are static, so no
 * instance of this type is required.
 */
class Exception
{
public:
	static void Init();
	static void Set(int vector, ExceptionHandler handler, int stackSize);

	static void SetDefault(int vector, ExceptionHandler handler, int stackSize);
	static void DefaultHandler(int vector, void * frame);
protected:

	static void FirstHandler(void * frame);
	static ExceptionHandler ExceptionHandlerArray[256];

};

#endif
