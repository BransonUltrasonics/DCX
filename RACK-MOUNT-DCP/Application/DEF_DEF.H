/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/DEF_DEF.H_v   1.3   16 Sep 2016 09:16:04   EGUAJARDO  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/DEF_DEF.H_v  $
 * 
 *    Rev 1.3   16 Sep 2016 09:16:04   EGUAJARDO
 * Modified error string size.
 * 
 *    Rev 1.1   19 Mar 2015 06:37:52   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.0   18 Aug 2014 09:17:22   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:12:58   amaurya
 * Initial revision.
 */

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
#define ADC11BITVALUE      2047 //10000
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

#define WR_ERRORMSG_SIZE			50

#endif   /* DEF_DEF_H_ */
