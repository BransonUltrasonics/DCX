/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/SysConfig.h_v   1.1   19 Mar 2015 07:08:54   sesharma  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/SysConfig.h_v  $
 * 
 *    Rev 1.1   19 Mar 2015 07:08:54   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.0   18 Aug 2014 09:17:30   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:06   amaurya
 * Initial revision.
 */

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

struct SystemConfiguration {
	BYTE CurrDisplay; // CUSTOM LCD or NONE
	UINT32 DCXCRC; //Flash CRC
	ip_addr systemIP; //IP address in case of DHCP server
	ip_addr systemIPStatic; //IP address in case of static configuration
	ip_addr netmask; //Net mask in case of static IP
	ip_addr gw; //gateway in case of static IP
	NetworkConfiguration currNetworkConfig; //current configuration
	DhcpServerConfiguration dhcpConfig;
	UINT32 systemConfigurationValidCheckSum;
	UINT8 dummyPlaceholder[17]; //adjusted place holder
};

extern SystemConfiguration sysConfig;
#endif /* SYSCONFIG_H_ */
