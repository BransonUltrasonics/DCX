// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/Arch/MCF/MCF5445x/MCF5445xTask.cpp_v   1.0   18 Aug 2014 09:15:34   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This module implements CPU-specific routines for the Task object.

#include "Task.h"
#include "DebugNew.h"

/**
 * GetRunAddress: Get the address of Run() function for this task. Run() address
 * is needed for the initial stack frame so that each task can start executing
 * the Run() function when RTOS starts. This method assumes that layout of the
 * Task object in memory is fixed and determined by the <b>TaskBase</b> class.
 * Current implementation assumes that first dword of TaskBase is the VMT
 * pointer, and Run() is the first entry in the VMT, which is true for G++ and
 * current class structure.
 * @return long Run() function address for the task.
 */
long Task::GetRunAddress() const
{
	long ** lptr = (long **) this;
	return lptr[0][2];
}

/**
 * GetTickAddress: Get the address of Tick() function for this task. Tick()
 * address is needed when task is created, if the function address is not same
 * as idle task tick address, the function is overridden in the user-defined
 * task and the task with this function needs to be placed into RTOS tick chain.
 * This method assumes that layout of the Task object in memory is fixed and
 * determined by the <b>TaskBase</b> class. Current implementation assumes that
 * first dword of TaskBase is the VMT pointer, and Tick() is the second entry
 * in the VMT right after Run(), which is true for G++ and current class
 * structure.
 * @return long Tick() function address for the task.
 */
long Task::GetTickAddress() const
{
	long ** lptr = (long **) this;
	return lptr[0][3];
}

/**
 * Initialize: Initialize new task structure. Routine creates task frame with
 * following contents:
 * <ol>
 * <li><b>this</b> pointer - points at the task object
 * <li>return address - ReturnFromTask, issues a diagnostic message
 * <li>task Run() function pointer - used by <b>rte</b> instruction to start
 * or resume execution from where it left off
 * <li>flags - a combination of Supervisor Mode, All Interrupts Enabled and
 * cause of interrupt 'Trap 15' - will also be used by <b>rte</b> instruction
 * <li>registers D0-D7 and A0-A6 - initially filled with values D0-D7 and
 * A0-A6, values are not critical and are predefined only for debugging
 * </ol>
 * TODO EMAC support is missing even if this CPU has it.
 */
void Task::Initialize()
{
	long * stk = stack;

	*--stk = (long) this; // simulate this->Run()
	*--stk = (long) ReturnFromTask;
	// now create exception frame
	*--stk = GetRunAddress();
	// return here when done
	*--stk = 0x40002000 | ((32 + 15) << 18);
	// using these flags (16-bit)
	// ints OK, supervisor
	*--stk = 0xA6; // create register values
	*--stk = 0xA5; // create register values
	*--stk = 0xA4; // create register values
	*--stk = 0xA3; // create register values
	*--stk = 0xA2; // create register values
	*--stk = 0xA1; // create register values
	*--stk = 0xA0; // create register values
	*--stk = 0xD7; // create register values
	*--stk = 0xD6; // create register values
	*--stk = 0xD5; // create register values
	*--stk = 0xD4; // create register values
	*--stk = 0xD3; // create register values
	*--stk = 0xD2; // create register values
	*--stk = 0xD1; // create register values
	*--stk = 0xD0; // create register values
	stack = stk;
}
