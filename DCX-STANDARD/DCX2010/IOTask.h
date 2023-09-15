/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/IOTask.h_v   1.9   09 Sep 2013 08:59:52   rjamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/IOTask.h_v  $
 * 
 *    Rev 1.9   09 Sep 2013 08:59:52   rjamloki
 * Added Seek Frequency Offset, Amplitude ramping, set all graph defaults as in WC.
 * 
 *    Rev 1.8   05 Mar 2013 06:47:08   amaurya
 * Added ADC changes.
 * 
 *    Rev 1.7   28 Aug 2011 11:42:24   ygupta
 * Ver1.01M
 * 
 *    Rev 1.6   16 Aug 2011 08:24:42   pdwivedi
 * code cleanup
 * 
 *    Rev 1.5   05 Aug 2011 10:32:48   pdwivedi
 * Removed warning.
 * 
 *    Rev 1.4   02 Aug 2011 07:38:16   pdwivedi
 * Added changes for PLL Loss lock, DCXManual and remving warnings.
 * 
 *    Rev 1.3   15 Jul 2011 09:46:14   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.2   06 Dec 2010 09:56:54   PDwivedi
 * Added changes for Revision Board C support, LCD Key Timeout changes.
 * 
 *    Rev 1.1   29 Nov 2010 07:47:36   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:44   ygupta
 * Initial revision.

******************************************************************************/

#ifndef IOTASK_H_
#define IOTASK_H_

#include "Task.h"
#include "MCP3208.h"
#include "TLV5604.h"
#include "AD7689.h"
#include <CPUdefs.h>
#include "FIFO.h"
#include "TYP_DEF.h"

#define IOTASK_STACKSIZE  2048 * 2
#define DSPICSPIN_ADC   0
#define REFVOLTAGE_ADCMCP3208    2500
#define REFVOLTAGE_ADCAD6789    5000
#define NUM_ANALOGIN_PS  5

//Factors to be applied on Amplitude, Power and Current input from
//Instrumentation board.
#define ADC_MULFACTOR   7
#define ADC_DIVFACTOR   6
struct AdcBuf
{
	int data[NUM_ANALOGIN_PS];
};

class IOTask : public Task
{
public:
	IOTask();
	static IOTask * thisPtr;
	void TestADC();
	void ReadData();
	int Receive(int *data, int timeout);
	Fifo<AdcBuf*> rxQueue;
	Fifo<char> IOTaskQ;
	Fifo<char> outQ;
	AdcBuf AdcBuff;
	ADC_RESOLUTION AdcRes;
protected:
	void Run();
	SPI * spi;
};
extern BOOLEAN AdcReInitRequired;
#endif /* IOTASK_H_ */

