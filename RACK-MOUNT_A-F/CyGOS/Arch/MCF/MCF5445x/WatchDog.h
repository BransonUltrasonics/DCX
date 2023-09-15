// $Header:
//
// Copyright (c) Cyber Group, Inc 2007
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
//
// This file is part of CyGOS-3.
//
// This module implements watchdog.

#ifndef WATCHDOG_H_
#define WATCHDOG_H_

class WatchDog
{
	protected:
    static void IntHandler(int level, void * frame);
    public:
	WatchDog();
	virtual ~WatchDog();
	static void InitWatchDog(int Cwcr_CWT);
	static void EnableWatchDog(bool CanDisable);
	static void DisableWatchDog();
	static void kickWatchDog();
};

#endif /* WATCHDOG_H_ */
