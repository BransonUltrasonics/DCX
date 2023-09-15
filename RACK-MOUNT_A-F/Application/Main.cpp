/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/Main.cpp_v   1.6   09 Feb 2016 16:22:16   tayars  $
*/
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*   Copyright (c) Branson Ultrasonics Corporation, 1995,96, 2009        	*/
/*   This program is the property of Branson Ultrasonics Corporation      	*/
/*   Copying of this software is expressly forbidden, without the prior     */
/*   written consent of Branson Ultrasonics Corporation.                    */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*************************                         **************************/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/Main.cpp_v  $
 * 
 *    Rev 1.6   09 Feb 2016 16:22:16   tayars
 * Set default power to percentage.
 * Removed the IP getting set to default in "SetSystemConfigurationDefaults()". This will make it so the system config webpage will no longer set IP settings back to default.
 * 
 *    Rev 1.5   28 Jan 2016 01:53:52   ygupta
 * SetSystemConfigurationDefaults() modified to store "Watt" as Default unit of power result.
 * 
 *    Rev 1.4   24 Jul 2015 08:37:00   sesharma
 * Function pointer assignments as per board type are added for common binary.
 * 
 *    Rev 1.3   02 Jul 2015 09:56:22   RJamloki
 * Pulse train frequency going to HW watchdog is changed to 1Hz.
 * 
 *    Rev 1.2   29 Jun 2015 03:20:26   rjamloki
 * Removed code in SetupLCDBoardSensor() function which was supposed to send reset to LCD board. It’s done by ReceiveCommandHandler.cpp Run () now.
 * 
 *    Rev 1.1   24 Jun 2015 08:36:38   amaurya
 * Added InitExternalWatchDogTrigger() to start feeding the HW watch dog.
 * 
 *    Rev 1.0   09 Jun 2015 09:10:32   tayars
 * Initial revision.
 * 
 *    Rev 1.38.1.2   24 Mar 2014 09:26:56   rjamloki
 * Fixed Field Service event Data 1 & Data 2 to write 32 bit number.
 * 
 */

#include "CyGOS.h"
#include "LED.h"
#include "InputTask.h"
#include "DebugNew.h"
#include "stdio.h"
#include "Board.h"
#include "MIATask.h"
#include "WCTask.h"
#include "FUNC.h"
#include "Log.h"
#include "SysConfig.h"
#include "Website.h"
#include "UploadFirmWareTask.h"
#include "GLOBALS.h"
#include "CpuDefs.h"
#include "FirmWareUpgrade.h"
#include "ModBusRcvTask.h"
#include "ModBusSendTask.h"
#include "NRTModbusTask.h"
#include "FieldbusTask.h"
#include "PasscodeConfig.h"
#include "portable.h"
#include "WelderSearchTask.h"
#include "TaskManager.h"
#include "ReceiveCommandHandler.h"
#include "RTOS.h"
#include "ShutDownTask.h"
#include "ExternalStatusTask.h"
#include "ProfiBus.h"
#include "FEC.h"
#include "MII.h"
#include "DcxManagerHandler.h"
#include "BDM08.h"

// Define for PIN AB14
#define CHECKIFAB14HIGH   	(MCF_EPORT_EPPDR & MCF_EPORT_EPPDR_EPPD3)

#define PHYNAME_LAN8701				"LAN8701"
#define PHYNAME_KSZ8863RLL			"KSZ8863RLL"

UINT16 CompressedWebPageCRC = 0;
extern bool PrintHighestTaskLoad;
extern bool PrintCurrentTaskLoad;

void SetLeds(int slow, int fast)
{
   LED::Set(1, slow);
   LED::Set(2, fast);
}

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  Lookup Tables.  The "hardware" method of computing CRCs involves bit   */
/*  manipulations, which is very inefficient for a software computation.   */
/*  Instead of computing the CRC bit-by-bit, a 256-element lookup table    */
/*  can be used to perform the equivalent of 8 bit operations at a time.   */
/*  (This is described in "Byte-wise CRC Calculations" in IEEE Micro,      */
/*  June 1983, pp. 40-50.)  For a CRC-16, the lookup table consists of 256 */
/*  2-byte WORDs (see below, or the CRC16.PAS unit for the actual table,   */
/*  or the CRCTable program for computation of the lookup table for the    */
/*  x16 + x15 + x2 + 1 generator polynomial):                              */
/*                                                                         */
/*-------------------------------------------------------------------------*/
const UINT16 CRCTable[256] =
 {0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241, 0xC601, 0x06C0,
  0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440, 0xCC01, 0x0CC0, 0x0D80, 0xCD41,
  0x0F00, 0xCFC1, 0xCE81, 0x0E40, 0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0,
  0x0880, 0xC841, 0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
  0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41, 0x1400, 0xD4C1,
  0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641, 0xD201, 0x12C0, 0x1380, 0xD341,
  0x1100, 0xD1C1, 0xD081, 0x1040, 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1,
  0xF281, 0x3240, 0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
  0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41, 0xFA01, 0x3AC0,
  0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840, 0x2800, 0xE8C1, 0xE981, 0x2940,
  0xEB01, 0x2BC0, 0x2A80, 0xEA41, 0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1,
  0xEC81, 0x2C40, 0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
  0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041, 0xA001, 0x60C0,
  0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740,
  0xA501, 0x65C0, 0x6480, 0xA441, 0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0,
  0x6E80, 0xAE41, 0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
  0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41, 0xBE01, 0x7EC0,
  0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40, 0xB401, 0x74C0, 0x7580, 0xB541,
  0x7700, 0xB7C1, 0xB681, 0x7640, 0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0,
  0x7080, 0xB041, 0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
  0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440, 0x9C01, 0x5CC0,
  0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40, 0x5A00, 0x9AC1, 0x9B81, 0x5B40,
  0x9901, 0x59C0, 0x5880, 0x9841, 0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1,
  0x8A81, 0x4A40, 0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
  0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641, 0x8201, 0x42C0,
  0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040};

UINT16 ByteCRC(UINT8 * Ptr, UINT32 Length)
/*****************************************************************************/
/*                                                                           */
/*   The following is a little cryptic (but executes very quickly).          */
/*   The algorithm is as follows:                                            */
/*   1. exclusive-or the input byte with the low-order byte of               */
/*      the CRC register to get an INDEX                                     */
/*   2. shift the CRC register eight bits to the right                       */
/*   3. exclusive-or the CRC register with the contents of                   */
/*      Table[INDEX]                                                         */
/*   4. repeat steps 1 through 3 for all bytes}                              */
/*                                                                           */
/*****************************************************************************/
{
   UINT16 CRCRegister = 0;
   UINT8  Index;
   while(Length-- > 0){
      Index = (UINT8)((CRCRegister ^ * Ptr++) & 0xff);
      CRCRegister = CRCRegister >> 8;
      CRCRegister = CRCRegister ^ CRCTable[Index];
   }
   return(CRCRegister);
}

/*   void SetDHCPServerIPDefaults(void)
 *
 *   Purpose:
 *		This function sets the defaults for DHCP server IP setup.
 *		It fixes the static IP of WC on DCP interface.
 *    	This function is called from SetSystemConfigurationDefaults () and ColdStart() function.
 *
 *   Entry condition:
 *		None.
 *
 *   Exit condition:
 *		None.
 */
void SetDHCPServerIPDefaults(void)
{
	DhcpServerConfiguration Config;
	memset(&Config, 0, sizeof(Config));
	memcpy(Config.id, "DHSV", DEFDHCPSERVERID_LEN);
	Config.length = sizeof(Config);
	Config.poolSize  = DEFAULTPOOLSIZE;
	Config.poolStart = MakeIP(DEFAULTPOOLSTARTDHCPSERVER);
	Config.netmask   = ClassC();
	Config.gateway   = MakeIP(DEFAULTGATEWAYDHCPSERVER);
	Config.wins = 0;
	Config.dns  = MakeIP(DEFAULTIPDHCPSERVER);
	Config.leaseTime   = DEFAULTLEASETIME;
	Config.renewTime   = DEFAULTRENEWTIME;
	Config.rebindTime  = DEFAULTREBINDTIME;
	Sysconfig->SystemIP = MakeIP(DEFAULTIPDHCPSERVER);
	Sysconfig->DhcpConfig.dns = MakeIP(DEFAULTIPDHCPSERVER);
	//Save It in FRAM
	memcpy(&Sysconfig->DhcpConfig, &Config, sizeof(Config));
}

/*   void SetStaticIPDefaults(void)
 *
 *   Purpose:
 *		It fixes the static IP of WC on DCP interface.
 *     	This function is called from SetSystemConfigurationDefaults () and ColdStart() function.
 *
 *   Entry condition:
 *		None.
 *
 *   Exit condition:
 *		None.
 */
void SetStaticIPDefaults(void)
{
	Sysconfig->CurrNetworkConfig = configStatic;
	CyGOS::WriteSystemEvent(EVENT_IPCONFIG_CHANGED, __LINE__,Sysconfig->CurrNetworkConfig, (('M' << 24) | ('a' << 16) | ('i' << 8) | ('n')), true);
	Sysconfig->SystemIPStatic.addr =  MakeIP(DEFAULTIPSTATIC);
	Sysconfig->Gw.addr =  MakeIP(DEFAULTGATEWAYSTATIC);
	Sysconfig->Netmask.addr =  MakeIP(DEFAULTNETMASK);
}

/*	void SetStaticIPDefaultsOnDCPInterface(void)
 *
 *   Purpose:
 *		It fixes the static IP of WC on DCP interface
 *     	This function is called from Main() at every power up.
 *
 *   Entry condition:
 *		None.
 *
 *   Exit condition:
 *		None.
 */
void SetStaticIPDefaultsOnDCPInterface(void)
{
	Sysconfig->IPDCP.addr = MakeIP(IPDCPINTERFACE);
	Sysconfig->GwDCP.addr = MakeIP(GATEWAYDCPINTERFACE);
	Sysconfig->NetmaskDCP.addr =  MakeIP(DEFAULTNETMASK);
}

void SetFBStaticIPDefaults(void)
{
   Sysconfig->FBCurrNetworkConfig = configStatic;
   Sysconfig->FBSystemIP.addr = MakeIP(DEFAULTIPSTATIC);
   Sysconfig->FBGw.addr = MakeIP(DEFAULTGATEWAYSTATIC);
   Sysconfig->FBNetmask.addr = MakeIP(DEFAULTNETMASK);
}

/*	void SetSystemConfigurationDefaults(void)
 *
 *  Purpose:
 *		This function is called from Main() function if the CRC of SystemConfiguration structure is not correct.
 *	 	It initializes the default values in the global FRAM object.
 *
 *  Entry condition:
 *		None.
 *
 *  Exit condition:
 *		None.
 */
void SetSystemConfigurationDefaults(void)
{
	Sysconfig->Dateformat = DFormat1;
	Sysconfig->Timeformat = TFormat1;
	Sysconfig->WeldHistorySampleStoreRate = DEFAULT_WELDHISTORYSAMPLERATE;
	Sysconfig->SystemConfigurationValidCheckSum = SYSTEMCONFIGVALIDCHECKSUM;
///////////
	Sysconfig->Externalstatus = FALSE;
	Sysconfig->ErrorFlag = FALSE;
	Sysconfig->AlarmFlag = FALSE;
	Sysconfig->PbAddress = PB_DEFAULT_ADDR;
	if(Sysconfig->RFSwitch.RFSwitchingType == RFSWITCHING_DIO)
	//If RF Switching is ON then External preset recall will be set and user will not be allowed to change it.
		Sysconfig->ExternalPresetFlag = TRUE;
	else
		Sysconfig->ExternalPresetFlag = FALSE;
///////////
	Sysconfig->PowerResUnit = PowerInPercent;
}

/*  void InitializeDhcpServerConfiguration(SINT32 intf, int &priority)
 *
 *   Purpose:
 *		This function creates the DHCP server task on given interface with given priority.
 *		This The IP address of DHCP server task must already initialized in system configuration structure.
 *
 *   Entry condition:
 *		intf:
 *		priority:
 *
 *   Exit condition:
 *		None.
 */
void InitializeDhcpServerConfiguration(SINT32 Intf, int &Priority)
{
	//Set pool start address to first address on the subnet defined by first three octest of IP address
	Sysconfig->DhcpConfig.poolStart.addr = (Sysconfig->SystemIP.addr & Sysconfig->DhcpConfig.netmask.addr) + htonl(1);
	CyGOS::SetupAddress(Intf, Sysconfig->SystemIP.addr, Sysconfig->DhcpConfig.netmask.addr);	 // to setup address of DHCP server.
	CyGOS::SetGW(Intf, Sysconfig->DhcpConfig.gateway.addr);									 // to set gateway.
	CyGOS::CreateDhcpServerTask(Intf, Sysconfig->DhcpConfig, Priority); 						 // to crate DhcpServer task
}

/*  BOOLEAN CheckForColdStart(void)
 *
 *   Purpose:
 *		This function is called once at power up from Main() function to check the jumper between digital input 4
 *	 	and digital output 4.
 *	 	//TODO: Which digital input and output should be used for jumper detection.
 *
 *   Entry condition:
 *		None.
 *
 *   Exit condition:
 *		Returns TRUE if jumper is found between Digital input 4 and Digital output 4 else return FALSE.
 */
BOOLEAN CheckForColdStart(void)
{
	/*Note - Digital output 4 is represented by the 4th bit of value written to memory mapped IO.
     	     Digital input 4 is represented by the 4th bit of  value read from memory mapped IO.
     	     The address is defined in UserIO section.
     	     The address of memory mapped IO is defined in UserIO section.*/
	SINT32 Indx;
	BOOLEAN  ColdStart = FALSE;
	UINT8    ToggleCount = 0,
			OpStatus = 0,//Set 0 for all outputs by default
			OpStatusTemp = 0;

	UINT8    OutBitDefs[NUM_DOUT] =  {0, DOUT1_BIT , DOUT2_BIT, DOUT3_BIT, DOUT4_BIT, DOUT5_BIT,
			DOUT6_BIT, DOUT7_BIT, DOUT8_BIT};

	for(Indx = 1; Indx < NUM_DOUT; Indx++){
		////functions is assigned and function is active{
		if((USERIOConfig->DigOutUse[Indx] > 0) && (USERIOConfig->DigOutUseStatus[Indx] == TRUE)){
			if((USERIOConfig->DigOutUse[Indx] > 0) && ((USERIOConfig->DigOutUse[Indx] < USERDEFINED_DIGOUTPUTFUNCEND))){
				if(UserIO::GetDigitaloutputActivityStatus(USERIOConfig->DigOutUse[Indx]) == LOW)
					OpStatus |= OutBitDefs[Indx];//set high if output is configured active low so that it doesn't go active
				else if(UserIO::GetDigitaloutputActivityStatus(USERIOConfig->DigOutUse[Indx]) == HIGH)
					OpStatus &= ~OutBitDefs[Indx];//set low if output is configured active high so that it doesn't go active
			}
		}
	}
	OpStatusTemp = OpStatus;
	for(Indx = 0;  Indx < N_COLDSTART_SEQUENCE_CHECK; Indx++){
		OpStatus |= DOUT4_BIT;
		DIGITAL_IO_WRITEADDR = AdvanceConfigData->Advrnd.RevDigitalOP?~OpStatus:OpStatus;	// Set the The digital output 4 to logic 1 don't worry about others
		//insert delay before reading back
		RTOS::DelayMs(1);
		//Read the digital input 4
		if(DIGITAL_IO_READADDR & ((UINT8)DIN4_BIT))		// If read 1
			ToggleCount++;
		OpStatus &= ~DOUT4_BIT;
		DIGITAL_IO_WRITEADDR = AdvanceConfigData->Advrnd.RevDigitalOP?~OpStatus:OpStatus;	// Set the The digital output 4 to logic 0 don't worry about others
		//insert delay before reading back
		RTOS::DelayMs(1);
		if(!(DIGITAL_IO_READADDR & ((UINT8)DIN4_BIT)))	// If read 0
			ToggleCount++;
	}//for


	if(ToggleCount == (N_COLDSTART_SEQUENCE_CHECK  * 2))
		ColdStart = TRUE;

	//Finally Digital Output 4 should also be set to reverse of whatever configured
	DIGITAL_IO_WRITEADDR = AdvanceConfigData->Advrnd.RevDigitalOP?~OpStatusTemp:OpStatusTemp;

	return ColdStart;
}

void ClearAllCounters(void)
{
	SysCounters->LifeTimeCycleCount = 0;
	SysCounters->PowerOnHours = 0;
	SysCounters->SonicsOnHours = 0;
	SysCounters->GeneralAlarmCount = 0;
}

/*   void ColdStart(void)
 *
 *   Purpose:
 *		This function is called from Main()function.It restores factory settings of preset parameters, clears Alarm and Event log
 *  	Log, Restore UserIO, Alarm, Passcode and other configuration parameters to default.
 *		This function is also called from ReceiveCommandHandler Task when default initialization command received from LCD-A.
 *
 *   Entry condition:
 *		None.
 *
 *   Exit condition:
 *		None.
 */
void ColdStart(void)
{
	UserIO::ResetUserIO();  // to restore default UserIO configuration.
	Log::ResetLog(); 		// to clear Log data.
	//Note Add other function calls to restore factory defaults as more modules gets added in code.
	//SetDefaultMACCHIPRWData();
	SetDefaultPasscodeConfiguration();
	SetColdStartPresetValues();
	SetDefaultAdvRnd();
	ClearAllCounters();
	SetDefaultAlarms();
	SetFBStaticIPDefaults();
}

/*  void SetupEportForSensingPowerDown(void)
*
*  Purpose:
*     This function sets up edge port module registers to sense 24 Volt
*     i.e the power for WC board(From External Source)
*     The sensing line for 24V is connected external interrupt 7 pin of CPU
*     The registers setting are done to read the port bit and not configured
*     as an interrupt.
*     This function is called by Main() function.
*
*  Entry condition: None.
*
*  Exit condition: None.
*/
void SetupEportForSensingPowerDown(void)
{
      //Eport data direction register settings
      MCF_EPORT_EPDDR &= ~(MCF_EPORT_EPDR_EPD7);      // configure as input
      //Eport interrupt enable register settings
      MCF_EPORT_EPIER &= ~(MCF_EPORT_EPIER_EPIE7); // disable interrupts
}
/*  void SetupLCDBoardSensor(void)
*
*  Purpose:
*     This function sets up timer module registers to sense 5 Volt
*     i.e the LCD Board BDCOM Signal.
*     The sensing line for 5V is connected to Timer/Interrupt 2 pin of CPU
*     The registers setting are done to read the port bit and not configured
*     as an interrupt.
*     This function is called by Main() function.
*
*  Entry condition: None.
*
*  Exit condition: None.
*/
void SetupLCDBoardSensor(void)
{
	//5V sensing for LCD board presence
	MCF_PAD_PAR_TIMER &= ~(MCF_PAD_PAR_TIMER_PAR_T2IN(3));	//BDCOM
}

TestPoints TPointsRam;
unsigned int CrashBufferRam[4131];
void CheckAndPrintTPs()
{
	if(DebugSection->TPoints.TPChkSum == 0xAA11F065){
		TPointsRam = DebugSection->TPoints;
	}
	memset(&DebugSection->TPoints, 0, sizeof(DebugSection->TPoints));
	DebugSection->TPoints.TPChkSum = 0xAA11F065;
	memcpy(CrashBufferRam, DebugSection->CrashBuffer, sizeof(DebugSection->CrashBuffer));
}

void TaskManCallbackEvent(bool log){

	int timeHigh = 0;
	int time = 0;
	for (int i = 0; i < prioTotal; i++)
	{
		time = CyGOS::GetTaskTime(i);
		timeHigh = CyGOS::GetTaskTimeHighest(i);
		if(time || timeHigh){
			if(log)
				CyGOS::WriteSystemEvent(EVENT_TASKLOAD, time, timeHigh, i, true);

			DebugSection->TPoints.TTP[i * 2] = time;
			DebugSection->TPoints.TTP[(i * 2) + 1] = timeHigh;
		}
	}

	for (int i = 0; i < 256; i++)
	{
		time = CyGOS::GetIntrTime(i);
		timeHigh = CyGOS::GetIntrTimeHighest(i);
		if(time || timeHigh){
			if(log)
			CyGOS::WriteSystemEvent(EVENT_INTLOAD, time, timeHigh, i, true);

			DebugSection->TPoints.ITP[i * 2] = time;
			DebugSection->TPoints.ITP[(i * 2) + 1] = timeHigh;
		}
	}

}
void TaskManCallback()
{
	TaskManCallbackEvent(false);
	int timeHigh = 0;
	static int SkipFirstFewSec = FALSE;
	if((PrintCurrentTaskLoad == TRUE)||(PrintHighestTaskLoad == TRUE))
		printf("    Time Priority Name:\n");
	for (int i = 0; i < prioTotal; i++)
	{
		int time = CyGOS::GetTaskTime(i);
		if(SkipFirstFewSec > 5)
		   timeHigh = CyGOS::GetTaskTimeHighest(i);
		if ((PrintCurrentTaskLoad == TRUE) && time)
			printf("C %7d %3d %s\n", time, i, CyGOS::GetTaskName(i));
		if((PrintHighestTaskLoad == TRUE) && timeHigh)
			printf("H %7d %3d %s\n", timeHigh, i, CyGOS::GetTaskName(i));
	}
	timeHigh = 0;
	if((PrintCurrentTaskLoad == TRUE)||(PrintHighestTaskLoad == TRUE))
		printf("    Time Vector Counts:\n");
	for (int i = 0; i < 256; i++)
	{
		int time = CyGOS::GetIntrTime(i);
		int ints = CyGOS::GetIntCount(i);
		if(SkipFirstFewSec > 5){
			timeHigh = CyGOS::GetIntrTimeHighest(i);
		}
		if((PrintCurrentTaskLoad == TRUE)&&(time || ints))
			printf("C %7d %3d %9d\n", time, i, ints);
		if((PrintHighestTaskLoad == TRUE) && (timeHigh || ints))
			printf("H %7d %3d\n", timeHigh, i);
	}
	if(	SkipFirstFewSec <= 5)
		SkipFirstFewSec++;
}

extern UINT8 CrashFBRecvBuffer[MAX_FBEXCHANGESIZE];
extern unsigned char CrashMailBoxbuff[1596];
extern UINT8 CrashSerialRecBuff[RXBUFSIZE];
extern UINT8 CrashMBRcvPktbuf[MB_UDP_BUF_SIZE];

void StoreInfoAtCrash()
{
	pprintf("\n Crashed");
	DebugSection->CrashValid = CrashValid;
	memcpy(DebugSection->TaskName, TaskName,sizeof(TaskName));
	memcpy(DebugSection->CrashBuffer, CrashBuffer,sizeof(CrashBuffer));
	DebugSection->CurrStackPtr = CurrStackPtr;
	DebugSection->Stackbegin = Stackbegin;
	DebugSection->StackEnd = StackEnd;
	DebugSection->CrashBuffPrintLen = CrashBuffPrintLen;
	DebugSection->MMUAR = MMUAR;
	memcpy(DebugSection->FRAMCrashFBRecvBuffer,CrashFBRecvBuffer,sizeof(CrashFBRecvBuffer));
	memcpy(DebugSection->FRAMCrashMailBoxbuff,CrashMailBoxbuff,sizeof(CrashMailBoxbuff));
	memcpy(DebugSection->FRAMCrashSerialRecBuff,CrashSerialRecBuff,sizeof(CrashSerialRecBuff));
	memcpy(DebugSection->FRAMCrashMBRcvPktbuf,CrashMBRcvPktbuf,sizeof(CrashMBRcvPktbuf));
}



/*  void InitSC1054ExternalWatchDogTrigger(void)
 *
 *  Purpose:
 *    This function sets up DMA Timer module of SC1054 board to start feeding
 *    the HW watch dog with continuous pulses of 1HZ frequency
 *    This function is called by Main() function.
 *
 *  Entry condition:
 *    Power up.
 *
 *  Exit condition:
 *    Pulses of 1 Hz Frequency started getting feed at WDI input of HW watch dog.
 */
void InitSC1054ExternalWatchDogTrigger(void)
{
   MCF_PAD_PAR_TIMER |= (MCF_PAD_PAR_TIMER_PAR_T1IN(
         MCF_PAD_PAR_TIMER_PAR_T0IN_T0OUT));
   MCF_DTIM_DTMR(1) = MCF_DTIM_DTMR_CLK(1)| MCF_DTIM_DTMR_PS(__SYSTEM_CLOCK - 1)   //1MHz
         | MCF_DTIM_DTMR_OM | MCF_DTIM_DTMR_FRR;//clock source 125 Mhz/Toggle output/Restart
   MCF_DTIM_DTXMR(1) |= MCF_DTIM_DTXMR_HALTED;   //stop counting when core halted/Increment by one
   MCF_DTIM_DTRR(1) = 500000;   //(500ms On 500 Ms off to generate pulse train of 1 Hz)
   MCF_DTIM_DTCN(1) = 0;
   MCF_DTIM_DTMR(1) |= MCF_DTIM_DTMR_RST;
}



/*  void InitWC1169ExternalWatchDogTrigger(void)
 *
 *  Purpose:
 *    This function sets up DMA Timer module of WC1169 board to start feeding
 *    the HW watch dog with continuous pulses of 1HZ frequency
 *    This function is called by Main() function.
 *
 *  Entry condition:
 *    Power up.
 *
 *  Exit condition:
 *    Pulses of 1 Hz Frequency started getting feed at WDI input of HW watch dog.
 */
void InitWC1169ExternalWatchDogTrigger(void)
{
   MCF_PAD_PAR_TIMER |= (MCF_PAD_PAR_TIMER_PAR_T3IN(
         MCF_PAD_PAR_TIMER_PAR_T0IN_T0OUT));
   MCF_DTIM_DTMR(3) = MCF_DTIM_DTMR_CLK(1)| MCF_DTIM_DTMR_PS(__SYSTEM_CLOCK - 1)   //1MHz
         | MCF_DTIM_DTMR_OM | MCF_DTIM_DTMR_FRR;//clock source 125 Mhz/Toggle output/Restart
   MCF_DTIM_DTXMR(3) |= MCF_DTIM_DTXMR_HALTED;   //stop counting when core halted/Increment by one
   MCF_DTIM_DTRR(3) = 500000;   //(500ms On 500 Ms off to generate pulse train of 1 Hz)
   MCF_DTIM_DTCN(3) = 0;
   MCF_DTIM_DTMR(3) |= MCF_DTIM_DTMR_RST;
}



/*  void InitWC991ExternalWatchDogTrigger(void)
 *
 *  Purpose:
 *     Do nothing as there is no hardware watchdog on WC 991 board.
 *
 *  Entry condition:
 *    None
 *
 *  Exit condition:
 *    None
 */
void InitWC991ExternalWatchDogTrigger(void)
{
	// Do nothing as there is no hardware watchdog on WC 991 board.
}



/*  void InitializeWC991SpecificFunction(void)
 *
 *  Purpose:
 *    This function initialize all global function pointers with the corresponding board
 *    ie. WC991 dependent functions.
 *
 *  Entry condition:
 * 	At Power up if WC_991_BOARD is detected.
 *
 *  Exit condition:
 * 	Initialize all global function pointers
 */
void InitializeWC991SpecificFunction(void)
{
   InitExternalWatchDogTrigger = InitWC991ExternalWatchDogTrigger;
   ConfigureResetPHYandHilscher = ConfigureResetForPHYandHilscherWC991;
   SendResetToPHYandHilscher =  SendResetToPhyAndHilscherWC991;
   ConfigDigitalOutputLatch = EnableDigitalOutputLatchWC991;
   ConfigureLCDRst = ConfigureResetLineForLCDWC991;
   SendResetToLCD = SendResetToLCDWC991;
   InitBDMPins = BDM08::InitBDMPinsWC991;
   SetReset = BDM08::SetResetWC991;
   SetBkgd = BDM08::SetBkgdWC991;
   GetBkgd = BDM08::GetBkgdWC991;
   SetBkgdOut = BDM08::SetBkgdOutWC991;
   SetBkgdIn = BDM08::SetBkgdInWC991;
}



/*  void InitializeSC1054SpecificFunction(void)
 *
 *  Purpose:
 *     This function initialize all global function pointers with the corresponding board
 *     ie. SC1054 dependent functions.
 *
 *  Entry condition:
 * 	At Power up if WC_1054_BOARD is detected.
 *
 *  Exit condition:
 * 	Initialize all global function pointers
 */
void InitializeSC1054SpecificFunction(void)
{
   InitExternalWatchDogTrigger = InitSC1054ExternalWatchDogTrigger;
   ConfigureResetPHYandHilscher = ConfigureResetForPHYandHilscherSC1054;
   SendResetToPHYandHilscher =  SendResetToPhyAndHilscherSC1054;
   ConfigDigitalOutputLatch = EnableDigitalOutputLatchSC1054;
   ConfigureLCDRst = ConfigureResetLineForLCDSC1054;
   SendResetToLCD = SendResetToLCDSC1054;
   InitBDMPins = BDM08::InitBDMPinsSC1054;
   SetReset = BDM08::SetResetSC1054;
   SetBkgd = BDM08::SetBkgdSC1054;
   GetBkgd = BDM08::GetBkgdSC1054;
   SetBkgdOut = BDM08::SetBkgdOutSC1054;
   SetBkgdIn = BDM08::SetBkgdInSC1054;
}



/*  void InitializeWC1169SpecificFunction(void)
 *
 *  Purpose:
 *     This function initialize all global function pointers with the corresponding board
 *     ie. WC1169 dependent functions.
 *
 *  Entry condition:
 * 	At Power up if WC_1169_BOARD is detected.
 *
 *  Exit condition:
 * 	Initialize all global function pointers
 */
void InitializeWC1169SpecificFunction(void)
{
   InitExternalWatchDogTrigger = InitWC1169ExternalWatchDogTrigger;
   ConfigureResetPHYandHilscher = ConfigureResetForPHYandHilscherWC1169;
   SendResetToPHYandHilscher =  SendResetToPhyAndHilscherWC1169;
   ConfigDigitalOutputLatch = EnableDigitalOutputLatchWC1169;
   ConfigureLCDRst = ConfigureResetLineForLCDWC1169;
   SendResetToLCD = SendResetToLCDWC1169;
   InitBDMPins = BDM08::InitBDMPinsWC1169;
   SetReset = BDM08::SetResetWC1169;
   SetBkgd = BDM08::SetBkgdWC1169;
   GetBkgd = BDM08::GetBkgdWC1169;
   SetBkgdOut = BDM08::SetBkgdOutWC1169;
   SetBkgdIn = BDM08::SetBkgdInWC1169;
}



/*  void DetectWCBoard(void)
 *
 *  Purpose:
 *    To determine the WC board based on logic at CPU’s AB14 pin and Phy chip present on Ethernet interface 0.
 *    This function is called once from main() function.
 *
 *  Entry condition:
 *  	System power on.
 *
 *  Exit condition:
 *   	WC board is determined.
 */
void DetectWCBoard(void)
{
   char * PHYName = (char *) CyGOS::GetInterfaceName(0);
   MCF_EPORT_EPIER &= ~(MCF_EPORT_EPIER_EPIE3);		//Make sure no interrupts
   MCF_EPORT_EPPAR &= ~(MCF_EPORT_EPPAR_EPPA3(3)); //Clear EPPA3 bits
   MCF_EPORT_EPDDR &= ~(MCF_EPORT_EPDDR_EPDD3);    // Clear EPDD7 bit in EPPDR register to configure IRQ3 line as input.

   if (CHECKIFAB14HIGH) {
      if (strcmp(PHYName, PHYNAME_LAN8701) == 0) {
         InitializeWC991SpecificFunction();
         BoardType = WC_991_BOARD;
      }
      else {
         InitializeSC1054SpecificFunction();
         BoardType = SC_1054_BOARD;
      }
   }
   else {
      InitializeWC1169SpecificFunction();
      BoardType = WC_1169_BOARD;
   }
}



/*   int main()
 *
 *   Purpose:
 *		This is the entry point of WC Application code. This function is called by startup assempbly routines.
 *
 *   Entry condition:
 *		None.
 *
 *   Exit condition:
 *		Never returns.
 */
int main()
{
	int Priority = 0;
	BYTE MacAddr[MAC_ADDRESS_LEN];
	BOOLEAN JumperFlag = FALSE;
	//In this sequence, all WC initialization work shall take place.
	CyGOS::Init();// for basic tasks initialization
	DetectWCBoard();
	InitExternalWatchDogTrigger();
	pprintf(" PowerUpInitialization ");
	PowerUpInitialization();
	CyGOS::RegisterTaskLoadLogCallback(TaskManCallbackEvent);
	CyGOS::RegisterAppCrashCallback(StoreInfoAtCrash);
	SetupEportForSensingPowerDown();
	SetupLCDBoardSensor();
	JumperFlag = CheckForColdStart();	   // to check for jumper between  digital input 4 and digital output 4 from MI-A board.
	if(JumperFlag == TRUE){
		pprintf("\n JumperFlag = TRUE \n");
		LogData2 = LOG_CLEARED_BY_USER_IO_JUMPER_AT_POWER_UP;
		ColdStart();
		SetStaticIPDefaults();
		SetStaticIPDefaultsOnDCPInterface();
	}
	CyGOS::CreateTaskManager(Priority, 4096, TaskManCallback);
	CyGOS::CreateTask(new InputTask(), Priority);

	//Initialize WCTask , MIATask  and FieldBus Task
	MIATask * MIA = new MIATask();
	WCTask  * WC  = new WCTask(MIA);

	CyGOS::CreateTask(MIA, Priority);
	if((MacchipData.CurrSystemType == SYSTEM_FIELDBUS) || ((MacchipData.CurrSystemType == SYSTEM_FIELDBUS_HD))){
		FieldBus * fbPt = FindFieldBusType();// to Detect the Hilscher FieldBus card and the supported FieldBus protocol.
		if(fbPt)
			CyGOS::CreateTask(new FieldbusTask(fbPt) , Priority);
		else
		   AlarmQueueWrite(CF03);
	}

	TcpIp * TcpIPTask = new TcpIp();
	SetStaticIPDefaultsOnDCPInterface();
	memcpy(MacAddr, (UINT8*)"\x12\xC6\x05\xDC\x1C\x02", MAC_ADDRESS_LEN);
	CyGOS::AddTcpInterface(ETHERNET_INTERFACE_WCTODCP, Priority, MacAddr, false);
	CyGOS::SetupAddress(ETHERNET_INTERFACE_WCTODCP, Sysconfig->IPDCP.addr, Sysconfig->NetmaskDCP.addr); // to assign IP to Ethernet interface 0.
	CyGOS::SetGW(ETHERNET_INTERFACE_WCTODCP, Sysconfig->GwDCP.addr);	 				// to assign gateway to Ethernet interface 0;
	//Initialize ModbusSend  and ModBusReceive Tasks on Ethernet Interface 0.(TBD)
	CyGOS::CreateTask(new ModBusRcvTask(ETHERNET_INTERFACE_WCTODCP), Priority);
	CyGOS::CreateTask(new ModBusSendTask(ETHERNET_INTERFACE_WCTODCP), Priority);
	CyGOS::CreateTask(new NRTModbus(), Priority);
	CyGOS::CreateTask(WC, Priority);
	ReadMACAddressFromMacChip(MacAddr);
	CyGOS::CreateTask(new ReceiveCommandHandler(),Priority);
	CyGOS::CreateTask(TcpIPTask, Priority);
	CyGOS::AddTcpInterface(ETHERNET_INTERFACE_WCTOWEB, Priority, MacAddr); // to Initialize Ethernet related task and
   //Check the system configuration checksum and IP configuration to be applied (i.e. static, dhcp client , dhcp server or default) on Ethernet Interface 1,
   //from the sysConfig variable saved in FRAM and initialize DHCP related Tasks accordingly.
   if(Sysconfig->SystemConfigurationValidCheckSum == SYSTEMCONFIGVALIDCHECKSUM){
      pprintf("\n Sysconfig->CurrNetworkConfig %d \n",Sysconfig->CurrNetworkConfig);
      if(Sysconfig->CurrNetworkConfig == configDhcpClient){
         pprintf("\n Setting as DHCP Client");
         CyGOS::SetupInterface(ETHERNET_INTERFACE_WCTOWEB, TRUE, TRUE);       // to initialize DHCP client on Ethernet Interface 1;
      }
      else if(Sysconfig->CurrNetworkConfig == configStatic){
         pprintf("Setting as Static IP");
         CyGOS::SetupAddress(ETHERNET_INTERFACE_WCTOWEB, Sysconfig->SystemIPStatic.addr, Sysconfig->Netmask.addr);  // to set IP on Ethernet interface 1;
         CyGOS::SetGW(ETHERNET_INTERFACE_WCTOWEB, Sysconfig->Gw.addr);         // to assign gateway on Ethernet Interface 1.
      }
      else if(Sysconfig->CurrNetworkConfig == configDhcpServer){
         pprintf("Setting as DHCP Server");
         InitializeDhcpServerConfiguration(ETHERNET_INTERFACE_WCTOWEB, Priority);// to initialize Dhcp server configuration on etherner interface 1;
      }
   }
   else{
      pprintf("Setting default network config");
      Log::WriteEventLog(EVENT_CRC_ERROR, SYSTEMCONFIGVALIDCHECKSUM, Sysconfig->SystemConfigurationValidCheckSum, CRC_ERROR_SYSCONFIG);
      SetDHCPServerIPDefaults();
      SetStaticIPDefaults();
      SetFBStaticIPDefaults();
      SetSystemConfigurationDefaults();
      CyGOS::SetupAddress(ETHERNET_INTERFACE_WCTOWEB, Sysconfig->SystemIPStatic.addr, Sysconfig->Netmask.addr);  // to Initialize static configuration with default static IP on Ethernet Interface 1;
      CyGOS::SetGW(ETHERNET_INTERFACE_WCTOWEB, Sysconfig->Gw.addr);
   }
   CyGOS::CreateTask(new ShutDownTask(), Priority);
   CyGOS::CreateHttpServerTask(ETHERNET_INTERFACE_WCTOWEB, DCXWEBSITE_PORT, Priority); // to Initialize HTTP Server task on Ethernet interface 1 required for website communication.
   CyGOS::CreateTftpClientTask(ETHERNET_INTERFACE_WCTOWEB, Priority);
	CyGOS::InitFileSystems(Priority);
	CyGOS::CreateBlockDevice(CreateRamDisk(), "A", Priority);
	RegisterTarFS(Priority, Priority + 1);
	Priority += 2;
	UINT32 ResLenght = ReadTarRegionFlash(0, (unsigned char *)__TAR_START, (int)__TAR_SIZE);
	CompressedWebPageCRC = ByteCRC((UINT8 *)__TAR_START, ResLenght);
	printf("Compressed page crc %d", CompressedWebPageCRC);
	//Create and Register FRAM file system and initialize FRAMDisk task to enable reading FRAM as a file.
	CyGOS::CreateBlockDevice(CreateFRAMDisk(), "B", Priority);
	RegisterFRAMFS(Priority, Priority + 1);
	Priority += 2;
	CyGOS::InitDriveImage(1 , (void*)FRAM_BASEADDRESS, FRAM_SIZE);
  // CyGOS::CreateTask(new TaskManager(), Priority);
   CyGOS::CreateTask(new UploadFirmware(), Priority);       // to create Upload Firmware task to deal with code download through website.
   CyGOS::CreateTask(new FirmWareUpgradeTask(ETHERNET_INTERFACE_WCTODCP), Priority);  // to create FirmwareUpgarde task which deals with firmware file transfer from WC to DCP and LCD.
 //  CyGOS::CreateTask(new InputTask(), Priority);	//test only!!!!!!!
   CyGOS::CreateTask(new ExternalStatusTask(), Priority);
   CyGOS::CreateTask(new WelderSearchTask(ETHERNET_INTERFACE_WCTOWEB), Priority);
   WebSite::InitWebSite(ETHERNET_INTERFACE_WCTOWEB, DCXWEBSITE_PORT);
   DcxMangerHandler::InitDcxMangerHandler(ETHERNET_INTERFACE_WCTOWEB, DCXWEBSITE_PORT);
   CyGOS::SetEthernetLinkDebouncing(AdvanceConfigData->Advrnd.Flag3);
   RTOS::DelayMs(1000);
   //CyGOS::InstallTimer(TIMERINTERRUPT_INTERVALUS, TimerInterruptCallback, 1);    // to initialize and register timer interrupt.
   CheckAndPrintTPs();
   //pprintf("\n Resetting MII chip LAN8701");
   //CyGOS::GetNetworkInterface(0)->driver->mii->WriteSMIRegister(0, 0x8000);
   pprintf("\n sizeof(DcxPs) %d size of Pascode preset %d"
  		   "reg count %d" , sizeof(DcxPreset), sizeof(PassCodeConfiguration) , PRESET_REGCOUNT);
   //Board::InitDMATimer(1);
   pprintf("\n CyGOS::Start\n");
   CyGOS::Start(10000);// to start CyGOS with tick value 10000 us(fix for now).
}

