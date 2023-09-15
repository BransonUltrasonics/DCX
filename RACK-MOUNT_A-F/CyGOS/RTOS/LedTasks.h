// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/RTOS/LedTasks.h_v   1.0   09 Jun 2015 09:14:00   tayars  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module declares diagnostics LED Task objects.

#ifndef LEDTASKS_H_
#define LEDTASKS_H_

#include "Task.h"
#include "CyGOS.h"

/**
 * class TaskFastLed: Fast blinking LED task.
 */
class TaskFastLed: public Task
{
public:
	TaskFastLed(int s);
protected:
	void Run();
	/**
	 * state: Fast LED state when LED is turned on.
	 */
	int state;
};

/**
 * class TaskSlowLed: Slow blinking LED task.
 */
class TaskSlowLed: public Task
{
public:
	TaskSlowLed(int s);
protected:
	void Run();
	/**
	 * state: Slow LED state when LED is turned on.
	 */
	int state;
};

/**
 * class TaskDispLed: LED hardware update task.
 */
class TaskDispLed: public Task
{
public:
	TaskDispLed(SetLedsProc slp);
protected:
	void Run();
	/**
	 * setLedsProc: Set slow and fast LED hardware state. Function is provided
	 * by the user to change actual LED states depending on board and
	 * application.
	 */
	SetLedsProc setLedsProc;
};

#endif
