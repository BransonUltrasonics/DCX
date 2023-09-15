// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/RTOS/LedTasks.cpp_v   1.0   09 Jun 2015 09:14:00   tayars  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements diagnostics LED Task objects.

#include "LedTasks.h"
#include "FIFO.h"
#include "DebugNew.h"
#include "CyGOS.h"

/**
 * f: This FIFO is used for communication between LED blinking tasks and LED
 * display task. LED blinking tasks send integer values 1 and 2 to this queue.
 * Actual values are ignored, but LED display task updates LED states from
 * static variables 'fast' and 'slow' when task receives any message from this
 * queue.
 */
static Fifo<int> * f = 0;
/**
 * fastLedTask: Fast LED task pointer. Slow LED task must wait until the fast
 * LED task is created so it can start suspending and resuming it.
 */
static TaskFastLed * fastLedTask = 0;
/**
 * slow: Slow LED state, on or off. On value is determined by the parameter that
 * was used to create the slow LED blinking task.
 */
static int slow;
/**
 * fast: Fast LED state, on or off. On value is determined by the parameter that
 * was used to create the fast LED blinking task.
 */
static int fast;

/**
 * TaskSlowLed: Initialize the slow LED task.
 * @param s Slow LED state when LED is turned on.
 */
TaskSlowLed::TaskSlowLed(int s) :
	Task("LED1", 256)
{
	state = s;
}

/**
 * Run: Run the slow LED task. Task waits until communication FIFO and fast LED
 * task are both created, then it starts slow blinking on the LED and suspends
 * and resumes the fast LED blinking task. Blinking times are hard-coded 1
 * second on and 1 second off.
 */
void TaskSlowLed::Run()
{
	while (!f)
		DelayMs(100);
	while (!fastLedTask)
		DelayMs(100);
	slow = 0;
	for (;;)
	{
		slow ^= state;
		f->Write(this, 1, 0);
		fastLedTask->Suspend();
		DelayMs(1000);
		fastLedTask->Resume();
		slow ^= state;
		f->Write(this, 1, 0);
		DelayMs(1000);
	}
}

/**
 * TaskFastLed: Initialize the fast LED task.
 * @param s Fast LED state when LED is turned on.
 */
TaskFastLed::TaskFastLed(int s) :
	Task("LED2", 256)
{
	state = s;
}

/**
 * Run: Run the fast LED blinking task. Task waits until the communication FIFO
 * is created, and then starts blinking the fast LED. Blink times are hard-coded
 * to 100ms on and 100ms off.
 */
void TaskFastLed::Run()
{
	fastLedTask = this;
	while (!f)
		DelayMs(100);
	fast = 0;
	for (;;)
	{
		fast ^= state;
		f->Write(this, 2, 0);
		DelayMs(100);
	}
}

/**
 * TaskDispLed: Create the LED state display task.
 */
TaskDispLed::TaskDispLed(SetLedsProc slp) :
	Task("Display", 1024)
{
	setLedsProc = slp;
}

/**
 * Run: Run the LED state display task. Function creates the communication FIFO
 * and waits for messages being posted. If any LED blinking task posts a message
 * to the FIFO, display task takes current slow and fast LED states and updates
 * the LED hardware.
 */
void TaskDispLed::Run()
{
	f = new Fifo<int> (10, "Display");
	for (;;)
	{
		int value;
		f->Read(this, value, -1);
		setLedsProc(slow, fast);
	}
}
