// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/RTOS/Mutex.cpp_v   1.1   24 Jul 2015 08:44:50   sesharma  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements the mutex object. Mutex is a task synchronization
// object. Mutex can have at most one owner. When mutex is owned, all other
// tasks that attempt to acquire the mutex will be suspended or delayed based
// on timeout specified to Acquire function. When mutex is released by the
// current owner, highest priority waiting task becomes the new owner.

#include "Mutex.h"
#include "FIFO.h"
#include "stdio.h"
#include "string.h"
#include "assert.h"
#include "DebugNew.h"
#include "RTOS.h"

/**
 * Mutex: Initialize the new mutex. Mutex has no owner and no waiting list.
 */
Mutex::Mutex(char * name)
{
	memset(waitingTasks, 0, sizeof(waitingTasks));
	ownerTask = 0;
	mutexName = name;
}

/**
 * ~Mutex: Destroy the mutex.
 * @deprecated This is potentially unsafe because there may be tasks waiting
 * for it with infinite timeout, and they may not be ready to handle the case
 * when they continue running with no mutex allocated. Also, they may attempt
 * to release a mutex that is long gone.
 */
Mutex::~Mutex()
{
	ASSERT(!ownerTask);
	for (int prio = 0; prio < prioTotal; prio++)
	{
		ASSERT(!waitingTasks[prio]);
	}
}

/**
 * Acquire: Acquire a mutex. If mutex is already acquired by some owner, this
 * task will suspend or delay based on timeout. Same task cannot acquire mutex
 * twice.
 * @param task Task that attempts to acquire the mutex.
 * @param timeout Timeout to wait if mutex is already acquired. -1 specifies
 * infinite wait.
 * @return bool True if mutex is acquired within allocated timeout. This
 * function can return false even on infinite timeout, if mutex disappeared
 * while waiting.
 */
bool Mutex::Acquire(Task * task, int timeout)
{
	if (!RTOS::IsRunning())
		return true;
	if (!task)
		task = RTOS::GetCurrentTask();
	CriticalSection s;
	int prio = task->GetPriority();
	ASSERT(waitingTasks[prio] == 0);
	// No owner for this mutex - we can become the owner and nothing else needs
	// to be done.
	if (!ownerTask)
	{
		ownerTask = task;
		return true;
	}
	// Some task owns it but our timeout is 0 - report error and return
	if (timeout == 0)
		return false;
	// We have some owner. Now we need to join a waiting list. TODO: Proprity
	// inversion problem here! Owner may be lower priority than the requesting
	// task, so high priority task will get stuck behind low priority task that
	// may be stuck behind another task of middle priority that just goes on
	// and on...
	waitingTasks[prio] = task;
	// Infinite wait - just suspend the requesting task...
	if (timeout == -1)
	{
		task->waitMutex = this;
		task->Suspend();
		// This code executes in task 'task' context. So here we woke up from
		// this suspend call. The only way this can happen is that some other
		// task released the mutex and we are already the owner. Release method
		// does all this. We are not recorded in the waiting task array either.
		if (task != ownerTask)
		{
			pprintf("Task %p - %s, owner %p - %s\n\n\n", task, task->GetName(),
					ownerTask, ownerTask->GetName());
		}
		ASSERT(task == ownerTask);
		// All we need here is to get out.
		return true;
	}
	task->Delay(timeout);
	// Wait is over, so this task can't be in the waiting list any more.
	waitingTasks[prio] = 0;
	// Timed out or woke up by the code in Mutex::Release. Check if we are the
	// owner because this will immediately tells us if we got the mutex or
	// not...
	return task == ownerTask;
}

/**
 * Release: Release the mutex. This can only be done by the owner task. Other
 * task may also release the mutex by providing (faling) the owner task pointer.
 * Highest priority waiting task will be made active.
 * @param task Task that was the owner.
 */
void Mutex::Release(Task * task)
{
	if (!RTOS::IsRunning())
		return;
	if (!task)
		task = RTOS::GetCurrentTask();
	CriticalSection s;
	if (task != ownerTask)
	{
		pprintf("Task %p - %s, owner %p - %s\n\n\n", task, task->GetName(),
				ownerTask, ownerTask->GetName());
	}
	ASSERT(task == ownerTask);
	ownerTask = 0;
	for (int prio = 0; prio < prioTotal; prio++)
	{
		task = waitingTasks[prio];
		if (task)
		{
			waitingTasks[prio] = 0;
			ownerTask = task;
			ownerTask->waitMutex = 0;
			ownerTask->Wakeup();
			break;
		}
	}
}

bool Mutex::Lock()
{
	return Acquire(RTOS::GetCurrentTask(), -1);
}

void Mutex::Unlock()
{
	Release(RTOS::GetCurrentTask());
}
