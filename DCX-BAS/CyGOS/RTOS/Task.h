// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/RTOS/Task.h_v   1.2   10 Sep 2013 03:56:36   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares Task object. All user tasks must be derived from
// this object. Default task behavior is defined here, most functions should
// not be overridden. The main function to override is Task::Run(). Also, tasks
// can implement their own functions as part of their implementation.

#ifndef TASK_H_
#define TASK_H_

class Mutex;
class Event;
class FifoBase;

/**
 * class TaskBase: The whole idea of this class is to predefine layout of
 * members and Virtual Method Table entries for the <b>Task</b> class. If Task
 * object structure gets modified for any reason, this class should ensure that
 * important pieces of information stay in right places.<p>
 * Object layout and VMT layout are both architecture-specific and CPU-specific,
 * and different compilers will almost always generate different code for all
 * this. So, the few routines that directly depend on this layout are always
 * placed into CPU-specific part of the source tree.
 */
class TaskBase
{
public:
	TaskBase()
	{
		stack = (long *) 0x55AA;
	}
	virtual ~TaskBase();
	virtual void Run();
	virtual void Tick();
protected:
	/**
	 * stack: Current stack pointer for the task. Pointer is used to save
	 * and restore task context. Also, pointer is used to detect stack
	 * overruns.
	 */
	long * stack;
};

/**
 * class Task: The base task class. Basic task capabilities are:
 * <ol>
 * <li>Create a task - allocate stack space, initialize registers, get ready
 * to run.
 * <li>Start a task - run it if it has the highest priority in the system.
 * <li>Delay a task - stop task execution for specified timeout. Any task can
 * delay any other task, including itself.
 * <li>Suspend a task - each task can suspend any other task, including
 * itself. Task will be stopped until it is explicitly resumed by other task.
 * <li>Resume a task - any task may resume any other task. Task to resume will
 * go into <b>running</b> or <b>delayed</b> state, depending on whether the
 * delay was zero or not at the time when task was suspended.
 * </ol>
 */
class Task: public TaskBase
{
	friend class RTOS;
public:
	Task(char * n, int stacksize);
	virtual ~Task();
	void Wakeup();
	void Suspend();
	void Resume();
	int Delay(long ticks);
	/**
	 * DelayMs: Delay the task by specified number of milliseconds.
	 * @param msec Delay in milliseconds. If delay is lower than the RTOS
	 * timer granularity, at least one tick delay will be used.
	 * @return int Time in microseconds how long this task has left to sleep
	 * after it was resumed by Wakeup call.
	 */
	int DelayMs(int msec)
	{
		return Delay(MsToTicks(msec));
	}
	/**
	 * DelayUs: Delay the task by specified number of microseconds.
	 * @param delay Delay in microseconds. If delay is lower than the RTOS
	 * timer granularity, at least one tick delay will be used.
	 * @return int Time in microseconds how long this task has left to sleep
	 * after it was resumed by Wakeup call.
	 */
	int DelayUs(int usec)
	{
		return Delay(UsToTicks(usec));
	}
	/**
	 * GetName: Return the task name string.
	 * @return char * Task name.
	 */
	char * GetName() const
	{
		return name;
	}
	int GetPriority() const
	{
		return priority;
	}
	static int MsToTicks(int ms)
	{
		return UsToTicks(ms * 1000);
	}
	static int UsToTicks(int us)
	{
		int ticks = us / usecPerTick;
		if (ticks == 0)
			ticks = 1;
		return ticks;
	}
	/**
	 * priority: Task priority.
	 */
	int priority;
protected:
	static int usecPerTick;
	/**
	 * name: Task name.
	 */
	char * name;
	/**
	 * state: Task state - running, delayed, suspended, etc.
	 * @see .TaskState TaskState enum
	 */
	int state;

private:
	long GetTickAddress() const;
	long GetRunAddress() const;
	void Initialize();
	static void ReturnFromTask();
	/**
	 * stackbegin: Beginning of the memory area that is allocated to task
	 * stack. This is the pointer to free the task stack memory when task
	 * is deleted.
	 */
	long * stackbegin;
	/**
	 * delay: If non-zero and task is running, delay execution by this
	 * many ticks.
	 */
	long delay;
	/**
	 * suspend: Suspend the task if this field is greater than 0. 0 or below
	 * lets the task run. Suspend() increments this value, Resume() decrements
	 * it. Initial value is 0 (running).
	 */
	int suspend;
	/**
	 * nextTick: Next task in tick chain. RTOS uses this list of pointers to
	 * execute Tick() only on tasks that actually have redefined the function,
	 * to reduce timer tick overhead.
	 */
	Task * nextTick;
	/**
	 * sleepUsec: This time in microseconds was left for task to sleep but the
	 * task was resumed externally before the time expired.
	 */
	int sleepUsec;
public:
	/**
	 * waitMutex: If not null, task is suspended behind a Mutex.
	 */
	Mutex * waitMutex;
	/**
	 * waitEvent: If not null, task is suspended waiting for event.
	 */
	Event * waitEvent;
	/**
	 * waitFifo: If not null, task is suspended behind a FIFO.
	 */
	FifoBase * waitFifo;
	int Tstacksize;
	long * GetStatckBegin(){
		return stackbegin;
	}
	long * GetCurrStackPtr(){
		return stack;
	}
};

#endif
