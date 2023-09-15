/*
$Header:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/EthernetIP.h_v   1.8   13 Mar 2014 15:53:58   rjamloki  $
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
$Log:   D:/databases/VMdb/archives/DCX/Weld Controller/Application/EthernetIP.h_v  $
 * 
 *    Rev 1.8   13 Mar 2014 15:53:58   rjamloki
 * FRAM is accessed through pointers. Removed compiler allocation of FRAM
 * 
 *    Rev 1.7   Dec 13 2013 11:16:20   builder
 * Change major version to 2 for resubmitting to ODVA.
 * 
 *    Rev 1.6   02 Dec 2013 07:34:18   rjamloki
 * Changes for EthIP Conformance Test.
 * 
 *    Rev 1.5   08 Apr 2013 08:21:46   amaurya
 * Fixed Tracker issue of Ver2.0P.
 * 
 *    Rev 1.4   11 Mar 2013 02:23:48   ygupta
 * Issues Fixed & Requirement Changes
 * 
 *    Rev 1.3   18 Feb 2013 03:04:48   ygupta
 * FieldBus Changes
 * 
 *    Rev 1.2   22 Jan 2013 10:38:08   ygupta
 * Profibus Bug Fixes
 * 
 *    Rev 1.1   16 Jan 2013 06:24:22   ygupta
 * Level2 Code with Profibus
*/
#ifndef ETHERNETIP_H_
#define ETHERNETIP_H_

#include "FieldBus.h"
#include "portable.h"
#include "EthIP_public.h"
//Ethernet IP Standard Error codes for Explicit Messaging.
enum GeneralStatusCode
{
	STAT_SUCCESS = 0x00,
	STAT_PATH_DESTINATION_UNKNOWN = 0x05,
	STAT_SERVICE_NOT_SUPPORTED = 0x08,
	STAT_ATTRIBUTE_NOT_SETTABLE = 0x0E,
	STAT_VENDOR_SPECIFIC_ERROR = 0x1F,
	STAT_PRIVILEGE_VIOLATION = 0x0F,
	STAT_INVALID_ATTRIBUTE = 0x09,
	STAT_INVALID_PARAMETER_VALUE = 0x03,
};

//Ethernet IP Vendor specific Error codes for Explicit Messaging.
enum ExtendedStatusCode
{
	EXTSTAT_SUCCESS = 0x00,
	EXTSTAT_VAL_ERR_MISC = 0xFFFFFFFF,
};

//Currently Supported Services by WC Application
enum ServiceCode
{
	GET_ATTRIBUTE_SINGLE = 0x0E ,
	SET_ATTRIBUTE_SINGLE = 0x10,
//	SET_ATTRIBUTE_ALL = 0x02,
};

//Status of Explicit message processing is represented by the object of below structure
struct EthIPStausCode
{
	GeneralStatusCode GSC;
	ExtendedStatusCode ESC;
};

#define ETHERNETLINK_OBJECT_CLASS 0xF6
#define ETHERNETLINK_OBJECT_INSTANCE 1
#define ETHERNETLINK_OBJECT_INTERFACECONTROL_ATTRIBUTE 6
#define ENABLE_AUTONEGOTIATION BITPOS(0)
#define ENABLE_FULLDUPLEX BITPOS(1)

#define INTSPDCONTROL_CHECKSUM 0xA5B5A032
//TCPIP Interface config object definition
struct InterfaceSpeedControl
{
   UINT16 InterFaceControlBits;
   UINT16 InterfaceSpeed;
   UINT32 CheckSum;
};


//EthernetIP standard class and instance for TCP IP configuration through EthernetIP master
#define TCPIP_INTERFACEOBJECT_CLASS 0xF5
#define TCPIP_INTERFACEOBJECT_INSTANCE 1
//Attribute to configure the network configuration static,bootp or dhcp client
#define TCPIP_INTERFACEOBJECT_CONFIGCONTROL_ATTRIBUTE 3
#define CONFIGCONTROL_STATIC 0
#define CONFIGCONTROL_BOOTP 1
#define CONFIGCONTROL_DHCP 2
//Attribute to configure the IP, Netmask and gateway through master.
#define TCPIP_INTERFACEOBJECT_INTERFACECONFIG_ATTRIBUTE 5
//TCPIP Interface config object definition
struct TCPIPInterfaceConfig
{
   UINT32 IpAddr;
   UINT32 NM;
   UINT32 GW;
   UINT8 Reserved[];
};

#define TCPIP_INTERFACEOBJECT_ACDATTRIBUTE 11

//TCPIP interface attribute 11 definition
typedef struct ACD
{
	UINT8 AcdActivity;
	UINT8 RemoteMac[MAC_ADDRESS_LEN];
	struct ARPPdu
	{
		UINT16 Hwtype;
		UINT16 Proto;
		UINT16 HwlenProtolen;
		UINT16 Opcode;
		UINT8 Shwaddr[MAC_ADDRESS_LEN];
		UINT32 Sipaddr;
		UINT8 Dhwaddr[MAC_ADDRESS_LEN];;
		UINT32 Dipaddr;
	}ArpPDU;
}__attribute__ ((packed)) AddrConflictDetection;

#define NUM_QOS_ATTRIBUTES 8
#define FIRST_QOS_ATTRIBUTE 1
#define QOS_CLASS 0x48
#define QOS_INSTANCE 1
#define QOS_CHECKSUM 0xA5B56627

#define QOS_ATTR1_DEFAULT 0x00
#define QOS_ATTR2_DEFAULT 0x3B
#define QOS_ATTR3_DEFAULT 0x2F
#define QOS_ATTR4_DEFAULT 0x37
#define QOS_ATTR5_DEFAULT 0x2F
#define QOS_ATTR6_DEFAULT 0x2B
#define QOS_ATTR7_DEFAULT 0x1F
#define QOS_ATTR8_DEFAULT 0x1B

struct QOS
{
	UINT8 AttributeVals[NUM_QOS_ATTRIBUTES];
	UINT32 CheckSum;
};

struct EthIPNVData
{
	QOS QosVals;
	InterfaceSpeedControl IntControl;
	AddrConflictDetection LastAcd;
};

class EthernetIP: public FieldBus
{
public:
	EthernetIP();
	virtual void UpdateOutputCyclic(UINT8* OutPtr);
	virtual void UpdateInputCyclic(UINT8* InPtr);
	virtual BOOLEAN GetWarmStartParams(CIFX_PACKET * WSPkt);
	virtual void ProcessMailBox(CIFX_PACKET * MBXPkt);
	virtual void ReadExtendedStatus(CIFXHANDLE HChannel);
	void RegisterForMSNSChange(CIFXHANDLE ptChannel);
	void RegisterAssembly(CIFXHANDLE HChannel, UINT32 Instance, UINT32 Size, UINT32 DpmOffset, UINT32 AssemFlags);
	void RegisterService(CIFXHANDLE HChannel, UINT32 ServiceCode);
	void RegisterVendorSpecificClass(CIFXHANDLE HChannel, UINT32 VendorObjClass, BOOLEAN OnReset = FALSE);
	void ProtocolSpecificInit(CIFXHANDLE HChannel);
	void HandleResetServiceIndication(EIP_OBJECT_PACKET_RESET_IND_T * IndPkt);
	EthIPStausCode ProcessExplicitMsgRequest(UINT32 Class, UINT32 Service, UINT32 Instance, UINT32 Attribute,
			UINT8 * Data, UINT32 * DataLen);
	UINT32 CheckConnIndicationDetails(EIP_OBJECT_PACKET_CONNECTION_IND_T * ConnInd);
	EthIPStausCode EthIPStat;
	virtual void GetFaultData(UINT32 FaultCode = 0);
protected:
	CIFX_PACKET TempPkt;
	EIP_OBJECT_PACKET_CIP_OBJECT_CHANGE_IND_T * Pkt;
	EIP_OBJECT_PACKET_CIP_OBJECT_CHANGE_RES_T * ResPkt;
	EIP_OBJECT_PACKET_CONNECTION_IND_T * ConnPkt;
	EIP_OBJECT_PACKET_CONNECTION_RES_T * ConnResPkt;
	EIP_APS_PACKET_MS_NS_CHANGE_IND_T * MSNSPkt;
	EIP_APS_PACKET_MS_NS_CHANGE_RES_T * MSNSResPkt;
	EIP_OBJECT_PACKET_CL3_SERVICE_IND_T * Cl3Pkt;
	EIP_OBJECT_PACKET_CL3_SERVICE_RES_T * Cl3Res;
	EIP_OBJECT_PACKET_RESET_IND_T * RstInd;
	RCX_LINK_STATUS_CHANGE_IND_T * RcxLinkStatusChangeInd;
	RCX_LINK_STATUS_CHANGE_RES_T * RcxLinkStatusChangeRes;
	EIP_OBJECT_PACKET_CIP_SERVICE_CNF_T * CipServiceCnf;
	TCPIPInterfaceConfig * Config;
	InterfaceSpeedControl * IntConfig;
	AddrConflictDetection * AcdInfo;
	BOOLEAN ResetIndication;
	BOOLEAN ResetCompleted;
	BOOLEAN GotIPAfterReset;
};

//Branson DCX EthernetIP product's unique description
#define ETHIP_VENDORID 1283
#define ETHIP_PRODUCTNAME "DCX_FE"
#define ETHIP_MINORVERSION 1
#define ETHIP_MAJORVERSION 2
#define ETHIP_PRODUCTTYPE  43
#define ETHIP_PRODUCTCODE  2

//Vendor specific classes supported by DCX product
#define ETHIP_VENDORSPECIFIC_CLASS1 0x64 //Weld preset
#define ETHIP_VENDORSPECIFIC_CLASS1_MAXINSTANCE 32 //Weld preset
#define ETHIP_VENDORSPECIFIC_CLASS2 0x65 //Weld Status
#define ETHIP_VENDORSPECIFIC_CLASS2_MAXINSTANCE 32 //Weld Status
#define ETHIP_VENDORSPECIFIC_CLASS3 0x66 //Horn Preset
#define ETHIP_VENDORSPECIFIC_CLASS3_MAXINSTANCE 16 //Horn Preset
#define ETHIP_VENDORSPECIFIC_CLASS4 0x67 //Horn status
#define ETHIP_VENDORSPECIFIC_CLASS4_MAXINSTANCE 16 //Horn status
#define ETHIP_VENDORSPECIFIC_CLASS5 0x68 //Alarm object
#define ETHIP_VENDORSPECIFIC_CLASS5_MAXINSTANCE 1//Alarm object
#define ETHIP_VENDORSPECIFIC_CLASS6 0x69 //PS Supply data object
#define ETHIP_VENDORSPECIFIC_CLASS6_MAXINSTANCE 1//PS Supply data object

#define ETHIP_VENDORSPECIFIC_CLASS7 0x70 //Others
#define ETHIP_VENDORSPECIFIC_CLASS7_MAXINSTANCE 1 //Others

//all preset and status params values are returned as 4 byte quantity in little endian format
//Only exception is char array params(like serial number) which may have any length only restricted by their max size
#define ETHIP_PARAM_DATALEN 4

EthIPStausCode HandleValueEthIP(UINT32 Attribute, UINT32 InstanceNum, UINT8 Access, void * Value, UINT32 * Len);
EthIPStausCode WriteParamEthIP(BOOLEAN TokenAvailable, void * Val, UINT32 * Len, UINT32 InstanceNum, UINT32 Attribute);
EthIPStausCode ReadParamEthIP(void * Val, UINT32 * Len , UINT32 ArrayIndex, UINT32 Attribute);
extern EIP_OBJECT_PACKET_CL3_SERVICE_IND_T  LastCl3IndPkt;
extern EIP_OBJECT_PACKET_CL3_SERVICE_RES_T  LastCl3ResPkt;
extern UINT32 ServiceDataSizeIn, ServiceDataSizeOut;
extern EthIPNVData * EthipnvData;
#endif /* ETHERNETIP_H_ */
