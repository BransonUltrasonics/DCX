/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/FieldbusTable.h_v   1.1   07 May 2015 13:30:54   gbutron  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/FieldbusTable.h_v  $
 * 
 *    Rev 1.1   07 May 2015 13:30:54   gbutron
 * #define	FB_PARAM_AMPLITUDEPROFILEENERGY(UINT16)1069 Added
 * 
 *    Rev 1.0   13 Mar 2014 16:04:48   rjamloki
 * Initial revision.
*/
#ifndef FIELDBUSTABLE_H_
#define FIELDBUSTABLE_H_

#define FB_PARAM_FPGAVERSIONLABEL					(UINT16)101
#define FB_PARAM_BOOTLOADERVERSIONLABEL				(UINT16)102
#define FB_PARAM_SWVERSIONLABEL						(UINT16)103
#define FB_PARAM_WCBOOTLOADERVERSIONLABEL			(UINT16)112
#define FB_PARAM_WCSWVERSIONLABEL					(UINT16)113
#define FB_PARAM_SERIALNO							(UINT16)154
#define	FB_PARAM_FBFREQOFFSET						(UINT16)439
#define	FB_PARAM_EXTAMPLITUDESET					(UINT16)440
#define	FB_PARAM_CURRCYCLETIME						(UINT16)860
#define	FB_PARAM_SONICSONTIME						(UINT16)861

#define	FB_PARAM_ENERGYCP							(UINT16)1062
#define	FB_PARAM_PEAKPOWERCP						(UINT16)1063
#define	FB_PARAM_GNDDTIME							(UINT16)1064


#define	FB_PARAM_AMPLITUDEPROFILEENERGY				(UINT16)1069                // ADDED


#define	FB_PARAM_FREQOFFSET							(UINT16)1074
#define	FB_PARAM_SEEKFREQOFFSET						(UINT16)1465
#define	FB_PARAM_TESTFREQOFFSET						(UINT16)1483
#define	FB_PARAM_HOLDTIMEFLAG						(UINT16)1075
#define	FB_PARAM_TIMEERRORHIGH						(UINT16)1086
#define	FB_PARAM_ENERGYERRORHIGH					(UINT16)1087
#define	FB_PARAM_PEAKPOWERERRORHIGH					(UINT16)1088
#define	FB_PARAM_TIMEERRORWINDOWLOW					(UINT16)1089
#define	FB_PARAM_TIMEERRORWINDOWHIGH				(UINT16)1090
#define	FB_PARAM_ENERGYERRORWINDOWLOW				(UINT16)1091
#define	FB_PARAM_ENERGYERRORWINDOWHIGH				(UINT16)1092
#define	FB_PARAM_PEAKPOWERERRORWINDOWLOW			(UINT16)1093
#define	FB_PARAM_PEAKPOWERERRORWINDOWHIGH			(UINT16)1094
#define	FB_PARAM_FREQUENCYERRORLOW					(UINT16)1095
#define	FB_PARAM_FREQUENCYERRORHIGH					(UINT16)1096
#define	FB_PARAM_CUSTOMAIN1CUTOFFFLAG				(UINT16)1098
#define	FB_PARAM_CUSTOMAIN2CUTOFFFLAG				(UINT16)1099
#define	FB_PARAM_SEEKFREQLOW						(UINT16)1467
#define	FB_PARAM_SEEKFREQHIGH						(UINT16)1468
#define	FB_PARAM_WELDFREQLOW						(UINT16)1486
#define	FB_PARAM_WELDFREQHIGH						(UINT16)1487
#define	FB_PARAM_FREQUENCYSTART						(UINT16)1490
#define	FB_PARAM_FREQUENCYSTOP						(UINT16)1491
#define	FB_PARAM_DIGITALTUNE						(UINT16)1505
#define	FB_PARAM_SCRUBTIMEFLAG						(UINT16)1164
#define	FB_PARAM_PEAKPOWERRAW						(UINT16)1242
#define	FB_PARAM_TESTPEAKPOWERRAW					(UINT16)1795




#endif /* FIELDBUSTABLE_H_ */
