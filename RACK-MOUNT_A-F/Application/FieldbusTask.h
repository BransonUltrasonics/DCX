/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/FieldbusTask.h_v   1.0   09 Jun 2015 09:10:34   tayars  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/FieldbusTask.h_v  $
 * 
 *    Rev 1.0   09 Jun 2015 09:10:34   tayars
 * Initial revision.
 * 
 *    Rev 1.8   02 Dec 2013 07:34:18   rjamloki
 * Changes for EthIP Conformance Test.
 * 
 *    Rev 1.7   03 Oct 2013 04:26:34   rjamloki
 * Watchdog related Changes
 * 
 *    Rev 1.6   01 Oct 2013 03:16:24   ygupta
 * Change for fieldbus version.
 * 
 *    Rev 1.5   11 Mar 2013 02:24:46   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.4   18 Feb 2013 03:05:40   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.3   22 Jan 2013 10:38:14   ygupta
 * Profibus Bug Fixes
 * 
 *    Rev 1.2   16 Jan 2013 06:25:24   ygupta
 * Level2 Code with Profibus
*/

#ifndef FIELDBUSTASK_H_
#define FIELDBUSTASK_H_

#include "Task.h"
#include "ArchDefs.h"
#include "fifo.h"
#include "portable.h"
#include "FieldBus.h"
#include "cifXUser.h"
#include "cifxHWFunctions.h"
#include "preset.h"

#define MAX_FBEXCHANGESIZE 64//expected maximum data transfer size per packet from fieldbus should be
							//good enough for all protocols
#define FB_COMM_CHECK_MAXLIMIT 100
#define FBVERSIONLEN 100

//Structure to be used for interrupt based implementation for hilscher fieldbus card interfacing
//currently not used
//struct COMXNotification
//{
//	unsigned int Type;//type of packet receive
//	unsigned int NMbx;//number of packets received
//};

class FieldbusTask: public Task
{
public:
	FieldbusTask(FieldBus * FbPtr);
	//Queue to invoke the task ever ms
	Fifo<UINT8> FbQ;
	//to be used with interrupt implementation. currently not used
   // Fifo<COMXNotification> Ntfc;
   //After power up init queue holds the acyclic mailbox packets need to be sent to Hilshcher slave card.
	Fifo<CIFX_PACKET> CifxPkt;
	static FieldbusTask * thisPtr;
	 //Handle to tool kit driver
	CIFXHANDLE  HDriver;
	//handle to communication channel in Hilscher DPM
	CIFXHANDLE HChannel;
   //Once the data received from fieldbus is processed the FieldbusTask wait upon following fifo for WC task to run the state machine
   //and prepare the status for field bus.
	Fifo<UINT8> WaitForWCTask;
	void UpdateMailBox(PCHANNELINSTANCE PtChannel);
	SINT32 UpdateIOImage(PCHANNELINSTANCE PtChannel);
	void SendWarmStart();
	//Cyclic data sent to fieldbus is first copied to this buffer.
	UINT8 SendBuffer[MAX_FBEXCHANGESIZE];
	//Cyclic data received from fieldbus is first copied to this buffer.
	UINT8 RecvBuffer[MAX_FBEXCHANGESIZE];
	SINT8 FBVersionInfo[FBVERSIONLEN];
	FieldBus * Fb;
	protected:
	 //Pointer to hold the value of currently running protocol class object.(Profibus or EthernetIP)
	void Run();
	void GetVersionInfo();
	void DPMMain();
};

#endif
