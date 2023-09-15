/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/MIATask.h_v   1.1   11 Sep 2012 10:12:26   rjamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/MIATask.h_v  $
 * 
 *    Rev 1.1   11 Sep 2012 10:12:26   rjamloki
 * User IO update and clean up
 * 
 *    Rev 1.0   02 Jul 2012 12:46:46   ygupta
 * Initial revision.
 *
 */

#ifndef MIATASK_H_
#define MIATASK_H_

#include "Task.h"
#include "FIFO.h"

#define MIATASK_STACKSIZE  4096

class MIATask : public Task
{
public:
	MIATask();
	static MIATask * thisPtr;
	Fifo<char> MIATaskQ;
	Fifo<char> OutQ;
protected:
	void Run();
};

#endif /* MIATASK_H_ */

