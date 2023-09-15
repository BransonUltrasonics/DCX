/*                                                                          */
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
 * $Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/ShutDownTask.h_v  $
 * 
 *    Rev 1.0   09 Jun 2015 09:10:50   tayars
 * Initial revision.
 * 
 *    Rev 1.1   24 Aug 2013 17:14:10   amaurya
 * Code review and tracker issue fixes.
 *
 *    Rev 1.3   17 Jul 2013 06:21:10   amaurya
 * Fixed WeldTime Issue on History page and added diagnostic code.
 *
 *    Rev 1.2   31 May 2013 10:37:18   amaurya
 * Level II testing fixes.
 *
 *    Rev 1.1   22 Jan 2013 10:38:32   ygupta
 * Profibus Bug Fixes
 *
 * ShutDownTask.h
 *
 *  Created on: Nov 21, 2012
 *      Author: abhishek.maurya
 */

#ifndef SHUTDOWNTASK_H_
#define SHUTDOWNTASK_H_

#include "Task.h"
#include "portable.h"

class ShutDownTask: public Task
{
public:
   ShutDownTask();
   static BOOLEAN RTOSShutDown;
protected:
   virtual void Run(void);
   void ShutDown(void);
};
void CheckForPowerandLCDSensor(void);
#endif /* SHUTDOWNTASK_H_ */
