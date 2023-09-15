/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/FieldBus.cpp_v   1.22.1.1   29 Jul 2015 07:53:16   RJamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/FieldBus.cpp_v  $
 * 
 *    Rev 1.22.1.1   29 Jul 2015 07:53:16   RJamloki
 * Added updates for Memory Clear signal from Fieldbus in control and status words
 * 
 *    Rev 1.22.1.0   13 Mar 2014 15:54:00   rjamloki
 * FRAM is accessed through pointers. Removed compiler allocation of FRAM
 * 
 *    Rev 1.22   02 Dec 2013 07:34:18   rjamloki
 * Changes for EthIP Conformance Test.
 * 
 *    Rev 1.21   06 Nov 2013 07:02:36   rjamloki
 * Disabled link up/down event, Fieldbus event, fixed tracker issue.
 * 
 *    Rev 1.20   08 Oct 2013 06:17:38   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.19   10 Jul 2013 07:50:02   amaurya
 * Fixed powerup DCP Event Log issue.
 * 
 *    Rev 1.18   08 Jul 2013 08:52:40   amaurya
 * Fixed tracker issue for Ver3.0y.
 * 
 *    Rev 1.17   27 Jun 2013 05:54:02   amaurya
 * System crash fixes.
 * 
 *    Rev 1.16   31 May 2013 10:37:12   amaurya
 * Level II testing fixes.
 * 
 *    Rev 1.15   21 May 2013 12:25:50   amaurya
 * Code review fixes.
 * 
 *    Rev 1.14   07 May 2013 10:12:38   amaurya
 * Code review fixed.
 * 
 *    Rev 1.13   06 May 2013 09:14:58   amaurya
 * Changes to use actual frequency.
 * 
 *    Rev 1.12   30 Apr 2013 11:19:00   amaurya
 * Changes related to State machine optimization.
 * 
 *    Rev 1.11   26 Apr 2013 11:02:10   amaurya
 * Fixed traker issue for Ver2.0U.
 * 
 *    Rev 1.10   08 Apr 2013 08:22:06   amaurya
 * Fixed Tracker issue of Ver2.0P.
 * 
 *    Rev 1.9   15 Mar 2013 10:48:04   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.8   11 Mar 2013 02:24:14   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.7   18 Feb 2013 03:05:00   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.6   22 Jan 2013 10:38:10   ygupta
 * Profibus Bug Fixes
 * 
 *    Rev 1.5   17 Jan 2013 07:24:28   ygupta
 * Profibus Related changes
 * 
 *    Rev 1.4   16 Jan 2013 06:24:38   ygupta
 * Level2 Code with Profibus
*/
#include "FieldBus.h"
#include "DEF_DEF.H"
#include "LOG.h"
#include "ProfiBus.h"
#include "EthernetIP.h"
#include "cifXEndianess.h"
#include "GLOBALS.h"
#include "cifXUser.h"
#include "cifxErrors.h"
#include "rcX_Public.h"
#include "RTOS.h"
#include "StateTable.h"
#include "FUNC.h"

//#define printf
//#define pprintf
static const bool EnableDebug = false;
//Global common status block object to read the status of slave card
NETX_COMMON_STATUS_BLOCK CStat;
//Control and Status word to be sent to field bus diagnostic page
UINT16 Controlword1, Controlword2;
UINT16 Statusword1, Statusword2;

//Global flag to be initialized when application register itself on slave card for notifications.
BOOLEAN RegApp;
//Global Object contains commands from fieldbus master
FBControlSignal FBCmd;
//Global object to be assigned the status to be sent to fieldbus master
FBStatusSignal  FBStat;
//Global flag to check whether Hilscher slave card is detected or not
BOOLEAN HilscherSlaveCardDetected;
//Global variable contains the type of protocol detected in slave card
//0x0050(Profibus) 0x0080(EthernetIP)
UINT16 FieldBusType;

//IO contains the cyclic command and status message.
//Parameter read/write commands for profibus.
FieldbusIO FbusIO;//, FbusIOTemp;


/*  FieldBus Constructor
 *
 *  Purpose:
 *    This function is called once at power up when FiledBus class object is created from FindFieldBusType() function.
 *
 *  Entry condition:
 *    None.
 *
 *  Exit condition:
 *    None.
 */
FieldBus::FieldBus()
{
//   pprintf("\n fb constructor");
   memset(&FBCmd, 0, sizeof(FBCmd));
   memset(&FBStat, 0, sizeof(FBStat));
   memset(&FbusIO, 0, sizeof(FbusIO));
   IODataExchangeSize = 0;
   InitBaseIndexes();
   SetMinMaxParamForFbTable();
}

/*  void FieldBus::RegisterApp(CIFXHANDLE HChannel)
 *
 *  Purpose:
 *    This function is called once at power up from FiledBusTask::Run()
 *    function.This function registers the Host i.e. WC to get the indication of configuration or
 *    parameter change from master.
 *
 *  Entry condition:
 *    HChannel:Handle to the communication channel(returned by XChannelOpen API)
 *
 *  Exit condition:
 *    None.
 */
void FieldBus::RegisterApp(CIFXHANDLE HChannel)
{
   RCX_REGISTER_APP_REQ_T pkt;
    RCX_REGISTER_APP_CNF_T cnfReg;
    SINT32 lRet;

    pkt.tHead.ulDest = HOST_TO_LE32(0x00000020);
    pkt.tHead.ulSrc = HOST_TO_LE32(0);
    pkt.tHead.ulDestId = HOST_TO_LE32(0);
    pkt.tHead.ulSrcId = HOST_TO_LE32(0);
    pkt.tHead.ulLen = HOST_TO_LE32(0x00000000);
    pkt.tHead.ulId = HOST_TO_LE32(0x00000000);
    pkt.tHead.ulSta = HOST_TO_LE32(0);
    pkt.tHead.ulCmd = HOST_TO_LE32(RCX_REGISTER_APP_REQ);
    pkt.tHead.ulExt = HOST_TO_LE32(0);
    pkt.tHead.ulRout = HOST_TO_LE32(0);

    if (CIFX_NO_ERROR != (lRet = xChannelPutPacket(HChannel,
          (CIFX_PACKET*) &pkt, 10))) {
    	Log::WriteEventLog(EVENT_FIELDBUSERROR, APP_REGISTOR_ERROR, lRet);
       pprintf("Error sending register app %08X!\r\n", lRet);
    } else {
       RTOS::DelayMs(1);
       if (CIFX_NO_ERROR != (lRet = xChannelGetPacket(HChannel,
             sizeof(CIFX_PACKET), (CIFX_PACKET*) &cnfReg, 10))) {
    	   Log::WriteEventLog(EVENT_FIELDBUSERROR, APP_REGISTOR_ERROR, lRet);
//          pprintf("Error getting register app CNF!\r\n");
       } else {
          if (RCX_S_OK == LE32_TO_HOST(cnfReg.tHead.ulSta)
        		  || (LE32_TO_HOST(cnfReg.tHead.ulSta)) == TLR_E_APP_ALREADY_REGISTERED) {
//             pprintf("Received CNF reg app success :\r\n");
             RegApp = TRUE;
          } else{
        	  Log::WriteEventLog(EVENT_FIELDBUSERROR, APP_REGISTOR_ERROR, lRet);
//             pprintf("Received CNF reg app error %08X :\r\n",
//                   LE32_TO_HOST(cnfReg.tHead.ulSta));
          }
       }
    }
}

/*  void FieldBus::ReadCommonStatus(CIFXHANDLE HChannel)
 *
 *  Purpose:
 *    This function is called by FiledBusTask::Run() function when Fieldbus Diagnostic page polls for the status
 *    of slave. Common status includes device status(offline,Stop,Idle,Operate) plus communication
 *    information(running, bus on etc) and is common for all protocols.
 *
 *  Entry condition:
 *    HChannel:Handle to the communication channel(returned by XChannelOpen API)
 *
 *  Exit condition:
 *    None.
 */
void FieldBus::ReadCommonStatus(CIFXHANDLE HChannel)
{
   //printf("common diag block size %d!\r\n", sizeof(CStat));
   if (xChannelCommonStatusBlock(HChannel, CIFX_CMD_READ_DATA, 0,
           sizeof(CStat), &CStat) != CIFX_NO_ERROR){
//        printf("Error Reading common diag!\r\n");
   }
}

/*  FieldBus * FindFieldBusType(void)
 *
 *   Purpose:
 *    This function is called once at power up from Main() function to detect the hilscher slave card
 *    and and to find the protocol installed.
 *
 *   Entry condition:
 *    None.
 *
 *   Exit condition:
 *    Pointer to the protocol object derived from FieldBus class else NULL.
 */
FieldBus * FindFieldBusType()
{
	if(EnableDebug)
		pprintf("\n %s", __FUNCTION__);
	FieldBus * fb = NULL;
	BOOLEAN FieldBusProtocolSupporeted = FALSE;
	char buff[5];
	UINT16 DataBuff[4];
	FieldBusType = 0;
	memset(buff, 0, sizeof(buff));
	for(int i = 0; (i < NUMBER_OF_RETRIES) && (HilscherSlaveCardDetected == FALSE); i++)
	{
		//Read the 4 bytes from base Address of DPRAM
		memcpy(buff, (void *)HILSCHERDPRAM_BASEADDRESS, 4);
		if(strcmp(buff, CIFX_DPMSIGNATURE_FW_STR) == 0){	//both strings are equal
			Log::WriteEventLog(EVENT_HILSCHERCARD_DETECTED);
			HilscherSlaveCardDetected = TRUE;
			if(EnableDebug)
				pprintf("\n slave card detected");
		}
		else
			if(EnableDebug)
				pprintf(" %s ", buff);
	}

	if(HilscherSlaveCardDetected == TRUE){
	   memcpy(DataBuff, (void *)(HILSCHERDPRAM_BASEADDRESS + HW_ASSEMBLY_VALIDATION_START), sizeof(DataBuff));
		for(int i = 0; i < 4; i++){
		   pprintf(" \n hwassembly %04X", DataBuff[i]);
			if(LE16_TO_HOST(DataBuff[i]) == HW_ASSEMBLY_PROFIBUS){
				//Create the object of found protocol
				fb = new ProfiBus();
//				Log::WriteEventLog(EVENT_FIELDBUSPROTOCOL_DETECTED, HW_ASSEMBLY_PROFIBUS, COMM_IFC_PB);
				FieldBusProtocolSupporeted = TRUE;
				FieldBusType = HW_ASSEMBLY_PROFIBUS;
				break;
			}
			else if(LE16_TO_HOST(DataBuff[i]) == HW_ASSEMBLY_ETHERNETIP){
				//Create the object of found protocol
				fb  = new EthernetIP();
//				Log::WriteEventLog(EVENT_FIELDBUSPROTOCOL_DETECTED, HW_ASSEMBLY_ETHERNETIP, COMM_IFC_ETHIP);
				FieldBusProtocolSupporeted = TRUE;
				FieldBusType = HW_ASSEMBLY_ETHERNETIP;
				break;
			}
		}//for(int i = 0; i < 4; i++)
		if(FieldBusProtocolSupporeted == FALSE)
			Log::WriteEventLog(EVENT_FIELDBUSERROR, PROTOCOL_DETECT_ERROR);
	}//if(hilscherSlaveCardDetected == TRUE)
	else
		Log::WriteEventLog(EVENT_FIELDBUSERROR, CARD_DETECT_ERROR);
	return fb;
}

/* void FieldBus::ProcessControlSignal(void)
 *
 *  Purpose:
 *     To initialize global FBCmd variable based on the PZD data received from Fieldbus and to update the
 *       the inputs for WC state machine. This function is called by UpdateInputCyclic function.
 *
 *  Entry condition: None.
 *
 *  Exit condition: None:
 */
void FieldBus::ProcessControlSignal()
{
   UINT32 Tempvar = 0;
   FBCmd.EmergencyStop = FbusIO.PZDIn.Ib.EmgcyStop;//1
   FBCmd.AutoMode = FbusIO.PZDIn.Ib.ManualAuto;//1
   FBCmd.HfSwitchFeedback = FbusIO.PZDIn.Ib.FldHfSwitchX;//4
   FBCmd.GndDetect = FbusIO.PZDIn.Ib.GndDtct;//1
   FBCmd.AmpProfiling = FbusIO.PZDIn.Ib.AmplitudeProfExt;//1
   FBCmd.StartUs = FbusIO.PZDIn.Ib.On;//1
   FBCmd.Rst = FbusIO.PZDIn.Ib.Rst;//1
   FBCmd.EnableWeldFunction = FbusIO.PZDIn.Ib.Fct;//1
   FBCmd.EnableHornFunction = FbusIO.PZDIn.Ib.SFct;//1
   FBCmd.HornFunctionNumber = FbusIO.PZDIn.Ib.FldSFctX; //3
   FBCmd.SelParmeterSet = FbusIO.PZDIn.Ib.FldParaSetNbrX;
   FBCmd.MemoryClear = FbusIO.PZDIn.Ib.MCLR;
   if(OpMode == AUTO){
	   if((FBCmd.SelParmeterSet != FBLoadPresetNumber)/* && (FBCmd.SelParmeterSet != 0)*/)
	   {
		  FBLoadPresetNumber = FBCmd.SelParmeterSet;
		  CheckForFBLoadNewPreset = TRUE;
	   }
   }
   if(FBCmd.Rst == TRUE)
      Tempvar |= FBRESET;
   if((OpMode == AUTO) && FBCmd.EnableWeldFunction && FBCmd.StartUs && !FBCmd.Rst){
      FBWeldFunction = TRUE;
      Tempvar |= FBCYCLE_START;
   }
   else{
      FBWeldFunction = FALSE;
   }

   if((OpMode == AUTO) && FBCmd.EnableHornFunction && FBCmd.StartUs && !FBCmd.Rst){
        FBHornFunction = TRUE;
        if(FBCmd.HornFunctionNumber == HFSEEK)
        	Tempvar |= FBSEEK;
        if(FBCmd.HornFunctionNumber == HFTEST)
        	Tempvar |= FBTEST;
        if(FBCmd.HornFunctionNumber == HFSCAN)
        	Tempvar |= FBSCAN;
     }
     else{
        FBHornFunction = FALSE;
        
	 }
   if(WebIOEnabled == FALSE)
	 ControlVariable = (Tempvar | (ControlVariable & ~FBMASK));


//   if(memcmp(&FbusIOTemp.PZDIn, &FbusIO.PZDIn, sizeof(FbusIO.PZDIn))){
//      memcpy(&FbusIOTemp.PZDIn, &FbusIO.PZDIn, sizeof(FbusIO.PZDIn));
//      printf("\n %s PS %d ES %d AM %d HFSF %d GD %d AP %d Us %d Rst %d EWF %d EHF %d HFunc %d",__FUNCTION__,
//            FBCmd.SelParmeterSet, FBCmd.EmergencyStop, FBCmd.AutoMode, FBCmd.HfSwitchFeedback, FBCmd.GndDetect, FBCmd.AmpProfiling,
//            FBCmd.StartUs, FBCmd.Rst , FBCmd.EnableWeldFunction, FBCmd.EnableHornFunction, FBCmd.HornFunctionNumber);
//   }

   if(FBCmd.EmergencyStop == TRUE){
	   FBStat.EmergencyStopActive = 1;
	   AlarmQueueWrite(HW06);
   }
}


/* void FieldBus::PrepareStatusSignal(void)
 *
 *  Purpose:
 *    To initialize global FbusIO PZD(STW) status field to be sent to Fieldbus and to determine the operating mode
 *    change. This function is called by UpdateOutputCyclic function.
 *
 *  Entry condition: None
 *
 *  Exit condition: None:
 */
void FieldBus::PrepareStatusSignal()
{
   SINT16 CurrIndx;
   	//Mode change allowed after powerup but report auto mode by default.
   CurrIndx = CurrentIndex;
   if((CurrIndx == READYSTATE_IDX) || (CurrIndx == ERROR_IDX)){
	   FBStat.AutoModeStaus = (FBCmd.AutoMode && ((CommIfcAccessToken == COMM_IFC_NO_IFC) || (CommIfcAccessToken == COMM_IFC_FBB)));
	   if(FBStat.AutoModeStaus)
		   OpMode = AUTO;
	   else
		   OpMode = MANUAL;
   }

   FbusIO.PZDOut.Ob.EmgcyStopError = FBStat.EmergencyStopActive;
   FbusIO.PZDOut.Ob.TempError = FBStat.TemperatureError;
   FbusIO.PZDOut.Ob.HfSwitchError =  FBStat.RfSwitchingError;
   FbusIO.PZDOut.Ob.FldHfSwitchX  = FBStat.HornNumber;
   FbusIO.PZDOut.Ob.FldParaSetNbrX =  FBStat.SelectedParamSet;
   FbusIO.PZDOut.Ob.ParaSetChangeAck =  FBStat.ParameterChangeDone;
   FbusIO.PZDOut.Ob.ManualAuto =  FBStat.AutoModeStaus;
   FbusIO.PZDOut.Ob.FctStopMode = FBStat.StopMode;
   FbusIO.PZDOut.Ob.Off = FBStat.ReadyForUs;
   FbusIO.PZDOut.Ob.On =  FBStat.UsActive;
   FbusIO.PZDOut.Ob.Ok = FBStat.CycleOK;
   FbusIO.PZDOut.Ob.MCLR = FBStat.MemoryClearStatus;

   if(FBStat.Error & BIT((OL >> 8)))
	   FbusIO.PZDOut.Ob.OLAlarm = 1;
   else
	   FbusIO.PZDOut.Ob.OLAlarm = 0;

   if(FBStat.Error & BIT((TP >> 8)))
	   FbusIO.PZDOut.Ob.TPAlarm = 1;
   else
	   FbusIO.PZDOut.Ob.TPAlarm = 0;

   if(FBStat.Error & BIT((NO >> 8)))
	   FbusIO.PZDOut.Ob.NOAlarm = 1;
   else
	   FbusIO.PZDOut.Ob.NOAlarm = 0;

   if(FBStat.Error & BIT((LM >> 8)))
	   FbusIO.PZDOut.Ob.LMAlarm = 1;
   else
	   FbusIO.PZDOut.Ob.LMAlarm = 0;

   if(FBStat.Error & BIT((CU >> 8)))
	   FbusIO.PZDOut.Ob.CUAlarm = 1;
   else
	   FbusIO.PZDOut.Ob.CUAlarm = 0;

   if(FBStat.Error & BIT((HW >> 8)))
	   FbusIO.PZDOut.Ob.HWAlarm = 1;
   else
	   FbusIO.PZDOut.Ob.HWAlarm = 0;

   if(FBStat.Error & BIT((CF >> 8)))
	   FbusIO.PZDOut.Ob.CFAlarm = 1;
   else
	   FbusIO.PZDOut.Ob.CFAlarm = 0;

   if(FBStat.Error & BIT((NC >> 8)))
	   FbusIO.PZDOut.Ob.NCAlarm = 1;
   else
	   FbusIO.PZDOut.Ob.NCAlarm = 0;

   if(FBStat.Error & BIT((EQ >> 8)))
	   FbusIO.PZDOut.Ob.EQAlarm = 1;
   else
	   FbusIO.PZDOut.Ob.EQAlarm = 0;

   if(FBStat.Error & BIT((WA >> 8)))
	   FbusIO.PZDOut.Ob.WAAlarm = 1;
   else
	   FbusIO.PZDOut.Ob.WAAlarm = 0;

   if(FBStat.Error & BIT((CM >> 8)))
	   FbusIO.PZDOut.Ob.CMAlarm = 1;
   else
	   FbusIO.PZDOut.Ob.CMAlarm = 0;

   if(FBStat.Error & BIT((SE >> 8)))
	   FbusIO.PZDOut.Ob.SEAlarm = 1;
   else
	   FbusIO.PZDOut.Ob.SEAlarm = 0;

//   if(memcmp(&FbusIOTemp.PZDOut, &FbusIO.PZDOut, sizeof(FbusIO.PZDOut))){
//         memcpy(&FbusIOTemp.PZDOut, &FbusIO.PZDOut, sizeof(FbusIO.PZDOut));
//         printf("\n %s PS PZDOut %04X %04X",__FUNCTION__,
//        		 FbusIO.PZDOut.PZDu16Out.ZSWOut1, FbusIO.PZDOut.PZDu16Out.ZSWOut2);
//    }
   ExternalControl.Amplitude = RtReceiveData.AmplitudeOut;
   ExternalControl.AmplitudeSet = RtReceiveData.NominalAmplitude;
   ExternalControl.Current = RtReceiveData.Current;
   ExternalControl.Power = RtReceiveData.PowerOut;
   ExternalControl.Phase = (RtReceiveData.Phase * 180) / MAXDCPADCRAWVALUE;
   ExternalControl.PSV = RtReceiveData.PWM;
   ExternalControl.Frequency = RtReceiveData.FreqOut;
   ExternalControl.Temperature = RtReceiveData.Temperature;
}


/*  void FieldBus::ProtocolSpecificInit(CIFXHANDLE HChannel)
 *
 *  Purpose:
 *    This function is called once at power up from FiledBusTask::Run()
 *    function if the derived class from Fieldbus has not implemented the same. Currently it does nothing
 *    but the function body is needed to make the code compilable.
 *
 *  Entry condition:
 *    HChannel:Handle to the communication channel(returned by XChannelOpen API)
 *
 *  Exit condition:
 *    None.
 */
void FieldBus::ProtocolSpecificInit(CIFXHANDLE HChannel)
{
	HChannel = 0;
}

/*  void FieldBus::CheckForRfswitchfeedback()
 *
 *  Purpose:
 *    To return the currently selected horn number on RFSwitch. This function is
 * called by Statemachine.
 *
 *  Entry condition:
 *    None
 *
 *  Exit condition:
 *    Currently selected horn on RFSwitch.
 */
UINT16 FieldBus::CheckForRfswitchfeedback()
{
  return FBCmd.HfSwitchFeedback;
}

/*  void FieldBus::SetRfSelect(UINT16 HornNumber)
 *
 *  Purpose:
 *    To send command to RF switch to select a horn.
 *
 *  Entry condition:
 *    Sel: The horn number to be selected on RF Switch.
 *
 *  Exit condition:
 *    None
 */
void FieldBus::SetRfSelect(UINT16 HornNumber)
{
	if(Sysconfig->RFSwitch.RFOutputType == UNCODED)
		HornNumber = (UINT16)(BIT((HornNumber - 1)));
	FBStat.HornNumber = HornNumber;
}

/*  void FieldBus::GetExternalAmplitude()
 *
 *  Purpose:
 *    To get the amplitude control value from Fieldbus.
 *    This function is called by Statemachine
 *  Entry condition:
 *    None
 *
 *  Exit condition:
 *    None
 */
SINT16 FieldBus::GetExternalAmplitude()
{
   //expected from Fieldbus 10 to 100
   SINT16 ExtAmp = ExternalControl.ExtAmplitudeSet;
   if (ExternalControl.ExtAmplitudeSet < AMP_IN_MIN)
      ExtAmp = AMP_IN_DEFAULT;
   else if (ExternalControl.ExtAmplitudeSet > AMP_IN_MAX)
      ExtAmp = AMP_IN_MAX;

   return ExtAmp;
}

/*  void FieldBus::GetExternalFrequencyOffset()
 *
 *  Purpose:
 *    To get the frequency offset value from fieldbus.
 *    This function is called by Statemachine
 *  Entry condition:
 *    None
 *
 *  Exit condition:
 *    None
 */
SINT16 FieldBus::GetExternalFrequencyOffset()
{
   SINT16 ExtFreq = 0;
   //expected from Fieldbus
   //-500 to 500 //20K
   //-750 to 750//30K
   //-1000 to 1000//40k
   if (RunningPreset.WcParam.FreqOffsetFlag == OFF)//Internal offset option selected. Ignore fieldbus value.
      ExtFreq = RunningPreset.WcParam.FreqOffset;
   else{
	   ExtFreq = (ExternalControl.ExtFrequencyOffset *  FREQ_SCALING_FACTOR);
      //verify the frequency specific range
      if ((ExtFreq < ParameterSet[Preset_MIN].WcParam.FreqOffset)
            || (ExtFreq > ParameterSet[Preset_MAX].WcParam.FreqOffset))
         ExtFreq = 0;
   }

   return ExtFreq;
}

/*  void FieldBus::CheckForMemoryClear()
 *
 *  Purpose:
 *    To get the memory clear signal status value from Fieldbus.
 *    This function is called by Statemachine
 *  Entry condition:
 *    StateMachine.
 *
 *  Exit condition:
 *    Memory Clear signal status is returned.
 */
BOOLEAN FieldBus::CheckForMemoryClear()
{
	return FBCmd.MemoryClear;
}

/*  void FieldBus::GetFaultData(UINT32 FaultCode)
 *
 *  Purpose:
 *    To get the fault data from slave card using EIP service. Right now its used only
 *    to get the Address conflict data. Called from website on url ACDInfo32923646.html.
 *
 *  Entry condition:
 *    FaultCode- The fault for which data is to be retrieved.
 *
 *  Exit condition:
 *    None.
 */
void FieldBus::GetFaultData(UINT32 FaultCode)
{
   FaultCode = 0;
}
