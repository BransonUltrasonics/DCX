/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/ModBusRcvTask.h_v   1.0   09 Jun 2015 09:10:30   tayars  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/ModBusRcvTask.h_v  $
 * 
 *    Rev 1.0   09 Jun 2015 09:10:30   tayars
 * Initial revision.
 * 
 *    Rev 1.1   10 Sep 2012 03:12:18   rjamloki
 * Modbus changed implementation with unions and code cleanup
 * 
 *    Rev 1.0   29 Aug 2012 13:31:00   rjamloki
 * Initial revision.
*/

#ifndef MODBUSRCVTASK_H_
#define MODBUSRCVTASK_H_

#include "Task.h"
#include "Fifo.h"
#include "Ethernet.h"
#include "TYP_DEF.h"
#include "ModBusSendTask.h"

/* -------------- Response ADU for func code 0x17(read write multiple regs--*/
//PDU- Protocol data unit(includes the function code and rest of the data)
//ADU- Application data unit(The complete modbus packet)
//MBAB header- Modbus Application Header
//TID- Transaction ID
//PID- Protocol ID
//UID- Unit ID or slave address for the remote slave connected to the IP device on some other
//bus e.g serial(pseudo address 255). We are expecting here a unique ID for DCP itself not
//the remote device.
/* <------- MBAP Header ------->
 * <------------------------ MODBUS UDP/IP ADU    ------------------------->
 *              		 		<----------- MODBUS PDU      -------------->
 *  +-----------+---------------+------------------------------------------+
 *  | TID | PID | Length | UID  |Code | 	                               |
 *  +-----------+---------------+------------------------------------------+
 *  |	  |		|		 |		|	  |Byte Count|Register Values----------+
 *  |     |     |        |     (0)	 (1)	     (2)<0ffsets in PDU>
 * (0)   (2)   (4)      (6)    (7)	 (8)<offsets in ADU>
 *
 */
/* ----------------------- Defines ------------------------------------------*/


//offsets define in Protocol Data Unit
#define MB_PDU_FUNC_OFFSET     0   /*!< Offset of function code in PDU. */
#define MB_PDU_BYTECOUNT_OFFSET     1   /*!< Offset of byte count in PDU. */
#define MB_PDU_REGVALUES_OFFSET     2   /*!< Offset of reg values in PDU. */

class ModBusRcvTask: public Task
{
public:
	ModBusRcvTask(SINT32 Intf);
protected:
	virtual void Run(void);
	void CreateAndBindSocket(void);
	void CloseSocket(void);
	void Block(void);
	void ProcessModBusReply(SINT32 MBUDPLen);
	MBPacket MBRcvPkt;
	SINT32 Fd;
	Ethernet * Netif;
};
extern Fifo<UINT16> MBwriteAddrLocal;
#endif /* MODBUSRCVTASK_H_ */
