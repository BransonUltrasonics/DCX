/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/NRTModbusTask.cpp_v   1.0   09 Jun 2015 09:10:30   tayars  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/NRTModbusTask.cpp_v  $
 * 
 *    Rev 1.0   09 Jun 2015 09:10:30   tayars
 * Initial revision.
 * 
 *    Rev 1.19   23 Jan 2015 15:12:58   ygupta
 * Increased the number of retries and reduced the wait time to 5 & 10ms for write and read the command and response.
 * 
 *    Rev 1.18   25 Oct 2013 11:59:14   ygupta
 * Disabled Nagle Algo on http connection, Added JSONStopDCX Data service, Tracker fixes, FinalData State renamed to CycleAbort State.
 * 
 *    Rev 1.17   15 Oct 2013 07:42:10   rjamloki
 * Tracker Fix, JSON Fix, Link Status debouncing configurable from Adv R&D page.
 * 
 *    Rev 1.16   08 Oct 2013 06:17:34   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.15   03 Sep 2013 12:51:24   ygupta
 * Moved to tip from 1.13.1.0
 * 
 *    Rev 1.13.1.0   24 Aug 2013 17:14:06   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.13   17 Jul 2013 06:20:40   amaurya
 * Fixed WeldTime Issue on History page and added diagnostic code.
 * 
 *    Rev 1.12   10 Jul 2013 07:51:52   amaurya
 * Fixed powerup DCP Event Log issue.
 * 
 *    Rev 1.11   21 May 2013 12:25:56   amaurya
 * Code review fixes.
 * 
 *    Rev 1.10   15 Mar 2013 10:50:54   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.9   11 Mar 2013 02:27:16   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.8   18 Feb 2013 03:08:24   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.7   14 Dec 2012 05:27:10   ygupta
 * Alarms Implemented
 * 
 *    Rev 1.6   24 Nov 2012 08:02:52   amaurya
 * Added changes for 40KHz supply.
 * 
 *    Rev 1.5   05 Nov 2012 08:57:50   amaurya
 * State Machine changes
 * 
 *    Rev 1.4   26 Oct 2012 02:37:20   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.2   21 Sep 2012 01:33:14   rjamloki
 * Added State machine
 * 
 *    Rev 1.1   10 Sep 2012 03:12:24   rjamloki
 * Modbus changed implementation with unions and code cleanup
 * 
 *    Rev 1.0   29 Aug 2012 13:31:00   rjamloki
 * Initial revision.
*/

#include "NRTModbusTask.h"
#include "GLOBALS.h"
#include "ModBusSendTask.h"
#include "RTClock.h"
#include "TYP_DEF.h"
#include "WeldResultHistory.h"
#include "StateTable.h"
#include "FUNC.h"

//#define printf
//#define pprintf
static const bool EnableDebug = false;
NRTModbus * NRTModbus::thisPtr;
UINT8 NRTDatabuffer[SIZE_NRTBUFF];
DcpWeldResult WeldHistoryData;
UINT8 NRTResponse;
SINT16 DCPEventLogs = 0;
EventLog DCPEventobj[MAX_DCP_EVENT_ENTRIES];
SINT16 DCPEventIndex = -1;
struct HornSeekPointData SeekData;
UINT32 DCPTimeSyncCounter = 0;
/*   Constructor.
 *   Purpose:
 *   	To initialize the task stack, fifo objects and
 *		member variables of class.
 *
 *   Entry condition:
 *   	None.
 *
 *   Exit condition:
 * 		None
 */
NRTModbus::NRTModbus():Task( "NRTModbus", NRTMBTASK_STACKSIZE ), Nrtq(NRTQ_SIZE , "Q" )
{
   thisPtr = this;
}

/*  Purpose:
 *    This task is required to read/write NRT data depending on
 *    the command issued.
 *
 *  Entry condition:
 *	  None
 *
 *  Exit condition:
 *	  None.
 */
void NRTModbus::Run(void)
{
   //Some delay to make sure all driver components are initialized properly
   DelayMs(100);
   NrtQ NrtQObj;
   NRTMBCmd Cmd = {0, 0, 0, 0, 0};
   Fifo<SINT32> Reply(1, "cmd");
   SINT32 RepliedDataLen;
   Cmd.Reply = &Reply;
   MODBusODEntry OdEntryNRT ={0, 0, 0, 0, 0,0};
  // AlarmType Alarmtype;
   AlarmLog Alarmobj;
   RTCTime TM;
   //Flag to send back the status to the task which requested NRT data
   BOOLEAN Status;
   //Expected data length in case of NRT read
   SINT32 ExpectedDataLen;
   //Attempt counts
   UINT16 Attemptcnt = 0;
   SINT16 ArrayIndex = 0;
   for(;;){
      //Wait infinitely for NRT command
      Nrtq.Read(this, NrtQObj, -1);     
      Cmd.RegsReadCount  = 0;
      Cmd.RegsWriteCount = 0;
      Cmd.Indx = 0;
      Status = TRUE;
      ExpectedDataLen = 0;
      Attemptcnt = 0;
      if(NrtQObj.ReadWrite == READ_REQ){
    	 NRTResponse = NRT_NORESPONSE;
         OdEntryNRT.Index = MB_NRT_RECEIVE_DATA_INDX;
         Cmd.Req = 0 | CMD(NrtQObj.Cmd);
         switch(NrtQObj.Cmd){
            case CMD_GETWELDRESULT:
               Cmd.RegsReadCount = (sizeof(WeldResult) / HOLDING_REG_SIZE + sizeof(WeldResult) % HOLDING_REG_SIZE);
            break;
            case CMD_DCPEVENT_LOG_COUNT:
//               printf("\n CMD_DCPEVENT_LOG_COUNT");
               Cmd.RegsReadCount = (sizeof(DCPEventLogs) / HOLDING_REG_SIZE + sizeof(DCPEventLogs) % HOLDING_REG_SIZE);
            break;
            case CMD_GETEVENTLOG:
               Cmd.RegsReadCount = (sizeof(EventLog) / HOLDING_REG_SIZE + sizeof(EventLog) % HOLDING_REG_SIZE);
            break;
            case CMD_GETALARMLOG:
               Cmd.RegsReadCount = (sizeof(AlarmLog) / HOLDING_REG_SIZE + sizeof(AlarmLog) % HOLDING_REG_SIZE);
            break;
            case CMD_GETSEEKDATA:
               Cmd.RegsReadCount = (sizeof(HornSeekPointData) / HOLDING_REG_SIZE + sizeof(HornSeekPointData) % HOLDING_REG_SIZE);
            break;
            //TODO:Add task load and interrupt load
            default:
               ASSERT(0);
            break;
         }//switch(q.cmd)
         ExpectedDataLen = Cmd.RegsReadCount * HOLDING_REG_SIZE;
         Cmd.Req &= (~(BIT(1)));//0 = read
         //Buffer where we will receive the data
         OdEntryNRT.Variable = NRTDatabuffer;
         OdEntryNRT.VarSize = sizeof(NRTDatabuffer);
         OdEntryNRT.Count = Cmd.RegsReadCount;
         //Modify the OD entry where we are going to receive the data
         MODtable[ODINDEXNRTRECEIVEDATA] = OdEntryNRT;
         RepliedDataLen = 0;
         while((ModBusErrorFlag == FALSE) && (RepliedDataLen < ExpectedDataLen) && (Attemptcnt < NUMBER_OF_MB_RETRIES)){
            //NRTCMD_WAITMS ms should be good enough to read write fifo without overflowing
            if(ModBusSendTask::thisptr -> ReqQueue.Write(Cmd, NRTCMD_WAITMS)){
               if(Cmd.Reply -> Read(RepliedDataLen, NRTREPLY_WAITMS)){
                  if(RepliedDataLen != ExpectedDataLen)
                     Attemptcnt++;
                  else{
                     switch(NrtQObj.Cmd){
                        case CMD_DCPEVENT_LOG_COUNT:
                           memcpy((void *)&DCPEventLogs, NRTDatabuffer, sizeof(DCPEventLogs));
                           NRTResponse = REPLY_RECEIVED;
                        break;
                        case CMD_GETEVENTLOG:
                           DCPEventIndex = ((DCPEventIndex + 1) % MAX_DCP_EVENT_ENTRIES);
                           ArrayIndex = DCPEventLogs - DCPEventIndex - 1;
                           if((ArrayIndex >= 0) && (ArrayIndex < MAX_DCP_EVENT_ENTRIES))
							   memcpy(&DCPEventobj[DCPEventLogs - DCPEventIndex - 1], NRTDatabuffer, sizeof(EventLog));
                           NRTResponse = REPLY_RECEIVED;
                        break;
                        case CMD_GETALARMLOG:
                           memcpy(&Alarmobj, NRTDatabuffer, sizeof(AlarmLog));
                           AlarmPtr = &Alarmobj.AlarmData;
//                              Alarmtype = UnknownAlarm;
//                              if(Alarmobj.AlarmID >= PHASE_OVERLOAD_AL_LOG &&
//                                    Alarmobj.AlarmID <= POWERLIMIT_OVERLOAD_AL_LOG)
//                                 Alarmtype = OverloadAlarm;
//                              else if(Alarmobj.AlarmID >= STARTFREQ_OUTSIDE_BANDWIDTH_AL_LOG &&
//                                    Alarmobj.AlarmID <= CUTOFF_ENERGY_AL_LOG)
//                                 Alarmtype = LimitAlarm;
                           Log::WriteAlarmLog((Alarms)Alarmobj.AlarmID, FALSE);
                        break;
                        case CMD_GETWELDRESULT:
                           memcpy(&WeldHistoryData, NRTDatabuffer, sizeof(DcpWeldResult));
                           NRTResponse = REPLY_RECEIVED;
                        break;
                        case CMD_GETSEEKDATA:
                           memcpy(&SeekData, NRTDatabuffer, sizeof(HornSeekPointData));
                           NRTResponse = REPLY_RECEIVED;
                        break;
                        default:
                           Attemptcnt++;
                        break;
                     }
                  }
               }
               else
                  Attemptcnt++;
            }
            else
               Attemptcnt++;
         }
      }
      else if(NrtQObj.ReadWrite == WRITE_REQ){
         OdEntryNRT.Index = MB_NRT_SEND_DATA_INDX;
         switch(NrtQObj.Cmd){
            case CMD_PRESETWRITE:
               Cmd.Req = 0 | CMD(CMD_PRESETWRITE);
               Cmd.RegsWriteCount  = PRESET_REGCOUNT;
               OdEntryNRT.Variable = &DcpParameterSet;
               OdEntryNRT.VarSize = sizeof(DcpParameterSet);
            break;
            case CMD_SYNCRTC:
               RTClock::GetCurrentTime(&TM);	// Read RTC
               SendRtc.Second = TM.Second;
               SendRtc.Minute = TM.Minute;
               SendRtc.Hour   = TM.Hour;
               SendRtc.Day    = TM.Day;
               SendRtc.Month  = TM.Century_Month;
               SendRtc.Year   = TM.Years;
               Cmd.Req = 0 | CMD(CMD_SYNCRTC);
               Cmd.RegsWriteCount = BASETIME_REGCOUNT;
               OdEntryNRT.Variable = &SendRtc;
               OdEntryNRT.VarSize = sizeof(SendRtc);
            break;
            case CMD_MACCHIPDATAWRITE:
               Cmd.Req = 0 | CMD(CMD_MACCHIPDATAWRITE);
               Cmd.RegsWriteCount = MACCHIPDATA_REGCOUNT;
               OdEntryNRT.Variable = &MacchipData;
               OdEntryNRT.VarSize = sizeof(MacchipData);
            break;
            case CMD_DCPTIMEPARAMWRITE:
               Cmd.Req = 0 | CMD(CMD_DCPTIMEPARAMWRITE);
			   Cmd.RegsWriteCount = DCPTIMEPARAM_REGCOUNT;
			   OdEntryNRT.Variable = &DCPTimeControlParam;
			   OdEntryNRT.VarSize = sizeof(DCPTimeControlParam);
            break;
            default:
                ASSERT(0);
         }
         Cmd.RegsReadCount = ACK_REGCOUNT;
         /////////////
         MODtable[ODINDEXNRTRECEIVEDATA].Count = ACK_REGCOUNT;
         MBack.ACK = 0;
         MODtable[ODINDEXNRTRECEIVEDATA].Variable = &MBack;
         MODtable[ODINDEXNRTRECEIVEDATA].VarSize = sizeof(MBack);
         MODtable[ODINDEXNRTRECEIVEDATA].Index = MB_NRT_RECEIVE_DATA_INDX;
         ExpectedDataLen = Cmd.RegsReadCount * HOLDING_REG_SIZE;
         /////////////
         Cmd.Req |= (BIT(1));		// 1 = write
         Cmd.Indx = 0;
         OdEntryNRT.Count = Cmd.RegsWriteCount;
         //Modify the OD entry according to the data we want to send
         MODtable[ODINDEXNRTSENDDATA] = OdEntryNRT;

         while((Attemptcnt < NUMBER_OF_MB_RETRIES) && (MBack.ACK == 0)){
            if(ModBusSendTask::thisptr -> ReqQueue.Write(Cmd, NRTCMD_WAITMS)){
               //if(MBTxTID == MBRxTID){
                  if(Cmd.Reply -> Read(RepliedDataLen, NRTREPLY_WAITMS)){
                     if(RepliedDataLen == ExpectedDataLen){
                        if(MBack.ACK == 0)
                           Attemptcnt++;
                        else{
                           switch(NrtQObj.Cmd){
                              case CMD_PRESETWRITE:
                                 if(EnableDebug) pprintf("\n preset Ack rcvd!");
                              break;
                              default:
                              break;
                           }
                        }//else
                     }
                     else
                        Attemptcnt++;
                  }
                  else
                     Attemptcnt++;
            }
            else
               Attemptcnt++;
         }
      }
      if((Attemptcnt >= NUMBER_OF_MB_RETRIES) || (ModBusErrorFlag == TRUE)){
         //TODO: Write event or raise alarm.
    	  printf("\n Retry%d for cmd%d R/W %d",Attemptcnt,NrtQObj.Cmd,NrtQObj.ReadWrite);
    	  Attemptcnt = 0;
         Status = FALSE;
         if(NrtQObj.ReadWrite == READ_REQ)
        	 NRTResponse = TIMEOUT;
         AlarmQueueWrite(CF01);
         Log::WriteEventLog(EVENT_MODBUSCOMMFAILURE, NrtQObj.ReadWrite, NrtQObj.Cmd, 2);
         if(EnableDebug) printf("\n Couldn't read/write NRT data from DCP");
      }
      //Notify the caller task in case the same is waiting
      if(NrtQObj.Status)
         NrtQObj.Status->Write(this, Status, 0);     
   }
}
