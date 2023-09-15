/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/SCAN.H_v   1.6   09 Oct 2012 07:08:08   ygupta  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/SCAN.H_v  $
 * 
 *    Rev 1.6   09 Oct 2012 07:08:08   ygupta
 * Fixed scaling issue on 30 kHz horn scan.
 * 
 *    Rev 1.5   16 Aug 2011 08:27:32   pdwivedi
 * code cleanup
 * 
 *    Rev 1.4   13 Jun 2011 09:16:58   PDwivedi
 * Default step size is doubled for 30Khz PS
 * 
 *    Rev 1.3   03 Jun 2011 05:49:22   PDwivedi
 * Default frequency step for 40KHZ Ps is doubled.
 * 
 *    Rev 1.2   16 Dec 2010 08:17:42   PDwivedi
 * Added changes for Website Code Clean Up and Compressed file download from serial.
 * 
 *    Rev 1.1   09 Dec 2010 07:54:26   PDwivedi
 * Added changes for preset and control Loop parameters according to original DUPS code in REV C board.
 * 
 *    Rev 1.0   29 Nov 2010 07:52:58   PDwivedi
 * Initial revision.
******************************************************************************/
#ifndef SCAN_H_
#define SCAN_H_

#define SCANTIMEOUT      45000

/*---------------------------- Default Scan Preset values -------------------------------*/
// 20 KHZ
#define STARTFREQ20K 	194500
#define STOPFREQ20K 	204500
#define FREQSTEP20K 	10
#define TIMEDELAY20K 	10
#define MAXAMPLITUDE20K 10
#define MAXCURRENT20K 	10

// 30 KHZ
#define STARTFREQ30K 	292500
#define STOPFREQ30K 	307500
#define FREQSTEP30K 	10
#define TIMEDELAY30K 	10
#define MAXAMPLITUDE30K 10
#define MAXCURRENT30K 	10

// 40 KHZ
#define STARTFREQ40K 	389000
#define STOPFREQ40K 	409000
#define FREQSTEP40K 	10
#define TIMEDELAY40K 	10
#define MAXAMPLITUDE40K 10
#define MAXCURRENT40K 	10

/*---------------------------- Minimum/Maximum scan Preset values -------------------------------*/

#define SCANFREQ20KSTEP_MIN       10
#define SCANFREQ20KSTEP_MAX      500

#define SCANFREQ30KSTEP_MIN       10
#define SCANFREQ30KSTEP_MAX      500

#define SCANFREQ40KSTEP_MIN       10
#define SCANFREQ40KSTEP_MAX      500

#define SCANFREQ15KSTART_MIN  195000
#define SCANFREQ15KSTART_MAX  205000

#define SCANFREQ15KEND_MIN    195000
#define SCANFREQ15KEND_MAX    205000

#define SCANFREQ20KSTART_MIN  194500
#define SCANFREQ20KSTART_MAX  204500

#define SCANFREQ20KEND_MIN    194500
#define SCANFREQ20KEND_MAX    204500

#define SCANFREQ30KSTART_MIN  292500
#define SCANFREQ30KSTART_MAX  307500

#define SCANFREQ30KEND_MIN    292500
#define SCANFREQ30KEND_MAX    307500

#define SCANFREQ40KSTART_MIN  389000
#define SCANFREQ40KSTART_MAX  409000

#define SCANFREQ40KEND_MIN    389000
#define SCANFREQ40KEND_MAX    409000


#define SCANMAXAMP_MIN        10
#define SCANMAXAMP_MAX        50

#define SCANMAXCURRENT_MIN    10
#define SCANMAXCURRENT_MAX    50

#define SCANTIMEDELAY_MIN     10
#define SCANTIMEDELAY_MAX     100

#endif /* SCAN_H_ */
