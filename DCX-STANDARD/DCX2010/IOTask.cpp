/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/IOTask.cpp_v   1.12   09 Sep 2013 08:59:52   rjamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/IOTask.cpp_v  $
 * 
 *    Rev 1.12   09 Sep 2013 08:59:52   rjamloki
 * Added Seek Frequency Offset, Amplitude ramping, set all graph defaults as in WC.
 * 
 *    Rev 1.11   05 Mar 2013 06:46:46   amaurya
 * Added ADC changes.
 * 
 *    Rev 1.10   16 Aug 2011 08:24:26   pdwivedi
 * code cleanup
 * 
 *    Rev 1.9   02 Aug 2011 07:38:04   pdwivedi
 * Added changes for PLL Loss lock, DCXManual and remving warnings.
 * 
 *    Rev 1.8   18 Jul 2011 09:25:38   pdwivedi
 * Added changes for serial buffer size.
 * 
 *    Rev 1.7   15 Jul 2011 09:46:02   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.6   14 Jan 2011 07:55:42   PDwivedi
 * Added changes for DCX bug and proposed changes.
 * 
 *    Rev 1.5   14 Dec 2010 11:15:12   PDwivedi
 * Added changes for WebSite support.
 * 
 *    Rev 1.4   06 Dec 2010 09:56:54   PDwivedi
 * Added changes for Revision Board C support, LCD Key Timeout changes.
 * 
 *    Rev 1.3   29 Nov 2010 07:47:36   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.2   15 Nov 2010 08:47:12   PDwivedi
 * Added comments and Scan Timeout changes.
 * 
 *    Rev 1.1   12 Nov 2010 10:42:02   nhahmed
 * Read IO values only if IOTaskStop Flag is not Set.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:44   ygupta
 * Initial revision.

******************************************************************************/

#include "IOTask.h"
#include "stdio.h"
#include "DupsTask.h"
#include "IOTask.h"
#include "Board.h"
#include "UserIO.h"
#include "RTOS.h"
#include "GLOBALS.h"
#include "DebugNew.h"
/*-------------------------- Global Variables -----------------------------*/
IOTask* IOTask::thisPtr;
DupsTask *DupsTask::thisPtr;
ADC * Adc;
BOOLEAN AdcReInitRequired = FALSE;
/*-------------------------- External Variables ---------------------------*/
extern int missedWrite;
extern int msgCount;
extern int missedWrite1;
extern int msgCount1;
extern BOOLEAN IOTaskStop;
extern BOOLEAN IORead;
/*--------------------------------------------- ---------------------------*/

void MSTickFunc()
/******************************************************************************/
/* MSTickFunc(void)															         		*/
/* Description:-                                                            	*/
/* The function wakes up IOTask and Dups Task.   										*/
/******************************************************************************/
{
	MiliSecTimer+=DCPTIME;

	if(MiliSecTimer >= CONV_MILLI_MICROSEC(MAXMILLISEC_ONE_SEC))
	{
		SecTimerSincePowerUp++;
		MiliSecTimer = 0;
	}
	if (!(RTOS::IsRunning()))
		return;

	if(IOTask::thisPtr->IOTaskQ.Write(0, 'i', 0))
		msgCount++;
	else
		missedWrite++;

	if(DupsTask::thisPtr->DupsTaskQ.Write(0, 'd', 0))
		msgCount1++;
	else
		missedWrite1++;
}

IOTask::IOTask() :
	Task("IOTask", IOTASK_STACKSIZE), rxQueue(1, "rxADC"), IOTaskQ(1, "IOTaskQ"), outQ(2, "IODACQ")
{
	SPI * spi = Board::GetSPI(0, 5, 5);
	this->spi = spi;
	AdcRes = ADC_RESOLUTION_11BIT;
	Adc = new AD7689(spi, DSPICSPIN_ADC, REFVOLTAGE_ADCAD6789);
	pprintf("\n Init ADC7689");
	Adc->SetResolution(AdcRes);
	Adc->EnableSequencing(NUM_ANALOGIN_PS - 1);
	thisPtr = this;
	for (int i =0; i<NUM_ANALOGIN_PS; i++)
		AdcBuff.data[i]= 0;
	AdcReInitRequired = FALSE;
}

void IOTask::Run()
{
	for (;;)
	{
		if(IOTaskStop)
		{
			DelayMs(1);
			continue;
		}
		char i;
		IOTaskQ.Read(this, i, -1);
		BOOLEAN r = outQ.Read(this, i , 0);

		if(r)//Dups has updated all outputs and it is the right time to send them to MI board
			UserIO::WriteOutputs();
		if(AdcReInitRequired == TRUE){
			  Adc->EnableSequencing(NUM_ANALOGIN_PS - 1);
			  AdcReInitRequired = FALSE;
		 }
		ReadData();
	}
}

void IOTask::ReadData()
/******************************************************************************/
/* ReadData(void)															         			*/
/* Description:-                                                            	*/
/* The function reads ADC values from Instrumentation board and writes into   */
/* FIFO which is read by DUPS task.It also reads Digital and Analog values    */
/* from MI board. 																				*/
/******************************************************************************/
{
	if(!IORead)
	{
		Adc ->ReadInputsThroughSequencing(AdcBuff.data);
		for(int ChannelIndex = 0; ChannelIndex < NUM_ANALOGIN_PS; ChannelIndex++){
			   if(AdcRes == ADC_RESOLUTION_11BIT)
				   AdcBuff.data[ChannelIndex] &= 0x07FF;
		      //analog input signal 0..+10V from Instrumentation board..we need 0 to 12.5 for current , power and phase.
		      //channel 0, 1 and 2 are for Amplitude, power and current respectively. Channel 3 and 4 are for temperature and phase.
		      if(ChannelIndex < TEMP_SEL)
		    	  AdcBuff.data[ChannelIndex] = (AdcBuff.data[ChannelIndex] * ADC_MULFACTOR / ADC_DIVFACTOR);
		}
		IORead = TRUE;
	}		
	UserIO::ReadInputs();//MI Inputs
	rxQueue.Write(this, &AdcBuff, 0);
}
