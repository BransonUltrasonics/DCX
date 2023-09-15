// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/CyGOS/API/CyGOS.cpp_v   1.0   18 Aug 2014 09:15:22   amaurya  $
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
const char * CyGOS::GetTaskName(int priority)
{
	if ((priority < 0) || (priority >= prioTotal))
		return "Bad priority";
	return RTOS::tasks[priority]->GetName();
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


/*  void CyGOS::GetTaskLoadInformation(TaskInformation * info)
 *
 *   Purpose:
 *		This function copies the time spent information of each task in *the pointer passed in info parameter. Application code has to make *sure to create memory space for the information of 64 tasks before *passing the pointer. Application code can check the name field of *structure
 *		To check the whether the information at some index of array is valid *or not.
 *
 *   Entry condition:
 *		info - The pointer to an array of TasInformation objects.
 *
 * 	 Exit Condition
 * 		None.
 */
//void CyGOS::GetTaskLoadInformation(TaskInformation * info)
//{
////	for(int i = 0; i < 64; i++)
////	info[i].timeSpent = taskInfo[i].timeSpent;
//}


/*   void CyGOS::GetInterruptLoadInformation(TaskInformation * info)
 *
 *   Purpose:
 *		This function copies the time spent information of each interrupt in the pointer passed in info parameter. Application code has to make *sure to create memory space for the information of 255 interrupts *before passing the pointer. Application code can check the name field *of structure
 *		To check the whether the information at some index of array is valid or not.
 *
 *   Entry condition:
 *		info- The pointer to an array of TaskInformation objects.
 *
 * 	 Exit Condition
 * 		None.
 */
//void CyGOS::GetInterruptLoadInformation(TaskInformation * info)
//{
////	for(int i = 0; i < 255; i++)
////	info[i].timeSpent = interruptInfo[i].timeSpent;
//}

/*   void CyGOS::ResetTimeSpentForTasks()
 *
 *   Purpose:
 *		This function resets the timeSpent field of all tasks to zero.
 *
 *   Entry condition:
 *   	None.
 *
 *   Exit condition:
 *    	None.
 */
void CyGOS::ResetTimeSpentForTasks()
{
//	for(int i = 0; i < 64; i++)
//		taskInfo[i].timeSpent = 0;
}

/*  void CyGOS::ResetTimeSpentForInterrupts()
 *
 *   Purpose:
 *		This function resets the timeSpent field of all interrupts to zero.
 *
 *   Entry condition:
 *   	None.
 *
 *   Exit condition:
 *      None.
 */
void CyGOS::ResetTimeSpentForInterrupts()
{
//	for(int i = 0; i < 255; i++)
//		interruptInfo [i].timeSpent = 0;

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

void CyGOS::RegisterAppCrashCallback(AppCrashCallBack callback)
{
	AppCrashCallBackFun = callback;
}

void CyGOS::ApplicationCrashCallBack()
{
	if(AppCrashCallBackFun)
		AppCrashCallBackFun();
}
