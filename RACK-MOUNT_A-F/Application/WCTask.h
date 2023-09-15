/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/WCTask.h_v   1.2   24 Jul 2015 08:39:58   sesharma  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/WCTask.h_v  $
 * 
 *    Rev 1.2   24 Jul 2015 08:39:58   sesharma
 * Added declaration for ConfigureResetForPHYandHilscher and SendResetToPhyAndHilscher for all three boards.
 * 
 *    Rev 1.1   24 Jun 2015 08:39:54   amaurya
 * Removed AmplitudeDecay
 * 
 *    Rev 1.0   09 Jun 2015 09:10:46   tayars
 * Initial revision.
 * 
 *    Rev 1.8   30 Apr 2013 11:23:04   amaurya
 * Changes related to State machine optimization.
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

#define WCTASK_STACKSIZE    2048 * 2
#define WCTASK_FIFOSIZE     1

//Bootloader version start offset from the FRAM base address
#define BOOTLOADERVERSIONOFFSET_FRAM  4
//Bootloader CRC start offset from the FRAM base address
#define BOOTLOADERCRCOFFSET_FRAM    12

#define CHECK_FOR_24V   (MCF_EPORT_EPPDR & MCF_EPORT_EPPDR_EPPD7)
#define CHECK_LCD_PREESENT (MCF_GPIO_PPDSDR_TIMER & MCF_GPIO_PPDSDR_TIMER_PPDSDR_TIMER2)


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
};

void SetDefaultMACCHIPRWData(void);
void TimerInterruptCallback(void);
void PrepareFBStatus(void);
void ConfigureResetForPHYandHilscherWC991(void);
void ConfigureResetForPHYandHilscherWC1169(void);
void ConfigureResetForPHYandHilscherSC1054(void);
void SendResetToPhyAndHilscherWC991(void);
void SendResetToPhyAndHilscherWC1169(void);
void SendResetToPhyAndHilscherSC1054(void);
#endif /*USERTASK_H_*/
