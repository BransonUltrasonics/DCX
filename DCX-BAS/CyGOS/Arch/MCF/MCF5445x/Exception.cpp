// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Arch/MCF/MCF5445x/Exception.cpp_v   1.12   08 Oct 2013 06:08:00   rjamloki  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This file implements interrupt routine management. All interrupt vectors
// jump into one handler in Exception.asm. From there CPU transfers into C++
// routines installed for each vector, through a parallel table of interrupt
// handlers. Default interrupt handler provides diagnostic output of what went
// wrong, because all interrupts should have handlers installed if interrupts
// are used.

#include "Exception.h"
#include "CPUdefs.h"
#include "RTOS.h"
#include "TaskManager.h"
#include "MMU.h"
#include "assert.h"
#include "stdio.h"
#include "DebugNew.h"
#include "CyGOS.h"
#include "CPU.h"
//#include "D:\DCP Ver1.01B_15July_Raman\DCX2010\LOG.h"
//#include "..\..\..\..\Application\Log.h"
/**
 * Exception handler SP values (top of stack).
 */
//uint8 * Exception::exceptionHandlerSP[256];

/**
 * Exception handler stacks as allocated (bottom of stack).
 */
//uint8 * Exception::exceptionHandlerStack[256];

/**
 * Interrupted task SP values as saved by exception handlers (top of stack).
 */
//uint8 * Exception::exceptionTaskStack[256];

/**
 * Default stack for interrupt handlers (exception processing, etc). Any
 * interrupt or exception handler with less than or equal to default stack
 * size is using this stack.
 */
//uint8 * Exception::defaultStack;

/**
 * ExceptionArray: C exception handler pointer table. The real table
 * pointed to by <b>VBR</b> contains pointers to a single assembly routine
 * that saves enough CPU context to call a C function, this table contains
 * specific handlers for all used interrupts. Table is filled in
 * <b>Exception::Init</b> function to point to function
 * <b>Exception::DefaultHandler</b> for all interrupts and exceptions.
 */
ExceptionHandler Exception::ExceptionHandlerArray[256];
/** Exception interrupt level, -1 if none */
//int Exception::vector = -1;

/**
 * Init: Initialize exception handler table. Default handler is installed for
 * all vectors at this moment, later any device driver can set it's own pointer
 * to a function that provides interrupt handling.
 */
void Exception::Init()
{
/*	memset(exceptionHandlerStack, 0, sizeof(exceptionHandlerStack));
	memset(exceptionTaskStack, 0, sizeof(exceptionTaskStack));
	memset(exceptionHandlerSP, 0, sizeof(exceptionHandlerSP));*/
	for (int i = 0; i < 256; i++)
		ExceptionHandlerArray[i] = DefaultHandler;
/*	defaultStack = new uint8[DefaultStackSize];
	for (int i = 0; i < 256; i++)
	{
		exceptionHandlerStack[i] = defaultStack;
		exceptionHandlerSP[i] = defaultStack + DefaultStackSize;
	}*/
}

/**
 * FirstHandler: This handler is called from assembly code for all
 * interrupts and exceptions. This function dispatches into appropriate
 * interrupt handler with the pointer in the ExceptionHandlerArray. By default
 * all exceptions are handled by DefaultHandler function.
 * @param frame Pointer to the exception stack frame. Pointer is used to
 * process 'return from main()' error and to provide some diagnostics for
 * invalid stack frame formats, this may happen only if stack is not
 * properly pushed/popped.
 */
//void UpdateITPVal(int PointIndex);
extern bool FramCsDone;
int LastInterruptLevel = 0;
void Exception::FirstHandler(void * frame)
{
//	int prio;
	static int oldLevel = -1;

	if (RTOS::IntNesting() != 0)
	{
		pprintf("\n\n\n");
		pprintf("Old interrupt level %d\n", oldLevel);
		pprintf("New interrupt level %d\n", MCF5XXX_RD_SF_VECTOR(frame));
		pprintf("\n\n\n");
		if(FramCsDone)
		  CyGOS::WriteSystemEvent(FAULT_ERROR, (short)oldLevel , MCF5XXX_RD_SF_VECTOR(frame), 1, true);
		if(MCF5XXX_RD_SF_VECTOR(frame) == 2){
			DefaultHandler(2, frame);
			return;
		}
		else
			ASSERT(0);
	}
	RTOS::IntNesting(+1);
	oldLevel = MCF5XXX_RD_SF_VECTOR(frame);

	int i = MCF5XXX_RD_SF_VECTOR(frame);
	if(i != IntVector(intWatchdogTimer))
		LastInterruptLevel = i;
//	UpdateITPVal(i);
	// early handling for 'return from main()'
	if (i == 0)
	{
		pprintf("\r\nReturn from main()\r\n");
	}
	else
	{
		switch (MCF5XXX_RD_SF_FORMAT(frame))
		{
		case 4:
		case 5:
		case 6:
		case 7:
			break;
		default:
			pprintf("\nIllegal stack type! -- PC = %#08X\n",
					MCF5XXX_SF_PC(frame));
		}
//	int currTime = MCF_DTIM_DTCN (1);// + TEST_OFFSET;
//	if((currTime - Dtim_counter) < 0)
//	 CyGOS::WriteSystemEvent(ASSERT_ERROR, __LINE__, (('E' << 24) | ('x' << 16) | ('c' << 8) | ('e')), (('p' << 24) | ('t' << 16) | ('i' << 8) | ('o')), true);//ASSERT(0);
//	prio = RTOS::GetCurrentTaskPriority();
	//prio = priorityInSchedular;TODO:

//    taskInfo[prio].timeSpent +=  (currTime - Dtim_counter);
//	  taskInfo[prio].microseconds += (currTime - Dtim_counter);
//    taskInfo[prio].seconds += (taskInfo[prio].microseconds / 1000000);
//    taskInfo[prio].microseconds= taskInfo[prio].microseconds % 1000000;
	TaskManager::UpdateTaskTime(RTOS::GetCurrentTask()->GetPriority());
	ExceptionHandlerArray[i](i, frame);
	TaskManager::UpdateIntrTime(i);
//	 Dtim_counter = MCF_DTIM_DTCN (1);// + TEST_OFFSET;
//	 if((Dtim_counter - currTime) < 0)
//		 CyGOS::WriteSystemEvent(ASSERT_ERROR, __LINE__, (('E' << 24) | ('x' << 16) | ('c' << 8) | ('e')), (('p' << 24) | ('t' << 16) | ('i' << 8) | ('o')), true);//ASSERT(0);
//	 interruptInfo[i].timeSpent +=  (Dtim_counter - currTime);
//	 interruptInfo[i].microseconds += (Dtim_counter - currTime);
//	 interruptInfo[i].seconds += (interruptInfo[i].microseconds / 1000000);
//	 interruptInfo[i].microseconds = interruptInfo[i].microseconds % 1000000;
	}
//	UpdateITPVal(i);
	oldLevel = -1;
	RTOS::IntNesting(-1);
}

/**
 * Set: Set up C interrupt handler for specified vector.
 * @param vector Interrupt vector number.
 * @param handler Function to handle the interrupt.
 */
void Exception::Set(int vector, ExceptionHandler handler, int stackSize)
{
	ASSERT(ExceptionHandlerArray[vector] == DefaultHandler);
	ExceptionHandlerArray[vector] = handler;
	stackSize = 0;//Avoid compiler warning (unused variable);
	/*if (stackSize > DefaultStackSize)
	{
		exceptionHandlerStack[vector] = new uint8[stackSize];
		exceptionHandlerSP[vector] = exceptionHandlerStack[vector] + stackSize;
	}*/
}

/**
 * DefaultHandler: This function is a default handler for all exceptions that
 * don't have their own handler installed. Function only provides diagnostics
 * message on the terminal and hangs up. All interrupts should be processed
 * using their specific handlers instead of this generic one.
 */
extern unsigned int CrashBuffer[4131];
extern char TaskName[24];
extern unsigned int CrashValid;
extern unsigned int CurrStackPtr;
extern unsigned int Stackbegin;
extern unsigned int StackEnd;
extern unsigned int CrashBuffPrintLen;
extern unsigned int MMUAR;
Task * ExTaskPtr;
long * ExStkBegin;
long * ExStkPtr;
int ExStkSize;
int ExIndx;
void Exception::DefaultHandler(int vector, void * frame)
{
	//CyGOS::DisableWatchDog();
	CyGOS::KickWatchDog();
	ExTaskPtr = RTOS::GetCurrentTask();
	if(ExTaskPtr){//do nothig if cygos is not running
		ExStkBegin = ExTaskPtr->GetStatckBegin();
		ExStkPtr = ExTaskPtr->GetCurrStackPtr();
		ExStkSize = ExTaskPtr->Tstacksize;
		CrashValid = 0xAA22EF55;
		strcpy(TaskName, ExTaskPtr->GetName());
		CurrStackPtr = (unsigned int)ExStkPtr;
		Stackbegin = (unsigned int)(ExStkBegin + ExTaskPtr->Tstacksize);
		StackEnd = 	(unsigned int)ExStkBegin;
		CyGOS::KickWatchDog();
	//	pprintf("\n i = %d\n", i);
		if(ExTaskPtr->Tstacksize <= 4131){
			CrashBuffPrintLen = ExTaskPtr->Tstacksize;
			for(int j =0;j < ExTaskPtr->Tstacksize; j++){
				CrashBuffer[j] = ExStkBegin[j];
				CyGOS::KickWatchDog();
			}
		}
		else{
			CrashBuffPrintLen = 4131;
			ExIndx = ExStkPtr - ExStkBegin;
			pprintf("\n ExIndx %d", ExIndx);
			if((ExTaskPtr->Tstacksize  - ExIndx) < 4131)
				ExIndx = ExTaskPtr->Tstacksize - 4131;
			pprintf("\n ExIndx %d", ExIndx);
			for(int j =0;((ExIndx < ExTaskPtr->Tstacksize) && (j < 4131)); ExIndx++, j++){
				CrashBuffer[j] = ExStkBegin[ExIndx];
				CyGOS::KickWatchDog();
			}
		}
//		pprintf("\n Current Task %s  curr stack ptr %08X stack begin %08X stack size %d init stack %08X\n\n",
//				ExTaskPtr->GetName(), ExStkPtr, ExStkBegin, ExTaskPtr->Tstacksize, ExStkBegin + ExStkSize);
		CyGOS::KickWatchDog();
//		for(int i = 0; i < ExTaskPtr->Tstacksize; i++){
//			pprintf("%08X ", ExStkBegin[i]);
//			CyGOS::KickWatchDog();
//		}
		if(FramCsDone)
			MMUAR = 0;
	}
	CyGOS::ApplicationCrashCallBack();
	//CyGOS::EnableWatchDog();
	switch (vector)
	{
	case 2:
		pprintf("Access Error -- PC = %#08X\n", MCF5XXX_SF_PC(frame));
		if(FramCsDone){
			CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)MCF5XXX_RD_SF_FS(frame) , (unsigned short)MCF5XXX_SF_PC(frame), vector, true);
			CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)(MCF5XXX_SF_PC(frame) >> 16) , (unsigned short)MCF5XXX_SF_PC(frame), vector, true);
		}
		switch (MCF5XXX_RD_SF_FS(frame))
		{
		case 4:
			pprintf("Error on instruction fetch\n");
			break;
		case 8:
			pprintf("Error on operand write\n");
			break;
		case 12:
			pprintf("Error on operand read\n");
			break;
		case 9:
		case 5: // Execute opword fetch fault
		case 6: // Execute extension word(s) fetch fault
		case 10: // data write error
		case 14: // data read error
			MMU::AccessError(frame);
			return;
		default:
			pprintf("Reserved Fault Status Encoding\n");
			break;
		}
		break;
	case 3:
		pprintf("Address Error -- PC = %#08X\n", MCF5XXX_SF_PC(frame));
		CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)MCF5XXX_RD_SF_FS(frame) , (unsigned short)MCF5XXX_SF_PC(frame), vector, true);
		CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)(MCF5XXX_SF_PC(frame) >> 16) , (unsigned short)MCF5XXX_SF_PC(frame), vector, true);


		switch (MCF5XXX_RD_SF_FS(frame))
		{
		case 4:
			pprintf("Error on instruction fetch\n");
			break;
		case 8:
			pprintf("Error on operand write\n");
			break;
		case 9:
			pprintf("Attempted write to write-protected space\n");
			break;
		case 12:
			pprintf("Error on operand read\n");
			break;
		default:
			pprintf("Reserved Fault Status Encoding\n");
			break;
		}
		break;
	case 4:
		CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)(MCF5XXX_SF_PC(frame) >> 16) , (unsigned short)MCF5XXX_SF_PC(frame), vector, true);

		pprintf("Illegal instruction -- PC = %#08X\n", MCF5XXX_SF_PC(frame));
		{
			unsigned short * pc = (unsigned short *) MCF5XXX_SF_PC(frame);
			for (int i = 0; i < 8; i++)
				pprintf("%04X ", pc[i]);
			pprintf("\n");
		}
		break;
	case 5:
		CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)(MCF5XXX_SF_PC(frame) >> 16) , (unsigned short)MCF5XXX_SF_PC(frame), vector, true);
		pprintf("Divide by 0 -- PC = %#08X\n", MCF5XXX_SF_PC(frame));
		break;
	case 8:
		CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)(MCF5XXX_SF_PC(frame) >> 16) , (unsigned short)MCF5XXX_SF_PC(frame), vector, true);
		pprintf("Privilege violation -- PC = %#08X\n", MCF5XXX_SF_PC(frame));
		break;
	case 9:
		CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)(MCF5XXX_SF_PC(frame) >> 16) , (unsigned short)MCF5XXX_SF_PC(frame), vector, true);
		pprintf("Trace Exception -- PC = %#08X\n", MCF5XXX_SF_PC(frame));
		break;
	case 10:
		CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)(MCF5XXX_SF_PC(frame) >> 16) , (unsigned short)MCF5XXX_SF_PC(frame), vector, true);
		pprintf("Unimplemented A-Line Instruction -- PC = %#08X\n",
				MCF5XXX_SF_PC(frame));
		break;
	case 11:
		CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)(MCF5XXX_SF_PC(frame) >> 16) , (unsigned short)MCF5XXX_SF_PC(frame), vector, true);
		pprintf("Unimplemented F-Line Instruction -- PC = %#08X\n",
				MCF5XXX_SF_PC(frame));
		break;
	case 12:
		CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)(MCF5XXX_SF_PC(frame) >> 16) , (unsigned short)MCF5XXX_SF_PC(frame), vector, true);
		pprintf("Debug Interrupt -- PC = %#08X\n", MCF5XXX_SF_PC(frame));
		break;
	case 14:
		CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)(MCF5XXX_SF_PC(frame) >> 16) , (unsigned short)MCF5XXX_SF_PC(frame), vector, true);
		pprintf("Format Error -- PC = %#08X\n", MCF5XXX_SF_PC(frame));
		break;
	case 15:
		CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)MCF5XXX_RD_SF_FORMAT(frame) , (unsigned short)MCF5XXX_RD_SF_FORMAT(frame), vector, true);

		CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)MCF5XXX_RD_SF_FS(frame) , (unsigned short)MCF5XXX_SF_PC(frame), vector, true);
		CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)(MCF5XXX_SF_PC(frame) >> 16) , (unsigned short)MCF5XXX_SF_PC(frame), vector, true);
		pprintf("Unitialized Interrupt -- PC = %#08X\n", MCF5XXX_SF_PC(frame));
		break;
	case 24:
		CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)(MCF5XXX_SF_PC(frame) >> 16) , (unsigned short)MCF5XXX_SF_PC(frame), vector, true);
		pprintf("Spurious Interrupt -- PC = %#08X\n", MCF5XXX_SF_PC(frame));
		return;
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
		CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)(MCF5XXX_RD_SF_VECTOR(frame) - 24) , 0, vector, true);
		pprintf("Autovector interrupt level %d\n", MCF5XXX_RD_SF_VECTOR(frame)
				- 24);
		break;
	case 32:
	case 33:
	case 34:
	case 35:
	case 36:
	case 37:
	case 38:
	case 39:
	case 40:
	case 41:
	case 42:
	case 43:
	case 44:
	case 45:
	case 46:
	case 47:
		CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)(MCF5XXX_RD_SF_VECTOR(frame) - 32) , 0, vector, true);
		pprintf("TRAP #%d\n", MCF5XXX_RD_SF_VECTOR(frame) - 32);
	break;
	case 6:
	case 7:
	case 13:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 48:
	case 49:
	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
	case 56:
	case 57:
	case 58:
	case 59:
	case 60:
	case 61:
	case 62:
	case 63:
		CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)(MCF5XXX_RD_SF_VECTOR(frame)) , 0, vector, true);
		pprintf("Reserved: #%d\n", MCF5XXX_RD_SF_VECTOR(frame));
	break;
	case 65: // Eport Interrupt 1
	case 66: // Eport Interrupt 2
	case 67: // Eport Interrupt 3
	case 68: // Eport Interrupt 4
	case 69: // Eport Interrupt 5
	case 70: // Eport Interrupt 6
	case 71: // Eport Interrupt 7
		// Clear the interrupt source
		// This clears the flag for edge triggered interrupts
		MCF_EPORT_EPFR = (uint8) (0x01 << (MCF5XXX_RD_SF_VECTOR(frame) - 64));
		CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)(MCF_EPORT_EPFR) , 0, vector, true);
		pprintf("Edge Port Interrupt #%d\n", MCF5XXX_RD_SF_VECTOR(frame) - 64);
	break;
	default:
		CyGOS::WriteSystemEvent(FAULT_ERROR, (unsigned short)(MCF5XXX_RD_SF_VECTOR(frame)) , 0, vector, true);
		pprintf("User Defined Vector #%d\n", MCF5XXX_RD_SF_VECTOR(frame));
	break;
	}
	pprintf("Current task %d %s\n", RTOS::GetCurrentTask()->GetPriority(),
			RTOS::GetCurrentTask()->GetName());
	pprintf("Interrupt %d\n", vector);
	// Since we got here, let's just sit and do nothing.
	CRASH("Default interrupt handler");
}
