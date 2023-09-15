// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/RTOS/Event.h_v   1.1   24 Jul 2015 08:44:50   sesharma  $
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module defines inter-task communication and synchronization objects.

#ifndef EVENT_H_
#define EVENT_H_

class Task;

class Event
{
public:
	Event(char * n);
	~Event();
	void ClearEvent()
	{
		event = false;
	}
	void SetEvent();
	bool Wait(int timeoutMs = -1);
	char * GetName() const
	{
		return name;
	}
protected:
	bool event;
	Task * waiting;
	char * name;
};

#endif
