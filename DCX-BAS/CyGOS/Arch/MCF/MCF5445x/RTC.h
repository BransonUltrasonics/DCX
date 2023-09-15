// $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/CyGOS/Arch/MCF/MCF5445x/RTC.h_v   1.0   21 Mar 2012 12:31:34   PDwivedi  $
//
// Copyright (c) Cyber Group, Inc 2008
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
// This file is part of CyGOS-3.
//
// This module defines the interface to Real Time Clock module
//

#include "CPUdefs.h"

#ifndef RTC_H_
#define RTC_H_


class RTC
{
public:
	RTC();
	virtual ~RTC();
	void Init(int level, int priority);
	static RTC * thisPtr;
	void EnableRTC();
	void DisableRTC();
	static uint32 Get_time();
	static void Set_time(int days, int hours, int mins, int seconds);
	int Valid_time(int days, int hours, int mins, int seconds);
protected:
	static void IntHandler(int vector, void * frame);
	void IntHandler();
};


#endif /* RTC_H_ */
