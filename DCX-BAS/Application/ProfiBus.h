/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/ProfiBus.h_v   1.6   20 Nov 2013 06:11:48   rjamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/ProfiBus.h_v  $
 * 
 *    Rev 1.6   20 Nov 2013 06:11:48   rjamloki
 * Profibus default address changed. Watchdog default timeout changed.
 * 
 *    Rev 1.5   11 Mar 2013 02:28:08   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.4   18 Feb 2013 03:09:38   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.3   22 Jan 2013 10:38:28   ygupta
 * Profibus Bug Fixes
 * 
 *    Rev 1.2   17 Jan 2013 07:25:48   ygupta
 * Profibus Related changes
 * 
 *    Rev 1.1   16 Jan 2013 06:28:34   ygupta
 * Level2 Code with Profibus
*/
#include "FieldBus.h"
#include "ProfibusFspms_Public.h"
#ifndef PROFIBUS_H_
#define PROFIBUS_H_


/*Supported Gsd-File-Verion (derived always from "User-Parameter[4]"      */
#define GSD_VERSION_0                  0x00U
#define GSD_VERSION_1                  0x01U
#define GSD_VERSION_UNDEFINED          0xffU

//Data format for the data which is defined by configuration modules. Derived always from "User-Parameter array[index 5]"
#define UP_DATA_FORMAT_UNDEF           0x00U//Data format is undefined
#define UP_DATA_FORMAT_BE              0x01U//Data format is big endian
#define UP_DATA_FORMAT_LE              0x02U//Data format is little endian
#define UP_DATA_FORMAT_CFG_DEF         0x04U//Data format is defined by configuration modules.

//Expected length of User-Parameter array
#define LEN_PRM_GSD_VER_0              0x05
#define LEN_PRM_GSD_VER_1              0x19

//Defines for configuration modules.
//WC gets the configuration modules in configuration change indication from Hilscher slave card.
//Indication contains the configuration byte array and the length of array. Configuration array contains the module
//defined in GSD based on which the data exchange size and the offset of data in IO data exchange is detemined.
//Expected Data-Exchange Size (I/O-Size) in Bytes depends on GSD_VERSION and derived from configuration modules
//define in GSD
#define DATA_EX_SIZE_MIN_INIT            0//min exchange size
#define DATA_EX_SIZE_GSD_VER_0           12//max IO data exchange size in bytes for GSD Version 0
#define DATA_EX_SIZE_MIN_PER_MODULE_IDX  2//Minimum data exchange size in bytes per defined module in GSD
#define DATA_EX_SIZE_MAX_GSD_VER_1       48//max IO data exchange size in bytes for GSD Version 1
#define DATA_EX_SIZE_MAX                 DATA_EX_SIZE_MAX_GSD_VER_1//max data exchange size in bytes

///Expected length of array in bytes conatining configuration modules
#define LEN_CFG_GSD_VER_0                0x02//length for gsd version 0
//PROFIBUS-Configuration (>GSD_VER_0)(defines based on GSD)
#define CFG_BYTES_PER_MODULE_IDX         4//max bytes in a single configuration module e.g. 0xC1,0xC3,0xC3,0x01
#define CFG_MODULE_IDX_MAX               (DATA_EX_SIZE_MAX / DATA_EX_SIZE_MIN_PER_MODULE_IDX)//maximum 24 modules can be selected to define 48 byte IO exchange
#define LEN_CFG_MAX_GSD_VER_1            (CFG_MODULE_IDX_MAX * CFG_BYTES_PER_MODULE_IDX)//every module conatains 4 bytes so total 96 bytes
#define LEN_CFG_MAX_INIT                 LEN_CFG_MAX_GSD_VER_1//Expected maximun lenth of confguration module byte array(96)

//configuration modules for GSD Version 0. See profibus specification for detailed module description.
#define CFG_BYTE_0_GSD_VER_0           0xf3//4 word input output
#define CFG_BYTE_1_GSD_VER_0           0xf1//2 word input output

//configuration modules for GSD Version 1. See profibus specification for detailed module description.
#define CFG_MODULE_FREE_PLACE          0x03000000//place holder no IO
#define CFG_MODULE_PKW_BE              0xc1c3c301//4 word I/O (vendor specific byte = 01 Big Endian)
#define CFG_MODULE_PKW_LE              0xc1c3c311//4 word input output (vendor specific byte 11 = little Endian)
#define CFG_MODULE_PZD_BE              0xc1c1c102//2 word I/O (vendor specific byte = 02 Big Endian)
#define CFG_MODULE_PZD_LE              0xc1c1c112//2 word I/O (vendor specific byte = 12 Little endian)
#define CFG_MODULE_PZDV_1_BE           0xc1c0c003//1 word I/O (vendor specific byte = 03 big endian)
#define CFG_MODULE_PZDV_1_LE           0xc1c0c013//1 word I/O (vendor specific byte = 13 little endian)
#define CFG_MODULE_PZDV_2_BE           0xc1c0c004//1 word I/O (vendor specific byte = 04 big endian)
#define CFG_MODULE_PZDV_2_LE           0xc1c0c014//1 word I/O (vendor specific byte = 14 little endian)
#define CFG_MODULE_PZDV_3_BE           0xc1c0c005//1 word I/O (vendor specific byte = 05 big endian)
#define CFG_MODULE_PZDV_3_LE           0xc1c0c015//1 word I/O (vendor specific byte = 15 liitle endian)
#define CFG_MODULE_PZDV_4_BE           0xc1c0c006//1 word I/O (vendor specific byte = 06 big endian)
#define CFG_MODULE_PZDV_4_LE           0xc1c0c016//1 word I/O (vendor specific byte = 14 little endian)
#define CFG_MODULE_PZDSELV_1_BE        0xc1c1c107//2 word I/O (vendor specific byte = 07 big endian)
#define CFG_MODULE_PZDSELV_1_LE        0xc1c1c117//2 word I/O (vendor specific byte = 17 little endian)
#define CFG_MODULE_PZDSELV_2_BE        0xc1c1c108//2 word I/O (vendor specific byte = 08 big endian)
#define CFG_MODULE_PZDSELV_2_LE        0xc1c1c118//2 word I/O (vendor specific byte = 18 little endian)
#define CFG_MODULE_PZDSELV_3_BE        0xc1c1c109//2 word I/O (vendor specific byte = 09 big endian)
#define CFG_MODULE_PZDSELV_3_LE        0xc1c1c119//2 word I/O (vendor specific byte = 19 little endian)
#define CFG_MODULE_PZDSELV_4_BE        0xc1c1c10a//2 word I/O (vendor specific byte = 0a big endian)
#define CFG_MODULE_PZDSELV_4_LE        0xc1c1c11a//2 word I/O (vendor specific byte = 1a little endian)


//Below supported configuration modules by GSD are represented as single byte for easy application usage.
#define DATA_EX_CFG_UNDEF              0x00U
#define DATA_EX_CFG_PZD_BE             0x01U//=0xc1c1c102
#define DATA_EX_CFG_PZD_LE             0x02U//=0xc1c1c112
#define DATA_EX_CFG_PKW_BE             0x03U//=0xc1c3c301
#define DATA_EX_CFG_PKW_LE             0x04U//=0xc1c3c311
#define DATA_EX_CFG_PZDV_1_BE          0x05U//=0xc1c0c003
#define DATA_EX_CFG_PZDV_1_LE          0x06U//=0xc1c0c013
#define DATA_EX_CFG_PZDV_2_BE          0x07U//=0xc1c0c004
#define DATA_EX_CFG_PZDV_2_LE          0x08U//=0xc1c0c014
#define DATA_EX_CFG_PZDV_3_BE          0x09U//=0xc1c0c005
#define DATA_EX_CFG_PZDV_3_LE          0x0aU//=0xc1c0c015
#define DATA_EX_CFG_PZDV_4_BE          0x0bU//=0xc1c0c006
#define DATA_EX_CFG_PZDV_4_LE          0x0cU//=0xc1c0c016
#define DATA_EX_CFG_PZDSELV_1_BE       0x0dU//=0xc1c1c107
#define DATA_EX_CFG_PZDSELV_1_LE       0x0eU//=0xc1c1c117
#define DATA_EX_CFG_PZDSELV_2_BE       0x0fU//=0xc1c1c108
#define DATA_EX_CFG_PZDSELV_2_LE       0x10U//=0xc1c1c118
#define DATA_EX_CFG_PZDSELV_3_BE       0x11U//=0xc1c1c109
#define DATA_EX_CFG_PZDSELV_3_LE       0x12U//=0xc1c1c119
#define DATA_EX_CFG_PZDSELV_4_BE       0x13U//=0xc1c1c10a
#define DATA_EX_CFG_PZDSELV_4_LE       0x14U//=0xc1c1c11a
#define DATA_EX_CFG_END_MARK           0x15U//Value to mark the end of configuration
#define DATA_EX_CFG_END_VALUE          (DATA_EX_CFG_END_MARK + 1)//value to break from loop


//defines for analog IO selection on profibus PZSELV channel
#define AN_IN_SEL_SET_VAL_P_CONT_AMPLITUDE		1
#define AN_IN_SEL_MAX							31

#define AN_OUT_SEL_SET_VAL_S_CONT_AMPLITUDE		1
#define AN_OUT_SEL_AMPLITUDE					20
#define AN_OUT_SEL_CURRENT						21
#define AN_OUT_SEL_POWER						22
#define AN_OUT_SEL_PHASE						23
#define AN_OUT_SEL_PSV							26
#define AN_OUT_SEL_FREQUENCY					27
#define AN_OUT_SEL_TEMPERATURE					28
#define AN_OUT_SEL_MAX							63
#define HS_NO_SELECTION_ALL                  0x00000000
#define HS_IN_NO_SELECTION                0x00
#define HS_IN_SELECTION_ERROR             0xff
#define HS_OUT_NO_SELECTION                  0x00
#define HS_OUT_SELECTION_ERROR               0xff
#define PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED      0xffU
#define NOT_SELECTED 0x00
#define NUM_PZDV_MODULES  4

//defines for error code field in PKW from slave to master
#define PKW_PKE_ERR_SHIFT        0
#define PKW_PKE_ERR_MASK         0x00ff

//defines for answer code field in PKW from master
#define PKW_PKE_AK_MASK          0xf000
#define PKW_PKE_AK_SHIFT         12

//Masks for special access of parameters in IND field of PKW
#define SPEC_ACC_IDX_MASK_STANDART        0x0000U
#define SPEC_ACC_IDX_MASK_READ_DEFAULT    0x0100U
#define SPEC_ACC_IDX_MASK_READ_LIMIT_LOW  0x0200U
#define SPEC_ACC_IDX_MASK_READ_LIMIT_HIGH 0x0400U
#define SPEC_ACC_IDX_MASK_WRITE_VOLATILE  0x0100U

//Instruction/Answer code in AK field
#define AK_NOAK		0
#define AK_READ8	1
#define AK_READ16	2
#define AK_READ32	3
#define AK_READA8	4
#define AK_READA16	5
#define AK_READA32	6
#define AK_WRITE8	7
#define AK_WRITE16	8
#define AK_WRITE32	9
#define AK_WRITEA8	10
#define AK_WRITEA16	11
#define AK_WRITEA32	12


//Acces token, and some special commands(like move preset from one slot to another) are
//represented by these defines
#define OS_PARA_SET                    1000
#define OS_SS_PARA_SET                 1400
#define OS_SS_STATUS_SET               1600
#define OS_STATUS_SET                  1200

//profibus slave address default,min and max
#define PB_DEFAULT_ADDR 125
#define PB_MIN_ADDR 0
#define PB_MAX_ADDR 126
#define PROFIBUS_UNIQUEIDENT ((UINT16)0x0E05)//unique for DCX product

//related to special commands when last weld status or horn status is requested
#define LAST_NOT_AVAILABLE       0x01U
#define LAST_OK                  0x02U
#define LAST_OK_NEW              0x04U
#define LAST_OK_NEW_OLD_OVERRUN     0x08U

//Error code to be sent to master on parameter access through communication channel(PKW)
typedef enum
   {
   VAL_ERR_NO_ERR = 0,
   VAL_ERR_INVALID_AK=1,
   VAL_ERR_INVALID_VAL_NBR=2,
   VAL_ERR_TYPE_MISMATCH=3,
   VAL_ERR_SPEC_ACCESS_MISMATCH=4,
   VAL_ERR_INDEX_MISMATCH=4,
   VAL_ERR_NOT_WRITEABLE=5,
   VAL_ERR_NO_ACCESS_COMM_IFC_TOKEN=6,
   VAL_ERR_NO_ACCESS_SEC_PROTECTED=7,
   VAL_ERR_LIMIT_LOW=8,
   VAL_ERR_LIMIT_HIGH=9,
   VAL_ERR_NV_WRITE_FAILED_ECAT1=10,
   VAL_ERR_NO_ACCESS_ECAT1=11,
   VAL_ERR_MISC=50,
   VAL_ERR_NOTIMPLEMENTED = 60,//added for now for unimplemented function
   }ParamAccessError;

//Special Access Type in Ind high byte
typedef enum
   {
   SPEC_ACC_ERROR,
   SPEC_ACC_IN_INDEX,
   SPEC_ACC_STANDARD,
   SPEC_ACC_READ_DEFAULT,
   SPEC_ACC_READ_LIMIT_LOW,
   SPEC_ACC_READ_LIMIT_HIGH,
   SPEC_ACC_WRITE_VOLATILE
   }ParamSpecialAccess;

//Analog IO Content Exchange from profibus related struct
struct ContExch
{
   //Input Selection value
   UINT16 ValueIn[NUM_PZDV_MODULES];
   //Output selction value
   UINT16 ValueOut[NUM_PZDV_MODULES];
   //Input Selection
   UINT8 SelIn[NUM_PZDV_MODULES];
   //Input Selection Acknowledgment
   UINT8 SelAckIn[NUM_PZDV_MODULES];
   //Output Selection
   UINT8 SelOut[NUM_PZDV_MODULES];
   //Output Selection Acknowledgment
   UINT8 SelAckOut[NUM_PZDV_MODULES];
   //Input selection offsets in IO data bytes
   UINT8 InPZDSVSelectOffset[NUM_PZDV_MODULES];
   //Output selection offsets in IO data bytes
   UINT8 OutPZDZVSelectOffset[NUM_PZDV_MODULES];
};

extern ContExch ContIO;
extern DcxPreset CommPS;
extern WSTATUS CommWeldSS;
extern HSPRESET  CommHSPS;
extern HSSTATUS CommHSSS[MAX_HS_FUNC];

extern BOOLEAN LastSSCopied;
extern UINT32 SSOverRunIndication;
extern UINT8 StatusSetNumber;

extern UINT8 HSStatusSetNumber;
extern UINT8 SFCT;
extern UINT8 LastSFCTInHSStatus[];
extern BOOLEAN LastHSSSCopied;
extern UINT8 HSSSOverRunIndication[];

ParamAccessError HandleValue(UINT16 ValNbr, ParaType eType, UINT16 ArrayIndex, UINT8 Access, void * Value);
ParamAccessError WriteParam(ParaType Type, ParamSpecialAccess SepcAcc, BOOLEAN TokenAvailable, void * Val, UINT32 ArrayIndex,
UINT16 ParamNumber);
ParamAccessError ReadParam(ParaType Type, ParamSpecialAccess SepcAcc, void * Val, UINT32 ArrayIndex,
UINT16 ParamNumber);

class ProfiBus: public FieldBus
{
public:
	ProfiBus();
	virtual void UpdateOutputCyclic(UINT8* OutPtr);
	virtual void UpdateInputCyclic(UINT8* InPtr);
	virtual void ProcessMailBox(CIFX_PACKET * MBXPkt);
	virtual BOOLEAN GetWarmStartParams(CIFX_PACKET * WSPkt);
	void ReadExtendedStatus(CIFXHANDLE HChannel);
	BOOLEAN HandleNewParameterData(void);
	BOOLEAN HandleNewConfigData(void);
	void ProcessPKW(void);
	void  ProcessAnalogInput(void);
	void  ProcessAnalogOutput(void);
	UINT8 AK;
	UINT16 PNU;
	UINT16 IND;
	UINT32 PWE;
};

extern BOOLEAN ParameterFault , ConfigurationFault;
extern FSPMS_EXTENDED_DIAG_T EStat;
#endif /* PROFIBUS_H_ */
