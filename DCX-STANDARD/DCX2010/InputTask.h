/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/InputTask.h_v   1.3   16 Dec 2010 08:17:40   PDwivedi  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/InputTask.h_v  $
 * 
 *    Rev 1.3   16 Dec 2010 08:17:40   PDwivedi
 * Added changes for Website Code Clean Up and Compressed file download from serial.
 * 
 *    Rev 1.2   14 Dec 2010 11:15:10   PDwivedi
 * Added changes for WebSite support.
 * 
 *    Rev 1.1   29 Nov 2010 07:47:36   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:42   ygupta
 * Initial revision.

******************************************************************************/

#ifndef INPUTTASK_H_
#define INPUTTASK_H_

#include "Task.h"
#include "ArchDefs.h"

//struct ADCData;


class InputTask: public Task
{
public:
	InputTask();
protected:
	void PrintHelp();
	void Run();
	void ProgramFlash(int intf);
	void ProgramCompressedTarRegionFlash(int intf);
	void ProgramCompressedTarRegionFromSerial();
};

#endif
