/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/FieldBus.h_v   1.13   29 Jul 2015 07:53:32   RJamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/FieldBus.h_v  $
 * 
 *    Rev 1.13   29 Jul 2015 07:53:32   RJamloki
 * Added wrapper function to check the memory clear command from fieldbus
 * 
 *    Rev 1.12   04 Mar 2014 07:49:22   rjamloki
 * Added Software Upgrade, Digital Tune and Memory Offset.
 * 
 *    Rev 1.11   02 Dec 2013 07:34:18   rjamloki
 * Changes for EthIP Conformance Test.
 * 
 *    Rev 1.10   24 Aug 2013 17:14:00   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.9   10 Jul 2013 07:50:14   amaurya
 * Fixed powerup DCP Event Log issue.
 * 
 *    Rev 1.8   28 May 2013 07:42:36   amaurya
 * Fixed issues of Level II testing.
 * 
 *    Rev 1.7   21 May 2013 12:25:50   amaurya
 * Code review fixes.
 * 
 *    Rev 1.6   08 Apr 2013 08:22:20   amaurya
 * Fixed Tracker issue of Ver2.0P.
 * 
 *    Rev 1.5   18 Feb 2013 03:05:14   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.4   22 Jan 2013 10:38:12   ygupta
 * Profibus Bug Fixes
 * 
 *    Rev 1.3   17 Jan 2013 07:24:44   ygupta
 * Profibus Related changes
 * 
 *    Rev 1.2   16 Jan 2013 06:24:52   ygupta
 * Level2 Code with Profibus
*/
#ifndef FIELDBUS_H_
#define FIELDBUS_H_
#include "TLR_Types.h"
#include "portable.h"
#include "cifXUser.h"
#include "TYP_DEF.h"
#include "rcX_User.h"
#include "preset.h"

//#define FIELDBUS_DEBUG
#define HW_ASSEMBLY_VALIDATION_START   0x0010    /* The offset from base of DPRAM which is pointing to an array of 4 16bit values. WC shall read this array to find the type of fieldbus protocol supported by Hilscher slave card*/

#define HW_ASSEMBLY_PROFIBUS           0x0050	 /*profibus protocol ID predefined by Hilscher */
#define HW_ASSEMBLY_ETHERNETIP         0x0080	 /*Ethernet IP protocol ID predefined by Hilscher*/

#define MAILBOX_PACKET_TIMEOUT 		1000//non real time packet transfer/acknowledgment timeout with Hilscher slave card.

//Error codes for logging
#define CARD_DETECT_ERROR ((UINT16)1)
#define PROTOCOL_DETECT_ERROR ((UINT16)2)
#define APP_REGISTOR_ERROR ((UINT16)3)
#define WARMSTART_CONFIG_ERROR ((UINT16)4)
#define ADD_DEVICE_ERROR ((UINT16)5)
#define DRIVER_OPEN_ERROR ((UINT16)6)
#define CHANNEL_OPEN_ERROR ((UINT16)7)
#define SYSDEVICE_NOT_READY ((UINT16)8)
#define COMMCHANNEL_NOT_READY ((UINT16)9)


//Any DCX supported field bus should send minimum following commands in real time data
struct FBControlSignal
{
   //Parameter set number to recall a preset
   UINT32 SelParmeterSet;
   //Selected Horn number sent by FieldBus master
   UINT16 HfSwitchFeedback;
   //Emergency Stop
   BOOLEAN EmergencyStop;
   //Mode Selection True(Auto), False(Manual)
   BOOLEAN AutoMode;
   //GrounDetect (True) Gnd detect Reached
   BOOLEAN GndDetect;
   //AmpProfile, True (Amp2) , False(Amp1)
   BOOLEAN AmpProfiling;
   //Start sonics(for either weld or horn function)
   BOOLEAN StartUs;
   //Reset if any error and clear the OK/ERROR status
   BOOLEAN Rst;
   //Enable weld function
   BOOLEAN EnableWeldFunction;
   //Enable horn function(either seek, test or scan)
   BOOLEAN EnableHornFunction;
   //Horn function (0-seek, 1-test, 2, scan)
   UINT8   HornFunctionNumber;
   //Keep memory clear signal from fieldbus
   BOOLEAN MemoryClear;
};

//Any DCX supported field bus should be sent minimum following status in real time data
struct FBStatusSignal
{
   //The currently active preset number
   UINT32 SelectedParamSet;
   //Individual bit represents one of the possible type of alarms.
   //Bit 0 to 11 represents OL,CU,SE,CM,WA,LM,EQ,NC,CF,TP,HW and NO alarms respectively.
   UINT32 Error;
   //Horn number in currently selected preset
   UINT16 HornNumber;
   //The current operating status of DCX
   BOOLEAN AutoModeStaus;
   //The status of parameter has been recalled
   BOOLEAN ParameterChangeDone;
   //Error in RF switching
   BOOLEAN RfSwitchingError;//Not implemented: equivalent Alarms NC05 and NC07
   //Temperature Error
   BOOLEAN TemperatureError;
   //Emergency Stop Active status
   BOOLEAN EmergencyStopActive;
   //Weld or horn function is completed successfully status
   BOOLEAN CycleOK;
   //The status of sonics being currently active
   BOOLEAN UsActive;
   //Power supply is ready for another weld or horn function
   BOOLEAN ReadyForUs;
   //TODO:
   BOOLEAN StopMode;
   //Keep memory cleared status for fieldbus
   BOOLEAN MemoryClearStatus;
};

extern FBControlSignal FBCmd;
extern FBStatusSignal  FBStat;

//Parameter access from Fieldbus
#define ACC_WRITE          0
#define ACC_READ           1

//Access token, and some special commands(like move preset from one slot to another) are
//represented by these defines
#define OS_ACCESS_TOKEN                50

//Parameter number start and end for different set of
//groups(Preset, hspreset etc) mapped in Fieldbus table
#define TEST_PARA_BASE                 1
#define TEST_PARA_LAST                  12
#define VERSIONINFO_BASE               100
#define VERSIONINFO_LAST               113
#define SYSINFO_BASE                   150
#define SYSINFO_LAST                   157
#define RTC_DATE                 170
#define RTC_TIME                 171
#define ERROR_INFO_BASE             200
#define ERROR_INFO_LAST             247
#define EXTCTRL_INFO_BASE           400
#define EXTCTRL_INFO_LAST           440
#define CNTR_INFO_BASE                 860
#define CNTR_INFO_LAST                 944
#define PARA_SET_BASE                  1010
#define PARA_SET_LAST                  1105
#define WS_SET_BASE                    1210
#define WS_SET_LAST                    1389
#define HSPARA_SET_BASE                1460
#define HSPARA_SET_LAST                1505


#define HSSS_SET_BASE                  1610
#define HSSS_SET_LAST                  1894

#define AMP_IN_MIN 10
#define AMP_IN_MAX 100
#define AMP_IN_DEFAULT 50

//Not visible due to poor syntax highlighting issue of editor but __GNUC__ is defined
#ifdef __GNUC__
   #define PACKED_POST   __attribute__ ((packed))
#else
   #define PACKED_POST
#endif


//Data in and out possible over Process and Communication channel from Fieldbus
//is structured using this structure.PKW Area is specific to Profibus. PZD Area is common for all protocols
struct FieldbusIO
{
   //PKW In
   union{
      struct{
         UINT8 PKWIn[8];
      }PKWu8In;
      struct{
         UINT16 Pke; //Instruction code plus parameter number field
         UINT16 Ind; //Index plus special access filed
         union{
            struct{
               UINT16 PweHigh;//parameter high word
               UINT16 PweLow; //parameter low word
            }PWEu16In;
            UINT32 PWEu32In;
         }PWE;
      }PKWu16In;
      struct{
         UINT32 PkwHigh;
         UINT32 PkwLow;
      }PKWu32In;
   }PKWIn;

   //PKW Out
   union{
      struct{
         UINT8 PKWOut[8];
      }PKWu8Out;
      struct{
         UINT16 Pke; //Answer or error code plus parameter number
         UINT16 Ind; //Accessed index
         union{
            struct{
               UINT16 PweHigh;//parameter high word
               UINT16 PweLow; //parameter low word
            }PWEu16Out;
            UINT32 PWEu32Out;
         }PWE;
      }PKWu16Out;
      struct{
         UINT32 PkwHigh;
         UINT32 PkwLow;
      }PKWu32Out;
   }PKWOut;

   union{
      struct{
         UINT8 PZDIn[4];
      }PZDu8In;
      struct{
         UINT16 STWIn1;
         UINT16 STWIn2;
      }PZDu16In;
      struct{
         UINT32 STW;
      }PZDu32In;
      struct{
         /*MSB->LSB*/
         unsigned int Dummy3                   :1;//Reserved
         unsigned int ManualAuto               :1;
         unsigned int Dummy2                   :1;//Reserved
         unsigned int FldParaSetNbrX           :5;

         unsigned int FldHfSwitchX             :4;
         unsigned int Dummy1                   :2;
         unsigned int EmgcyStop                :1;
         unsigned int Dummy0                   :1;//Reserved

         unsigned int XIn1                     :1;//Reserved
         unsigned int XIn0                     :1;//Reserved
         unsigned int AmplitudeProfExt         :1;
         unsigned int GndDtct                  :1;
         unsigned int Dummy5                   :2;//Reserved
         unsigned int On                       :1;
         unsigned int Rst                      :1;

         unsigned int Dummy4                   :1;//Reserved
         unsigned int SqFctX                   :1;//Reserved
         unsigned int MCLR                     :1;
         unsigned int FldSFctX                 :3;
         unsigned int SFct                     :1;
         unsigned int Fct                      :1;
      }Ib;
   }PZDIn;

   union{
      struct{
         UINT8 PZDOut[4];
      }PZDu8Out;
      struct{
         UINT16 ZSWOut1;
         UINT16 ZSWOut2;
      }PZDu16Out;
      struct{
         UINT32 ZSW;
      }PZDu32Out;
      struct{
         /*MSB->LSB*/
         unsigned int OLAlarm                  :1;
         unsigned int ManualAuto               :1;
         unsigned int ParaSetChangeAck         :1;
         unsigned int FldParaSetNbrX           :5;

         unsigned int FldHfSwitchX             :4;
         unsigned int HfSwitchError            :1;
         unsigned int TempError                :1;
         unsigned int EmgcyStopError           :1;
         unsigned int NOAlarm                  :1;

         unsigned int XOut1                    :1;
         unsigned int MCLR                     :1;
         unsigned int LMAlarm                  :1;
         unsigned int Ok                       :1;
         unsigned int On                       :1;
         unsigned int Off                      :1;
         unsigned int FctStopMode              :1;
         unsigned int TPAlarm                  :1;

         unsigned int CUAlarm                  :1;
         unsigned int HWAlarm                  :1;
         unsigned int CFAlarm                  :1;
         unsigned int NCAlarm                  :1;
         unsigned int EQAlarm                  :1;
         unsigned int WAAlarm                  :1;
         unsigned int CMAlarm                  :1;
         unsigned int SEAlarm                  :1;
      }Ob;
   }PZDOut;
}PACKED_POST;



//Expected parameter types which can be accessed for read or write from communication channel(PKW)
//from application point of view
typedef enum
{
   TYPE_VOID,  //not valid
   TYPE_8,     //8 bit
   TYPE_16,    //16 bit
   TYPE_32,    //32 bit
   TYPE_A_8,   //8 bit and part of an array
   TYPE_A_16,  //16 bit and part of an array
   TYPE_A_32,  //32 bit and part of an array
   TYPE_S8,    //signed 8 bit
   TYPE_S16,   //signed 16 bit
   TYPE_S32,   //signed 32 bit
   TYPE_A_S8,  //signed 8 bit and part of an array
   TYPE_A_S16, //signed 16 bit and part of an array
   TYPE_A_S32, //signed 32 bit and part of an array
}ParaType;


//Array the parameter is related to
enum ArrayType{
   ARRAYNA = 1,//Non Array parameter
   ARRAYPS,    //Parameter in ParameterSet array
   ARRAYHSPS,  //Parameter in Horn specific ParameterSet array
   ARRAYSS,    //Parameter in weld status array
   ARRAYHSSS,  //Parameter in horn specific status set status array
   ARRAYSN,    //Parameter in SysConfig.SerialNumber array
};

//Struct for representing an parameter accessed by Fieldbus
struct FieldbusParameter
{
   UINT32 ParamInfo;//Contains parameter number,Type(actual type 8,16 or 32 bit signed/unsigned array or non array variable),
                    //(PBType 8,16, or 32 bit array or non array variable). The PBtype is for already programmed PLCs
                    //for professional compact product.PBTYpe is the type which comes in AK field of PKW from master while accessing
                    //a parameter for read or write.
   void * ParamVal; //Pointer to param val
   void * MinVal;   //Pointer to param min val if exists else 0
   void * MaxVal;   //Pointer to param max val if exists else 0
   void * DfltVal;  //Pointer to param default val if exists else 0
   UINT32 ParamInfoExt;//Contains Array the parameter is related to, CRC to be affected in preset,Access rights of parameter.
};

//struct to map the analog values in Fieldbus Table.
//Values are updated in real time.
//values can be read/written using PKW in case of Profibus and using GET/SET single attribute service in case of
//Ethernet IP
struct EXTCTRL{
   //values can be read from Fielbus
   SINT16 AmplitudeSet;
   SINT16 Amplitude;
   SINT16 Current;
   SINT16 Power;
   SINT16 Phase;
   SINT16 PSV;
   UINT16 Frequency;
   SINT16 Temperature;
   //values can be written from Fielbus. Should affect ruuningpreset.amplitude
   SINT16 ExtAmplitudeSet;
   SINT16 ExtFrequencyOffset;
   SINT16 ExtRes1;
   SINT16 ExtRes2;
   SINT16 ExtRes3;
   SINT16 ExtRes4;
   SINT16 ExtRes5;
   SINT16 ExtRes6;
};

class FieldBus
{
public:
	FieldBus();
	//Every protocol class must have to implement following functions.
	//Cyclic data related functions
	virtual void UpdateOutputCyclic(UINT8* OutPtr) = 0;
	virtual void UpdateInputCyclic(UINT8* InPtr) = 0;
	//acyclic data related functions.
	virtual BOOLEAN GetWarmStartParams(CIFX_PACKET * WSPkt) = 0;
	virtual void ProcessMailBox(CIFX_PACKET * MBXPkt) = 0;
	virtual void ReadExtendedStatus(CIFXHANDLE HChannel) = 0;
	virtual void ProtocolSpecificInit(CIFXHANDLE HChannel);
	//functions common to all fieldbus protocols
	void RegisterApp(CIFXHANDLE HChannel);
	virtual void ReadCommonStatus(CIFXHANDLE HChannel);
	void ProcessControlSignal();
	void PrepareStatusSignal();
	static UINT16 CheckForRfswitchfeedback();
	static void SetRfSelect(UINT16 HornNumber);
	static SINT16 GetExternalAmplitude();
	static SINT16 GetExternalFrequencyOffset();
	virtual void GetFaultData(UINT32 FaultCode = 0);
	static BOOLEAN CheckForMemoryClear();
	UINT32 IODataExchangeSize;
};

extern FieldbusIO FbusIO;//, FbusIOTemp;
extern UINT8 DataformatWeb;
extern NETX_COMMON_STATUS_BLOCK CStat;
extern UINT16 Controlword1, Controlword2;
extern UINT16 Statusword1, Statusword2;
extern BOOLEAN HilscherSlaveCardDetected;
extern UINT16 FieldBusType;
extern EXTCTRL ExternalControl;
extern DcxPreset FbMaxPs , FbMinPs;
extern HSPRESET  FbMaxHsPs, FbMinHsPs;
FieldBus * FindFieldBusType();
void InitBaseIndexes(void);
void SetMinMaxParamForFbTable(PSUPDATESPECIAL PsUpdateSpcl = UPDATEMINMAX);
#endif /* FIELDBUS_H_ */
