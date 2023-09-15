// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/API/CyGOS.cpp_v   1.1   24 Jul 2015 09:01:48   sesharma  $
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
TaskIntLoadCallBack LoadLogCallBack = 0;
AppCrashCallBack AppCrashCallBackFun = 0;
WatchDogCallBack WatchDogCallbackFun = 0;
bool CyGOS::EnableDebugFlag = true;
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

void CyGOS::DelayMs(int ms)
{
	RTOS::DelayMs(ms);
}

const char * CyGOS::GetTaskName(int priority) {
	if ((priority < 0) || (priority >= prioTotal))
		return "Bad priority";
	return RTOS::tasks[priority]->GetName();
}

void CyGOS::Restart()
{
	CPU::Restart();
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

void CyGOS::RemoveTimer(int instance)
{
	Board::RemoveTimer(instance);
}

void CyGOS::InitWatchDog(int Cwcr_CWT)
{
	WatchDog::InitWatchDog(Cwcr_CWT);
}

void CyGOS::EnableWatchDog(bool CanDisable)
{
	WatchDog::EnableWatchDog(CanDisable);
}

void CyGOS::KickWatchDog()
{
	WatchDog::kickWatchDog();
}

void CyGOS::DisableWatchDog()
{
	WatchDog::DisableWatchDog();
}


void CyGOS::EnableDebug()
{
	EnableDebugFlag = true;
}

void CyGOS::RegisterTaskLoadLogCallback(TaskIntLoadCallBack callback)
{
	LoadLogCallBack = callback;
}
void CyGOS::LogTaskLoad()
{
	if(LoadLogCallBack)
		LoadLogCallBack(true);
}

void CyGOS::DisableDebug()
{
	EnableDebugFlag = false;
}
void CyGOS::RegisterAppCrashCallback(AppCrashCallBack callback)
{
	AppCrashCallBackFun = callback;
}

void CyGOS::ApplicationCrashCallBack()
{
	if(AppCrashCallBackFun)
		AppCrashCallBackFun();
}

void CyGOS::RegisterWatchdogCallback(WatchDogCallBack WdCb)
{
	WatchDogCallbackFun = WdCb;
}

void CyGOS::WatchDogCallback()
{
	if(WatchDogCallbackFun)
		WatchDogCallbackFun();
}
