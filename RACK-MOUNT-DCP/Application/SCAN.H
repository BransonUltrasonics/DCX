/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/SCAN.H_v   1.1   19 Mar 2015 07:01:38   sesharma  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/SCAN.H_v  $
 * 
 *    Rev 1.4.0   Feb 02 2022 07:00:00   RVITE
 * Added STARTFREQ20KSIG and STOPFREQ20KSIG definitions
 *
 *    Rev 1.1   19 Mar 2015 07:01:38   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.0   18 Aug 2014 09:17:28   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:04   amaurya
 * Initial revision.
 */

#ifndef SCAN_H_
#define SCAN_H_

#define SCANTIMEOUT      45000

/*---------------------------- Default Scan Preset values -------------------------------*/
// 20 KHZ
#define STARTFREQ20K 	194500
#define STOPFREQ20K 	204500
#define STARTFREQ20KSIG 198000
#define STOPFREQ20KSIG 	202500
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
