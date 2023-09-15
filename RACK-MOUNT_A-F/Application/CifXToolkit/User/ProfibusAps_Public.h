/*
 * ProfibusAps_Public.h
 *
 *  Created on: Aug 3, 2012
 *      Author: vivek.sharma
 */

#ifndef PROFIBUSAPS_PUBLIC_H_
#define PROFIBUSAPS_PUBLIC_H_

/* Profibus Aps public, constants */
#define PROFIBUS_APS_PROCESS_QUEUE_NAME   "PB_APS_QUE"


/* definitions for the warmstart prm command */

/* Warmstart System Flags */
#define WRMSTRT_FLG_START_MSK            0x00000001
#define WRMSTRT_FLG_START_APPLICATION    0x00000001
#define WRMSTRT_FLG_START_AUTO           0x00000000

#define WRMSTRT_FLG_IOSTAT_ENABLE_MSK    0x00000002
#define WRMSTRT_FLG_IOSTAT_ENABLED_MSK   0x00000002
#define WRMSTRT_FLG_IOSTAT_DISABLED_MSK  0x00000000

#define WRMSTRT_FLG_IOSTAT_LEN_MSK       0x00000004
#define WRMSTRT_FLG_IOSTAT_LEN_8BIT      0x00000004
#define WRMSTRT_FLG_IOSTAT_LEN_32BIT     0x00000000

/* Flags */
#define WRMSTRT_FLG_DPV1_ENABLE     0x01
#define WRMSTRT_FLG_SYNC_SUPP       0x02
#define WRMSTRT_FLG_FREEZE_SUPP     0x04
#define WRMSTRT_FLG_FAILSAFE_SUPP   0x08
#define WRMSTRT_FLG_RESERVED0       0x10
#define WRMSTRT_FLG_IO_SWAP         0x20
#define WRMSTRT_FLG_AUTOCONFIG      0x40
#define WRMSTRT_FLG_NO_ADDR_CHANGE  0x80


#define PROFIBUS_APS_WARMSTART_PRM_REQ        0x00003100
#define PROFIBUS_APS_WARMSTART_PRM_CNF        0x00003101

#define PROFIBUS_APS_SET_CONFIGURATION_REQ    0x00003102
#define PROFIBUS_APS_SET_CONFIGURATION_CNF    0x00003103

#define PROFIBUS_APS_CHECK_USER_PRM_IND       0x00003104
#define PROFIBUS_APS_CHECK_USER_PRM_RES       0x00003105

#define PROFIBUS_APS_CHECK_CFG_IND            0x00003106
#define PROFIBUS_APS_CHECK_CFG_RES            0x00003107

#define PROFIBUS_APS_GET_USER_PRM_REQ         0x00003108
#define PROFIBUS_APS_GET_USER_PRM_CNF         0x00003109

#define PROFIBUS_APS_GET_CFG_REQ              0x0000310A
#define PROFIBUS_APS_GET_CFG_CNF              0x0000310B


/* reserved Area */

/* 0x000031F0 - 0x000031FF */

/***************************************************************************************/
/* Profibus Aps public, types */
/***************************************************************************************/
/* Profibus Aps public, packets */

/* pragma pack */
#ifdef PRAGMA_PACK_ENABLE
#pragma PRAGMA_PACK_1(PROFIBUS_APS_PUBLIC)
#endif


/* request packet data */
/* confirmation packet data */

  typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_APS_WARMSTART_PRM_REQ_Ttag {
	TLR_UINT32  ulSystemFlags; /* reserved for IO status */
	TLR_UINT32  ulWdgTime;
	TLR_UINT16  usIdentNumber;
	TLR_UINT8   bBusAddr;
	TLR_UINT8   bBaudRate;
	TLR_UINT8   bFlags;
	TLR_UINT8   bRes[2];
	TLR_UINT8   bCfgLen;
	TLR_UINT8   abCfgData[244];
  } PROFIBUS_APS_WARMSTART_PRM_REQ_T;

   #define PROFIBUS_APS_WARMSTART_PRM_REQ_SIZE (sizeof(PROFIBUS_APS_WARMSTART_PRM_REQ_T))

 /* Indication Packet for acknowledged connectionless data transfer */
  typedef struct  PROFIBUS_APS_PACKET_WARMSTART_PRM_REQ_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_APS_WARMSTART_PRM_REQ_T tData;
  } PROFIBUS_APS_PACKET_WARMSTART_PRM_REQ_T;


   #define PROFIBUS_APS_WARMSTART_PRM_CNF_SIZE 0

 /* Indication Packet for acknowledged connectionless data transfer */
  typedef struct  PROFIBUS_APS_PACKET_WARMSTART_PRM_CNF_Ttag {
    TLR_PACKET_HEADER_T tHead;
  } PROFIBUS_APS_PACKET_WARMSTART_PRM_CNF_T;



  typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_APS_SET_CONFIGURATION_REQ_Ttag {
	TLR_UINT32  ulSystemFlags; /* reserved for IO status */
	TLR_UINT32  ulWdgTime;
	TLR_UINT16  usIdentNumber;
	TLR_UINT8   bBusAddr;
	TLR_UINT8   bBaudRate;
	TLR_UINT8   bFlags;
	TLR_UINT8   bRes[2];
	TLR_UINT8   bCfgLen;
	TLR_UINT8   abCfgData[244];
  } PROFIBUS_APS_SET_CONFIGURATION_REQ_T;

   #define PROFIBUS_APS_SET_CONFIGURATION_REQ_SIZE (sizeof(PROFIBUS_APS_SET_CONFIGURATION_REQ_T)-244)

 /* Indication Packet for acknowledged connectionless data transfer */
  typedef struct  PROFIBUS_APS_PACKET_SET_CONFIGURATION_REQ_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_APS_SET_CONFIGURATION_REQ_T tData;
  } PROFIBUS_APS_PACKET_SET_CONFIGURATION_REQ_T;


   #define PROFIBUS_APS_SET_CONFIGURATION_CNF_SIZE 0

 /* Indication Packet for acknowledged connectionless data transfer */
  typedef struct  PROFIBUS_APS_PACKET_SET_CONFIGURATION_CNF_Ttag {
    TLR_PACKET_HEADER_T tHead;
  } PROFIBUS_APS_PACKET_SET_CONFIGURATION_CNF_T;


  /* check user parameter indication */
  #define PROFIBUS_APS_MAX_USER_PRM_DATA_SIZE   237
  typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_APS_CHECK_USER_PRM_IND_Ttag {
    TLR_UINT8 abUserPrmData[PROFIBUS_APS_MAX_USER_PRM_DATA_SIZE];
  } PROFIBUS_APS_CHECK_USER_PRM_IND_T;

  typedef struct  PROFIBUS_APS_PACKET_CHECK_USER_PRM_IND_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_APS_CHECK_USER_PRM_IND_T tData;
  } PROFIBUS_APS_PACKET_CHECK_USER_PRM_IND_T;


  typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_APS_CHECK_USER_PRM_RES_Ttag {
    TLR_BOOLEAN8 fPrmOk;
  } PROFIBUS_APS_CHECK_USER_PRM_RES_T;

  #define PROFIBUS_APS_CHECK_USER_PRM_RES_SIZE sizeof(PROFIBUS_APS_CHECK_USER_PRM_RES_T);

  typedef struct  PROFIBUS_APS_PACKET_CHECK_USER_PRM_RES_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_APS_CHECK_USER_PRM_RES_T tData;
  } PROFIBUS_APS_PACKET_CHECK_USER_PRM_RES_T;

  /* check configuration indication */
  #define PROFIBUS_APS_MAX_CFG_DATA_SIZE   244
  typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_APS_CHECK_CFG_IND_Ttag {
  TLR_UINT8 abCfgData[PROFIBUS_APS_MAX_CFG_DATA_SIZE]; /* Configuration data that needs to be checked */
  } PROFIBUS_APS_CHECK_CFG_IND_T;

  typedef struct PROFIBUS_APS_PACKET_CHECK_CFG_IND_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_APS_CHECK_CFG_IND_T tData;
  } PROFIBUS_APS_PACKET_CHECK_CFG_IND_T;


  typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_APS_CHECK_CFG_RES_Ttag {
    TLR_BOOLEAN8 fCfgOk;
  } PROFIBUS_APS_CHECK_CFG_RES_T;

  #define PROFIBUS_APS_CHECK_CFG_RES_SIZE sizeof(PROFIBUS_APS_CHECK_CFG_RES_T);

  typedef struct  PROFIBUS_APS_PACKET_CHECK_CFG_RES_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_APS_CHECK_CFG_RES_T tData;
  } PROFIBUS_APS_PACKET_CHECK_CFG_RES_T;

  /* get user parameter data */
  #define PROFIBUS_APS_GET_USER_PRM_REQ_SIZE 0

  typedef struct  PROFIBUS_APS_PACKET_GET_USER_PRM_REQ_Ttag {
    TLR_PACKET_HEADER_T tHead;
  } PROFIBUS_APS_PACKET_GET_USER_PRM_REQ_T;


  typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_APS_GET_USER_PRM_CNF_Ttag {
    TLR_UINT8 abUserPrmData[PROFIBUS_APS_MAX_USER_PRM_DATA_SIZE];
  } PROFIBUS_APS_GET_USER_PRM_CNF_T;

  typedef struct  PROFIBUS_APS_PACKET_GET_USER_PRM_CNF_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_APS_GET_USER_PRM_CNF_T tData;
  } PROFIBUS_APS_PACKET_GET_USER_PRM_CNF_T;

  /* get configuration data */
  #define PROFIBUS_APS_GET_CFG_REQ_SIZE 0

  typedef struct PROFIBUS_APS_PACKET_GET_CFG_REQ_Ttag {
    TLR_PACKET_HEADER_T tHead;
  } PROFIBUS_APS_PACKET_GET_CFG_REQ_T;


  typedef __PACKED_PRE struct __PACKED_POST PROFIBUS_APS_GET_CFG_CNF_Ttag {
    TLR_UINT8 abCfgData[PROFIBUS_APS_MAX_CFG_DATA_SIZE];
  } PROFIBUS_APS_GET_CFG_CNF_T;

  typedef struct  PROFIBUS_APS_PACKET_GET_CFG_CNF_Ttag {
    TLR_PACKET_HEADER_T tHead;
    PROFIBUS_APS_GET_CFG_CNF_T tData;
  } PROFIBUS_APS_PACKET_GET_CFG_CNF_T;





/* pragma unpack */
#ifdef PRAGMA_PACK_ENABLE
#pragma PRAGMA_UNPACK_1(PROFIBUS_APS_PUBLIC)
#endif



/***************************************************************************************/
/* Profibus Aps public, prototypes */




#endif /* PROFIBUSAPS_PUBLIC_H_ */
