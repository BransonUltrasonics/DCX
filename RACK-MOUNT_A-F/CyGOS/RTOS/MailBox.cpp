// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/RTOS/MailBox.cpp_v   1.0   09 Jun 2015 09:14:00   tayars  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module defines inter-task communication and synchronization objects.

#include "MailBox.h"
#include "RTOS.h"

MailBox::MailBox(int size) :
	queue(size, "MailBox")
{
}

bool MailBox::TryFetch(void ** ptr)
{
	return queue.Read(0, *ptr, 0);
}

bool MailBox::TryPost(void * ptr)
{
	return queue.Write(0, ptr, 0);
}

bool MailBox::Fetch(void ** ptr, int timeoutMs)
{
	if (timeoutMs == 0)
		timeoutMs = -1;
	int ticks = timeoutMs == -1 ? -1 : Task::MsToTicks(timeoutMs);
	bool result = queue.Read(RTOS::GetCurrentTask(), *ptr, ticks);
	return result;
}

bool MailBox::Post(void * ptr, int timeoutMs)
{
	if (timeoutMs == 0)
		timeoutMs = -1;
	int ticks = timeoutMs == -1 ? -1 : Task::MsToTicks(timeoutMs);
	bool result = queue.Write(RTOS::GetCurrentTask(), ptr, ticks);
	return result;
}
