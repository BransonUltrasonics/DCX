/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/FieldbusTask.cpp_v   1.6   21 Mar 2016 09:03:10   BKymer  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/FieldbusTask.cpp_v  $
 * 
 *    Rev 1.6   21 Mar 2016 09:03:10   BKymer
 * Swap the way Build and Revision are displayed for Hilscher 
 * General cleanup and reformatting
 * 
 *    Rev 1.5   18 Feb 2016 14:03:00   ygupta
 * Added ProtocolSpecificPreInit() call before send warmstart
 * 
 *    Rev 1.4   21 Aug 2015 05:50:24   RJamloki
 * Moved extern statements in the middle of file to the top
 * 
 *    Rev 1.3   11 Aug 2015 05:34:40   RJamloki
 * Fix for multiple event logs in case of Fieldbus Communication error
 * 
 *    Rev 1.2   02 Jul 2015 09:55:08   RJamloki
 * UpdateMailBox function fixed to use descriptive define.
 * 
 *    Rev 1.1   24 Jun 2015 08:36:04   amaurya
 * Added AskIPTimer elapsed calculation in UpdateMailBox
 * 
 *    Rev 1.0   09 Jun 2015 09:10:34   tayars
 * Initial revision.
 * 
 *    Rev 1.16   04 Mar 2015 08:22:44   amaurya
 * Logging event on communication failure
 * 
*/
#include "FieldbusTask.h"
#include "DPMInterface.h"
#include "cifXHWFunctions.h"
#include "cifXErrors.h"
#include "cifXToolkit.h"
#include "stdio.h"
#include "RTOS.h"
#include "FieldBus.h"
#include "cifXEndianess.h"
#include "GLOBALS.h"
#include "CPU.h"
#include "DEF_DEF.H"
#include "preset.h"
#include "FUNC.h"

//#define printf
#define pprintf printf

FieldbusTask * FieldbusTask::thisPtr;
UINT8 CrashFBRecvBuffer[MAX_FBEXCHANGESIZE];

extern void DumpPacket(CIFX_PACKET* ptPacket);
BOOLEAN BasicDPRAMTest(void);
//Demo function for board testing
extern SINT32 IOMirrorDemo(void);
extern SINT32 EnumBoardDemo(void);
extern SINT32 SysdeviceDemo(void);
extern SINT32 ChannelDemo(void);
extern SINT32 BlockDemo( void);
extern SINT32 SysStatusBlockDemo(void);
extern void PerformanceDemo(UINT32 ulMaxIOSize);

//#define printf pprintf

extern Fifo<UINT16> FbDiagQ;
//time incremented every ms used by tool kit
//to count delay while retrying read/write from slave card
extern volatile UINT32 s_ulTime;
//diagnostic counters to check
//that read/write to IO area is happening every milisecond
unsigned int DiagCntr1, DiagCntr2;

//flag indicates that tool kit has been initialized
BOOLEAN kitInitDone = false;
//Device instance object(describes the DPM) needs to be initialized once at power up.
PDEVICEINSTANCE PtDevInstance;

extern BOOLEAN SendDiagCmd;
extern BOOLEAN RegApp;

unsigned char CrashMailBoxbuff[1596];
//diagnostic counters and error reason. These get updated in Hilscher tool kit
extern int FbFailCounter[7];
extern int FbLastErrorReason;


void FbTaskMSTick(void) {
/*  
 *   Purpose:
 *      To invoke the FiledbusTask every millisecond.
 *    This function is called by TimerInterruptCallback() function every millisecond.
 *
 *   Entry condition:
 *    None.
 *   Exit condition:
 *    None.
 */
   s_ulTime += 1;
   if (kitInitDone && RegApp) {
      //checkCOSFlag = 0;
      if (FieldbusTask::thisPtr->FbQ.Write(0, 'a', 0))
         DiagCntr1++;
      else
         DiagCntr2++;
   }
}



void FieldbusTask::GetVersionInfo(void)
{
   CIFXHANDLE hDriver = NULL;
   SINT32 lRet    = xDriverOpen(&hDriver);
//  printf("\r\n---------- Board/Channel enumeration ---------\r\n");

   if(CIFX_NO_ERROR == lRet)
   {
      /* Driver/Toolkit successfully opened */
      UINT32          ulBoard    = 0;
      BOARD_INFORMATION tBoardInfo;// = {0};
      memset(&tBoardInfo, 0, sizeof(tBoardInfo));
      /* Iterate over all boards */
      while(CIFX_NO_ERROR == xDriverEnumBoards(hDriver, ulBoard, sizeof(tBoardInfo), &tBoardInfo))
      {
         UINT32 ulChannel    = 0;
         CHANNEL_INFORMATION tChannelInfo;// = {{0}};
         memset(&tChannelInfo, 0, sizeof(tChannelInfo));

         if(strlen( (char*)tBoardInfo.abBoardAlias) != 0){
         }

         /* iterate over all channels on the current board */
         while(CIFX_NO_ERROR == xDriverEnumChannels(hDriver, ulBoard, ulChannel, sizeof(tChannelInfo), &tChannelInfo))
         {
            pprintf(" - Channel %u:\r\n", (int)ulChannel);
            pprintf("    Firmware : %s\r\n", tChannelInfo.abFWName);
            pprintf("    Version  : %u.%u.%u build %u\r\n",
                   tChannelInfo.usFWMajor,
                   tChannelInfo.usFWMinor,
                   tChannelInfo.usFWRevision,
                   tChannelInfo.usFWBuild);
            pprintf("    Date     : %02u/%02u/%04u\r\n",
                   tChannelInfo.bFWMonth,
                   tChannelInfo.bFWDay,
                   tChannelInfo.usFWYear);
            memset(FBVersionInfo,0,sizeof(FBVersionInfo));
            sprintf(&FBVersionInfo[0],"%d.%d.%d-%d-%d/%d/%d",
                       tChannelInfo.usFWMajor,
                       tChannelInfo.usFWMinor,
                       tChannelInfo.usFWBuild,
                       tChannelInfo.usFWRevision,
                       tChannelInfo.bFWMonth,
                       tChannelInfo.bFWDay,
                       tChannelInfo.usFWYear);

            ++ulChannel;
         }
         ++ulBoard;
      }
      /* close previously opened driver */
      xDriverClose(hDriver);
   }
}



void FieldbusTask::DPMMain(void) {
/*  void DPMMain(void)
 *
 *   Purpose:
 *    To initialize PDEVICEINSTANCE object describing the Hilscher DPM. This function is called once
 *    at powerup from FieldbusTask::Run() function.
 *
 *
 *   Entry condition:
 *    None.
 *   Exit condition:
 *   None.
 */

   SINT32 TkitRetVal = CIFX_NO_ERROR;

   /* First of all initialize toolkit */
   TkitRetVal = cifXTKitInit();

   if (CIFX_NO_ERROR == TkitRetVal) {
      PtDevInstance = (PDEVICEINSTANCE) OS_Memalloc(
            sizeof(*PtDevInstance));
      OS_Memset(PtDevInstance, 0, sizeof(*PtDevInstance));

      /* Set trace level of toolkit */
      g_ulTraceLevel = TRACE_LEVEL_ERROR | TRACE_LEVEL_WARNING
            | TRACE_LEVEL_INFO | TRACE_LEVEL_DEBUG;

      /* Insert the basic device information , into the DeviceInstance structure
       for the toolkit.
       NOTE: The physical address is the one mapped using CPU:SetupChipSelect in PowerupInit function.
       The irq number is for information use only. Interrupt is currently not supported
       and ignored, so we don't need to set it */
      PtDevInstance->fPCICard = 0;
      PtDevInstance->pvOSDependent = PtDevInstance;
      PtDevInstance->ulDPMSize = 0x4000;
      PtDevInstance->fIrqEnabled = 0;
      OS_Strncpy(PtDevInstance->szName, "cifX0",
            sizeof(PtDevInstance->szName));

      PtDevInstance->pbDPM = (UINT8*) HILSCHERDPRAM_BASEADDRESS;

      /* Add the device to the toolkits handled device list */
      TkitRetVal = cifXTKitAddDevice(PtDevInstance);
      /* If it succeeded do device tests */
      if (CIFX_NO_ERROR != TkitRetVal) {
         pprintf("\n add device error. Run basic DPRAM test");
         Log::WriteEventLog(EVENT_FIELDBUSERROR, ADD_DEVICE_ERROR, TkitRetVal);
         for (;;) {
            RTOS::DelayMs(1000);
            if(BasicDPRAMTest() == FALSE)
                AlarmQueueWrite(CF03);
         }
         /* Uninitialise Toolkit, this will remove all handled boards from the toolkit and
          deallocate the device instance */
         pprintf("\n error adding device");
         cifXTKitDeinit();
      }
      else {
         pprintf(" \n Device pointer %08X", PtDevInstance);
         GetVersionInfo();
         kitInitDone = true;
      }
   }
}



/*  FieldbusTask Constructor
 *
 *  Purpose:
 *    To initialize FieldbusTask stack and Fieldbus task class variables. Called when task is created from
 *    Main() function.
 *
 *  Entry condition:
 *     FbPtr: Pointer to one of the protocols class derive from FieldBus class.
 *
 *  Exit condition:
 *    None.
 */
FieldbusTask::FieldbusTask(FieldBus * FbPtr) :
   Task("FBT", 16384), FbQ(1, "fbQ"),/* Ntfc(50, "notification"),*/ CifxPkt(30, "CifxPkt"), WaitForWCTask(1, "WFWCTask") {
   thisPtr = this;
   Fb = FbPtr;
   HDriver = NULL;
   HChannel = NULL;
   memset(SendBuffer , 0 , MAX_FBEXCHANGESIZE);
   memset(RecvBuffer, 0, MAX_FBEXCHANGESIZE);
}



void FieldbusTask::Run(void) {
/*  
 *   Purpose:
 *    This function implements the main forever loop of this task.
 *
 *   Entry condition:
 *    None.
 *   Exit condition:
 *   None.
 */
   DelayMs(500);
   UINT8 DummyChar;
   HDriver = NULL;
   HChannel = NULL;
   SINT32 Retry = NUMBER_OF_RETRIES;
   BOOLEAN SlaveDeviceOK = FALSE;
   SINT32 Ret;
   ////interrupt implementation
   //COMXNotification testNtfc;
   DPMMain();
   Ret = xDriverOpen(&HDriver);
   if (CIFX_NO_ERROR == Ret) {
      /* Driver/Toolkit successfully opened */
       Ret = xChannelOpen(NULL, "cifX0", 0, &HChannel);
      if (CIFX_NO_ERROR != Ret) {
         pprintf("Error opening Channel from FieldbusTask!");
         Log::WriteEventLog(EVENT_FIELDBUSERROR, CHANNEL_OPEN_ERROR, Ret);
         AlarmQueueWrite(CF03);
         for (;;) {
            DelayMs(100);
         }
      }
   } else {
      pprintf("Error opening Driver from FieldbusTask!");
      Log::WriteEventLog(EVENT_FIELDBUSERROR, DRIVER_OPEN_ERROR, Ret);
      AlarmQueueWrite(CF03);
      for (;;) {
         DelayMs(100);
      }
   }

   DEV_ReadHostFlags(&PtDevInstance->tSystemDevice, 0);
   DEV_ReadHostFlags((PCHANNELINSTANCE) HChannel, 0);
   //Check for device ready
   while((Retry > 0) && (SlaveDeviceOK == FALSE)){
       if(DEV_IsReady(&PtDevInstance->tSystemDevice) == FALSE){
           pprintf("\n sys device not ready");
           Retry--;
       }
       else
           SlaveDeviceOK = TRUE;
   }
   if(SlaveDeviceOK == FALSE){
       Log::WriteEventLog(EVENT_FIELDBUSERROR, SYSDEVICE_NOT_READY);
       AlarmQueueWrite(CF03);
       for (;;) {
          DelayMs(100);
       }
   }
   Retry = NUMBER_OF_RETRIES;
   SlaveDeviceOK = FALSE;
   //Check for communication channel rady
   while((Retry > 0) && (SlaveDeviceOK == FALSE)){
       if(DEV_IsReady((PCHANNELINSTANCE) HChannel) == FALSE){
           pprintf("\n communication channel not ready");
           Retry--;
       }
       else
           SlaveDeviceOK = TRUE;
   }
   if(SlaveDeviceOK == FALSE){
       AlarmQueueWrite(CF03);
       Log::WriteEventLog(EVENT_FIELDBUSERROR, COMMCHANNEL_NOT_READY);
       for (;;) {
          DelayMs(100);
       }
   }

   if (xChannelHostState(HChannel, CIFX_HOST_STATE_READY, 0, 1000)
          == CIFX_NO_ERROR){
       pprintf("\n Set host state success");
   }
   else{
       pprintf("\n Set host state failure");
   }


   if (DEV_IsRunning((PCHANNELINSTANCE)HChannel) == TRUE){
       pprintf("\n device already running send warmstart");
       DEV_RemoveChannelFiles(&PtDevInstance->tSystemDevice, ((PCHANNELINSTANCE) HChannel)->ulChannelNumber,
               DEV_TransferPacket, NULL, NULL, ".NXF");
       DEV_DeleteConfig((PCHANNELINSTANCE)HChannel);

   }
   else{
       pprintf("\n device already not running send warmstart");
   }
   Fb->ProtocolSpecificPreInit(HChannel);
   SendWarmStart();


   Fb->RegisterApp(HChannel);
   Fb->ProtocolSpecificInit(HChannel);

   for (;;) {
      FbQ.Read(this, DummyChar, -1);
      UpdateMailBox((PCHANNELINSTANCE) HChannel);
      UpdateIOImage((PCHANNELINSTANCE) HChannel);

      if (SendDiagCmd == TRUE) {
         SendDiagCmd = FALSE;
         Fb->ReadCommonStatus(HChannel);
         Fb->ReadExtendedStatus(HChannel);
         FbDiagQ.Write(this, FieldBusType, 0);
      }
      //interrupt implementation

   }
   xChannelClose(HChannel);
   xDriverClose(HDriver);
}



void FieldbusTask::SendWarmStart(void) {
/*  
 *   Purpose:
 *    This function calls the function to get protocol specific warm start packet and send it
 *    to the slave card. This function is called once at power up from FieldbusTask::Run() function.
 *   Entry condition:
 *    None.
 *   Exit condition:
 *   None.
 */
   CIFX_PACKET WSPkt;
   SINT32 Ret;
   if(/*Fb &&*/ Fb->GetWarmStartParams(&WSPkt)){
       Ret = cifXHandleWarmstartParameter(PtDevInstance, &WSPkt);
      if(Ret != 0){
          pprintf("\n error sending warm start %08X", Ret);
          Log::WriteEventLog(EVENT_FIELDBUSERROR, WARMSTART_CONFIG_ERROR, Ret);
      }
      else{
          pprintf("\n sent warm start");
      }
   }

   if (xChannelHostState(HChannel, CIFX_HOST_STATE_READY, 0, 1000)
         == CIFX_NO_ERROR){
      pprintf("\n Set host state success");
   }
   else {
      pprintf("\n Set host state failure");
   }
}



SINT32 FieldbusTask::UpdateIOImage(PCHANNELINSTANCE PtChannel) {
/*  
 *  Purpose:
 *    To read and write cyclic fieldbus data from/to DPRAM.
 *    This function is called every millisecond by FieldbusTask::Run() function.
 *
 *  Entry condition:
 *    PtChannel: Handle to the communication channel.
 *
 *  Exit condition:
 *   Returns CIFX_NO_ERROR if there is no error else returns error code.
 */

   UINT8 IOExchangeSize = Fb->IODataExchangeSize;
   SINT32 Ret = CIFX_NO_ERROR;
   UINT8 FHandshakeMode = 0;
   UINT8 Dummy;
   PIOINSTANCE PtCommIn;
   PIOINSTANCE PtCommOut;
   static UINT32 CommCheckCounter = 0;
   static BOOLEAN LoggedCommFailureEvent = TRUE;//Flag to make sure that we log events only once in case of communication failure

   if(IOExchangeSize > MAX_FBEXCHANGESIZE){
       //UpdateTPVal(95);
       IOExchangeSize = MAX_FBEXCHANGESIZE;
   }
   /* Execute a I/O data transfer */
   if (DEV_IsCommunicating(PtChannel, &Ret)){
       if(CommCheckCounter > 0){
          //pprintf("\n CF3 %d", CommCheckCounter);
       }
#ifdef FIELDBUS_DEBUG
       static int xx = 0;
       if(++xx > 1000){
           pprintf("Y");
           xx = 0;
       }
#endif
       CommCheckCounter = 0;
       LoggedCommFailureEvent = FALSE;
       /* Initialize pointer to the Input and Output process data image from the hardware */
        PtCommIn = PtChannel->pptIOInputAreas[0];
        PtCommOut = PtChannel->pptIOOutputAreas[0];

       /*------------------------------*/
       /* Handle Input data area       */
       /*------------------------------*/
       /* Read the actual II/O data handshake mode for the input areas */
       FHandshakeMode = DEV_GetIOBitstate(PtChannel, PtCommIn, 0);

       /* Check handshake mode mode */
       if (RCX_FLAGS_NONE == FHandshakeMode) {

          /* This is RCX_IO_MODE_UNCONTROLLED, copy data without using the handshake flags */
          memcpy((void*) RecvBuffer, (void*) PtCommIn->pbDPMAreaStart,
                  IOExchangeSize);
          memcpy(CrashFBRecvBuffer,RecvBuffer,sizeof(RecvBuffer));
       }
       else {
          /* This is a "controlled" mode, synchronized via the handshake flags */
          /* Check if the handshake flags are in the correct state, otherwise wait */
          /* until the state is reached and it is allowed to access the INPUT process data image */

          if (!DEV_WaitForBitState(PtChannel, PtCommIn->bHandshakeBit,
                FHandshakeMode, 0)) {
             /* Failed to get the correct handshake flag state */
             Ret = CIFX_DEV_EXCHANGE_FAILED;
          }
          else {
             //OS_Memcpy((void*)RecvBuffer, (void*)ptCommIn->pbDPMAreaStart, sizeof(RecvBuffer));
             /* Read access is allowed, copy data to user buffer */
             memcpy((void*) RecvBuffer, (void*) PtCommIn->pbDPMAreaStart,
                     IOExchangeSize);
             memcpy(CrashFBRecvBuffer,RecvBuffer,sizeof(RecvBuffer));
             /* Signal / Achnowledge -> "Read Data Done" */
             DEV_ToggleBit(PtChannel, 1UL << PtCommIn->bHandshakeBit);
          }
       }

       //if(Fb)
          Fb->UpdateInputCyclic(RecvBuffer);

       WaitForWCTask.Read(this , Dummy, -1);
      // if(Fb)
          Fb->UpdateOutputCyclic(SendBuffer);
       /*------------------------------*/
       /* Handle Output data area      */
       /*------------------------------*/

       FHandshakeMode = DEV_GetIOBitstate(PtChannel, PtCommOut, 1);

       /* Check transfer mode */
       if (RCX_FLAGS_NONE == FHandshakeMode) {
          /* This is RCX_IO_MODE_UNCONTROLLED, copy data without using the handshake flags */
          memcpy((void*) PtCommOut->pbDPMAreaStart, (void*) SendBuffer,
                  IOExchangeSize);
       }
       else {
          /* This is a "controlled" mode, synchronized via the handshake flags */
          /* Check if the handshake flags are in the correct state, otherwise wait */
          /* until the state is reached and it is allowed to access the OUTPUT process data image */
          if (!DEV_WaitForBitState(PtChannel, PtCommOut->bHandshakeBit,
                FHandshakeMode, 0)) {
             Ret = CIFX_DEV_EXCHANGE_FAILED;
          }
          else {
             /* Write data */
             memcpy((void*) PtCommOut->pbDPMAreaStart, (void*) SendBuffer,
                     IOExchangeSize);
             /* Signal -> "Write Data Done" */
             DEV_ToggleBit(PtChannel, 1UL << PtCommOut->bHandshakeBit);
          }
       }
   }
   else
   {
       FbFailCounter[6]++;//Total
       if(CommCheckCounter++ > FB_COMM_CHECK_MAXLIMIT){
          if(OpMode == AUTO)
          {
              if(LoggedCommFailureEvent == FALSE){
                  Log::WriteEventLog(EVENT_FIELDBUSERROR, COMMUNICATION_ERROR, Ret, FbLastErrorReason);
                  LoggedCommFailureEvent = TRUE;            
              }
              AlarmQueueWrite(CF03);//com failure
          }
          Fb->UpdateInputCyclic(0);
          if(BasicDPRAMTest() == FALSE)
              AlarmQueueWrite(EQ08);//fieldbus removed
          CommCheckCounter = 0;
      }
   }
   return Ret;
}



void FieldbusTask::UpdateMailBox(PCHANNELINSTANCE PtChannel) {
/*  
 *   Purpose:
 *    To read and write acyclic fieldbus data from/to DPRAM. This function first checks if there is a mailbox packet
 *    pending. If yes Reads the packet and send it to the protocol specific class for processing.
 *    This function is called every millisecond by FieldbusTask::Run() function.
 *
 *   Entry condition:
 *    PtChannel: Handle to the communication channel.
 *
 *   Exit condition:
 *   None
 */
   SINT32 Ret = CIFX_NO_ERROR;
   UINT32 ReceivePktCnt = 0;
   UINT32 SendPktCnt = 0;
   //  static bool RegApp = false;
   BOOLEAN SndPkt = TRUE;

   CIFX_PACKET RecvPkt;
   memset(&RecvPkt, 0, sizeof(RecvPkt));
   /*------------------------------*/
   /* Read the mailboxe state      */
   /*------------------------------*/
   Ret = DEV_GetMBXState(PtChannel, &ReceivePktCnt, &SendPktCnt);
   if (CIFX_NO_ERROR != Ret) {/* Unable to query the packet mailbox states */
      return;
   }
   if (ReceivePktCnt > 0){
   }

   /*------------------------------*/
   /* Process receive packets      */
   /*------------------------------*/
   while (ReceivePktCnt > 0) {
      /* Try to read a packet from the mailbox */
      Ret = DEV_GetPacket(PtChannel, &RecvPkt, sizeof(RecvPkt), 0);
      if (CIFX_NO_ERROR != Ret) {
         break;
      }
      memcpy(CrashMailBoxbuff,&RecvPkt,sizeof(CIFX_PACKET));
         Fb->ProcessMailBox(&RecvPkt);
      ReceivePktCnt--;
   } /* end while */

   /*------------------------------*/
   /* Process send packets         */
   /*------------------------------*/
   if (SndPkt && (SendPktCnt > 0)) {
      /* We can send a packt, and the card is able to accept it */
      /* TODO: handle your request packets */
      SndPkt = CifxPkt.Read(this, RecvPkt, 0);
      if(SndPkt == TRUE){
         if (CIFX_NO_ERROR != (Ret = DEV_PutPacket(PtChannel,
             (CIFX_PACKET*) &RecvPkt, 10))){
#ifdef FIELDBUS_DEBUG
             pprintf("Error sending pkt %08X Ret %08X!\r\n", LE32_TO_HOST(RecvPkt.tHeader.ulCmd), Ret);
#endif
         }
         else
         {
#ifdef FIELDBUS_DEBUG
            pprintf("\n sent %08X code %08X", LE32_TO_HOST(RecvPkt.tHeader.ulCmd),
                    LE32_TO_HOST(RecvPkt.tHeader.ulState));
#endif
         }
         SendPktCnt--;
      }
   }
   //Check for 500 ms elapsed after slave card has indicated that its IP is configured or link is up.
   if(Fb->AskIPTimer >= 0){
       if(Fb->AskIPTimer == 0){
           RecvPkt.tHeader.ulCmd = SLAVEIP_CONFIGURED_INDICATION;//Fake a command to send get ip request to slave
           Fb->ProcessMailBox(&RecvPkt);
       }
       Fb->AskIPTimer--;
   }
}



BOOLEAN BasicDPRAMTest(void) {
/*  
 *   Purpose:
 *    To check whether Hilscher slave card is alive or not. This is done by reading first four bytes
 *    from card's DPRAM. Read value is always expected to be "netX"
 *
 *   Entry condition:
 *    Called from fieldbus task when master slave Communication is not happening.
 *
 *   Exit condition:
 *   True if card is alive. If card is not alive this function returns false and fieldbus card removed
 *   is generated if the dcx system is in auto mode.
 *   None
 */
   BOOLEAN TestStat = FALSE;
   char Test[5];
   memset(Test, 0, sizeof(Test));
   memcpy(Test, (void *)HILSCHERDPRAM_BASEADDRESS, 4);
   if(strcmp(Test, CIFX_DPMSIGNATURE_FW_STR) == 0)
       TestStat = TRUE;
   return TestStat;
}

