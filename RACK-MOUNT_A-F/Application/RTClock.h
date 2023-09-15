/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/RTClock.h_v   1.0   09 Jun 2015 09:10:50   tayars  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/RTClock.h_v  $
 * 
 *    Rev 1.0   09 Jun 2015 09:10:50   tayars
 * Initial revision.
 * 
 *    Rev 1.2   29 Jun 2012 12:16:08   ygupta
 * First Compilable code, First check in after review, Needs more clanup
 *
 */

#ifndef RTCLOCK_H_
#define RTCLOCK_H_

#include "portable.h"
#include "PCF8563.h"

//The members of this structure are in the same sequence as of
//the registers of PCF8563 device.
//The device understands only BCD format data and
//accessed using I2C.
struct RTCTime
{
	UINT8 Second; //<seconds 00 to 59 coded in BCD>
	UINT8 Minute; //<minutes 00 to 59 coded in BCD>
	UINT8 Hour;	  //<hours 00 to 23 coded in BCD>
	UINT8 Day;	  //<days 01 to 31 coded in BCD>
	UINT8 Weekdays; //<weekdays 0 to 6 > Not in BCD
	UINT8 Century_Month; //<months 01 to 12 coded in BCD>,Month or century
	UINT8 Years; //<years 00 to 99 coded in BCD>
};

#define ADDR_CONTOL_STATUS1  0x00
#define ADDR_CONTOL_STATUS2  0x01

#define ADDR_TIME_VL_SECONDS 0x02
#define ADDR_TIME_MINUTES    0x03
#define ADDR_TIME_HOURS      0x04
#define ADDR_TIME_DAYS       0x05
#define ADDR_TIME_WEEKDAYS   0x06
#define ADDR_TIME_CENTURY_MONTH	 0x07
#define ADDR_TIME_YEAR       0x08

#define RTC_SLAVE_ADDR_READ  0xA3
#define RTC_SLAVE_ADDR_WRITE 0xA2

#define STOP_BIT 			 0x20

//Some of the high order bits in the bcd values
//read from device are garbasge and need to be masked.
#define MASK_REG_SECONDS 	 0x7F
#define MASK_REG_MINUTES 	 0x7F
#define MASK_REG_HOURS 		 0x3F
#define MASK_REG_DAYS 		 0x3F
#define MASK_REG_WEEKDAYS 	 0x07
#define MASK_REG_CENTURY_MONTH   0x1F
#define MASK_REG_YEAR 		 0xFF

#define ADDR_ALARM_MINUTE 	 0x09
#define ADDR_ALARM_HOUR   	 0x0A
#define ADDR_ALARM_DAY    	 0x0B
#define ADDR_ALARM_WEEKDAY   0x0C

#define ADDR_CLKOUT_CONTROL  0x0D
#define ADDR_TIMER_CONTROL 	 0x0E
#define ADDR_TIMER 0x0F

class RTClock
{
	static UINT8 Bin2Bcd8(UINT8 Bin_val);
	static UINT8 Bcd2Bin8(UINT8 Bcd_val);
 public:
	static BOOLEAN InitRTC();
	static BOOLEAN GetCurrentTime(RTCTime * Rt);
	static BOOLEAN SetCurrentTime(RTCTime * Rt);
};
#endif /* RTCLOCK_H_ */
