// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/CyGOS/RTOS/TaskManager.h_v   1.1   20 Sep 2013 06:57:26   rjamloki  $
//
// Copyright (c) Cyber Group, Inc 2008, 2013
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3 MultiBoot loader.
//
// This is the CyGOS task manager.

#ifndef TASKMANAGER_H_
#define TASKMANAGER_H_

#include "Task.h"
#include "Fifo.h"
#include "Board.h"
#include "CyGOS.h"

#define PROFILER_TIMER_INSTANCE 0

class TaskManager: public Task
{
	struct Info
	{
		/**
		 * Microseconds during this second. Task manager will add the number to
		 * seconds/microseconds every second when it runs.
		 */
		int usecThisSecond;
		int percentLastSecond;
		int secondsTotal;
		int usecondsTotal;
	};
public:
	TaskManager(void (*TaskManCallback)(), int stack);
	static void InitTiming()
	{
		Board::InitProfilerTimer(PROFILER_TIMER_INSTANCE);
	}
	static void StartTiming()
	{
		lastTime = Board::GetProfilerTimerCounter(PROFILER_TIMER_INSTANCE);
	}
	static int GetTaskTime(int priority)
	{
		return taskTimes[priority];
	}
	static int GetTaskTimeHighest(int priority)
	{
		if(priority != prioIdle){
			if(taskTimes[priority] > TaskTimesHighest[priority])
				TaskTimesHighest[priority] = taskTimes[priority];
		}
		else
		{
			if(taskTimes[priority] < TaskTimesHighest[priority])
				TaskTimesHighest[priority] = taskTimes[priority];
		}
		return TaskTimesHighest[priority];
	}
	static int GetIntrTime(int vector)
	{
		return intrTimes[vector];
	}
	static int GetIntrTimeHighest(int vector)
	{
		if(intrTimes[vector] > intrTimesHighest[vector])
			intrTimesHighest[vector] = intrTimes[vector];
		return intrTimesHighest[vector];
	}
	static int GetIntCount(int vector)
	{
		return intCount[vector];
	}
	inline static void UpdateTaskTime(int priority)
	{
		int currentTime = Board::GetProfilerTimerCounter(
				PROFILER_TIMER_INSTANCE);
		taskData[priority].usecThisSecond += currentTime - lastTime;
		lastTime = currentTime;
	}
	inline static void UpdateIntrTime(int vector)
	{
		int currentTime = Board::GetProfilerTimerCounter(
				PROFILER_TIMER_INSTANCE);
		intrData[vector].usecThisSecond += currentTime - lastTime;
		lastTime = currentTime;
		intCount[vector]++;
	}
	inline static int RecalculateTaskTime(int priority)
	{
		Info * info = &taskData[priority];
		int result = info->usecThisSecond;
		info->usecThisSecond = 0;
		info->usecondsTotal += result;
		if (info->usecondsTotal >= 1000000)
		{
			info->usecondsTotal -= 1000000;
			info->secondsTotal++;
		}
		return result;
	}
	inline static int RecalculateIntrTime(int level)
	{
		Info * info = &intrData[level];
		int result = info->usecThisSecond;
		info->usecThisSecond = 0;
		info->usecondsTotal += result;
		if (info->usecondsTotal >= 1000000)
		{
			info->usecondsTotal -= 1000000;
			info->secondsTotal++;
		}
		return result;
	}
protected:
	void Run();
	void Tick();
	int timer;
	Fifo<int> wakeupQueue;
	void (*taskManCallback)();
	static Info taskData[prioTotal];
	static Info intrData[256];
	static int intCount[256];
	static int lastTime;
	static int taskTimes[];
	static int intrTimes[];
	static int intrTimesHighest[];
	static int TaskTimesHighest[];
};

#endif
