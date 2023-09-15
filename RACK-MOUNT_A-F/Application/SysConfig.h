/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/SysConfig.h_v   1.3   03 Feb 2016 00:59:28   ygupta  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/SysConfig.h_v  $
 * 
 *    Rev 1.3   03 Feb 2016 00:59:28   ygupta
 * Added 8-bit variable in "SystemConfiguration" structure to hold recalled preset number.
 * 
 *    Rev 1.2   28 Jan 2016 02:00:28   ygupta
 * Added enum for Unit of Power result; Added 8-bit variable in "SystemConfiguration" structure to hold unit of Power result.
 * 
 *    Rev 1.1   10 Aug 2015 09:41:40   RJamloki
 * Added variables for Clear memory before seek, Clear memory with reset, Clear with horn scan and clear at power up options in system configuration
 * 
 *    Rev 1.0   09 Jun 2015 09:10:48   tayars
 * Initial revision.
 * 
 *    Rev 1.24   13 Mar 2014 15:55:06   rjamloki
 * FRAM is accessed through pointers. Removed compiler allocation of FRAM
 *
 */

#ifndef SYSCONFIG_H_
#define SYSCONFIG_H_

#include "Portable.h"
#include "CyGOS.h"
#include "DhcpServer.h"
//#include "TYP_DEF.h"
#include "portable.h"

//Hard coded checksum to calculate the validity of System configuration data
#define SYSTEMCONFIGVALIDCHECKSUM 0x13ADF8BA
//IPs define on ethernet interface 1
#define DEFAULTIPDHCPSERVER			 100, 100, 1,   101
#define DEFAULTPOOLSTARTDHCPSERVER 	 100, 100, 1,   1
#define DEFAULTGATEWAYDHCPSERVER	 0,   0,   0,   0
#define DEFAULTIPSTATIC 	 	192,168,10,100
#define DEFAULTGATEWAYSTATIC 	192,168, 10 ,1
#define DEFDHCPSERVERID_LEN  	4
#define DEFAULTPOOLSIZE      	64
#define DEFAULTLEASETIME     	600
#define DEFAULTRENEWTIME     	120
#define DEFAULTREBINDTIME    	300
#define DEFAULTNETMASK 		 	255, 255, 255, 0

//WC's fixed static IP on DCP interface
#define IPDCPINTERFACE 		 	192, 167, 10, 100
#define GATEWAYDCPINTERFACE  	192, 167, 10, 1

//fixed static pre known DCP IP. Needed for modbus communication and Firmware upgrade on DCP
#define DCPIP 192, 167, 10, 105
#define DEFAULT_WELDHISTORYSAMPLERATE 100

//PS available frequencies
enum FREQUENCY
{
	FREQ_20KHZ = 20,
	FREQ_30KHZ = 30,
	FREQ_40KHZ = 40,
};

//system type to be set from manufacturing webpage goes into MAC chip RW area
enum SYSTEMTYPE
{
   SYSTEM_UNKNOWN,
	SYSTEM_ADVANCED,
	SYSTEM_FIELDBUS,
	SYSTEM_ADVANCED_HD,
	SYSTEM_FIELDBUS_HD,
	//add for balun in future if required
};

//The enum represents format of date while showing event, alarm and preset information on web pages
enum DateFormat
{
	DFormatUnknown,
	DFormat1, //DDMMYY
	DFormat2, //MMDDYY
};

//The format of time while showing event, alarm and preset information on web pages
enum TimeFormat
{
	TFormatUnknown,
	TFormat1, //12Hour
	TFormat2, //24Hour
};

//Language in which strings should be shown on web pages currently only English language is supported
enum Language
{
	English,
	German,
	Spanish,
	Italian,
	French,
	ChineseSimplified,
	ChineseTraditional,
	Japanese,
	Korean,
};

enum DCPSpeedControl
{
   SPEEDCONTROL_STANDARD = 2,//S and V compatible fixed 1000 microsecond loop time
   SPEEDCONTROL_CUSTOM = 3,//Configurable Loop time. Default 100 microsecond.
   SPEEDCONTROL_MAX = SPEEDCONTROL_CUSTOM,
};

//Below given enum contains the unit for Power result to be displayed in Alarm log and history
enum PowerUnit
{
	PowerInWatts,
	PowerInPercent,
};

enum RFSwitching
{
   NORFSWITCHING,
   RFSWITCHING_FB,
   RFSWITCHING_DIO,
};

enum RFSwitchingFeedBack
{
   NOFEEDBACK,
   STATUSFEEDBACK,
   UNCODEDFEEDBACK,
   CODEDFEEDBACK,
};

enum RFSwitchingOutput
{
   UNCODED,
   CODED
};

enum RFSwitchOption
{
   DIRECTSWITCH,
   SWITCHWITHOFF,
};
enum RFSwitchingStatus
{
   SWITCHINGINPROGRESS,
   SWITCHINGDONE,
   SWITCHINGABORTED_FAILED,
   SWITCHINGNOTDONE,
};

struct RFSwitchingConfig
{
   RFSwitching RFSwitchingType;
   RFSwitchingFeedBack RFFeedback;
   UINT16 FeedBackTimeOut;
   UINT16 RFSwitchDelay;
   RFSwitchOption RFOption;
   UINT16 RelaySwitchTime;
   RFSwitchingOutput RFOutputType;
};

struct FieldBusAddr{
   UINT8 Addr1;
   UINT8 Addr2;
   UINT8 Addr3;
   UINT8 Addr4;
};

struct SystemConfiguration
{
	UINT32  DCXCRC;		// to be assigned at run time. Here for log purpose
	ip_addr SystemIP;		// IP address in case of DHCP server
	ip_addr SystemIPStatic;	// IP address in case of static configuration
	ip_addr Netmask;		// Net mask in case of static IP
	ip_addr Gw;				// gateway in case of static IP
	NetworkConfiguration CurrNetworkConfig;	// current configuration
	//DCP side IP
	ip_addr IPDCP;			// IP address on DCP interface
	ip_addr NetmaskDCP;	// Net mask on DCP interface
	ip_addr GwDCP;			// gateway on DCP interface
	DhcpServerConfiguration DhcpConfig;
	UINT16  WeldHistorySampleStoreRate;//Discuss
	DateFormat Dateformat;
	TimeFormat Timeformat;
	BOOLEAN TweakRangeEnabled;
	UINT32  ActivePreset;
	Language CurrLang;//For future??
	UINT8   PowerupOption;
	FieldBusAddr FBAddr;
	UINT32  SystemConfigurationValidCheckSum;
	///////website compilation
	BOOLEAN Externalstatus;
	BOOLEAN ErrorFlag;
	BOOLEAN AlarmFlag;
	BOOLEAN ExternalPresetFlag;
	BOOLEAN PresetChanged;
	RFSwitchingConfig RFSwitch;
	UINT8   PbAddress;
	//// FB /////
	ip_addr FBSystemIP; // IP address in case of static configuration
	ip_addr FBNetmask;     // Net mask in case of static IP
	ip_addr FBGw;          // gateway in case of static IP
	NetworkConfiguration FBCurrNetworkConfig;   // current configuration
	DCPSpeedControl DcpSpeedControl;
	BOOLEAN ClearMemoryBeforeSeek;//Whether to clear memory before seek
	BOOLEAN ClearMemoryWithReset;//whether to clear memory on reset
	BOOLEAN SetDigTuneWithHornScan;//whether to set digital tune equal to resonance frequency after scan
	BOOLEAN ClearMemoryAtPowerUp;//Whether to clear memory at Power up
	UINT8 PowerResUnit;	//Unit of Power results i.e. Watts or %age to be displayed in in alarm log and history
	UINT8 RecalledPreset;
	UINT8 DummyPlaceholder[5];//place holder
};

struct CounterSt
{
   UINT32 PowerOnHours;
   UINT32 SonicsOnHours;
   UINT32 LifeTimeCycleCount;
   UINT32 GeneralAlarmCount;
};

struct TestPoints{
	UINT32 TPChkSum;
	UINT32 TTP[128];
	UINT32 ITP[512];
};

struct TestPointsOld{
	UINT32 TPChkSum;
	UINT32 TTP[300];
	UINT32 ITP[300];
};

extern SystemConfiguration * Sysconfig;
extern CounterSt * SysCounters;
#endif /* SYSCONFIG_H_ */
