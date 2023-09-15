// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/RTOS/TaskManager.cpp_v   1.0   18 Aug 2014 09:16:46   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2008, 2013
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3 MultiBoot loader.
//
// This is the CyGOS task manager.

#include "TaskManager.h"
#include "ArchDefs.h"
#include "CyGOS.h"
#include "RTOS.h"
#include "string.h"

TaskManager::Info TaskManager::taskData[prioTotal];
TaskManager::Info TaskManager::intrData[256];
int TaskManager::intCount[256];
int TaskManager::lastTime;
int TaskManager::taskTimes[prioTotal];
int TaskManager::TaskTimesHighest[prioTotal];
int TaskManager::intrTimes[256];
int TaskManager::intrTimesHighest[256];

void CyGOS::CreateTaskManager(int & priority, int stack,
		void (*TaskManCallback)())
{
	RTOS::Create(new TaskManager(TaskManCallback, stack), priority++);
}

int CyGOS::GetTaskTime(int priority)
{
	return TaskManager::GetTaskTime(priority);
}

int CyGOS::GetTaskTimeHighest(int priority)
{

	return TaskManager::GetTaskTimeHighest(priority);
}

int CyGOS::GetIntrTime(int priority)
{
	return TaskManager::GetIntrTime(priority);
}

int CyGOS::GetIntrTimeHighest(int priority)
{
	return TaskManager::GetIntrTimeHighest(priority);
}
int CyGOS::GetIntCount(int priority)
{
	return TaskManager::GetIntCount(priority);
}

TaskManager::TaskManager(void (*TaskManCallback)(), int stack) :
		Task("TaskMan", stack), wakeupQueue(10, "TaskMan")
{
	taskManCallback = TaskManCallback;
	timer = -1;
}

void TaskManager::Run()
{
	memset(taskTimes, 0, sizeof(taskTimes));
	memset(TaskTimesHighest, 0, sizeof(TaskTimesHighest));
	memset(intrTimes, 0, sizeof(intrTimes));
	memset(intrTimesHighest, 0, sizeof(intrTimesHighest));
	memset(taskData, 0, sizeof(taskData));
	memset(intrData, 0, sizeof(intrData));
	memset(intCount, 0, sizeof(intCount));
	TaskTimesHighest[prioIdle] = 1000000;//1Sec in microsec
	timer = 0;
	for (;;)
	{
		int integer;
		wakeupQueue.Read(0, integer, -1);
		SR sr = EnterCritical();
		for (int i = 0; i < prioTotal; i++)
		{
			taskTimes[i] = RecalculateTaskTime(i);
		}
		for (int i = 0; i < 256; i++)
		{
			intrTimes[i] = RecalculateIntrTime(i);
		}
		ExitCritical(sr);
		taskManCallback();
	}
}

void TaskManager::Tick()
{
	if (timer >= 0)
	{
		timer += usecPerTick;
		if (timer >= 1000000)
		{
			timer -= 1000000;
			wakeupQueue.Write(0, 0, 0);
		}
	}
}
