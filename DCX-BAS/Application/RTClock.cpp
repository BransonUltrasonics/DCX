/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/Application/RTClock.cpp_v   1.8   08 Oct 2013 06:17:32   rjamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/WC/Application/RTClock.cpp_v  $
 * 
 *    Rev 1.8   08 Oct 2013 06:17:32   rjamloki
 * Fixed warnings.
 * 
 *    Rev 1.7   24 Aug 2013 17:14:10   amaurya
 * Code review and tracker issue fixes.
 * 
 *    Rev 1.6   11 Jun 2013 09:50:48   amaurya
 * MH round I testing fixes.
 * 
 *    Rev 1.5   29 Oct 2012 02:56:14   rjamloki
 * Printfs disbaled and changes in website
 * 
 *    Rev 1.4   26 Oct 2012 02:39:10   rjamloki
 * Website Related files Added
 * 
 *    Rev 1.3   05 Jul 2012 04:46:50   ygupta
 * Cleanup after review summary on database and BSP.
 * 
 *    Rev 1.2   29 Jun 2012 12:15:56   ygupta
 * First Compilable code, First check in after review, Needs more clanup
 *
 */

#include "Board.h"
#include "RTClock.h"
#include "DEF_DEF.H"
#include "portable.h"

#define printf
#define pprintf

#define CHK_INTEGRITY_BIT(V)  ((V & 0x80)? FALSE : TRUE)	//Register VL_seconds (BIT 7 = 0(clock integrity is guaranteed),
																				 //BIT 7 = 1(integrity of the clock information is not guaranteed))
PCF8563 * RtcPtr;


/*   Purpose:
 *   	This function is called once at power up from PowerUpInitialization() function to initialize RTC object.
 *
 *   Entry condition:
 *   	None.
 *
 *   Exit condition:
 * 		Returns TRUE if integrity OK else returns FALSE.
 */
BOOLEAN RTClock::InitRTC()
{
	RTCTime Rt;
	int Len = sizeof(RTCTime);
	BOOLEAN RtcIntegrityFlag = TRUE;
//	pprintf("\n P4");
	RtcPtr = new  PCF8563(Board::GetI2C(0, 5, 5), RTC_SLAVE_ADDR_READ, RTC_SLAVE_ADDR_WRITE);

	RtcPtr -> Read(ADDR_TIME_VL_SECONDS, (unsigned char *)&Rt, Len, NUMBER_OF_RETRIES);
//	pprintf("\n P5");
	if(CHK_INTEGRITY_BIT(Rt.Second) == FALSE)
		RtcIntegrityFlag = FALSE;
//	pprintf("\n P6");
	return RtcIntegrityFlag;
}


/*   Purpose:
 *   	This function is called by LOG module to determine the event and alarm time and for
 * 		any other time stamping purpose.
 *
 *   Entry condition:
 *		Rt:pointer to RTCTime object where the current date time gets copied.
 *      The hours are in 24 hour format.
 *
 *   Exit condition:
 *		returns TRUE on successful read else returns FALSE.
 */
BOOLEAN RTClock::GetCurrentTime(RTCTime * Rt)
{
	int Len = sizeof(RTCTime);
	//flag to generate integrity flag alarm
	BOOLEAN RtcIntegrityFlag = TRUE;
	RtcPtr -> Read(ADDR_TIME_VL_SECONDS, (unsigned char *)Rt, Len, NUMBER_OF_RETRIES);

	if(CHK_INTEGRITY_BIT(Rt -> Second) == FALSE){
		RtcIntegrityFlag = FALSE;
		memset(Rt , 0, sizeof(RTCTime));
	}
	else{
		//Mask unused bits in the get values convert the given BCD data by chip to binary
		Rt -> Second = Bcd2Bin8(Rt -> Second & MASK_REG_SECONDS);
		Rt -> Minute = Bcd2Bin8(Rt -> Minute & MASK_REG_MINUTES);
		Rt -> Hour = Bcd2Bin8(Rt -> Hour & MASK_REG_HOURS);
		Rt -> Day  = Bcd2Bin8(Rt -> Day & MASK_REG_DAYS);
		Rt -> Century_Month = Bcd2Bin8(Rt -> Century_Month & MASK_REG_CENTURY_MONTH);
		Rt -> Weekdays = Bcd2Bin8(Rt -> Weekdays & MASK_REG_WEEKDAYS);
		Rt -> Years = Bcd2Bin8(Rt -> Years & MASK_REG_YEAR);
	}//else
	return RtcIntegrityFlag;
}

/*   Purpose:
 *		This function is called to set the time for RTC.
 *		It shall be called by Website module or Fieldbus module to set the RTC time.
 *
 *   Entry condition:
 *		Rt: The RTCTime object contains the date time to be set.
 *
 *   Exit condition:
 *		return TRUE if the time is successfully set else returns FALSE.
 */
BOOLEAN RTClock::SetCurrentTime(RTCTime * Rt)
{
	//flag to generate RTC set failed alarm
	BOOLEAN RtcSetFlagSuccess = TRUE;
	UINT8 ControlByte = 0;
	//check limits
	if((Rt->Second > 59)||(Rt->Minute > 59)||(Rt->Hour > 23)||(Rt->Weekdays > 6)
			|| (Rt->Day < 1) || (Rt->Day > 31)|| (Rt->Century_Month < 1) || (Rt->Century_Month > 12)
			||(Rt->Years > 99))
		RtcSetFlagSuccess = FALSE;
	else{//stop RTC
		ControlByte |= STOP_BIT;
		RtcPtr -> Write(ADDR_CONTOL_STATUS1, (unsigned char *)&ControlByte, sizeof(ControlByte));
		//chip requires data in BCD. Convert to BCD
		Rt -> Second = Bin2Bcd8(Rt -> Second);
		Rt -> Minute = Bin2Bcd8(Rt -> Minute);
		Rt -> Day  = Bin2Bcd8(Rt -> Day);
		Rt -> Hour = Bin2Bcd8(Rt -> Hour);
		Rt -> Century_Month = Bin2Bcd8(Rt -> Century_Month);
		Rt -> Years = Bin2Bcd8(Rt -> Years);
		//set current time
		RtcPtr -> Write(ADDR_TIME_VL_SECONDS, (unsigned char *)Rt, sizeof(RTCTime));
		//first check integrity is OK before verifying that the time we read
		//is same as time we set
		//start RTC
		ControlByte &= ~STOP_BIT;
		RtcPtr -> Write(ADDR_CONTOL_STATUS1, (unsigned char *)&ControlByte, sizeof(ControlByte));
	}
	return RtcSetFlagSuccess;
}



/*   Purpose:
 *		This function is called to convert the binary 8 bit data to corresponding BCD 8bit data.
 * 		The function is private to this class and only called by SetCurrentTime()function of this class
 * 		while setting the RTC time.
 *
 *   Entry condition:
 *		Bin_val- 8 bit binary data (maximum value 99)
 *
 *   Exit condition:
 *		It returns the BCD equivalent of binary data.
 */
UINT8 RTClock::Bin2Bcd8(UINT8 Bin_val)
{
	UINT8 Bcd_val = 0;

	if(Bin_val <= 99)
	{
		while(Bin_val > 9)
		{
			Bcd_val += 0x10;
			Bin_val -= 10;
		}
		Bcd_val += Bin_val;
	}
	return(Bcd_val);
}


/*
 * 	 Purpose:
 *		This function is called to convert the bcd 8 bit data to corresponding binary 8bit data.
 * 		The function is private to this class and only called by GetCurrentTime () function of this class.
 *
 *   Entry condition:
 *		Bcd_val- 8 bit binary data (maximum value 153 i.e. binary equivalent of bcd 99)
 *
 *   Exit condition:
 *		It returns the binary equivalent of bcd data.
 */
UINT8 RTClock::Bcd2Bin8(UINT8 Bcd_val)
{
	UINT8 Bin_val = 0;
    /*
     * Check for a valid BCD formated number
     * max expected value 0x99 = 153 decimal
     */
    if(Bcd_val <= 153)
    {
    	while(Bcd_val > 9)
        {
        	Bcd_val -= 0x10;
            Bin_val += 10;
        }
        Bin_val += Bcd_val;
    }
    return(Bin_val);
}
