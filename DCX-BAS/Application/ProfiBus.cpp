/*
 $Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/ProfiBus.cpp_v   1.12   02 Sep 2016 12:12:44   EGUAJARDO  $
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
 $Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/ProfiBus.cpp_v  $
 * 
 *    Rev 1.12   02 Sep 2016 12:12:44   EGUAJARDO
 * Test Selection radio button is changed to Test Amplitude mode from web page when test amplitude is changed via profibus.
 * 
 *    Rev 1.11   13 Mar 2014 15:54:58   rjamloki
 * FRAM is accessed through pointers. Removed compiler allocation of FRAM
 * 
 *    Rev 1.10   14 Nov 2013 09:55:14   rjamloki
 * Tracker issue fixes.
 * 
 *    Rev 1.9   08 Oct 2013 06:17:32   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.8   03 Sep 2013 12:53:06   ygupta
 * Moved to tip from 1.6.1.3.1.0
 * 
 *    Rev 1.6.1.3.1.0   24 Aug 2013 17:14:08   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.6.1.3   02 Aug 2013 10:55:48   amaurya
 * Coverty fixes.
 * 
 *    Rev 1.6.1.2   27 Jun 2013 05:56:16   amaurya
 * System crash fixes.
 * 
 *    Rev 1.6.1.1   08 Apr 2013 08:24:38   amaurya
 * Fixed Tracker issue of Ver2.0P.
 * 
 *    Rev 1.6.1.0   20 Mar 2013 09:38:22   ygupta
 * RF Switch Changes, Requirement Changes
 * 
 *    Rev 1.7   20 Mar 2013 08:18:28   ygupta
 * RF Switch Changes, Requirement Changes
 * 
 *    Rev 1.6   11 Mar 2013 03:08:26   ygupta
 * Issues fixed & Requirement Changes
 * 
 *    Rev 1.5   18 Feb 2013 03:09:24   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.4   22 Jan 2013 10:38:24   ygupta
 * Profibus Bug Fixes
 * 
 *    Rev 1.3   17 Jan 2013 07:25:32   ygupta
 * Profibus Related changes
 * 
 *    Rev 1.2   16 Jan 2013 06:28:16   ygupta
 * Level2 Code with Profibus
 */
#include "ProfiBus.h"
#include "ProfibusAps_Public.h"
#include "string.h"
#include "cifXEndianess.h"
#include "cifxErrors.h"
#include "stdio.h"
#include "CpuDefs.h"
#include "rcX_User.h"
#include "FieldbusTask.h"
#include "GLOBALS.h"
#include "FUNC.h"

#define printf
#define pprintf
#define TESTAMPSET_A_PROFI_PARAM	1475


////Flag to be made TRUE in case the received User Parameters from master are not correct. This is shown on Fieldbus diagnostic page
BOOLEAN ParameterFault;
////Flag to be made TRUE in case the received configuration from master is not correct. This is shown on Fieldbus diagnostic page
BOOLEAN ConfigurationFault;

//At powerup contains the warmstart configuration i.e. CFG_MODULE_PKW_BE and CFG_MODULE_PZD_BE
//later contains the configuration modules received from profibus master
UINT8 CurrentConfig[PROFIBUS_APS_MAX_CFG_DATA_SIZE];
//Contains the UserParameter receive from master
UINT8 CurrentUserParam[PROFIBUS_APS_MAX_USER_PRM_DATA_SIZE];
//The length of configuration data in bytes
UINT8 ConfigLen = 0;
//The length of User Parameter data in bytes
UINT8 UserParamLen = 0;

//Contains the Bytes defining configuration modules. A single byte represents a configuration
//module defined in GSD
static UINT8 DataExCfg[CFG_MODULE_IDX_MAX + 1];
//Data format Little, Big endian, Config defined or undefined.
static UINT8 DataFormat;

//Contains Data Exchange size every millisecond between profibus master and slave in bytes
//Initialized based on the received configuration
static UINT8 DataExSize;

//Contains Data Exchange size for the previous configuration.
//Initialized once when new configuration is received.
static UINT8 DataExSizeClear;
//Contains the GSD version received in User parameter
static UINT8 GsdVersion;

//Global Extended diagnostic object to be filled when fieldbus diagnostic page is opened
//Contains the BusAddresse of slave and Baudrate information
FSPMS_EXTENDED_DIAG_T EStat;


//special commands 1204 to 1209 related vars
BOOLEAN LastSSCopied = TRUE;
UINT32 SSOverRunIndication = 0, LastSSOverRunIndication;
UINT8 LastSSStatus = LAST_NOT_AVAILABLE;
UINT8 LastStatusSetNumber = MAXIMUM_USER_PRESETS, StatusSetNumber = MAXIMUM_USER_PRESETS;

//special commands 1604 to 1609 related vars
BOOLEAN LastHSSSCopied = TRUE;
UINT8 HSSSOverRunIndication[MAXIMUM_USER_HSPRESETS],
   //updated on 1606 from HSSSOverRunIndication
   LastHSSSOverRunIndication[MAXIMUM_USER_HSPRESETS];
UINT8 LastHSSSStatus = LAST_NOT_AVAILABLE;
//Last SFct Number in Comm.-SSSS
UINT8 LastSFCT = MAX_HS_FUNC , SFCT = MAX_HS_FUNC;
//Last SFct Number for all HSSTATUS
UINT8 LastSFCTInHSStatus[MAXIMUM_USER_HSPRESETS] = { MAX_HS_FUNC, MAX_HS_FUNC, MAX_HS_FUNC, MAX_HS_FUNC, MAX_HS_FUNC,
      MAX_HS_FUNC, MAX_HS_FUNC, MAX_HS_FUNC, MAX_HS_FUNC, MAX_HS_FUNC, MAX_HS_FUNC, MAX_HS_FUNC, MAX_HS_FUNC,
      MAX_HS_FUNC, MAX_HS_FUNC, MAX_HS_FUNC };

UINT8 HSStatusSetNumber = MAXIMUM_USER_HSPRESETS,
      //updated on 1606 from HSStatusSetNumber and from 1604
      //SSSS Number in Comm.-SSSS
      LastHSStatusSetNumber = MAXIMUM_USER_HSPRESETS;


union InPZDSV{
   UINT32 In;
   UINT8 AIn[NUM_PZDV_MODULES];
} InPZDSVSelectUserPara;

union OutPZDSV{
   UINT32 Out;
   UINT8 AOut[NUM_PZDV_MODULES];
} OutPZDZVSelectUserPara;

//For Cyclic parameter(like amplitude/frequency exchange without going through the PKW
//handshake model)
ContExch ContIO;
extern BOOLEAN RegUpdateCmdFlag;
/*  ProfiBus Constructor
 *
 *  Purpose:
 *       One time initialization of profibus related things.Called once at powerup when Profibus object is created from FieldBus FindFieldBusType() function..
 *
 *  Entry condition: None.
 *
 *  Exit condition: None.
 */
ProfiBus::ProfiBus() {
//   pprintf("\n pb constructor");
   AK = 0;
   PNU = 0;
   IND = 0;
   PWE = 0;
}


/* void ProfiBus::UpdateInputCyclic(UINT8 * InPtr)
 *
 * Purpose:
 * To initialize input PZD and PKW field in global FieldbusIO object FbusIO from the data received from profibus master,
 * based on the current configuration. This function is called by FieldBusTask::Run() function
 * every millisecond if the Hilscher Profibus slave card is detected at power up.
 *
 *  Entry condition:
 *       InPtr- Pointer to the cyclic data buffer recived from profibus master
 *
 *  Exit condition:
 *       None:
 */
void ProfiBus::UpdateInputCyclic(UINT8 * InPtr) {

   /*Local Definitiones ****************************************************/
   BOOLEAN UpdatePZD = FALSE;
   BOOLEAN UpdatePKWPKWDiag = FALSE;
   BOOLEAN UpdatePZDV[NUM_PZDV_MODULES] = {FALSE, FALSE, FALSE, FALSE};
   BOOLEAN UpdatePZDSELV[NUM_PZDV_MODULES] = {FALSE, FALSE, FALSE, FALSE};
   UINT8 ModuleIdxCount;
   UINT16 InVal = 0;
   Controlword1 = 0;
   Controlword2 = 0;
   UINT32 Indx;
   if ((InPtr != NULL) && (DataExSize > DATA_EX_SIZE_MIN_INIT)) {
       /*Select all Channels                                    */
      for (Indx = 0; Indx < NUM_PZDV_MODULES; Indx++) {
         if (ContIO.InPZDSVSelectOffset[Indx] != PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)//offset is there means not defined in param
            ContIO.SelIn[Indx] = *(InPtr + ContIO.InPZDSVSelectOffset[Indx]);
         else
            ContIO.SelIn[Indx] = InPZDSVSelectUserPara.AIn[Indx];

         if (ContIO.OutPZDZVSelectOffset[Indx] != PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)
            ContIO.SelOut[Indx] = *(InPtr + ContIO.OutPZDZVSelectOffset[Indx]);
         else
            ContIO.SelOut[Indx] = OutPZDZVSelectUserPara.AOut[Indx];
      }
      /*Handle Modules                                      */
      for (ModuleIdxCount = 0; DataExCfg[ModuleIdxCount] != DATA_EX_CFG_END_MARK; ModuleIdxCount++) {
         switch (DataExCfg[ModuleIdxCount]) {
         case DATA_EX_CFG_PZD_BE: {
            /*Update PZD-area                            */
            if (UpdatePZD == 0) {
               FbusIO.PZDIn.PZDu8In.PZDIn[0] = *InPtr++;
               FbusIO.PZDIn.PZDu8In.PZDIn[1] = *InPtr++;
               FbusIO.PZDIn.PZDu8In.PZDIn[2] = *InPtr++;
               FbusIO.PZDIn.PZDu8In.PZDIn[3] = *InPtr++;
               UpdatePZD = 1;
               Controlword1 = FbusIO.PZDIn.PZDu16In.STWIn1;
               Controlword2 = FbusIO.PZDIn.PZDu16In.STWIn2;
            }
            else {
               InPtr += 4;
            }
         }
            break;
         case DATA_EX_CFG_PZD_LE: {
            /*Update PZD-area                            */
            if (UpdatePZD == 0) {
               FbusIO.PZDIn.PZDu8In.PZDIn[1] = *InPtr++;
               FbusIO.PZDIn.PZDu8In.PZDIn[0] = *InPtr++;
               FbusIO.PZDIn.PZDu8In.PZDIn[3] = *InPtr++;
               FbusIO.PZDIn.PZDu8In.PZDIn[2] = *InPtr++;
               Controlword1 = FbusIO.PZDIn.PZDu16In.STWIn1;
               Controlword2 = FbusIO.PZDIn.PZDu16In.STWIn2;
               UpdatePZD = 1;
            }
            else {
               InPtr += 4;
            }
         }
            break;
         case DATA_EX_CFG_PKW_BE: {
            /*Update PKW-area                            */
            if (UpdatePKWPKWDiag == 0) {
               FbusIO.PKWIn.PKWu8In.PKWIn[0] = *InPtr++;
               FbusIO.PKWIn.PKWu8In.PKWIn[1] = *InPtr++;
               FbusIO.PKWIn.PKWu8In.PKWIn[2] = *InPtr++;
               FbusIO.PKWIn.PKWu8In.PKWIn[3] = *InPtr++;
               FbusIO.PKWIn.PKWu8In.PKWIn[4] = *InPtr++;
               FbusIO.PKWIn.PKWu8In.PKWIn[5] = *InPtr++;
               FbusIO.PKWIn.PKWu8In.PKWIn[6] = *InPtr++;
               FbusIO.PKWIn.PKWu8In.PKWIn[7] = *InPtr++;
               AK = (FbusIO.PKWIn.PKWu8In.PKWIn[0] & 0xF0) >> 4;
               PNU = (((FbusIO.PKWIn.PKWu8In.PKWIn[0] & 0x0F) << 8) | FbusIO.PKWIn.PKWu8In.PKWIn[1]);
               IND = ((FbusIO.PKWIn.PKWu8In.PKWIn[2] << 8) | FbusIO.PKWIn.PKWu8In.PKWIn[3]);
               PWE = ((FbusIO.PKWIn.PKWu8In.PKWIn[4] << 24) | (FbusIO.PKWIn.PKWu8In.PKWIn[5] << 16)
                     | (FbusIO.PKWIn.PKWu8In.PKWIn[6] << 8) | FbusIO.PKWIn.PKWu8In.PKWIn[7]);
               UpdatePKWPKWDiag = 1;
            }
            else {
               InPtr += 8;
            }
         }
            break;
         case DATA_EX_CFG_PKW_LE: {
            /*Update PKW-area                            */
            if (UpdatePKWPKWDiag == 0) {
               FbusIO.PKWIn.PKWu8In.PKWIn[1] = *InPtr++;
               FbusIO.PKWIn.PKWu8In.PKWIn[0] = *InPtr++;
               FbusIO.PKWIn.PKWu8In.PKWIn[3] = *InPtr++;
               FbusIO.PKWIn.PKWu8In.PKWIn[2] = *InPtr++;
               FbusIO.PKWIn.PKWu8In.PKWIn[5] = *InPtr++;
               FbusIO.PKWIn.PKWu8In.PKWIn[4] = *InPtr++;
               FbusIO.PKWIn.PKWu8In.PKWIn[7] = *InPtr++;
               FbusIO.PKWIn.PKWu8In.PKWIn[6] = *InPtr++;
               AK = (FbusIO.PKWIn.PKWu8In.PKWIn[0] & 0xF0) >> 4;
               PNU = (((FbusIO.PKWIn.PKWu8In.PKWIn[0] & 0x0F) << 8) | FbusIO.PKWIn.PKWu8In.PKWIn[1]);
               IND = ((FbusIO.PKWIn.PKWu8In.PKWIn[2] << 8) | FbusIO.PKWIn.PKWu8In.PKWIn[3]);
               PWE = ((FbusIO.PKWIn.PKWu8In.PKWIn[4] << 24) | (FbusIO.PKWIn.PKWu8In.PKWIn[5] << 16)
                     | (FbusIO.PKWIn.PKWu8In.PKWIn[6] << 8) | FbusIO.PKWIn.PKWu8In.PKWIn[7]);
               UpdatePKWPKWDiag = 1;
            }
            else {
               InPtr += 8;
            }
         }
            break;
         case DATA_EX_CFG_PZDV_1_BE: {
            /*Update PZDV-area                              */
            if (UpdatePZDV[0] == 0) {
               InVal = ((UINT16) (*InPtr++)) << 8;
               InVal |= ((UINT16) (*InPtr++));
               ContIO.ValueIn[0] = InVal;
               UpdatePZDV[0] = 1;
            }
            else {
               InPtr += 2;
            }
         }
            break;
         case DATA_EX_CFG_PZDV_1_LE: {
            /*Update PZDV-area                              */
            if (UpdatePZDV[0] == 0) {
               InVal = ((UINT16) (*InPtr++));
               InVal |= ((UINT16) (*InPtr++)) << 8;
               ContIO.ValueIn[0] = InVal;
               UpdatePZDV[0] = 1;
            }
            else {
               InPtr += 2;
            }
         }
            break;
         case DATA_EX_CFG_PZDV_2_BE: {
            /*Update PZDV-area                              */
            if (UpdatePZDV[1] == 0) {
               InVal = ((UINT16) (*InPtr++)) << 8;
               InVal |= ((UINT16) (*InPtr++));
               ContIO.ValueIn[1] = InVal;
               UpdatePZDV[1] = 1;
            }
            else {
               InPtr += 2;
            }
         }
            break;
         case DATA_EX_CFG_PZDV_2_LE: {
            /*Update PZDV-area                              */
            if (UpdatePZDV[1] == 0) {
               InVal = ((UINT16) (*InPtr++));
               InVal |= ((UINT16) (*InPtr++)) << 8;
               ContIO.ValueIn[1] = InVal;
               UpdatePZDV[1] = 1;
            }
            else {
               InPtr += 2;
            }
         }
            break;
         case DATA_EX_CFG_PZDV_3_BE: {
            /*Update PZDV-area                              */
            if (UpdatePZDV[2] == 0) {
               InVal = ((UINT16) (*InPtr++)) << 8;
               InVal |= ((UINT16) (*InPtr++));
               ContIO.ValueIn[2] = InVal;
               UpdatePZDV[2] = 1;
            }
            else {
               InPtr += 2;
            }
         }
            break;
         case DATA_EX_CFG_PZDV_3_LE: {
            /*Update PZDV-area                              */
            if (UpdatePZDV[2] == 0) {
               InVal = ((UINT16) (*InPtr++));
               InVal |= ((UINT16) (*InPtr++)) << 8;
               ContIO.ValueIn[2] = InVal;
               UpdatePZDV[2] = 1;
            }
            else {
               InPtr += 2;
            }
         }
            break;
         case DATA_EX_CFG_PZDV_4_BE: {
            /*Update PZDV-area                              */
            if (UpdatePZDV[3] == 0) {
               InVal = ((UINT16) (*InPtr++)) << 8;
               InVal |= ((UINT16) (*InPtr++));
               ContIO.ValueIn[3] = InVal;
               UpdatePZDV[3] = 1;
            }
            else {
               InPtr += 2;
            }
         }
            break;
         case DATA_EX_CFG_PZDV_4_LE: {
            /*Update PZDV-area                              */
            if (UpdatePZDV[3] == 0) {
               InVal = ((UINT16) (*InPtr++));
               InVal |= ((UINT16) (*InPtr++)) << 8;
               ContIO.ValueIn[3] = InVal;
               UpdatePZDV[3] = 1;
            }
            else {
               InPtr += 2;
            }
         }
            break;
         case DATA_EX_CFG_PZDSELV_1_BE: {
            /*Update PZDV-area                              */
            if (UpdatePZDSELV[0] == 0) {
               InPtr += 2;
               InVal = ((UINT16) (*InPtr++)) << 8;
               InVal |= ((UINT16) (*InPtr++));
               ContIO.ValueIn[0] = InVal;
               UpdatePZDSELV[0] = 1;
            }
            else {
               InPtr += 4;
            }
         }
            break;
         case DATA_EX_CFG_PZDSELV_1_LE: {
            /*Update PZDV-area                              */
            if (UpdatePZDSELV[0] == 0) {
               InPtr += 2;
               InVal = ((UINT16) (*InPtr++));
               InVal |= ((UINT16) (*InPtr++)) << 8;
               ContIO.ValueIn[0] = InVal;
               UpdatePZDSELV[0] = 1;
            }
            else {
               InPtr += 4;
            }
         }
            break;
         case DATA_EX_CFG_PZDSELV_2_BE: {
            /*Update PZDV-area                              */
            if (UpdatePZDSELV[1] == 0) {
               InPtr += 2;
               InVal = ((UINT16) (*InPtr++)) << 8;
               InVal |= ((UINT16) (*InPtr++));
               ContIO.ValueIn[1] = InVal;
               UpdatePZDSELV[1] = 1;
            }
            else {
               InPtr += 4;
            }
         }
            break;
         case DATA_EX_CFG_PZDSELV_2_LE: {
            /*Update PZDV-area                              */
            if (UpdatePZDSELV[1] == 0) {
               InPtr += 2;
               InVal = ((UINT16) (*InPtr++));
               InVal |= ((UINT16) (*InPtr++)) << 8;
               ContIO.ValueIn[1] = InVal;
               UpdatePZDSELV[1] = 1;
            }
            else {
               InPtr += 4;
            }
         }
            break;
         case DATA_EX_CFG_PZDSELV_3_BE: {
            /*Update PZDV-area                              */
            if (UpdatePZDSELV[2] == 0) {
               InPtr += 2;
               InVal = ((UINT16) (*InPtr++)) << 8;
               InVal |= ((UINT16) (*InPtr++));
               ContIO.ValueIn[2] = InVal;
               UpdatePZDSELV[2] = 1;
            }
            else {
               InPtr += 4;
            }
         }
            break;
         case DATA_EX_CFG_PZDSELV_3_LE: {
            /*Update PZDV-area                              */
            if (UpdatePZDSELV[2] == 0) {
               InPtr += 2;
               InVal = ((UINT16) (*InPtr++));
               InVal |= ((UINT16) (*InPtr++)) << 8;
               ContIO.ValueIn[2] = InVal;
               UpdatePZDSELV[2] = 1;
            }
            else {
               InPtr += 4;
            }
         }
            break;
         case DATA_EX_CFG_PZDSELV_4_BE: {
            /*Update PZDV-area                              */
            if (UpdatePZDSELV[3] == 0) {
               InPtr += 2;
               InVal = ((UINT16) (*InPtr++)) << 8;
               InVal |= ((UINT16) (*InPtr++));
               ContIO.ValueIn[3] = InVal;
               UpdatePZDSELV[3] = 1;
            }
            else {
               InPtr += 4;
            }
         }
            break;
         case DATA_EX_CFG_PZDSELV_4_LE: {
            /*Update PZDV-area                              */
            if (UpdatePZDSELV[3] == 0) {
               InPtr += 2;
               InVal = ((UINT16) (*InPtr++));
               InVal |= ((UINT16) (*InPtr++)) << 8;
               ContIO.ValueIn[3] = InVal;
               UpdatePZDSELV[3] = 1;
            }
            else {
               InPtr += 4;
            }
         }
            break;
         default:
         case DATA_EX_CFG_END_VALUE: {
         }
            break;
         }
         /*END, Use "JUMP_TABLE" optimized for speed!, Set to Default   */
      }
   }
//   else{
//      if(CommIfcAccessToken == COMM_IFC_FBB)
//          ReleaseAccessToken(COMM_IFC_FBB);
//   }


   /*Clear PZD                                              */

   if (UpdatePZD == 0) {
      memset(&FbusIO.PZDIn.PZDu8In.PZDIn, '\0', sizeof(FbusIO.PZDIn.PZDu8In.PZDIn));
   }

   /*Clear PKW, Update,- Clear PKW-Diagnosis                      */
   if (UpdatePKWPKWDiag == 0) {
      memset(&FbusIO.PKWIn, 0, sizeof(FbusIO.PKWIn));
   }

   //Clear PZDV_X
   for (Indx = 0; Indx < NUM_PZDV_MODULES; Indx++) {
      if ((UpdatePZDV[Indx] == 0) && (UpdatePZDSELV[Indx] == 0)) {
         ContIO.SelIn[Indx] = 0x00;
         ContIO.ValueIn[Indx] = 0;
         ContIO.SelOut[Indx] = 0x00;
         ContIO.ValueOut[Indx] = 0;
      }
   }

//   if (memcmp(&FbusIOTemp.PKWIn, &FbusIO.PKWIn, sizeof(FbusIO.PKWIn))) {
//      memcpy(&FbusIOTemp.PKWIn, &FbusIO.PKWIn, sizeof(FbusIO.PKWIn));
//      printf("\n  %08X %08X", FbusIOTemp.PKWIn.PKWu32In.PkwLow, FbusIOTemp.PKWIn.PKWu32In.PkwHigh);
//   }
   ProcessControlSignal();
   ProcessPKW();
   ProcessAnalogInput();
}

/*  void ProfiBus::UpdateOutputCyclic(UINT8 * OutPtr)
 *
 *  Purpose:
 *     To initialize output PZD and PKW field in global FieldbusIO object FbusIO to be sent to profibus master,
 *     based on the current configuration. This function is called by FieldBusTask::Run() function
 *     every millisecondv if the Hilscher Profibus slave card is detected at power up.
 *
 *  Entry condition:
 *     OutPtr- Pointer to the cyclic data buffer to be sent to profibus master
 *
 *  Exit condition: None:.
 */
void ProfiBus::UpdateOutputCyclic(UINT8* OutPtr) {
   /*Local Definitions ****************************************************/
   UINT8 DataExCount;
   UINT8 ModuleIdxCount;
   Statusword1 = 0;
   Statusword2 = 0;
   PrepareStatusSignal();
   ProcessAnalogOutput();
   if ((DataExSize == DATA_EX_SIZE_MIN_INIT) && (OutPtr != NULL)) {
      if (DataExSizeClear != 0) {
         for (DataExCount = 0; DataExCount < DataExSizeClear; DataExCount++) {
            *OutPtr++ = 0x00;
         }
         /*Do Onetime                                       */
         DataExSizeClear = 0;
      }
   }
   else if(OutPtr != NULL){
      /*Handle Modules                                      */
      for (ModuleIdxCount = 0; DataExCfg[ModuleIdxCount] != DATA_EX_CFG_END_MARK; ModuleIdxCount++) {
         switch (DataExCfg[ModuleIdxCount]) {
         case DATA_EX_CFG_PZD_BE: {
            /*Update PZD-area                            */
            *OutPtr++ = FbusIO.PZDOut.PZDu8Out.PZDOut[0];
            *OutPtr++ = FbusIO.PZDOut.PZDu8Out.PZDOut[1];
            *OutPtr++ = FbusIO.PZDOut.PZDu8Out.PZDOut[2];
            *OutPtr++ = FbusIO.PZDOut.PZDu8Out.PZDOut[3];

            Statusword1 = FbusIO.PZDOut.PZDu16Out.ZSWOut1;
            Statusword2 = FbusIO.PZDOut.PZDu16Out.ZSWOut2;
         }
            break;
         case DATA_EX_CFG_PZD_LE: {
            Statusword1 = FbusIO.PZDOut.PZDu16Out.ZSWOut1;
            Statusword2 = FbusIO.PZDOut.PZDu16Out.ZSWOut2;
            /*Update PZD-area*/
            *OutPtr++ = FbusIO.PZDOut.PZDu8Out.PZDOut[1];
            *OutPtr++ = FbusIO.PZDOut.PZDu8Out.PZDOut[0];
            *OutPtr++ = FbusIO.PZDOut.PZDu8Out.PZDOut[3];
            *OutPtr++ = FbusIO.PZDOut.PZDu8Out.PZDOut[2];
         }
            break;
         case DATA_EX_CFG_PKW_BE: {
            /*Update PKW-area                            */
            *OutPtr++ = FbusIO.PKWOut.PKWu8Out.PKWOut[0];
            *OutPtr++ = FbusIO.PKWOut.PKWu8Out.PKWOut[1];
            *OutPtr++ = FbusIO.PKWOut.PKWu8Out.PKWOut[2];
            *OutPtr++ = FbusIO.PKWOut.PKWu8Out.PKWOut[3];
            *OutPtr++ = FbusIO.PKWOut.PKWu8Out.PKWOut[4];
            *OutPtr++ = FbusIO.PKWOut.PKWu8Out.PKWOut[5];
            *OutPtr++ = FbusIO.PKWOut.PKWu8Out.PKWOut[6];
            *OutPtr++ = FbusIO.PKWOut.PKWu8Out.PKWOut[7];
         }
            break;
         case DATA_EX_CFG_PKW_LE: {
            /*Update PKW-area                            */
            *OutPtr++ = FbusIO.PKWOut.PKWu8Out.PKWOut[1];
            *OutPtr++ = FbusIO.PKWOut.PKWu8Out.PKWOut[0];
            *OutPtr++ = FbusIO.PKWOut.PKWu8Out.PKWOut[3];
            *OutPtr++ = FbusIO.PKWOut.PKWu8Out.PKWOut[2];
            *OutPtr++ = FbusIO.PKWOut.PKWu8Out.PKWOut[5];
            *OutPtr++ = FbusIO.PKWOut.PKWu8Out.PKWOut[4];
            *OutPtr++ = FbusIO.PKWOut.PKWu8Out.PKWOut[7];
            *OutPtr++ = FbusIO.PKWOut.PKWu8Out.PKWOut[6];
         }
            break;
         case DATA_EX_CFG_PZDV_1_BE: {
            /*Update PZDV-area                              */
            *OutPtr++ = ContIO.ValueOut[0] >> 8;
            *OutPtr++ = ContIO.ValueOut[0];
         }
            break;
         case DATA_EX_CFG_PZDV_1_LE: {
            /*Update PZDV-area                              */
            *OutPtr++ = ContIO.ValueOut[0];
            *OutPtr++ = ContIO.ValueOut[0] >> 8;
         }
            break;
         case DATA_EX_CFG_PZDV_2_BE: {
            /*Update PZDV-area                              */
            *OutPtr++ = ContIO.ValueOut[1] >> 8;
            *OutPtr++ = ContIO.ValueOut[1];
         }
            break;
         case DATA_EX_CFG_PZDV_2_LE: {
            /*Update PZDV-area                              */
            *OutPtr++ = ContIO.ValueOut[1];
            *OutPtr++ = ContIO.ValueOut[1] >> 8;
         }
            break;
         case DATA_EX_CFG_PZDV_3_BE: {
            /*Update PZDV-area                              */
            *OutPtr++ = ContIO.ValueOut[2] >> 8;
            *OutPtr++ = ContIO.ValueOut[2];
         }
            break;
         case DATA_EX_CFG_PZDV_3_LE: {
            /*Update PZDV-area                              */
            *OutPtr++ = ContIO.ValueOut[2];
            *OutPtr++ = ContIO.ValueOut[2] >> 8;
         }
            break;
         case DATA_EX_CFG_PZDV_4_BE: {
            /*Update PZDV-area                              */
            *OutPtr++ = ContIO.ValueOut[3] >> 8;
            *OutPtr++ = ContIO.ValueOut[3];
         }
            break;
         case DATA_EX_CFG_PZDV_4_LE: {
            /*Update PZDV-area                              */
            *OutPtr++ = ContIO.ValueOut[3];
            *OutPtr++ = ContIO.ValueOut[3] >> 8;
         }
            break;
         case DATA_EX_CFG_PZDSELV_1_BE: {
            /*Update PZDV-area                              */
            *OutPtr++ = ContIO.SelAckIn[0];
            *OutPtr++ = ContIO.SelAckOut[0];
            *OutPtr++ = ContIO.ValueOut[0] >> 8;
            *OutPtr++ = ContIO.ValueOut[0];
         }
            break;
         case DATA_EX_CFG_PZDSELV_1_LE: {
            /*Update PZDV-area                              */
            *OutPtr++ = ContIO.SelAckOut[0];
            *OutPtr++ = ContIO.SelAckIn[0];
            *OutPtr++ = ContIO.ValueOut[0];
            *OutPtr++ = ContIO.ValueOut[0] >> 8;
         }
            break;
         case DATA_EX_CFG_PZDSELV_2_BE: {
            /*Update PZDV-area                              */
            *OutPtr++ = ContIO.SelAckIn[1];
            *OutPtr++ = ContIO.SelAckOut[1];
            *OutPtr++ = ContIO.ValueOut[1] >> 8;
            *OutPtr++ = ContIO.ValueOut[1];
         }
            break;
         case DATA_EX_CFG_PZDSELV_2_LE: {
            /*Update PZDV-area                              */
            *OutPtr++ = ContIO.SelAckOut[1];
            *OutPtr++ = ContIO.SelAckIn[1];
            *OutPtr++ = ContIO.ValueOut[1];
            *OutPtr++ = ContIO.ValueOut[1] >> 8;
         }
            break;
         case DATA_EX_CFG_PZDSELV_3_BE: {
            /*Update PZDV-area                              */
            *OutPtr++ = ContIO.SelAckIn[2];
            *OutPtr++ = ContIO.SelAckOut[2];
            *OutPtr++ = ContIO.ValueOut[2] >> 8;
            *OutPtr++ = ContIO.ValueOut[2];
         }
            break;
         case DATA_EX_CFG_PZDSELV_3_LE: {
            /*Update PZDV-area                              */
            *OutPtr++ = ContIO.SelAckOut[2];
            *OutPtr++ = ContIO.SelAckIn[2];
            *OutPtr++ = ContIO.ValueOut[2];
            *OutPtr++ = ContIO.ValueOut[2] >> 8;
         }
            break;
         case DATA_EX_CFG_PZDSELV_4_BE: {
            /*Update PZDV-area                              */
            *OutPtr++ = ContIO.SelAckIn[3];
            *OutPtr++ = ContIO.SelAckOut[3];
            *OutPtr++ = ContIO.ValueOut[3] >> 8;
            *OutPtr++ = ContIO.ValueOut[3];
         }
            break;
         case DATA_EX_CFG_PZDSELV_4_LE: {
            /*Update PZDV-area                              */
            *OutPtr++ = ContIO.SelAckOut[3];
            *OutPtr++ = ContIO.SelAckIn[3];
            *OutPtr++ = ContIO.ValueOut[3];
            *OutPtr++ = ContIO.ValueOut[3] >> 8;

         }
            break;
         default:
         case DATA_EX_CFG_END_VALUE: {
         }
            break;
         }
         /*END, Use "JUMP_TABLE" optimized for speed!, Set to Default   */

      }
   }
}

/* BOOLEAN ProfiBus::GetWarmStartParams(CIFX_PACKET * WSPkt)
 *
 *  Purpose:
 *     To initialize the warm start packet which needs to be sent to Filscher slave card at powerup.
 *     This function is called once by FieldBusTask::Run() function if the Hilscher Profibus slave card
 *     is detected at power up.
 *
 *  Entry condition:
 *     WSPkt- Pointer to the CIFX_PACKET where warm start parameters needs to be fille
 *
 *  Exit condition:
 *     Returns TRUE if the warm start packet is supported else returns FALSE
 */
BOOLEAN ProfiBus::GetWarmStartParams(CIFX_PACKET * WSPkt) {

   memset(WSPkt, 0, sizeof(*WSPkt));
   UINT32 CfgLen = 0;

   PROFIBUS_APS_PACKET_SET_CONFIGURATION_REQ_T* PtProfibusSetConfigReq;
   PtProfibusSetConfigReq = (PROFIBUS_APS_PACKET_SET_CONFIGURATION_REQ_T*) WSPkt;
   memset((void*) PtProfibusSetConfigReq, 0, sizeof(*PtProfibusSetConfigReq));

   PtProfibusSetConfigReq->tHead.ulDest = HOST_TO_LE32(0x00000020);
   PtProfibusSetConfigReq->tHead.ulSrc = HOST_TO_LE32(0);
   PtProfibusSetConfigReq->tHead.ulCmd = HOST_TO_LE32(PROFIBUS_APS_SET_CONFIGURATION_REQ);
   PtProfibusSetConfigReq->tData.ulSystemFlags = HOST_TO_LE32(0);
   PtProfibusSetConfigReq->tData.ulWdgTime = HOST_TO_LE32(0);
   PtProfibusSetConfigReq->tData.usIdentNumber = HOST_TO_LE16(PROFIBUS_UNIQUEIDENT);
   if (Sysconfig->PbAddress <= PB_MAX_ADDR)
      PtProfibusSetConfigReq->tData.bBusAddr = Sysconfig->PbAddress;
   else {
      Sysconfig->PbAddress = PB_DEFAULT_ADDR;
      PtProfibusSetConfigReq->tData.bBusAddr = Sysconfig->PbAddress;
   }

   PtProfibusSetConfigReq->tData.bBaudRate = 15;//Auto
   PtProfibusSetConfigReq->tData.bFlags = 0xC0;//On notifications 1100 0000
   //Initial settings to be overridden by master
   PtProfibusSetConfigReq->tData.abCfgData[CfgLen++] = 0xC1;
   PtProfibusSetConfigReq->tData.abCfgData[CfgLen++] = 0xC3;
   PtProfibusSetConfigReq->tData.abCfgData[CfgLen++] = 0xC3;
   PtProfibusSetConfigReq->tData.abCfgData[CfgLen++] = 0x01;
   PtProfibusSetConfigReq->tData.abCfgData[CfgLen++] = 0xC1;
   PtProfibusSetConfigReq->tData.abCfgData[CfgLen++] = 0xC1;
   PtProfibusSetConfigReq->tData.abCfgData[CfgLen++] = 0xC1;
   PtProfibusSetConfigReq->tData.abCfgData[CfgLen++] = 0x02;
   PtProfibusSetConfigReq->tData.bCfgLen = CfgLen;
   ConfigLen = CfgLen;
   memcpy(CurrentConfig, PtProfibusSetConfigReq->tData.abCfgData, CfgLen);
   PtProfibusSetConfigReq->tHead.ulLen
         = HOST_TO_LE32(sizeof(PtProfibusSetConfigReq->tData) - sizeof(PtProfibusSetConfigReq->tData.abCfgData) + CfgLen);
   return TRUE;
}

/* void ProfiBus::ProcessMailBox(CIFX_PACKET * MBXPkt)
 *
 * Purpose:
 *    To process the indication from Hilsher slave card. Mainly two indications are of interset parameter and configuration change.
 *    This function is called by FieldBusTask::Run() function when an indication is received from Hilscher slave card.
 *
 * Entry condition:
 *    MBXPkt- Pointer to the CIFX_PACKET which contains the parameter or configuration change indication.
 *
 * Exit condition:
 *    None.
 */
void ProfiBus::ProcessMailBox(CIFX_PACKET * MBXPkt) {
   //PROFIBUS_APS_PACKET_CHECK_CFG_IND_T ChkCnfIndPkt;
   PROFIBUS_APS_PACKET_CHECK_CFG_RES_T Pkt;
   CIFX_PACKET * CifxPkt = MBXPkt;
   /* Check if we have command or answer packet */
   if ((LE32_TO_HOST(MBXPkt->tHeader.ulCmd) & CIFX_MSK_PACKET_ANSWER) == 0) {
//      pprintf("\n %s rec ind %08X ", __FUNCTION__, LE32_TO_HOST(MBXPkt->tHeader.ulCmd));
      memcpy(&Pkt.tHead, &MBXPkt->tHeader, sizeof(MBXPkt->tHeader));
      Pkt.tHead.ulLen = HOST_TO_LE32(0x00000000);
      Pkt.tHead.ulSta = HOST_TO_LE32(RCX_E_UNEXPECTED);
      Pkt.tHead.ulCmd = HOST_TO_LE32(LE32_TO_HOST(MBXPkt->tHeader.ulCmd) + 1);//HOST_TO_LE32(MBXPkt->tHeader.ulCmd) | 1;
      /*-------------------------------------*/
      /* this is a indication packet*/
      /*-------------------------------------*/
      /* TODO: handle the indication packet */
      if ((PROFIBUS_APS_CHECK_CFG_IND == LE32_TO_HOST(MBXPkt->tHeader.ulCmd))) {
         ConfigLen = LE32_TO_HOST(MBXPkt->tHeader.ulLen);
         if(ConfigLen > PROFIBUS_APS_MAX_CFG_DATA_SIZE){
        	// UpdateTPVal(86);
        	 ConfigLen = PROFIBUS_APS_MAX_CFG_DATA_SIZE;
         }
         memcpy(CurrentConfig, MBXPkt->abData, ConfigLen);
//         pprintf
//         ("\n new config len %d\n", ConfigLen);
//         for (int i = 0; i < ConfigLen; i++){
//            pprintf("%02X ", CurrentConfig[i]);
//         }

         Pkt.tHead.ulSta = HOST_TO_LE32(RCX_S_OK);
         Pkt.tHead.ulLen = HOST_TO_LE32(0x00000001);
         if (HandleNewConfigData() == TRUE) {
//            pprintf("Config OK");
            ConfigurationFault = FALSE;
            Pkt.tData.fCfgOk = (1);
         }
         else {
            Pkt.tData.fCfgOk = (0);
            ConfigurationFault = TRUE;
//            pprintf("Config Not OK");
         }
      }
      else if (PROFIBUS_APS_CHECK_USER_PRM_IND == LE32_TO_HOST(MBXPkt->tHeader.ulCmd)) {
         UserParamLen = LE32_TO_HOST(MBXPkt->tHeader.ulLen);
         if(UserParamLen > PROFIBUS_APS_MAX_USER_PRM_DATA_SIZE){
        	 UserParamLen = PROFIBUS_APS_MAX_USER_PRM_DATA_SIZE;
        	// UpdateTPVal(87);
         }
         memcpy(CurrentUserParam, MBXPkt->abData, UserParamLen);
         printf
//         ("\n New Param ----- \n");
//         for (int i = 0; i < UserParamLen; i++){
//            pprintf("%02X ", CurrentUserParam[i]);
//         }

         Pkt.tHead.ulSta = HOST_TO_LE32(RCX_S_OK);
         Pkt.tHead.ulLen = HOST_TO_LE32(0x00000001);
         if (HandleNewParameterData() == TRUE) {
            Pkt.tData.fCfgOk = (1);
            ParameterFault = FALSE;
         }
         else {
            ParameterFault = TRUE;
            Pkt.tData.fCfgOk = (0);
         }
      }
     // CifxPkt = (CIFX_PACKET*) &Pkt;
      memcpy(CifxPkt, &Pkt, sizeof(Pkt));
      if (FieldbusTask::thisPtr->CifxPkt.Write(0, *CifxPkt, 0) == FALSE){
//         pprintf("Error sending Check Configuration/user param Indication Response to device!\r\n");
      }
   }
   else {
//      pprintf("\n Received CNF %08X with err code %08X", LE32_TO_HOST(MBXPkt->tHeader.ulCmd), LE32_TO_HOST(MBXPkt->tHeader.ulState));
   }
}

/* BOOLEAN ProfiBus::HandleNewParameterData(void)
 *
 * Purpose:
 *    To check the User Parameters received in User Parameter indication and to determine the data communication
 *    format(little,big or config definable) plus the analog input/output parameters to be processed. This function is called by
 *    ProcessMailBox() function of this class.
 *
 * Entry condition: None.
 *
 * Exit condition: Returns TRUE if the parameters are validated else returns FALSE.
 */
BOOLEAN ProfiBus::HandleNewParameterData(void) {
   /*Local Definitiones ****************************************************/

   /*Functioncode **********************************************************/
   /*New Parameter Data, Check and Acknowledge
    -first 7 Bytes (0..6) of Parameter data are handled by SPC3/VPC3+X
    -next 3 Bytes (7..9) of User Parameter data are handled and checked
    by VPC3+X (Mode2, Bit "NO_CHECK_PRM_RESERVED"), if detected
    a parameter error, this IRQ-Event doesen't occur!             */

   /*Get,- Switch Gsd-Version                                  */
   /*ERROR, Handle User Parameter                                */
   GsdVersion = GSD_VERSION_UNDEFINED;
   DataFormat = UP_DATA_FORMAT_UNDEF;
   InPZDSVSelectUserPara.In = HS_NO_SELECTION_ALL;
   OutPZDZVSelectUserPara.Out = HS_NO_SELECTION_ALL;
   UINT8 RetVal = FALSE;
   UINT32 Indx;
   GsdVersion = CurrentUserParam[4];
   switch (GsdVersion) {
   case GSD_VERSION_0:
      if ((UserParamLen == LEN_PRM_GSD_VER_0) &&
      /*User Param. 1, Reserved, VPC3+X, DP-V1           (0x00)   */
      (CurrentUserParam[0] == 0x00) &&
      /*User Param. 2, Reserved, VPC3+X, DP-V1           (0x00)   */
      (CurrentUserParam[1] == 0x00) &&
      /*User Param. 3, Reserved, VPC3+X, DP-V1           (0x00)   */
      (CurrentUserParam[2] == 0x00) &&
      /*User Param. 4, Application:                (0x00)
       Bit0=0&Bit1=0 or More then 1 of 2 Bits
       -> ERROR_WRONG_USER_PARM_DATA
       Bit0 = 1 -> Dataformat, Big Endian = Motorola     (0x01)
       Bit1 = 1 -> Dataformat, Little Endian = Intel     (0x02)   */
      ((CurrentUserParam[3] & 0xfc) == 0x00) && (((CurrentUserParam[3] & 0x03) == UP_DATA_FORMAT_BE)
            || ((CurrentUserParam[3] & 0x03) == UP_DATA_FORMAT_LE)) &&
      /*User Param. 5, Application, Gsd-Version       (0x00)   */
      (CurrentUserParam[4] == 0x00)) {
         /*OK, Handle User Parameter                           */
         DataFormat = (CurrentUserParam[3] & 0x03);

         InPZDSVSelectUserPara.In = HS_NO_SELECTION_ALL;
         OutPZDZVSelectUserPara.Out = HS_NO_SELECTION_ALL;
         /*OK                                         */
         RetVal = TRUE;
      }
      break;
   case GSD_VERSION_1:
      if ((UserParamLen == LEN_PRM_GSD_VER_1) &&
      /*User Param. 1, Reserved, VPC3+X, DP-V1           (0x00)   */
      (CurrentUserParam[0] == 0x00) &&
      /*User Param. 2, Reserved, VPC3+X, DP-V1           (0x00)   */
      (CurrentUserParam[1] == 0x00) &&
      /*User Param. 3, Reserved, VPC3+X, DP-V1           (0x00)   */
      (CurrentUserParam[2] == 0x00) &&
      /*User Param. 4, Application:                (0x00)
       Bit0=0&Bit1=0&Bit2=0 or More then 1 of 3 Bits
       -> ERROR_WRONG_USER_PARM_DATA
       Bit0 = 1 -> Dataformat, Big Endian = Motorola     (0x01)
       Bit1 = 1 -> Dataformat, Little Endian = Intel     (0x02)
       Bit2 = 1 -> Dataformat, Configuration-Defined     (0x04)   */
      ((CurrentUserParam[3] & 0xf8) == 0x00) && (((CurrentUserParam[3] & 0x07) == UP_DATA_FORMAT_BE)
            || ((CurrentUserParam[3] & 0x07) == UP_DATA_FORMAT_LE) || ((CurrentUserParam[3] & 0x07)
            == UP_DATA_FORMAT_CFG_DEF)) &&
      /*User Param. 5, Application, Gsd-Version       (0x01)   */
      (CurrentUserParam[4] == 0x01) &&
      /*User Param. 6, Application, Reserved          (0x00)   */
      (CurrentUserParam[5] == 0x00) &&
      /*User Param. 7, Application, Reserved          (0x00)   */
      (CurrentUserParam[6] == 0x00) &&
      /*User Param. 8, Application, Reserved          (0x00)   */
      (CurrentUserParam[7] == 0x00) &&
      /*User Param. 9, Application, Reserved          (0x00)   */
      (CurrentUserParam[8] == 0x00) &&
      /*User Param. 10..13, Application, PZDSV1..4       (0xXX)   */
      /*User Param. 14..17, Application, PZDZV1..4       (0xXX)   */
      /*User Param. 18, Application, Reserved            (0x00)   */
      (CurrentUserParam[17] == 0x00) &&
      /*User Param. 19, Application, Reserved            (0x00)   */
      (CurrentUserParam[18] == 0x00) &&
      /*User Param. 20, Application, Reserved            (0x00)   */
      (CurrentUserParam[19] == 0x00) &&
      /*User Param. 21, Application, Reserved            (0x00)   */
      (CurrentUserParam[20] == 0x00) &&
      /*User Param. 22, Application, Reserved            (0x00)   */
      (CurrentUserParam[21] == 0x00) &&
      /*User Param. 23, Application, Reserved            (0x00)   */
      (CurrentUserParam[22] == 0x00) &&
      /*User Param. 24, Application, Reserved            (0x00)   */
      (CurrentUserParam[23] == 0x00) &&
      /*User Param. 25, Application, Reserved            (0x00)   */
      (CurrentUserParam[24] == 0x00)) {
         /*OK, Handle User Parameter                           */
         DataFormat = (CurrentUserParam[3] & 0x07);
         for(Indx = 0; Indx < NUM_PZDV_MODULES; Indx++){
            InPZDSVSelectUserPara.AIn[Indx] = CurrentUserParam[Indx + 9];
            OutPZDZVSelectUserPara.AOut[Indx] = CurrentUserParam[Indx + 13];
//            printf("\n In%d sel=%d Out%d sel=%d", Indx,InPZDSVSelectUserPara.AIn[Indx],Indx, OutPZDZVSelectUserPara.AOut[Indx]);
         }
         /*OK                                         */
         DataformatWeb = DataFormat;
         RetVal = TRUE;
      }
      break;
   default:
      /*ERROR, Do Nothing*/
      break;
   }
   //make analog ins 0 once at new parameters
   ExternalControl.ExtAmplitudeSet = 0;
   return RetVal;
}

/* BOOLEAN ProfiBus::HandleNewConfigData(void)
 *
 *  Purpose:
 *     To check the configuration modules received in configuration indication and to build the global configuration-Array for Data-Exchange
 *     This function is called by ProcessMailBox() function of this class.
 *
 *  Entry condition:
 *     None.
 *
 *  Exit condition:
 *     Returns TRUE if the configuration modules are validated else returns FALSE.
 */
BOOLEAN ProfiBus::HandleNewConfigData(void) {
   /*Local Definitiones ****************************************************/
   UINT8 DataExSizeTemp;
   UINT8 ModuleIdxCount;
   UINT32 Config;
   UINT8 DataExCfgIdx;
   UINT8 RetVal = FALSE;
   BOOLEAN ExitFlag = FALSE;
   UINT32 Indx;
   IODataExchangeSize = 0;
   /*Functioncode **********************************************************/
   /*New Configuration Data, Check,- and Acknowledge                 */
   DataExSizeClear = DataExSize; /*Initiate Clearing  */
   DataExSizeTemp = DATA_EX_SIZE_MIN_INIT;
   DataExSize = DataExSizeTemp;
   memset((UINT8*) &DataExCfg, '\0', sizeof(DataExCfg));
   DataExCfg[0] = DATA_EX_CFG_END_MARK;
   for(Indx = 0; Indx < NUM_PZDV_MODULES; Indx++){
      ContIO.InPZDSVSelectOffset[Indx] = PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED;
      ContIO.OutPZDZVSelectOffset[Indx] = PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED;
   }
   //make analog ins 0 once at new configuration
   ExternalControl.ExtAmplitudeSet = 0;
   /*Switch Gsd-Version                                     */
   switch (GsdVersion) {
   case GSD_VERSION_0:
      if ((ConfigLen == LEN_CFG_GSD_VER_0) && (CurrentConfig[0] == CFG_BYTE_0_GSD_VER_0) && (CurrentConfig[1]
            == CFG_BYTE_1_GSD_VER_0)) {
         /*OK                                         */
         if (DataFormat == UP_DATA_FORMAT_BE) {
            DataExCfg[0] = DATA_EX_CFG_PKW_BE;
            DataExSizeTemp += 8;
            DataExCfg[1] = DATA_EX_CFG_PZD_BE;
            DataExSizeTemp += 4;
         }
         else if (DataFormat == UP_DATA_FORMAT_LE) {
            DataExCfg[0] = DATA_EX_CFG_PKW_LE;
            DataExSizeTemp += 8;
            DataExCfg[1] = DATA_EX_CFG_PZD_LE;
            DataExSizeTemp += 4;
         }
         if (DataExSizeTemp == DATA_EX_SIZE_GSD_VER_0) {
            /*OK                                      */
            DataExCfg[2] = DATA_EX_CFG_END_MARK;
            DataExSize = DataExSizeTemp;
            RetVal = TRUE;
         }
      }
      break;
   case GSD_VERSION_1:
      if (ConfigLen <= LEN_CFG_MAX_GSD_VER_1) {
         /*OK, Build Configuration-Array for Data-Exchange        */
         DataExCfgIdx = 0;
         for (ModuleIdxCount = 0; ((ModuleIdxCount < (ConfigLen >> 2)) && (ExitFlag == FALSE)); ModuleIdxCount++) /*/4  */
         {
            /*Check Array-Boundarys                         */
            if (ModuleIdxCount >= CFG_MODULE_IDX_MAX) {
               /*Error, to much Module-Indizies             */
               ExitFlag = TRUE;
            }
            else {
               /*Build DWORD from 4BYTES                       */
               Config = (UINT32) CurrentConfig[(ModuleIdxCount << 2) + 3] << 0;
               Config |= (UINT32) CurrentConfig[(ModuleIdxCount << 2) + 2] << 8;
               Config |= (UINT32) CurrentConfig[(ModuleIdxCount << 2) + 1] << 16;
               Config |= (UINT32) CurrentConfig[(ModuleIdxCount << 2) + 0] << 24;

//               pprintf
//               ("\n ucfg %08X ConfigLen %d idx %d", Config, ConfigLen, ModuleIdxCount);
               /*Handle Module-Index                           */
               switch (Config) {
               case CFG_MODULE_FREE_PLACE:
                  if ((DataFormat == UP_DATA_FORMAT_BE) || (DataFormat == UP_DATA_FORMAT_LE) || (DataFormat
                        == UP_DATA_FORMAT_CFG_DEF)) {
                     /*Do Nothing                        */
                  }
                  else {
                     /*Error, No Valid Data Format          */
                     ExitFlag = TRUE;
                  }
                  DataExSizeTemp += 0;
                  break;
               case CFG_MODULE_PKW_BE:
                  if ((DataFormat == UP_DATA_FORMAT_BE) || (DataFormat == UP_DATA_FORMAT_CFG_DEF)) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PKW_BE;
                  }
                  else if (DataFormat == UP_DATA_FORMAT_LE) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PKW_LE;
                  }
                  else {
                     /*Error, No Valid Data Format          */
                     ExitFlag = TRUE;
                  }
                  DataExSizeTemp += 8;

                  break;
               case CFG_MODULE_PKW_LE:
                  if ((DataFormat == UP_DATA_FORMAT_LE) || (DataFormat == UP_DATA_FORMAT_CFG_DEF)) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PKW_LE;
                  }
                  else if (DataFormat == UP_DATA_FORMAT_BE) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PKW_BE;
                  }
                  else {
                     /*Error, No Valid Data Format          */
                     ExitFlag = TRUE;
                  }
                  DataExSizeTemp += 8;

                  break;
               case CFG_MODULE_PZD_BE:
                  if ((DataFormat == UP_DATA_FORMAT_BE) || (DataFormat == UP_DATA_FORMAT_CFG_DEF)) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZD_BE;
                     DataformatWeb = DATA_EX_CFG_PZD_BE;
                  }
                  else if (DataFormat == UP_DATA_FORMAT_LE) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZD_LE;
                     DataformatWeb = DATA_EX_CFG_PZD_LE;
                  }
                  else {
                     DataformatWeb = 0;
//                     pprintf
//                     ("###1##");
                     /*Error, No Valid Data Format          */
                     ExitFlag = TRUE;
                  }
                  DataExSizeTemp += 4;

                  break;
               case CFG_MODULE_PZD_LE:
                  if ((DataFormat == UP_DATA_FORMAT_LE) || (DataFormat == UP_DATA_FORMAT_CFG_DEF)) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZD_LE;
                     DataformatWeb = DATA_EX_CFG_PZD_LE;
                  }
                  else if (DataFormat == UP_DATA_FORMAT_BE) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZD_BE;
                     DataformatWeb = DATA_EX_CFG_PZD_BE;
                  }
                  else {
                     DataformatWeb = 0;
                     /*Error, No Valid Data Format          */
//                     pprintf
//                     ("###2##");
                     ExitFlag = TRUE;
                  }
                  DataExSizeTemp += 4;
                  break;
               case CFG_MODULE_PZDV_1_BE:
                  if ((DataFormat == UP_DATA_FORMAT_BE) || (DataFormat == UP_DATA_FORMAT_CFG_DEF)) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDV_1_BE;
                  }
                  else if (DataFormat == UP_DATA_FORMAT_LE) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDV_1_LE;
                  }
                  else {
                     /*Error, No Valid Data Format          */
                     ExitFlag = TRUE;
                  }
                  DataExSizeTemp += 2;
                  break;
               case CFG_MODULE_PZDV_1_LE:
                  if ((DataFormat == UP_DATA_FORMAT_LE) || (DataFormat == UP_DATA_FORMAT_CFG_DEF)) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDV_1_LE;
                  }
                  else if (DataFormat == UP_DATA_FORMAT_BE) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDV_1_BE;
                  }
                  else {
                     /*Error, No Valid Data Format          */
                     ExitFlag = TRUE;
                  }
                  DataExSizeTemp += 2;
                  break;
               case CFG_MODULE_PZDV_2_BE:
                  if ((DataFormat == UP_DATA_FORMAT_BE) || (DataFormat == UP_DATA_FORMAT_CFG_DEF)) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDV_2_BE;
                  }
                  else if (DataFormat == UP_DATA_FORMAT_LE) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDV_2_LE;
                  }
                  else {
                     /*Error, No Valid Data Format          */
                     ExitFlag = TRUE;
                  }
                  DataExSizeTemp += 2;

                  break;
               case CFG_MODULE_PZDV_2_LE:
                  if ((DataFormat == UP_DATA_FORMAT_LE) || (DataFormat == UP_DATA_FORMAT_CFG_DEF)) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDV_2_LE;
                  }
                  else if (DataFormat == UP_DATA_FORMAT_BE) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDV_2_BE;
                  }
                  else {
                     /*Error, No Valid Data Format          */
                     ExitFlag = TRUE;
                  }
                  DataExSizeTemp += 2;
                  break;
               case CFG_MODULE_PZDV_3_BE:
                  if ((DataFormat == UP_DATA_FORMAT_BE) || (DataFormat == UP_DATA_FORMAT_CFG_DEF)) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDV_3_BE;
                  }
                  else if (DataFormat == UP_DATA_FORMAT_LE) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDV_3_LE;
                  }
                  else {
                     /*Error, No Valid Data Format          */
                     ExitFlag = TRUE;
                  }
                  DataExSizeTemp += 2;
                  break;
               case CFG_MODULE_PZDV_3_LE:
                  if ((DataFormat == UP_DATA_FORMAT_LE) || (DataFormat == UP_DATA_FORMAT_CFG_DEF)) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDV_3_LE;
                  }
                  else if (DataFormat == UP_DATA_FORMAT_BE) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDV_3_BE;
                  }
                  else {
                     /*Error, No Valid Data Format          */
                     ExitFlag = TRUE;
                  }
                  DataExSizeTemp += 2;
                  break;
               case CFG_MODULE_PZDV_4_BE:
                  if ((DataFormat == UP_DATA_FORMAT_BE) || (DataFormat == UP_DATA_FORMAT_CFG_DEF)) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDV_4_BE;
                  }
                  else if (DataFormat == UP_DATA_FORMAT_LE) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDV_4_LE;
                  }
                  else {
                     /*Error, No Valid Data Format          */
                     ExitFlag = TRUE;
                  }
                  DataExSizeTemp += 2;
                  break;
               case CFG_MODULE_PZDV_4_LE:
                  if ((DataFormat == UP_DATA_FORMAT_LE) || (DataFormat == UP_DATA_FORMAT_CFG_DEF)) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDV_4_LE;
                  }
                  else if (DataFormat == UP_DATA_FORMAT_BE) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDV_4_BE;
                  }
                  else {
                     /*Error, No Valid Data Format          */
                     ExitFlag = TRUE;
                  }
                  DataExSizeTemp += 2;
                  break;
               case CFG_MODULE_PZDSELV_1_BE:
                  if ((DataFormat == UP_DATA_FORMAT_BE) || (DataFormat == UP_DATA_FORMAT_CFG_DEF)) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDSELV_1_BE;
                     if ((InPZDSVSelectUserPara.AIn[0] == HS_IN_NO_SELECTION) && (ContIO.InPZDSVSelectOffset[0]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.InPZDSVSelectOffset[0] = DataExSizeTemp + 0;
                     }
                     if ((OutPZDZVSelectUserPara.AOut[0] == HS_OUT_NO_SELECTION) && (ContIO.OutPZDZVSelectOffset[0]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.OutPZDZVSelectOffset[0] = DataExSizeTemp + 1;
                     }
                  }
                  else if (DataFormat == UP_DATA_FORMAT_LE) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDSELV_1_LE;
                     if ((InPZDSVSelectUserPara.AIn[0] == HS_IN_NO_SELECTION) && (ContIO.InPZDSVSelectOffset[0]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.InPZDSVSelectOffset[0] = DataExSizeTemp + 1;
                     }
                     if ((OutPZDZVSelectUserPara.AOut[0] == HS_OUT_NO_SELECTION) && (ContIO.OutPZDZVSelectOffset[0]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.OutPZDZVSelectOffset[0] = DataExSizeTemp + 0;
                     }
                  }
                  else {
                     /*Error, No Valid Data Format          */
                     ExitFlag = TRUE;
                  }
                  DataExSizeTemp += 4;
                  break;
               case CFG_MODULE_PZDSELV_1_LE:
                  if ((DataFormat == UP_DATA_FORMAT_LE) || (DataFormat == UP_DATA_FORMAT_CFG_DEF)) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDSELV_1_LE;
                     if ((InPZDSVSelectUserPara.AIn[0] == HS_IN_NO_SELECTION) && (ContIO.InPZDSVSelectOffset[0]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.InPZDSVSelectOffset[0] = DataExSizeTemp + 1;
                     }
                     if ((OutPZDZVSelectUserPara.AOut[0] == HS_OUT_NO_SELECTION) && (ContIO.OutPZDZVSelectOffset[0]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.OutPZDZVSelectOffset[0] = DataExSizeTemp + 0;
                     }
                  }
                  else if (DataFormat == UP_DATA_FORMAT_BE) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDSELV_1_BE;
                     if ((InPZDSVSelectUserPara.AIn[0] == HS_IN_NO_SELECTION) && (ContIO.InPZDSVSelectOffset[0]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.InPZDSVSelectOffset[0] = DataExSizeTemp + 0;
                     }
                     if ((OutPZDZVSelectUserPara.AOut[0] == HS_OUT_NO_SELECTION) && (ContIO.OutPZDZVSelectOffset[0]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.OutPZDZVSelectOffset[0] = DataExSizeTemp + 1;
                     }
                  }
                  else {
                     /*Error, No Valid Data Format          */
                     ExitFlag = TRUE;
                  }
                  DataExSizeTemp += 4;
                  break;
               case CFG_MODULE_PZDSELV_2_BE:
                  if ((DataFormat == UP_DATA_FORMAT_BE) || (DataFormat == UP_DATA_FORMAT_CFG_DEF)) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDSELV_2_BE;
                     if ((InPZDSVSelectUserPara.AIn[1] == HS_IN_NO_SELECTION) && (ContIO.InPZDSVSelectOffset[1]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.InPZDSVSelectOffset[1] = DataExSizeTemp + 0;
                     }
                     if ((OutPZDZVSelectUserPara.AOut[1] == HS_OUT_NO_SELECTION) && (ContIO.OutPZDZVSelectOffset[1]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.OutPZDZVSelectOffset[1] = DataExSizeTemp + 1;
                     }
                  }
                  else if (DataFormat == UP_DATA_FORMAT_LE) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDSELV_2_LE;
                     if ((InPZDSVSelectUserPara.AIn[1] == HS_IN_NO_SELECTION) && (ContIO.InPZDSVSelectOffset[1]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.InPZDSVSelectOffset[1] = DataExSizeTemp + 1;
                     }
                     if ((OutPZDZVSelectUserPara.AOut[1] == HS_OUT_NO_SELECTION) && (ContIO.OutPZDZVSelectOffset[1]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.OutPZDZVSelectOffset[1] = DataExSizeTemp + 0;
                     }
                  }
                  else {
                     /*Error, No Valid Data Format          */
                     ExitFlag = TRUE;
                  }
                  DataExSizeTemp += 4;
                  break;
               case CFG_MODULE_PZDSELV_2_LE:
                  if ((DataFormat == UP_DATA_FORMAT_LE) || (DataFormat == UP_DATA_FORMAT_CFG_DEF)) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDSELV_2_LE;
                     if ((InPZDSVSelectUserPara.AIn[1] == HS_IN_NO_SELECTION) && (ContIO.InPZDSVSelectOffset[1]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.InPZDSVSelectOffset[1] = DataExSizeTemp + 1;
                     }
                     if ((OutPZDZVSelectUserPara.AOut[1] == HS_OUT_NO_SELECTION) && (ContIO.OutPZDZVSelectOffset[1]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.OutPZDZVSelectOffset[1] = DataExSizeTemp + 0;
                     }
                  }
                  else if (DataFormat == UP_DATA_FORMAT_BE) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDSELV_2_BE;
                     if ((InPZDSVSelectUserPara.AIn[1] == HS_IN_NO_SELECTION) && (ContIO.InPZDSVSelectOffset[1]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.InPZDSVSelectOffset[1] = DataExSizeTemp + 0;
                     }
                     if ((OutPZDZVSelectUserPara.AOut[1] == HS_OUT_NO_SELECTION) && (ContIO.OutPZDZVSelectOffset[1]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.OutPZDZVSelectOffset[1] = DataExSizeTemp + 1;
                     }
                  }
                  else {
                     /*Error, No Valid Data Format          */
                     ExitFlag = TRUE;
                  }
                  DataExSizeTemp += 4;
                  break;
               case CFG_MODULE_PZDSELV_3_BE:
                  if ((DataFormat == UP_DATA_FORMAT_BE) || (DataFormat == UP_DATA_FORMAT_CFG_DEF)) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDSELV_3_BE;
                     if ((InPZDSVSelectUserPara.AIn[2] == HS_IN_NO_SELECTION) && (ContIO.InPZDSVSelectOffset[2]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.InPZDSVSelectOffset[2] = DataExSizeTemp + 0;
                     }
                     if ((OutPZDZVSelectUserPara.AOut[2] == HS_OUT_NO_SELECTION) && (ContIO.OutPZDZVSelectOffset[2]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.OutPZDZVSelectOffset[2] = DataExSizeTemp + 1;
                     }
                  }
                  else if (DataFormat == UP_DATA_FORMAT_LE) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDSELV_3_LE;
                     if ((InPZDSVSelectUserPara.AIn[2] == HS_IN_NO_SELECTION) && (ContIO.InPZDSVSelectOffset[2]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.InPZDSVSelectOffset[2] = DataExSizeTemp + 1;
                     }
                     if ((OutPZDZVSelectUserPara.AOut[2] == HS_OUT_NO_SELECTION) && (ContIO.OutPZDZVSelectOffset[2]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.OutPZDZVSelectOffset[2] = DataExSizeTemp + 0;
                     }
                  }
                  else {
                     /*Error, No Valid Data Format          */
                     ExitFlag = TRUE;
                  }
                  DataExSizeTemp += 4;
                  break;
               case CFG_MODULE_PZDSELV_3_LE:
                  if ((DataFormat == UP_DATA_FORMAT_LE) || (DataFormat == UP_DATA_FORMAT_CFG_DEF)) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDSELV_3_LE;
                     if ((InPZDSVSelectUserPara.AIn[2] == HS_IN_NO_SELECTION) && (ContIO.InPZDSVSelectOffset[2]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.InPZDSVSelectOffset[2] = DataExSizeTemp + 1;
                     }
                     if ((OutPZDZVSelectUserPara.AOut[2] == HS_OUT_NO_SELECTION) && (ContIO.OutPZDZVSelectOffset[2]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.OutPZDZVSelectOffset[2] = DataExSizeTemp + 0;
                     }
                  }
                  else if (DataFormat == UP_DATA_FORMAT_BE) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDSELV_3_BE;
                     if ((InPZDSVSelectUserPara.AIn[2] == HS_IN_NO_SELECTION) && (ContIO.InPZDSVSelectOffset[2]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.InPZDSVSelectOffset[2] = DataExSizeTemp + 0;
                     }
                     if ((OutPZDZVSelectUserPara.AOut[2] == HS_OUT_NO_SELECTION) && (ContIO.OutPZDZVSelectOffset[2]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.OutPZDZVSelectOffset[2] = DataExSizeTemp + 1;
                     }
                  }
                  else {
                     /*Error, No Valid Data Format          */
                     ExitFlag = TRUE;
                  }
                  DataExSizeTemp += 4;

                  break;
               case CFG_MODULE_PZDSELV_4_BE:
                  if ((DataFormat == UP_DATA_FORMAT_BE) || (DataFormat == UP_DATA_FORMAT_CFG_DEF)) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDSELV_4_BE;
                     if ((InPZDSVSelectUserPara.AIn[3] == HS_IN_NO_SELECTION) && (ContIO.InPZDSVSelectOffset[3]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.InPZDSVSelectOffset[3] = DataExSizeTemp + 0;
                     }
                     if ((OutPZDZVSelectUserPara.AOut[3] == HS_OUT_NO_SELECTION) && (ContIO.OutPZDZVSelectOffset[3]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.OutPZDZVSelectOffset[3] = DataExSizeTemp + 1;
                     }
                  }
                  else if (DataFormat == UP_DATA_FORMAT_LE) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDSELV_4_LE;
                     if ((InPZDSVSelectUserPara.AIn[3] == HS_IN_NO_SELECTION) && (ContIO.InPZDSVSelectOffset[3]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.InPZDSVSelectOffset[3] = DataExSizeTemp + 1;
                     }
                     if ((OutPZDZVSelectUserPara.AOut[3] == HS_OUT_NO_SELECTION) && (ContIO.OutPZDZVSelectOffset[3]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.OutPZDZVSelectOffset[3] = DataExSizeTemp + 0;
                     }
                  }
                  else {
                     /*Error, No Valid Data Format          */
                     ExitFlag = TRUE;
                  }
                  DataExSizeTemp += 4;

                  break;
               case CFG_MODULE_PZDSELV_4_LE:
                  if ((DataFormat == UP_DATA_FORMAT_LE) || (DataFormat == UP_DATA_FORMAT_CFG_DEF)) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDSELV_4_LE;
                     if ((InPZDSVSelectUserPara.AIn[3] == HS_IN_NO_SELECTION) && (ContIO.InPZDSVSelectOffset[3]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.InPZDSVSelectOffset[3] = DataExSizeTemp + 1;
                     }
                     if ((OutPZDZVSelectUserPara.AOut[3] == HS_OUT_NO_SELECTION) && (ContIO.OutPZDZVSelectOffset[3]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.OutPZDZVSelectOffset[3] = DataExSizeTemp + 0;
                     }
                  }
                  else if (DataFormat == UP_DATA_FORMAT_BE) {
                     DataExCfg[DataExCfgIdx++] = DATA_EX_CFG_PZDSELV_4_BE;
                     if ((InPZDSVSelectUserPara.AIn[3] == HS_IN_NO_SELECTION) && (ContIO.InPZDSVSelectOffset[3]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.InPZDSVSelectOffset[3] = DataExSizeTemp + 0;
                     }
                     if ((OutPZDZVSelectUserPara.AOut[3] == HS_OUT_NO_SELECTION) && (ContIO.OutPZDZVSelectOffset[3]
                           == PZD_VAL_SEL_OFFSET_NOT_MODULE_DEFINED)) {
                        ContIO.OutPZDZVSelectOffset[3] = DataExSizeTemp + 1;
                     }
                  }
                  else {
                     /*Error, No Valid Data Format          */
                     ExitFlag = TRUE;
                  }
                  DataExSizeTemp += 4;
                  break;
               default:
                  /*Error                                */
                  ExitFlag = TRUE;
                  break;

               }
            }
         }
//         printf(" Exit flag %d DataFormat %d DataExSizeTemp %d", ExitFlag, DataExSizeTemp, DataformatWeb);
         if ((DataExSizeTemp <= DATA_EX_SIZE_MAX_GSD_VER_1) && (ExitFlag == FALSE)) {
            /*OK                                      */
            DataExCfg[DataExCfgIdx] = DATA_EX_CFG_END_MARK;
            DataExSize = DataExSizeTemp;
            IODataExchangeSize = DataExSize;
            RetVal = TRUE;
         }
      }
      break;
   default:
      /*ERROR, Do Nothing                                   */
      break;
   }

   return RetVal;
}

/*Purpose: To process communication channel area. This function is called by UpdateInputCyclic() function of
 * this class.
 *	Entry Condition: None.
 *	Exit Condition: None
 */
void ProfiBus::ProcessPKW(void) {
   ParaType Type = TYPE_VOID;
   UINT8 Access = ACC_READ;

   UINT16 PkwError = 0;
   ParamAccessError ValueError = VAL_ERR_NO_ERR;
   UINT32 Value = 0;
   static BOOLEAN StartDetect = TRUE;
   //TODO:If required move the code below to low priority task
   if ((AK == AK_NOAK) && (StartDetect == FALSE)) {
      StartDetect = TRUE;//send all zeros in PKW out at least once
      memset(FbusIO.PKWOut.PKWu8Out.PKWOut, 0, sizeof(FbusIO.PKWOut.PKWu8Out.PKWOut));
   }
   else if ((AK > AK_NOAK) && (StartDetect == TRUE)) {
      /*Convert AKRequ                                      */
      switch (AK) {
      case AK_READ8:
         Type = TYPE_8;
         Access = ACC_READ;
         break;
      case AK_READ16:
         Type = TYPE_16;
         Access = ACC_READ;
         break;
      case AK_READ32:
         Type = TYPE_32;
         Access = ACC_READ;
         break;
      case AK_READA8:
         Type = TYPE_A_8;
         Access = ACC_READ;
         break;
      case AK_READA16:
         Type = TYPE_A_16;
         Access = ACC_READ;
         break;
      case AK_READA32:
         Type = TYPE_A_32;
         Access = ACC_READ;
         break;
      case AK_WRITE8:
         Type = TYPE_8;
         Access = ACC_WRITE;
         break;
      case AK_WRITE16:
         Type = TYPE_16;
         Access = ACC_WRITE;
         break;
      case AK_WRITE32:
         Type = TYPE_32;
         Access = ACC_WRITE;
         break;
      case AK_WRITEA8:
         Type = TYPE_A_8;
         Access = ACC_WRITE;
         break;
      case AK_WRITEA16:
         Type = TYPE_A_16;
         Access = ACC_WRITE;
         break;
      case AK_WRITEA32:
         Type = TYPE_A_32;
         Access = ACC_WRITE;
         break;
      default:
         PkwError = VAL_ERR_INVALID_AK; /*Error, Invalid AKRequest       */
         break;
      }
      if (PkwError == VAL_ERR_NO_ERR) {
         if (Access == ACC_WRITE) {
            if ((Type == TYPE_32) || (Type == TYPE_A_32)) {
               Value = ((UINT32) FbusIO.PKWIn.PKWu16In.PWE.PWEu16In.PweHigh << 16)
                     | ((UINT32) FbusIO.PKWIn.PKWu16In.PWE.PWEu16In.PweLow);
            }
            else if ((Type == TYPE_16) || (Type == TYPE_A_16)) {
               Value |= (UINT32) FbusIO.PKWIn.PKWu16In.PWE.PWEu16In.PweLow;
            }
            else if ((Type == TYPE_8) || (Type == TYPE_A_8)) {
               Value |= (UINT32) (FbusIO.PKWIn.PKWu16In.PWE.PWEu16In.PweLow & 0x00ff);
            }
         }

         ValueError = HandleValue(PNU, Type, IND, Access, (void*) &Value);

      }

      /*Set PKW-Out*/
      if ((PkwError == 0) && (ValueError == VAL_ERR_NO_ERR)) {
         FbusIO.PKWOut.PKWu16Out.Pke = FbusIO.PKWIn.PKWu16In.Pke;
         FbusIO.PKWOut.PKWu16Out.Ind = FbusIO.PKWIn.PKWu16In.Ind;
         FbusIO.PKWOut.PKWu16Out.PWE.PWEu16Out.PweHigh = (UINT16) (Value >> 16);
         FbusIO.PKWOut.PKWu16Out.PWE.PWEu16Out.PweLow = (UINT16) (Value >> 0);

      }
      else {
         if (PkwError != VAL_ERR_INVALID_AK) {
            switch (ValueError) {
            case VAL_ERR_INVALID_VAL_NBR:
            case VAL_ERR_TYPE_MISMATCH:
            case VAL_ERR_INDEX_MISMATCH:
            case VAL_ERR_NOT_WRITEABLE:
            case VAL_ERR_NO_ACCESS_COMM_IFC_TOKEN:
            case VAL_ERR_NO_ACCESS_SEC_PROTECTED:
            case VAL_ERR_LIMIT_LOW:
            case VAL_ERR_LIMIT_HIGH:
            case VAL_ERR_NV_WRITE_FAILED_ECAT1:
            case VAL_ERR_NO_ACCESS_ECAT1:
            case VAL_ERR_MISC:
            case VAL_ERR_NOTIMPLEMENTED:
               PkwError = ValueError;
               break;
            default:
               PkwError = VAL_ERR_MISC; /*Last PKW-Error     */
               break;
            }
         }

         FbusIO.PKWOut.PKWu16Out.Pke = ((PkwError << PKW_PKE_ERR_SHIFT) & PKW_PKE_ERR_MASK)
               | ((15U << PKW_PKE_AK_SHIFT) & PKW_PKE_AK_MASK);
         FbusIO.PKWOut.PKWu16Out.Ind = 0;
         FbusIO.PKWOut.PKWu16Out.PWE.PWEu32Out = 0;
      }
      StartDetect = FALSE;
   }
}

/*Purpose: To process Analog Inputs received from Profibus. This function is called by UpdateInputCyclic() function of this class.
 *	Entry Condition: None.
 *	Exit Condition: None
 */
void  ProfiBus::ProcessAnalogInput()
{
//   static int Cntr = 0;
   SINT32 Indx = 0;
//   Cntr++;
   for (Indx = 0; Indx < NUM_PZDV_MODULES; Indx++) {
      switch (ContIO.SelIn[Indx]) {
      case HS_IN_NO_SELECTION:
         ContIO.SelAckIn[Indx] = HS_IN_NO_SELECTION;
         ContIO.ValueIn[Indx] = 0;
         break;
      case AN_IN_SEL_SET_VAL_P_CONT_AMPLITUDE:
         ExternalControl.ExtAmplitudeSet = ContIO.ValueIn[Indx];
         ContIO.SelAckIn[Indx] = AN_IN_SEL_SET_VAL_P_CONT_AMPLITUDE;
         break;
      default:
         ContIO.SelAckIn[Indx] = HS_IN_SELECTION_ERROR;
      }
//      if (Cntr >= 1000) {
//         printf("\n SelIn %d valIn %d AckSelin %d", ContIO.SelIn[Indx], ContIO.ValueIn[Indx], ContIO.SelAckIn[Indx]);
//
//      }
   }
//   if(Cntr >= 1000)
//      Cntr = 0;
}

/*Purpose: To determine Analog Output  values to be sent to Profibus. This function is called by UpdateOutputCyclic() function of this class.
 *	Entry Condition: None.
 *	Exit Condition: None
 */
void ProfiBus::ProcessAnalogOutput()
{
//   static int Cntr = 0;
    SINT32 Indx = 0;
//    Cntr++;
   for (Indx = 0; Indx < NUM_PZDV_MODULES; Indx++) {
      switch (ContIO.SelOut[Indx]) {
      case HS_OUT_NO_SELECTION:
         ContIO.SelAckOut[Indx] = HS_OUT_NO_SELECTION;
         ContIO.ValueOut[Indx] = 0;
         break;
      case AN_OUT_SEL_SET_VAL_S_CONT_AMPLITUDE:
         ContIO.ValueOut[Indx] = ExternalControl.AmplitudeSet;
         ContIO.SelAckOut[Indx] = AN_OUT_SEL_SET_VAL_S_CONT_AMPLITUDE;
         break;
      case AN_OUT_SEL_AMPLITUDE:
         ContIO.ValueOut[Indx] = ExternalControl.Amplitude;
         ContIO.SelAckOut[Indx] = AN_OUT_SEL_AMPLITUDE;
         break;
      case AN_OUT_SEL_CURRENT:
         ContIO.ValueOut[Indx] = ExternalControl.Current;
         ContIO.SelAckOut[Indx] = AN_OUT_SEL_CURRENT;
         break;
      case AN_OUT_SEL_POWER:
         ContIO.ValueOut[Indx] = ExternalControl.Power;
         ContIO.SelAckOut[Indx] = AN_OUT_SEL_POWER;
         break;
      case AN_OUT_SEL_PHASE:
         ContIO.ValueOut[Indx] = ExternalControl.Phase;
         ContIO.SelAckOut[Indx] = AN_OUT_SEL_PHASE;
         break;
      case AN_OUT_SEL_PSV:
         ContIO.ValueOut[Indx] = ExternalControl.PSV;
         ContIO.SelAckOut[Indx] = AN_OUT_SEL_PSV;
         break;
      case AN_OUT_SEL_FREQUENCY:
         ContIO.ValueOut[Indx] = ExternalControl.Frequency;
         ContIO.SelAckOut[Indx] = AN_OUT_SEL_FREQUENCY;
         break;
      case AN_OUT_SEL_TEMPERATURE:
         ContIO.ValueOut[Indx] = ExternalControl.Temperature;
         ContIO.SelAckOut[Indx] = AN_OUT_SEL_TEMPERATURE;
         break;
      default:
         ContIO.ValueOut[Indx] = 0;
         ContIO.SelAckOut[Indx] = HS_OUT_SELECTION_ERROR;
      }
//      if (Cntr >= 1000) {
//         printf("\n SelOut %d valout %d AckSelOut %d", ContIO.SelOut[Indx], ContIO.ValueOut[Indx],
//               ContIO.SelAckOut[Indx]);
//      }
   }
//   if(Cntr >= 1000)
//         Cntr = 0;
}

/*  void ProfiBus::ReadExtendedStatus(CIFXHANDLE HChannel)
 *
 *   Purpose:
 *    This function is called by FiledBusTask::Run() function
 *    when Fieldbus Diagnostic page polls for the status
 *    of slave. Extended status includes the baud rate and slave address information
 *   Entry condition:
 *    HChannel:Handle to the communication channel(returned by XChannelOpen API)
 *
 *   Exit condition:
 *    None.
 */
void ProfiBus::ReadExtendedStatus(CIFXHANDLE HChannel) {
   //printf("Extended diag block size %d!\r\n", sizeof(EStat));
   if (xChannelExtendedStatusBlock(HChannel, CIFX_CMD_READ_DATA, 0, sizeof(EStat), &EStat) != CIFX_NO_ERROR){
//      pprintf("Error Reading extended diag!\r\n");
   }
}


/* ParamAccessError HandleValue(UINT16 ValNbr, ParaType Type, UINT16 ArrayIndex,UINT8 Access, void * Value)
 * Purpose:
 *    To process the command received from Profibus in ValNbr parameter.
 *    This function is called by Profibus::UpdateInputCyclic() function.
 *
 * Entry condition:
 *  ValNbr:Represents the unique number for the parameter to be accessed. Also represents special commands like
 *         GET/RELEASE access token or Move one preset to another preset
 *  Type: Data Type of the parameter to be accessed.
 *  ArrayIndex: The lower byte contains the index of array accesses by ValNbr parameter number and upper byte contains special access request like
 *          read default, min or max value.
 *  Access: Standard Access either read or write.
 *  Value: pointer points to the value of parameter after read or before write command.
 *
 * Exit condition:
 *  Returns VAL_ERR_NO_ERR if the cmd is successful else returns the ParamAccessError code;
 */
ParamAccessError HandleValue(UINT16 ValNbr, ParaType Type, UINT16 ArrayIndex, UINT8 Access, void * Value) {
   /*Local Definitiones ****************************************************/

   // pprintf("\n%s %d %d %d %d %d %d",__FUNCTION__, CalledFromCommIfc, ValNbr, Type, ArrayIndex, Access, PSpecialAccess);
   ParamAccessError ReturnError = VAL_ERR_NO_ERR;
   ParamSpecialAccess PSpecialAccess = SPEC_ACC_STANDARD;
   UINT16 SpecialArrayIndex;
   UINT8 SpecialAccessCounter;
   BOOLEAN AccessTokenStatus;
   UINT32 Index = 0;

   /*Handle Special Access                                     */
   SpecialArrayIndex = ArrayIndex;
   /*Special-Access in "ArrayIndex"                        */
   SpecialAccessCounter = 0;
   if (Access == ACC_READ) {
      if (SpecialArrayIndex & SPEC_ACC_IDX_MASK_READ_DEFAULT) {
         SpecialArrayIndex &= ~SPEC_ACC_IDX_MASK_READ_DEFAULT;
         PSpecialAccess = SPEC_ACC_READ_DEFAULT;
         SpecialAccessCounter++;
      }
      if (SpecialArrayIndex & SPEC_ACC_IDX_MASK_READ_LIMIT_LOW) {
         SpecialArrayIndex &= ~SPEC_ACC_IDX_MASK_READ_LIMIT_LOW;
         PSpecialAccess = SPEC_ACC_READ_LIMIT_LOW;
         SpecialAccessCounter++;
      }
      if (SpecialArrayIndex & SPEC_ACC_IDX_MASK_READ_LIMIT_HIGH) {
         SpecialArrayIndex &= ~SPEC_ACC_IDX_MASK_READ_LIMIT_HIGH;
         PSpecialAccess = SPEC_ACC_READ_LIMIT_HIGH;
         SpecialAccessCounter++;
      }
   }
   else if (Access == ACC_WRITE) {
      if (SpecialArrayIndex & SPEC_ACC_IDX_MASK_WRITE_VOLATILE) {
         SpecialArrayIndex &= ~SPEC_ACC_IDX_MASK_WRITE_VOLATILE;
         PSpecialAccess = SPEC_ACC_WRITE_VOLATILE;
         SpecialAccessCounter++;
      }
   }
   else {
      /*Undefined, Error                                    */
      ReturnError = VAL_ERR_MISC;
   }
   if (SpecialAccessCounter == 0) {
      /*No Special-Access in "ArrayIndex" -> "SPEC_ACC_STANDARD  */
      PSpecialAccess = SPEC_ACC_STANDARD;
   }
   else if (SpecialAccessCounter > 1) {
      /*May, Special-Access Error, Handled later as
       "VAL_ERR_SPEC_ACCESS_MISMATCH"                      */
      PSpecialAccess = SPEC_ACC_ERROR;
   }

   /*Currently Acc Token with interface or not*/
   if (CommIfcAccessToken == COMM_IFC_FBB) {
      AccessTokenStatus = 1;
   }
   else {
      AccessTokenStatus = 0;
   }

   /*Switch ValNbr, Use "JUMP_TABLE" optimized for speed!               */
   switch (ValNbr) {
   /*Access Token                                        */
   case OS_ACCESS_TOKEN + 0:
   case OS_PARA_SET + 0:
   case OS_SS_PARA_SET + 0: {
      /*CMD(Access Token) -> Get Access Token                */
      /*Check for Value-Errors                              */
      if ((Type != TYPE_8)) {
         ReturnError = VAL_ERR_TYPE_MISMATCH;
      }
      else if (ArrayIndex != 0) {
         ReturnError = VAL_ERR_INDEX_MISMATCH;
      }
      else if (Access == ACC_READ) {
         /*Do Command, Read Value*/
         if (GetAccessToken(COMM_IFC_FBB))
            *((UINT32*) Value) = 0;
         else
            *((UINT32*) Value) = 1;

         ReturnError = VAL_ERR_NO_ERR;
      }
      else if (Access == ACC_WRITE) {
         ReturnError = VAL_ERR_NOT_WRITEABLE;
      }
      else {
         /*Undefined, Error                                 */
         ReturnError = VAL_ERR_MISC;
      }
   }
      break;
      /*END, CMD(Access Token) -> Get Access Token             */
   case OS_ACCESS_TOKEN + 1:
   case OS_PARA_SET + 1:
   case OS_SS_PARA_SET + 1: {
      /*CMD(Access Token) -> Put Access Token                     */

      /*Check for Value-Errors                              */
      if ((Type != TYPE_8)) {
         ReturnError = VAL_ERR_TYPE_MISMATCH;
      }
      else if (ArrayIndex != 0) {
         ReturnError = VAL_ERR_INDEX_MISMATCH;
      }
      else if (Access == ACC_READ) {
         /*Read Value, always 0                             */
         *((UINT32*) Value) = 0;
         ReturnError = VAL_ERR_NO_ERR;
      }
      else if (Access == ACC_WRITE) {
         /*Check Limits, must be always 0                   */
         if ((*((UINT32*) Value)) != 0) {
            ReturnError = VAL_ERR_LIMIT_HIGH;
         }
         else if (ReleaseAccessToken(COMM_IFC_FBB) == FALSE)
            ReturnError = VAL_ERR_NO_ACCESS_COMM_IFC_TOKEN;
         else
            ReturnError = VAL_ERR_NO_ERR;
      }
      else {
         /*Undefined, Error                                 */
         ReturnError = VAL_ERR_MISC;
      }
   }
      break;
   case OS_PARA_SET + 2://CMD: Copy Defaultvalues to PS
      if (Type != TYPE_A_8)
         ReturnError = VAL_ERR_TYPE_MISMATCH;
      else if (SpecialArrayIndex > MAXIMUM_USER_PRESETS)
         ReturnError = VAL_ERR_INDEX_MISMATCH;
      else if (Access == ACC_READ) {
         ArrayIndex = (ArrayIndex & 0x00FF);
         if (PSpecialAccess == SPEC_ACC_STANDARD) {
            if (ArrayIndex == MAXIMUM_USER_PRESETS)
               *((UINT32*) Value) = CommPS.WcParam.WeldMode;
            else
               *((UINT32*) Value) = ParameterSet[ArrayIndex].WcParam.WeldMode;
         }
         else {
            /*Set Pointer to Default,- Limits for Reading                  */
            switch (PSpecialAccess) {
            case SPEC_ACC_READ_DEFAULT:
               *((UINT32*) Value) = ParameterSet[Preset_Default].WcParam.WeldMode;
               break;
            case SPEC_ACC_READ_LIMIT_LOW:
               *((UINT32*) Value) = ParameterSet[Preset_MIN].WcParam.WeldMode;
               break;
            case SPEC_ACC_READ_LIMIT_HIGH:
               *((UINT32*) Value) = ParameterSet[Preset_MAX].WcParam.WeldMode;
               break;
            default:
               /*Error                                         */
               ReturnError = VAL_ERR_SPEC_ACCESS_MISMATCH;
            }
         }
      }
      else if (Access == ACC_WRITE) {
         if (AccessTokenStatus == FALSE)
            ReturnError = VAL_ERR_NO_ACCESS_COMM_IFC_TOKEN;
         else if (*((UINT32*) Value) < (UINT32)ParameterSet[Preset_MIN].WcParam.WeldMode)
            ReturnError = VAL_ERR_LIMIT_LOW;
         else if (*((UINT32*) Value) > (UINT32)ParameterSet[Preset_MAX].WcParam.WeldMode)
            ReturnError = VAL_ERR_LIMIT_HIGH;
         else {
            ArrayIndex = (ArrayIndex & 0x00FF);
            if (ArrayIndex == MAXIMUM_USER_PRESETS) {
               if (PSpecialAccess != SPEC_ACC_WRITE_VOLATILE)
                  ReturnError = VAL_ERR_SPEC_ACCESS_MISMATCH;
               else
                  CommPS = ParameterSet[Preset_Default];
            }
            else {
               if ((PSpecialAccess != SPEC_ACC_STANDARD) && (PSpecialAccess != SPEC_ACC_WRITE_VOLATILE))
                  ReturnError = VAL_ERR_SPEC_ACCESS_MISMATCH;
               else{
                  ParameterSet[ArrayIndex] = ParameterSet[Preset_Default];
                  if (ArrayIndex == CurrPreset)
                       RegUpdateCmdFlag = TRUE;
               }
            }
         }
      }
      else
         ReturnError = VAL_ERR_MISC;
      break;
   case OS_PARA_SET + 4://CMD: Copy PS to Comm.-PS
          if(Type != TYPE_A_8)
             ReturnError = VAL_ERR_TYPE_MISMATCH;
          else if(ArrayIndex >= MAXIMUM_USER_PRESETS)
             ReturnError = VAL_ERR_INDEX_MISMATCH;
          else if(Access == ACC_READ)
             /*Read Value, always 0                             */
             *((UINT32*) Value) = 0;
          else if(Access == ACC_WRITE){
             ArrayIndex = (ArrayIndex & 0x00FF);
             if(AccessTokenStatus == FALSE)
                ReturnError = VAL_ERR_NO_ACCESS_COMM_IFC_TOKEN;
             else if(*((UINT32*) Value) != 0)
                ReturnError = VAL_ERR_LIMIT_HIGH;
             else
                CommPS = ParameterSet[ArrayIndex];
          }
          else
              ReturnError = VAL_ERR_MISC;
      break;
   case OS_PARA_SET + 5://CMD: Copy Comm.-PS to PS
      if (Type != TYPE_A_8)
         ReturnError = VAL_ERR_TYPE_MISMATCH;
      else if (Access == ACC_READ) {
         if (ArrayIndex >= MAXIMUM_USER_PRESETS)
            ReturnError = VAL_ERR_INDEX_MISMATCH;
         else
            *((UINT32*) Value) = 0;
      }
      else if (Access == ACC_WRITE) {
         if (AccessTokenStatus == 0)
            ReturnError = VAL_ERR_NO_ACCESS_COMM_IFC_TOKEN;
         else if (SpecialArrayIndex >= MAXIMUM_USER_PRESETS)
            ReturnError = VAL_ERR_INDEX_MISMATCH;
         else if ((*((UINT32*) Value)) != 0)
            ReturnError = VAL_ERR_LIMIT_HIGH;
         else {
            ArrayIndex = ArrayIndex & 0xFF;
            if ((PSpecialAccess != SPEC_ACC_STANDARD) && (PSpecialAccess != SPEC_ACC_WRITE_VOLATILE))
               ReturnError = VAL_ERR_SPEC_ACCESS_MISMATCH;
            else{
               ParameterSet[ArrayIndex] = CommPS;
               if (ArrayIndex == CurrPreset)
                   RegUpdateCmdFlag = TRUE;
            }
         }
      }
      else
         ReturnError = VAL_ERR_MISC;
      break;
   case OS_STATUS_SET + 4://CMD: Copy SS to Comm.-SS
      if (Type != TYPE_A_8)
         ReturnError = VAL_ERR_TYPE_MISMATCH;
      else if (ArrayIndex >= MAXIMUM_USER_PRESETS) /*excl. 1 "Communication-SS"*/
         ReturnError = VAL_ERR_INDEX_MISMATCH;
      else if (Access == ACC_READ) {
         *((UINT32*) Value) = 0;
         ArrayIndex = (ArrayIndex & 0x00FF);
         CommWeldSS = WeldStatusSet[ArrayIndex];
         LastStatusSetNumber = (UINT8)ArrayIndex;
      }
      else if (Access == ACC_WRITE)
         ReturnError = VAL_ERR_NOT_WRITEABLE;
      else
         ReturnError = VAL_ERR_MISC;
      break;
   case OS_STATUS_SET + 6://CMD: Copy Last SS to Comm.-SS
       if (Type != TYPE_8)
         ReturnError = VAL_ERR_TYPE_MISMATCH;
      else if (ArrayIndex != 0)
         ReturnError = VAL_ERR_INDEX_MISMATCH;
      if (Access == ACC_READ) {
         if (LastSSCopied == TRUE)
            LastSSStatus = LAST_OK;
         else if (StatusSetNumber < MAXIMUM_USER_PRESETS) {
            if (SSOverRunIndication > 0)
               LastSSStatus = LAST_OK_NEW_OLD_OVERRUN;
            else
               LastSSStatus = LAST_OK_NEW;
            CommWeldSS = WeldStatusSet[StatusSetNumber];
            //Update for 1207
            LastStatusSetNumber = StatusSetNumber;
            LastSSCopied = TRUE;
         }
         else
            LastSSStatus = LAST_NOT_AVAILABLE;

         //Update for 1209
         LastSSOverRunIndication = SSOverRunIndication;
         //Update 1206 reply
         *((UINT32*) Value) = (UINT32) LastSSStatus;
      }
      else if (Access == ACC_WRITE)
         ReturnError = VAL_ERR_NOT_WRITEABLE;
      else
         ReturnError = VAL_ERR_MISC;
      break;
   case OS_STATUS_SET + 7://SS Number in Comm.-SS
      if (Type != TYPE_8)
         ReturnError = VAL_ERR_TYPE_MISMATCH;
      else if (ArrayIndex != 0)
         ReturnError = VAL_ERR_INDEX_MISMATCH;
      else if (Access == ACC_READ)
         *((UINT32*) Value) = (UINT32)LastStatusSetNumber;
      else if (Access == ACC_WRITE)
         ReturnError = VAL_ERR_NOT_WRITEABLE;
      else
         ReturnError = VAL_ERR_MISC;
      break;
   case OS_STATUS_SET + 9://SS Overrun Indication
       if (Type != TYPE_32)
         ReturnError = VAL_ERR_TYPE_MISMATCH;
      else if (ArrayIndex != 0)
         ReturnError = VAL_ERR_INDEX_MISMATCH;
      if (Access == ACC_READ)
         *((UINT32*) Value) = (UINT32)LastSSOverRunIndication;
      else if (Access == ACC_WRITE)
         ReturnError = VAL_ERR_NOT_WRITEABLE;
      else
         ReturnError = VAL_ERR_MISC;
      break;
   case OS_SS_PARA_SET + 2://CMD: Copy Defaultvalues to SSPS
      if (Type != TYPE_A_8)
         ReturnError = VAL_ERR_TYPE_MISMATCH;
      else if (Access == ACC_READ) {
         if (ArrayIndex > MAXIMUM_USER_HSPRESETS)
            ReturnError = VAL_ERR_INDEX_MISMATCH;
         else
            *((UINT32*) Value) = 0;
      }
      else if (Access == ACC_WRITE) {
         if (AccessTokenStatus == FALSE)
            ReturnError = VAL_ERR_NO_ACCESS_COMM_IFC_TOKEN;
         else if (SpecialArrayIndex > MAXIMUM_USER_HSPRESETS)
            ReturnError = VAL_ERR_INDEX_MISMATCH;
         else if (*((UINT32*) Value) != 0)
            ReturnError = VAL_ERR_LIMIT_HIGH;
         else {
            if (SpecialArrayIndex == MAXIMUM_USER_HSPRESETS) {
               if (PSpecialAccess != SPEC_ACC_WRITE_VOLATILE)
                  ReturnError = VAL_ERR_SPEC_ACCESS_MISMATCH;
               else
                  CommHSPS = HsParameterSet[HsPreset_Default];
            }
            else {
               if ((PSpecialAccess != SPEC_ACC_STANDARD) && (PSpecialAccess != SPEC_ACC_WRITE_VOLATILE))
                  ReturnError = VAL_ERR_SPEC_ACCESS_MISMATCH;
               else{
                  ArrayIndex = (ArrayIndex & 0x00FF);
                  HsParameterSet[ArrayIndex] = HsParameterSet[HsPreset_Default];
                  if (ArrayIndex == CurrHsPreset)
                      RegUpdateCmdFlag = TRUE;
               }
            }
         }
      }
      else
         ReturnError = VAL_ERR_MISC;
      break;
   case OS_SS_PARA_SET + 4://CMD: Copy SSPS to Comm.-SSPS
      if (Type != TYPE_A_8)
         ReturnError = VAL_ERR_TYPE_MISMATCH;
      else if (ArrayIndex >= MAXIMUM_USER_HSPRESETS)
         ReturnError = VAL_ERR_INDEX_MISMATCH;
      else if(Access == ACC_READ)
         *((UINT32*) Value) = 0;
      else if(Access == ACC_WRITE){
         if (AccessTokenStatus == FALSE)
            ReturnError = VAL_ERR_NO_ACCESS_COMM_IFC_TOKEN;
         else if (*((UINT32*) Value) != 0)
            ReturnError = VAL_ERR_LIMIT_HIGH;
         else{
            ArrayIndex = (ArrayIndex & 0x00FF);
            CommHSPS = HsParameterSet[ArrayIndex];
         }
      }
      else
         ReturnError = VAL_ERR_MISC;
      break;
   case OS_SS_PARA_SET + 5://CMD: Copy Comm.-SSPS to SSPS
      if (Type != TYPE_A_8)
         ReturnError = VAL_ERR_TYPE_MISMATCH;
      else if (Access == ACC_READ){
         if(ArrayIndex >= MAXIMUM_USER_HSPRESETS)
            ReturnError = VAL_ERR_INDEX_MISMATCH;
         else
            *((UINT32*) Value) = 0;
      }
      else if (Access == ACC_WRITE){
         if (AccessTokenStatus == FALSE)
            ReturnError = VAL_ERR_NO_ACCESS_COMM_IFC_TOKEN;
         else if (SpecialArrayIndex >= MAXIMUM_USER_HSPRESETS)
            ReturnError = VAL_ERR_INDEX_MISMATCH;
         else if (*((UINT32*) Value) != 0)
            ReturnError = VAL_ERR_LIMIT_HIGH;
         else {
            ArrayIndex = ArrayIndex & 0x00FF;
            if ((PSpecialAccess != SPEC_ACC_STANDARD) && (PSpecialAccess != SPEC_ACC_WRITE_VOLATILE))
               ReturnError = VAL_ERR_SPEC_ACCESS_MISMATCH;
            else{
               HsParameterSet[ArrayIndex] = CommHSPS;
               if (ArrayIndex == CurrHsPreset)
                    RegUpdateCmdFlag = TRUE;
            }
         }
      }
      else
         ReturnError = VAL_ERR_MISC;
      break;
   case OS_SS_STATUS_SET + 4://CMD: Copy SSSS to Comm.-SSSS
      if (Type != TYPE_A_8)
         ReturnError = VAL_ERR_TYPE_MISMATCH;
      else if (ArrayIndex >= MAXIMUM_USER_HSPRESETS)
         ReturnError = VAL_ERR_INDEX_MISMATCH;
      else if(Access == ACC_READ){
         ArrayIndex = ArrayIndex & 0x00FF;
         for(Index = 0; Index < MAX_HS_FUNC; Index++)
            CommHSSS[Index] = HsStatusSet[ArrayIndex][Index];
         LastHSStatusSetNumber = ArrayIndex;
         LastSFCT = LastSFCTInHSStatus[ArrayIndex];
         *((UINT32*) Value) = 0;
      }
      else if(Access == ACC_WRITE)
         ReturnError = VAL_ERR_NOT_WRITEABLE;
      else
         ReturnError = VAL_ERR_MISC;
      break;
   case OS_SS_STATUS_SET + 6://CMD: Copy Last SSSS to C.-SSSS
   if (Type != TYPE_8)
      ReturnError = VAL_ERR_TYPE_MISMATCH;
   else if (ArrayIndex != 0)
      ReturnError = VAL_ERR_INDEX_MISMATCH;
   if (Access == ACC_READ) {
      if (LastHSSSCopied == TRUE)
         LastHSSSStatus = LAST_OK;
      else if (HSStatusSetNumber < MAXIMUM_USER_HSPRESETS) {
         if (HSSSOverRunIndication[HSStatusSetNumber] > 0)
            LastHSSSStatus = LAST_OK_NEW_OLD_OVERRUN;
         else
            LastHSSSStatus = LAST_OK_NEW;

         //for(Index = 0; Index < MAX_HS_FUNC; Index++)
         CommHSSS[SFCT] = HsStatusSet[HSStatusSetNumber][SFCT];

         LastSFCT = LastSFCTInHSStatus[HSStatusSetNumber];
         LastHSStatusSetNumber = HSStatusSetNumber;
         LastHSSSCopied = TRUE;
      }
      else
         LastHSSSStatus = LAST_NOT_AVAILABLE;
      if (HSStatusSetNumber < MAXIMUM_USER_HSPRESETS){
    	  LastHSSSOverRunIndication[HSStatusSetNumber] = HSSSOverRunIndication[HSStatusSetNumber];
    	  *((UINT32*) Value) = (UINT32) LastHSSSStatus;
      }
      else
    	  ReturnError = VAL_ERR_INDEX_MISMATCH;
   }
   else if (Access == ACC_WRITE)
      ReturnError = VAL_ERR_NOT_WRITEABLE;
   else
      ReturnError = VAL_ERR_MISC;
      break;
   case OS_SS_STATUS_SET + 7://SSSS Number in Comm.-SSSS
   case OS_SS_STATUS_SET + 8://Last SFct Number in Comm.-SSSS
      if (Type != TYPE_8)
         ReturnError = VAL_ERR_TYPE_MISMATCH;
      else if (ArrayIndex != 0)
         ReturnError = VAL_ERR_INDEX_MISMATCH;
      else if (Access == ACC_READ) {
         if (ValNbr == (OS_SS_STATUS_SET + 7))
            *((UINT32 *) Value) = (UINT32) LastHSStatusSetNumber;
         else if (ValNbr == (OS_SS_STATUS_SET + 8))
            *((UINT32 *) Value) = (UINT32) LastSFCT;
         else
            ReturnError = VAL_ERR_MISC;
      }
      else if (Access == ACC_WRITE)
         ReturnError = VAL_ERR_NOT_WRITEABLE;
      else
         ReturnError = VAL_ERR_MISC;
      break;
   case OS_SS_STATUS_SET + 9://SSSS Overrun Indication(0-15)
      if (Type != TYPE_A_8)
         ReturnError = VAL_ERR_TYPE_MISMATCH;
      else if (ArrayIndex >= MAXIMUM_USER_HSPRESETS)
         ReturnError = VAL_ERR_INDEX_MISMATCH;
      else if (Access == ACC_READ)
         *((UINT32 *) Value) = LastHSSSOverRunIndication[ArrayIndex];
      else if (Access == ACC_WRITE)
         ReturnError = VAL_ERR_NOT_WRITEABLE;
      else
         ReturnError = VAL_ERR_MISC;
      break;
   default:
      if (Access == ACC_READ)
         ReturnError = ReadParam(Type, PSpecialAccess, Value, SpecialArrayIndex, ValNbr);

      else if (Access == ACC_WRITE)
      {
         ReturnError = WriteParam(Type, PSpecialAccess, AccessTokenStatus, Value, SpecialArrayIndex, ValNbr);
         if(ReturnError ==  VAL_ERR_NO_ERR)
         {
        	 if(ValNbr == TESTAMPSET_A_PROFI_PARAM)
        	 {
        		 HsParameterSet[CurrHsPreset].TestSelection = TEST_AMPLITUDE;
        	 }
         }
      }
      else
         ReturnError = VAL_ERR_MISC;
   }
   /*OK                                                  */
   return ReturnError;
}
