// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/RTOS/Mutex.h_v   1.0   21 Mar 2012 12:47:56   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares the mutex object. Mutex is a task synchronization
// object. Mutex can have at most one owner. When mutex is owned, all other
// tasks that attempt to acquire the mutex will be suspended or delayed based
// on timeout specified to Acquire function. When mutex is released by the
// current owner, highest priority waiting task becomes the new owner.

#ifndef MUTEX_H_
#define MUTEX_H_

#include "CyGOS.h"

class Task;

class Mutex
{
public:
	Mutex(char * name);
	~Mutex();
	bool Acquire(Task * task, int timeout);
	void Release(Task * task);
	bool Lock();
	void Unlock();
	Task * GetOwnerTask() const
	{
		return ownerTask;
	}
	char * GetName() const
	{
		return mutexName;
	}
protected:
	/**
	 * waitingTasks:
	 */
	Task * waitingTasks[prioTotal];
	/**
	 * ownerTask:
	 */
	Task * ownerTask;
	char * mutexName;
};

#endif
