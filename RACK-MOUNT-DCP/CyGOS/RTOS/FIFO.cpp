// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/RTOS/FIFO.cpp_v   1.0   18 Aug 2014 09:16:42   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module defines a FIFO buffer of arbitrary data types (integers,
// characters, object pointers etc). FIFOs are used for one-way task-to-task
// communication.

#include "FIFO.h"
#include "RTOS.h"
#include "string.h"
#include "stdio.h"
#include "assert.h"

/**
 * FifoBase: Constructor, allocates memory for the data buffer and
 * initializes other fields.
 * @param size Fifo size in objects.
 * TODO parameters in this constructor are not used at this time, only common
 * code is carried over, but nothing FIFO type-specific.
 */
FifoBase::FifoBase(int itemcount, int itemsize, const char * name)
{
	maxCount = itemcount;
	count = 0;
	read = 0;
	write = 0;
	waiting = 0;
	fifoName = name;
	itemsize = 0;
}

bool FifoBase::WriteTest(Task * writer, int timeout)
{
	// all different possibilities of buffer full
	if (count == maxCount)
	{
		// no delay - return immediately
		if (timeout == 0)
			return false;
		// delay specified - try to wait it out
		// also - must have a task...
		if (writer == 0)
			writer = RTOS::GetCurrentTask();
		ASSERT(writer != 0);
		// at this time we should not have any task waiting for anything to
		// happen to this queue: reader should not wait because queue is full,
		// and we should have no other writers waiting, there can be only one
		if (waiting)
		{
			printf("\n\nWaiting: %s, this %s, queue %s\n\n\n",
					waiting->GetName(), writer->GetName(), fifoName);
			// For now just delay a second and hope previous task is done
			// with waiting
			writer->DelayMs(1000);
			ASSERT(waiting == 0);
		}
		waiting = writer;
		// infinite wait - suspend and wait till we wake up if some other task
		// reads from the queue
		if (timeout == -1)
		{
			writer->waitFifo = this;
			writer->Suspend();
			// after wakeup clear the waiting pointer
			waiting = 0;
		}
		else
		{
			// delay the task for specified time
			writer->Delay(timeout);
			// after wakeup or timeout clear the waiting pointer
			waiting = 0;
			// when we wake up, it's either time expired or some other task made
			// some space available in the queue, so we need to check if space
			// is available
			if (count == maxCount)
				return false;
		}
	}
	return true;
}

void FifoBase::WriteCleanup()
{
	Task * task = 0;
	write++;
	if (write >= maxCount)
		write = 0;
	count++;
	if (waiting)
	{
		task = waiting;
		waiting = 0;
	}
	if (task)
	{
		task->waitFifo = 0;
		task->Wakeup();
	}
}

bool FifoBase::ReadTest(Task * reader, int timeout)
{
	if (count == 0)
	{
		// nothing to read and timeout 0 - fail immediately
		if (timeout == 0)
			return false;
		// at this state we should not have a task waiting for anything: write
		// task has enough space to write something into the queue, and we
		// should not have other tasks attempting to read from this queue
		ASSERT(waiting == 0);
		if (reader == 0)
			reader = RTOS::GetCurrentTask();
		waiting = reader;
		if (timeout == -1)
		{
			// wait indefinitely
			reader->waitFifo = this;
			reader->Suspend();
			// after wakeup clear the waiting pointer
			waiting = 0;
		}
		else
		{
			// wait till timer expires
			reader->Delay(timeout);
			// after wakeup or timeout clear the waiting pointer
			waiting = 0;
			// timeout expired, still nothing?
			if (count == 0)
				return false;
		}
	}
	return true;
}

void FifoBase::ReadCleanup()
{
	Task * task = 0;
	read++;
	if (read >= maxCount)
		read = 0;
	count--;
	// We read something from the queue. Check if any writer was waiting for
	// this space to become available.
	if (waiting)
	{
		task = waiting;
		waiting = 0;
	}
	if (task)
	{
		task->waitFifo = 0;
		task->Wakeup();
	}
}
