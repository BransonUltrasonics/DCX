/**************************************************************************************
 
   Copyright (c) Hilscher GmbH. All Rights Reserved.
 
 **************************************************************************************
 
   Filename:
    $Workfile:   OS_Custom.cpp  $
   Last Modification:
    $Author:   RJamloki  $
    $Modtime:   21 Aug 2015 08:29:46  $
    $Revision:   1.1  $
   
   Targets:
    None
 
   Description:
     Target system abstraction layer
       
   Changes:
 
     Version   Date        Author   Description
     ----------------------------------------------------------------------------------
     1         07.08.2006  MT       initial version
     
**************************************************************************************/

/*****************************************************************************/
/*! \file OS_Custom.c
*    Sample Target system abstraction layer. Implementation must be done
*    according to used target system                                         */
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/CifXToolkit/OSAbstraction/OS_Custom.cpp_v  $
 * 
 *    Rev 1.1   21 Aug 2015 05:46:36   RJamloki
 * Fix for coverity CID 10926
*/

#include "OS_Dependent.h"
#include "RTOS.h"
#include "DPMInterface.h"
#include "cifXErrors.h"
#include "cifxToolkit.h"
#include "CPU.h"

volatile uint32_t s_ulTime      = 0;
static          int      s_fOSInitDone = 0;

/*****************************************************************************/
/*!  \addtogroup CIFX_TK_OS_ABSTRACTION Operating System Abstraction
*    \{                                                                      */
/*****************************************************************************/

/*****************************************************************************/
/*! Memory allocation function
*   \param ulSize    Length of memory to allocate
*   \return Pointer to allocated memory                                      */
/*****************************************************************************/
void* OS_Memalloc(uint32_t ulSize)
{
	unsigned char * ret = new unsigned char[ulSize];
	return ret;
}

/*****************************************************************************/
/*! Memory freeing function
*   \param pvMem Memory block to free                                        */
/*****************************************************************************/
void OS_Memfree(void* pvMem)
{
	unsigned char * dMem = (unsigned char*)pvMem;
	delete dMem;
}

/*****************************************************************************/
/*! Memory reallocating function (used for resizing dynamic toolkit arrays)
*   \param pvMem     Memory block to resize
*   \param ulNewSize new size of the memory block
*   \return pointer to the resized memory block                              */
/*****************************************************************************/
void* OS_Memrealloc(void* pvMem, uint32_t ulNewSize)
{

	return Memrealloc(pvMem, ulNewSize, __FILE__, __LINE__);
}

/*****************************************************************************/
/*! Memory setting
*   \param pvMem     Memory block
*   \param bFill     Byte to use for memory initialization
*   \param ulSize    Memory size for initialization)                         */
/*****************************************************************************/
void OS_Memset(void* pvMem, uint8_t bFill, uint32_t ulSize)
{
	memset(pvMem, bFill, ulSize);
}

uint8_t ReadByte(uint32_t Addr)
{

	uint8_t data1, data2;

   data1 = *(uint8_t *)Addr; // Here you read memory
   data2 = *(uint8_t *)Addr; // Do 2 readings to start with

   do

   {

      data1 = *(uint8_t *)Addr; // Here you read memory

      data2 = *(uint8_t *)Addr;

   } while (data1 != data2) ;

   return data1;

}


/*****************************************************************************/
/*! Copy memory from one block to another
*   \param pvDest    Destination memory block
*   \param pvSrc     Source memory block
*   \param ulSize    Copy size in bytes                                      */
/*****************************************************************************/
void OS_Memcpy(void* pvDest, void* pvSrc, uint32_t ulSize)
{
//	CriticalSection cs;
	memcpy(pvDest, pvSrc, ulSize);
//	 int i;
//	 unsigned char temp;
//	   for (i = 0; i < ulSize; i++)
//
//	   {
//	       ((unsigned char*)pvDest)[i] = ReadByte((uint32_t)(pvSrc) +  i);
//	   }
//	   //temp = ReadByte((uint32_t)(pvSrc));
//	   //pprintf("\n %c %02X", temp, temp);
//
//	   return;


//	unsigned char temp = 0;
//	for(int i = 0; i < ulSize;i++)
//	{
//		while(temp != ((unsigned char*)pvSrc)[i])
//		{
//			temp = ((unsigned char*)pvSrc)[i];
//		}
//		((unsigned char*)pvDest)[i] = temp;
//		//temp = pvDest[i];
//	}
//	CriticalSection cs;
//	unsigned char * dst = (unsigned char *)pvDest;
//	unsigned char * src = (unsigned char *)pvSrc;
//	 while (ulSize--)
//	 {
//		while(!(MCF_GPIO_PPDSDR_PWM & MCF_GPIO_PPDSDR_PWM_PPDSDR_PWM2));
//		*dst = *(volatile uint8_t*)src;
//		dst++;
//		src++;
//	}

//	 for(int i = (ulSize -1); i >=0; i--)
//	{
//		while(!(MCF_GPIO_PPDSDR_PWM & MCF_GPIO_PPDSDR_PWM_PPDSDR_PWM2));
//		((unsigned char *)pvDest)[i] = ((unsigned char *)pvSrc)[i];
//	}
	//temp = ReadByte((uint32_t)(pvSrc));
//	temp = *(uint8_t *)pvSrc;
//		   pprintf("\n %c %02X", temp, temp);
	//
}

/*****************************************************************************/
/*! Compare two memory blocks
*   \param pvBuf1    First memory block
*   \param pvBuf2    Second memory block
*   \param ulSize    Compare size in bytes
*   \return 0 if both buffers are equal                                      */
/*****************************************************************************/
int OS_Memcmp(void* pvBuf1, void* pvBuf2, uint32_t ulSize)
{
	return memcmp(pvBuf1, pvBuf2, ulSize);
}

/*****************************************************************************/
/*! Move memory
*   \param pvDest    Destination memory
*   \param pvSrc     Source memory
*   \param ulSize    Size in byte to move                                    */
/*****************************************************************************/
void OS_Memmove(void* pvDest, void* pvSrc, uint32_t ulSize)
{
	//memmove(pvDest, pvSrc, ulSize);
	unsigned char * src = new unsigned char[ulSize];
	memcpy(src, pvSrc, ulSize);
	memcpy(pvDest, src, ulSize);
	delete[] src;//coverity CID: 10926
}


/*****************************************************************************/
/*! Sleep for a specific time
*   \param ulSleepTimeMs  Time in ms to sleep for                            */
/*****************************************************************************/
void OS_Sleep(uint32_t ulSleepTimeMs)
{
	RTOS::DelayMs(ulSleepTimeMs);
}

/*****************************************************************************/
/*! Retrieve a counter based on millisecond used for timeout monitoring
*   \return Current counter value (resolution of this value will influence
*           timeout monitoring in driver/toolkit functions(                  */
/*****************************************************************************/
uint32_t OS_GetMilliSecCounter(void)
{
//	ASSERT(0);
	return s_ulTime;
}

/*****************************************************************************/
/*! Create an auto reset event
*   \return handle to the created event                                      */
/*****************************************************************************/
void* OS_CreateEvent(void)
{
	return (void*)1;
}

/*****************************************************************************/
/*! Set an event
*   \param pvEvent Handle to event being signalled                           */
/*****************************************************************************/
void OS_SetEvent(void* pvEvent)
{
	pvEvent = 0;
}

/*****************************************************************************/
/*! Reset an event
*   \param pvEvent Handle to event being reset                               */
/*****************************************************************************/
//void OS_ResetEvent(void* pvEvent)
//{
//	pvEvent = 0;
//}

/*****************************************************************************/
/*! Delete an event
*   \param pvEvent Handle to event being deleted                             */
/*****************************************************************************/
void OS_DeleteEvent(void* pvEvent)
{
	pvEvent = 0;
}

/*****************************************************************************/
/*! Wait for the signalling of an event
*   \param pvEvent   Handle to event being wait for
*   \param ulTimeout Timeout in ms to wait for event
*   \return 0 if event was signalled                                         */
/*****************************************************************************/
uint32_t OS_WaitEvent(void* pvEvent, uint32_t ulTimeout)
{
	pvEvent = 0;
	ulTimeout = 0;
	return CIFX_EVENT_SIGNALLED;
}

/*****************************************************************************/
/*! Compare two ASCII string
*   \param pszBuf1   First buffer
*   \param pszBuf2   Second buffer
*   \return 0 if strings are equal                                           */
/*****************************************************************************/
int OS_Strcmp(const char* pszBuf1, const char* pszBuf2)
{
	return strcmp(pszBuf1, pszBuf2);
}

/*****************************************************************************/
/*! Compare characters of two strings without regard to case
*   \param pszBuf1   First buffer
*   \param pszBuf2   Second buffer
*   \param ulLen     Number of characters to compare
*   \return 0 if strings are equal                                           */
/*****************************************************************************/
int OS_Strnicmp(const char* pszBuf1, const char* pszBuf2, uint32_t ulLen)
{
	return strnicmp(pszBuf1, pszBuf2, ulLen);
}  

/*****************************************************************************/
/*! Query the length of an ASCII string
*   \param szText    ASCII string
*   \return character count of szText                                        */
/*****************************************************************************/
int OS_Strlen(const char* szText)
{
	return strlen(szText);
}

/*****************************************************************************/
/*! Copies one string to another monitoring the maximum length of the target
*   buffer.
*   \param szDest    Destination buffer
*   \param szSource  Source buffer
*   \param ulLength  Maximum length to copy
*   \return pointer to szDest                                                */
/*****************************************************************************/
char* OS_Strncpy(char* szDest, const char* szSource, uint32_t ulLength)
{
	 return strncpy(szDest,szSource,ulLength);
}


/*****************************************************************************/
/*! Create an interrupt safe locking mechanism (Spinlock/critical section)
*   \return handle to the locking object                                     */
/*****************************************************************************/
void* OS_CreateLock(void)
{
	 return OS_CreateMutex();
}

/*****************************************************************************/
/*! Enter a critical section/spinlock
*   \param pvLock Handle to the locking object                               */
/*****************************************************************************/
void OS_EnterLock(void* pvLock)
{
	OS_WaitMutex(pvLock, 0);
}

/*****************************************************************************/
/*! Leave a critical section/spinlock
*   \param pvLock Handle to the locking object                               */
/*****************************************************************************/
void OS_LeaveLock(void* pvLock)
{
	OS_ReleaseMutex(pvLock);
}

/*****************************************************************************/
/*! Delete a critical section/spinlock object
*   \param pvLock Handle to the locking object being deleted                 */
/*****************************************************************************/
void OS_DeleteLock(void* pvLock)
{
	OS_DeleteMutex(pvLock);
}

/*****************************************************************************/
/*! Create an Mutex object for locking code sections 
*   \return handle to the mutex object                                       */
/*****************************************************************************/
void* OS_CreateMutex(void)
{
	return (void*)1;
}

/*****************************************************************************/
/*! Wait for mutex
*   \param pvMutex    Handle to the Mutex locking object
*   \param ulTimeout  Wait timeout                                           
*   \return !=0 on succes                                                    */
/*****************************************************************************/
int OS_WaitMutex(void* pvMutex, uint32_t ulTimeout)
{
	pvMutex = 0;
	ulTimeout = 0;
	return 1;
}

/*****************************************************************************/
/*! Release a mutex section section
*   \param pvMutex Handle to the locking object                              */
/*****************************************************************************/
void OS_ReleaseMutex(void* pvMutex)
{
	pvMutex = 0;
}

/*****************************************************************************/
/*! Delete a Mutex object
*   \param pvMutex Handle to the mutex object being deleted                  */
/*****************************************************************************/
void OS_DeleteMutex(void* pvMutex)
{
	pvMutex = 0;
}

/*****************************************************************************/
/*! Opens a file in binary mode
*   \param szFile     Full file name of the file to open
*   \param pulFileLen Returned length of the opened file
*   \return handle to the file, NULL mean file could not be opened           */
/*****************************************************************************/
void* OS_FileOpen(char* szFile, uint32_t* pulFileLen)
{
	szFile = 0;
	pulFileLen = 0;
   return 0;
}

/*****************************************************************************/
/*! Closes a previously opened file
*   \param pvFile Handle to the file being closed                            */
/*****************************************************************************/
void OS_FileClose(void* pvFile)
{
	pvFile = 0;
   return;
}

/*****************************************************************************/
/*! Read a specific amount of bytes from the file
*   \param pvFile   Handle to the file being read from
*   \param ulOffset Offset inside the file, where read starts at
*   \param ulSize   Size in bytes to be read
*   \param pvBuffer Buffer to place read bytes in
*   \return number of bytes actually read from file                          */
/*****************************************************************************/
uint32_t OS_FileRead(void* pvFile, uint32_t ulOffset, uint32_t ulSize, void* pvBuffer)
{
	pvFile = 0;
	ulOffset = 0;
	ulSize = 0;
	pvBuffer = 0;
   return 0;
}

/*****************************************************************************/
/*! OS specific initialization (if needed), called during cifXTKitInit()    */
/*!  \return CIFX_NO_ERROR on success										*/
/*****************************************************************************/
int32_t OS_Init()
{
	//DPM_Init();
	pprintf("\n %s",__FUNCTION__);
	//RTOS::DelayMs(1000);
	s_fOSInitDone = 1;

	  return CIFX_NO_ERROR;
}

/*****************************************************************************/
/*! OS specific de-initialization (if needed), called during cifXTKitInit()  */
/*****************************************************************************/
void OS_Deinit()
{
}

/*****************************************************************************/
/*! This functions is called for PCI cards in the toolkit. It is expected to
* write back all BAR's (Base address registers), Interrupt and Command
* Register. These registers are invalidated during cifX Reset and need to be
* re-written after the reset has succeeded
*   \param pvOSDependent OS Dependent Variable passed during call to
*                        cifXTKitAddDevice
*   \param pvPCIConfig   Configuration returned by OS_ReadPCIConfig
*                        (implementation dependent)                          */
/*****************************************************************************/
void OS_WritePCIConfig(void* pvOSDependent, void* pvPCIConfig)
{
	pvOSDependent = 0;
	pvPCIConfig = 0;
  /* TODO: Implement PCI register accesss, needed for cifX cards, */
}

/*****************************************************************************/
/*! This functions is called for PCI cards in the toolkit. It is expected to
* read all BAR's (Base address registers), Interrupt and Command Register.
* These registers are invalidated during cifX Reset and need to be
* re-written after the reset has succeeded
*   \param pvOSDependent OS Dependent Variable passed during call to
*                        cifXTKitAddDevice
*   \return pointer to stored register copies (implementation dependent)     */
/*****************************************************************************/
void* OS_ReadPCIConfig(void* pvOSDependent)
{
	pvOSDependent = 0;
  /* TODO: Implement PCI register accesss, needed for cifX cards. */
	return 0;
}

/*****************************************************************************/
/*! This function Maps a DPM pointer to a user application if needed.
*   This example just returns the pointer valid inside the driver.
*   \param pvDriverMem   Pointer to be mapped
*   \param ulMemSize     Size to be mapped
*   \param ppvMappedMem  Returned mapped pointer (usable by application)
*   \param pvOSDependent OS Dependent variable passed during call to
*                        cifXTKitAddDevice
*   \return Handle that is needed for unmapping NULL is a mapping failure    */
/*****************************************************************************/
void* OS_MapUserPointer(void* pvDriverMem, uint32_t ulMemSize, void** ppvMappedMem, void* pvOSDependent)
{
	pvOSDependent = 0;
	ulMemSize = 0;
	 *ppvMappedMem = pvDriverMem;
	  return pvDriverMem;
}

/*****************************************************************************/
/*! This function unmaps a previously mapped user application pointer 
*   \param phMapping      Handle that was returned by OS_MapUserPointer
*   \param pvOSDependent  OS Dependent variable passed during call to
*                         cifXTKitAddDevice
*   \return !=0 on success                                                   */
/*****************************************************************************/
int OS_UnmapUserPointer(void* phMapping, void* pvOSDependent)
{
	phMapping = 0;
	pvOSDependent = 0;
	return 1;
}

/*****************************************************************************/
/*! This function enables the interrupts for the device physically
*   \param pvOSDependent OS Dependent Variable passed during call to
*                        cifXTKitAddDevice                                   */
/*****************************************************************************/
void OS_EnableInterrupts(void* pvOSDependent)
{
	pvOSDependent = 0;
	pprintf("\n %s ", __FUNCTION__);
	CPU::EnableExternalInterrupt(intEPFR6);
}

/*****************************************************************************/
/*! This function enables the interrupts for the device physically
*   \param pvOSDependent OS Dependent Variable passed during call to
*                        cifXTKitAddDevice                                   */
/*****************************************************************************/
void OS_DisableInterrupts(void* pvOSDependent)
{
	pvOSDependent = 0;
	pprintf("\n %s ", __FUNCTION__);
	CPU::DisableExternalInterrupt(intEPFR6);
}

/*****************************************************************************/
/*! \}                                                                       */
/*****************************************************************************/
