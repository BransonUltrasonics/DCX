/**************************************************************************************

  Copyright (C) 2010 Hilscher Gesellschaft für Systemautomation mbH.

  This program can be used by everyone according the "industrialNETworX Public License INPL".

  The license can be downloaded under <http://www.industrialNETworX.com>.

 **************************************************************************************

   Last Modification:
    @version $Id:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/CifXToolkit/User/ProfibusFspm_Common.h_v   1.0   27 Jul 2015 08:56:46   sesharma  $

   Description:
    Profibus FSPM
    common structures for FSPM

   Changes:

     Date        Author        Description
     ----------------------------------------------------------------------------------
     2010-03-23  RH            Created
**************************************************************************************/
#ifndef __PROFIBUS_FSPM_COMMON_H
#define __PROFIBUS_FSPM_COMMON_H


/****************************************************************************************
 * Profibus Fspms, constants
 *
 */

  #define PROFIBUS_FSPM_MAX_DIAG_DATA_SIZE 238          /* Maximum supported diagnostic data size, exlusive 6 standard bytes */

  #define PROFIBUS_FSPM_MAX_USER_PRM_DATA_SIZE 237      /* Maximum supported user parameter data size */

  #define PROFIBUS_FSPM_MAX_DLPDU_SIZE 246

  #define PROFIBUS_FSPM_APDU_GET_MASTER_DIAG_REQ_PDU 0x41
  #define PROFIBUS_FSPM_APDU_GET_MASTER_DIAG_RES_PDU 0x41
  #define PROFIBUS_FSPM_APDU_GET_MASTER_DIAG_NRS_PDU 0xC1

  #define PROFIBUS_FSPM_APDU_START_SEQ_REQ_PDU 0x42
  #define PROFIBUS_FSPM_APDU_START_SEQ_RES_PDU 0x42
  #define PROFIBUS_FSPM_APDU_START_SEQ_NRS_PDU 0xC2

  #define PROFIBUS_FSPM_APDU_DOWNLOAD_REQ_PDU 0x43
  #define PROFIBUS_FSPM_APDU_DOWNLOAD_RES_PDU 0x43
  #define PROFIBUS_FSPM_APDU_DOWNLOAD_NRS_PDU 0xC3

  #define PROFIBUS_FSPM_APDU_UPLOAD_REQ_PDU 0x44
  #define PROFIBUS_FSPM_APDU_UPLOAD_RES_PDU 0x44
  #define PROFIBUS_FSPM_APDU_UPLOAD_NRS_PDU 0xC4

  #define PROFIBUS_FSPM_APDU_END_SEQ_REQ_PDU 0x45
  #define PROFIBUS_FSPM_APDU_END_SEQ_RES_PDU 0x45
  #define PROFIBUS_FSPM_APDU_END_SEQ_NRS_PDU 0xC5

  #define PROFIBUS_FSPM_APDU_ACT_PARA_BRCT_REQ_PDU 0x46
  #define PROFIBUS_FSPM_APDU_ACT_PARA_BRCT_RES_PDU 0x46
  #define PROFIBUS_FSPM_APDU_ACT_PARA_BRCT_NRS_PDU 0xC6

  #define PROFIBUS_FSPM_APDU_ACT_PARAM_REQ_PDU 0x47
  #define PROFIBUS_FSPM_APDU_ACT_PARAM_RES_PDU 0x47
  #define PROFIBUS_FSPM_APDU_ACT_PARAM_NRS_PDU 0xC7

  #define PROFIBUS_FSPM_APDU_IDLE_REQ_PDU 0x48
  #define PROFIBUS_FSPM_APDU_IDLE_RES_PDU 0x48
  #define PROFIBUS_FSPM_APDU_IDLE_NRS_PDU 0xC8

  #define PROFIBUS_FSPM_APDU_DATA_TRANSPORT_REQ_PDU 0x51
  #define PROFIBUS_FSPM_APDU_DATA_TRANSPORT_RES_PDU 0x51
  #define PROFIBUS_FSPM_APDU_DATA_TRANSPORT_NRS_PDU 0xD1

  #define PROFIBUS_FSPM_APDU_RM_REQ_PDU 0x56
  #define PROFIBUS_FSPM_APDU_RM_RES_PDU 0x56
  #define PROFIBUS_FSPM_APDU_RM_NRS_PDU 0xD6

  #define PROFIBUS_FSPM_APDU_INITIATE_REQ_PDU 0x57
  #define PROFIBUS_FSPM_APDU_INITIATE_RES_PDU 0x57
  #define PROFIBUS_FSPM_APDU_INITIATE_NRS_PDU 0xD7

  #define PROFIBUS_FSPM_APDU_ABORT_REQ_PDU 0x58
  #define PROFIBUS_FSPM_APDU_ABORT_RES_PDU 0x58
  #define PROFIBUS_FSPM_APDU_ABORT_NRS_PDU 0xD8

  #define PROFIBUS_FSPM_APDU_ALARM_ACK_REQ_PDU 0x5C
  #define PROFIBUS_FSPM_APDU_ALARM_ACK_RES_PDU 0x5C
  #define PROFIBUS_FSPM_APDU_ALARM_ACK_NRS_PDU 0xDC

  #define PROFIBUS_FSPM_APDU_READ_REQ_PDU 0x5E
  #define PROFIBUS_FSPM_APDU_READ_RES_PDU 0x5E
  #define PROFIBUS_FSPM_APDU_READ_NRS_PDU 0xDE

  #define PROFIBUS_FSPM_APDU_WRITE_REQ_PDU 0x5F
  #define PROFIBUS_FSPM_APDU_WRITE_RES_PDU 0x5F
  #define PROFIBUS_FSPM_APDU_WRITE_NRS_PDU 0xDF

/****************************************************************************************
* Profibus Fspm, common structures */

/* pragma pack */
#ifdef PRAGMA_PACK_ENABLE
#pragma PRAGMA_PACK_1(PROFIBUS_FSPM_COMMON)
#endif

  /* APDU structure */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPM_APDU_INITIATE_REQ_Ttag {
    TLR_UINT8 bFunction_Num;
    TLR_UINT8 bReserved[3];
    TLR_UINT16 usSend_Timeout;
    TLR_UINT16 usFeatures_Supported;
    TLR_UINT16 usProfile_Features_Supported;
    TLR_UINT16 usProfile_Ident_Number;
    TLR_UINT8  bAdd_Addr_Param[PROFIBUS_FSPM_MAX_DLPDU_SIZE - 10];
  }__TLR_PACKED_POST PROFIBUS_FSPM_APDU_INITIATE_REQ_T;

  /* APDU structure */
  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPM_APDU_INITIATE_RES_Ttag {
    TLR_UINT8 bFunction_Num;
    TLR_UINT8 bMax_Len_Data;
    TLR_UINT16 usFeatures_Supported;
    TLR_UINT16 usProfile_Features_Supported;
    TLR_UINT16 usProfile_Ident_Number;
    TLR_UINT8  bAdd_Addr_Param[PROFIBUS_FSPM_MAX_DLPDU_SIZE - 10];
  }__TLR_PACKED_POST PROFIBUS_FSPM_APDU_INITIATE_RES_T;


  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPM_ADD_ADDR_PARAM_Ttag {
    TLR_UINT8 bS_Type;
    TLR_UINT8 bS_Len;
    TLR_UINT8 bD_Type;
    TLR_UINT8 bD_Len;
    TLR_UINT8 bAddr_Data[1];  /* len is defined by S_len or D_len !! */
  }__TLR_PACKED_POST PROFIBUS_FSPM_ADD_ADDR_PARAM_T;


  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPM_PARAMETER_DPV0_PRM_Ttag {
    TLR_UINT8 abUsr_Prm_Data[PROFIBUS_FSPM_MAX_USER_PRM_DATA_SIZE];  /* user parameter data without DPV1-bytes */
  }__TLR_PACKED_POST PROFIBUS_FSPM_PARAMETER_DPV0_PRM_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPM_PARAMETER_DPV1_USR_PRM_Ttag {

    #define MSK_DPV1_STATUS_1_RESERVED1    0x03
    #define MSK_DPV1_STATUS_1_WD_BASE1MS   0x04
    #define MSK_DPV1_STATUS_1_RESERVED2    0x18
    #define MSK_DPV1_STATUS_1_PUBLISHER_EN 0x20
    #define MSK_DPV1_STATUS_1_FAIL_SAFE    0x40
    #define MSK_DPV1_STATUS_1_DPV1_ENABLED 0x80
    TLR_UINT8 bDPV1_Status_1;

    #define MSK_DPV1_STATUS_2_RUN_ON_CFG_FAULT    0x01
    #define MSK_DPV1_STATUS_2_RESERVED1           0x02
    #define MSK_DPV1_STATUS_2_EN_UPD_ALARM        0x04
    #define MSK_DPV1_STATUS_2_EN_STATUS_ALARM     0x08
    #define MSK_DPV1_STATUS_2_EN_MFG_ALARM        0x10
    #define MSK_DPV1_STATUS_2_EN_DIAG_ALARM       0x20
    #define MSK_DPV1_STATUS_2_EN_PROC_ALARM       0x40
    #define MSK_DPV1_STATUS_2_EN_PULL_PLUGALARM   0x80
    TLR_UINT8 bDPV1_Status_2;

    #define MSK_DPV1_STATUS_3_ALARM_MODE          0x07
    #define MSK_DPV1_STATUS_3_PRM_STRUCTURE       0x08
    #define MSK_DPV1_STATUS_3_ISOM_REQ            0x10
    #define MSK_DPV1_STATUS_3_RESERVED1           0x60
    #define MSK_DPV1_STATUS_3_PRM_CMD             0x80

    TLR_UINT8 bDPV1_Status_3;

  }__TLR_PACKED_POST PROFIBUS_FSPM_PARAMETER_DPV1_USR_PRM_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPM_PARAMETER_DPV1_PRM_Ttag {
    PROFIBUS_FSPM_PARAMETER_DPV1_USR_PRM_T tDpv1;
    TLR_UINT8     abUsr_Prm_Data[PROFIBUS_FSPM_MAX_USER_PRM_DATA_SIZE-sizeof(PROFIBUS_FSPM_PARAMETER_DPV1_USR_PRM_T)];
  }__TLR_PACKED_POST PROFIBUS_FSPM_PARAMETER_DPV1_PRM_T;


  #define PROFIBUS_FSPM_STRUCTURE_TYPE_PRM_CMD             0x02
  #define PROFIBUS_FSPM_STRUCTURE_TYPE_DXB_LINKTABLE       0x03
  #define PROFIBUS_FSPM_STRUCTURE_TYPE_ISOM_PARAMETER      0x04
  #define PROFIBUS_FSPM_STRUCTURE_TYPE_DXB_SUBSCRIBERTABLE 0x07


  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPM_PARAMETER_DPV2_HEADER_Ttag {
    TLR_UINT8     bStructure_Length;
    TLR_UINT8     bStructure_Type;
    TLR_UINT8     bSlot_Number;
    TLR_UINT8     bReserved;
  }__TLR_PACKED_POST PROFIBUS_FSPM_PARAMETER_DPV2_HEADER_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPM_PARAMETER_DPV2_ISOM_PRM_Ttag {
    PROFIBUS_FSPM_PARAMETER_DPV2_HEADER_T tDpv2Header;
    TLR_UINT8  bVersion;
    TLR_UINT32 bTBase_Dp;
    TLR_UINT16 bTDp;
    TLR_UINT8  bTMapc;
    TLR_UINT32 bTBase_Io;
    TLR_UINT16 bTI;
    TLR_UINT16 bTO;
    TLR_UINT32 bTDx;
    TLR_UINT16 bTPll_W;
    TLR_UINT16 bTPll_D;
  }__TLR_PACKED_POST PROFIBUS_FSPM_PARAMETER_DPV2_ISOM_PRM_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPM_PARAMETER_DPV2_DBX_LINK_FILTER_Ttag {
    TLR_UINT8 bPublisher_Addr;
    TLR_UINT8 bPublisher_Length;
    TLR_UINT8 bSample_Offset;
    TLR_UINT8 bSample_Length;
  }__TLR_PACKED_POST PROFIBUS_FSPM_PARAMETER_DPV2_DBX_LINK_FILTER_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPM_PARAMETER_DPV2_DBX_LINK_Ttag {
    PROFIBUS_FSPM_PARAMETER_DPV2_HEADER_T tDpv2Header;
    TLR_UINT8 bVersion;
    PROFIBUS_FSPM_PARAMETER_DPV2_DBX_LINK_FILTER_T tDbxLinkFilter;
  }__TLR_PACKED_POST PROFIBUS_FSPM_PARAMETER_DPV2_DBX_LINK_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPM_PARAMETER_DPV2_PRM_Ttag {
    PROFIBUS_FSPM_PARAMETER_DPV1_USR_PRM_T tDpv1;
    PROFIBUS_FSPM_PARAMETER_DPV2_ISOM_PRM_T tDpv2Iso;
    TLR_UINT8 abUsr_Prm_Data[PROFIBUS_FSPM_MAX_USER_PRM_DATA_SIZE-
                             sizeof(PROFIBUS_FSPM_PARAMETER_DPV1_USR_PRM_T)-
                             sizeof(PROFIBUS_FSPM_PARAMETER_DPV2_ISOM_PRM_T)];
  }__TLR_PACKED_POST PROFIBUS_FSPM_PARAMETER_DPV2_PRM_T;

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPM_PARAMETER_DATA_Ttag {


    #define MSK_FSPM_PARAMETER_STATION_STATUS_RESERVED    0x07
    #define MSK_FSPM_PARAMETER_STATION_STATUS_WD_ON       0x08
    #define MSK_FSPM_PARAMETER_STATION_STATUS_FREEZE_REQ  0x10
    #define MSK_FSPM_PARAMETER_STATION_STATUS_SYNC_REQ    0x20
    #define MSK_FSPM_PARAMETER_STATION_STATUS_UNLOCK_REQ  0x40
    #define MSK_FSPM_PARAMETER_STATION_STATUS_LOCK_REQ    0x80
    TLR_UINT8  bStation_Status;             /* status of supported functions */

    TLR_UINT8  bWD_Fact_1;     /* watchdog factor 1 */
    TLR_UINT8  bWD_Fact_2;     /* watchdog factor 2 */
    TLR_UINT8  bMin_Tsdr;      /* min. station delay reponder */
    TLR_UINT16 usIdent_Number; /* ident number of the station, motorola format */
    TLR_UINT8  bGroup_Ident;   /* configures group number */

    __TLR_PACKED_PRE union
    {
      PROFIBUS_FSPM_PARAMETER_DPV0_PRM_T tDpv0;     /* standard parameter , or */
      PROFIBUS_FSPM_PARAMETER_DPV1_PRM_T tDpv1Only; /* parameter data with DPV1 */
      PROFIBUS_FSPM_PARAMETER_DPV2_PRM_T tDpv1Dpv2; /* parameter data with DPV1,DPV2 */
    }__TLR_PACKED_POST un;
  }__TLR_PACKED_POST PROFIBUS_FSPM_PARAMETER_DATA_T;



  #define PROFIBUS_FSPM_DIAGNOSTIC_MASTER_ADD_INVALID 0xFF

  typedef __TLR_PACKED_PRE struct PROFIBUS_FSPM_DIAGNOSTIC_DATA_Ttag {

    #define MSK_FSPM_DIAGNOSTIC_STAT1_STATE_NON_EXISTENT 0x01
    #define MSK_FSPM_DIAGNOSTIC_STAT1_STATE_NOT_READY    0x02
    #define MSK_FSPM_DIAGNOSTIC_STAT1_STATE_CFG_FAULT    0x04
    #define MSK_FSPM_DIAGNOSTIC_STAT1_STATE_EXT_DIAG     0x08
    #define MSK_FSPM_DIAGNOSTIC_STAT1_STATE_NOT_SUPP     0x10
    #define MSK_FSPM_DIAGNOSTIC_STAT1_STATE_INVALID_RESP 0x20
    #define MSK_FSPM_DIAGNOSTIC_STAT1_STATE_PRM_FAULT    0x40
    #define MSK_FSPM_DIAGNOSTIC_STAT1_STATE_MASTER_LOCK  0x80
    TLR_UINT8 bStationstatus_1;

    #define MSK_FSPM_DIAGNOSTIC_STAT2_STATE_PRM_REQ      0x01
    #define MSK_FSPM_DIAGNOSTIC_STAT2_STATE_STAT_DIAG    0x02
    #define MSK_FSPM_DIAGNOSTIC_STAT2_STATE_TRUE         0x04
    #define MSK_FSPM_DIAGNOSTIC_STAT2_STATE_WD_ON        0x08
    #define MSK_FSPM_DIAGNOSTIC_STAT2_STATE_FREEZE_MODE  0x10
    #define MSK_FSPM_DIAGNOSTIC_STAT2_STATE_SYNC_MODE    0x20
    #define MSK_FSPM_DIAGNOSTIC_STAT2_STATE_RESERVED     0x40
    #define MSK_FSPM_DIAGNOSTIC_STAT2_STATE_DEACTIVATED  0x80
    TLR_UINT8 bStationstatus_2;

    #define MSK_FSPM_DIAGNOSTIC_STAT3_STATE_RESERVED           0x7F
    #define MSK_FSPM_DIAGNOSTIC_STAT3_STATE_EXT_DIAG_OVERFLOW  0x80
    TLR_UINT8 bStationstatus_3;

    TLR_UINT8 bMaster_Add;               /* corresponding master address */
    TLR_UINT16 usIdent_Number;           /* ident number, motorola format */
    TLR_UINT8 abExt_Diag_Data[PROFIBUS_FSPM_MAX_DIAG_DATA_SIZE];
                                         /* extended diagnostic field */
  }__TLR_PACKED_POST PROFIBUS_FSPM_DIAGNOSTIC_DATA_T;

  #define PROFIBUS_FSPM_DIAGNOSTIC_DATA_SIZE (sizeof(PROFIBUS_FSPM_DIAGNOSTIC_DATA_T) - PROFIBUS_FSPM_MAX_DIAG_DATA_SIZE)

/* pragma unpack */
#ifdef PRAGMA_PACK_ENABLE
#pragma PRAGMA_UNPACK_1(PROFIBUS_FSPM_COMMON)
#endif


/***************************************************************************************/
#endif /* #ifndef __PROFIBUS_FSPM_COMMON_H */
