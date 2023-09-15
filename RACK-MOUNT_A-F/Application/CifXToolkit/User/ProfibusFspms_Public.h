/**************************************************************************************

  Copyright (C) 2010 Hilscher Gesellschaft für Systemautomation mbH.

  This program can be used by everyone according the "industrialNETworX Public License INPL".

  The license can be downloaded under <http://www.industrialNETworX.com>.

 **************************************************************************************

   Last Modification:
    @version $Id:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/CifXToolkit/User/ProfibusFspms_Public.h_v   1.0   27 Jul 2015 08:56:46   sesharma  $

   Description:
    Task Profibus Fspms
    public declaration of task
      - queue packets

   Changes:

     Date        Author        Description
     ----------------------------------------------------------------------------------
     2010-03-23  RH            Created
**************************************************************************************/
#ifndef __PROFIBUS_FSPMS_PUBLIC_H
#define __PROFIBUS_FSPMS_PUBLIC_H


#include "ProfibusFspm_Common.h"

/****************************************************************************************
 * Profibus Fspms, constants
 * !!! add additional public constants
 */
  #define PROFIBUS_FSPMS_PROCESS_QUEUE_NAME   "PB_FSPMS_QUE"



  #define PROFIBUS_FSPMS_MAX_CFG_DATA_SIZE 244           /* Maximum supported config size */

  #define PROFIBUS_FSPMS_MAX_SET_SLAVE_ADD_DATA_SIZE 240 /* Maximum supported set slave address permanent user data */

  #define PROFIBUS_FSPMS_MAX_EXT_USER_PRM_DATA_SIZE 244  /* Maximum supported extended user parameter data size */

  #define PROFIBUS_FSPMS_MAX_INPUT_DATA_SIZE 244         /* Maximum supported input data length */
  #define PROFIBUS_FSPMS_MAX_OUTPUT_DATA_SIZE 244        /* Maximum supported output data length */

  #define PROFIBUS_FSPMS_MAX_ACYCLIC_DATA_SIZE 244       /* Maximum supported acyclic data length */
  #define PROFIBUS_FSPMS_MAX_ACYCLIC_CLASS2_DATA_SIZE 240/* Maximum supported acyclic class2 data length */

  #define PROFIBUS_FSPMS_MAX_READ_DATA_SIZE 240          /* Maximum supported acyclic read data length */
  #define PROFIBUS_FSPMS_MAX_WRITE_DATA_SIZE 240         /* Maximum supported acyclic write data length */

  #define PROFIBUS_FSPMS_MAX_DATA_TRANSPORT_DATA_SIZE 240 /* Maximum supported acyclic transport data length */

  #define PROFIBUS_FSPMS_MAX_INITIATE_ADD_TABLE_SIZE  234 /* Maximum supported acyclic initiate address data length */

  #define PROFIBUS_FSPMS_MAX_ALARM_USERDATA_SIZE 59       /* Maximum allowed user specific Alarm data */

  /* ErrorDecode, ErrorClass1 and 2 field */
  #define PROFIBUS_FSPMS_ERROR_DECODE_DPV1 128
  #define PROFIBUS_FSPMS_ERROR_CLASS_APPLICATION 0xA0 /* Application error class */
  #define PROFIBUS_FSPMS_ERROR_CLASS_ACCESS      0xB0 /* Access error class */
  #define PROFIBUS_FSPMS_ERROR_CLASS_RESOURCE    0xC0 /* Resource error class */

  #define PROFIBUS_FSPMS_ERROR_CODE_APPL_READ          0x00
  #define PROFIBUS_FSPMS_ERROR_CODE_APPL_WRITE         0x01
  #define PROFIBUS_FSPMS_ERROR_CODE_APPL_MODULE        0x02
  #define PROFIBUS_FSPMS_ERROR_CODE_APPL_VERSION       0x08
  #define PROFIBUS_FSPMS_ERROR_CODE_APPL_NOT_SUPPORTED 0x09

  #define PROFIBUS_FSPMS_ERROR_CODE_ACC_INDEX             0x00
  #define PROFIBUS_FSPMS_ERROR_CODE_ACC_WRITE_LENGTH      0x01
  #define PROFIBUS_FSPMS_ERROR_CODE_ACC_SLOT_INVALID      0x02
  #define PROFIBUS_FSPMS_ERROR_CODE_ACC_TYPE_CONFLICT     0x03
  #define PROFIBUS_FSPMS_ERROR_CODE_ACC_AREA_INVALID      0x04
  #define PROFIBUS_FSPMS_ERROR_CODE_ACC_STATE_CONFLICT    0x05
  #define PROFIBUS_FSPMS_ERROR_CODE_ACC_ACCESS_DENIED     0x06
  #define PROFIBUS_FSPMS_ERROR_CODE_ACC_RANGE_INVALID     0x07
  #define PROFIBUS_FSPMS_ERROR_CODE_ACC_PARAMETER_INVALID 0x08
  #define PROFIBUS_FSPMS_ERROR_CODE_ACC_TYPE_INVALID      0x09

  #define PROFIBUS_FSPMS_ERROR_CODE_RSC_READ_CONSTRAIN    0x00
  #define PROFIBUS_FSPMS_ERROR_CODE_RSC_WRITE_CONSTRAIN   0x01
  #define PROFIBUS_FSPMS_ERROR_CODE_RSC_BUSY              0x02
  #define PROFIBUS_FSPMS_ERROR_CODE_RSC_UNAVAILABLE       0x03

  #define PROFIBUS_FSPMS_ALARM_ENABLED_PULL_PLUG             0x80
  #define PROFIBUS_FSPMS_ALARM_ENABLED_PROCESS               0x40
  #define PROFIBUS_FSPMS_ALARM_ENABLED_DIAGNOSTIC            0x20
  #define PROFIBUS_FSPMS_ALARM_ENABLED_MANUFACTURER_SPECIFIC 0x10
  #define PROFIBUS_FSPMS_ALARM_ENABLED_STATUS                0x08
  #define PROFIBUS_FSPMS_ALARM_ENABLED_UPDATE                0x04


  #define PROFIBUS_FSPMS_MSAC2_SUBNET_NO                     0x00
  #define PROFIBUS_FSPMS_MSAC2_SUBNET_LOCAL                  0x01
  #define PROFIBUS_FSPMS_MSAC2_SUBNET_REMOTE                 0x02

  #define PROFIBUS_FSPMS_MSAC2_INSTANCE_DLL                  0x00
  #define PROFIBUS_FSPMS_MSAC2_INSTANCE_MSAC2                0x10
  #define PROFIBUS_FSPMS_MSAC2_INSTANCE_USER                 0x20

  #define PROFIBUS_FSPMS_MSAC2_ERR_ABT_FE                    0x02
  #define PROFIBUS_FSPMS_MSAC2_ERR_ABT_RE                    0x04
  #define PROFIBUS_FSPMS_MSAC2_ERR_ABT_SE                    0x01
  #define PROFIBUS_FSPMS_MSAC2_ERR_ABT_TO                    0x03
  #define PROFIBUS_FSPMS_MSAC2_ERR_ABT_IA                    0x07

  #define PROFIBUS_FSPMS_MSAC2_INITIATE                      0x00
  #define PROFIBUS_FSPMS_MSAC2_VALID_SERVICE                 0x01

  #define PROFIBUS_MAX_MODULE                                0x18

  typedef enum PROFIBUS_FSPMS_DATA_RATEtag{
    PROFIBUS_FSPMS_DATA_RATE_96     =  0,
    PROFIBUS_FSPMS_DATA_RATE_19_2   =  1,
    PROFIBUS_FSPMS_DATA_RATE_93_75  =  2,
    PROFIBUS_FSPMS_DATA_RATE_187_5  =  3,
    PROFIBUS_FSPMS_DATA_RATE_500    =  4,
    PROFIBUS_FSPMS_DATA_RATE_1500   =  6,
    PROFIBUS_FSPMS_DATA_RATE_3000   =  7,
    PROFIBUS_FSPMS_DATA_RATE_6000   =  8,
    PROFIBUS_FSPMS_DATA_RATE_12000  =  9,
    PROFIBUS_FSPMS_DATA_RATE_31_25  = 10,
    PROFIBUS_FSPMS_DATA_RATE_45_45  = 11,
    PROFIBUS_FSPMS_DATA_RATE_AUTO   = 15
  } PROFIBUS_FSPMS_DATA_RATE;


/****************************************************************************************
* Profibus Fspms, command codes  */

  /* Start of the reserved area from 0x400 - 0x4FF for Profibus FSPMS commands */
#define  PROFIBUS_FSPMS_PACKET_COMMAND_START              0x00000400


  /* Summary of the Profibus FSPMS (Fieldbus Service Protocol Machine - Slave) service commands and primitives */
#define  PROFIBUS_FSPMS_CMD_END_PROCESS_REQ               0x00000400
#define  PROFIBUS_FSPMS_CMD_END_PROCESS_CNF               0x00000401

#define  PROFIBUS_FSPMS_CMD_INIT_MS0_REQ                  0x00000402
#define  PROFIBUS_FSPMS_CMD_INIT_MS0_CNF                  0x00000403

#define  PROFIBUS_FSPMS_CMD_INIT_MS1_REQ                  0x00000404
#define  PROFIBUS_FSPMS_CMD_INIT_MS1_CNF                  0x00000405

#define  PROFIBUS_FSPMS_CMD_INIT_MS2_REQ                  0x00000406
#define  PROFIBUS_FSPMS_CMD_INIT_MS2_CNF                  0x00000407

#define  PROFIBUS_FSPMS_CMD_ABORT_REQ                     0x00000408
#define  PROFIBUS_FSPMS_CMD_ABORT_CNF                     0x00000409

#define  PROFIBUS_FSPMS_CMD_GET_OUTPUT_REQ                0x0000040A
#define  PROFIBUS_FSPMS_CMD_GET_OUTPUT_CNF                0x0000040B

#define  PROFIBUS_FSPMS_CMD_SET_INPUT_REQ                 0x0000040C
#define  PROFIBUS_FSPMS_CMD_SET_INPUT_CNF                 0x0000040D

#define  PROFIBUS_FSPMS_CMD_SET_SLAVE_DIAG_REQ            0x0000040E
#define  PROFIBUS_FSPMS_CMD_SET_SLAVE_DIAG_CNF            0x0000040F

#define  PROFIBUS_FSPMS_CMD_APPLICATION_READY_REQ         0x00000412
#define  PROFIBUS_FSPMS_CMD_APPLICATION_READY_CNF         0x00000413

#define  PROFIBUS_FSPMS_CMD_CHECK_CFG_IND                 0x00000414
#ifdef DPS_VERSION_V2_0_0_X
#define  PROFIBUS_FSPMS_CMD_CHECK_CFG_INDRET              0x00000415
#else
#define  PROFIBUS_FSPMS_CMD_CHECK_CFG_RES                 0x00000415
#endif

#define  PROFIBUS_FSPMS_CMD_CHECK_USER_PRM_IND            0x00000416
#ifdef DPS_VERSION_V2_0_0_X
#define  PROFIBUS_FSPMS_CMD_CHECK_USER_PRM_INDRET         0x00000417
#else
#define  PROFIBUS_FSPMS_CMD_CHECK_USER_PRM_RES            0x00000417
#endif

#define  PROFIBUS_FSPMS_CMD_GLOBAL_CONTROL_IND            0x00000418
#define  PROFIBUS_FSPMS_CMD_GLOBAL_CONTROL_INDRET         0x00000419

#define  PROFIBUS_FSPMS_CMD_SET_SLAVE_ADD_IND             0x0000041A
#define  PROFIBUS_FSPMS_CMD_SET_SLAVE_ADD_INDRET          0x0000041B

#define  PROFIBUS_FSPMS_CMD_NEW_OUTPUT_IND                0x0000041C
#define  PROFIBUS_FSPMS_CMD_NEW_OUTPUT_INDRET             0x0000041D

#define  PROFIBUS_FSPMS_CMD_SET_IM0_REQ                   0x00000420
#define  PROFIBUS_FSPMS_CMD_SET_IM0_CNF                   0x00000421

#define  PROFIBUS_FSPMS_CMD_IM_READ_IND                   0x00000422
#define  PROFIBUS_FSPMS_CMD_IM_READ_RES                   0x00000423

#define  PROFIBUS_FSPMS_CMD_IM_WRITE_IND                  0x00000424
#define  PROFIBUS_FSPMS_CMD_IM_WRITE_RES                  0x00000425

#define  PROFIBUS_FSPMS_CMD_SET_CFG_REQ                   0x0000042E
#define  PROFIBUS_FSPMS_CMD_SET_CFG_CNF                   0x0000042F

#ifdef DPS_VERSION_V2_0_0_X
  #define  PROFIBUS_FSPMS_CMD_CHECK_CFG_RESULT_REQ        0x00000430
  #define  PROFIBUS_FSPMS_CMD_CHECK_CFG_RESULT_CNF        0x00000431

  #define  PROFIBUS_FSPMS_CMD_CHECK_USER_PRM_RESULT_REQ   0x00000432
  #define  PROFIBUS_FSPMS_CMD_CHECK_USER_PRM_RESULT_CNF   0x00000433
#endif

#define  PROFIBUS_FSPMS_CMD_WATCHDOG_TIMER_REQ            0x00000436
#define  PROFIBUS_FSPMS_CMD_WATCHDOG_TIMER_CNF            0x00000437

#define  PROFIBUS_FSPMS_CMD_CHECK_EXT_USER_PRM_IND        0x00000438
#ifdef DPS_VERSION_V2_0_0_X
  #define  PROFIBUS_FSPMS_CMD_CHECK_EXT_USER_PRM_INDRET   0x00000439
#else
  #define  PROFIBUS_FSPMS_CMD_CHECK_EXT_USER_PRM_RES      0x00000439
#endif

#define  PROFIBUS_FSPMS_CMD_STARTED_IND                   0x00000442
#define  PROFIBUS_FSPMS_CMD_STARTED_INDRET                0x00000443

#define  PROFIBUS_FSPMS_CMD_STOPPED_IND                   0x00000444
#define  PROFIBUS_FSPMS_CMD_STOPPED_INDRET                0x00000445

#define  PROFIBUS_FSPMS_CMD_STATE_CHANGED_IND             0x00000448
#define  PROFIBUS_FSPMS_CMD_STATE_CHANGED_INDRET          0x00000449

#define  PROFIBUS_FSPMS_CMD_REGISTER_DIAG_STRUCT_REQ		  0x0000044A
#define  PROFIBUS_FSPMS_CMD_REGISTER_DIAG_STRUCT_CNF		  0x0000044B

#define  PROFIBUS_FSPMS_CMD_IND_SETTING_REQ               0x0000044C
#define  PROFIBUS_FSPMS_CMD_IND_SETTING_CNF               0x0000044D

#define  PROFIBUS_FSPMS_CMD_RESET_REQ                     0x0000044E
#define  PROFIBUS_FSPMS_CMD_RESET_CNF                     0x0000044F

#define  PROFIBUS_FSPMS_CMD_BAUDDETECT_TIMER_REQ          0x00000450
#define  PROFIBUS_FSPMS_CMD_BAUDDETECT_TIMER_CNF          0x00000451

#define  PROFIBUS_FSPMS_CMD_SET_STAT_DIAG_REQ             0x00000452
#define  PROFIBUS_FSPMS_CMD_SET_STAT_DIAG_CNF             0x00000453


#define  PROFIBUS_FSPMS_CMD_C1_ALARM_NOTIFICATION_REQ     0x00000480
#define  PROFIBUS_FSPMS_CMD_C1_ALARM_NOTIFICATION_CNF     0x00000481

#define  PROFIBUS_FSPMS_CMD_C1_ALARM_ACK_IND              0x00000482
#define  PROFIBUS_FSPMS_CMD_C1_ALARM_ACK_RES              0x00000483

#define  PROFIBUS_FSPMS_CMD_C1_READ_IND                   0x00000484
#define  PROFIBUS_FSPMS_CMD_C1_READ_RES                   0x00000485

#define  PROFIBUS_FSPMS_CMD_C1_WRITE_IND                  0x00000486
#define  PROFIBUS_FSPMS_CMD_C1_WRITE_RES                  0x00000487


#define  PROFIBUS_FSPMS_CMD_C2_TIMER_EXPIRED_IND          0x000004A0
#define  PROFIBUS_FSPMS_CMD_C2_TIMER_EXPIRED_RES          0x000004A1

#define  PROFIBUS_FSPMS_CMD_C2_INITIATE_IND               0x000004A2
#define  PROFIBUS_FSPMS_CMD_C2_INITIATE_RES               0x000004A3

#define  PROFIBUS_FSPMS_CMD_C2_READ_IND                   0x000004A4
#define  PROFIBUS_FSPMS_CMD_C2_READ_RES                   0x000004A5

#define  PROFIBUS_FSPMS_CMD_C2_WRITE_IND                  0x000004A6
#define  PROFIBUS_FSPMS_CMD_C2_WRITE_RES                  0x000004A7

#define  PROFIBUS_FSPMS_CMD_C2_DATA_TRANSPORT_IND         0x000004A8
#define  PROFIBUS_FSPMS_CMD_C2_DATA_TRANSPORT_RES         0x000004A9

#define  PROFIBUS_FSPMS_CMD_C2_ABORT_IND                  0x000004AA
#define  PROFIBUS_FSPMS_CMD_C2_ABORT_RES                  0x000004AB

#define PROFIBUS_FSPMS_CMD_UPDATE_IO_LENGTH_DIAG_DATA_REQ             0x000004AC
#define PROFIBUS_FSPMS_CMD_UPDATE_IO_LENGTH_DIAG_DATA_CNF             0x000004AD


/****************************************************************************************
 * Profibus Fspms, types
 * !!! add additional public types
 */


/****************************************************************************************
* Profibus Fspms, packets and corresponding structures */

/* pragma pack */
#ifdef PRAGMA_PACK_ENABLE
#pragma PRAGMA_PACK_1(PROFIBUS_FSPMS_PUBLIC)
#endif

  /*
   * Structures of all diagnostic information of the tasks
   */
#if 0
  typedef __TLR_PACKED_PRE struct FSPMS_EXTENDED_DIAG_Ttag
  {
    TLR_UINT32  ulBusAddress;
    TLR_UINT32  ulIdentNumber;
		TLR_UINT32	ulBaudrate;
		TLR_UINT16  usOutputLength;
		TLR_UINT16  usInputLength;
  }__TLR_PACKED_POST FSPMS_EXTENDED_DIAG_T;


  typedef __TLR_PACKED_PRE struct FSPMS_CFG_DATA_Ttag
  {
    TLR_UINT uCfgDataLen;
    TLR_UINT8 abCfgData[PROFIBUS_FSPMS_MAX_CFG_DATA_SIZE];
  }__TLR_PACKED_POST FSPMS_CFG_DATA_T;
#endif

  typedef __TLR_PACKED_PRE struct FSPMS_IM0_Ttag
  {
    TLR_UINT8     abManufactor[10];
    TLR_UINT16    usManufactorId;
    TLR_UINT8     abOrder_Id[20];
    TLR_UINT8     abSerialNumber[16];
    TLR_UINT16    usHwRevision;
    TLR_UINT8     abSwRevision[4];
    TLR_UINT16    usRevisionCounter;
    TLR_UINT16    usProfileId;
    TLR_UINT16    usProfileSpecType;
    TLR_UINT16    usIMVersion;
    TLR_UINT16    usIMSupported;
  }__TLR_PACKED_POST FSPMS_IM0_T;

  typedef __TLR_PACKED_PRE struct FSPMS_IM_Ttag
  {
    FSPMS_IM0_T tIM0;
    TLR_BOOLEAN fProfSpecSupp;
    TLR_BOOLEAN fManuSpecSupp;
    TLR_BOOLEAN fSlotsNotZeroSupp;
  }__TLR_PACKED_POST FSPMS_IM_T;

  /*
   * Structures of all Request/Confirmation commands the task is able to send and receive
   *
   * Request and Confirmation Packets PROFIBUS_PACKET_FSPMS_xx_REQ/CNF
   * (xx = Command)
   *
   * Use the same order as the commands in TLR_Commands.h
   * PROFIBUS_FSPMS_xx_REQ/CNF (xx = Command)!
   */


/*
 * incoming request packets
 * structures of all request/confirmation commands the task is able to receive/sent
 */
  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_INIT_MS0_REQ_Ttag {
    TLR_BOOLEAN8 fSyncSupported;
    TLR_BOOLEAN8 fFreezeSupported;
    TLR_BOOLEAN8 fNoAddChg;
    TLR_BOOLEAN8 fFailSafeSupp;
    TLR_BOOLEAN8 fDpv1Enabled;
    TLR_UINT16 usIdentNumber;
    TLR_UINT8 bSlaveAddr;
    TLR_UINT8 bDataRate;

    TLR_UINT8 abRealCfgData[PROFIBUS_FSPMS_MAX_CFG_DATA_SIZE];
  }__TLR_PACKED_POST PROFIBUS_FSPMS_INIT_MS0_REQ_T;

  #define PROFIBUS_FSPMS_INIT_MS0_REQ_SIZE (sizeof(PROFIBUS_FSPMS_INIT_MS0_REQ_T)-PROFIBUS_FSPMS_MAX_CFG_DATA_SIZE)

  /* Request-Packet for the starting the Master-Slave cyclic state machine */
  typedef struct PROFIBUS_FSPMS_PACKET_INIT_MS0_REQ_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_INIT_MS0_REQ_T tData;
  } PROFIBUS_FSPMS_PACKET_INIT_MS0_REQ_T;

  typedef struct PROFIBUS_FSPMS_INIT_MS0_CNF_Ttag {
    TLR_UINT32 ulFSPMS0Id; /* Handle to the FSPMS end point */
    TLR_HANDLE hOutputTriB;
    TLR_HANDLE hInputTriB;
  } PROFIBUS_FSPMS_INIT_MS0_CNF_T;

  /* Confirmation-Packet when the Master-Slave cyclic state machine has been started */
  typedef struct PROFIBUS_FSPMS_PACKET_INIT_MS0_CNF_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_INIT_MS0_CNF_T tData;
  } PROFIBUS_FSPMS_PACKET_INIT_MS0_CNF_T;

  /* ******************************************************************************************* */
//  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_CMD_RESET_REQ_Ttag {
//  }__TLR_PACKED_POST PROFIBUS_FSPMS_CMD_RESET_REQ_T;

  #define PROFIBUS_FSPMS_CMD_RESET_REQ_SIZE 0

  /* Request-Packet for the starting the Master-Slave cyclic state machine */
  typedef struct PROFIBUS_FSPMS_PACKET_CMD_RESET_REQ_Ttag {
    TLR_PACKET_HEADER_T tHead;
//    PROFIBUS_FSPMS_CMD_RESET_REQ_T tData;
  } PROFIBUS_FSPMS_PACKET_CMD_RESET_REQ_T;

//  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_CMD_RESET_CNF_Ttag {
//  }__TLR_PACKED_POST PROFIBUS_FSPMS_CMD_RESET_CNF_T;

  #define PROFIBUS_FSPMS_CMD_RESET_CNF_SIZE 0

  /* Request-Packet for the starting the Master-Slave cyclic state machine */
  typedef struct PROFIBUS_FSPMS_PACKET_CMD_RESET_CNF_Ttag {
    TLR_PACKET_HEADER_T tHead;
//    PROFIBUS_FSPMS_CMD_RESET_CNF_T tData;
  } PROFIBUS_FSPMS_PACKET_CMD_RESET_CNF_T;




  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_IND_SETTING_REQ_Ttag {
    TLR_BOOLEAN8 fOutputIndDeact;                 /* deactivate the output indication */
  }__TLR_PACKED_POST PROFIBUS_FSPMS_IND_SETTING_REQ_T;

  #define PROFIBUS_FSPMS_IND_SETTING_REQ_SIZE (sizeof(PROFIBUS_FSPMS_IND_SETTING_REQ_T))

  /* Request-Packet for the starting the Master-Slave cyclic state machine */
  typedef struct PROFIBUS_FSPMS_PACKET_IND_SETTING_REQ_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_IND_SETTING_REQ_T tData;
  } PROFIBUS_FSPMS_PACKET_IND_SETTING_REQ_T;



  #define PROFIBUS_FSPMS_IND_SETTING_CNF_SIZE 0

  /* Request-Packet for the starting the Master-Slave cyclic state machine */
  typedef struct PROFIBUS_FSPMS_PACKET_IND_SETTING_CNF_Ttag {
    TLR_PACKET_HEADER_T tHead;
  } PROFIBUS_FSPMS_PACKET_IND_SETTING_CNF_T;


  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_INIT_MS1_REQ_Ttag {
    TLR_UINT8 bAlarmModeSlave; /* Specifies the number of maximum possible active alarms */
    TLR_UINT8 bAlarmsSupported; /* Variable that indicates the types of alarms which shall be supported */
  }__TLR_PACKED_POST  PROFIBUS_FSPMS_INIT_MS1_REQ_T;

  /* Request-Packet for the starting the Master Class1 -Slave acyclic state machine */
  typedef struct PROFIBUS_FSPMS_PACKET_INIT_MS1_REQ_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_INIT_MS1_REQ_T tData;
  } PROFIBUS_FSPMS_PACKET_INIT_MS1_REQ_T;

  typedef struct PROFIBUS_FSPMS_INIT_MS1_CNF_Ttag {
    TLR_UINT32 ulFSPMS1Id; /* Handle to the FSPMS end point */
  } PROFIBUS_FSPMS_INIT_MS1_CNF_T;

  /* Confirmation-Packet when the Master Class1-Slave acyclic state machine has been started */
  typedef struct PROFIBUS_FSPMS_PACKET_INIT_MS1_CNF_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_INIT_MS1_CNF_T tData;
  } PROFIBUS_FSPMS_PACKET_INIT_MS1_CNF_T;


  /* ******************************************************************************************* */
  /* Request-Packet for the starting the Master Class2 -Slave acyclic state machine */
  typedef struct PROFIBUS_FSPMS_PACKET_INIT_MS2_REQ_Ttag {
    TLR_PACKET_HEADER_T tHead;
  } PROFIBUS_FSPMS_PACKET_INIT_MS2_REQ_T;

  typedef struct PROFIBUS_FSPMS_INIT_MS2_CNF_Ttag {
    TLR_UINT32 ulFSPMS2Id; /* Handle to the FSPMS end point */
  } PROFIBUS_FSPMS_INIT_MS2_CNF_T;

  /* Confirmation-Packet when the Master Class2-Slave acyclic state machine has been started */
  typedef struct PROFIBUS_FSPMS_PACKET_INIT_MS2_CNF_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_INIT_MS2_CNF_T tData;
  } PROFIBUS_FSPMS_PACKET_INIT_MS2_CNF_T;



  /* ******************************************************************************************* */
  /* Request-Packet to declare the application as ready */
  typedef struct PROFIBUS_FSPMS_PACKET_DP_SLAVE_APPLICATION_READY_REQ_Ttag {
    TLR_PACKET_HEADER_T tHead;
  } PROFIBUS_FSPMS_PACKET_DP_SLAVE_APPLICATION_READY_REQ_T;

  /* Confirmation-Packet */
  typedef struct PROFIBUS_FSPMS_PACKET_DP_SLAVE_APPLICATION_READY_CNF_Ttag {
    TLR_PACKET_HEADER_T    tHead;
  } PROFIBUS_FSPMS_PACKET_DP_SLAVE_APPLICATION_READY_CNF_T;


  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_SET_SLAVE_DIAG_REQ_Ttag {
    TLR_BOOLEAN8 fExtDiagOverflow;
    TLR_BOOLEAN8 fExtDiagFlag;
    TLR_UINT8 abExtDiagData[PROFIBUS_FSPM_MAX_DIAG_DATA_SIZE]; /* Diagnostic data that'll be sent */
  }__TLR_PACKED_POST  PROFIBUS_FSPMS_SET_SLAVE_DIAG_REQ_T;

  #define PROFIBUS_FSPMS_SET_SLAVE_DIAG_REQ_SIZE (sizeof(PROFIBUS_FSPMS_SET_SLAVE_DIAG_REQ_T)-PROFIBUS_FSPM_MAX_DIAG_DATA_SIZE)

  /* Request-Packet to set a slave diagnostic */
  typedef struct PROFIBUS_FSPMS_PACKET_SET_SLAVE_DIAG_REQ_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_SET_SLAVE_DIAG_REQ_T tData;
  } PROFIBUS_FSPMS_PACKET_SET_SLAVE_DIAG_REQ_T;

  /* Confirmation-Packet that the diagnostic has been sent */
  typedef struct PROFIBUS_FSPMS_PACKET_SET_SLAVE_DIAG_CNF_Ttag {
    TLR_PACKET_HEADER_T tHead;
  } PROFIBUS_FSPMS_PACKET_SET_SLAVE_DIAG_CNF_T;


  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_SET_STAT_DIAG_REQ_Ttag {
    TLR_BOOLEAN8 fStatDiag;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_SET_STAT_DIAG_REQ_T;

  #define PROFIBUS_FSPMS_SET_STAT_DIAG_REQ_SIZE (sizeof(PROFIBUS_FSPMS_SET_STAT_DIAG_REQ_T))

  /* Request-Packet to set a slave diagnostic */
  typedef struct PROFIBUS_FSPMS_PACKET_SET_STAT_DIAG_REQ_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_SET_STAT_DIAG_REQ_T tData;
  } PROFIBUS_FSPMS_PACKET_SET_STAT_DIAG_REQ_T;

  /* Confirmation-Packet that the diagnostic has been sent */
  typedef struct PROFIBUS_FSPMS_PACKET_SET_STAT_DIAG_CNF_Ttag {
    TLR_PACKET_HEADER_T tHead;
  } PROFIBUS_FSPMS_PACKET_SET_STAT_DIAG_CNF_T;


/* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_SET_CFG_REQ_Ttag {
    TLR_UINT8 abCfgData[PROFIBUS_FSPMS_MAX_CFG_DATA_SIZE]; /* Configuration data that'll be set */
  }__TLR_PACKED_POST PROFIBUS_FSPMS_SET_CFG_REQ_T;

  /* Request-Packet to set the configuration data */
  typedef struct PROFIBUS_FSPMS_PACKET_SET_CFG_REQ_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_SET_CFG_REQ_T tData;
  } PROFIBUS_FSPMS_PACKET_SET_CFG_REQ_T;

  /* Confirmation-Packet that the diagnostic has been sent */
  typedef struct PROFIBUS_FSPMS_PACKET_SET_CFG_CNF_Ttag {
    TLR_PACKET_HEADER_T tHead;
  } PROFIBUS_FSPMS_PACKET_SET_CFG_CNF_T;

  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_SET_INPUT_REQ_Ttag {
    TLR_UINT8 abInputData[PROFIBUS_FSPMS_MAX_INPUT_DATA_SIZE]; /* Input data that'll be updated */
  }__TLR_PACKED_POST PROFIBUS_FSPMS_SET_INPUT_REQ_T;

  /* Request-Packet to set the input data */
  typedef struct PROFIBUS_FSPMS_PACKET_SET_INPUT_REQ_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_SET_INPUT_REQ_T tData;
  } PROFIBUS_FSPMS_PACKET_SET_INPUT_REQ_T;

  /* Confirmation-Packet confirming setting the inputs */
  typedef struct PROFIBUS_FSPMS_PACKET_SET_INPUT_CNF_Ttag {
    TLR_PACKET_HEADER_T tHead;
  } PROFIBUS_FSPMS_PACKET_SET_INPUT_CNF_T;


/* ******************************************************************************************* */
  /* Request-Packet to set the input data */
  typedef struct PROFIBUS_FSPMS_PACKET_GET_OUTPUT_REQ_Ttag {
    TLR_PACKET_HEADER_T tHead;
  } PROFIBUS_FSPMS_PACKET_GET_OUTPUT_REQ_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_GET_OUTPUT_CNF_Ttag {
    TLR_BOOLEAN8 fClearFlag;
    TLR_BOOLEAN8 fNewFlag;
    TLR_UINT8 abOutputData[PROFIBUS_FSPMS_MAX_OUTPUT_DATA_SIZE]; /* Last output data that has been updated */
  }__TLR_PACKED_POST PROFIBUS_FSPMS_GET_OUTPUT_CNF_T;

  #define PROFIBUS_FSPMS_GET_OUTPUT_CNF_SIZE (sizeof(PROFIBUS_FSPMS_GET_OUTPUT_CNF_T)-PROFIBUS_FSPMS_MAX_OUTPUT_DATA_SIZE)

  /* Confirmation-Packet confirming setting the inputs */
  typedef struct PROFIBUS_FSPMS_PACKET_GET_OUTPUT_CNF_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_GET_OUTPUT_CNF_T tData;
  } PROFIBUS_FSPMS_PACKET_GET_OUTPUT_CNF_T;


  /* ******************************************************************************************* */
  /* Request-Packet to send an Abort */
  typedef struct PROFIBUS_FSPMS_PACKET_ABORT_REQ_Ttag {
    TLR_PACKET_HEADER_T tHead;
  } PROFIBUS_FSPMS_PACKET_ABORT_REQ_T;

  /* Confirmation-Packet that Abort has been performed */
  typedef struct PROFIBUS_FSPMS_PACKET_ABORT_CNF_Ttag {
    TLR_PACKET_HEADER_T tHead;
  } PROFIBUS_FSPMS_PACKET_ABORT_CNF_T;


  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C1_ALARM_NOTIFICATION_REQ_Ttag {
    TLR_UINT8 bSlotNumber;
    TLR_UINT8 bAlarmType;
    TLR_UINT8 bSeqNr;
    TLR_BOOLEAN8 fAddAck;
    TLR_UINT8 bAlarmSpecifier;
    TLR_UINT8 abAlarmData[PROFIBUS_FSPMS_MAX_ALARM_USERDATA_SIZE];
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C1_ALARM_NOTIFICATION_REQ_T;

  #define PROFIBUS_FSPMS_C1_ALARM_NOTIFICATION_REQ_SIZE (sizeof(PROFIBUS_FSPMS_C1_ALARM_NOTIFICATION_REQ_T)-PROFIBUS_FSPMS_MAX_ALARM_USERDATA_SIZE)

  /* Request-Packet of an Alarm Notification */
  typedef struct PROFIBUS_FSPMS_PACKET_C1_ALARM_NOTIFICATION_REQ_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C1_ALARM_NOTIFICATION_REQ_T tData;
  } PROFIBUS_FSPMS_PACKET_C1_ALARM_NOTIFICATION_REQ_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C1_ALARM_NOTIFICATION_CNF_Ttag {
    TLR_UINT8 bSlotNumber;
    TLR_UINT8 bAlarmType;
    TLR_UINT8 bSeqNr;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C1_ALARM_NOTIFICATION_CNF_T;

  /* Confirmation-Packet of an Alarm Notification */
  typedef struct PROFIBUS_FSPMS_PACKET_C1_ALARM_NOTIFICATION_CNF_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C1_ALARM_NOTIFICATION_CNF_T tData;
  } PROFIBUS_FSPMS_PACKET_C1_ALARM_NOTIFICATION_CNF_T;


  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C1_ALARM_ACK_RES_POS_Ttag {
    TLR_UINT8 bSlotNumber;
    TLR_UINT8 bAlarmType;
    TLR_UINT8 bSeqNr;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C1_ALARM_ACK_RES_POS_T;

  /* Positive response packet of an acyclic Alarm Acknowledgement command */
  typedef struct PROFIBUS_FSPMS_PACKET_C1_ALARM_ACK_RES_POS_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C1_ALARM_ACK_RES_POS_T tData;
  } PROFIBUS_FSPMS_PACKET_C1_ALARM_ACK_RES_POS_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C1_ALARM_ACK_RES_NEG_Ttag {
    TLR_UINT8 bErrorDecode;
    TLR_UINT8 bErrorCode1;
    TLR_UINT8 bErrorCode2;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C1_ALARM_ACK_RES_NEG_T;

  /* Negative response packet of an acyclic Alarm Acknowledgement command */
  typedef struct PROFIBUS_FSPMS_PACKET_C1_ALARM_ACK_RES_NEG_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C1_ALARM_ACK_RES_NEG_T tData;
  } PROFIBUS_FSPMS_PACKET_C1_ALARM_ACK_RES_NEG_T;

  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C1_READ_RES_POS_Ttag {
    TLR_UINT8 bSlotNumber;
    TLR_UINT8 bIndex;
    TLR_UINT8 bLength;
    TLR_UINT8 abData[PROFIBUS_FSPMS_MAX_READ_DATA_SIZE];
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C1_READ_RES_POS_T;

  #define PROFIBUS_FSPMS_C1_READ_RES_POS_SIZE (sizeof(PROFIBUS_FSPMS_C1_READ_RES_POS_T)-PROFIBUS_FSPMS_MAX_READ_DATA_SIZE)

  /* Positive response packet of an acyclic read command */
  typedef struct PROFIBUS_FSPMS_PACKET_C1_READ_RES_POS_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C1_READ_RES_POS_T tData;
  } PROFIBUS_FSPMS_PACKET_C1_READ_RES_POS_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C1_READ_RES_NEG_Ttag {
    TLR_UINT8 bErrorDecode;
    TLR_UINT8 bErrorCode1;
    TLR_UINT8 bErrorCode2;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C1_READ_RES_NEG_T;

   /* Negative response packet of an acyclic read command */
  typedef struct PROFIBUS_FSPMS_PACKET_C1_READ_RES_NEG_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C1_READ_RES_NEG_T tData;
  } PROFIBUS_FSPMS_PACKET_C1_READ_RES_NEG_T;

  /* Return packet to a previously sent read response packet */
  typedef struct PROFIBUS_FSPMS_PACKET_C1_READ_RES_RET_Ttag {
    TLR_PACKET_HEADER_T tHead;
  } PROFIBUS_FSPMS_PACKET_C1_READ_RES_RET_T;


  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C1_WRITE_RES_POS_Ttag {
    TLR_UINT8 bSlotNumber;
    TLR_UINT8 bIndex;
    TLR_UINT8 bLength;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C1_WRITE_RES_POS_T;

  /* Positive response packet of an acyclic write command */
  typedef struct PROFIBUS_FSPMS_PACKET_C1_WRITE_RES_POS_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C1_WRITE_RES_POS_T tData;
  } PROFIBUS_FSPMS_PACKET_C1_WRITE_RES_POS_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C1_WRITE_RES_NEG_Ttag {
    TLR_UINT8 bErrorDecode;
    TLR_UINT8 bErrorCode1;
    TLR_UINT8 bErrorCode2;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C1_WRITE_RES_NEG_T;

  /* Negative response packet of an acyclic read command */
  typedef struct PROFIBUS_FSPMS_PACKET_C1_WRITE_RES_NEG_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C1_READ_RES_NEG_T tData;
  } PROFIBUS_FSPMS_PACKET_C1_WRITE_RES_NEG_T;

  /* Return packet to a previously sent write response packet */
  typedef struct PROFIBUS_FSPMS_PACKET_C1_WRITE_RES_RET_Ttag {
    TLR_PACKET_HEADER_T tHead;
  } PROFIBUS_FSPMS_PACKET_C1_WRITE_RES_RET_T;


  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_REGISTER_DIAG_STRUCT_REQ_Ttag {
		TLR_UINT8 *	pbDiagArea;
		TLR_UINT32	ulLength;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_REGISTER_DIAG_STRUCT_REQ_T;

  #define PROFIBUS_FSPMS_REGISTER_DIAG_STRUCT_REQ_SIZE sizeof(PROFIBUS_FSPMS_REGISTER_DIAG_STRUCT_REQ_T)

  typedef struct PROFIBUS_FSPMS_PACKET_REGISTER_DIAG_STRUCT_REQ_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_REGISTER_DIAG_STRUCT_REQ_T tData;
  } PROFIBUS_FSPMS_PACKET_REGISTER_DIAG_STRUCT_REQ_T;

#if 0
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_REGISTER_DIAG_STRUCT_CNF_Ttag {
  }__TLR_PACKED_POST PROFIBUS_FSPMS_REGISTER_DIAG_STRUCT_CNF_T;
#endif
	#define PROFIBUS_FSPMS_REGISTER_DIAG_STRUCT_CNF_SIZE  0 /* sizeof(PROFIBUS_FSPMS_REGISTER_DIAG_STRUCT_CNF_T) */

  typedef struct PROFIBUS_FSPMS_PACKET_REGISTER_DIAG_STRUCT_CNF_Ttag {
    TLR_PACKET_HEADER_T tHead;
  /*  PROFIBUS_FSPMS_REGISTER_DIAG_STRUCT_CNF_T tData; */
  } PROFIBUS_FSPMS_PACKET_REGISTER_DIAG_STRUCT_CNF_T;



/*
 * indication packets
 * structures of all indication commands the task is able to sent
 */
  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_CHECK_CFG_IND_Ttag {
    TLR_UINT8 abCfgData[PROFIBUS_FSPMS_MAX_CFG_DATA_SIZE]; /* Configuration data that needs to be checked */
  }__TLR_PACKED_POST PROFIBUS_FSPMS_CHECK_CFG_IND_T;

   typedef struct PROFIBUS_FSPMS_PACKET_CHECK_CFG_IND_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_CHECK_CFG_IND_T tData;
  } PROFIBUS_FSPMS_PACKET_CHECK_CFG_IND_T;

#ifdef DPS_VERSION_V2_0_0_X
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_CHECK_CFG_RESULT_REQ_Ttag {
    TLR_BOOLEAN8 fCfgOk;
    TLR_UINT8 bInputDataLen;
    TLR_UINT8 bOutputDataLen;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_CHECK_CFG_RESULT_REQ_T;

  /* Request-Packet confirming the check of the configuration data */
  typedef struct PROFIBUS_FSPMS_PACKET_CHECK_CFG_RESULT_REQ_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_CHECK_CFG_RESULT_REQ_T tData;
  } PROFIBUS_FSPMS_PACKET_CHECK_CFG_RESULT_REQ_T;

  /* Confirmation-Packet confirming the valid reception of the request packet */
  typedef struct PROFIBUS_FSPMS_PACKET_CHECK_CFG_RESULT_CNF_Ttag {
    TLR_PACKET_HEADER_T tHead;
  } PROFIBUS_FSPMS_PACKET_CHECK_CFG_RESULT_CNF_T;
#endif
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_CHECK_CFG_RES_Ttag {
    TLR_BOOLEAN8 fCfgOk;
    TLR_UINT8 bInputDataLen;
    TLR_UINT8 bOutputDataLen;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_CHECK_CFG_RES_T;

   typedef struct PROFIBUS_FSPMS_PACKET_CHECK_CFG_RES_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_CHECK_CFG_RES_T tData;
  } PROFIBUS_FSPMS_PACKET_CHECK_CFG_RES_T;


  /* ******************************************************************************************* */
  #define PROFIBUS_FSPMS_GLOBAL_CONTROL_NOACTION 0
  #define PROFIBUS_FSPMS_GLOBAL_CONTROL_SYNC     1
  #define PROFIBUS_FSPMS_GLOBAL_CONTROL_UNSYNC   2
  #define PROFIBUS_FSPMS_GLOBAL_CONTROL_FREEZE   1
  #define PROFIBUS_FSPMS_GLOBAL_CONTROL_UNFREEZE 2

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_GLOBAL_CONTROL_IND_Ttag {
    TLR_BOOLEAN8 fClearCommand;
    TLR_UINT8 bSyncCommand;
    TLR_UINT8 bFreezeCommand;
    TLR_UINT8 bGroupSelect;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_GLOBAL_CONTROL_IND_T;

  typedef struct PROFIBUS_FSPMS_PACKET_GLOBAL_CONTROL_IND_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_GLOBAL_CONTROL_IND_T tData;
  } PROFIBUS_FSPMS_PACKET_GLOBAL_CONTROL_IND_T;


  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_NEW_OUTPUT_IND_Ttag {
    TLR_BOOLEAN8 fClearFlag;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_NEW_OUTPUT_IND_T;

  typedef struct PROFIBUS_FSPMS_PACKET_NEW_OUTPUT_IND_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_NEW_OUTPUT_IND_T tData;
  } PROFIBUS_FSPMS_PACKET_NEW_OUTPUT_IND_T;

  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_CHECK_USER_PRM_IND_Ttag {
    TLR_UINT8 abUserPrmData[PROFIBUS_FSPM_MAX_USER_PRM_DATA_SIZE]; /* Parameter data that needs to be checked */
  }__TLR_PACKED_POST PROFIBUS_FSPMS_CHECK_USER_PRM_IND_T;

  typedef struct PROFIBUS_FSPMS_PACKET_CHECK_USER_PRM_IND_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_CHECK_USER_PRM_IND_T tData;
  } PROFIBUS_FSPMS_PACKET_CHECK_USER_PRM_IND_T;

#ifdef DPS_VERSION_V2_0_0_X
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_CHECK_USER_PRM_RESULT_REQ_Ttag {
    TLR_BOOLEAN8 fPrmOk;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_CHECK_USER_PRM_RESULT_REQ_T;

  /* Request-Packet confirming the check of the user parameter data */
  typedef struct PROFIBUS_FSPMS_PACKET_CHECK_USER_PRM_RESULT_REQ_Ttag {
    TLR_PACKET_HEADER_T    tHead;
    PROFIBUS_FSPMS_CHECK_USER_PRM_RESULT_REQ_T tData;
  } PROFIBUS_FSPMS_PACKET_CHECK_USER_PRM_RESULT_REQ_T;

  /* Confirmation-Packet confirming the valid reception of the request packet */
  typedef struct PROFIBUS_FSPMS_PACKET_CHECK_USER_PRM_RESULT_CNF_Ttag {
    TLR_PACKET_HEADER_T    tHead;
  } PROFIBUS_FSPMS_PACKET_CHECK_USER_PRM_RESULT_CNF_T;
#endif
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_CHECK_USER_PRM_RES_Ttag {
    TLR_BOOLEAN8 fPrmOk;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_CHECK_USER_PRM_RES_T;

  typedef struct PROFIBUS_FSPMS_PACKET_CHECK_USER_PRM_RES_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_CHECK_USER_PRM_RES_T tData;
  } PROFIBUS_FSPMS_PACKET_CHECK_USER_PRM_RES_T;


  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_CHECK_EXT_USER_PRM_IND_Ttag {
    TLR_UINT8 abExtUserPrmData[PROFIBUS_FSPMS_MAX_EXT_USER_PRM_DATA_SIZE]; /* extended parameter data that needs to be checked */
  }__TLR_PACKED_POST PROFIBUS_FSPMS_CHECK_EXT_USER_PRM_IND_T;

  typedef struct PROFIBUS_FSPMS_PACKET_CHECK_EXT_USER_PRM_IND_Ttag {
    TLR_PACKET_HEADER_T  tHead;
    PROFIBUS_FSPMS_CHECK_EXT_USER_PRM_IND_T tData;
  } PROFIBUS_FSPMS_PACKET_CHECK_EXT_USER_PRM_IND_T;

#ifdef DPS_VERSION_V2_0_0_X
  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_CHECK_EXT_USER_PRM_RESULT_REQ_Ttag {
    TLR_BOOLEAN8 fExtPrmOk;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_CHECK_EXT_USER_PRM_RESULT_REQ_T;

  /* Request-Packet confirming the check of the extended user parameter data */
  typedef struct PROFIBUS_FSPMS_PACKET_CHECK_EXT_USER_PRM_RESULT_REQ_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_CHECK_EXT_USER_PRM_RESULT_REQ_T tData;
  } PROFIBUS_FSPMS_PACKET_CHECK_EXT_USER_PRM_RESULT_REQ_T;

  /* Confirmation-Packet confirming the valid reception of the request packet */
  typedef struct PROFIBUS_FSPMS_PACKET_CHECK_EXT_USER_PRM_RESULT_CNF_Ttag {
    TLR_PACKET_HEADER_T tHead;
  } PROFIBUS_FSPMS_PACKET_CHECK_EXT_USER_PRM_RESULT_CNF_T;
#endif
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_CHECK_EXT_USER_PRM_RES_Ttag {
    TLR_BOOLEAN8 fExtPrmOk;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_CHECK_EXT_USER_PRM_RES_T;

  typedef struct PROFIBUS_FSPMS_PACKET_CHECK_EXT_USER_PRM_RES_Ttag {
    TLR_PACKET_HEADER_T  tHead;
    PROFIBUS_FSPMS_CHECK_EXT_USER_PRM_RES_T tData;
  } PROFIBUS_FSPMS_PACKET_CHECK_EXT_USER_PRM_RES_T;

  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_SET_SLAVE_ADD_IND_Ttag {
    TLR_UINT8 bNewSlaveAdd;
    TLR_UINT8 bNoAddChange;
    TLR_UINT8 abRemSlaveData[PROFIBUS_FSPMS_MAX_SET_SLAVE_ADD_DATA_SIZE];
  }__TLR_PACKED_POST PROFIBUS_FSPMS_SET_SLAVE_ADD_IND_T;

  #define PROFIBUS_FSPMS_SET_SLAVE_ADD_IND_SIZE (sizeof(PROFIBUS_FSPMS_SET_SLAVE_ADD_IND_T)-PROFIBUS_FSPMS_MAX_SET_SLAVE_ADD_DATA_SIZE)

  typedef struct PROFIBUS_FSPMS_PACKET_SET_SLAVE_ADD_IND_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_SET_SLAVE_ADD_IND_T tData;
  } PROFIBUS_FSPMS_PACKET_SET_SLAVE_ADD_IND_T;


  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C1_READ_IND_Ttag {
    TLR_UINT8 bSlotNumber;
    TLR_UINT8 bIndex;
    TLR_UINT8 bLength;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C1_READ_IND_T;

 #define PROFIBUS_FSPMS_C1_READ_IND_SIZE (sizeof(PROFIBUS_FSPMS_C1_READ_IND_T))

  typedef struct PROFIBUS_FSPMS_PACKET_C1_READ_IND_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C1_READ_IND_T tData;
  } PROFIBUS_FSPMS_PACKET_C1_READ_IND_T;

  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C1_WRITE_IND_Ttag {
    TLR_UINT8 bSlotNumber;
    TLR_UINT8 bIndex;
    TLR_UINT8 bLength;
    TLR_UINT8 abData[PROFIBUS_FSPMS_MAX_WRITE_DATA_SIZE];
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C1_WRITE_IND_T;

   #define PROFIBUS_FSPMS_C1_WRITE_IND_SIZE (sizeof(PROFIBUS_FSPMS_C1_WRITE_IND_T)-PROFIBUS_FSPMS_MAX_WRITE_DATA_SIZE)

  typedef struct PROFIBUS_FSPMS_PACKET_C1_WRITE_IND_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C1_WRITE_IND_T tData;
  } PROFIBUS_FSPMS_PACKET_C1_WRITE_IND_T;

  /* ******************************************************************************************* */

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_STARTED_IND_Ttag {
    TLR_UINT8 bActualEnabledAlarms;
    TLR_BOOLEAN8 fAlarmSequence;
    TLR_UINT8 bAlarmLimit;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_STARTED_IND_T;

  typedef struct PROFIBUS_FSPMS_PACKET_STARTED_IND_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_STARTED_IND_T tData;
  } PROFIBUS_FSPMS_PACKET_STARTED_IND_T;


  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C1_ALARM_ACK_IND_Ttag {
    TLR_UINT8 bSlotNumber;
    TLR_UINT8 bAlarmType;
    TLR_UINT8 bSeqNr;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C1_ALARM_ACK_IND_T;

  typedef struct PROFIBUS_FSPMS_PACKET_C1_ALARM_ACK_IND_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C1_ALARM_ACK_IND_T tData;
  } PROFIBUS_FSPMS_PACKET_C1_ALARM_ACK_IND_T;

  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_STATE_CHANGED_IND_Ttag {
		TLR_UINT32 	ulState;
		TLR_UINT32	ulError;
		TLR_UINT32	ulReason;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_STATE_CHANGED_IND_T;

  #define PROFIBUS_FSPMS_STATE_CHANGED_IND_DATA_SIZE sizeof(PROFIBUS_FSPMS_STATE_CHANGED_IND_T)

  typedef struct PROFIBUS_FSPMS_PACKET_STATE_CHANGED_IND_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_STATE_CHANGED_IND_T tData;
  } PROFIBUS_FSPMS_PACKET_STATE_CHANGED_IND_T;


/****************************************************************************************
 * DPV1 Class 2 packets
 *
 */


  /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_INITIATE_ADDR_Ttag {
    TLR_UINT8 bAPI;
    TLR_UINT8 bSCL;
    TLR_UINT8 abNetworkAddress[6];
    TLR_UINT8 abMACAddress[];
  }__TLR_PACKED_POST PROFIBUS_FSPMS_INITIATE_ADDR_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C2_INITIATE_IND_Ttag {
    TLR_UINT32  ulReference;
    __TLR_PACKED_PRE struct  {
      TLR_UINT16 biDPV1_RW  : 1;
      TLR_UINT16 biReserved : 15;
    }__TLR_PACKED_POST tFeaturesSupported;
    __TLR_PACKED_PRE struct  {
      TLR_UINT16 biReserved : 16;
    }__TLR_PACKED_POST tProfileFeaturesSupported;
    TLR_UINT16 usProfileIdentNumber;
    TLR_BOOLEAN8 fSType;
    TLR_UINT8 bSLen;
    TLR_BOOLEAN8 fDType;
    TLR_UINT8 bDLen;
    TLR_UINT8 abSAddrDAddr[PROFIBUS_FSPMS_MAX_INITIATE_ADD_TABLE_SIZE];
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C2_INITIATE_IND_T;

  #define PROFIBUS_FSPMS_C2_INITIATE_IND_DATA_SIZE (sizeof(PROFIBUS_FSPMS_C2_INITIATE_IND_T) - PROFIBUS_FSPMS_MAX_INITIATE_ADD_TABLE_SIZE)

  typedef struct PROFIBUS_FSPMS_PACKET_C2_INITIATE_IND_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C2_INITIATE_IND_T tData;
  } PROFIBUS_FSPMS_PACKET_C2_INITIATE_IND_T;


  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C2_INITIATE_RES_POS_Ttag {
    TLR_UINT32 ulReference;
    TLR_UINT16 usMaxLenDataUnit;
    __TLR_PACKED_PRE struct {
      TLR_UINT16 biDPV1_RW  : 1;
      TLR_UINT16 biReserved : 15;
    }__TLR_PACKED_POST tFeaturesSupported;
    __TLR_PACKED_PRE struct {
      TLR_UINT16 biReserved : 16;
    }__TLR_PACKED_POST tProfileFeaturesSupported;
    TLR_UINT16 usProfileIdentNumber;
    TLR_BOOLEAN8 fSType;
    TLR_UINT8 bSLen;
    TLR_BOOLEAN8 fDType;
    TLR_UINT8 bDLen;
    TLR_UINT8 abSAddrDAddr[PROFIBUS_FSPMS_MAX_INITIATE_ADD_TABLE_SIZE];
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C2_INITIATE_RES_POS_T;

   #define PROFIBUS_FSPMS_C2_INITIATE_RES_DATA_SIZE (sizeof(PROFIBUS_FSPMS_C2_INITIATE_RES_POS_T) - PROFIBUS_FSPMS_MAX_INITIATE_ADD_TABLE_SIZE)

   /* Positive response packet of an acyclic DP Master Class 2 initialization */
  typedef struct PROFIBUS_FSPMS_PACKET_C2_INITIATE_RES_POS_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C2_INITIATE_RES_POS_T tData;
  } PROFIBUS_FSPMS_PACKET_C2_INITIATE_RES_POS_T;


  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C2_INITIATE_RES_NEG_Ttag {
    TLR_UINT32  ulReference;
    TLR_UINT8   bErrorDecode;
    TLR_UINT8   bErrorCode1;
    TLR_UINT8   bErrorCode2;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C2_INITIATE_RES_NEG_T;

   /* Negative response packet of an acyclic DP Master Class 2 initialization */
  typedef struct PROFIBUS_FSPMS_PACKET_C2_INITIATE_RES_NEG_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C2_INITIATE_RES_NEG_T tData;
  } PROFIBUS_FSPMS_PACKET_C2_INITIATE_RES_NEG_T;

  /* ******************************************************************************************* */

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C2_READ_IND_Ttag {
    TLR_UINT32  ulReference;
    TLR_UINT8   bSlotNumber;
    TLR_UINT8   bIndex;
    TLR_UINT8   bLength;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C2_READ_IND_T;

 #define PROFIBUS_FSPMS_C2_READ_IND_SIZE (sizeof(PROFIBUS_FSPMS_C2_READ_IND_T))

  typedef struct PROFIBUS_FSPMS_PACKET_C2_READ_IND_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C2_READ_IND_T tData;
  } PROFIBUS_FSPMS_PACKET_C2_READ_IND_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C2_READ_RES_POS_Ttag {
    TLR_UINT32  ulReference;
    TLR_UINT8   bSlotNumber;
    TLR_UINT8   bIndex;
    TLR_UINT8   bLength;
    TLR_UINT8   abData[PROFIBUS_FSPMS_MAX_READ_DATA_SIZE];
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C2_READ_RES_POS_T;

  #define PROFIBUS_FSPMS_C2_READ_RES_POS_SIZE (sizeof(PROFIBUS_FSPMS_C2_READ_RES_POS_T)-PROFIBUS_FSPMS_MAX_READ_DATA_SIZE)

  /* Positive response packet of an acyclic read command */
  typedef struct PROFIBUS_FSPMS_PACKET_C2_READ_RES_POS_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C2_READ_RES_POS_T tData;
  } PROFIBUS_FSPMS_PACKET_C2_READ_RES_POS_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C2_READ_RES_NEG_Ttag {
    TLR_UINT32  ulReference;
    TLR_UINT8   bErrorDecode;
    TLR_UINT8   bErrorCode1;
    TLR_UINT8   bErrorCode2;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C2_READ_RES_NEG_T;

   /* Negative response packet of an acyclic read command */
  typedef struct PROFIBUS_FSPMS_PACKET_C2_READ_RES_NEG_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C2_READ_RES_NEG_T tData;
  } PROFIBUS_FSPMS_PACKET_C2_READ_RES_NEG_T;

  /* ******************************************************************************************* */

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C2_WRITE_IND_Ttag {
    TLR_UINT32  ulReference;
    TLR_UINT8   bSlotNumber;
    TLR_UINT8   bIndex;
    TLR_UINT8   bLength;
    TLR_UINT8   abData[PROFIBUS_FSPMS_MAX_WRITE_DATA_SIZE];
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C2_WRITE_IND_T;

 #define PROFIBUS_FSPMS_C2_WRITE_IND_SIZE (sizeof(PROFIBUS_FSPMS_C2_WRITE_IND_T)-PROFIBUS_FSPMS_MAX_WRITE_DATA_SIZE)

  typedef struct PROFIBUS_FSPMS_PACKET_C2_WRITE_IND_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C2_WRITE_IND_T tData;
  } PROFIBUS_FSPMS_PACKET_C2_WRITE_IND_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C2_WRITE_RES_POS_Ttag {
    TLR_UINT32  ulReference;
    TLR_UINT8   bSlotNumber;
    TLR_UINT8   bIndex;
    TLR_UINT8   bLength;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C2_WRITE_RES_POS_T;

  #define PROFIBUS_FSPMS_C2_WRITE_RES_POS_SIZE (sizeof(PROFIBUS_FSPMS_C2_WRITE_RES_POS_T))

  /* Positive response packet of an acyclic read command */
  typedef struct PROFIBUS_FSPMS_PACKET_C2_WRITE_RES_POS_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C2_WRITE_RES_POS_T tData;
  } PROFIBUS_FSPMS_PACKET_C2_WRITE_RES_POS_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C2_WRITE_RES_NEG_Ttag {
    TLR_UINT32  ulReference;
    TLR_UINT8   bErrorDecode;
    TLR_UINT8   bErrorCode1;
    TLR_UINT8   bErrorCode2;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C2_WRITE_RES_NEG_T;

   /* Negative response packet of an acyclic read command */
  typedef struct PROFIBUS_FSPMS_PACKET_C2_WRITE_RES_NEG_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C2_WRITE_RES_NEG_T tData;
  } PROFIBUS_FSPMS_PACKET_C2_WRITE_RES_NEG_T;


 /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C2_DATA_TRANSPORT_IND_Ttag {
    TLR_UINT32  ulReference;
    TLR_UINT8   bSlotNumber;
    TLR_UINT8   bIndex;
    TLR_UINT8   bLength;
    TLR_UINT8   abData[PROFIBUS_FSPMS_MAX_DATA_TRANSPORT_DATA_SIZE];
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C2_DATA_TRANSPORT_IND_T;

   #define PROFIBUS_FSPMS_C2_DATA_TRANSPORT_IND_SIZE (sizeof(PROFIBUS_FSPMS_C2_DATA_TRANSPORT_IND_T)-PROFIBUS_FSPMS_MAX_DATA_TRANSPORT_DATA_SIZE)

  typedef struct PROFIBUS_FSPMS_PACKET_C2_DATA_TRANSPORT_IND_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C2_DATA_TRANSPORT_IND_T tData;
  } PROFIBUS_FSPMS_PACKET_C2_DATA_TRANSPORT_IND_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C2_DATA_TRANSPORT_RES_POS_Ttag {
    TLR_UINT32  ulReference;
    TLR_UINT8   bSlotNumber;
    TLR_UINT8   bIndex;
    TLR_UINT8   bLength;
    TLR_UINT8 abData[PROFIBUS_FSPMS_MAX_DATA_TRANSPORT_DATA_SIZE];
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C2_DATA_TRANSPORT_RES_POS_T;

  #define PROFIBUS_FSPMS_C2_DATA_TRANSPORT_RES_POS_SIZE (sizeof(PROFIBUS_FSPMS_C2_DATA_TRANSPORT_RES_POS_T)-PROFIBUS_FSPMS_MAX_DATA_TRANSPORT_DATA_SIZE)

  /* Positive response packet of an acyclic read command */
  typedef struct PROFIBUS_FSPMS_PACKET_C2_DATA_TRANSPORT_RES_POS_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C2_DATA_TRANSPORT_RES_POS_T tData;
  } PROFIBUS_FSPMS_PACKET_C2_DATA_TRANSPORT_RES_POS_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C2_DATA_TRANSPORT_RES_NEG_Ttag {
    TLR_UINT32  ulReference;
    TLR_UINT8   bErrorDecode;
    TLR_UINT8   bErrorCode1;
    TLR_UINT8   bErrorCode2;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C2_DATA_TRANSPORT_RES_NEG_T;

   /* Negative response packet of an acyclic read command */
  typedef struct PROFIBUS_FSPMS_PACKET_C2_DATA_TRANSPORT_RES_NEG_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C2_DATA_TRANSPORT_RES_NEG_T tData;
  } PROFIBUS_FSPMS_PACKET_C2_DATA_TRANSPORT_RES_NEG_T;


 /* ******************************************************************************************* */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C2_ABORT_IND_Ttag {
    TLR_UINT32  ulReference;
    TLR_UINT32  ulLocal;
    TLR_UINT32  ulSubnet;
    TLR_UINT32  ulInstance;
    TLR_UINT32  ulReasonCode;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C2_ABORT_IND_T;

   #define PROFIBUS_FSPMS_C2_ABORT_IND_SIZE (sizeof(PROFIBUS_FSPMS_C2_ABORT_IND_T))

  typedef struct PROFIBUS_FSPMS_PACKET_C2_ABORT_IND_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C2_ABORT_IND_T tData;
  } PROFIBUS_FSPMS_PACKET_C2_ABORT_IND_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_C2_ABORT_RES_Ttag {
    TLR_UINT32  ulReference;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_C2_ABORT_RES_T;

  #define PROFIBUS_FSPMS_C2_ABORT_RES_SIZE (sizeof(PROFIBUS_FSPMS_C2_ABORT_RES_T))

  /* Positive response packet of an acyclic read command */
  typedef struct PROFIBUS_FSPMS_PACKET_C2_ABORT_RES_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_C2_ABORT_RES_T tData;
  } PROFIBUS_FSPMS_PACKET_C2_ABORT_RES_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_IO_LENGTH_DATA_Ttag{
    TLR_UINT32 ulInputLen;
    TLR_UINT32 ulOutputLen;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_UPDATE_IO_LENGTH_DIAG_DATA_T;

  typedef struct PROFIBUS_FSPMS_PACKET_IO_LENGTH_DATA_REQ_Ttag{
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_UPDATE_IO_LENGTH_DIAG_DATA_T tData;
  }PROFIBUS_FSPMS_PACKET_UPDATE_IO_LENGTH_DIAG_DATA_REQ_T;

  /* I&M packets */

  typedef struct PROFIBUS_FSPMS_PACKET_SET_IM0_REQ_Ttag{
    TLR_PACKET_HEADER_T tHead;
    FSPMS_IM_T tData;
  }PROFIBUS_FSPMS_PACKET_SET_IM0_REQ_T;

  typedef struct PROFIBUS_FSPMS_PACKET_SET_IM0_CNF_Ttag{
    TLR_PACKET_HEADER_T tHead;
  }PROFIBUS_FSPMS_PACKET_SET_IM0_CNF_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_IM_ERROR_Ttag{
    TLR_UINT8  bErrorDecode;
    TLR_UINT8  bErrorCode1;
    TLR_UINT8  bErrorCode2;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_IM_ERROR_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_IM_INDEX_Ttag{
    TLR_UINT32 ulSAPIdx;
    TLR_UINT32 ulSlot;
    TLR_UINT32 ulIM_Index;
    TLR_UINT32 ulLen;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_IM_INDEX_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_IM_READ_RES_Ttag{
    PROFIBUS_FSPMS_IM_INDEX_T tInfo;
    __TLR_PACKED_PRE union{
      TLR_UINT8  abData[244];
      PROFIBUS_FSPMS_IM_ERROR_T tError;
    }__TLR_PACKED_POST un;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_IM_READ_RES_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_IM_WRITE_IND_Ttag{
    PROFIBUS_FSPMS_IM_INDEX_T tInfo;
    TLR_UINT8 abData[238];
  }__TLR_PACKED_POST PROFIBUS_FSPMS_IM_WRITE_IND_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPMS_IM_WRITE_RES_Ttag{
    PROFIBUS_FSPMS_IM_INDEX_T tInfo;
    PROFIBUS_FSPMS_IM_ERROR_T tError;
  }__TLR_PACKED_POST PROFIBUS_FSPMS_IM_WRITE_RES_T;

  typedef struct PROFIBUS_FSPMS_PACKET_IM_WRITE_IND_Ttag{
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_IM_WRITE_IND_T tData;
  }PROFIBUS_FSPMS_PACKET_IM_WRITE_IND_T;

  typedef struct PROFIBUS_FSPMS_PACKET_IM_WRITE_RES_Ttag{
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_IM_WRITE_RES_T tData;
  }PROFIBUS_FSPMS_PACKET_IM_WRITE_RES_T;

  typedef struct PROFIBUS_FSPMS_PACKET_IM_READ_IND_Ttag{
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_IM_INDEX_T tData;
  }PROFIBUS_FSPMS_PACKET_IM_READ_IND_T;

  typedef struct PROFIBUS_FSPMS_PACKET_IM_READ_RES_Ttag{
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_FSPMS_IM_READ_RES_T tData;
  }PROFIBUS_FSPMS_PACKET_IM_READ_RES_T;

/* pragma unpack */
#ifdef PRAGMA_PACK_ENABLE
#pragma PRAGMA_UNPACK_1(PROFIBUS_FSPMS_PUBLIC)
#endif

///////////////Added for slave diagnostic
#define  PROFIBUS_FSPM_ACT_MAX_EXT_DIAG_LEN 20
#define PROFIBUS_FSPMM_CMD_GET_SLAVE_DIAG_REQ 0x0000220A
#define PROFIBUS_FSPMM_GET_SLAVE_DIAG_CNF     0x0000220B

#define PROFIBUS_FSPMM_GET_DIAG_FLAG_PEEK 0x00000001
#define PROFIBUS_FSPMM_GET_DIAG_FLAG_QUERY 0x00000002
typedef struct PROFIBUS_FSPMM_GET_SLAVE_DIAG_REQ_Ttag
{
   TLR_UINT32 ulRemAdd;
   TLR_UINT32 ulFlags;
}PROFIBUS_FSPMM_GET_SLAVE_DIAG_REQ_T;
typedef struct PROFIBUS_FSPMM_PACKET_GET_SLAVE_DIAG_REQ_Ttag
{
   TLR_PACKET_HEADER_T tHead;
   PROFIBUS_FSPMM_GET_SLAVE_DIAG_REQ_T tData;
}PROFIBUS_FSPMM_PACKET_GET_SLAVE_DIAG_REQ_T;
#define PROFIBUS_FSPMM_GET_SLAVE_DIAG_REQ_SIZE \
      (sizeof(PROFIBUS_FSPMM_PACKET_GET_SLAVE_DIAG_REQ_T))

typedef struct PROFIBUS_FSPMM_DIAGNOSTIC_DATA_Ttag {
   struct
   {
      TLR_UINT8 bStation_Non_Existent : 1; /* no response */
      TLR_UINT8 bStation_Not_Ready : 1; /* station not ready */
      TLR_UINT8 bCfg_Fault : 1; /* configuration fault */
      TLR_UINT8 bExt_Diag : 1; /* extended diagnostic */
      TLR_UINT8 bNot_Supported : 1; /* sync, freeze not supported */
      TLR_UINT8 bInvalid_Response : 1; /* response faulty */
      TLR_UINT8 bPrm_Fault : 1; /* parameters faulty */
      TLR_UINT8 bMaster_Lock : 1; /* locked by a master */
   } Stationstatus_1;
   struct
   {
      TLR_UINT8 bPrm_Req : 1; /* request new parameters */
      TLR_UINT8 bStat_Diag : 1; /* static diagnostic */
      TLR_UINT8 bTrue : 1; /* set to 1 by a slave */
      TLR_UINT8 bWd_On : 1; /* watchdog function on/off */
      TLR_UINT8 bFreeze_Mode : 1; /* freeze mode active */
      TLR_UINT8 bSync_Mode : 1; /* sync mode active */
      TLR_UINT8 bReserved : 1; /* reserved */
      TLR_UINT8 bDeactivated : 1; /* slave deactivated */
   } Stationstatus_2;
   struct
   {
      TLR_UINT8 bReserved : 7;
      TLR_UINT8 bExt_Diag_Overflow : 1; /* ext. diagnostic overflow */
   } Stationstatus_3;
   TLR_UINT8 bMaster_Add; /* corresponding master address */
   TLR_UINT16 usIdent_Number; /* ident number, motorola format */
   TLR_UINT8 abExt_Diag_Data[PROFIBUS_FSPM_ACT_MAX_EXT_DIAG_LEN];
   /* extended diagnostic field */
} PROFIBUS_FSPMM_DIAGNOSTIC_DATA_T;

typedef struct PROFIBUS_FSPMM_GET_SLAVE_DIAG_CNF_Ttag
{
   TLR_UINT32 ulRemAdd;
   PROFIBUS_FSPMM_DIAGNOSTIC_DATA_T tDiagnostic;
}PROFIBUS_FSPMM_GET_SLAVE_DIAG_CNF_T;

   #define PROFIBUS_FSPMM_GET_SLAVE_DIAG_CNF_SIZE \
   (sizeof(PROFIBUS_FSPMM_GET_SLAVE_DIAG_CNF_T) - \
   sizeof(PROFIBUS_FSPMM_DIAGNOSTIC_DATA_T) )

typedef struct PROFIBUS_FSPMM_PACKET_GET_SLAVE_DIAG_CNF_Ttag
{
   TLR_PACKET_HEADER_T tHead;
   PROFIBUS_FSPMM_GET_SLAVE_DIAG_CNF_T tData;
}PROFIBUS_FSPMM_PACKET_GET_SLAVE_DIAG_CNF_T;
/****************************************************************************************
 * necessary Callback functions of the Profibus slave stack
 *
 */


//Extended Hilsher provided diagnostic information for Profibus
typedef struct FSPMS_EXTENDED_DIAG_Ttag {
UINT32 ulBusAddresse;
UINT32 ulIdentNumber;
UINT32 ulBaudrate;
UINT16 usOutputLength;
UINT16 usInputLength;
} FSPMS_EXTENDED_DIAG_T;
/***************************************************************************************/
#endif /* #ifndef __PROFIBUS_FSPMS_PUBLIC_H */
