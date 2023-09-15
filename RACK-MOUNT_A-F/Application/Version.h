/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/Version.h_v   1.13   12 Apr 2016 16:15:42   hasanchez  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/Version.h_v  $
 * 
 *    Rev 1.13   12 Apr 2016 16:15:42   hasanchez
 * Version changed to 6.9.3
 * 
 *    Rev 1.11   21 Mar 2016 16:47:20   hasanchez
 * Software version changed
 * 
 *    Rev 1.10   Mar 04 2016 13:56:44   Builder
 * Change version to V6.8.0 for official build
 * 
 *    Rev 1.9   18 Feb 2016 14:10:02   ygupta
 * version change for profibus power up fixes
 * 
 *    Rev 1.8   09 Feb 2016 16:27:02   tayars
 * version string change for power defaults fix and IP getting set to defaults fix
 * 
 *    Rev 1.6.1.5   03 Feb 2016 01:00:04   ygupta
 * Software version changed to 6.7.3.
 * 
 *    Rev 1.6.1.4   29 Jan 2016 08:59:18   ygupta
 * Software version changed to 6.7.2.
 * 
 *    Rev 1.6.1.3   28 Jan 2016 02:01:10   ygupta
 * Software version changed to 6.7.1.
 * 
 *    Rev 1.6.1.2   24 Dec 2015 02:41:38   sesharma
 * Software version changed to 6.7.0
 * 
 *    Rev 1.6.1.1   17 Nov 2015 07:41:28   sesharma
 * Software Version changed to 6.6.9.
 * 
 *    Rev 1.6.1.0   06 Nov 2015 07:03:20   sesharma
 * Version changed to 6.6.8.
 * 
 *    Rev 1.6   21 Aug 2015 05:52:08   RJamloki
 * Version changed to 6.6.7
 * 
 *    Rev 1.5   10 Aug 2015 09:42:10   RJamloki
 * changed version string from 6.6.5 to 6.6.6.
 * 
 *    Rev 1.4   24 Jul 2015 08:39:30   sesharma
 * Changed version string from 6.6.4 to 6.6.5.
 * 
 *    Rev 1.3   08 Jul 2015 02:43:00   sesharma
 * Changed version string to 6.6.4
 * 
 *    Rev 1.2   02 Jul 2015 10:03:04   RJamloki
 * Changed version string to 6.6.3.
 * 
 *    Rev 1.1   25 Jun 2015 05:51:48   rjamloki
 * Version string changed to 6.6.2
 * 
 *    Rev 1.0   09 Jun 2015 09:10:46   tayars
 * Initial revision.
 * 
 *    Rev 1.82.1.2   07 May 2015 13:49:34   gbutron
 * Version changed to 6.6.1
 * 
 *    Rev 1.82.1.1   04 Mar 2015 08:23:00   amaurya
 * Logging event on communication failure
 */

#include "portable.h"
#include "DEF_DEF.h"

//Current Version string of WC
const SINT8 SWVersionLabel[SWVERSION_LEN] = "6.9.4";
