/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/Application/DEF_DEF.H_v   1.33   Mar 06 2017 16:35:42   EGUAJARDO  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/Application/DEF_DEF.H_v  $
 * 
 *    Rev 1.33   Mar 06 2017 16:35:42   EGUAJARDO
 * Removed comment about available bit for future use.
 * 
 *    Rev 1.32   Feb 09 2017 09:02:42   EGUAJARDO
 * Changes for AC line restore sequence.
 * 
 *    Rev 1.31   23 Jan 2015 15:11:10   ygupta
 * Added NUMBER_OF_MB_RETRIES and made the vale 10.
 * 
 *    Rev 1.30   11 Dec 2013 03:32:00   rjamloki
 * Power Default set to 4000, Ethernet Link debouncing disabled by default.
 * 
 *    Rev 1.29   06 Dec 2013 09:38:42   rjamloki
 * S and V loop constants incorporated.
 * 
 *    Rev 1.28   20 Nov 2013 06:05:40   rjamloki
 * Profibus default address changed. Watchdog default timeout changed.
 * 
 *    Rev 1.27   15 Nov 2013 05:51:16   rjamloki
 * Export log fixes, Preready clearing the web command for statemachine, set up alarm implemented for invalid preset.
 * 
 *    Rev 1.26   11 Oct 2013 04:11:30   rjamloki
 * DPRAM wait states changed to 20. Critical section removed from watchdog feed.
 * 
 *    Rev 1.25   08 Oct 2013 06:17:38   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.24   03 Oct 2013 04:26:34   rjamloki
 * Watchdog related Changes
 * 
 *    Rev 1.23   24 Aug 2013 17:13:58   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.22   10 Jul 2013 07:49:46   amaurya
 * Fixed powerup DCP Event Log issue.
 * 
 *    Rev 1.21   27 Jun 2013 05:53:48   amaurya
 * System crash fixes.
 * 
 *    Rev 1.20   14 Jun 2013 11:21:06   amaurya
 * MH1 Round fixes
 * 
 *    Rev 1.19   24 May 2013 10:15:30   ygupta
 * Fixed Tracker Issue.
 * 
 *    Rev 1.18   21 May 2013 12:25:48   amaurya
 * Code review fixes.
 * 
 *    Rev 1.17   07 May 2013 10:12:26   amaurya
 * Code review fixed.
 * 
 *    Rev 1.16   06 May 2013 09:14:28   amaurya
 * Changes to use actual frequency.
 * 
 *    Rev 1.15   30 Apr 2013 11:18:46   amaurya
 * Changes related to State machine optimization.
 * 
 *    Rev 1.14   22 Apr 2013 11:28:56   amaurya
 * Fixed tracker issue of Ver2.0S
 * 
 *    Rev 1.13   15 Mar 2013 10:47:50   ygupta
 * Issues Resolved, Requirement Changes
 * 
 *    Rev 1.12   11 Mar 2013 02:23:36   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.11   11 Jan 2013 02:50:20   ygupta
 * Level2 Requirement Changes
 * 
 *    Rev 1.10   29 Nov 2012 08:44:00   rjamloki
 * Scan Changes and Coverty issues Resolved
 * 
 *    Rev 1.9   24 Nov 2012 08:00:18   amaurya
 * Added changes for 40KHz supply.
 * 
 *    Rev 1.8   26 Oct 2012 02:33:48   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.7   29 Aug 2012 13:27:46   rjamloki
 * WC Modbus checkin and related changes
 * 
 *    Rev 1.5   05 Jul 2012 04:44:50   ygupta
 * Cleanup after review summary on database and BSP.
 * 
 *    Rev 1.4   02 Jul 2012 13:04:06   ygupta
 * Code cleanup for meeting coding standard. and updates after review. All varibles name in upper case. 
 * 
 *    Rev 1.3   29 Jun 2012 12:13:22   ygupta
 * First Compilable code, First check in after review, Needs more clanup
 * 
 *    Rev 1.2   21 Jun 2012 12:31:54   ygupta
 * Cleaned Up further
 * 
 *    Rev 1.1   20 Jun 2012 10:36:02   ygupta
 * Added defines for the number of General retry sequence. File Needs more cleanup.
 *
 */

#ifndef DEF_DEF_H_
#define DEF_DEF_H_

/*(Definitions):											*/
#include "WCTask.h"
#include "SysConfig.h"
/*******************************************************/
#define FRAM_BASEADDRESS   			0x0B000000 //(flexbus chip select 1)
#define HILSCHERDPRAM_BASEADDRESS   0x08000000 //(flexbus chip select 2)
#define MI_DIGITALIO_BASEADDRESS 	0x08200000 //(flexbus chip select 3)

//Chip select pin number on CPU
#define CHIPSELCT_PIN1 1
#define CHIPSELCT_PIN2 2
#define CHIPSELCT_PIN3 3

//wait states for memory mapped devices
//for flexbus interface initialization
#define WAITSTATES_FRAM_CS1   	  7//20//5
#define WAITSTATES_DPRAM_CS2  	  20
#define WAITSTATES_DIGITALIO_CS3  5

//databus width for memory mapped device
#define DATABUSWIDTH_FRAM_CS1  		8
#define DATABUSWIDTH_DPRAM_CS2 		8
#define DATABUSWIDTH_DIGITALIO_CS3  8

//Number of retries in general applicable to any retry sequence in application code
#define NUMBER_OF_RETRIES 	3
//Number of retries in Modbus communication
#define NUMBER_OF_MB_RETRIES 10

//Number of times the writing of 1 and 0 on digital output should be check back on  shorted //digital input. It is required for checking the coldstart at power up.
#define N_COLDSTART_SEQUENCE_CHECK  2

//WC on board MAC chip (i.e.24AA025E48) specific defines.
//MAC chip has to store the serial number in read/write area apart from providing unique
//MAC address from read only location. The chip supports I2C.

//slave address on I2C bus
#define MACCHIIP_I2C_ADDRESS  0xA6

//Address of read only location in MAC chip  where MAC gets stored.
#define MACSTART_ADDRESS	0xFA	//0xFA to FF(6 byte MAC)
#define MAC_ADDRESS_LEN		6// 6 byte MAC address

//The MAC to be used in case reading the MAC from device is failed
#define DEFAULT_MAC_ADDR 	(UINT8*)"\x12\xC6\x05\xDC\x1C\x01"

//The start address of read/write area in device
#define MACCHIP_RWDATAAREA_STARTADDR	0x00
//The length of the read/write area in device in bytes
#define MACCHIP_RWDATAAREA_LEN	128

//The default serial number.
#define DEFAULT_SERIAL_NUMBER	"DEFAULT1234"
#define DEFAULT_FREQ	FREQ_20KHZ
#define DEFAULT_SYSTEM  SYSTEM_ADVANCED
#define DEFAULT_POWER	4000 //watt

//The maximum length of serial number
//The max length which can be set from website is 20
//4 bytes are reserved
#define SERIALNUMBER_LEN	24

//defines for Ethernet interface instance for DCP and outside world
#define ETHERNET_INTERFACE_WCTODCP  0
#define ETHERNET_INTERFACE_WCTOWEB  1

//timer interrupt interval in microseconds.
#define TIMERINTERRUPT_INTERVALUS   1000

//global define for BIT postion check and set
#define BIT(x)	(1 << (x))

/*******************************************************/

#define FREQUENCY_MULTFACTOR   40

// Various counters defines
#define MAX_CYCLE_COUNTER 999999999
#define MAX_ERROR_COUNTER 999999999
#define MAX_SEC_COUNTER   999999999
#define MAX_HOUR_COUNTER  999999
#define MAX_TIMER_COUNTER 60000000     //1 Minute
#define SIZE_WELDBUFFER   5000
#define SIZE_WELDHISTORY_BUFF   50
#define SIZE_SCANBUFFER   	2100
#define MILLISEC_ONE_SEC 1000
#define MAXMILLISEC_ONE_SEC 1000
#define GRAPHPOINT   5000
#define DCPTIME  1000
#define CONV_MILLI_MICROSEC(x)  ((x) * 1000)
#define CONVMICRO_MILLI(x)      ((x) / 1000)
#define SCALEDMAXMILLISEC_ONE_SEC   ((MILLISEC_ONE_SEC * 1000) / TIMERINTERRUPT_INTERVALUS)
#define SCALEDTIME(x)            	(((double)((x) * 1000)) / TIMERINTERRUPT_INTERVALUS)

#define SCALEDTIMETOWEBSITE(x)   	(UINT32)((((double)(x) * TIMERINTERRUPT_INTERVALUS)) / 1000)
//#define SCALEDTIMETOWEBSITE(x)   (UINT32)((((double)x * DCPTIME)) / 1000)
// AMPLITUDE AND FRQUENCY Maximum and Minimum VALUES

#define FREQ_MIN          (SINT32)(-5000)
#define FREQ_MAX          5000

// Control Loop Definitions
#define MAXDCPADCRAWVALUE    ((1 << DCPTimeControlParam.AdcResolution) - 1)
#define FREQ_SCALING_FACTOR 10

// FPGA inputs
#define FPGA_SIGN      4
#define OL_TEMPERATURE 8
#define OL_POWER       16
#define OL_VOLTAGE     32
#define OL_CURRENT     64
#define OVL_FUNCTION   128

#define FOREVER 	1    //for infinite looping
#define SWVERSION_LEN    16
#define OPERATORIDLEN_MAX 16

//Manufacturing
#define MIN_CYCLE_TIME 0
//Max Cycle Time 99.59 hours settable from System Test page converted to milliseconds.
#define MAX_CYCLE_TIME 359940000

#define MIN_ONIME_PERCYCLE 10
#define MAX_ONIME_PERCYCLE 99999
#define MIN_OFFIME_PERCYCLE 10
#define MAX_OFFIME_PERCYCLE 99999

// Midband defines for different power supply specification
//#define F20KHZF_FACTOR  100
#define F20KHZMIDBAND   (199500)
#define F20KHZMINABS (F20KHZMIDBAND - 5000)
#define F20KHZMAXABS (F20KHZMIDBAND + 5000)
#define F30KHZMIDBAND (300000)
#define F30KHZMINABS (F30KHZMIDBAND - 7500)
#define F30KHZMAXABS (F30KHZMIDBAND + 7500)

#define F40KHZMIDBAND (399000)
#define F40KHZMINABS (F40KHZMIDBAND - 10000)
#define F40KHZMAXABS (F40KHZMIDBAND + 10000)
#define SCALING_FACTOR  100

//DCP Time control Params
#define DCP_TIMERINTERRUPTUS_MIN 100
#define DCP_TIMERINTERRUPTUS_MAX 1000
#define DCP_TIMERINTERRUPTUS_DEFAULT DCP_TIMERINTERRUPTUS_MIN

#define DCP_STATEMACHINETIMEUS_MIN DCP_TIMERINTERRUPTUS_MIN
#define DCP_STATEMACHINETIMEUS_MAX DCP_TIMERINTERRUPTUS_MAX
#define DCP_STATEMACHINETIMEUS_DEFAULT DCP_STATEMACHINETIMEUS_MAX

#define DCP_CONTROLLOOPTIMEUS_MIN DCP_TIMERINTERRUPTUS_MIN
#define DCP_CONTROLLOOPTIMEUS_MAX DCP_TIMERINTERRUPTUS_MAX
#define DCP_CONTROLLOOPTIMEUS_DEFAULT DCP_CONTROLLOOPTIMEUS_MIN

#define DCP_MODBUSCOMMTIMEUS_MIN DCP_TIMERINTERRUPTUS_MIN
#define DCP_MODBUSCOMMTIMEUS_MAX DCP_TIMERINTERRUPTUS_MAX
#define DCP_MODBUSCOMMTIMEUS_DEFAULT DCP_MODBUSCOMMTIMEUS_MAX

//Available ADC Resolutions
#define DCP_ADCRESOLUTION_11BIT 11
#define DCP_ADCRESOLUTION_16BIT 16
//default ADC resolution
#define DCP_ADCRESOLUTION_DEFAULT 11

#define DCP_BLINDTIMEUS_MIN	0
#define DCP_BLINDTIMEUS_MAX	10000
#define DCP_BLINDTIMEUS_DEFAULT 5000

//Defaults from S and V
#define DCP_TIMERINTERRUPTUS_DEFAULT_SV 1000
#define DCP_STATEMACHINETIMEUS_DEFAULT_SV 1000
#define DCP_CONTROLLOOPTIMEUS_DEFAULT_SV 1000
#define DCP_MODBUSCOMMTIMEUS_DEFAULT_SV 1000
#define DCP_ADCRESOLUTION_DEFAULT_SV 11
#define DCP_BLINDTIMEUS_DEFAULT_SV 0


#define UINT_MAX 0xFFFFFFFFu
#define UINT_MIN 0x00000000u

#define SINT_MAX 0x7FFFFFFF
#define SINT_MIN 0x80000001

#define UINT16_MAX 0xFFFF
#define UINT16_MIN 0x0000

#define SINT16_MAX 0x7FFF
#define SINT16_MIN 0x8001

#define SINT8_MAX 0x7F
#define SINT8_MIN 0x81

#define UINT8_MAX 0xFF
#define UINT8_MIN 0x00

#define NUSINMINUTE (60000000u)
//Added Yugal
#define IOCYCLE_START  BIT(0)
#define IOWELD_START   BIT(1)
#define IOTRIGGER      BIT(2)
#define FBCYCLE_START  BIT(3)
#define WEBWELD_START  BIT(4)
//Leave Bit4 for future(Used for web)
#define WELDSTART_MASKNOACT (IOCYCLE_START | IOWELD_START | FBCYCLE_START | WEBWELD_START)
#define WELDSTART_MASKACT (IOCYCLE_START | IOWELD_START | FBCYCLE_START | WEBWELD_START)

#define WELDSTART_MASK  (FBCYCLE_START | WEBWELD_START | IOWELD_START)

#define IOEXTTEST    BIT(5)
#define WEBTEST      BIT(6)
#define LCDTEST      BIT(7)
#define FBTEST       BIT(8)
//Leave BIT9 for future
#define TESTSTART_MASK (IOEXTTEST | WEBTEST | LCDTEST | FBTEST)

#define AC_RESTORE_SEEK  	BIT(9)
#define POWERUPSEEK    		BIT(10)
#define IOSEEK       		BIT(11)
#define TIMEDSEEK    		BIT(12)
#define FBSEEK       		BIT(13)
#define WEB_SEEK    		BIT(14)

#define SEEKSTART_MASK (POWERUPSEEK | IOSEEK | TIMEDSEEK | FBSEEK | WEB_SEEK | AC_RESTORE_SEEK)

#define POWERUPSCAN   		BIT(15)
#define WEBSCAN      		BIT(16)
#define IOSCAN       		BIT(17)
#define FBSCAN       		BIT(18)
#define AC_RESTORE_SCAN 	BIT(19)

#define SCANSTART_MASK (POWERUPSCAN | WEBSCAN | IOSCAN | FBSCAN | AC_RESTORE_SCAN)

#define IORESET  			BIT(20)
#define LCDRESET 			BIT(21)
#define WEBRESET 			BIT(22)
#define FBRESET  			BIT(23)
#define AC_RESTORE_RESET	BIT(24)

#define RESET_MASK (IORESET | LCDRESET | WEBRESET | FBRESET | AC_RESTORE_RESET)

#define MANUFACT_WELD BIT(25)
#define MANUFACT_STARTMASK (MANUFACT_WELD)

#define IOMASK (IOCYCLE_START | IOWELD_START | IOTRIGGER | IOSEEK | IOSCAN | IOEXTTEST | IORESET)
#define FBMASK (FBCYCLE_START | FBSEEK | FBSCAN | FBTEST | FBRESET)
#define WEBMASK (WEBSCAN | WEBTEST | WEB_SEEK | WEBWELD_START | WEBRESET)
#define AC_RESTORE_MASK (AC_RESTORE_SEEK | AC_RESTORE_SCAN)

#define SCANABORT_USER 1
#define SCANABORT_IO 2
#define SCANABORT_FB 3
#define SCANABORT_TIMEOUT 4
#define TASKNAMELEN			24

#define WD_DEFAULT  29 // 2^29 / system clock = 2.15Sec
#define WD_MIN 		8
#define WD_MAX		31
#endif   /* DEF_DEF_H_ */
