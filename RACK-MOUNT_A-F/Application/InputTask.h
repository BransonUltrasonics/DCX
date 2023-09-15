/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/InputTask.h_v   1.0   09 Jun 2015 09:10:32   tayars  $
*/
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*      Copyright (c) Branson Ultrasonics Corporation, 1995,96, 2009        */
/*     This program is the property of Branson Ultrasonics Corporation      */
/*   Copying of this software is expressly forbidden, without the prior     */
/*   written consent of Branson Ultrasonics Corporation.                    */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*************************                         **************************/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/InputTask.h_v  $
 * 
 *    Rev 1.0   09 Jun 2015 09:10:32   tayars
 * Initial revision.
 * 
 *    Rev 1.4   21 May 2013 12:25:52   amaurya
 * Code review fixes.
 * 
 *    Rev 1.3   16 Jan 2013 06:26:42   ygupta
 * Level2 Code with Profibus
 * 
 *    Rev 1.2   26 Oct 2012 02:35:44   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.1   29 Aug 2012 13:27:52   rjamloki
 * WC Modbus checkin and related changes
 */

#ifndef INPUTTASK_H_
#define INPUTTASK_H_
#include "Task.h"
#include "ArchDefs.h"
#include "portable.h"
#include "DcxManagerHandler.h"

//struct ADCData;
class InputTask: public Task
{
public:
	InputTask();
	void Tick();
protected:
	void PrintHelp();
	void Run();
	void ProgramFlash(int intf);
	void ProgramBootFlash(int intf);
	void ProgramCompressedTarRegionFlash(int intf);
	void ProgramCompressedTarRegionFromSerial();
};
BOOLEAN TestCheckForExStart(void);
BOOLEAN TestCheckForReset(void);
#endif
