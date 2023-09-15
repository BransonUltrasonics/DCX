/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/InputTask.h_v   1.1   19 Mar 2015 06:52:30   sesharma  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/InputTask.h_v  $
 * 
 *    Rev 1.1   19 Mar 2015 06:52:30   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.0   18 Aug 2014 09:17:24   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:00   amaurya
 * Initial revision.
 */

#ifndef INPUTTASK_H_
#define INPUTTASK_H_

#include "Task.h"
#include "ArchDefs.h"

//struct ADCData;

class InputTask: public Task {
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
