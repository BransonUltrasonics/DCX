/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/ModBusSendTask.h_v   1.14   13 Mar 2014 15:54:48   rjamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/ModBusSendTask.h_v  $
 * 
 *    Rev 1.14   13 Mar 2014 15:54:48   rjamloki
 * FRAM is accessed through pointers. Removed compiler allocation of FRAM
 * 
 *    Rev 1.13   03 Sep 2013 12:50:52   ygupta
 * Moved to tip from 1.11.1.7.1.0
 * 
 *    Rev 1.11.1.7.1.0   24 Aug 2013 17:14:06   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.11.1.7   02 Aug 2013 10:55:48   amaurya
 * Coverty fixes.
 * 
 *    Rev 1.11.1.6   14 Jun 2013 11:21:08   amaurya
 * MH1 Round fixes
 * 
 *    Rev 1.11.1.5   28 May 2013 07:43:08   amaurya
 * Fixed issues of Level II testing.
 * 
 *    Rev 1.11.1.4   22 May 2013 10:41:36   ygupta
 * Fixes for Disable Stepping in while in after burst, energy braking and scrub time.
 * 
 *    Rev 1.11.1.3   21 May 2013 12:25:54   amaurya
 * Code review fixes.
 * 
 *    Rev 1.11.1.2   06 May 2013 09:56:16   ygupta
 * Changes to use actual frequency.
 * 
 *    Rev 1.11.1.1   22 Apr 2013 11:31:10   amaurya
 * Fixed tracker issue of Ver2.0S
 * 
 *    Rev 1.11.1.0   20 Mar 2013 09:37:48   ygupta
 * RF Switch Changes, Requirement Changes
 * 
 *    Rev 1.12   20 Mar 2013 08:18:00   ygupta
 * RF Switch Changes, Requirement Changes
 * 
 *    Rev 1.11   15 Mar 2013 10:50:42   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.10   11 Mar 2013 02:27:04   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.9   18 Feb 2013 03:08:12   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.8   03 Jan 2013 07:18:42   ygupta
 * Modbus Comm problem Resolved
 * 
 *    Rev 1.7   29 Nov 2012 08:45:52   rjamloki
 * Scan Changes and Coverty issues Resolved
 * 
 *    Rev 1.6   24 Nov 2012 08:02:40   amaurya
 * Added changes for 40KHz supply.
 * 
 *    Rev 1.5   05 Nov 2012 08:57:38   amaurya
 * State Machine changes
 * 
 *    Rev 1.4   26 Oct 2012 02:37:08   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.3   21 Sep 2012 01:32:58   rjamloki
 * Added State machine
 * 
 *    Rev 1.2   10 Sep 2012 03:12:22   rjamloki
 * Modbus changed implementation with unions and code cleanup
 * 
 *    Rev 1.1   29 Aug 2012 13:28:00   rjamloki
 * WC Modbus checkin and related changes
*/

#ifndef MODBUSSENDTASK_CPP_H_
#define MODBUSSENDTASK_CPP_H_

#include "Task.h"
#include "Fifo.h"
#include "Ethernet.h"
#include "portable.h"
#include "SysConfig.h"
#include "DEF_DEF.H"
#include "NRTModbusTask.h"

//Size of a holding register in bytes in standard modbus protocol.
#define HOLDING_REG_SIZE	2
//The defines for count field of Modbus table Entry. The count represents the number of holding registers
//corresponding to an entry. If the entry(object of structure mapped in OD) size is not multiple of HOLDING_REG_SIZE
//the count should be added with one for remaining single byte.
#define POWERUPDATA_REGCOUNT   (sizeof(DCPPowerUpData)/HOLDING_REG_SIZE + sizeof(DCPPowerUpData)%HOLDING_REG_SIZE)
#define RTRECEIVEDATA_REGCOUNT (sizeof(ModBusRTReceiveData)/HOLDING_REG_SIZE + sizeof(ModBusRTReceiveData)%HOLDING_REG_SIZE)
#define RTSENDDATA_REGCOUNT    (sizeof(ModBusRTSendData)/HOLDING_REG_SIZE + sizeof(ModBusRTSendData)%HOLDING_REG_SIZE)
#define BASETIME_REGCOUNT      (sizeof(TimeStamp)/HOLDING_REG_SIZE + sizeof(TimeStamp)%HOLDING_REG_SIZE)
#define PRESET_REGCOUNT        (((sizeof(DCPPreset))/HOLDING_REG_SIZE) + ((sizeof(DCPPreset))%HOLDING_REG_SIZE))
#define ACK_REGCOUNT	       (sizeof(ModbusAck)/HOLDING_REG_SIZE +  sizeof(ModbusAck)%HOLDING_REG_SIZE)
#define MACCHIPDATA_REGCOUNT   (sizeof(MACCHIPRWData)/HOLDING_REG_SIZE + sizeof(MACCHIPRWData)%HOLDING_REG_SIZE)
#define DCPTIMEPARAM_REGCOUNT   (sizeof(DCPTimeControl)/HOLDING_REG_SIZE + sizeof(DCPTimeControl)%HOLDING_REG_SIZE)

//Defines for the index filed of modbus table entry
#define MB_POWER_UP_DATA_INDX	 1800
#define MB_RT_SEND_DATA_INDX	 2000
#define MB_NRT_SEND_DATA_INDX 	 (MB_RT_SEND_DATA_INDX + RTSENDDATA_REGCOUNT)
#define MB_RT_RECEIVE_DATA_INDX	 10000
#define MB_NRT_RECEIVE_DATA_INDX (MB_RT_RECEIVE_DATA_INDX + RTRECEIVEDATA_REGCOUNT)
#define MB_BASE_TIME_INDX		 10118
#define MB_PRESET_INDX			 10130
#define MB_ACK_INDX				 15000
#define MB_MACCHIP_INDX			 15010
#define MB_DCPTIMEPARA_INDX		 15080

/* Must hold a complete Modbus UDP frame. 7 bytes header + PDU*/
#define MB_UDP_BUF_SIZE     	 ( 256 + 7 )

/* ----------------------- Modbus TCP/UDP Packet------------------------------------------*/
//PDU- Protocol data unit(includes the function code and rest of the data)
//ADU- Application data unit(The complete modbus packet)
//MBAB header- Modbus Application Header
//TID- Transaction ID
//PID- Protocol ID
//UID- Unit ID or slave address for the remote slave connected to the IP device on some other
//bus e.g serial(pseudo address 255). We are expecting here a unique ID for DCP itself not
//the remote device.
/* <------- MBAP Header ------->
 * <------------------------ MODBUS UDP/IP ADU(1) ------------------------->
 *              		 		<----------- MODBUS PDU (1') -------------->
 *  +-----------+---------------+------------------------------------------+
 *  | TID | PID | Length | UID  |Code | Data                               |
 *  +-----------+---------------+------------------------------------------+
 *  |     |     |        |      |
 * (0)   (2)   (4)      (6)    (7) <--offsets in modbus UDP packet without UDP header-->
 *
 * (2)  ... MB_UDP_TID          = 0 (Transaction Identifier - 2 Byte)
 * (3)  ... MB_UDP_PID          = 2 (Protocol Identifier - 2 Byte)
 * (4)  ... MB_UDP_LEN          = 4 (Number of bytes - 2 Byte)
 * (5)  ... MB_UDP_UID          = 6 (Unit Identifier - 1 Byte)
 * (6)  ... MB_UDP_FUNC         = 7 (Modbus Function Code)
 *
 * (1)  ... Modbus UDP/IP Application Data Unit
 * (1') ... Modbus Protocol Data Unit
 */

/*  <----------- MODBUS PDU For Read Write Multiple reg Command) --------------------------------->
 *  Code |ReadIndx|ReadRegCnt|WriteAddress|WriteRegCnt|WriteByteCount| Data = (WriteRegCnt * 2) bytes
 *  ----------------------------------------------------------------------------------------------+
 *  |    |        |          |            |			  |				 |
 *  (7)  (8)      (10)       (12)         (14)        (16)			 (17)
 *
 */
//The modbus TCP/UDP ADU can be represented
//by following union. The commands and reply
//can use the same union.
union MBPacket{
   struct{
	   struct{
		  UINT16 TID;
		  UINT16 ProtocolID;
		  UINT16 Length;
		  UINT8  SlaveId;
		  UINT8  FunctinCode;
	   }MBAPheader;
	   union{
		   struct{
		      UINT16 Readaddr;
			  UINT16 ReadRegs;
			  UINT16 WriteAddr;
			  UINT16 WriteRegs;
			  UINT8  WriteByteCount;
		   }MBPDUReadWriteMultipleReq;
		   union{
			  UINT8 ByteCount;
			  UINT8 Exception;
		   }MBReply;
	   }MBReadWriteMultiple;
   }MBAdu;
   UINT8 Query[MB_UDP_BUF_SIZE];
};

//offset defines in Modbus UDP packet. Common for all commands
#define MB_UDP_TID_OFFSET          0
#define MB_UDP_PID_OFFSET          2
#define MB_UDP_LEN_OFFSET          4
#define MB_UDP_UID_OFFSET          6
#define MB_UDP_FUNC_OFFSET         7
//offsets specific to read write multiple registers command
#define MB_UDP_READADDR_OFFSET       8
#define MB_UDP_READREGCOUNT_OFFSET   10
#define MB_UDP_WRITEADDR_OFFSET      12
#define MB_UDP_WRITEREGCOUNT_OFFSET  14
#define MB_UDP_WRITEBYTECOUNT_OFFSET 16
#define MB_UDP_WRITEDATA_OFFSET		 17

//Max permitted reg read/write count for function code 0x17(read write multiple registers)
#define MAX_REGISTERS_READ      125
#define MAX_REGISTERS_WRITE     121
#define MB_UDP_PROTOCOL_ID  0   /* 0 = Modbus TCP/UDP Protocol */

//function code for read write multiple holding register command (0x17)
#define MB_FUNC_READWRITE_MULTIPLE_REGISTERS  ( 23 )
//The function code received in reply has the highest bit set in case of some exception.
//below define is used to detect the same.
#define MB_FUNC_ERROR                         ( 128 )

#define DCPMBSLAVEADDRESS	1
#define MODBUSTASK_STACKSIZE 	 4096
#define REG_HOLDING_START 		 MB_POWER_UP_DATA_INDX
#define MB_UDP_DEFAULT_PORT 	 502 /* UDP listening port. */
#define MB_SOURCE_UDPPORT		 503/* can be anyone*/

/*Modbus standard exception code*/
typedef enum
{
   MB_EX_NONE = 0x00,
   MB_EX_ILLEGAL_FUNCTION = 0x01,
   MB_EX_ILLEGAL_DATA_ADDRESS = 0x02,
   MB_EX_ILLEGAL_DATA_VALUE = 0x03,
   MB_EX_SLAVE_DEVICE_FAILURE = 0x04,
   MB_EX_ACKNOWLEDGE = 0x05,
   MB_EX_SLAVE_BUSY  = 0x06,
   MB_EX_MEMORY_PARITY_ERROR = 0x08,
   MB_EX_GATEWAY_PATH_FAILED = 0x0A,
   MB_EX_GATEWAY_TGT_FAILED  = 0x0B
}MBException;

/*! \in group modbus
 * \brief Error codes used by all function in the protocol stack.
 */
typedef enum
{
    MB_ENOERR,                  /*!< no error. */
    MB_ENOREG,                  /*!< illegal register address. */
    MB_EINVAL,                  /*!< illegal argument. */
    MB_EPORTERR,                /*!< porting layer error. */
    MB_ENORES,                  /*!< insufficient resources. */
    MB_EIO,                     /*!< I/O error. */
    MB_EILLSTATE,               /*!< protocol stack in illegal state. */
    MB_ETIMEDOUT                /*!< timeout error occurred. */
} MBErrorCode;

typedef enum
{
    MB_REG_READ,                /*!< Read register values and pass to protocol stack. */
    MB_REG_WRITE                /*!< Update register values. */
} MBRegisterMode;

// Real time data from WC to DCP exchanged every millisecond
struct ModBusRTSendData
{
	UINT16 	CmdFlags;
	SINT16 	AmplitudeIn;
	UINT16 	RampTime;
	SINT32 	FreqIn;
	SINT32   MemoryValue;
	UINT16 	CmdFlagsReserved;
	UINT16 	Reserved16;
	UINT16 	ReqReg;
	UINT16 	IndxReg;
};

// Real time data from DCP to WC exchanged every millisecond
struct ModBusRTReceiveData
{
   UINT16  DigOutputs;
   UINT16  StatusFlags;
   SINT32  ErrorReason;
   SINT16  PowerOut;
   SINT16  NominalAmplitude;
   SINT16  AmplitudeOut;
   SINT32  FreqOut;
   SINT32  MemOut;
   SINT16  Phase;
   SINT16  PWM;
   SINT16  Current;
   SINT16  Temperature;
   UINT16  Reserved16;
   UINT16  StatReg;
   UINT16  IndxReg;
};

//The BITs representing individual digital signal of (RtsendData.CmdFlags)
//in Modbus real time data from WC to DCP
#define MB_START  			BIT(0)	//start sonics
#define MB_RESET  			BIT(1) //power supply reset
#define MB_SEEK   			BIT(2)
#define MB_TEST   			BIT(3)
#define MB_MEMCLR 			BIT(4)
#define MB_HSCAN  			BIT(5)
#define MB_ACCUM_REQ  		BIT(6)//Bit needs to set by WC in peakPower and Energy weld mode
#define MB_USEREALTIMEDATA 	BIT(7)//Notify to DCP to use Amplitude and ramp time of real time send data
#define MB_INITIATE_BOARDTEST BIT(8)
#define MB_SONICS_DISABLE  BIT(9)
#define MB_CLEAREVENTLOG  BIT(10)
#define MB_DISABLESTEPPING  BIT(11)

//The BITs which DCP need to set in Modbus real time data to WC in RtReceiveData.DigOutputs
#define MB_READY_OUT 			BIT(0)//Indicates DCP is ready
#define MB_SONICACTIVE_OUT 		BIT(1)
#define MB_GENERALALARM_OUT 	BIT(2)
#define MB_OVERLOADALARM_OUT 	BIT(3)
#define MB_SEEKSCAN_OUT 		BIT(4)
#define MB_AMPLITUDEDECAY_OUT 	BIT(5)


//The below definitions are related to ModBusRTReceiveData structure data member
//StatusFlags – This variable contains extra information like a Event signaling to WC.
//Bit 0 – Notify to WC about event has happened on DCP side.
//Bit 1 – Notify to WC that preset has been updated on DCP side.
//Bit 2 – Notify to WC that Primary parameter(Time or Energy or Peak Power) has been reached.
//Bit 3 - Notify to WC that SCAN has started.
//Bit 4 - Notify to WC that Weld has been started.
//Bit 5 - Notify to WC that Stepping is done on DCP side.
//Bit 6 – Notify to WC that firmware upgrade process is going on.
//Bit 7 - 15 – reserved.
#define SIGNAL_EVENT             			BIT(0)
#define SIGNAL_EVENTCLEARED      		BIT(1)
#define SIGNAL_PRIMARYPARAMETER_REACHED		BIT(2)		// Primary Parameters may be Time, energy or peak power
#define SIGNAL_COLLECTHORNSCANPOINT	   		BIT(3)
#define SIGNAL_COLLECTWELDPOINT   		 	BIT(4)
#define SIGNAL_STEPPING_DONE	   		 	BIT(5)
#define SIGNAL_FIRMWAREUPGRADE   		 	BIT(6)

//while the DCP is in board test mode ModBusRTSendData structure member
//CmdFlags contains the digital outputs which goes from FPGA in DCP board to
//Instrumentation board(Note:The instrumentation board is not connected in test mode)
#define TEST_BIT BIT(0)//bit to force the FPGA in Test mode
//other output values
#define OLRST_BIT BIT(1)
#define DPSSTOP_BIT BIT(2)
#define PHASEA_BIT BIT(3)
#define PHASEB_BIT BIT(4)

//The below definitions are related to ModBusRTSendData structure data member ReqReg
//which WC sends every millisecond to DCP.
//ReqReg- Contains a value depending on the Non real time data required.
//Bit 0 – 3 meaning.
//Bit 0 – Does this data belong to some array? 1- Yes 0- No(IndxReg of ModBusRTSendData in array data case
//contains the index of array element)
//Bit 1 – Read or write request? 1- Write 0- Read
//Bit 2 – 3 reserved
//Data additional data read/write requested by WC is specified by Weightage of Bit 4 – 7 of ReqReg.
//Total 15 available read commands
#define CMD_NODATA         	  0
#define CMD_GETWELDRESULT  	  1
#define CMD_GETEVENTLOG    	  2
#define CMD_GETALARMLOG    	  3
#define CMD_GETTASKLOAD    	  4
#define CMD_GETINTERRUPTLOAD  5
#define CMD_GETSEEKDATA       6
#define CMD_DCPEVENT_LOG_COUNT   7
//7 to 15 reserved

//Total 15 available write commands
#define CMD_PRESETWRITE 	  1
#define CMD_SYNCRTC           2
#define CMD_MACCHIPDATAWRITE  3
#define CMD_DCPTIMEPARAMWRITE  4

//ModBusRTReceiveData Expected StatReg value-
//Bit 0 – 3 Reserved.
//Bit 4 – 7 same as ReqReg member of ModBusRTSendData structure.
//Bit 8 -15 Reserved.
//ModBusRTReceiveData Expected IndxReg value-Same as ModBusRTSendData IndexReg

//DCPPowerUpData related defines
#define BOOTSWVERSION_LEN   8
#define FPGASWVERSION_LEN   8
//Power up data received from DCP
struct DCPPowerUpData
{
   FREQUENCY  Freq;
   UINT32  Pwr;
   SYSTEMTYPE CurrSystemType;
   //DCP
   SINT8   SWVersionLabel[SWVERSION_LEN];
   UINT32  DCXCRC;//Flash CRC
   //FPGA
   SINT8   FPGAVersionLabel[FPGASWVERSION_LEN];

   //BootLoader
   SINT8   BootLoaderVersionLabel[BOOTSWVERSION_LEN];
   UINT32  BootLoaderCRC;//BootLoader CRC
   UINT8   DCPMACAddr[MAC_ADDRESS_LEN];
   UINT16  CRC;
};

struct DCPTimeControl
{
	UINT32 DCPTime;
	UINT32 ControlLoopTime;
	UINT32 StateMachineTime;
	UINT32 ModBusCommTime;
	UINT32 AdcResolution;
	UINT32 BlindTime;
	UINT16 CRC;
};

//A object of this type is mapped in Modbus Table to
//read back the result of write operation on remote OD for
//couple of commands (write preset, write mac chip data)
//Modbus doesn't require to send back additional
//data to indicate that the write operation was successful
//This is just for our confirmation.
struct ModbusAck
{
   UINT16  ACK;
};

//size for the ReqQueue fifo of type NRTMBCmd
#define REQ_QUEUESIZE 50
//size for the ModBusRealTimeQ fifo invoked by timer interrupt
#define MBRTQ_SIZE  100

class ModBusSendTask: public Task
{
  public:
	Ethernet * Netif;
	ModBusSendTask(SINT32 Intf);
	static ModBusSendTask * thisptr;
    void NotifyReadWriteEvent(SINT32 Length);
    Fifo<SINT8> ModBusRealTimeQ;
	Fifo<NRTMBCmd> ReqQueue;
	NRTMBCmd Cmd;
  protected:
	void CreateAndBindSocket(void);
	void CloseSocket(void);
	void NonBlock(void);
	virtual void Run(void);
	void Tick(void);
	void ReadWriteMultipleRegisters(UINT32 SlaveAddress, UINT16 ReadAddrSlave,
			UINT16 NRegRead, UINT16 WriteAddrSlave, UINT16 NRegWrite, UINT16 ReadAddrLocal);
	void BuildQueryBasis(UINT32 Slave, UINT32 Function, UINT16 ReadAddr, UINT16 NRegsRead, UINT16 WriteAddr,
			UINT16 NRegWrite);
	void StartPowerupCommunicationWithDCP();
    SINT32 Fd;
    MBPacket MBPkt;
    BOOLEAN Running ;//Goes TRUE and FALSE with Ethernet link status
    EthAddr Dest;
	UINT8 Query[MB_UDP_BUF_SIZE + sizeof(EthHeader)];
};
MBErrorCode MBRegHoldingCB(UINT8 * RegBuffer, UINT16 UsAddress, UINT16 UsNRegs, MBRegisterMode EMode, UINT8 * DataBuff, int MaxBuffSize = 0);
extern ModbusAck MBack;
extern struct DCPPowerUpData  PowerUpData;
extern struct DCPTimeControl DCPTimeControlParam;
#endif /* MODBUSRECEIVETASK_CPP_H_ */
