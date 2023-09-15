/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/NRTModbusTask.h_v   1.5   23 Jan 2015 15:13:00   ygupta  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/NRTModbusTask.h_v  $
 * 
 *    Rev 1.5   23 Jan 2015 15:13:00   ygupta
 * Increased the number of retries and reduced the wait time to 5 & 10ms for write and read the command and response.
 * 
 *    Rev 1.4   21 May 2013 12:25:56   amaurya
 * Code review fixes.
 * 
 *    Rev 1.3   05 Nov 2012 08:58:04   amaurya
 * State Machine changes
 * 
 *    Rev 1.2   26 Oct 2012 02:37:34   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.1   10 Sep 2012 03:12:24   rjamloki
 * Modbus changed implementation with unions and code cleanup
 * 
 *    Rev 1.0   29 Aug 2012 13:31:00   rjamloki
 * Initial revision.
*/

#ifndef NRTMODBUSTASK_H_
#define NRTMODBUSTASK_H_

#include "Task.h"
#include "Fifo.h"
#include "portable.h"
#include "ObjDictionary.h"

/*Command Passed to ModbsuSendTask
 * Whenever extra data other than real time
 * is required. This command is prepared by
 * NRTask based on the received NrtQ command below
 */
struct NRTMBCmd
{
   UINT16  Req;// Value for ReqReg member of RTSendData structure
   UINT16  Indx;//array index for IndxReg of RTSendData structure
   UINT16  RegsReadCount;
   UINT16  RegsWriteCount;
   Fifo<SINT32> * Reply;
};

struct NrtQ
{
	UINT16 Cmd;//cmd like GET_WELDRESULT are filled here
	UINT8 ReadWrite;//1= write, 0=read
	//A task issuing NRT command
	//can wait for this fifo.
	Fifo<BOOLEAN> * Status;
};

#define NRT_READWRITE_ERROR 	(-1)
#define ODINDEXNRTSENDDATA      2
#define ODINDEXNRTRECEIVEDATA 	4
#define SIZE_NRTBUFF 300
#define NRTMBTASK_STACKSIZE		1024
#define NRTQ_SIZE				50
#define NRTCMD_WAITMS      		5
#define NRTREPLY_WAITMS      	10
#define READ_REQ 	 0
#define WRITE_REQ	 1
#define  CMD(x)    	 (x << 4)

enum NRTreplyState{
   NRT_NORESPONSE,
   REPLY_RECEIVED,
   TIMEOUT
};


class NRTModbus: public Task
{
public:
	NRTModbus();
	Fifo<NrtQ> Nrtq;
	static NRTModbus * thisPtr;
protected:
	virtual void Run(void);
};
extern MODBusODEntry MODtable[];
extern UINT8 NRTResponse;
#endif /* NRTMODBUSTASK_H_ */
