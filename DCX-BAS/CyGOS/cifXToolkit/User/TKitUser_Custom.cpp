/**************************************************************************************
 
   Copyright (c) Hilscher GmbH. All Rights Reserved.
 
 **************************************************************************************
 
   Filename:
    $Workfile:   TKitUser_Custom.cpp  $
   Last Modification:
    $Author:   rjamloki  $
    $Modtime:   04 Oct 2013 19:42:32  $
    $Revision:   1.4  $
   
   Targets:
     None
 
   Description:
     USER implemented functions called by the cifX Toolkit. 
       
   Changes:
 
     Version   Date        Author   Description
     ----------------------------------------------------------------------------------
      1        07.08.2006  MT       initial version
 
**************************************************************************************/

/*****************************************************************************/
/*! \file TKitUser.c                                                         *
*    USER implemented functions called by the cifX Toolkit                   */
/*****************************************************************************/

#include "cifXToolkit.h"
#include "cifXErrors.h"
#include "cifXEndianess.h"
#include "ProfibusAps_Public.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
//#error "Implement target system specifc user functions in this file"

/*****************************************************************************/
/*!  \addtogroup CIFX_TK_USER User specific implementation
*    \{                                                                      */
/*****************************************************************************/

/*****************************************************************************/
/*! Returns the number of firmware files associated with the card/channel,
*   passed as argument.
*   \param ptDevInfo DeviceInfo including channel number, for which the
*                    firmware file count should be read
*   \return number for firmware files, to download. The returned value will
*           be used as maximum value for ulIdx in calls to
*           USER_GetFirmwareFile                                             */
/*****************************************************************************/
uint32_t USER_GetFirmwareFileCount(PCIFX_DEVICE_INFORMATION ptDevInfo)
{
	ptDevInfo = 0;
	return 0;
}

/*****************************************************************************/
/*! Returns firmware file information for the given device/channel and Idx
*   passed as argument.
*   \param ptDevInfo  DeviceInfo including channel number, for which the
*                     firmware file should be delivered
*   \param ulIdx      Index of the returned file
*                     (0..USER_GetFirmwareFileCount() - 1)
*   \param ptFileInfo Short and full file name of the firmware. Used in
*                     calls to OS_OpenFile()
*   \return !=0 on success                                                   */
/*****************************************************************************/
int32_t USER_GetFirmwareFile(PCIFX_DEVICE_INFORMATION ptDevInfo, uint32_t ulIdx, PCIFX_FILE_INFORMATION ptFileInfo)
{
	ptDevInfo = 0;
	ulIdx = 0;
	ptFileInfo = 0;
	return 0;
}

/*****************************************************************************/
/*! Returns the number of configuration files associated with the card/
*   channel, passed as argument.
*   \param ptDevInfo DeviceInfo including channel number, for which the
*                    configuration file count should be read
*   \return number for confgiuration files, to download. The returned value
*           will be used as maximum value for ulIdx in calls to
*           USER_GetConfgirationFile                                         */
/*****************************************************************************/
uint32_t USER_GetConfigurationFileCount(PCIFX_DEVICE_INFORMATION ptDevInfo)
{
	ptDevInfo = 0;
	return 0;
}

/*****************************************************************************/
/*! Returns configuration file information for the given device/channel and
*   Idx passed as argument.
*   \param ptDevInfo  DeviceInfo including channel number, for which the
*                     configuration file should be delivered
*   \param ulIdx      Index of the returned file
*                     (0..USER_GetConfigurationFileCount() - 1)
*   \param ptFileInfo Short and full file name of the configuration. Used in
*                     calls to OS_OpenFile()
*   \return !=0 on success                                                   */
/*****************************************************************************/
int32_t USER_GetConfigurationFile(PCIFX_DEVICE_INFORMATION ptDevInfo, uint32_t ulIdx, PCIFX_FILE_INFORMATION ptFileInfo)
{
	ptDevInfo = 0;
	ulIdx = 0;
	ptFileInfo = 0;
	return 0;
}

/*****************************************************************************/
/*! Returns OS file information for the given device/channel and Idx
*   passed as argument. This is needed for module downloading
*   \param ptDevInfo  DeviceInfo for which the
*                     firmware file should be delivered
*   \param ptFileInfo Short and full file name of the firmware. Used in
*                     calls to OS_OpenFile()
*   \return != 0 on success                                                   */
/*****************************************************************************/
int32_t USER_GetOSFile(PCIFX_DEVICE_INFORMATION ptDevInfo, PCIFX_FILE_INFORMATION ptFileInfo)
{
	ptDevInfo = 0;
	ptFileInfo = 0;
	return 0;
}

/*****************************************************************************/
/*! Retrieve the full file name of the cifX Romloader binary image
*   \param ptDevInstance  Pointer to the device instance
*   \param ptFileInfo Short and full file name of the bootloader. Used in
*                     calls to OS_OpenFile()
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
void USER_GetBootloaderFile(PDEVICEINSTANCE ptDevInstance, PCIFX_FILE_INFORMATION ptFileInfo)
{
	ptDevInstance = 0;
	ptFileInfo = 0;
}

/*****************************************************************************/
/*! Retrieve the alias name of a cifX Board depending on the Device and
*   Serialnumber passed during this call
*   \param ptDevInfo Device- and Serial number of the card
*   \param ulMaxLen  Maximum length of alias
*   \param szAlias   Buffer to copy alias to. A string of length 0 means
*                    no alias                                                */
/*****************************************************************************/
void USER_GetAliasName(PCIFX_DEVICE_INFORMATION ptDevInfo, uint32_t ulMaxLen, char* szAlias)
{
	ptDevInfo = 0;
	ulMaxLen = 0;
	szAlias = 0;
}

/*****************************************************************************/
/*! Read the warmstart data from a given warmstart file
*   \param ptDevInfo Device- and Serial number of the card
*   \param ptPacket  Buffer for the warmstart packet                         */
/*****************************************************************************/
int32_t USER_GetWarmstartParameters(PCIFX_DEVICE_INFORMATION ptDevInfo, CIFX_PACKET* ptPacket)
{
	ptDevInfo = 0;
	ptPacket = 0;
	return 0;
//	unsigned int CfgLen = 0;
//	PROFIBUS_APS_PACKET_SET_CONFIGURATION_REQ_T* ptProfibusSetConfigReq;
//	ptProfibusSetConfigReq = (PROFIBUS_APS_PACKET_SET_CONFIGURATION_REQ_T*)ptPacket;
//	OS_Memset((void*)ptProfibusSetConfigReq, 0, sizeof(*ptProfibusSetConfigReq));
//
//	ptProfibusSetConfigReq->tHead.ulDest = HOST_TO_LE32(0x00000020);
//	ptProfibusSetConfigReq->tHead.ulSrc = HOST_TO_LE32(0);
//	ptProfibusSetConfigReq->tHead.ulCmd = HOST_TO_LE32(PROFIBUS_APS_SET_CONFIGURATION_REQ);
//	ptProfibusSetConfigReq->tData.ulSystemFlags = HOST_TO_LE32(0);
//	ptProfibusSetConfigReq->tData.ulWdgTime = HOST_TO_LE32(0);
//	ptProfibusSetConfigReq->tData.usIdentNumber = HOST_TO_LE16(0x0D0F);
//	ptProfibusSetConfigReq->tData.bBusAddr = 2;
//	ptProfibusSetConfigReq->tData.bBaudRate = 15;
//	ptProfibusSetConfigReq->tData.bFlags = 0x06;
//	ptProfibusSetConfigReq->tData.abCfgData[CfgLen++] = 0xC1;
//	ptProfibusSetConfigReq->tData.abCfgData[CfgLen++] = 0xC3;
//	ptProfibusSetConfigReq->tData.abCfgData[CfgLen++] = 0xC3;
//	ptProfibusSetConfigReq->tData.abCfgData[CfgLen++] = 0x01;
//	ptProfibusSetConfigReq->tData.abCfgData[CfgLen++] = 0xC1;
//	ptProfibusSetConfigReq->tData.abCfgData[CfgLen++] = 0xC1;
//	ptProfibusSetConfigReq->tData.abCfgData[CfgLen++] = 0xC1;
//	ptProfibusSetConfigReq->tData.abCfgData[CfgLen++] = 0x02;
//	ptProfibusSetConfigReq->tData.bCfgLen = CfgLen;
//	ConfigLen = CfgLen;
//	memcpy(CurrentConfig, ptProfibusSetConfigReq->tData.abCfgData, CfgLen);
//	ptProfibusSetConfigReq->tHead.ulLen = HOST_TO_LE32(sizeof(ptProfibusSetConfigReq->tData) - sizeof(ptProfibusSetConfigReq->tData.abCfgData) + CfgLen);

	return 1;
}



/*****************************************************************************/
/*! User trace function
*   right while cifXTKitAddDevice is being processed
*   \param ptDevInstance  Device instance
*   \param ulTraceLevel   Trace level
*   \param szFormat       Format string                                      */
/*****************************************************************************/
void USER_Trace(PDEVICEINSTANCE ptDevInstance, uint32_t ulTraceLevel, const char* szFormat, ...)
{
	ptDevInstance = 0;
	ulTraceLevel = 0;
	szFormat = 0;
}

/*****************************************************************************/
/*! Check if interrupts should be enabled for this device
*   \param ptDevInfo  Device Infotmation
*   \return !=0 to enable interrupts                                         */
/*****************************************************************************/
int32_t USER_GetInterruptEnable(PCIFX_DEVICE_INFORMATION ptDevInfo)
{
	ptDevInfo = 0;
	return 0;
}

/*****************************************************************************/
/*! \}                                                                       */
/*****************************************************************************/
