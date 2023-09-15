// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/RTOS/Event.cpp_v   1.0   09 Jun 2015 09:14:00   tayars  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module defines inter-task communication and synchronization objects.

#include "Event.h"
#include "RTOS.h"
#include "assert.h"

Event::Event(char * n)
{
	event = false;
	waiting = 0;
	name = n;
}

Event::~Event()
{
	ASSERT(!waiting);
}

void Event::SetEvent()
{
	CriticalSection s;
	event = true;
	if (waiting)
	{
		Task * task = waiting;
		waiting = 0;
		task->waitEvent = 0;
		event = false;
		task->Wakeup();
	}
}

bool Event::Wait(int timeoutMs)
{
	int ticks = timeoutMs == -1 ? -1 : Task::MsToTicks(timeoutMs);
	CriticalSection s;
	if ((timeoutMs == 0) || event)
	{
		bool result = event;
		event = false;
		return result;
	}
	ASSERT(!waiting);
	Task * task = RTOS::GetCurrentTask();
	waiting = task;
	if (ticks == -1)
	{
		task->waitEvent = this;
		task->Suspend();
		// If we wake up here, we got the event pulsed from somewhere...
		// However, the event is already reset so that we need to set it again
		// if we need another wakeup call.
		// All we need here is to get out.
		event = false;
		return true;
	}
	task->Delay(ticks);
	// Not waiting any more...
	waiting = 0;
	// Did we get the event triggered or we just timed out?
	bool result = event;
	event = false;
	return result;
}
