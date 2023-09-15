/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/ShutDownTask.h_v   1.1   19 Mar 2015 07:06:08   sesharma  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/ShutDownTask.h_v  $
 * 
 *    Rev 1.1   19 Mar 2015 07:06:08   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.0   18 Aug 2014 09:17:28   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:04   amaurya
 * Initial revision.
 */

#include "Task.h"
#include "portable.h"

#ifndef SHUTDOWNTASK_H_
#define SHUTDOWNTASK_H_

// 24 Volt sensing at IRQ4 pin  and AC supply(15V) sensing at IRQ7using EPORT Pin data register.
//0 means 24 volt lost. 1 means 24 volt is present
#define CHECK_FOR_24V	(MCF_EPORT_EPPDR & MCF_EPORT_EPPDR_EPPD4)
#define CHECK_FOR_AC	(MCF_EPORT_EPPDR & MCF_EPORT_EPPDR_EPPD7)

class ShutDownTask: public Task {
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
