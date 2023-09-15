// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/RTOS/Task.cpp_v   1.4   04 Sep 2013 13:56:06   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements Task object. All user tasks must be derived from
// this object. Default task behavior is defined here, most functions should
// not be overridden. CPU-specific task functions (CPU context related, etc)
// must be implemented in CPU-specific section of code.

#include "Task.h"
#include "CPU.h"
#include "RTOS.h"
#include "assert.h"
#include "DebugNew.h"

TaskBase::~TaskBase()
{
}

void TaskBase::Run()
{
	CRASH("TaskBase::Run called");
}
/**
 * Tick: Let task know we got a timer tick. This routine should not do much
 * because it is called from RTOS timer interrupt handler.
 */
void TaskBase::Tick()
{
}

/**
 * usecPerTick: Microseconds per one RTOS timer tick. All task timers work from
 * this time base.
 */
int Task::usecPerTick;

/**
 * Task: Create a new task object. Task starts up in initial state
 * <b>Running</b>.
 * @@param n Nice printable name for diagnostics.
 * @@param stacksize Stack size for the new task, in longs. Stack size is
 * adjusted by minimum stack requirement to save CPU registers.
 */
Task::Task(char * n, int stacksize)
{
//	stacksize /= 4;
	stacksize += CPU::GetStackExtra();
	stackbegin = new int32[stacksize];
	for (int i = 0; i < stacksize; i++)
		stackbegin[i] = 0xDEADBABE;
	stack = stackbegin + stacksize; // don't need -1 because of -(SP)
	Tstacksize = stacksize;
	pprintf("\n %s %d %08X", n , Tstacksize, stackbegin);
	// stack addressing
	state = Running;
	suspend = 0;
	delay = 0;
	name = n;
	nextTick = 0;
	waitFifo = 0;
	waitMutex = 0;
	waitEvent = 0;
}

/**
 * ~Task: Destroy the task object. Default implementation deletes the task
 * stack.
 */
Task::~Task()
{
	delete stackbegin;
}

/**
 * Resume: Resume a task. Task state is set to <b>Running</b> or <b>Delayed</b>
 * depending on the delay value. Scheduler is invoked after the task state is
 * changed, so the next highest priority task may be switched to.
 */
void Task::Resume()
{
	ASSERT(this != RTOS::taskIdle);

	CriticalSection s;
	suspend--;
	if (suspend <= 0)
	{
		if (delay > 0)
		{
			state = Delayed;
		}
		else
		{
			state = Running;
		}
	}
	RTOS::Schedule();
}

/**
 * Suspend: Suspend a task. Any task can be suspended, task state is set to
 * <b>Suspended</b>. Scheduler is invoked after the task state is changed,
 * so the next highest priority task may be switched to.
 */
void Task::Suspend()
{
	ASSERT(this != RTOS::taskIdle);

	CriticalSection s;
	suspend++;
	if (suspend > 0)
	{
		state = Suspended;
		sleepUsec = -1;
	}
	RTOS::Schedule();
}

/**
 * Wakeup: Wake up a task. Task state is set to <b>Running</b> and delay is
 * set to 0. Scheduler is invoked after the task state is changed, so the
 * next highest priority task may be switched to.
 */
void Task::Wakeup()
{
	ASSERT(this != RTOS::taskIdle);

	CriticalSection s;
	state = Running;
	delay = 0;
	suspend = 0;
	RTOS::Schedule();
}

/**
 * Delay: Cause a task to sleep specified amount of ticks. If ticks parameter
 * is positive, task state is changed to <b>Delayed</b>.
 * @param ticks Ticks to sleep. 0 will not sleep at all, but task will not wake
 * up from existing sleep condition either.
 * @return int Time in microseconds how long this task has left to sleep after
 * it was resumed by Wakeup call.
 */
int Task::Delay(long ticks)
{
	ASSERT(this != RTOS::taskIdle);

	CriticalSection s;
	sleepUsec = ticks * usecPerTick;
	if (ticks)
	{
		state = Delayed;
		delay = ticks;
	}
	RTOS::Schedule();
	return sleepUsec;
}

/**
 * ReturnFromTask: Function issues a diagnostics message 'Task returned from
 * Run() function' and locks up.
 */
void Task::ReturnFromTask()
{
	CRASH("ReturnFromTask");
}
