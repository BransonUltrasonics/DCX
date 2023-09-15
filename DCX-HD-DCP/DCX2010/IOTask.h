/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/IOTask.h_v   1.7.3.0   17 Mar 2016 16:23:42   hasanchez  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/IOTask.h_v  $
 * 
 *    Rev 1.7.3.0   17 Mar 2016 16:23:42   hasanchez
 * Added the changes to the function Adjust Stack
 * 
 *    Rev 1.7.2.0   17 Mar 2016 16:07:22   hasanchez
 * Added the changes to include the function Adjust amplitude.
 * 
 *    Rev 1.7   28 Aug 2011 11:42:24   ygupta
 * Ver1.01M
 * 
 *    Rev 1.6   16 Aug 2011 08:24:42   pdwivedi
 * code cleanup
 * 
 *    Rev 1.5   05 Aug 2011 10:32:48   pdwivedi
 * Removed warning.
 * 
 *    Rev 1.4   02 Aug 2011 07:38:16   pdwivedi
 * Added changes for PLL Loss lock, DCXManual and remving warnings.
 * 
 *    Rev 1.3   15 Jul 2011 09:46:14   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.2   06 Dec 2010 09:56:54   PDwivedi
 * Added changes for Revision Board C support, LCD Key Timeout changes.
 * 
 *    Rev 1.1   29 Nov 2010 07:47:36   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:44   ygupta
 * Initial revision.

******************************************************************************/

#ifndef IOTASK_H_
#define IOTASK_H_

#include "Task.h"
#include "MCP3208.h"
#include "TLV5604.h"
#include <CPUdefs.h>
#include "FIFO.h"

#define IOTASK_STACKSIZE  2048 * 2

struct AdcBuf
{
	int data1[5];
	int len;
};

class IOTask : public Task
{
	enum BuffSel
	{
		bufA, bufB,
	};
public:
	IOTask();
	static IOTask * thisPtr;
	void TestADC();
	void ReadData();
	int Receive(int *data, int timeout);
	Fifo<AdcBuf*> rxQueue;
	Fifo<char> IOTaskQ;
	Fifo<char> outQ;
	AdcBuf aa, bb;
	BuffSel buffSel;
	AdcBuf * buffPtr;
protected:
	void Run();
	SPI * spi;
};

#endif /* IOTASK_H_ */

