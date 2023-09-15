/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/FieldbusTask.cpp_v   1.17   Feb 08 2017 17:09:32   EGUAJARDO  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/FieldbusTask.cpp_v  $
 * 
 *    Rev 1.17   Feb 08 2017 17:09:32   EGUAJARDO
 * The fieldbus loss communication error is now shown when the DCX is not cycling.
 * 
 *    Rev 1.16   04 Mar 2015 08:22:44   amaurya
 * Logging event on communication failure
 * 
 *    Rev 1.15   02 Dec 2013 07:34:18   rjamloki
 * Changes for EthIP Conformance Test.
 * 
 *    Rev 1.14   06 Nov 2013 07:02:36   rjamloki
 * Disabled link up/down event, Fieldbus event, fixed tracker issue.
 * 
 *    Rev 1.13   11 Oct 2013 04:11:30   rjamloki
 * DPRAM wait states changed to 20. Critical section removed from watchdog feed.
 * 
 *    Rev 1.12   08 Oct 2013 07:52:24   rjamloki
 * DPRAM wait state changes after power up.
 * 
 *    Rev 1.11   08 Oct 2013 06:17:36   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.10   01 Oct 2013 03:16:20   ygupta
 * Change for fieldbus version.
 * 
 *    Rev 1.9   17 Sep 2013 04:14:00   rjamloki
 * Added Task Manager again and reduced time for WatchDogTimer.
 * 
 *    Rev 1.8   03 Sep 2013 12:35:44   ygupta
 * Moved to tip from 1.6.1.5.1.0
 * 
 *    Rev 1.6.1.5.1.0   24 Aug 2013 17:14:00   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.6.1.5   02 Aug 2013 10:55:44   amaurya
 * Coverty fixes.
 * 
 *    Rev 1.6.1.3   27 Jun 2013 05:54:36   amaurya
 * System crash fixes.
 * 
 *    Rev 1.6.1.2   31 May 2013 10:37:14   amaurya
 * Level II testing fixes.
 * 
 *    Rev 1.6.1.1   22 Apr 2013 11:29:44   amaurya
 * Fixed tracker issue of Ver2.0S
 * 
 *    Rev 1.6.1.0   20 Mar 2013 09:36:50   ygupta
 * RF Switch Changes, Requirement Changes
 * 
 *    Rev 1.7   20 Mar 2013 08:17:10   ygupta
 * RF Switch Changes, Requirement Changes
 * 
 *    Rev 1.6   11 Mar 2013 02:24:34   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.5   18 Feb 2013 03:05:26   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.4   22 Jan 2013 10:38:14   ygupta
 * Profibus Bug Fixes
 * 
 *    Rev 1.3   16 Jan 2013 06:25:06   ygupta
 * Level2 Code with Profibus
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

//extern void IntHandlerExternal(int level, void * frame);
extern void DumpPacket(CIFX_PACKET* ptPacket);
BOOLEAN BasicDPRAMTest();
//Demo function for board testing
extern SINT32 IOMirrorDemo(void);
extern SINT32 EnumBoardDemo(void);
extern SINT32 SysdeviceDemo();
extern SINT32 ChannelDemo();
extern SINT32 BlockDemo( void);
extern SINT32 SysStatusBlockDemo();
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

extern BOOLEAN TokenAvailable;

unsigned char CrashMailBoxbuff[1596];

/*  void FbTaskMSTick(void)
 *
 *   Purpose:
 *      To invoke the FiledbusTask every millisecond.
 *    This function is called by TimerInterruptCallback() function every millisecond.
 *
 *   Entry condition:
 *    None.
 *   Exit condition:
 *    None.
 */
void FbTaskMSTick() {
   //static int checkCOSFlag = 0;
   s_ulTime += 1;
   //if(++checkCOSFlag >= 500)
   //{
   //	cifXTKitCyclicTimer();
   //	checkCOSFlag = 0;
   //}
   if (kitInitDone && RegApp) {
         //checkCOSFlag = 0;
         if (FieldbusTask::thisPtr->FbQ.Write(0, 'a', 0))
            DiagCntr1++;
         else
            DiagCntr2++;
   }
}

void FieldbusTask::GetVersionInfo()
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

//	  printf("Found Board %s\r\n", tBoardInfo.abBoardName);
	  if(strlen( (char*)tBoardInfo.abBoardAlias) != 0){
//		printf(" Alias        : %s\r\n", tBoardInfo.abBoardAlias);
	  }

//	  printf(" DeviceNumber : %u\r\n", (int)tBoardInfo.tSystemInfo.ulDeviceNumber);
//	  printf(" SerialNumber : %u\r\n", (int)tBoardInfo.tSystemInfo.ulSerialNumber);
//	  printf(" Board ID     : %u\r\n", (int)tBoardInfo.ulBoardID);
//	  printf(" System Error : 0x%08X\r\n", (int)tBoardInfo.ulSystemError);
//	  printf(" Channels     : %u\r\n", (int)tBoardInfo.ulChannelCnt);
//	  printf(" DPM Size     : %u\r\n", (int)tBoardInfo.ulDpmTotalSize);

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
//		Log::WriteEventLog(EVENT_FBSLAVECARDVERSION, ((tChannelInfo.usFWMajor << 16) | (tChannelInfo.usFWMinor)) ,
//				((tChannelInfo.usFWRevision << 16) | (tChannelInfo.usFWBuild)),
//				((tChannelInfo.bFWMonth << 24) | (tChannelInfo.bFWDay << 16) | (tChannelInfo.usFWYear)));
		memset(FBVersionInfo,0,sizeof(FBVersionInfo));
		sprintf(&FBVersionInfo[0],"%d.%d.%d-%d-%d/%d/%d",
				   tChannelInfo.usFWMajor,
				   tChannelInfo.usFWMinor,
				   tChannelInfo.usFWRevision,
				   tChannelInfo.usFWBuild,
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
//  printf(" State = 0x%08X\r\n", (int)lRet);
//  printf("----------------------------------------------------\r\n");
}

//void NotCallBack(UINT32 UlNotification, UINT32 UlDataLen,
//      void * Data, void * UserData) {
//   pprintf("\n %s %d %d %x", __FUNCTION__, UlNotification, UlDataLen,
//         UserData);
//   COMXNotification Ntf = { 0, 0 };
//   Ntf.Type = UlNotification;
//   if (Data)
//      Ntf.NMbx = ((CIFX_NOTIFY_RX_MBX_FULL_DATA_T *) Data)->ulRecvCount;
//
//   FieldbusTask::thisPtr->Ntfc.Write(0, Ntf, 0);
//
//}

/*  void DPMMain()
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
void FieldbusTask::DPMMain() {

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
//      if(PtDevInstance->fIrqEnabled)
//		  CPU::InitExternalInterrupt(IntHandlerExternal, intEPFR6, 4, 0,
//				CpuIntModeLevelLow, false);
      OS_Strncpy(PtDevInstance->szName, "cifX0",
            sizeof(PtDevInstance->szName));

      //PtDevInstance->pfnHwIfRead   = DPM_Read;
      //PtDevInstance->pfnHwIfWrite  = DPM_Write;
      PtDevInstance->pbDPM = (UINT8*) HILSCHERDPRAM_BASEADDRESS;

      /* Add the device to the toolkits handled device list */
      TkitRetVal = cifXTKitAddDevice(PtDevInstance);
      //PFN_NOTIFY_CALLBACK NCallBack = NotCallBack;
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
      } else {
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

/*  void FieldbusTask::Run(void)
 *
 *   Purpose:
 *    This function implements the main forever loop of this task.
 *
 *   Entry condition:
 *    None.
 *   Exit condition:
 *   None.
 */
void FieldbusTask::Run() {
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
      /*	ntfc.Read(this , testNtfc, -1);
       {
       switch(testNtfc.type)
       {
       case CIFX_NOTIFY_RX_MBX_FULL:
       for(ulCycle = 0; ulCycle < testNtfc.NMbx; ulCycle++)
       {
       if(CIFX_NO_ERROR != (lRet = xChannelGetPacket(HChannel, sizeof(recPkt), (CIFX_PACKET*)&recPkt, 10)) )
       {
       printf("Error CIFX_NOTIFY_RX_MBX_FULL !\r\n");
       }
       else
       {
       / *
       UINT32   ulDest;
       UINT32   ulSrc;
       UINT32   ulDestId;
       UINT32   ulSrcId;
       UINT32   ulLen;
       UINT32   ulId;
       UINT32   ulState;
       UINT32   ulCmd;
       UINT32   ulExt;
       UINT32   ulRout;
       * /

       break;
       case CIFX_NOTIFY_TX_MBX_EMPTY:
       break;
       case CIFX_NOTIFY_PD0_IN:
       break;
       case CIFX_NOTIFY_PD1_IN:
       break;
       case CIFX_NOTIFY_PD0_OUT:
       break;
       case CIFX_NOTIFY_PD1_OUT:
       break;
       case CIFX_NOTIFY_SYNC:
       break;
       };*/

   }
   xChannelClose(HChannel);
   xDriverClose(HDriver);
}

/*  void FieldbusTask::SendWarmStart(void)
 *
 *   Purpose:
 *    This function calls the function to get protocol specific warm start packet and send it
 *    to the slave card. This function is called once at power up from FieldbusTask::Run() function.
 *   Entry condition:
 *    None.
 *   Exit condition:
 *   None.
 */
void FieldbusTask::SendWarmStart() {
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
   else{
      pprintf("\n Set host state failure");
   }

   if(xChannelBusState(HChannel, CIFX_BUS_STATE_ON, 0, 3000) == CIFX_NO_ERROR){
     // pprintf("\n Set Bus state success");
   }
   else{
      //pprintf("\n Set Bus state failure");
   }
   //lRet = cifXTkHWFunctions_WaitUntilCommunicating( ptChannel);
}

/*  SINT32 FieldbusTask::UpdateIOImage(PCHANNELINSTANCE PtChannel)
 *
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
SINT32 FieldbusTask::UpdateIOImage(PCHANNELINSTANCE PtChannel) {

   UINT8 IOExchangeSize = Fb->IODataExchangeSize;
   SINT32 Ret = CIFX_NO_ERROR;
   UINT8 FHandshakeMode = 0;
   UINT8 Dummy;
   PIOINSTANCE PtCommIn;
   PIOINSTANCE PtCommOut;
   static UINT32 CommCheckCounter = 0;

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
	   } else {
		  /* This is a "controlled" mode, synchronized via the handshake flags */
		  /* Check if the handshake flags are in the correct state, otherwise wait */
		  /* until the state is reached and it is allowed to access the INPUT process data image */

		  if (!DEV_WaitForBitState(PtChannel, PtCommIn->bHandshakeBit,
				FHandshakeMode, 0)) {
			 /* Failed to get the correct handshake flag state */
			 Ret = CIFX_DEV_EXCHANGE_FAILED;
		  } else {
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
	   } else {
		  /* This is a "controlled" mode, synchronized via the handshake flags */
		  /* Check if the handshake flags are in the correct state, otherwise wait */
		  /* until the state is reached and it is allowed to access the OUTPUT process data image */
		  if (!DEV_WaitForBitState(PtChannel, PtCommOut->bHandshakeBit,
				FHandshakeMode, 0)) {
			 Ret = CIFX_DEV_EXCHANGE_FAILED;
		  } else {
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
	   extern int FbFailCounter[7];
	   extern int FbLastErrorReason;
	   FbFailCounter[6]++;//Total

      if(CommCheckCounter++ > FB_COMM_CHECK_MAXLIMIT){
    	  if(OpMode == AUTO || (OpMode == MANUAL && TokenAvailable == FALSE))
    	  {
    		  Log::WriteEventLog(EVENT_FIELDBUSERROR, FbLastErrorReason, Ret, FbFailCounter[6]);
    		  Log::WriteEventLog(EVENT_FIELDBUSERROR, FbFailCounter[0], FbFailCounter[1]);
    		  Log::WriteEventLog(EVENT_FIELDBUSERROR, FbFailCounter[2], FbFailCounter[3]);
    		  Log::WriteEventLog(EVENT_FIELDBUSERROR, FbFailCounter[4], FbFailCounter[5]);
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

/*  void FieldbusTask::UpdateMailBox(PCHANNELINSTANCE PtChannel)
 *
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
void FieldbusTask::UpdateMailBox(PCHANNELINSTANCE PtChannel) {
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
//      printf("Unable to query the packet mailbox states %d", Ret);
      return;
   }
   if (ReceivePktCnt > 0){
//      printf("\n %d %d", ReceivePktCnt, SendPktCnt);
   }

   /*------------------------------*/
   /* Process receive packets      */
   /*------------------------------*/
   while (ReceivePktCnt > 0) {
      /* Try to read a packet from the mailbox */
      Ret = DEV_GetPacket(PtChannel, &RecvPkt, sizeof(RecvPkt), 0);
      if (CIFX_NO_ERROR != Ret) {
//         printf(" Error reading a packet ");
         break;
      }
      //if(Fb)
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
}

BOOLEAN BasicDPRAMTest() {
   BOOLEAN TestStat = FALSE;
   char Test[5];
   memset(Test, 0, sizeof(Test));
   memcpy(Test, (void *)HILSCHERDPRAM_BASEADDRESS, 4);
   if(strcmp(Test, CIFX_DPMSIGNATURE_FW_STR) == 0)
	   TestStat = TRUE;
   return TestStat;
}

