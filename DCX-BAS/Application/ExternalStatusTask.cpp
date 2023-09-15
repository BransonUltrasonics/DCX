/*
 $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/ExternalStatusTask.cpp_v   1.5   03 Sep 2013 12:34:46   ygupta  $
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
 $Log:
 *
 * ExternalStatusTask.cpp
 *
 *  Created on: Dec 24, 2012
 *      Author: seema.sharma
 */

#include "ExternalStatusTask.h"
#include "DEF_DEF.H"
#include "StateTable.h"
#include "RTOS.h"
#include "UserIO.h"
#include "FUNC.h"

#define pprintf

#define PULSE_ON_OFF_DURATION    500000      //500ms
#define ALARM_2_PULSE_COUNT  3

ExternalStatusTask  * ExternalStatusTask::thisPtr;
BOOLEAN AlarmStatus = FALSE, TriggerStatus = FALSE, PulseOnOffFlag = FALSE;
UINT16 PulseCount = 0;

/****************************************************************************/
/* Description:-                                                            */
/* ExternalStatusTask Constructor                                                */
/****************************************************************************/
ExternalStatusTask::ExternalStatusTask() :
   Task("ExtStatus", EXTSTATUS_TASK_STACKSIZE), ExtStatusTaskQ(EXTSTATUS_TASK_FIFOSIZE, "ExtStatusQ")
{
   thisPtr = this;
}

void ExternalStatusTask::Run(void)
{
   char Ch;
   DelayMs(1000);
   while(FOREVER){  //Forever loop
      ExtStatusTaskQ.Read(this, Ch, -1);
      if(Ch == 'a'){
         AlarmStatus = TRUE;
         UserIO::SetStatusOutput(TRUE);
         PulseOnOffFlag = TRUE;
         PulseCount = 0;
      }
      else if(Ch == 't'){
         TriggerStatus = TRUE;
         PulseOnOffFlag = TRUE;
         UserIO::SetStatusOutput(TRUE);
      }
   }
}

void ExternalStatusTask::Tick(void)
{
   static UINT32 PulseDuration = 0;
   static UINT32 TrgPulseDuration = 0;
   if(AlarmStatus == TRUE){
      PulseDuration += usecPerTick;
      if(PulseDuration > PULSE_ON_OFF_DURATION){
         PulseDuration = 0;
         if(PulseOnOffFlag == TRUE){
            PulseOnOffFlag = FALSE;
            UserIO::SetStatusOutput(FALSE);
         }
         else{
            PulseCount++;
            if(PulseCount >= ALARM_2_PULSE_COUNT){
               AlarmStatus = FALSE;
               PulseOnOffFlag = FALSE;
               UserIO::SetStatusOutput(FALSE);
            }
            else{
               PulseOnOffFlag = TRUE;
               UserIO::SetStatusOutput(TRUE);
            }
         }
      }
   }
   else{
      AlarmStatus = FALSE;
      PulseDuration = 0;
   }

   if(TriggerStatus == TRUE){
      TrgPulseDuration += usecPerTick;
      if(TrgPulseDuration > PULSE_ON_OFF_DURATION){
         TrgPulseDuration = 0;
         if(PulseOnOffFlag == TRUE){
            PulseOnOffFlag = FALSE;
            UserIO::SetStatusOutput(FALSE);
         }
         else
            TriggerStatus = FALSE;
      }
   }
   else{
      TriggerStatus = FALSE;
      TrgPulseDuration = 0;
   }
}
