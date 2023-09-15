/**************************************************************************************

   Copyright (c) Hilscher GmbH. All Rights Reserved.

 **************************************************************************************

   Filename:
    $Workfile:   DPMInterface.cpp  $
   Last Modification:
    $Author:   ygupta  $
    $Modtime:   29 Jan 2013 11:27:30  $
    $Revision:   1.2  $

   Targets:
     MCF51CN128   : yes

   Description:
   cifX parallel DPM Interface

   Changes:

     Version   Date        Author   Description
      ----------------------------------------------------------------------------------
      1        01.03.2011  SS       initial version

**************************************************************************************/

/*****************************************************************************/
/*! \file MCF51CN128ParDPMInterface.c
*   cifX parallel DPM interface function implementation                               */
/*****************************************************************************/
//#include "cifXToolkit.h"
//#include "DPMInterface.h"
//#include "CPU.h"
//#include "assert.h"
//#include "CpuDefs.h"
//#include "string.h"
//#include <stdio.h>
////#define COMX16
//#define COMX8
//#ifdef COMX16
//	#undef COMX8
//#endif
//
//#define SWAP(a) ( (((a) & 0x00FF) << 8) | (((a) & 0xFF00) >> 8) )
//
//static uint16_t s_abHskMirrorCell[16] = { 0x00B0, /* Mirror cell of system channel */
//                                          0x00B4, /* Mirror cell of sync bits */
//                                          0x0300, /* Mirror cell of communication channel 0 */
//                                          0x0000, /* 0x4000 = Mirror cell of communication channel 1 */
//                                          0x0000, /* 0x7D00 = Mirror cell of communication channel 2 */
//                                          0x0000, /* 0xBA00 = Mirror cell of communication channel 3 */
//                                          0x0000, /* application channels are not yet supported */
//                                          0x0000, /* application channels are not yet supported */
//                                          0x0000, /* reserved */
//                                          0x0000, /* reserved */
//                                          0x0000, /* reserved */
//                                          0x0000, /* reserved */
//                                          0x0000, /* reserved */
//                                          0x0000, /* reserved */
//                                          0x0000, /* reserved */
//                                          0x0000, /* reserved */
//                                       };


#ifdef COMX16
void IntHandlerExternal(int level, void * frame)
{
	//use the toolkit provided handler
	//read handshake cells to clear interrupt
}
#endif
#ifdef NX50_PARDPM16
/*****************************************************************************/
/*! Read a number of bytes from 16Bit-Mode DPM interface
*   \param pvDst         Buffer to store read data
*   \param ulDpmAddr     Address in DPM to read data from
*   \param ulLen         Number of bytes to read                             */
/*****************************************************************************/
static void Read_DPM16(void* pvDst, uint32_t ulDpmAddr, uint32_t ulLen)
{
  uint32_t ulIdx = 0;
  
  do
  {
    if ((ulDpmAddr&0x1) || (ulIdx+1 == ulLen))
    {
      uint16_t uiTmp = *(uint16_t*)((ulDpmAddr+ulIdx)&0xFFFFFFFE);
      *((uint8_t*)pvDst+ulIdx) = ((uint8_t*)&uiTmp)[(~(ulDpmAddr+ulIdx))&0x1];
      ulIdx++;
      
    } else
    {
      uint16_t* puiDst = (uint16_t*)((uint8_t*)pvDst+ulIdx);
      *puiDst = SWAP(*(uint16_t*)(ulDpmAddr+ulIdx));
      ulIdx += 2;
    }
  } while(ulLen != ulIdx);
}

/*****************************************************************************/
/*! Write a number of bytes to 16Bit-Mode DPM interface
*   \param ulDpmAddr     Address in DPM to write data
*   \param pvSrc         Buffer storing data to write
*   \param ulLen         Number of bytes to write                            */
/*****************************************************************************/
static void Write_DPM16(uint32_t ulDpmAddr, void* pvSrc, uint32_t ulLen)
{
  uint32_t ulIdx = 0;
  
  do
  {
    if ((ulDpmAddr&0x1) || (ulIdx+1 == ulLen))
    {
      uint16_t uiTmp = *(uint16_t*)((ulDpmAddr+ulIdx)&0xFFFFFFFE);
      if ((ulDpmAddr+ulIdx)&0x1)
        uiTmp = (uiTmp&0x00FF) | (uint16_t)(*((uint8_t*)pvSrc+ulIdx))<<8;
      else
        uiTmp = (uiTmp&0xFF00) | (uint16_t)(*((uint8_t*)pvSrc+ulIdx));
      *(uint16_t*)((ulDpmAddr+ulIdx)&0xFFFFFFFE) = uiTmp;
      ulIdx++;
      
    } else
    {
      uint16_t uiTmp = *((uint16_t*)((uint8_t*)pvSrc+ulIdx));
      *(uint16_t*)((uint8_t*)(ulDpmAddr+ulIdx)) = SWAP(uiTmp);
      ulIdx += 2;
    }
  } while (ulLen != ulIdx);
}

/*****************************************************************************/
/*! Read from handshake cell area, verified with handshake cell mirror  
*   \param ulDpmAddr  Start address of DPM
*   \param pbHskAddr  Address of handshake cell to read                   
*   \param pbDst      Buffer to store handshake cell values
*   \param bLen       Bytes to read from handshake area                  */
/*****************************************************************************/
static void ReadHskArea(uint32_t ulDpmAddr, uint16_t* pusHskAddr, uint16_t* pusDst, uint8_t bLen)
{
  /* Calculate offset of handshake cell in DPM */
  uint8_t  bHskOffset = (uint8_t)((uint32_t)pusHskAddr-ulDpmAddr);
  
  if (bLen ==1)
  {
    /* Get offset of mirrored handshake cell in DPM from lookup table */
    uint16_t usHskMirrorOffset = s_abHskMirrorCell[bHskOffset>>2];
    
    /* Read handshake cell value */
    Read_DPM16(pusDst, (uint32_t)pusHskAddr, 1);
    
    if (usHskMirrorOffset)
    {
      /* Read handshake cell value from mirror */
      uint8_t bHskMirrorVal = 0;

      Read_DPM16(&bHskMirrorVal, ulDpmAddr + usHskMirrorOffset + (bHskOffset&0x3), 1);

      /* Loop until handshake cell and mirror match */
      while (*(uint8_t*)pusDst != bHskMirrorVal)
        Read_DPM16(pusDst, (uint32_t)pusHskAddr, 1);
    }
  } else
  {
    while (bLen)
    {
      /* Get offset of mirrored handshake cell in DPM from lookup table */
      uint16_t usHskMirrorOffset = s_abHskMirrorCell[bHskOffset>>2];
      
      /* Read handshake cell value */
      Read_DPM16(pusDst, (uint32_t)pusHskAddr, 2);
      
      if (usHskMirrorOffset)
      {
        /* Read handshake cell value from mirror */
        uint16_t usHskMirrorVal = 0;
  
        Read_DPM16(&usHskMirrorVal, ulDpmAddr + usHskMirrorOffset + (bHskOffset&0x3), 2);
  
        /* Loop until handshake cell and mirror match */
        while (*pusDst != usHskMirrorVal)
          Read_DPM16(pusDst, (uint32_t)pusHskAddr, 2);
      }
      pusDst++;
      bHskOffset+=2;
      pusHskAddr++;
      bLen-=2;
    }
  }
}

/*****************************************************************************/
/*! Write handshake cell area, verified by subsequent read
*   \param ulDpmAddr     Start address of DPM
*   \param pbHskCellAddr Address of handshake cell to write                   
*   \param pbValue       Buffer holding values to write to handshake area
*   \param bLen          Bytes to write to handshake area                    */
/*****************************************************************************/
static void WriteHskArea(uint32_t ulDpmAddr, uint8_t* pbHskAddr, uint8_t* pbValue, uint8_t bLen)
{
  /* Write handshake cell area */
  Write_DPM16((uint32_t)pbHskAddr, pbValue, bLen);
  
  for (;;)
  {
    char szCookie[5] = {0};
    Read_DPM16(szCookie, ulDpmAddr, 4);
    
    /* Write access to handshake cell is completed, if a subsequent read access succeeds */
    if( (0 == OS_Strcmp( szCookie, CIFX_DPMSIGNATURE_FW_STR)) ||
        (0 == OS_Strcmp( szCookie, CIFX_DPMSIGNATURE_BSL_STR)) )
      break;
  }
}
//#else
//
///*****************************************************************************/
///*! Read from handshake cell area, verified with handshake cell mirror
//*   \param ulDpmAddr  Start address of DPM
//*   \param pbHskAddr  Address of handshake cell to read
//*   \param pbDst      Buffer to store handshake cell values
//*   \param bLen       Bytes to read from handshake area                      */
///*****************************************************************************/
//static void ReadHskArea(uint32_t ulDpmAddr, uint8_t* pbHskAddr, uint8_t* pbDst, uint8_t bLen)
//{
//  /* Calculate offset of handshake cell in DPM */
//  uint8_t  bHskOffset = (uint8_t)((uint32_t)pbHskAddr-ulDpmAddr);
//
//  while (bLen--)
//  {
//    /* Get offset of mirrored handshake cell in DPM from lookup table */
//    uint16_t usHskMirrorOffset = s_abHskMirrorCell[bHskOffset>>2];
//    /* Read handshake cell value */
//    *pbDst                     = *(volatile uint8_t*)pbHskAddr;
//
//    if (usHskMirrorOffset)
//    {
//      /* Read handshake cell value from mirror */
//      uint8_t bHskMirrorVal = *(uint8_t*)(ulDpmAddr         +
//                                          usHskMirrorOffset +
//                                          (bHskOffset&0x3));
//      /* Loop until handshake cell and mirror match */
//      while (*pbDst != bHskMirrorVal)
//        *pbDst = *(volatile uint8_t*)pbHskAddr;
//    }
//    pbDst++;
//    bHskOffset++;
//    pbHskAddr++;
//  }
//}
//
///*****************************************************************************/
///*! Write handshake cell area, verified by subsequent read
//*   \param ulDpmAddr     Start address of DPM
//*   \param pbHskCellAddr Address of handshake cell to write
//*   \param pbValue       Buffer holding values to write to handshake area
//*   \param bLen          Bytes to write to handshake area                    */
///*****************************************************************************/
//static void WriteHskArea(uint32_t ulDpmAddr, uint8_t* pbHskAddr, uint8_t* pbValue, uint8_t bLen)
//{
//  while (bLen--)
//  {
//    /* Write handshake cell area */
//    *pbHskAddr++ = *pbValue++;
//    for (;;)
//    {
//      /* Write access to handshake cell is completed, if a subsequent read access succeeds */
//      if( (0 == OS_Strcmp( (char*)ulDpmAddr, CIFX_DPMSIGNATURE_FW_STR)) ||
//          (0 == OS_Strcmp( (char*)ulDpmAddr, CIFX_DPMSIGNATURE_BSL_STR)) )
//        break;
//    }
//  }
//}
//#endif /* NX50_PARDPM16 */

/*****************************************************************************/
/*! Init DPM interface
*   \return 0 on success                                                     */
/*****************************************************************************/
//int32_t DPM_Init( void)
//{
//
//	//configure chip select
//		//base address and size of memory doesn't make any difference until it
//		//is overlapping some other memory mapped device address space
//		//For now 20 wait states to be in safer side
//		//Using the 5th chip select signal out of available six(0 to 5) in MCF5329 processor.
//		//width of data is 8. Can change it to 16 if we hardwire other 8 data line (16 to 23)
//		//on COM-X host interface
//	#ifdef COMX8
//		//As the DPRAM size in COM-X module is 16K.
//		//The available addresses for DPRAM are from 0xC0000000 to 0x300003FFF
//		CPU::SetupChipSelect(5, 60, 8, (void *) 0x08000000);
//	#else
//		CPU::SetupChipSelect(5, 20, 16, MCF_FBCS_CSMR_BAM_64K, (void *) 0x08000000);
//	#endif
//
//		//configure GPIO for Busy line(H16 processor pin)
//		//Pin assignment
////		MCF_GPIO_PAR_PWM &= ~MCF_GPIO_PAR_PWM_PAR_PWM1(3);//bit 0 nd 1 set to 0 for GPIO.default is same
//		//data direction
//		//output of comx input for us
//		//configure PWM1 port pin as input
////		MCF_GPIO_PDDR_PWM &= ~MCF_GPIO_PDDR_PWM_PDDR_PWM2;
////
//		//Used for Testing the time of DPRAM read write cycle
//		//Pin assignment
////		MCF_GPIO_PAR_PWM &= ~MCF_GPIO_PAR_PWM_PAR_PWM3(3);//bit 3 and 4 cleared for GPIO.Defaultis same
//		//data direction
//		//input to COMX, output for us//set 1 for direction
//		//configure PWM3 port pin as output
////		MCF_GPIO_PDDR_PWM |= MCF_GPIO_PDDR_PWM_PDDR_PWM3;
//		//configure Edge port module for external interrupt from COM-X module. Interrupt is active low
//		//Note: for 8 bit mode Hilscher has recommended to use polling mode so for now we can leave edge port
//		//module initialization for 8 bit mode.
//
//	#ifdef COMX16
//		/*All below is alresdy done in InitExternalInterrupt function in CPU.cpp file*/
//			//clear bits for IRQ6 in pin assignment register
//			/*MCF_GPIO_PAR_IRQ &= ~MCF_GPIO_PAR_IRQ_PAR_IRQ6(3);
//			//clear bits for making IRQ6 as low level sensitive
//			MCF_EPORT_EPPAR &= ~MCF_EPORT_EPPAR_EPPA6(3);
//			//clear bit in EPDDR to select the source of interrupt as external
//			MCF_EPORT_EPDDR &= ~MCF_EPORT_EPDDR_EPDD6;
//			//enable IRQ6 interrupt
//			MCF_EPORT_EPIER |= MCF_EPORT_EPIER_EPIE6;
//			//setup handler for IRQ6. Initially disables the interrupt*/
//
//			//CPU::InitExternalInterrupt(IntHandlerExternal, intEPFR6, 4, CpuIntModeLevelLow, false);
//			//enable interrupt
//			//TODO: We may need to move it somewhere else
//			//CPU::EnableExternalInterrupt(intEPFR6);
//	#endif
//  return 0;
//}




/*****************************************************************************/
/*! Write a number of bytes to DPM interface
*   \param pvDevInstance Toolkit device instance
*   \param ulDpmAddr     Address in DPM to store data to
*   \param pvDst         Buffer holding data to store
*   \param ulLen         Number of bytes to store                            */
/*****************************************************************************/
//void* DPM_Write( void* pvDevInstance, uint32_t ulDpmAddr, void* pvSrc, uint32_t ulLen)
//{
//  PDEVICEINSTANCE ptDevInst     = (PDEVICEINSTANCE)pvDevInstance;
//  uint32_t        ulDPMOffset   = ulDpmAddr - (uint32_t)ptDevInst->pbDPM;
//
//  if (NETX_SYSTEM_CHANNEL_SIZE == (ulDPMOffset&(~(NETX_HANDSHAKE_CHANNEL_SIZE-1))))
//  {
//    WriteHskArea((uint32_t)ptDevInst->pbDPM, (uint8_t*)ulDpmAddr, (uint8_t*)pvSrc, (uint8_t)ulLen);
//
//  } else
//  {
//    #ifdef NX50_PARDPM16
//      Write_DPM16(ulDpmAddr, pvSrc, ulLen);
//    #else
//      OS_Memcpy((void*)ulDpmAddr, pvSrc, ulLen);
//    #endif /* NX50_PARDPM_16BIT */
//  }
//
//  return (void*)ulDpmAddr;
//}

/*****************************************************************************/
/*! Read a number of bytes from DPM interface
*   \param pvDevInstance Toolkit device instance
*   \param ulDpmAddr     Address in DPM to read data from
*   \param pvDst         Buffer to store read data
*   \param ulLen         Number of bytes to read                             */
/*****************************************************************************/
//void* DPM_Read( void* pvDevInstance, uint32_t ulDpmAddr, void* pvDst, uint32_t ulLen)
//{
//  PDEVICEINSTANCE ptDevInst       = (PDEVICEINSTANCE)pvDevInstance;
//  uint32_t        ulDPMOffset     = ulDpmAddr - (uint32_t)ptDevInst->pbDPM;
//
//  if (NETX_SYSTEM_CHANNEL_SIZE == (ulDPMOffset&~(NETX_HANDSHAKE_CHANNEL_SIZE-1)))
//  {
//    ReadHskArea((uint32_t)ptDevInst->pbDPM, (uint8_t*)ulDpmAddr, (uint8_t*)pvDst, (uint8_t)ulLen);
//
//  } else
//  {
//    #ifdef NX50_PARDPM16
//      Read_DPM16(pvDst, ulDpmAddr, ulLen);
//    #else
//      OS_Memcpy(pvDst, (void*)ulDpmAddr, ulLen);
//    #endif /* NX50_PARDPM16 */
//  }
  
//  return pvDst;
//}

/*@memAdrr:DPRAM pointer
 * @data: Data pointer
 */
/*
void ReadDPRam(uint32_t memAddr, uint32_t length, void * data)
{
	volatile int wait = 0;
	//can leave BUS high enable in polling mode
#ifdef COMX16
	//A0-address line 0 should be 0 for word access
	ASSERT((memAddr % 2) == 0);//make sure the start address is word align
	MCF_GPIO_PODR_PWM &= ~MCF_GPIO_PODR_PWM_PODR_PWM3;
#endif
	//check while busy, busy is active low
	//while low hold tranfer
	//TODO:Figure out whether this is needed in interrupt mode
//	for(int i = 0; i <length; i++)
//	{
//		while(!(MCF_GPIO_PPDSDR_PWM & MCF_GPIO_PPDSDR_PWM_PPDSDR_PWM2));
//		((unsigned char *)data)[i] = ((unsigned char *)(memAddr + i))[0];
//		while(!(MCF_GPIO_PPDSDR_PWM & MCF_GPIO_PPDSDR_PWM_PPDSDR_PWM2));
//		{
//			while(wait++ <10000);
//			((unsigned char *)data)[i] = ((unsigned char *)(memAddr + i))[0];
//			wait = 0;
//		}
//
//	}
 *

#ifdef COMX16
	MCF_GPIO_PODR_PWM |= MCF_GPIO_PODR_PWM_PODR_PWM3;
#endif
} */

/*
void WriteDPRam(uint32_t memAddr, uint32_t length, void * data)
{
#ifdef COMX16
	//A0-address line 0 should be 0 for word access
	ASSERT((memAddr % 2) == 0);//make sure the start address is word align
	MCF_GPIO_PODR_PWM &= ~MCF_GPIO_PODR_PWM_PODR_PWM3;
#endif
	//check while busy
	//TODO:Figure out whether this is needed in interrupt mode
//	for(int i = 0; i <length; i++)
//	{
//			while(!(MCF_GPIO_PPDSDR_PWM & MCF_GPIO_PPDSDR_PWM_PPDSDR_PWM2));
//				((unsigned char *)(memAddr + i))[0] = ((unsigned char *)data)[i];
//	}
#ifdef COMX16
	MCF_GPIO_PODR_PWM |= MCF_GPIO_PODR_PWM_PODR_PWM3;
#endif
}
*/
//uint8_t ReadDPRam8(uint32_t memAddr)
//{
//	//while(!(MCF_GPIO_PPDSDR_PWM & MCF_GPIO_PPDSDR_PWM_PPDSDR_PWM2));
//		return ((unsigned char*)memAddr)[0];
//}
//
//uint16_t ReadDPRam16(uint32_t memAddr)
//{
//	uint16_t retVal = 0;
//	for(int i =0; i < 2; i++)
//	{
//	//	while(!(MCF_GPIO_PPDSDR_PWM & MCF_GPIO_PPDSDR_PWM_PPDSDR_PWM2));
//			retVal |= ((((unsigned char *)(memAddr + i))[0]) << (i * 8));
//			//retVal |= (((unsigned char*)(memAddr + i))[0]) << (i * 8));
//	}
//	return retVal;
//}
//
//uint32_t ReadDPRam32(uint32_t memAddr)
//{
//	uint32_t retVal = 0;
//	for(int i =0; i < 4; i++)
//	{
//	//	while(!(MCF_GPIO_PPDSDR_PWM & MCF_GPIO_PPDSDR_PWM_PPDSDR_PWM2));
//			retVal |= ((((unsigned char *)(memAddr + i))[0]) << (i * 8));
//	}
//	return retVal;
//}
#endif

