// $Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/CyGOS/Arch/MCF/MCF5445x/RTC.cpp_v   1.0   09 Jun 2015 09:13:30   tayars  $
//
// Copyright (c) Cyber Group, Inc 2008
// This program is the property of Cyber Group Inc
// Copying of this software is expressly forbidden, without the prior written
// consent of Cyber Group Inc
// This file is part of CyGOS-3.
//
// This module implements the interface to Real Time Clock module
//

#include "RTC.h"
#include "CPU.h"
#include "stdio.h"
#include "assert.h"

RTC * RTC::thisPtr;
RTC::RTC()
{
	thisPtr = this;
}

RTC::~RTC()
{
}
//Note: RTC register adresses in MCF5329 and MCF54455x are different
void RTC::Init(int level, int priority)
{
//	MCF_RTC_CR = MCF_RTC_CR_XTL(MCF_RTC_CR_32768);

    MCF_RTC_RTC_GOCU = 0x00000000; 
	MCF_RTC_RTC_GOCL = 0x00008000;
//	MCF_RTC_RTC_CR |= MCF_RTC_RTC_CR_SWR;
	EnableRTC();
	MCF_RTC_RTC_DAYS = 0x00000000;
	MCF_RTC_RTC_SECONDS = 0x00000000;
	MCF_RTC_RTC_HOURMIN = 0x00000000;
	MCF_RTC_RTC_IER = 0x00000000;//All RTC interrupts disabled
//	MCF_RTC_RTC_ISR = 0xFFFFFFFF;
//	MCF_RTC_RTC_IER |=MCF_RTC_RTC_ISR_1HZ;

//	CPU::SetupHandler(IntHandler,intRTC,1,0,256);
}

void RTC::EnableRTC()
{
	MCF_RTC_RTC_CR |= MCF_RTC_RTC_CR_EN;
	if ((MCF_RTC_RTC_CR & MCF_RTC_RTC_CR_EN) == 0)
		pprintf(" RTC not initialized");
		
}

void RTC::DisableRTC()
{
	MCF_RTC_RTC_CR &= (~MCF_RTC_RTC_CR_EN);
}

void RTC::IntHandler(int vector, void * frame)
{
	//pprintf("\n RTC IntHandler");
	thisPtr->IntHandler();
}

/* This function is the RTC interrupt service routine. */
void RTC::IntHandler()
{
	  uint32 status;
	  uint32 events = 0;
	  uint32 intr_isr, intr_enable;
	  intr_isr = MCF_RTC_RTC_ISR;
	  intr_enable = MCF_RTC_RTC_IER;
	  status = intr_isr & intr_enable;
	  /* clear interrupt sources */
	  MCF_RTC_RTC_ISR = status;
	
	  /* check if 1 HZ interrupt has occured*/
	  if (status & MCF_RTC_RTC_ISR_1HZ)
	  {
	        pprintf("\n i hz interrupt");
	  }
}

/*
 * This function is used to obtain the RTC time or the alarm value in
 * second.
 */
uint32 RTC::Get_time()
{
	uint32 day = 0, hr = 0, min = 0, sec = 0, hr_min = 0;
	day = MCF_RTC_RTC_DAYS;
	hr_min = MCF_RTC_RTC_HOURMIN;
	hr = hr_min >> 8;
	min = hr_min & 0xff;
	sec = MCF_RTC_RTC_SECONDS;
	return (((day * 24 + hr) * 60) + min) * 60 + sec;
//	return 0;
}

/*
 * This function sets the RTC alarm value or the time value.
 */
void RTC::Set_time(int days, int hours, int mins, int seconds)
{
uint32 temp;
	if (thisPtr->Valid_time(days, hours, mins, seconds))
	{
		temp = (hours << 8) + mins;
		MCF_RTC_RTC_DAYS = days;
		MCF_RTC_RTC_HOURMIN = temp;
		MCF_RTC_RTC_SECONDS = seconds;
	}
	else
	{
		MCF_RTC_RTC_DAYS = 0;
		MCF_RTC_RTC_HOURMIN = 0;
		MCF_RTC_RTC_SECONDS = 0;
	}
}

/*
 * This function reads the current RTC time into tm in Gregorian date.
 */

//int RTC::mxc_rtc_read_time(struct rtc_time *tm)
//{
//	uint32 val;

	/* Avoid roll-over from reading the different registers */
//	do
//	{
//		val = Get_time(tm);
//		rtc_time_to_tm(val, tm);
//	} while (val != Get_time(tm));

//	return val;
//}


/*
 * Does the rtc_time represent a valid time?
 */
int RTC::Valid_time(int days, int hours, int mins, int seconds)
{
	if (((days >= 0) && (days <= 65535)) && ((hours >= 0)
			&& (hours <= 23))
			&& ((mins >= 0) && (mins<= 59)) && ((seconds >= 0) && (seconds <= 59)))
		return 1;

	return 0;
}
