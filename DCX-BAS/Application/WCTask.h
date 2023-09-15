/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/Application/WCTask.h_v   1.8.1.1   Mar 06 2017 16:25:58   EGUAJARDO  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/Application/WCTask.h_v  $
 * 
 *    Rev 1.8.1.1   Mar 06 2017 16:25:58   EGUAJARDO
 * Defined constants in upper case.
 * 
 *    Rev 1.8.1.0   Feb 08 2017 17:25:50   EGUAJARDO
 * Added  static members for watchdog implementation changes.
 * 
 *    Rev 1.8   30 Apr 2013 11:23:04   amaurya
 * Changes related to State machine optimization.
 * 
 *    Rev 1.7   16 Jan 2013 06:31:10   ygupta
 * Level2 Code with Profibus
 * 
 *    Rev 1.6   24 Nov 2012 08:07:12   amaurya
 * Added changes for 40KHz supply.
 * 
 *    Rev 1.5   05 Nov 2012 09:00:44   amaurya
 * State Machine changes
 * 
 *    Rev 1.4   26 Oct 2012 02:41:58   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.3   10 Sep 2012 03:13:46   rjamloki
 * Modbus changed implementation with unions and code cleanup
 * 
 *    Rev 1.2   29 Aug 2012 13:28:16   rjamloki
 * WC Modbus checkin and related changes
 * 
 *    Rev 1.1   02 Jul 2012 13:04:16   ygupta
 * Code cleanup for meeting coding standard. and updates after review. All varibles name in upper case. 
 * 
 *    Rev 1.0   21 Mar 2012 12:22:10   PDwivedi
 * Initial revision.
 *
 */

#ifndef WCTASK_H_
#define WCTASK_H_

#include "Task.h"
#include "FIFO.h"
#include "portable.h"


#define WCTIME  TIMERINTERRUPT_INTERVALUS

#define FRAM_BASEADDRESS   				0x0B000000
#define FPGA_BASEADDRESS   				0x08000000
#define MI_DIGITALIO_BASEADDRESS		0x08200000

#define ADC_MULT_FACTOR_PHASE_REVB	   	4
#define ADC_MULT_FACTOR_AMPL_REVB      	4
#define ADC_MULT_FACTOR_EXTAMPL_REVB   	4
#define ADC_MULT_FACTOR_CURR_REVB      	4
#define ADC_MULT_FACTOR_TEMP_REVB      	4
#define ADC_MULT_FACTOR_POWER_REVB     	5

#define ADC_MULT_FACTOR_PHASE_REVC	   	4
#define ADC_MULT_FACTOR_AMPL_REVC      	5
#define ADC_MULT_FACTOR_EXTAMPL_REVC   	5
#define ADC_MULT_FACTOR_CURR_REVC      	5
#define ADC_MULT_FACTOR_TEMP_REVC      	4
#define ADC_MULT_FACTOR_POWER_REVC     	5

//#define DAC_MULFACTOR  		4000
//#define DAC_DIVFACTOR   	2047

#define ADC_MULFACTOR   	10
#define ADC_DIVFACTOR   	8

#define FREQ_OUTFACTOR		10000

#define AMPLDECAYCUTOFF     3   //3% cutt off amplitude
#define WCTASK_STACKSIZE    2048 * 2
#define WCTASK_FIFOSIZE     1

//Bootloader version start offset from the FRAM base address
#define BOOTLOADERVERSIONOFFSET_FRAM  4
//Bootloader CRC start offset from the FRAM base address
#define BOOTLOADERCRCOFFSET_FRAM    12

#define CHECK_FOR_24V   (MCF_EPORT_EPPDR & MCF_EPORT_EPPDR_EPPD7)
#define CHECK_LCD_PREESENT (MCF_GPIO_PPDSDR_TIMER & MCF_GPIO_PPDSDR_TIMER_PPDSDR_TIMER2)
#define DUMMY_VALUES_SOFTWARE_RESET  	     0x00
#define DUMMY_VALUES_SOFTWARE_WATCHDOG_RESET 0x00

class MIATask;

struct AdcBuf;

class WCTask : public Task
{
public:
	WCTask(MIATask * Adc);
	~WCTask();

	static WCTask * thisPtr;
	Fifo<char> WCTaskQ;
	void CycleRun(void);
protected:
	void Run(void);
	MIATask * AdcTask;
private:
	void ConfigureResetforPHYandilscherWC(void);
	static UINT8 TempMcfSoftwareReset;
	static UINT8 TempMcfSoftwareWatchdogReset;
};

void SetDefaultMACCHIPRWData(void);
void TimerInterruptCallback(void);
void PrepareFBStatus(void);
#endif /*USERTASK_H_*/
