/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/DupsTask.h_v   1.8.1.2.1.1   02 Jul 2014 09:15:40   amaurya  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/DupsTask.h_v  $
 * 
 *    Rev 1.8.1.2.1.1   02 Jul 2014 09:15:40   amaurya
 * Removed Amplitude Decay function.
 * 
 *    Rev 1.8.1.2.1.0   09 Sep 2013 08:59:50   rjamloki
 * Added Seek Frequency Offset, Amplitude ramping, set all graph defaults as in WC.
 * 
 *    Rev 1.8.1.2   01 Sep 2011 13:05:54   ygupta
 * Added LED support for Watchdog and LCD communication.Removed CPU Restart function call and removed some pprintfs.
 * 
 *    Rev 1.8.1.1   28 Aug 2011 11:42:24   ygupta
 * Ver1.01M
 * 
 *    Rev 1.8.1.0   16 Aug 2011 08:22:52   pdwivedi
 * code cleanup
 * 
 *    Rev 1.8   15 Jul 2011 09:44:52   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.7   14 Dec 2010 11:15:10   PDwivedi
 * Added changes for WebSite support.
 * 
 *    Rev 1.6   09 Dec 2010 07:54:26   PDwivedi
 * Added changes for preset and control Loop parameters according to original DUPS code in REV C board.
 * 
 *    Rev 1.4   08 Dec 2010 07:42:28   PDwivedi
 * Added changes for User Configurable Analog outputs.
 * 
 *    Rev 1.3   06 Dec 2010 09:56:54   PDwivedi
 * Added changes for Revision Board C support, LCD Key Timeout changes.
 * 
 *    Rev 1.2   29 Nov 2010 07:47:34   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.1   22 Nov 2010 09:04:22   PDwivedi
 * Added changes for code clean up, Software Digital tune,Digital output.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:42   ygupta
 * Initial revision.

******************************************************************************/

#ifndef DUPSTASK_H_
#define DUPSTASK_H_
#include "Task.h"
#include "FIFO.h"

#define DCPTIME  1000

#define  FRAM_BASEADDRESS   			0x0B000000
#define  FPGA_BASEADDRESS   			0x08000000
#define  MI_DIGITALIO_BASEADDRESS 	0x08200000

#define ADC_MULT_FACTOR_PHASE_REVB		4
#define ADC_MULT_FACTOR_AMPL_REVB      4
#define ADC_MULT_FACTOR_EXTAMPL_REVB   4
#define ADC_MULT_FACTOR_CURR_REVB      4
#define ADC_MULT_FACTOR_TEMP_REVB      4
#define ADC_MULT_FACTOR_POWER_REVB     5

#define ADC_MULT_FACTOR_PHASE_REVC		4
#define ADC_MULT_FACTOR_AMPL_REVC      5
#define ADC_MULT_FACTOR_EXTAMPL_REVC   5
#define ADC_MULT_FACTOR_CURR_REVC      5
#define ADC_MULT_FACTOR_TEMP_REVC      4
#define ADC_MULT_FACTOR_POWER_REVC     5

#define DAC_MULFACTOR  4000
#define DAC_DIVFACTOR   2047

#define FREQ_OUTFACTOR  10000

#define DUPSTASK_STACKSIZE       2048 * 2
#define DUPSTASK_FIFOSIZE        1

class IOTask;

struct AdcBuf;

class DupsTask : public Task
{
public:
	DupsTask(IOTask *adc);
	~DupsTask();
	unsigned char ReadAddress(unsigned char Addr);
	void WriteAddress(unsigned char Addr, unsigned char val);
	static DupsTask * thisPtr;
	Fifo<char> DupsTaskQ;
	void Cycle_Run(void);
protected:
	void Run();
	void Tick();
	IOTask *adcTask;
};
#endif /*USERTASK_H_*/
