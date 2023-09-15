// $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/CyGOS/API/CyGOS.cpp_v   1.7   01 Sep 2011 13:07:34   ygupta  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This file implements CyGOS user accessible API.

#include "CyGOS.h"
#include "RTOS.h"
#include "LedTasks.h"
#include "Board.h"
#include "CPU.h"
#include "watchDog.h"

void CyGOS::Init()
{
	RTOS::Init();
}

void CyGOS::Start(int usec)
{
	RTOS::Start(usec);
}
void CyGOS::Stop()
{
	RTOS::Stop();
}

void CyGOS::CreateLedTasks(int slow, int fast, SetLedsProc slp)
{
	RTOS::Create(new TaskFastLed(fast), prioTotal - 5);
	RTOS::Create(new TaskSlowLed(slow), prioTotal - 4);
	RTOS::Create(new TaskDispLed(slp), prioTotal - 3);
}

void CyGOS::PrintStatus(PrintStatusMode mode)
{
	RTOS::PrintStatus(mode);
}

void CyGOS::CreateTask(Task * task, int & priority)
{
	RTOS::Create(task, priority++);
}

void CyGOS::Restart()
{
	//CPU::Restart();
}

void CyGOS::InstallTimer(int usInterval, void(*callback)(), int instance)
{
	Board::InstallTimer(usInterval, callback, instance);
}

void CyGOS::StartTimer(int instance)
{
	Board::StartTimer(instance);
}

void CyGOS::StopTimer(int instance)
{
	Board::StopTimer(instance);
}

void CyGOS::InitWatchDog()
{
	WatchDog::InitWatchDog();
}

void CyGOS::EnableWatchDog()
{
	WatchDog::EnableWatchDog();
}

void CyGOS::KickWatchDog()
{
	WatchDog::kickWatchDog();
}

void CyGOS::DisableWatchDog()
{
	WatchDog::DisableWatchDog();
}


