/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/WelderSearchTask.h_v   1.6   20 Sep 2013 07:00:26   rjamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/WelderSearchTask.h_v  $
 * 
 *    Rev 1.6   20 Sep 2013 07:00:26   rjamloki
 * Added changes to build bootloader with same CyGOS.
 * 
 *    Rev 1.5   03 Sep 2013 18:21:10   ygupta
 * Moved to tip from 1.3.1.0
 * 
 *    Rev 1.3.1.0   24 Aug 2013 17:14:28   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.3   02 Aug 2013 10:56:00   amaurya
 * Coverty fixes.
 * 
 *    Rev 1.2   21 May 2013 12:26:14   amaurya
 * Code review fixes.
 * 
 *    Rev 1.1   29 Aug 2012 13:28:18   rjamloki
 * WC Modbus checkin and related changes
 * 
 *    Rev 1.0   02 Jul 2012 12:46:50   ygupta
 * Initial revision.
 * 
 *    Rev 1.1   04 Apr 2011 10:56:10   PDwivedi
 * Added comments, defines and code cleanup
 * 
 *    Rev 1.0   02 Mar 2011 08:14:44   PDwivedi
 * Initial revision.
*/

#ifndef WELDERSEARCHTASK_H_
#define WELDERSEARCHTASK_H_

#include "Task.h"
#include "IPv4.h"
#include "LinkedList.h"
#include "Fifo.h"
#include "Socket.h"
#include "Ethernet.h"
#include "SysConfig.h"
#include "DEF_DEF.H"

#define WELDERSEARCH_UDP_PORT 49500                           // UDP port on which the welder info will
#define MAX_PACKETTYPE_LEN 20
#define MAX_DCXM_DISCOVERYDATA_LEN 100
#define MAX_DCXTYPE_LEN 15


#define DCXMANAGERDISCOVERY_PACKET "DCXM DISCOVERY"
#define DCXMANAGERLOGOUT_PACKET "DCXM LOGOUT"  
                                                                                                         //   don’t receive any junk on the port
									//it is sent in the broadcast packet
									 //along with serial number
#define WELDERSEARCHTASK_STACKSIZE 		512 * 8             // stack size of the welder search task

#define WELDERSEARCHMSG_SOCKET_TIMEOUT 	100000  // time out duration for a socket for the select API
#define DCXM_TIMEOUT 10000000



struct WelderInfoParam
{
	struct
	{
		SINT8 PktID[MAX_PACKETTYPE_LEN];// the checksum
	}WelderInfo;  // the information about welder which needs to be broadcasted
                                     // packet reception.
};

struct DcxMData
{
	SINT8 PktData[MAX_DCXM_DISCOVERYDATA_LEN];
};

class WelderSearchTask : public Task
{
	public:
	WelderSearchTask(SINT32 Intf);
    protected:
    virtual void Run ();                                      // for processing data
    void Tick();                                                 // to be called on every RTOS tick
    int SendDcxMdata(UINT32 DestAddr);
    void CreateAndBindSocket ();                    // to create and bind a UDP socket
    void CloseSocket ();                                    // to close the binded socket
    void PrepareFDSets ();                                // to prepare FD sets to be used in socket
    //functions
    SINT32 NonBlock();                                           // to make the socket non-blocking
    BOOLEAN Poll();                                                  // waits for the data on the port
    Ethernet * Netif;                                           // Ethernet interface
    SINT32 Fd;                                                            // the socket descriptor
    fd_set InputSet;                                           // the input FD set
    fd_set ExcSet;                                              // the error FD set
    DcxMData Dcxmdata;
    ip_addr DcxMIP;
    UINT32 DcxmTimeOut;
    SINT8 IpAddr[20];
};
#endif /* TESTUDPSELECT_H_ */
