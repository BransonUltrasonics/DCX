// $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/MultiBoot/Main.cpp_v   1.0   18 Aug 2014 09:18:26   amaurya  $
//
// Copyright (c) Cyber Group, Inc 2008
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3 MultiBoot loader.
//
// This is the main module for MultiBoot loader.

// FIXME Verify on all platforms.

#include "CyGOS.h"
#include "LED.h"
#include "DebugNew.h"
#include "Board.h"
#include "stdio.h"

void SetLeds(int slow, int fast)
{
	LED::Set(1, slow);
	LED::Set(2, fast);
}

char * buildDate = __DATE__;
char * buildTime = __TIME__;

int main()
{
	printf("\x0C \x0C \x0CMultiBoot loader ver. 2.0\n");
	printf("Build Date: %s %s\n\n", buildDate, buildTime);

	CyGOS::Init();
	CyGOS::CreateLedTasks(1, 1, SetLeds);

	int priority = 20;
	CyGOS::CreateSerialBootTask(priority);
	CyGOS::Start(1000);
	return 0;
}
