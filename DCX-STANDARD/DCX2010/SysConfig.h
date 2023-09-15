/*
$Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/SysConfig.h_v   1.13   16 Aug 2011 08:29:26   pdwivedi  $
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
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/SysConfig.h_v  $
 * 
 *    Rev 1.13   16 Aug 2011 08:29:26   pdwivedi
 * code cleanup
 * 
 *    Rev 1.12   05 Aug 2011 10:34:16   pdwivedi
 * Removed warning.
 * 
 *    Rev 1.11   15 Jul 2011 09:48:44   pdwivedi
 * Added changes for Team Track issue and Requirement changes.
 * 
 *    Rev 1.10   25 Apr 2011 08:45:06   PDwivedi
 * Changes after MAC chip addition
 * 
 *    Rev 1.9   25 Apr 2011 06:33:36   PDwivedi
 * Restore default should not modify the frquency,power and serial number
 * 
 *    Rev 1.8   14 Apr 2011 06:23:22   PDwivedi
 * Added changes for IP Setup issue and added comments.
 * 
 *    Rev 1.7   04 Apr 2011 11:08:58   PDwivedi
 * Increased serial number size in FRAM. Added  TODOs for later version.
 * 
 *    Rev 1.6   02 Mar 2011 08:35:20   PDwivedi
 * Removed Tar size from sysconfig structure, added serail number and placeholders
 * 
 *    Rev 1.5   14 Jan 2011 07:55:44   PDwivedi
 * Added changes for DCX bug and proposed changes.
 * 
 *    Rev 1.4   16 Dec 2010 08:17:42   PDwivedi
 * Added changes for Website Code Clean Up and Compressed file download from serial.
 * 
 *    Rev 1.3   14 Dec 2010 11:15:14   PDwivedi
 * Added changes for WebSite support.
 * 
 *    Rev 1.2   29 Nov 2010 07:47:40   PDwivedi
 * Added changes for Copyright comment,Verify preset , LCD support changes and  Flash CRC calculations.
 * 
 *    Rev 1.1   22 Nov 2010 09:04:26   PDwivedi
 * Added changes for code clean up, Software Digital tune,Digital output.
 * 
 *    Rev 1.0   29 Oct 2010 10:40:48   ygupta
 * Initial revision.

******************************************************************************/

#ifndef SYSCONFIG_H_
#define SYSCONFIG_H_
#include "Portable.h"
#include "CyGOS.h"

#define SYSTEMCONFIGVALIDCHECKSUM 0x13ADF8BA
#define DEFAULTIP 100,100,1,101
#define DEFAULTPOOLSTART 100,100,1,1
#define DEFAULTGATEWAY 0,0,0,0
#define DEFAULTIPSTATIC 192,168,10,100
#define DEFAULTGATEWAYSTATIC 192,168,10,1
#define DEFDHCPSERVERID_LEN  4
#define DEFAULTPOOLSIZE     64
#define DEFAULTLEASETIME   600
#define DEFAULTRENEWTIME   120
#define DEFAULTREBINDTIME  300

#define SIZE_SERAILNUMBER 24

#include "DhcpServer.h"

struct SystemConfiguration
{
	BYTE   CurrSystemType; //OEM or STD
	BYTE   CurrDisplay; // CUSTOM LCD or NONE
	UINT32 DCXCRC;//Flash CRC
	ip_addr systemIP;//IP address in case of DHCP server
	ip_addr systemIPStatic;//IP address in case of static configuration
	ip_addr netmask;//Net mask in case of static IP
	ip_addr gw;//gateway in case of static IP
	NetworkConfiguration currNetworkConfig;//current configuration
	DhcpServerConfiguration dhcpConfig;
	UINT32 systemConfigurationValidCheckSum;
	UINT8 dummyPlaceholder[17];//adjusted place holder
};

extern SystemConfiguration sysConfig;
#endif /* SYSCONFIG_H_ */
