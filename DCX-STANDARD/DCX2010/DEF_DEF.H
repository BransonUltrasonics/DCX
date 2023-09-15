/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/DEF_DEF.H_v   1.18.1.1   12 Mar 2014 07:52:28   rjamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/DEF_DEF.H_v  $
 * 
 *    Rev 1.18.1.1   12 Mar 2014 07:52:28   rjamloki
 * Sequencing Error Fix, Generic OL bit taken care, Time field in logs separated by colon.
 * 
 *    Rev 1.18.1.0   09 Sep 2013 08:59:50   rjamloki
 * Added Seek Frequency Offset, Amplitude ramping, set all graph defaults as in WC.
 * 
 *    Rev 1.18   15 Jan 2013 04:39:08   amaurya
 * Energy Special.
 * 
 *    Rev 1.15.1.0   23 Aug 2012 11:34:38   amaurya
 * Implemented S & V Graph Plot.
 * 
 *    Rev 1.15   12 Dec 2011 11:12:04   tayars
 * Removed 15kHz reference
 * 
 *    Rev 1.14   17 Oct 2011 07:36:10   RatneshA
 * changed vesion string define length
 * 
 *    Rev 1.13   04 Oct 2011 14:08:54   Builder
 * The Builder machine had a problem in the first go around of building the release code. 
 * This is the correct file that will be released.
 * Changed the software version string length
 * 
 *    Rev 1.12   02 Aug 2011 07:36:50   pdwivedi
 * Added changes for PLL Loss lock, DCXManual and remving warnings.
 * 
 *    Rev 1.11   22 Jul 2011 16:53:44   tayars
 * Added New Control Loops
 * 
 *    Rev 1.10   21 Jun 2011 11:05:22   PDwivedi
 * Fixes after mongle horde round1
 * 
 *    Rev 1.9   25 Apr 2011 08:43:50   PDwivedi
 * Changes after MAC chip addition
 * 
 *    Rev 1.8   19 Apr 2011 10:46:26   PDwivedi
 * Added changes for DCX Tracker Issue.
 * 
 *    Rev 1.7   04 Apr 2011 10:58:28   PDwivedi
 * Changes to support Manufacturing WebPage functionality
 * 
 *    Rev 1.6   02 Mar 2011 08:20:46   PDwivedi
 * Added FPGA_BOARDTESTOUTPUT_ADDR for board test.
 * 
 *    Rev 1.5   09 Dec 2010 07:54:24   PDwivedi
 * Added changes for preset and control Loop parameters according to original DUPS code in REV C board.
 * 
 *    Rev 1.4   06 Dec 2010 09:56:52   PDwivedi
 * Added changes for Revision Board C support, LCD Key Timeout changes.
 * 
 *    Rev 1.3   29 Nov 2010 07:47:34   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.2   22 Nov 2010 09:04:22   PDwivedi
 * Added changes for code clean up, Software Digital tune,Digital output.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:40   ygupta
 * Initial revision.

******************************************************************************/

#ifndef DEF_DEF_H_
#define DEF_DEF_H_

/*(Definitions):											*/
#include "DupsTask.h"

// Midband defines for different power supply specification
#define F20KHZF_FACTOR 100
#define F20KHZMIDBAND 199500

#define F30KHZF_FACTOR 150
#define F30KHZMIDBAND 200000

#define F40KHZF_FACTOR 200
#define F40KHZMIDBAND 199500

#define FREQUENCY_MULTFACTOR   40

// Various counters defines
#define MAX_CYCLE_COUNTER 999999999
#define MAX_ERROR_COUNTER 999999999
#define MAX_SEC_COUNTER   999999999
#define MAX_HOUR_COUNTER  999999
#define MAX_TIMER_COUNTER 60000
#define SIZE_WELDBUFFER   5000
#define SIZE_SCANBUFFER   2000
#define MAXMILLISEC_ONE_SEC 1000
#define GRAPHPOINT 5000

#define CONV_MILLI_MICROSEC(x)  (x * 1000)
#define CONVMICRO_MILLI(x)      (x / 1000)

#define SCALEDMAXMILLISEC_ONE_SEC   ((MAXMILLISEC_ONE_SEC * 1000) / DCPTIME)
#define SCALEDTIMETOWEBSITE(x)   (UINT32)((((double)x * DCPTIME)) / 1000)
// AMPLITUDE AND FRQUENCY Maximum and Minimum VALUES
#define MAX_PWM_AMP       255
#define MIN_PWM_AMP       0
#define FREQ_MIN          -5000
#define FREQ_MAX          5000
#define RESOLUTION_FACTOR 2684
#define CLK_FREQ          6000

// Control Loop Definitions
#define AMPLOOP_C1FACTOR   10000
#define AMPLOOP_C2FACTOR   1000000
#define FREQLOOP_KI				10000
#define FREQLOOP_KP				10000
#define AMPLOOP_C1FACTORNEW   10000
#define AMPLOOP_C2FACTORNEW   1000000
#define MAX_NUM_PHASE_AVERAGES 		500
#define MIN_NUM_PHASE_AVERAGES 		1
#define ADC11BITVALUE      2047//10000
#define SCALING_FACTOR     100
#define REFERENCE_ADCVOLT  2500 // 2.5V

// Preset Definitions
#define MAX_WORK_NUMBER             1
#define MAXPARAMNAME_LEN            15

// FPGA inputs
#define FPGA_SIGN      4
#define OL_TEMPERATURE 8
#define OL_POWER       16
#define OL_VOLTAGE     32
#define OL_CURRENT     64
#define OVL_FUNCTION   128

//All overloads excluding Temperature and generic bit
#define MULTIPLE_ALARM_MASK  (OL_POWER | OL_VOLTAGE| OL_CURRENT)
//All overloads including the generic OL bit
#define OL_MASK   (OL_TEMPERATURE | OL_POWER | OL_VOLTAGE | OL_CURRENT | OVL_FUNCTION)
//All overloads excluding the generic OL bit
#define OL_MASK_EXCGEN   (OL_TEMPERATURE | OL_POWER | OL_VOLTAGE | OL_CURRENT)
// FPGA output
#define FPGAOVERLOAD_OUT    1
#define SONIC_ON_OUT 		2

#define NO_OUT        0

#define FOREVER 1    //for infinite looping

// ADC Analog input channel Select from power board.
#define POWER_SEL             1
#define AMP_SEL               0
#define CURRENT_SEL           2
#define TEMP_SEL              3
#define PHASE_MAG_SEL         4

// FPGA Address
#define FPGA_INPUT_ADDR        0x01
#define FPGA_AMP_ADDR_BYTE1    0x01
#define FPGA_AMP_ADDR_BYTE2    0x02
#define FPGA_FREQ_ADDR_BYTE1   0x04
#define FPGA_FREQ_ADDR_BYTE2   0x05
#define FPGA_FREQ_ADDR_BYTE3   0x06
#define FPGA_FREQ_ADDR_BYTE4   0x07
#define FPGA_OFFSET_ADDR_BYTE1 0x08
#define FPGA_OFFSET_ADDR_BYTE2 0x09
#define FPGA_OUTPUT_ADDR       0x0A
#define FPGA_BOARDTESTOUTPUT_ADDR 0xAA

#define SWVERSION_LEN    9
#define FPGASWVERSION_LEN    5
#define MAXI2CRETRIES    5
#define I2CCHIP_AT24C64_ADDRESS   0xA0
#define I2C_AT24C64_READADDRESS   0
#define FPGAVERSTRINGREVB  "1.0C"

#define I2CCHIP_24AA025E48_ADDRESS   0xA6//A0 and A1 are pulled up.
#define I2C_24AA025E48_MACSTART_ADDRESS 0xFA//(start of MAC address)0xFA to FF(6 byte MAC)
#define MAC_ADDRESS_LEN 6// 6 byte MAC address
#define DEFAULT_MAC_ADDR (uint8*)"\x12\xC6\x05\xDC\x1C\x01"
#define DEFAULT_SERIAL_NUMBER "DEFAULT1234"

//24AA025E48 specific defines
//#define I2C_24AA025E48_CHECKSUM 0x12A1356F
#define I2C_24AA025E48_DATA_ADDR 0x00 //
#define I2C_24AA025E48_DATA_LEN 128
#define SERAILNUMBER_LEN 24


//Manufacturing
#define MIN_CYCLE_TIME 0
#define MAX_CYCLE_TIME 359940000

#define MIN_ONIME_PERCYCLE 10
#define MAX_ONIME_PERCYCLE 99999


#define MIN_OFFIME_PERCYCLE 10
#define MAX_OFFIME_PERCYCLE 99999

#endif   /* DEF_DEF_H_ */
