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
/*
 * DemoFunctions.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: rjamloki
 */


#include <stdint.h>
#include "cifXHWFunctions.h"
#include "cifXErrors.h"
#include <stdio.h>
#include "string.h"
#include "assert.h"
#include "RTOS.h"
#include "cifXEndianess.h"
#include "DEF_DEF.H"
//pprintf
#define printf
#define pprintf

extern UINT32 g_ulTraceLevel;
typedef struct PERFORMANCE_TEST_DATAtag {
  UINT32 ulMax;
  UINT32 ulMin;
  UINT32 ulSum;
  UINT32 ulCnt;

} PERFORMANCE_TEST_DATA, *PPERFORMANCE_TEST_DATA;

/*****************************************************************************/
/*! Displays a hex dump on the debug console (16 bytes per line)
*   \param pbData     Pointer to dump data
*   \param ulDataLen  Length of data dump                                    */
/*****************************************************************************/
void DumpData(UINT8* pbData, UINT32 ulDataLen)
{
  UINT32 ulIdx = 0;
  for(ulIdx = 0; ulIdx < ulDataLen; ++ulIdx)
  {
    if(0 == (ulIdx % 16)){
      printf("\r\n");
    }

    printf("%02X ", pbData[ulIdx]);
  }
  printf("\r\n");
}

/*****************************************************************************/
/*! Dumps a rcX packet to debug console
*   \param ptPacket Pointer to packed being dumped                           */
/*****************************************************************************/
void DumpPacket(CIFX_PACKET* ptPacket)
{
  printf("Dest   : 0x%08X      ID   : 0x%08X\r\n", (int)ptPacket->tHeader.ulDest,   (int)ptPacket->tHeader.ulId);
  printf("Src    : 0x%08X      Sta  : 0x%08X\r\n", (int)ptPacket->tHeader.ulSrc,    (int)ptPacket->tHeader.ulState);
  printf("DestID : 0x%08X      Cmd  : 0x%08X\r\n", (int)ptPacket->tHeader.ulDestId, (int)ptPacket->tHeader.ulCmd);
  printf("SrcID  : 0x%08X      Ext  : 0x%08X\r\n", (int)ptPacket->tHeader.ulSrcId,  (int)ptPacket->tHeader.ulExt);
  printf("Len    : 0x%08X      Rout : 0x%08X\r\n", (int)ptPacket->tHeader.ulLen,    (int)ptPacket->tHeader.ulRout);

  printf("Data:");
  DumpData(ptPacket->abData, ptPacket->tHeader.ulLen);
}

/*****************************************************************************/
/*! Function to demonstrate the board/channel enumeration
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
SINT32 EnumBoardDemo(void)
{
  CIFXHANDLE hDriver = NULL;
  SINT32 lRet    = xDriverOpen(&hDriver);

  printf("\r\n---------- Board/Channel enumeration demo ----------\r\n");

  if(CIFX_NO_ERROR == lRet)
  {
    /* Driver/Toolkit successfully opened */
    UINT32          ulBoard    = 0;
    BOARD_INFORMATION tBoardInfo = {0};

    /* Iterate over all boards */
    while(CIFX_NO_ERROR == xDriverEnumBoards(hDriver, ulBoard, sizeof(tBoardInfo), &tBoardInfo))
    {
      UINT32            ulChannel    = 0;
      CHANNEL_INFORMATION tChannelInfo = {{0}};

      printf("Found Board %s\r\n", tBoardInfo.abBoardName);
      if(strlen( (char*)tBoardInfo.abBoardAlias) != 0){
        printf(" Alias        : %s\r\n", tBoardInfo.abBoardAlias);
      }

      printf(" DeviceNumber : %u\r\n", (int)tBoardInfo.tSystemInfo.ulDeviceNumber);
      printf(" SerialNumber : %u\r\n", (int)tBoardInfo.tSystemInfo.ulSerialNumber);
      printf(" Board ID     : %u\r\n", (int)tBoardInfo.ulBoardID);
      printf(" System Error : 0x%08X\r\n", (int)tBoardInfo.ulSystemError);
      printf(" Channels     : %u\r\n", (int)tBoardInfo.ulChannelCnt);
      printf(" DPM Size     : %u\r\n", (int)tBoardInfo.ulDpmTotalSize);

      /* iterate over all channels on the current board */
      while(CIFX_NO_ERROR == xDriverEnumChannels(hDriver, ulBoard, ulChannel, sizeof(tChannelInfo), &tChannelInfo))
      {
        printf(" - Channel %u:\r\n", (int)ulChannel);
        printf("    Firmware : %s\r\n", tChannelInfo.abFWName);
        printf("    Version  : %u.%u.%u build %u\r\n",
               tChannelInfo.usFWMajor,
               tChannelInfo.usFWMinor,
               tChannelInfo.usFWRevision,
               tChannelInfo.usFWBuild);
        printf("    Date     : %02u/%02u/%04u\r\n",
               tChannelInfo.bFWMonth,
               tChannelInfo.bFWDay,
               tChannelInfo.usFWYear);

        ++ulChannel;
      }

      ++ulBoard;
    }

    /* close previously opened driver */
    xDriverClose(hDriver);
  }

  printf(" State = 0x%08X\r\n", (int)lRet);
  printf("----------------------------------------------------\r\n");

  return lRet;
}

/*****************************************************************************/
/*! Function to demonstrate system channel functionality (PacketTransfer)
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
SINT32 SysdeviceDemo()
{
  CIFXHANDLE hDriver = NULL;
  SINT32    lRet    = xDriverOpen(&hDriver);

  printf("\r\n---------- System Device handling demo ----------\r\n");

  if(CIFX_NO_ERROR == lRet)
  {
    /* Driver/Toolkit successfully opened */
    CIFXHANDLE hSys = NULL;
    lRet = xSysdeviceOpen(hDriver, "cifX0", &hSys);

    if(CIFX_NO_ERROR != lRet)
    {
      printf("Error opening SystemDevice!\r\n");

    } else
    {
      SYSTEM_CHANNEL_SYSTEM_INFO_BLOCK tSysInfo = {{0}};
      UINT32 ulSendPktCount = 0;
      UINT32 ulRecvPktCount = 0;
      CIFX_PACKET tSendPkt = {{0}};
      CIFX_PACKET tRecvPkt = {{0}};

      /* System channel successfully opened, try to read the System Info Block */
      if( CIFX_NO_ERROR != (lRet = xSysdeviceInfo(hSys, CIFX_INFO_CMD_SYSTEM_INFO_BLOCK, sizeof(tSysInfo), &tSysInfo)))
      {
        printf("Error querying system information block\r\n");
      } else
      {
        printf("System Channel Info Block:\r\n");
        printf("DPM Size         : %u\r\n", (int)tSysInfo.ulDpmTotalSize);
        printf("Device Number    : %u\r\n", (int)tSysInfo.ulDeviceNumber);
        printf("Serial Number    : %u\r\n", (int)tSysInfo.ulSerialNumber);
        printf("Manufacturer     : %u\r\n", (int)tSysInfo.usManufacturer);
        printf("Production Date  : %u\r\n", (int)tSysInfo.usProductionDate);
        printf("Device Class     : %u\r\n", (int)tSysInfo.usDeviceClass);
        printf("HW Revision      : %u\r\n", (int)tSysInfo.bHwRevision);
        printf("HW Compatibility : %u\r\n", (int)tSysInfo.bHwCompatibility);
        printf("\n Available HW assembly : \r\n");
        for(int i = 0; i < 4;i++){
        	printf("\n %04X", (int)tSysInfo.ausHwOptions[i]);
        }
      }

      /* Do a simple Packet exchange via system channel */
      xSysdeviceGetMBXState(hSys, &ulRecvPktCount, &ulSendPktCount);
      printf("System Mailbox State: MaxSend = %u, Pending Receive = %u\r\n",
             (int)ulSendPktCount, (int)ulRecvPktCount);

      if(CIFX_NO_ERROR != (lRet = xSysdevicePutPacket(hSys, &tSendPkt, 10)))
      {
        printf("Error sending packet to device!\r\n");
      } else
      {
        printf("Send Packet:\r\n");
        DumpPacket(&tSendPkt);

        xSysdeviceGetMBXState(hSys, &ulRecvPktCount, &ulSendPktCount);
        printf("System Mailbox State: MaxSend = %u, Pending Receive = %u\r\n",
              (int)ulSendPktCount, (int)ulRecvPktCount);

        if(CIFX_NO_ERROR != (lRet = xSysdeviceGetPacket(hSys, sizeof(tRecvPkt), &tRecvPkt, 20)) )
        {
          printf("Error getting packet from device!\r\n");
        } else
        {
          printf("Received Packet:\r\n");
          DumpPacket(&tRecvPkt);

          xSysdeviceGetMBXState(hSys, &ulRecvPktCount, &ulSendPktCount);
          printf("System Mailbox State: MaxSend = %u, Pending Receive = %u\r\n",
               (int)ulSendPktCount, (int)ulRecvPktCount);
        }
      }
      xSysdeviceClose(hSys);
    }

    xDriverClose(hDriver);
  }

  printf(" State = 0x%08X\r\n", (int)lRet);
  printf("----------------------------------------------------\r\n");

  return lRet;
}



/*****************************************************************************/
/*! Function to demonstrate system channel functionality (PacketTransfer)
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
SINT32 SysStatusBlockDemo()
{
  CIFXHANDLE hDriver = NULL;
  SINT32    lRet    = xDriverOpen(&hDriver);

  printf("\r\n---------- System Device handling demo ----------\r\n");

  if(CIFX_NO_ERROR == lRet)
  {
    /* Driver/Toolkit successfully opened */
    CIFXHANDLE hSys = NULL;
    lRet = xSysdeviceOpen(hDriver, "cifX0", &hSys);

    if(CIFX_NO_ERROR != lRet)
    {
      printf("Error opening SystemDevice!\r\n");

    } else
    {
    	SYSTEM_CHANNEL_SYSTEM_STATUS_BLOCK tSysStatusBlockInfo;// = {{0}};
    	memset(&tSysStatusBlockInfo, 0, sizeof(SYSTEM_CHANNEL_SYSTEM_STATUS_BLOCK));
      /* System channel successfully opened, try to read the System Info Block */
      if( CIFX_NO_ERROR != (lRet = xSysdeviceInfo(hSys, CIFX_INFO_CMD_SYSTEM_STATUS_BLOCK, sizeof(tSysStatusBlockInfo), &tSysStatusBlockInfo)))
      {
        printf("Error querying system information block\r\n");
      } else
      {
        printf("System Status Info Block:\r\n");
        printf("ulSystemCO0s         : %08X\r\n", tSysStatusBlockInfo.ulSystemCOS);
        printf("CpuLoad         : %04X\r\n", tSysStatusBlockInfo.usCpuLoad);

      }
       xSysdeviceClose(hSys);
    }
    xDriverClose(hDriver);
  }

  printf(" State = 0x%08X\r\n", (int)lRet);
  printf("----------------------------------------------------\r\n");

  return lRet;
}

/*****************************************************************************/
/*! Function to demonstrate communication channel functionality
*   Packet Transfer and I/O Data exchange
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
SINT32 ChannelDemo()
{
  CIFXHANDLE  hDriver = NULL;
  SINT32     lRet    = xDriverOpen(&hDriver);

  printf("\r\n---------- Communication Channel demo ----------\r\n");

  if(CIFX_NO_ERROR == lRet)
  {
    /* Driver/Toolkit successfully opened */
    CIFXHANDLE hChannel = NULL;
    lRet = xChannelOpen(NULL, "cifX0", 0, &hChannel);

    if(CIFX_NO_ERROR != lRet)
    {
      printf("Error opening Channel!");

    } else
    {
      CHANNEL_INFORMATION tChannelInfo = {{0}};
      UINT32 ulCycle = 0;
      UINT8 abSendData[32] = {0};
      UINT8 abRecvData[32] = {0};
//      /*
//       * typedef __CIFx_PACKED_PRE struct CIFX_PACKET_HEADERtag
//{
//  UINT32   ulDest;   /*!< destination of packet, process queue */
//  UINT32   ulSrc;    /*!< source of packet, process queue */
//  UINT32   ulDestId; /*!< destination reference of packet */
//  UINT32   ulSrcId;  /*!< source reference of packet */
//  UINT32   ulLen;    /*!< length of packet data without header */
//  UINT32   ulId;     /*!< identification handle of sender */
//  UINT32   ulState;  /*!< status code of operation */
//  UINT32   ulCmd;    /*!< packet command defined in TLR_Commands.h */
//  UINT32   ulExt;    /*!< extension */
//  UINT32   ulRout;   /*!< router */
//} __CIFx_PACKED_POST CIFX_PACKET_HEADER;

      CIFX_PACKET tSendPkt = {{0}};
      CIFX_PACKET tRecvPkt = {{0}};

      /*
       * typedef __CIFx_PACKED_PRE struct CIFX_PACKETtag{
		  CIFX_PACKET_HEADER  tHeader;
		  UINT8             abData[CIFX_MAX_DATA_SIZE];
		} __CIFx_PACKED_POST CIFX_PACKET;
       */
//      tSendPkt.tHeader.ulDest = 0x20;
//      tSendPkt.tHeader.ulSrc = 0;
//      tSendPkt.tHeader.ulDestId = 0;
//      tSendPkt.tHeader.ulSrcId = 0;
//      tSendPkt.tHeader.ulLen = 0x10;
//      tSendPkt.tHeader.ulId = 0x02;
//      tSendPkt.tHeader.ulState = 0;
//      tSendPkt.tHeader.ulCmd = 0x2814;
//      tSendPkt.tHeader.ulExt = 0;
//      tSendPkt.tHeader.ulRout = 0;

//      UINT8 dataarr[] = {0x02, 0x00, 0x00, 0x00, 0x18, 0x10, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00};

      /* Channel successfully opened, so query basic information */
      if( CIFX_NO_ERROR != (lRet = xChannelInfo(hChannel, sizeof(CHANNEL_INFORMATION), &tChannelInfo)))
      {
        printf("Error querying system information block\r\n");
      } else
      {
        printf("Communication Channel Info:\r\n");
        printf("Device Number    : %u\r\n", (int)tChannelInfo.ulDeviceNumber);
        printf("Serial Number    : %u\r\n", (int)tChannelInfo.ulSerialNumber);
        printf("Firmware         : %s\r\n", tChannelInfo.abFWName);
        printf("FW Version       : %u.%u.%u build %u\r\n",
                tChannelInfo.usFWMajor,
                tChannelInfo.usFWMinor,
                tChannelInfo.usFWRevision,
                tChannelInfo.usFWBuild);
        printf("FW Date          : %02u/%02u/%04u\r\n",
                tChannelInfo.bFWMonth,
                tChannelInfo.bFWDay,
                tChannelInfo.usFWYear);

        printf("Mailbox Size     : %u\r\n", (int)tChannelInfo.ulMailboxSize);
      }

      /* Do a basic Packet Transfer */
      if(CIFX_NO_ERROR != (lRet = xChannelPutPacket(hChannel, &tSendPkt, 10)))
      {
        printf("Error sending packet to device!\r\n");
      } else
      {
        printf("Send Packet:\r\n");
        DumpPacket(&tSendPkt);
		if(CIFX_NO_ERROR != (lRet = xChannelGetPacket(hChannel, sizeof(tRecvPkt), &tRecvPkt, 20)) )
		{
		  printf("Error getting packet from device!\r\n");
		} else
		{
		  printf("Received Packet:\r\n");
		  DumpPacket(&tRecvPkt);
		}
      }

      /* Read and write I/O data (32Bytes). Output data will be incremented each cyle */
      xChannelIOReadSendData(hChannel, 0, 0, sizeof(abRecvData), abRecvData);

      for( ulCycle = 0; true; ++ulCycle)
      {
    	 // printf("\n calling xChannelIORead");
    	  lRet = xChannelIORead(hChannel, 0, 0, sizeof(abRecvData), abRecvData, 10);
        if(CIFX_NO_ERROR != lRet)
        {
          printf("Error reading IO Data area!\r\n");
          RTOS::DelayMs(10);
          continue;
        } else
        {
          printf("IORead Data:");
          DumpData(abRecvData, sizeof(abRecvData));

          if(CIFX_NO_ERROR != (lRet = xChannelIOWrite(hChannel, 0, 0, sizeof(abSendData), abSendData, 10)))
          {
            printf("Error writing to IO Data area!\r\n");
            RTOS::DelayMs(10);
             continue;
          } else
          {
            printf("IOWrite Data:");
            DumpData(abSendData, sizeof(abSendData));

            memset(abSendData, (int)ulCycle + 1, sizeof(abSendData));
          }
        }
        RTOS::DelayMs(1);
      }

      xChannelClose(hChannel);
    }

    xDriverClose(hDriver);
  }

  printf(" State = 0x%08X\r\n", (int)lRet);
  printf("----------------------------------------------------\r\n");

  return lRet;

}


/*****************************************************************************/
/*! Function to demonstrate control/status block functionality
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
SINT32 BlockDemo( void)
{
//	return 0;
  SINT32 lRet = CIFX_NO_ERROR;
  CIFXHANDLE hDevice = NULL;
  CIFXHANDLE hDriver = NULL;
  lRet   = xDriverOpen(&hDriver);
  if(lRet == CIFX_NO_ERROR)
  {
	  printf("\n--- Read / Write Block Information ---\r\n");

	  /* Open channel */
	  lRet = xChannelOpen(NULL, "cifX0", 0, &hDevice);
	  if(lRet != CIFX_NO_ERROR)
	  {
		printf("Error opening Channel!\r\n");
	  } else
	  {
		UINT8 abBuffer[4] = {0};

		/* Read / Write control block */
		printf("Read CONTROL Block \r\n");
		memset( abBuffer, 0, sizeof(abBuffer));
		lRet = xChannelControlBlock( hDevice, CIFX_CMD_READ_DATA, 0, 4, &abBuffer[0]);

		DumpData(abBuffer, 4);

		printf("Write CONTROL Block \r\n");
		lRet = xChannelControlBlock( hDevice, CIFX_CMD_WRITE_DATA, 0, 4, &abBuffer[0]);

		if(lRet != CIFX_NO_ERROR){
			pprintf("\n error writing control block");
		}

		printf("Read COMMON Status Block \r\n");
		memset( abBuffer, 0, sizeof(abBuffer));
		lRet = xChannelCommonStatusBlock( hDevice, CIFX_CMD_READ_DATA, 0, 4, &abBuffer[0]);

		DumpData(abBuffer, 4);

		printf("Write COMMON Status Block \r\n");
		lRet = xChannelCommonStatusBlock( hDevice, CIFX_CMD_WRITE_DATA, 0, 4, &abBuffer[0]);

		/* this is expected to fail, as this block must not be written by Host */
		if(CIFX_NO_ERROR != lRet)
		  printf("Error writing to common status block. lRet = 0x%08x\r\n", (int)lRet);

		printf("Read EXTENDED Status Block \r\n");
		memset( abBuffer, 0, sizeof(abBuffer));
		lRet = xChannelExtendedStatusBlock( hDevice, CIFX_CMD_READ_DATA, 0, 4, &abBuffer[0]);
		DumpData(abBuffer, 4);

		printf("Write EXTENDED Status Block \r\n");
		lRet = xChannelExtendedStatusBlock( hDevice, CIFX_CMD_WRITE_DATA, 0, 4, &abBuffer[0]);

		/* this is expected to fail, as this block must not be written by Host */
		if(CIFX_NO_ERROR != lRet)
		  printf("Error writing to extended status block. lRet = 0x%08x\r\n", (int)lRet);

		xChannelClose(hDevice);
	  }
	  xDriverClose(hDriver);
  }

  return lRet;
}

/*****************************************************************************/
/*! Function to demonstrate communication channel functionality
*   Packet Transfer and I/O Data exchange
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
SINT32 xChannelIOPerformance (CIFXHANDLE hChannel, UINT8 bWrite, PPERFORMANCE_TEST_DATA ptPerfData, UINT8* pabData, UINT32 ulDataLen)
{

//	ASSERT(0);
//	return 0;
  SINT32    lRet    = CIFX_NO_ERROR;
  UINT32   ulDiff  = 0;
//
//  TPM2SC_CLKSx = 0b00;   /* Stop TPM counter */
//  TPM2SC_PS    = 0b000;  /* Set divisor to 1 making the clock 40ns*/
//  TPM2MOD      = 0xC350; /* Set Modulo for a count of 50000 =~ 2ms */
//  TPM2CNT      = 0x0000; /* Clear the counter */
//  TPM2SC_CLKSx = 0b01;   /* Start TPM counter with clock source BUSCLK */
//
  if (bWrite)
    lRet = xChannelIOWrite(hChannel, 0, 0, ulDataLen, pabData, 10);
  else
    lRet = xChannelIORead(hChannel, 0, 0, ulDataLen, pabData, 10);

//  ulDiff = TPM2CNT;
//  #if (CPU_FREQ_50MHZ == 1)
//    ulDiff *= 40;
//  #elif (CPU_FREQ_37MHZ == 1)
//    ulDiff *= 53;
//  #endif
//
  ptPerfData->ulMin = min(ptPerfData->ulMin, ulDiff);
  ptPerfData->ulMax = max(ptPerfData->ulMax, ulDiff);
  ptPerfData->ulSum+= ulDiff;
  ptPerfData->ulCnt++;

  return lRet;
}

/*****************************************************************************/
/*! Long term test function
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
SINT32 IOMirrorDemo(void)
{
  UINT8     abRecvData[32] = {0};
  CIFXHANDLE  hChannel       = NULL;
  CIFXHANDLE  hDriver        = NULL;
  SINT32     lRet           = CIFX_NO_ERROR;
  SINT32     ulCycle        = 0;
  int         fStop          = 0;

  if ( (CIFX_NO_ERROR == (lRet = xDriverOpen(&hDriver))) &&
       (CIFX_NO_ERROR == (lRet = xChannelOpen(NULL, "cifX0", 0, &hChannel))) )
  {
    while (1)
    {
      if(CIFX_NO_ERROR != (lRet = xChannelIORead(hChannel, 0, 0, sizeof(abRecvData), abRecvData, 10)))
      {
        pprintf("Error reading IO Data area! (lRet = 0x%08x)\r\n", lRet);
        break;
      } else
      {
        if (fStop)
        {
          pprintf("IO Data area corrupted! ulCycle = %d\r\n", ulCycle);
          DumpData(abRecvData, sizeof(abRecvData));
          break;
        }

        if(CIFX_NO_ERROR != (lRet = xChannelIOWrite(hChannel, 0, 0, sizeof(abRecvData), abRecvData, 10)))
        {
         pprintf("Error writing to IO Data area! (lRet = 0x%08x)\r\n", lRet);
          break;
        }
      }
      ulCycle++;
    }
  }

  return lRet;
}

/*****************************************************************************/
/*! Function to demonstrate communication channel functionality
*   Packet Transfer and I/O Data exchange
*   \return CIFX_NO_ERROR on success                                         */
/*****************************************************************************/
void PerformanceDemo(UINT32 ulMaxIOSize)
{
  CIFXHANDLE  hDriver     = NULL;
  SINT32     lRet        = CIFX_NO_ERROR;

  if(CIFX_NO_ERROR == (lRet = xDriverOpen(&hDriver)))
  {
    /* Driver/Toolkit successfully opened */
    CIFXHANDLE hChannel = NULL;
    lRet = xChannelOpen(NULL, "cifX0", 0, &hChannel);

    if(CIFX_NO_ERROR != lRet)
    {
      printf("Error opening Channel!");

    } else
    {
      /* Read and write I/O data (32Bytes). Output data will be incremented each cyle */
      UINT8* pabSendData = (UINT8*)OS_Memalloc(ulMaxIOSize);
      UINT8* pabRecvData = (UINT8*)OS_Memalloc(ulMaxIOSize);
      UINT32 iIOSize        = 0;

      for (iIOSize = 0; iIOSize<=ulMaxIOSize; iIOSize+=5)
      {
        UINT32              ulCycle        = 0;
        PERFORMANCE_TEST_DATA tPerfReadData  = {0};
        PERFORMANCE_TEST_DATA tPerfWriteData = {0};
        tPerfReadData.ulMin = ~0;
        tPerfWriteData.ulMin = ~0;
        if (!iIOSize) continue;

        for( ulCycle = 0; ulCycle < 100; ++ulCycle)
        {
          if( (CIFX_NO_ERROR != (lRet = xChannelIOPerformance(hChannel, 0, &tPerfReadData, pabRecvData, iIOSize))) &&
              (CIFX_DEV_NO_COM_FLAG != lRet ) )
          {
            printf("Error reading IO Data area!\r\n");
            break;
          } else
          {
            if( (CIFX_NO_ERROR != (lRet = xChannelIOPerformance(hChannel, 1, &tPerfWriteData, pabSendData, iIOSize))) &&
                (CIFX_DEV_NO_COM_FLAG != lRet ) )
            {
              printf("Error writing to IO Data area!\r\n");
              break;
            } else
            {
              memset(pabSendData, (int)ulCycle + 1, iIOSize);
            }
          }
        }
          if( (CIFX_NO_ERROR == lRet) || (CIFX_DEV_NO_COM_FLAG == lRet ) )
          {
            printf("IO Performance Test IO size=%d\r\n", iIOSize);
//            printf("IO Read Performance ulMin=%dus, ulMax=%dus, ulAvg=%dus, %d KBit/s\r\n",  tPerfReadData.ulMin/1000,
//                                                                        tPerfReadData.ulMax/1000,
//                                                                        (tPerfReadData.ulSum/tPerfReadData.ulCnt)/1000,
//                                                                        iIOSize*8000/((tPerfReadData.ulSum/tPerfReadData.ulCnt)/1000));
//            printf("IO Write Performance ulMin=%dus, ulMax=%dus, ulAvg=%dus, %d KBit/s\r\n", tPerfWriteData.ulMin/1000,
//                                                                        tPerfWriteData.ulMax/1000,
//                                                                        (tPerfWriteData.ulSum/tPerfWriteData.ulCnt)/1000,
//                                                                        iIOSize*8000/((tPerfWriteData.ulSum/tPerfWriteData.ulCnt)/1000));
            printf("--------------------------------------\r\n");
          }
      }

      xChannelClose(hChannel);
    }

    xDriverClose(hDriver);
  }

  printf(" State = 0x%08X\r\n", (int)lRet);
  printf("----------------------------------------------------\r\n");
}




//void BasicDPRAMTest()
//{
//	char test[5];
//	memset(test , 0, sizeof(test));
//	memcpy(test, (void*)HILSCHERDPRAM_BASEADDRESS, 4);
//	printf("\n test val= %02X %02X %02X %02X , %s ", test[0], test[1], test[2], test[3], test);
//}
