/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/IOTask.cpp_v   1.3   06 Jun 2016 15:00:44   GButron  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/IOTask.cpp_v  $
 * 
 *    Rev 1.3   06 Jun 2016 15:00:44   GButron
 * V1.3.7A FT Commands
 * 
 *    Rev 1.2   19 Mar 2015 06:52:56   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.1   17 Sep 2014 08:41:20   amaurya
 * Added Comments, code review changes fixes.
 * 
 *    Rev 1.0   18 Aug 2014 09:17:24   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:00   amaurya
 * Initial revision.
 */

#include "IOTask.h"
#include "stdio.h"
#include "DupsTask.h"
#include "IOTask.h"
#include "Board.h"
#include "UserIO.h"
#include "RTOS.h"
#include "GLOBALS.h"
#include "DebugNew.h"
#include "CommandsFT.h"
/*-------------------------- Global Variables -----------------------------*/
IOTask* IOTask::thisPtr;
DupsTask *DupsTask::thisPtr;
ADC * Adc;
BOOLEAN AdcReInitRequired = FALSE;
/*-------------------------- External Variables ---------------------------*/
extern SINT32 missedWrite;
extern SINT32 msgCount;
extern SINT32 missedWrite1;
extern SINT32 msgCount1;
extern BOOLEAN IOTaskStop;
extern BOOLEAN IOReadFlag;
/*--------------------------------------------- ---------------------------*/

void MSTickFunc()
/******************************************************************************/
/* MSTickFunc(void)															         		*/
/* Description:-                                                            	*/
/* The function wakes up IOTask and Dups Task.   										*/
/******************************************************************************/
{
	MiliSecTimer += DCPTIME;

	if (MiliSecTimer >= CONV_MILLI_MICROSEC(MAXMILLISEC_ONE_SEC)) {
		SecTimerSincePowerUp++;
		MiliSecTimer = 0;
	}
	if (!(RTOS::IsRunning()))
		return;

	if (IOTask::thisPtr->IOTaskQ.Write(0, 'i', 0))
		msgCount++;
	else
		missedWrite++;

	if (DupsTask::thisPtr->DupsTaskQ.Write(0, 'd', 0))
		msgCount1++;
	else
		missedWrite1++;
}

IOTask::IOTask() :
		Task("IOTask", IOTASK_STACKSIZE), rxQueue(1, "rxADC"), IOTaskQ(1,
		      "IOTaskQ"), outQ(2, "IODACQ")
/***************************************************************************************/
/*Description:
 This is the constructor for IOTask. Initialises the ADC AD7689*/
/***************************************************************************************/
{
	SPI * spi = Board::GetSPI(0, 5, 5);
	this->spi = spi;
	AdcRes = ADC_RESOLUTION_11BIT;
	Adc = new AD7689(spi, DSPICSPIN_ADC, REFVOLTAGE_ADCAD6789);
	pprintf("\n Init ADC7689");
	Adc->SetResolution(AdcRes);
	Adc->EnableSequencing(NUM_ANALOGIN_PS - 1);
	thisPtr = this;
	for (UINT8 i = 0; i < NUM_ANALOGIN_PS; i++)
		AdcBuff.data[i] = 0;
	AdcReInitRequired = FALSE;
}
void EnableDigitalOutputLatch()
/***************************************************************************************/
/*Description:
 Toggles the latch enable and inverts the output*/
/***************************************************************************************/
{
	MCF_PAD_PAR_FBCTL &= (~MCF_PAD_PAR_FBCTL_PAR_TS(3)); //configure A3 as GPIO
	MCF_GPIO_PDDR_FBCTL |= MCF_GPIO_PDDR_FBCTL_PDDR_FBCTL0; // coniure data direction as out
	MCF_GPIO_PODR_FBCTL &= ~MCF_GPIO_PODR_FBCTL_PODR_FBCTL0; //Drive low to enable latch;
}
void IOTask::Run()
/***************************************************************************************/
/*Description:
 This is the forever loop for IOTask. It inverts the output, read and write analog
 and digital values.*/
/***************************************************************************************/
{
	UserIO::ClearOutputs();
	EnableDigitalOutputLatch();
	for (;;) {
		while (IOTaskStop) {
			DelayMs(1);
		}
		char i;
		IOTaskQ.Read(this, i, -1);
		BOOLEAN r = outQ.Read(this, i, 0);

		if (r) //Dups has updated all outputs and it is the right time to send them to MI board
			if(CommandsFT::Status==CommandsFT::FT_OFF) {
				UserIO::WriteOutputs();
			}
		if (AdcReInitRequired == TRUE) {
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
	if (IOReadFlag==FALSE) {
		Adc->ReadInputsThroughSequencing(AdcBuff.data);
		for (UINT8 ChannelIndex = 0; ChannelIndex < NUM_ANALOGIN_PS;
		      ChannelIndex++) {
			if (AdcRes == ADC_RESOLUTION_11BIT)
				AdcBuff.data[ChannelIndex] &= 0x07FF;
			//analog input signal 0..+10V from Instrumentation board..we need 0 to 12.5 for current , power and phase.
			//channel 0, 1 and 2 are for Amplitude, power and current respectively. Channel 3 and 4 are for temperature and phase.
			if (ChannelIndex < TEMP_SEL)
				AdcBuff.data[ChannelIndex] = (AdcBuff.data[ChannelIndex]
				      * ADC_MULFACTOR / ADC_DIVFACTOR);
		}
		IOReadFlag = TRUE;
	}
	UserIO::ReadInputs();		      //MI Inputs
	rxQueue.Write(this, &AdcBuff, 0);
}
