/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/Version.h_v   1.82.1.2.2.0.1.0   28 Jul 2015 15:27:26   GButron  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/Version.h_v  $
 * 
 *    Rev 1.82.1.2.2.0.1.0   28 Jul 2015 15:27:26   GButron
 * Version string changed to V4.1
 * 
 *    Rev 1.82.1.2.2.0   Feb 25 2015 08:36:44   builder
 * Version 4.1a
 * 
 *    Rev 1.82.1.2   04 Jul 2014 07:21:48   amaurya
 * Changed version string Ver4.0R to V4.0
 * 
 *    Rev 1.82.1.1   02 Jul 2014 09:15:40   amaurya
 * Removed Amplitude Decay function.
 * 
 *    Rev 1.82.1.0   30 Jun 2014 10:04:34   amaurya
 * Removed Amplitude decay from digital I/O.
 * 
 *    Rev 1.82   13 Jun 2014 10:20:16   amaurya
 * Fixed manufacturing page.
 * 
 *    Rev 1.81   13 Jun 2014 08:46:20   amaurya
 * Fixed Temperature Overload Issue
 * 
 *    Rev 1.80   09 Jun 2014 08:42:48   amaurya
 * Changed version string from Ver4.0N to OfficialRelease_100-266-364_V4.0.
 * 
 *    Rev 1.79   05 Jun 2014 11:04:26   amaurya
 * Fixed DEF00692
 * 
 *    Rev 1.78   04 Jun 2014 07:53:30   amaurya
 * Fixed issue Digital Tune set to Zero if no Parallel Resonance is Found
 * 
 *    Rev 1.77   23 May 2014 09:59:16   amaurya
 * Code review fixes.
 * 
 *    Rev 1.76   30 Apr 2014 06:20:18   amaurya
 * Added Digital Tune and fixed DEF00638
 * 
 *    Rev 1.75   02 Apr 2014 05:04:24   rjamloki
 * Version change to 4.0J
 * 
 *    Rev 1.74   28 Mar 2014 04:34:56   rjamloki
 * Version Change to 4.0I
 * 
 *    Rev 1.73   12 Mar 2014 07:52:30   rjamloki
 * Sequencing Error Fix, Generic OL bit taken care, Time field in logs separated by colon.
 * 
 *    Rev 1.72   24 Jan 2014 06:48:10   rjamloki
 * Fixed User I/O Signal Sequence Error when Alarms are Generated.
 * 
 *    Rev 1.71   04 Dec 2013 06:57:12   rjamloki
 * Fixed PowerUpScan graph issue.
 * 
 *    Rev 1.70   19 Oct 2013 06:34:46   rjamloki
 * Memory issue fixed.
 * 
 *    Rev 1.69   10 Oct 2013 10:07:58   rjamloki
 * Fixed Temperature Overload issue at power down.
 * 
 *    Rev 1.68   10 Sep 2013 05:32:40   ygupta
 * Fixed bar graph on Seek & Weld Graphs tab.
 * 
 *    Rev 1.67   09 Sep 2013 08:59:56   rjamloki
 * Added Seek Frequency Offset, Amplitude ramping, set all graph defaults as in WC.
 * 
 *    Rev 1.66   05 Mar 2013 06:47:22   amaurya
 * Added ADC changes.
 * 
 *    Rev 1.65   01 Mar 2013 08:17:22   amaurya
 * Added Energy Range In 250J.
 * 
 *    Rev 1.64   12 Feb 2013 00:40:44   amaurya
 * Fixwd issue of resetting alarms under some conditions.
 * 
 *    Rev 1.63   07 Feb 2013 05:16:30   amaurya
 * Fixed PS Frequency issue on maufacturing page.
 * 
 *    Rev 1.62   05 Feb 2013 06:28:50   amaurya
 * Fixed Energy Special disable on power down issue.
 * 
 *    Rev 1.61   15 Jan 2013 04:43:26   amaurya
 * Energy Special.
 * 
 *    Rev 1.50.2.1.1.11   09 Nov 2012 01:21:14   amaurya
 * Fixed Bar Graph wrap issue.
 * 
 *    Rev 1.50.2.1.1.10   02 Nov 2012 09:15:38   amaurya
 * Fixed Graph plot start from 0 issue (in 0 to 100 ms)
 * 
 *    Rev 1.50.2.1.1.9   02 Nov 2012 09:04:20   amaurya
 * Fixed Graph start with 0 issue in 0 to 100ms.
 * 
 *    Rev 1.50.2.1.1.8   01 Nov 2012 05:15:34   amaurya
 * Added multiple resonance marker.
 * 
 *    Rev 1.50.2.1.1.7   23 Oct 2012 07:51:00   amaurya
 * Fixed Frequency Step to 1.
 * 
 *    Rev 1.50.2.1.1.6   18 Oct 2012 10:05:40   ygupta
 * Fixed Horn Scan Graph issue for 30 Khz.
 * 
 *    Rev 1.50.2.1.1.5   12 Oct 2012 10:05:30   ygupta
 * Fixed Reset Overload Clear Issue.
 * 
 *    Rev 1.50.2.1.1.4   09 Oct 2012 09:59:18   ygupta
 * Fixed Scaling Issue.
 * 
 *    Rev 1.50.2.1.1.3   09 Oct 2012 07:08:56   ygupta
 * Fixed scaling issue on 30 kHz horn scan.
 * 
 *    Rev 1.50.2.1.1.2   05 Oct 2012 07:35:48   ygupta
 * Added resonance marker in Horn Scan graph.
 * 
 *    Rev 1.50.2.1.1.1   07 Sep 2012 08:54:38   amaurya
 * Fixed resonance frequency issue of Horn Scan.
 * 
 *    Rev 1.50.2.1.1.0   23 Aug 2012 11:34:44   amaurya
 * Implemented S & V Graph Plot.
 * 
 *    Rev 1.50.2.1   30 Mar 2012 06:43:10   PDwivedi
 * Table driven CRC routine for all CRC calculations
 * 
 *    Rev 1.50.2.0   28 Mar 2012 11:14:08   PDwivedi
 * Added changes for simultaneous activation of digital inputs(like reset and memory clear) problem for correct operation.
 * 
 *    Rev 1.50   12 Dec 2011 11:15:04   tayars
 * Changed 10x and advanced preset min/max
 * 
 *    Rev 1.49   22 Nov 2011 11:41:24   Builder
 * Added the 10x multiplier in the frequency control loops.
 * 
 *    Rev 1.48   15 Nov 2011 04:34:12   RatneshA
 * Release V1.3 with NaN fix
 * 
 *    Rev 1.47   21 Oct 2011 11:05:02   Builder
 * Release V1.2 with Cable detect and inconsistency fixes
 * 
 *    Rev 1.46   17 Oct 2011 07:51:18   RatneshA
 * Modified version string to V1.11
 * 
 *    Rev 1.45   28 Sep 2011 14:06:24   Builder
 * Changed software version string to Ver1.1 for pilot release
 * 
 *    Rev 1.44   27 Sep 2011 06:20:50   RatneshA
 * Frequency out bug fix
 * 
 *    Rev 1.43   26 Sep 2011 03:34:20   RatneshA
 * Frequency Out fix for 30 and 40 Khz PS
 * 
 *    Rev 1.42   16 Sep 2011 08:06:22   RatneshA
 * Ver1.01T
 * 
 *    Rev 1.41   07 Sep 2011 17:21:32   ygupta
 * Ver1.01S
 * 
 *    Rev 1.40   06 Sep 2011 14:00:48   ygupta
 * Ver1.01R
 * 
 *    Rev 1.39   01 Sep 2011 13:06:22   ygupta
 * Added LED support for Watchdog and LCD communication.Removed CPU Restart function call and removed some pprintfs.
 * 
 *    Rev 1.38   29 Aug 2011 11:37:50   ygupta
 * Ver1.01P. Implemented Shut Down Process.
 * 
 *    Rev 1.37   28 Aug 2011 12:09:04   ygupta
 * Ver1.01N
 * 
 *    Rev 1.36   28 Aug 2011 11:42:26   ygupta
 * Ver1.01M
 * 
 *    Rev 1.35   16 Aug 2011 08:30:20   pdwivedi
 * code cleanup
 * 
 *    Rev 1.34   11 Aug 2011 08:57:24   pdwivedi
 * Added test button on manufacturing.
 * 
 *    Rev 1.33   05 Aug 2011 10:35:08   pdwivedi
 * Removed warning.
 * 
 *    Rev 1.32   03 Aug 2011 09:36:18   pdwivedi
 * Added changes for CRC Error.
 * 
 *    Rev 1.31   02 Aug 2011 10:54:18   pdwivedi
 * Added changes for Memory management and removing warnings.
 * 
 *    Rev 1.30   02 Aug 2011 07:40:02   pdwivedi
 * Added changes for PLL Loss lock, DCXManual and remving warnings.
 * 
 *    Rev 1.29   25 Jul 2011 11:32:42   tayars
 * Added New Control Loops
 * 
 *    Rev 1.28   18 Jul 2011 11:08:16   pdwivedi
 * Added changes for Special version with DCPTIME  2ms.
 * 
 *    Rev 1.25   15 Jul 2011 09:49:32   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.24   21 Jun 2011 11:07:54   PDwivedi
 * Fixes after mongle horde round1
 * 
 *    Rev 1.23   13 Jun 2011 09:20:08   PDwivedi
 * Version string modified
 * 
 *    Rev 1.22   24 May 2011 04:20:04   ygupta
 * Startup digital output fix
 * 
 *    Rev 1.21   09 May 2011 08:33:42   ygupta
 * modified version string
 * 
 *    Rev 1.19   25 Apr 2011 08:45:30   PDwivedi
 * Changes after MAC chip addition
 * 
 *    Rev 1.18   20 Apr 2011 11:38:28   ygupta
 * Added changes for tracker issues.
 * 
 *    Rev 1.17   19 Apr 2011 10:50:14   PDwivedi
 * Added changes for DCX Tracker Issue.
 * 
 *    Rev 1.16   14 Apr 2011 06:23:34   PDwivedi
 * Added changes for IP Setup issue and added comments.
 * 
 *    Rev 1.15   04 Apr 2011 10:59:26   PDwivedi
 * Modified version string to Ver1.0Q
 * 
 *    Rev 1.14   02 Mar 2011 08:18:42   PDwivedi
 * Ver1.0P
 * 
 *    Rev 1.13   14 Jan 2011 07:55:46   PDwivedi
 * Added changes for DCX bug and proposed changes.
 * 
 *    Rev 1.12   21 Dec 2010 05:56:04   PDwivedi
 * Added changes for Http Server Hang Problem.
 * 
 *    Rev 1.11   16 Dec 2010 08:17:42   PDwivedi
 * Added changes for Website Code Clean Up and Compressed file download from serial.
 * 
 *    Rev 1.10   14 Dec 2010 11:15:16   PDwivedi
 * Added changes for WebSite support.
 * 
 *    Rev 1.8   10 Dec 2010 09:25:14   ygupta
 * Ver1.0I, Changes to use Amplitude from LCD during current weld cycle.
 * 
 *    Rev 1.7   09 Dec 2010 07:54:28   PDwivedi
 * Added changes for preset and control Loop parameters according to original DUPS code in REV C board.
 * 
 *    Rev 1.6   08 Dec 2010 07:42:30   PDwivedi
 * Added changes for User Configurable Analog outputs.
 * 
 *    Rev 1.5   07 Dec 2010 08:58:22   PDwivedi
 * Added changes for REV Board C , code clean up.
 * 
 *    Rev 1.4   06 Dec 2010 09:56:56   PDwivedi
 * Added changes for Revision Board C support, LCD Key Timeout changes.
 * 
 *    Rev 1.3   29 Nov 2010 07:47:40   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.2   22 Nov 2010 09:04:28   PDwivedi
 * Added changes for code clean up, Software Digital tune,Digital output.
 * 
 *    Rev 1.1   15 Nov 2010 08:47:14   PDwivedi
 * Added comments and Scan Timeout changes.
 * 
 *    Rev 1.0   12 Nov 2010 10:34:28   nhahmed
 * Initial revision.
******************************************************************************/

#include "portable.h"
#include "DEF_DEF.h"

//Current Version string of DCP
const SINT8 SWVersionLabel[SWVERSION_LEN] = "V4.2";
