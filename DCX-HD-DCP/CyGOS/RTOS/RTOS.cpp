// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/RTOS/RTOS.cpp_v   1.0   29 Oct 2010 10:39:44   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements the Real-Time Operating System kernel object. Object
// uses CPU-specific features where appropriate, so this source should be
// common to all supported architectures and CPUs. This module does not implment
// CPU-specific features related to interrupt handling and task context
// switching, this functionality must be implemented on per CPU basis. When
// these features are implemented in CPU-specific source tree, file names for
// this implementation should contain CPU family name and not just be RTOS.asm
// or RTOS.cpp so that the sources are clearly CPU-specific and Eclipse can
// figure out where compile errors are, if any, if all file names are unique to
// the project.

// FIXME Logging code was removed from RTOS. Need to be optional and less coupled.
// iMX27 logging leftovers are still there...

#include "RTOS.h"
#include "CPU.h"
#include "string.h"
#include "DebugNew.h"
#include "Mutex.h"
#include "Event.h"
#include "assert.h"
#include "stdio.h"

/**
 * Idle: Create an idle task.
 */
IdleTask::IdleTask() :
	Task("Idle", 1024)
{
	ctr = 0;
}

/**
 * Run: Run function for Idle task. Function only increments the local
 * counter, having nothing better to do. By design, it must be always
 * runnable, it cannot sleep, suspend or wait for any resource.
 */
void IdleTask::Run()
{
	for (;;)
	{
		ctr++;
	}
}

/**
 * running: <b>true</b> if multitasking is running. All created tasks
 * are initially runnable, but they start only after the <b>Start</b>
 * function is called. If task is created after the multitasker is
 * started it is scheduled for execution immediately.
 * @see #Start
 */
bool RTOS::running;
/**
 * intNesting: Interrupt nesting level. Every time we enter interrupt
 * handler, value gets 1 added. When interrupt handling in C routine
 * is done, 1 is subtracted. The value is initially set to 0 in the
 * constructor. Schedule() function invokes TRAP 15 instruction only
 * when the variable is 0, so that the trap instruction is only used
 * in user task code. Interrupt handlers have their own method of
 * switching tasks that is executed properly even when Schedule() was
 * called and it determined that task switch needs to happen.
 * @deprecated Currently this variable is only used to track if we have
 * interrupt overrun, but this variable could be also used to handle task
 * switch in nested interrupts if needed later.
 */
int RTOS::intNesting;
/**
 * taskCurrent: Pointer to the current running task. Variable is
 * static to provide easy access from assembler modules.
 */
Task * RTOS::taskCurrent;
/**
 * taskHighest: Highest priority task at the moment. Variable is used
 * to determine next running task. Variable is static to provide easy
 * access from assembler modules.
 */
Task * RTOS::taskHighest;
/**
 * taskIdle: Pointer to the idle task. Idle task is running whenever
 * there is nothing else to run.
 */
Task * RTOS::taskIdle;
/**
 * tasks: Task table. Table contains one pointer slot for each
 * priority level, each priority level may have only one task to run.
 * Pointer is NULL if no task exisis at this priority level.
 */
Task * RTOS::tasks[prioTotal];
/**
 * taskPrintf: Pointer to the one and only printf task. The value exists in the
 * task table, but using this pointer C++ compiler knows we are dealing with
 * this particular task, and no type cats sare necessary to go into PrintfTask
 * functions.
 */
PrintfTask * RTOS::taskPrintf;
/**
 * ticking: The highest priority ticking task. Ticking task is defined as a
 * task that has Tick() function overridden. Default RTOS configuration does
 * not have any ticking tasks, so this pointer can be 0. Tasks are inserted
 * into this list in order of their priority.
 */
Task * RTOS::ticking;
/**
 * timestamp: RTOS running time stamp. Variable is updated once every timer
 * tick.
 */
long long RTOS::timestamp;

/**
 * RTOS: Creates and initializes RTOS object. After assigning initial values
 * to internal variables also creates the Idle task.
 */
void RTOS::Init()
{
	running = false;
	taskCurrent = 0;
	taskHighest = 0;
	intNesting = 0;
	ticking = 0;
	memset(tasks, 0, sizeof(tasks));
	taskIdle = new IdleTask;
	Create(taskIdle, prioIdle);
	taskPrintf = new PrintfTask;
	Create(taskPrintf, prioPrintf);
}

/**
 * Start: Start the tasker after at least one task was created. The absolute
 * minimum requirement is to have an idle task that never sleeps, this is
 * taken care of in the RTOS constructor.
 * @param usec RTOS timer time between ticks, in microseconds. 10 is the
 * minimum, below that we have no time outside the slice timer interrupt
 * handler to run user tasks, so the system will look like it crashed, no
 * visible life signs...
 */
void RTOS::Start(int usec)
{
	ASSERT(usec >= 10);
	DisableInterrupts(); // disable interrupts for now
	// Initialize CPU-specific parts of RTOS task switching
	CPU::InstallTaskSwitchRoutine(TaskSwitch);
	// Tick handler stack is quite large because task Tick functions are
	// executing using this stack as well.
	CPU::InstallRtosTimer(usec, TickHandler, 1024);
	Task::usecPerTick = usec;
	timestamp = 0;
	// now run the thing
	if (!running)
	{
		for (int i = 0; i < prioTotal; i++)
		{
			Task * task = tasks[i];
			if (task) // task exists?

			{
				if (task->state == Running) // and can run?

				{
					taskCurrent = task;
					taskHighest = task;
					running = true;
					ExecuteFirstTask();
					// interrupts enabled after this point,
					// execution IP is somewhere else
				}
			}
		}
	}
	CRASH("No task to run");
}

/**
 * Create: Create a task given a task object. Initializes the stack and
 * registers the task in the priority table. Also, if tasker is already
 * running, re-schedule tasks so that if the new task has the highest
 * priority it is run.
 * @param task Task object
 * @param prio Task priority value, must be not used yet.
 */
void RTOS::Create(Task *task, int prio)
{
	ASSERT(task);
	ASSERT(tasks[prio] == 0);

	task->priority = prio;
	task->Initialize(); // initialize dynamic parts from here
	tasks[prio] = task;
	if (task->GetTickAddress() != taskIdle->GetTickAddress())
		InsertIntoTickChain(task);
	Schedule(); // schedule highest task to run
}

/**
 * Schedule: Find next task to run. Task must be in <b>Ready</b> state. It is
 * assumed by design that there always will be one task ready to run -
 * <b>Idle</b> task. If there are no higher-priority tasks to run, Idle task
 * is resumed. As part of this design, Idle task must never sleep, suspend etc.
 * <p>This function can be called from interrupt handler or from user code. If
 * called from interrupt handler, intNesting will be >0 and TRAP 15
 * instruction will not be executed, ForceTaskSwitch will be executed when all
 * nested interrupts are processed.
 * <p>This function verifies if task stack is overwritten and issues error
 * message if so.
 * @see #DoTaskSwitch
 * @see .Idle
 */
void RTOS::Schedule()
{
	if (running)
	{
		CriticalSection s;
		taskHighest = 0;
		for (int i = 0; i < prioTotal; i++)
		{
			Task * task = tasks[i];
			if (task)
			{
				if (task->state == Running)
				{
					taskHighest = task;
					break;
				}
			}
		}
		// don't execute task switch from interrupt handler! If we are executing
		// this code from interrupt handler, we'll get to task switch when we're
		// done.
		if ((intNesting == 0) && (taskHighest != taskCurrent))
		{
			// switch to new task
			CPU::ForceTaskSwitch();
		}
		// We had interrupts disabled before this point. For some reason we
		// need to execute task switch while interrupts are still disabled. If
		// we try this with interrupts enabled, tasker crashes. So we execute
		// task switch with interrupts disabled in here and enable interrupts
		// in this task (current being switched away) right after we switch
		// back to this task. Until then, tasker will run with interrupts
		// enabled because the task we switch to will enable them eventually.
	}
}

/**
 * TickHandler: OS timer tick handler. On each tick task delay counters are
 * updated, and if any higher priority task can be run, internal OS variables
 * for that task are updated. Task switch is done in the interrupt handler
 * later when this function returns.
 */
void RTOS::TickHandler()
{
	timestamp += Task::usecPerTick;
	int prio;
	// timer interrupt first phase - let all tasks know tick passed...
	// Time spent scanning 64 tasks and executing 5 empty Tick() functions -
	// 5.1us. Time executing 64 empty Tick() while stack checking on one word -
	// 13.9us.
	Task * task = ticking;
	while (task)
	{
		task->Tick();
		task = task->nextTick;
	}
	// timer interrupt second phase - update task delay counters and also, while
	// at it, determine the next highest priority task since some task may just
	// wake up in time...
	taskHighest = 0;
	for (prio = 0; prio < prioTotal; prio++)
	{
		Task * task = tasks[prio];
		if (task)
		{
			// First thing first - check for stack overflow here because if
			// something goes wrong we want to crash as early as possible,
			// before the stack error causes other errors and we assert here
			// instead of failing somewhere else seemingly for no reason.
			if (task->stackbegin[0] != 0xDEADBABE)
				CRASH("Stack overflow");
			// if task is waiting for something - let the tick pass
			// Look at task state first because it is less likely that task is
			// delayed, and all non-delayed tasks have delay value 0 so in this
			// order we skip through non-delayed tasks faster. Saves 100ns for
			// all 64 tasks.
			if ((task->state == Delayed) && (task->delay >= 0))
			{
				if (task->delay > 0)
				{
					task->delay--;
					task->sleepUsec -= Task::usecPerTick;
				}
				else
					task->state = Running;
			}
			// First running task we see will be the highest priority running
			// task. Here we look at task state first because we have it in
			// cache. Rearranging to look at taskHighest would slow down by
			// 250ns for all 64 tasks.
			if ((task->state == Running) && (taskHighest == 0))
				taskHighest = task;
		}
	}
}

void RTOS::InsertIntoTickChain(Task *task)
{
	Task * thisTick = ticking;
	Task * prevTick = 0;
	while (thisTick)
	{
		if (thisTick->priority > task->priority)
		{
			break;
		}
		prevTick = thisTick;
		thisTick = thisTick->nextTick;
	}
	task->nextTick = thisTick;
	if (prevTick)
	{
		prevTick->nextTick = task;
	}
	else
	{
		ticking = task;
	}
}

void RTOS::RemoveFromTickChain(Task *task)
{
	Task * thisTick = ticking;
	Task * prevTick = 0;
	while (thisTick)
	{
		if (thisTick == task)
		{
			break;
		}
		prevTick = thisTick;
		thisTick = thisTick->nextTick;
	}
	// If we didn't find the task, we really ha
	if (thisTick == task)
	{
		if (prevTick)
		{
			prevTick->nextTick = task->nextTick;
		}
		else
		{
			ticking = task->nextTick;
		}
	}
}

/**
 * Stop: Stop the operating system. This function disables interrupts and
 * also disables the RTOS task switching mechanism, so printf() will switch
 * to polling serial I/O. If printf() has something in the buffer, the data
 * is flushed using polling I/O at this point.
 */
void RTOS::Stop()
{
	DisableInterrupts();
	running = false;
	if (taskPrintf)
		taskPrintf->FlushOutputBuffer();
}

/**
 * Delete: Delete a task. RTOS may delete a task if there is no longer need to
 * run. This releases the task priority level to run other tasks when needed.
 */
void RTOS::Delete(Task * task)
{
	CriticalSection s;
	for (int prio = 0; prio < prioTotal; prio++)
	{
		if (task == tasks[prio])
		{
			tasks[prio] = 0;
			break;
		}
	}
	if (task->GetTickAddress() != taskIdle->GetTickAddress())
		RemoveFromTickChain(task);
	Schedule();
}

/**
 * Create: Create a task given a task object. Initializes the stack and
 * registers the task in the priority table. Also, if tasker is already
 * running, re-schedule tasks so that is the new task has the highest
 * priority it is run. Function is given a range of priorities where the
 * task can be assigned, so task will use highest available priority within
 * that range.
 * @param task Task object
 * @param prioLow Task low priority range value (higher priority).
 * @param prioHigh Task high priority range value (lower priority).
 * @return int Actual priority assigned to the task, or -1 if no slots were
 * available within specified range.
 */
int RTOS::Create(Task * task, int prioLow, int prioHigh)
{
	CriticalSection s;
	for (int prio = prioLow; prio <= prioHigh; prio++)
	{
		if (tasks[prio] == 0)
		{
			Create(task, prio);
			return prio;
		}
	}
	return -1;
}

/**
 * DelayMs: Delay the current task by specified number of milliseconds.
 * @param delay Delay in milliseconds. If delay is lower than the RTOS
 * timer granularity, at least one tick delay will be used.
 */
void RTOS::DelayMs(int delay)
{
	if (running)
		taskCurrent->DelayMs(delay);
	else
		CPU::DelayMs(delay);
}

/**
 * DelayUs: Delay the current task by specified number of microseconds.
 * @param delay Delay in microseconds. If delay is lower than the RTOS
 * timer granularity, at least one tick delay will be used.
 */
void RTOS::DelayUs(int delay)
{
	if (running)
		taskCurrent->DelayUs(delay);
	else
		CPU::DelayUs(delay);
}

void RTOS::PrintStatus(PrintStatusMode mode)
{
	printf("\n\nRTOS task status:\n");
	for (int prio = 0; prio < prioTotal; prio++)
	{
		Task * task = tasks[prio];
		if (!task)
			continue;
		printf("%2d %20s %-10s ", prio, task->name,
				task->state == Delayed ? "Delayed"
						: task->state == Suspended ? "Suspended" : task->state
								== Running ? "Running" : "Unknown");
		if (mode == PrintTasksWithStack)
			printf("Stack: %08X (%6d) ", task->stack, (task->stack
					- task->stackbegin) * 4);
		if (task->waitMutex)
		{
			printf("Mutex %p %s, owned by %s", task->waitMutex,
					task->waitMutex->GetName(),
					task->waitMutex->GetOwnerTask()->GetName());
		}
		if (task->waitEvent)
		{
			printf("Event %p %s", task->waitEvent, task->waitEvent->GetName());
		}
		if (task->waitFifo)
		{
			printf("Queue %p %s", task->waitFifo, task->waitFifo->GetName());
		}
		printf("\n");
	}
	printf("\n");
}
