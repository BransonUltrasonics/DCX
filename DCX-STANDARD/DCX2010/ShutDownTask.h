/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/ShutDownTask.h_v   1.0.2.1   13 Jun 2014 08:46:12   amaurya  $
*/
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      Copyright (c) Branson Ultrasonics Corporation, 1995,96, 2013        */
/*     This program is the property of Branson Ultrasonics Corporation      */
/*   Copying of this software is expressly forbidden, without the prior     */
/*   written consent of Branson Ultrasonics Corporation.                    */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*************************                         **************************/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/ShutDownTask.h_v  $
 * 
 *    Rev 1.0.2.1   13 Jun 2014 08:46:12   amaurya
 * Added AlarmReasons
 * 
 *    Rev 1.0.2.0   09 Sep 2013 08:59:54   rjamloki
 * Added Seek Frequency Offset, Amplitude ramping, set all graph defaults as in WC.
 * 
 *    Rev 1.4   06 May 2013 06:19:04   amaurya
 * Changes to use actual frequency.
 * 
 *    Rev 1.3   27 Aug 2012 08:51:56   rjamloki
 * Clean up after modbus and State machine review
 * 
 *    Rev 1.2   01 Aug 2012 08:17:52   ygupta
 * Code cleanup
 * 
 *    Rev 1.1   18 Jul 2012 14:00:28   ygupta
 * Ver1.0A check in for review
 * 
 *    Rev 1.0   09 May 2012 03:27:56   PDwivedi
 * Initial revision.

******************************************************************************/

#include "Task.h"
#include "portable.h"

#ifndef SHUTDOWNTASK_H_
#define SHUTDOWNTASK_H_

// 24 Volt sensing at IRQ4 pin  and AC supply(15V) sensing at IRQ7using EPORT Pin data register.
//0 means 24 volt lost. 1 means 24 volt is present
#define CHECK_FOR_24V	(MCF_EPORT_EPPDR & MCF_EPORT_EPPDR_EPPD4)
#define CHECK_FOR_AC	(MCF_EPORT_EPPDR & MCF_EPORT_EPPDR_EPPD7)

class ShutDownTask: public Task
{
public:
	ShutDownTask();
	static BOOLEAN RTOSShutDown;
	static SINT32 AlarmReasons;
protected:
	virtual void Run(void);
	void ShutDown(void);
};
void CheckForPowerDown(void);
#endif /* TESTUDPSELECT_H_ */
