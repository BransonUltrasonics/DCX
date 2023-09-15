// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/RTOS/RTOS.h_v   1.0   29 Oct 2010 10:39:44   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares the Real-Time Operating System kernel object.

#ifndef RTOS_H_
#define RTOS_H_

#include "Task.h"
#include "PrintfTask.h"
#include "CyGOS.h"

/**
 * enum TaskState: Task states.
 * TODO There is still some confision between tasks suspended and delayed, but
 * current implementation works fairly well provided that task cannot be both
 * suspended and delayed at same time. Both these conditions may only happen at
 * same time if one task delays this task and another puts to sleep, which can
 * be avoided by design. If this is ever resolved, Wakeup() function in the
 * task may have to be tweaked.
 */
enum TaskState
{
	/**
	 * Delayed: Task is delayed for some amount of ticks.
	 */
	Delayed,
	/**
	 * Suspended: Suspend() function was used to temporarily stop the task.
	 */
	Suspended,
	/**
	 * Running: Task is running or waiting for it's turn to run.
	 */
	Running,
};

/**
 * class Idle: Idle task implementation. This task must always run otherwise
 * CPU will run out of things to do and bad things will happen.
 */
class IdleTask: public Task
{
public:
	IdleTask();
	void ResetCounter()
	{
		ctr = 0;
	}
	long GetCounter()
	{
		return ctr;
	}

protected:
	void Run();
	/**
	 * ctr: A counter that increments at full CPU speed, it's just something
	 * for the CPU to do in this task. Also, variable can be used for statistics
	 * on how much time CPU is spending in all other tasks compared to idling.
	 */
	volatile long ctr;
};

/**
 * class RTOS: The kernel of the real-time operating system. RTOS uses unique
 * priorities for all tasks and always schedules the highest priority task that
 * can run. Tasks are stored in a fixed size table determined by the Priority
 * enumeration. The only design fault in this implementation is that if tasks
 * use timer ticks to perform periodic functions, more tasks will add more
 * overhead to RTOS timer tick handling even if most tasks do not use timer tick
 * functionality. The overhead is constant and proportional to number of tasks.
 * TODO New class CyGOS should be designed that has more of a general-purpose
 * task scheduler implemented, with less unique priority values and more tasks
 * using same priorities. External interface should be roughly same so that
 * existing code will not require much tweaking.
 */
class RTOS
{
	friend class Task;
public:
	static void Init();
	static void Create(Task * task, int prio);
	static int Create(Task * task, int prioLow, int prioHigh);
	static void Delete(Task * task);
	static void Start(int usec);
	static void Schedule();
	static void Stop();
	static void PrintStatus(PrintStatusMode mode);
	/**
	 * IsRunning: Determine whether the RTOS is running.
	 * @return bool True if RTOS was started.
	 */
	static bool IsRunning()
	{
		return running;
	}
	/**
	 * Print: Function is called from Terminal::Out to send a character to
	 * serial port, if RTOS is running. Function calls PrintfTask::Print(ch)
	 * function to buffer the character.
	 * @param ch Character to print.
	 */
	static void Print(char ch)
	{
		taskPrintf->Print(ch);
	}
	/**
	 * Print: Function is called at the end of terminal output when the whole
	 * string is buffered. Function calls PrintfTask::Print() function to send
	 * the whole buffered string to serial port in one transfer.
	 */
	static void Print()
	{
		taskPrintf->Resume();
	}
	/**
	 * IntNesting: Return the interrupt nesting count.
	 * @return int The interrupt nesting count value. If 0, function was called
	 * from user code, if 1, function was called from an interrupt handler.
	 * Values greater than 1 should not be returned by current design where
	 * nested interrupts are not allowed.
	 */
	static int IntNesting()
	{
		return intNesting;
	}
	/**
	 * IntNesting: Change interrupt nesting by some value. Value should be +1
	 * or -1 by current design.
	 * @param delta Value to add to the interrupt nesting counter.
	 */
	static void IntNesting(int delta)
	{
		intNesting += delta;
	}
	/**
	 * GetTask: Get a task pointer from the task table.
	 * @param prio Task priority value.
	 * @return Task * Task pointer. If priority is out of range, current task
	 * pointer is returned.
	 */
	static Task * GetTask(int prio)
	{
		if ((prio >= 0) && (prio < prioTotal))
		{
			return tasks[prio];
		}
		return taskCurrent;
	}
	/**
	 * GetCurrentTask: Return currently executing task pointer.
	 * @return Task * Currently executing task pointer.
	 */
	static Task * GetCurrentTask()
	{
		return taskCurrent;
	}
	static void DelayMs(int delay);
	static void DelayUs(int delay);
	static long long GetTimestamp()
	{
		// need atomic access to variable that takes two transfers to CPU...
		CriticalSection s;
		return timestamp;
	}
	static bool WriteLog(char * msg, int msgLen, bool crash = true);
	static bool ClearLogData();
	static void DisplayLog();
	static void GetLogIntoBuff(char * buff, int startIndx, int endIndx);
protected:
	static void TickHandler();
	static void TaskSwitch();
	static void ExecuteFirstTask();
	static void RemoveFromTickChain(Task * task);
	static void InsertIntoTickChain(Task * task);

	static int intNesting;
	static bool running;
	static RTOS * thisPtr;
	static Task * taskIdle;
	static Task * taskCurrent;
	static Task * taskHighest;
	static Task * tasks[prioTotal];
	static Task * ticking;
	static PrintfTask * taskPrintf;
	static long long timestamp;
};

#endif
