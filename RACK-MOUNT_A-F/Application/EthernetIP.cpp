/*
$Header:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/EthernetIP.cpp_v   1.7   29 Jan 2016 08:57:46   ygupta  $
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
$Log:   D:/databases/VMdb/archives/DCX/RackMount/A&F/WC/Application/EthernetIP.cpp_v  $
 * 
 *    Rev 1.7   29 Jan 2016 08:57:46   ygupta
 * CIP conformance issues fixed for CT Tool v12.
 * Type 1 Reset service returns a failure status stating this service is not supported by DUT.
 * TCP/IP interface object attribute 2 bit6 tell CT tool a device reset is expected after a configuration change is done on DUT.
 * Created a new function EthernetIP::SetAttribute() to set attributes on EIP stack firmware from host application.
 * 
 *    Rev 1.6   21 Aug 2015 05:49:32   RJamloki
 * Fixes for coverity CID 11190 and 12576
 * 
 *    Rev 1.5   10 Aug 2015 09:39:04   RJamloki
 * System Config parameters for clear memory options are added under class 0x70
 * 
 *    Rev 1.4   24 Jul 2015 08:35:16   sesharma
 * Created copy of service response in temporary packet in ProcessMailBox() to fix serial number issue in EthernetIP.
 * 
 *    Rev 1.3   02 Jul 2015 12:26:32   ygupta
 * Used Define for 0xAA5555AA in function ProcessMailBox and cleanedup Checkin comment history.
 * 
 *    Rev 1.2   02 Jul 2015 09:50:28   RJamloki
 * ProcessMailBox function fixed to use defines instead of hardcoded numbers.
 * 
 *    Rev 1.1   24 Jun 2015 08:35:08   amaurya
 * Added GetAttribute() to get slave IP.
 * 
 *    Rev 1.0   09 Jun 2015 09:10:36   tayars
 * Initial revision. Taken From WC Version Ver6.6.1.
 * 
 *    Rev 1.14   25 Aug 2014 10:10:04   amaurya
 * Added RegUpdateCmdFlag to update EtherNetIp register in Lcd
 *  
*/
#include "EthernetIP.h"
#include "string.h"
#include "cifXEndianess.h"
#include "cifxErrors.h"
#include "stdio.h"
#include "CpuDefs.h"
#include "rcX_User.h"
#include "FieldbusTask.h"
#include "TYP_DEF.h"
#include "GLOBALS.h"
#include "FUNC.h"
#include "StateTable.h"
#include "RTOS.h"

#define ETHIP_CONTROLSIGNAL_SIZE 4
#define ETHIP_ANALOGIO_SIZE_BYTE 2
#define ETHIP_NUM_ANALOG_IO 8
#define ETHIP_ANALOG_IO_EXCHANGESIZE_BYTE (ETHIP_ANALOGIO_SIZE_BYTE * ETHIP_NUM_ANALOG_IO)

#define pprintf printf
//This define makes sure that extended warm start parameters are going to Hilscher slave card.
//comment this define to send normal warm start parameters.
#define EXTENDED_WS

#define RESET_EMULATEPOWERCYCLE 0
#define RESET_FACTORYCONFIG 1
#define RESET_OUTOFBOXCONFIG 2

//For Adv Fieldbus Diagnostic purpose
EIP_OBJECT_PACKET_CL3_SERVICE_IND_T  LastCl3IndPkt;
EIP_OBJECT_PACKET_CL3_SERVICE_RES_T  LastCl3ResPkt;
UINT32 ServiceDataSizeIn, ServiceDataSizeOut;
extern BOOLEAN ACDInfoUpdated;
//These values are retrieved from card and should go into web page
TCPIPInterfaceConfig EthIPConfig;
extern BOOLEAN RegUpdateCmdFlag;
NetworkConfiguration FBCurrNetworkConfig;
/*   Purpose:
 * 		One time initialization of EthernetIP related things.
 *
 *   Entry condition:
 *
 *   Exit condition:
 *
 */
EthernetIP::EthernetIP()
{
	IODataExchangeSize = ETHIP_CONTROLSIGNAL_SIZE + ETHIP_ANALOG_IO_EXCHANGESIZE_BYTE;
	if(EthipnvData->IntControl.CheckSum != INTSPDCONTROL_CHECKSUM)
	{
		memset(&EthipnvData->IntControl, 0 , sizeof(EthipnvData->IntControl));
		EthipnvData->IntControl.InterFaceControlBits = (ENABLE_AUTONEGOTIATION |  ENABLE_FULLDUPLEX);
		EthipnvData->IntControl.CheckSum = INTSPDCONTROL_CHECKSUM;
	}
	if(EthipnvData->QosVals.CheckSum != QOS_CHECKSUM)
	{
		pprintf("\n Applying QOS defaults");
		EthipnvData->QosVals.AttributeVals[0] = QOS_ATTR1_DEFAULT;
		EthipnvData->QosVals.AttributeVals[1] = QOS_ATTR2_DEFAULT;
		EthipnvData->QosVals.AttributeVals[2] = QOS_ATTR3_DEFAULT;
		EthipnvData->QosVals.AttributeVals[3] = QOS_ATTR4_DEFAULT;
		EthipnvData->QosVals.AttributeVals[4] = QOS_ATTR5_DEFAULT;
		EthipnvData->QosVals.AttributeVals[5] = QOS_ATTR6_DEFAULT;
		EthipnvData->QosVals.AttributeVals[6] = QOS_ATTR7_DEFAULT;
		EthipnvData->QosVals.AttributeVals[7] = QOS_ATTR8_DEFAULT;
		EthipnvData->QosVals.CheckSum = QOS_CHECKSUM;
	}
	ResetCompleted = FALSE;
	ResetIndication = FALSE;
	GotIPAfterReset = FALSE;

	//coverity CID:11190
	memset(&TempPkt, 0, sizeof(TempPkt));
	memset(&EthIPStat, 0, sizeof(EthIPStat));
	Config = 0;
	Pkt = 0;
	RcxLinkStatusChangeRes = 0;
	AcdInfo = 0;
	ConnPkt = 0;
	MSNSResPkt = 0;
	Cl3Res = 0;
	CipServiceCnf = 0;
	MSNSPkt = 0;
	Cl3Pkt = 0;
	memset(&Cl3IndPkt, 0,  sizeof(Cl3IndPkt));
	RcxLinkStatusChangeInd = 0;
	IntConfig = 0;
	RstInd = 0;
	ConnResPkt = 0;
	ResPkt = 0;
}

/* void EthernetIP::UpdateInputCyclic(UINT8 * InPtr)
 *
 * Purpose:
 * To initialize input PZD area in global FieldbusIO object FbusIO and Analog Input values in global ExternalControl
 * object from the data received from EthernetIP master. This function is called by FieldBusTask::Run() function
 * every millisecond if the Hilscher EthernetIP slave card is detected at power up.
 *
 *  Entry condition:
 *       InPtr- Pointer to the cyclic data buffer recived from EthernetIP master
 *
 *  Exit condition:
 *       None:
 */
void EthernetIP::UpdateInputCyclic(UINT8* InPtr)
{
   UINT16 InVal, Index, *Ptr;
   Controlword1 = 0;
   Controlword2 = 0;
   if(InPtr != NULL){
	  //received values from Ethernet IP master are in always in little endian format and we are running
	  //this code on big endian system.First four bytes contain command words(two 16 bit values). The order of these statements
	  //shall be intact.
      FbusIO.PZDIn.PZDu8In.PZDIn[1] = *InPtr++;
      FbusIO.PZDIn.PZDu8In.PZDIn[0] = *InPtr++;
      FbusIO.PZDIn.PZDu8In.PZDIn[3] = *InPtr++;
      FbusIO.PZDIn.PZDu8In.PZDIn[2] = *InPtr++;
      Controlword1 = FbusIO.PZDIn.PZDu16In.STWIn1;
      Controlword2 = FbusIO.PZDIn.PZDu16In.STWIn2;

      Ptr = (UINT16*)&ExternalControl.ExtAmplitudeSet;//point to first analog in val
      //Analog value all little endian
      for(Index = 0; Index < ETHIP_NUM_ANALOG_IO; Index++){
         InVal = ((UINT16) (*InPtr++));
         InVal |= ((UINT16) (*InPtr++)) << 8;
         *Ptr++ = InVal;
      }
   }
   else{
//      if(CommIfcAccessToken == COMM_IFC_FBB)
//            ReleaseAccessToken(COMM_IFC_FBB);
      memset(&FbusIO.PZDIn.PZDu8In.PZDIn, '\0', sizeof(FbusIO.PZDIn.PZDu8In.PZDIn));
      Ptr = (UINT16*)&ExternalControl.ExtAmplitudeSet;//point to first analog in val
      //Analog value all little endian
      for(Index = 0; Index < ETHIP_NUM_ANALOG_IO; Index++)
        *Ptr++ = 0;

   }
   ProcessControlSignal();
}


/*  void EthernetIP::UpdateOutputCyclic(UINT8 * OutPtr)
 *
 *  Purpose:
 *     To initialize output PZD field in global FieldbusIO object FbusIO and analog output signal values in global
 *     ExternalControl object to be sent to EthernetIP master. This function is called by FieldBusTask::Run() function
 *     every millisecond if the Hilscher EthernetIP slave card is detected at power up.
 *
 *  Entry condition:
 *     OutPtr- Pointer to the cyclic data buffer to be sent to EthernetIP master
 *
 *  Exit condition: None:
 */
void EthernetIP::UpdateOutputCyclic(UINT8* OutPtr)
{
   UINT16 OutVal, Index, *Ptr;
	Statusword1 = 0;
	Statusword2 = 0;
	PrepareStatusSignal();
	if(OutPtr != NULL){
      Statusword1 = FbusIO.PZDOut.PZDu16Out.ZSWOut1;
      Statusword2 = FbusIO.PZDOut.PZDu16Out.ZSWOut2;
      /*Update PZD-area*/
      //The Ethernet IP master needs values in little endian format and we are running
      //this code on big endian system.First four bytes contain status words(two 16 bit values). The order of these statements
      //shall be intact.
      *OutPtr++ = FbusIO.PZDOut.PZDu8Out.PZDOut[1];
      *OutPtr++ = FbusIO.PZDOut.PZDu8Out.PZDOut[0];
      *OutPtr++ = FbusIO.PZDOut.PZDu8Out.PZDOut[3];
      *OutPtr++ = FbusIO.PZDOut.PZDu8Out.PZDOut[2];

      Ptr = (UINT16*)&ExternalControl.AmplitudeSet;//point to first analog out val
      //Analog output value. send all in little endian format.
      for(Index = 0; Index < ETHIP_NUM_ANALOG_IO; Index++){
         OutVal = *Ptr++;
         *OutPtr++ = (UINT8)(OutVal & 0xFF);//Lower byte first
         *OutPtr++ = (UINT8)((OutVal >> 8) & 0xFF);
      }
	}
}

/* BOOLEAN EthernetIP::GetWarmStartParams(CIFX_PACKET * WSPkt)
 *
 *  Purpose:
 *     To initialize the warm start packet which needs to be sent to Hilscher slave card at powerup.
 *     This function is called once by FieldBusTask::Run() function if the Hilscher EthernetIP slave card
 *     is detected at power up.
 *
 *  Entry condition:
 *     WSPkt- Pointer to the CIFX_PACKET where warm start parameters needs to be filled.
 *
 *  Exit condition:
 *     Returns TRUE if the warm start packet is supported else returns FALSE.
 */
BOOLEAN EthernetIP::GetWarmStartParams(CIFX_PACKET * WSPkt)
{
	UINT32 TcpIPFlags = 0;
	EIP_APS_PACKET_SET_CONFIGURATION_REQ_T * PtConfigReq;
	PtConfigReq = (EIP_APS_PACKET_SET_CONFIGURATION_REQ_T*) WSPkt;
	memset((void*) PtConfigReq, 0, sizeof(*PtConfigReq));

	EIP_APS_PACKET_SET_CONFIGURATIONEXT_REQ_T * PtConfigExt;
	PtConfigExt = (EIP_APS_PACKET_SET_CONFIGURATIONEXT_REQ_T*) WSPkt;
	memset((void*) PtConfigExt, 0, sizeof(*PtConfigExt));
	FBCurrNetworkConfig = Sysconfig->FBCurrNetworkConfig;
	if(Sysconfig->FBCurrNetworkConfig == configStatic){
		if(Sysconfig->FBSystemIP.addr != 0)
		   TcpIPFlags |= TCPIPFLAGS_IPAVAILABLE;
		if(Sysconfig->FBNetmask.addr != 0)
		   TcpIPFlags |= TCPIPFLAGS_NMAVAILABLE;
		if(Sysconfig->FBGw.addr != 0)
		   TcpIPFlags |= TCPIPFLAGS_GWAVAILABLE;
	}
	if((EthipnvData->IntControl.InterFaceControlBits & ENABLE_AUTONEGOTIATION) == ENABLE_AUTONEGOTIATION){
		TcpIPFlags |= TCPIPFLAGS_PORT1_ENABLEAUTONEGOTIATION;
	}
	else{
		if((EthipnvData->IntControl.InterFaceControlBits & ENABLE_FULLDUPLEX) == ENABLE_FULLDUPLEX)
			TcpIPFlags |= TCPIPFLAGS_PORT1_ENABLEFULLDUPLEX;
		else{
#ifdef FIELDBUS_DEBUG
			pprintf("\n Half duplex selected");
#endif
		}
		if(EthipnvData->IntControl.InterfaceSpeed == 100)
			TcpIPFlags |= TCPIPFLAGS_PORT1_ENABLEHUNDREDMBPS;
		else{
#ifdef FIELDBUS_DEBUG
			pprintf("\n 10 MBPS selected");
#endif
		}
	}
	if(Sysconfig->FBCurrNetworkConfig == configDhcpClient)
	   TcpIPFlags |= TCPIPFLAGS_CONFIGFROMDHCP;
	else if(Sysconfig->FBCurrNetworkConfig == configBootP)
	   TcpIPFlags |= TCPIPFLAGS_CONFIGFROMBOOTP;

	PtConfigReq->tHead.ulDest = HOST_TO_LE32(0x00000020);
	PtConfigReq->tHead.ulSrc = HOST_TO_LE32(0);
	PtConfigReq->tHead.ulCmd = HOST_TO_LE32(EIP_APS_SET_CONFIGURATION_REQ);
	PtConfigReq->tData.ulSystemFlags = HOST_TO_LE32(0);//EIP_APS_SET_CONFIGURATION_SYSFLAGS_APPCONTROLLED); //Autostart
	PtConfigReq->tData.ulWdgTime = HOST_TO_LE32(0);// ms
	PtConfigReq->tData.ulInputLen = HOST_TO_LE32(480);//length of input data instance 100
	PtConfigReq->tData.ulOutputLen = HOST_TO_LE32(480);//length of output data instance 101
	PtConfigReq->tData.ulTcpFlag = HOST_TO_LE32(TcpIPFlags);//static ip auto negotiation
	PtConfigReq->tData.ulIpAddr = HOST_TO_LE32(Sysconfig->FBSystemIP.addr);
	PtConfigReq->tData.ulNetMask = HOST_TO_LE32(Sysconfig->FBNetmask.addr);
	PtConfigReq->tData.ulGateway = HOST_TO_LE32(Sysconfig->FBGw.addr);
	PtConfigReq->tData.usVendId = HOST_TO_LE16(ETHIP_VENDORID);

	PtConfigReq->tData.usProductType = HOST_TO_LE16(ETHIP_PRODUCTTYPE);
	PtConfigReq->tData.usProductCode = HOST_TO_LE16(ETHIP_PRODUCTCODE);
	//PtConfigReq->tData.ulSerialNumber
	PtConfigReq->tData.bMinorRev = ETHIP_MINORVERSION;
	PtConfigReq->tData.bMajorRev = ETHIP_MAJORVERSION;
	PtConfigReq->tData.abDeviceName[0] = strlen(ETHIP_PRODUCTNAME);
	memcpy((char*) PtConfigReq->tData.abDeviceName + 1, ETHIP_PRODUCTNAME, strlen(ETHIP_PRODUCTNAME));

	PtConfigReq->tHead.ulLen = sizeof(EIP_APS_SET_CONFIGURATION_REQ_T);
//	printf("\n pkt size %d", PtConfigReq->tHead.ulLen);
	PtConfigReq->tHead.ulLen = HOST_TO_LE32(sizeof(EIP_APS_SET_CONFIGURATION_REQ_T));

#ifdef EXTENDED_WS
	//Extended
	PtConfigExt->tData.ulInputAssInstance = HOST_TO_LE32(100);
	PtConfigExt->tData.ulOutputAssInstance = HOST_TO_LE32(101);
//	PtConfigExt->tData.ulInputAssFlags = ASSEM_FIX_CONNECTION_SIZE;
//	PtConfigExt->tData.ulOutputAssFlags = ASSEM_FIX_CONNECTION_SIZE;
	PtConfigExt->tData.tQoS_Config.ulQoSFlags = HOST_TO_LE32(EIP_OBJECT_QOS_FLAGS_ENABLE);

	PtConfigExt->tData.tQoS_Config.bTag802Enable = EthipnvData->QosVals.AttributeVals[0];
	PtConfigExt->tData.tQoS_Config.bDSCP_PTP_Event = EthipnvData->QosVals.AttributeVals[1];
	PtConfigExt->tData.tQoS_Config.bDSCP_PTP_General = EthipnvData->QosVals.AttributeVals[2];
	PtConfigExt->tData.tQoS_Config.bDSCP_Urgent =  EthipnvData->QosVals.AttributeVals[3];
	PtConfigExt->tData.tQoS_Config.bDSCP_Scheduled = EthipnvData->QosVals.AttributeVals[4];
	PtConfigExt->tData.tQoS_Config.bDSCP_High =  EthipnvData->QosVals.AttributeVals[5];
	PtConfigExt->tData.tQoS_Config.bDSCP_Low = EthipnvData->QosVals.AttributeVals[6];
	PtConfigExt->tData.tQoS_Config.bDSCP_Explicit = EthipnvData->QosVals.AttributeVals[7];
	PtConfigExt->tData.ulNameServer = HOST_TO_LE32(MakeIP(0, 0 , 0, 0));
	PtConfigExt->tData.ulNameServer_2 = HOST_TO_LE32(MakeIP(0, 0 , 0, 0));
	PtConfigExt->tData.bSelectAcd = (1);
	PtConfigReq->tHead.ulLen = sizeof(EIP_APS_SET_CONFIGURATIONEXT_REQ_T);
//	printf("\n pkt size %d", PtConfigReq->tHead.ulLen);
	PtConfigReq->tHead.ulLen = HOST_TO_LE32(sizeof(EIP_APS_SET_CONFIGURATIONEXT_REQ_T));
#endif
	return TRUE;
}


/* void CheckConnIndicationDetails(EIP_OBJECT_PACKET_CONNECTION_IND_T * ConnInd)
 *
 * Purpose:
 *    To check the connection indication.
 *
 * Entry condition:
 *    ConnInd- Pointer to connection indication packet
 *
 * Exit condition:
 *    The Error code.
 */
UINT32 EthernetIP::CheckConnIndicationDetails(EIP_OBJECT_PACKET_CONNECTION_IND_T * ConnInd)
{
#ifdef FIELDBUS_DEBUG
	#define CON32(x) LE32_TO_HOST((x))
	#define CON16(x) LE16_TO_HOST((x))
	pprintf("\n conn indication state %08X %08X %08X %08X %08X %08X %08X "
			" %08X "
			" Cls %08X Inst %08X OTConnPoint %08X TOConnPoint %08X"
			" ExtState %08X"
			" ProConId %08X ConConId %08X ConSN %08X OrigVID %04X OrigSN %08X usPro %08X ProParam %04X "
			"usCon %08X ConParam %04X tmmul %02X",
			CON32(ConnInd->tData.ulConnectionState), CON32(ConnInd->tData.ulConnectionCount), CON32(ConnInd->tData.ulOutConnectionCount),
			CON32(ConnInd->tData.ulConfiguredCount), CON32(ConnInd->tData.ulActiveCount), CON32(ConnInd->tData.ulDiagnosticCount),
			CON32(ConnInd->tData.ulOrginator),
			CON32(ConnInd->tData.tConnection.tOTConnection.ulConnHandle),

			CON32(ConnInd->tData.tConnection.tTOConnection.ulClass), CON32(ConnInd->tData.tConnection.tTOConnection.ulInstance),
			CON32(ConnInd->tData.tConnection.tTOConnection.ulOTConnPoint),CON32(ConnInd->tData.tConnection.tTOConnection.ulTOConnPoint),

			CON32(ConnInd->tData.ulExtendedState),

			CON32(ConnInd->tData.tExtInfo.ulProConnId), CON32(ConnInd->tData.tExtInfo.ulConConnId),
			CON32(ConnInd->tData.tExtInfo.ulConnSerialNum), CON16(ConnInd->tData.tExtInfo.usOrigVendorId),
			CON32(ConnInd->tData.tExtInfo.ulOrigDeviceSn),
			CON32(ConnInd->tData.tExtInfo.ulProApi),CON16(ConnInd->tData.tExtInfo.usProConnParams),
			CON32(ConnInd->tData.tExtInfo.ulConApi), CON16(ConnInd->tData.tExtInfo.usConConnParams),
			ConnInd->tData.tExtInfo.bTimeoutMultiplier);
#endif

	return 0;
}
/* void EthernetIP::ProcessMailBox(CIFX_PACKET * MBXPkt)
 *
 * Purpose:
 *    To process the indication from Hilsher slave card. Explicit message indication is handled by this function.
 *    This function is called by FieldBusTask::Run() function when an indication is received from Hilscher slave card.
 *
 * Entry condition:
 *    MBXPkt- Pointer to the CIFX_PACKET which contains indication data deatils.
 *
 * Exit condition:
 *    None.
 */
void EthernetIP::ProcessMailBox(CIFX_PACKET * MBXPkt)
{
	BOOLEAN RcvdTcpIpIntrfcCfg = FALSE;
	UINT32 ResetServiceStatus = HOST_TO_LE32(0x21);	//set to RCX_E_FAIL
	UINT32 ServiceDataSize = 0;
	UINT32 ServiceData = 0;
#ifdef FIELDBUS_DEBUG
	UINT32 i = 0;
#endif
	UINT32 ConnReply;
	CIFX_PACKET * CifxPkt = MBXPkt;
	BOOLEAN AskForIP = FALSE;
	/*------------------------------*/
	/* Process receive packets      */
	/*------------------------------*/
	/* Check if we have a command or answer packet */
	if ((LE32_TO_HOST(MBXPkt->tHeader.ulCmd) & CIFX_MSK_PACKET_ANSWER) == 0) {
#ifdef FIELDBUS_DEBUG
		pprintf("\n received IND %08X code %08X",   LE32_TO_HOST(MBXPkt->tHeader.ulCmd),
				LE32_TO_HOST(MBXPkt->tHeader.ulState));
#endif
		/*-------------------------------------*/
		/* this is a commad / indication packet*/
		/*-------------------------------------*/
		switch (LE32_TO_HOST(MBXPkt->tHeader.ulCmd)) {

		case EIP_OBJECT_CIP_OBJECT_CHANGE_IND:
			Pkt = (EIP_OBJECT_PACKET_CIP_OBJECT_CHANGE_IND_T*) MBXPkt;
#ifdef FIELDBUS_DEBUG
			pprintf("\n %X %X %X %X %X", LE32_TO_HOST(Pkt->tData.ulAttribute), LE32_TO_HOST(Pkt->tData.ulClass),
			      LE32_TO_HOST(Pkt->tData.ulInfoFlags), LE32_TO_HOST(Pkt->tData.ulInstance),
			      LE32_TO_HOST(Pkt->tData.ulService));
			pprintf("\n service data");
			for (i = 0; i < LE32_TO_HOST(MBXPkt->tHeader.ulLen) - EIP_OBJECT_CIP_OBJECT_CHANGE_IND_SIZE; i++) {
				pprintf("%02X", Pkt->tData.abData[i]);
			}
#endif
			if((LE32_TO_HOST(Pkt->tData.ulClass) == TCPIP_INTERFACEOBJECT_CLASS) &&
			      (LE32_TO_HOST(Pkt->tData.ulInstance) == TCPIP_INTERFACEOBJECT_INSTANCE)){
			   if((LE32_TO_HOST(Pkt->tData.ulService) == SET_ATTRIBUTE_SINGLE)&& (LE32_TO_HOST(Pkt->tData.ulAttribute) ==
			         TCPIP_INTERFACEOBJECT_CONFIGCONTROL_ATTRIBUTE)){
			      ServiceData = *(((UINT32*)Pkt->tData.abData));
			      ServiceData = LE32_TO_HOST(ServiceData) & 0x0000000F;
			      if(ServiceData == CONFIGCONTROL_STATIC)
			         Sysconfig->FBCurrNetworkConfig = configStatic;
			      else if(ServiceData == CONFIGCONTROL_BOOTP)
			         Sysconfig->FBCurrNetworkConfig = configBootP;
			      else if(ServiceData == CONFIGCONTROL_DHCP)
			         Sysconfig->FBCurrNetworkConfig = configDhcpClient;

			      if((FBCurrNetworkConfig != Sysconfig->FBCurrNetworkConfig)
			    		  && (Sysconfig->FBCurrNetworkConfig == configStatic))
			      {
					EthIPConfig.IpAddr = 0;
					EthIPConfig.NM = 0;
					EthIPConfig.GW = 0;
					ResetIndication = TRUE;
			      }
			      FBCurrNetworkConfig = Sysconfig->FBCurrNetworkConfig;
			   }
			   if((LE32_TO_HOST(Pkt->tData.ulService) == SET_ATTRIBUTE_SINGLE)&& (LE32_TO_HOST(Pkt->tData.ulAttribute) ==
			         TCPIP_INTERFACEOBJECT_INTERFACECONFIG_ATTRIBUTE)){

				  RcvdTcpIpIntrfcCfg = TRUE;

				  if(ResetCompleted)
				  {
					   GotIPAfterReset = TRUE;
					   RcvdTcpIpIntrfcCfg = FALSE;
				  }
			      Config = (TCPIPInterfaceConfig*)Pkt->tData.abData;
			      EthIPConfig.IpAddr = LE32_TO_HOST(Config->IpAddr);
			      EthIPConfig.NM = LE32_TO_HOST(Config->NM);
			      EthIPConfig.GW = LE32_TO_HOST(Config->GW);
			      if(Sysconfig->FBCurrNetworkConfig == configStatic){//Change these values only in case of static configuration
					  Sysconfig->FBSystemIP = EthIPConfig.IpAddr;
					  Sysconfig->FBNetmask = EthIPConfig.NM;
					  Sysconfig->FBGw = EthIPConfig.GW;
			      }

			      RegUpdateCmdFlag = TRUE;
			   }
			   if((LE32_TO_HOST(Pkt->tData.ulService) == SET_ATTRIBUTE_SINGLE)&& (LE32_TO_HOST(Pkt->tData.ulAttribute) ==
					   TCPIP_INTERFACEOBJECT_ACDATTRIBUTE)){
				   AcdInfo = (AddrConflictDetection *)Pkt->tData.abData;
				   EthipnvData->LastAcd = * AcdInfo;
				   ACDInfoUpdated = TRUE;
#ifdef FIELDBUS_DEBUG
				   pprintf("\n Acd Info updated on indication");
#endif
			   }
			}
			else if((LE32_TO_HOST(Pkt->tData.ulClass) == ETHERNETLINK_OBJECT_CLASS) &&
			      (LE32_TO_HOST(Pkt->tData.ulInstance) == ETHERNETLINK_OBJECT_INSTANCE))
			{
				if((LE32_TO_HOST(Pkt->tData.ulService) == SET_ATTRIBUTE_SINGLE)&& (LE32_TO_HOST(Pkt->tData.ulAttribute) ==
						ETHERNETLINK_OBJECT_INTERFACECONTROL_ATTRIBUTE)){
					IntConfig = (InterfaceSpeedControl*)Pkt->tData.abData;
					EthipnvData->IntControl.InterFaceControlBits = LE16_TO_HOST(IntConfig->InterFaceControlBits);
					EthipnvData->IntControl.InterfaceSpeed = LE16_TO_HOST(IntConfig->InterfaceSpeed);
#ifdef FIELDBUS_DEBUG
					pprintf("\n flags %d speed %d", EthipnvData->IntControl.InterFaceControlBits, EthipnvData->IntControl.InterfaceSpeed);
#endif
				}
			}
			else if((LE32_TO_HOST(Pkt->tData.ulClass) == QOS_CLASS) &&
						      (LE32_TO_HOST(Pkt->tData.ulInstance) == QOS_INSTANCE))
			{
				if((LE32_TO_HOST(Pkt->tData.ulService) == SET_ATTRIBUTE_SINGLE)&&
						(LE32_TO_HOST(Pkt->tData.ulAttribute) >= FIRST_QOS_ATTRIBUTE) &&
						(LE32_TO_HOST(Pkt->tData.ulAttribute) <= NUM_QOS_ATTRIBUTES)){
#ifdef FIELDBUS_DEBUG
					pprintf("\n QOS Attribute %d changed to %02X", LE32_TO_HOST(Pkt->tData.ulAttribute), Pkt->tData.abData[0]);
#endif
					EthipnvData->QosVals.AttributeVals[LE32_TO_HOST(Pkt->tData.ulAttribute) - 1] =
							Pkt->tData.abData[0];
				}
			}
#ifdef FIELDBUS_DEBUG
			else
			{
				pprintf("\n not done anything");
			}
#endif
			ResPkt = (EIP_OBJECT_PACKET_CIP_OBJECT_CHANGE_RES_T *)MBXPkt;
			ResPkt->tHead.ulLen = HOST_TO_LE32(0);
			ResPkt->tHead.ulCmd = HOST_TO_LE32(LE32_TO_HOST(MBXPkt->tHeader.ulCmd) + 1);
			break;
		case EIP_OBJECT_CONNECTION_IND:
			ConnPkt = (EIP_OBJECT_PACKET_CONNECTION_IND_T *) MBXPkt;
			ConnReply = CheckConnIndicationDetails(ConnPkt);

			ConnResPkt = (EIP_OBJECT_PACKET_CONNECTION_RES_T *)MBXPkt;
			ConnResPkt->tHead.ulLen = HOST_TO_LE32(0);
			ConnResPkt->tHead.ulCmd = HOST_TO_LE32(LE32_TO_HOST(MBXPkt->tHeader.ulCmd) + 1);
			ConnResPkt->tHead.ulSta = HOST_TO_LE32(ConnReply);
			break;
		case EIP_APS_MS_NS_CHANGE_IND:
			MSNSPkt = (EIP_APS_PACKET_MS_NS_CHANGE_IND_T *) MBXPkt;
#ifdef FIELDBUS_DEBUG
			printf("\n Module and network status %08X %08X", LE32_TO_HOST(MSNSPkt->tData.ulModuleStatus),
			      LE32_TO_HOST(MSNSPkt->tData.ulNetworkStatus));
#endif
			MSNSResPkt = (EIP_APS_PACKET_MS_NS_CHANGE_RES_T *)MBXPkt;
			MSNSResPkt->tHead.ulLen = HOST_TO_LE32(0);
			MSNSResPkt->tHead.ulCmd = HOST_TO_LE32(LE32_TO_HOST(MBXPkt->tHeader.ulCmd) + 1);
			if(((LE32_TO_HOST(MSNSPkt->tData.ulModuleStatus) == EIP_MS_STANDBY) || (LE32_TO_HOST(MSNSPkt->tData.ulModuleStatus) == EIP_MS_OPERATE))
				      && ((LE32_TO_HOST(MSNSPkt->tData.ulNetworkStatus) == EIP_NS_NO_CONNECTION) || (LE32_TO_HOST(MSNSPkt->tData.ulNetworkStatus) == EIP_NS_CONNECTED)))
			{
				AskIPTimer = ASKIP_TIMER;
			}
			break;
		case EIP_OBJECT_CL3_SERVICE_IND:
			Cl3Pkt = (EIP_OBJECT_PACKET_CL3_SERVICE_IND_T*) MBXPkt;
			LastCl3IndPkt = *Cl3Pkt;
#ifdef FIELDBUS_DEBUG
			printf("\n EIP_OBJECT_CL3_SERVICE_IND %X %X %X %X %X", LE32_TO_HOST(Cl3Pkt->tData.ulAttribute),
			      LE32_TO_HOST(Cl3Pkt->tData.ulConnectionId), LE32_TO_HOST(Cl3Pkt->tData.ulInstance),
			      LE32_TO_HOST(Cl3Pkt->tData.ulObject), LE32_TO_HOST(Cl3Pkt->tData.ulService));
			printf("\n  data %d %d \n", LE32_TO_HOST(Cl3Pkt->tHead.ulLen), sizeof(EIP_OBJECT_CL3_SERVICE_IND_T));
			for (i = 0; i < (LE32_TO_HOST(Cl3Pkt->tHead.ulLen) - (sizeof(EIP_OBJECT_CL3_SERVICE_IND_T)
					- MAX_SERVICEDATA_LEN)); i++) {
				printf("%02X", Cl3Pkt->tData.abData[i]);
			}
#endif
			ServiceDataSize = (LE32_TO_HOST(Cl3Pkt->tHead.ulLen) - (sizeof(EIP_OBJECT_CL3_SERVICE_IND_T)
					- MAX_SERVICEDATA_LEN));
			ServiceDataSizeIn = ServiceDataSize;
			// Prepare the response header
			Cl3Res = (EIP_OBJECT_PACKET_CL3_SERVICE_RES_T *)MBXPkt;
			Cl3Res->tHead.ulCmd = HOST_TO_LE32(LE32_TO_HOST(MBXPkt->tHeader.ulCmd) + 1);
			// Get service response
			EthIPStat = ProcessExplicitMsgRequest(LE32_TO_HOST(Cl3Pkt->tData.ulObject), LE32_TO_HOST(Cl3Pkt->tData.ulService), LE32_TO_HOST(Cl3Pkt->tData.ulInstance), LE32_TO_HOST(Cl3Pkt->tData.ulAttribute),
					Cl3Pkt->tData.abData, &ServiceDataSize);
			Cl3IndPkt = *Cl3Pkt;		// Copy the service response in temporary packet.

			ServiceDataSizeOut = ServiceDataSize;
			if(ServiceDataSize > MAX_SERVICEDATA_LEN){
				ServiceDataSize = MAX_SERVICEDATA_LEN;
			}
			// Prepare the response data.
     		memcpy(Cl3Res->tData.abData, Cl3IndPkt.tData.abData, ServiceDataSize);
			Cl3Res->tHead.ulLen = HOST_TO_LE32(sizeof(EIP_OBJECT_CL3_SERVICE_RES_T) - MAX_SERVICEDATA_LEN + ServiceDataSize);
			Cl3Res->tData.ulAttribute = Cl3IndPkt.tData.ulAttribute;
			Cl3Res->tData.ulConnectionId = Cl3IndPkt.tData.ulConnectionId;
			Cl3Res->tData.ulGRC = HOST_TO_LE32(EthIPStat.GSC);
			Cl3Res->tData.ulERC = HOST_TO_LE32(EthIPStat.ESC);
			Cl3Res->tData.ulInstance = Cl3IndPkt.tData.ulInstance;
			Cl3Res->tData.ulObject = Cl3IndPkt.tData.ulObject;
			Cl3Res->tData.ulService = Cl3IndPkt.tData.ulService;
			LastCl3ResPkt = *Cl3Res;
			break;
		case EIP_OBJECT_RESET_IND:
			RstInd = (EIP_OBJECT_PACKET_RESET_IND_T *) MBXPkt;

			if(LE32_TO_HOST(RstInd->Data.ulResetType) == 0) {
#ifdef FIELDBUS_DEBUG
			pprintf("\n reset type %08X", LE32_TO_HOST(RstInd->Data.ulResetType));
#endif
				ConnResPkt = (EIP_OBJECT_PACKET_CONNECTION_RES_T *)MBXPkt;
				ConnResPkt->tHead.ulLen = HOST_TO_LE32(0);
				ConnResPkt->tHead.ulCmd = HOST_TO_LE32(LE32_TO_HOST(MBXPkt->tHeader.ulCmd) + 1);
				EthIPConfig.IpAddr = 0;
				EthIPConfig.NM = 0;
				EthIPConfig.GW = 0;
				ResetIndication = TRUE;
			}
			else {
				ConnResPkt = (EIP_OBJECT_PACKET_CONNECTION_RES_T *)MBXPkt;
				ConnResPkt->tHead.ulLen = HOST_TO_LE32(0);
				ConnResPkt->tHead.ulCmd = HOST_TO_LE32(LE32_TO_HOST(MBXPkt->tHeader.ulCmd) + 1);
				ConnResPkt->tHead.ulSta = HOST_TO_LE32(RCX_E_FAIL);
			}
			break;
		case RCX_LINK_STATUS_CHANGE_IND:
			RcxLinkStatusChangeInd = (RCX_LINK_STATUS_CHANGE_IND_T*) MBXPkt;
#ifdef FIELDBUS_DEBUG
			for (i = 0;i < 2; i++){
				pprintf("\n link status port %d duplex %d link %d speed %d",
						LE32_TO_HOST(RcxLinkStatusChangeInd->tData.atLinkData[i].ulPort),
						LE32_TO_HOST(RcxLinkStatusChangeInd->tData.atLinkData[i].fIsFullDuplex),
						LE32_TO_HOST(RcxLinkStatusChangeInd->tData.atLinkData[i].fIsLinkUp),
						LE32_TO_HOST(RcxLinkStatusChangeInd->tData.atLinkData[i].ulSpeed));
			}
#endif
			RcxLinkStatusChangeRes = (RCX_LINK_STATUS_CHANGE_RES_T *)MBXPkt;
			RcxLinkStatusChangeRes->tHead.ulLen = HOST_TO_LE32(0);
			RcxLinkStatusChangeRes->tHead.ulCmd = HOST_TO_LE32(LE32_TO_HOST(MBXPkt->tHeader.ulCmd) + 1);

			if((LE32_TO_HOST(RcxLinkStatusChangeInd->tData.atLinkData[0].fIsLinkUp) == 0)//coverity CID:12576
			 && (LE32_TO_HOST(RcxLinkStatusChangeInd->tData.atLinkData[1].fIsLinkUp) == 0))
			{
				EthIPConfig.IpAddr = LE32_TO_HOST(0);
				EthIPConfig.NM = LE32_TO_HOST(0);
				EthIPConfig.GW = LE32_TO_HOST(0);
			}
			else
				AskIPTimer = ASKIP_TIMER;

			break;
		case EIP_OBJECT_FAULT_IND:
#ifdef FIELDBUS_DEBUG
			pprintf("\n Got fault %08X", LE32_TO_HOST(MBXPkt->tHeader.ulCmd));
#endif
			CifxPkt->tHeader.ulLen = HOST_TO_LE32(0x00000000);
			CifxPkt->tHeader.ulState = HOST_TO_LE32(0x00000000);
			CifxPkt->tHeader.ulCmd = HOST_TO_LE32(LE32_TO_HOST(MBXPkt->tHeader.ulCmd) + 1);
			break;
		case SLAVEIP_CONFIGURED_INDICATION:
			AskForIP = TRUE;
			break;
		default:
#ifdef FIELDBUS_DEBUG
			pprintf("\n Un handled IND %08X with err code %08X", LE32_TO_HOST(MBXPkt->tHeader.ulCmd),
			      LE32_TO_HOST(MBXPkt->tHeader.ulState));
#endif

			CifxPkt->tHeader.ulLen = HOST_TO_LE32(0x00000000);
			CifxPkt->tHeader.ulState = HOST_TO_LE32(RCX_E_UNEXPECTED);
			CifxPkt->tHeader.ulCmd = HOST_TO_LE32(LE32_TO_HOST(MBXPkt->tHeader.ulCmd) + 1);//HOST_TO_LE32(MBXPkt->tHeader.ulCmd) | 1;

		}
		if ((AskForIP == FALSE) && FieldbusTask::thisPtr->CifxPkt.Write(0, *CifxPkt, 0) == FALSE){
#ifdef FIELDBUS_DEBUG
				printf("Error sending Response to Queue!\r\n");
#endif
		}

		if(RcvdTcpIpIntrfcCfg == TRUE)
		{
			SetAttribute(TCPIP_INTERFACEOBJECT_CLASS, TCPIP_INTERFACEOBJECT_INSTANCE, TCPIP_INTERFACEOBJECT_STATUS_ATTRIBUTE, ResetServiceStatus, NULL, TRUE);
			RcvdTcpIpIntrfcCfg = FALSE;
		}
		else if(ResetIndication == TRUE){
			//MBXPkt Data part is still unchanged
#ifdef FIELDBUS_DEBUG
			pprintf("\n Handle reset");
#endif
			HandleResetServiceIndication(RstInd);
			ResetIndication = FALSE;
		}
		else if(ResetCompleted && GotIPAfterReset){
			ResetCompleted = FALSE;
			GotIPAfterReset = FALSE;
			CIFXHANDLE HChannel = 0;
			RegisterVendorSpecificClass(HChannel, ETHIP_VENDORSPECIFIC_CLASS1 , TRUE);
			RegisterVendorSpecificClass(HChannel, ETHIP_VENDORSPECIFIC_CLASS2, TRUE);
			RegisterVendorSpecificClass(HChannel, ETHIP_VENDORSPECIFIC_CLASS3, TRUE);
			RegisterVendorSpecificClass(HChannel, ETHIP_VENDORSPECIFIC_CLASS4, TRUE);
			RegisterVendorSpecificClass(HChannel, ETHIP_VENDORSPECIFIC_CLASS5, TRUE);
			RegisterVendorSpecificClass(HChannel, ETHIP_VENDORSPECIFIC_CLASS6, TRUE);
			RegisterVendorSpecificClass(HChannel, ETHIP_VENDORSPECIFIC_CLASS7, TRUE);
		}
		else if(AskForIP){
			GetAttribute(TCPIP_INTERFACEOBJECT_CLASS, TCPIP_INTERFACEOBJECT_INSTANCE, TCPIP_INTERFACEOBJECT_INTERFACECONFIG_ATTRIBUTE);
		}
	} else {
#ifdef FIELDBUS_DEBUG
		pprintf("\n Received CNF %08X with err code %08X", LE32_TO_HOST(MBXPkt->tHeader.ulCmd),
		      LE32_TO_HOST(MBXPkt->tHeader.ulState));
#endif
			switch(LE32_TO_HOST(MBXPkt->tHeader.ulCmd))
			{
				case EIP_OBJECT_CIP_SERVICE_CNF:
					   CipServiceCnf = (EIP_OBJECT_PACKET_CIP_SERVICE_CNF_T *)MBXPkt;
#ifdef FIELDBUS_DEBUG
					   pprintf("\n %d %d %d %d %08X %08X", LE32_TO_HOST(CipServiceCnf->tData.ulClass),
							   LE32_TO_HOST(CipServiceCnf->tData.ulInstance), LE32_TO_HOST(CipServiceCnf->tData.ulAttribute),
							   LE32_TO_HOST(CipServiceCnf->tData.ulService) ,
							    LE32_TO_HOST(CipServiceCnf->tData.ulGRC),
							   LE32_TO_HOST(CipServiceCnf->tData.ulERC));
#endif

					   if((LE32_TO_HOST(CipServiceCnf->tData.ulClass) == TCPIP_INTERFACEOBJECT_CLASS) && (LE32_TO_HOST(CipServiceCnf->tData.ulInstance) == TCPIP_INTERFACEOBJECT_INSTANCE)
							   && (LE32_TO_HOST(CipServiceCnf->tData.ulAttribute) == TCPIP_INTERFACEOBJECT_ACDATTRIBUTE) && (LE32_TO_HOST(CipServiceCnf->tData.ulService) == GET_ATTRIBUTE_SINGLE)
							   && ((LE32_TO_HOST(CipServiceCnf->tData.ulGRC) == STAT_SUCCESS)))
					   {
								   AcdInfo = (ACD *)CipServiceCnf->tData.abData;
								   EthipnvData->LastAcd = * AcdInfo;
								   ACDInfoUpdated = TRUE;
#ifdef FIELDBUS_DEBUG
								   pprintf("\n Acd Info updated on confirmation");
								   pprintf("\n AcdAct %d RemoteMac %02X%02X%02X%02X%02X%02X HwType %04X Protocol Type %04X HwProLen %04X OpCode %04X"
															   " ShHwAddr %02X%02X%02X%02X%02X%02X Sip %08X DhHwAddr %02X%02X%02X%02X%02X%02X Dip %08X", AcdInfo->AcdActivity,
															   AcdInfo->RemoteMac[0], AcdInfo->RemoteMac[1], AcdInfo->RemoteMac[2],
															   AcdInfo->RemoteMac[3], AcdInfo->RemoteMac[4], AcdInfo->RemoteMac[5],
															   AcdInfo->ArpPDU.Hwtype, AcdInfo->ArpPDU.Proto, AcdInfo->ArpPDU.HwlenProtolen, AcdInfo->ArpPDU.Opcode,
															   AcdInfo->ArpPDU.Shwaddr[0],AcdInfo->ArpPDU.Shwaddr[1],AcdInfo->ArpPDU.Shwaddr[2],
															   AcdInfo->ArpPDU.Shwaddr[3],AcdInfo->ArpPDU.Shwaddr[4],AcdInfo->ArpPDU.Shwaddr[5],
															   AcdInfo->ArpPDU.Sipaddr,
															   AcdInfo->ArpPDU.Dhwaddr[0],AcdInfo->ArpPDU.Dhwaddr[1],AcdInfo->ArpPDU.Dhwaddr[2],
															   AcdInfo->ArpPDU.Dhwaddr[3],AcdInfo->ArpPDU.Dhwaddr[4],AcdInfo->ArpPDU.Dhwaddr[5], AcdInfo->ArpPDU.Dipaddr
								  );
#endif

					    }
					   else if((LE32_TO_HOST(CipServiceCnf->tData.ulClass) == TCPIP_INTERFACEOBJECT_CLASS) && (LE32_TO_HOST(CipServiceCnf->tData.ulInstance) == TCPIP_INTERFACEOBJECT_INSTANCE)
							   && (LE32_TO_HOST(CipServiceCnf->tData.ulAttribute) == TCPIP_INTERFACEOBJECT_INTERFACECONFIG_ATTRIBUTE) && (LE32_TO_HOST(CipServiceCnf->tData.ulService) == GET_ATTRIBUTE_SINGLE)
							   && ((LE32_TO_HOST(CipServiceCnf->tData.ulGRC) == STAT_SUCCESS)))
					   {
#ifdef FIELDBUS_DEBUG
							printf("\n replied get attr single");
							for (i = 0; i < 12; i++) {
									printf("%02X", CipServiceCnf->tData.abData[i]);
							}
#endif
							Config = (TCPIPInterfaceConfig*)CipServiceCnf->tData.abData;
							EthIPConfig.IpAddr = LE32_TO_HOST(Config->IpAddr);
							EthIPConfig.NM = LE32_TO_HOST(Config->NM);
							EthIPConfig.GW = LE32_TO_HOST(Config->GW);
							if(Sysconfig->FBCurrNetworkConfig == configStatic){//Change these values only in case of static configuration
							  Sysconfig->FBSystemIP = EthIPConfig.IpAddr;
							  Sysconfig->FBNetmask = EthIPConfig.NM;
							  Sysconfig->FBGw = EthIPConfig.GW;
							}
							RegUpdateCmdFlag = TRUE;
					   }
					break;
			}
	}

}

/* void EthernetIP::GetFaultData(UINT32 FaultCode)
 *
 * Purpose:
 *    To get the fault data from slave card using EIP service. Right now its used only to get the Address conflict
 *    data. Called from website on url ACDInfo32923646.html.
 *
 * Entry condition:
 *    FaultCode- The fault for which data is to be retrieved
 *
 * Exit condition:
 *    None.
 */
void EthernetIP::GetFaultData(UINT32 FaultCode)
{
	EIP_OBJECT_PACKET_CIP_SERVICE_REQ_T * ServicReqFromHost = (EIP_OBJECT_PACKET_CIP_SERVICE_REQ_T *)&TempPkt;
	if(FaultCode == RCX_E_DUPLICATE_NODE){
		ServicReqFromHost->tHead.ulDest = HOST_TO_LE32(0x00000020);
		ServicReqFromHost->tHead.ulSrc = HOST_TO_LE32(0);
		ServicReqFromHost->tHead.ulDestId = HOST_TO_LE32(0);
		ServicReqFromHost->tHead.ulSrcId = HOST_TO_LE32(0);
		ServicReqFromHost->tHead.ulLen = HOST_TO_LE32(EIP_OBJECT_CIP_SERVICE_REQ_SIZE);
		ServicReqFromHost->tHead.ulId = HOST_TO_LE32(0x00000000);
		ServicReqFromHost->tHead.ulSta = HOST_TO_LE32(0);
		ServicReqFromHost->tHead.ulCmd = HOST_TO_LE32(EIP_OBJECT_CIP_SERVICE_REQ);
		ServicReqFromHost->tHead.ulExt = HOST_TO_LE32(0);
		ServicReqFromHost->tHead.ulRout = HOST_TO_LE32(0);

		ServicReqFromHost->tData.ulClass = HOST_TO_LE32(TCPIP_INTERFACEOBJECT_CLASS);
		ServicReqFromHost->tData.ulInstance = HOST_TO_LE32(TCPIP_INTERFACEOBJECT_INSTANCE);
		ServicReqFromHost->tData.ulAttribute = HOST_TO_LE32(TCPIP_INTERFACEOBJECT_ACDATTRIBUTE);
		ServicReqFromHost->tData.ulService = HOST_TO_LE32(GET_ATTRIBUTE_SINGLE);
		FieldbusTask::thisPtr->CifxPkt.Write(0, TempPkt, 0);
	}
}

/* void EthernetIP::GetAttribute(UINT32 ulClass, UINT32 ulInstance, UINT32 ulAttribute)
 *
 * Purpose:
 *    To get the attribute data from slave card using EIP Get Attribute single service. This function only sends the command to slave card.
 *    Reply is received in ProcessMailBox function. Currently this function is called after 500 ms of Ethernet link up or ip configured indication
 *    from Hilscher's EthernetIP slave card
 * Entry condition:
 *    ulClass- The preknown class for which get attribute service needs to be send
 *    ulInstance- The instance of class
 *    ulAttribute- The attribute of class
 *
 * Exit condition:
 *    None.
 */
void EthernetIP::GetAttribute(UINT32 ulClass, UINT32 ulInstance, UINT32 ulAttribute)
{
#ifdef FIELDBUS_DEBUG
		pprintf("\nCalling Get Single Attribute cls %d inst % Attr %d Service %d",
				ulClass, ulInstance, ulAttribute, ulService);
#endif
	EIP_OBJECT_PACKET_CIP_SERVICE_REQ_T * ServicReqFromHost = (EIP_OBJECT_PACKET_CIP_SERVICE_REQ_T *)&TempPkt;
	ServicReqFromHost->tHead.ulDest = HOST_TO_LE32(0x00000020);
	ServicReqFromHost->tHead.ulSrc = HOST_TO_LE32(0);
	ServicReqFromHost->tHead.ulDestId = HOST_TO_LE32(0);
	ServicReqFromHost->tHead.ulSrcId = HOST_TO_LE32(0);
	ServicReqFromHost->tHead.ulLen = HOST_TO_LE32(EIP_OBJECT_CIP_SERVICE_REQ_SIZE);
	ServicReqFromHost->tHead.ulId = HOST_TO_LE32(0x00000000);
	ServicReqFromHost->tHead.ulSta = HOST_TO_LE32(0);
	ServicReqFromHost->tHead.ulCmd = HOST_TO_LE32(EIP_OBJECT_CIP_SERVICE_REQ);
	ServicReqFromHost->tHead.ulExt = HOST_TO_LE32(0);
	ServicReqFromHost->tHead.ulRout = HOST_TO_LE32(0);

	ServicReqFromHost->tData.ulClass = HOST_TO_LE32(ulClass);
	ServicReqFromHost->tData.ulInstance = HOST_TO_LE32(ulInstance);
	ServicReqFromHost->tData.ulAttribute = HOST_TO_LE32(ulAttribute);
	ServicReqFromHost->tData.ulService = HOST_TO_LE32(GET_ATTRIBUTE_SINGLE);
	FieldbusTask::thisPtr->CifxPkt.Write(0, TempPkt, 0);
}


/* void EthernetIP::SetAttribute(UINT32 ulClass, UINT32 ulInstance, UINT32 ulAttribute, UINT32 value, CIFXHANDLE HChannel, BOOLEAN AtReset)
 *
 * Purpose:
 *    To set the attribute data to slave card using EIP Set Attribute single service. This function only sends the command to slave card.
 *    Reply is received in ProcessMailBox function.
 * Entry condition:
 *    ulClass- The pre-known class for which get attribute service needs to be send
 *    ulInstance- The instance of class
 *    ulAttribute- The attribute of class
 *	  value	- value to be set to the attribute
 *	  HChannel	- Null
 *	  AtReset - tells whether the attribute to be set should be accessed directly via Dev_putPacket if AtReset  = FALSE or via mailbox if AtReset = TRUE
 * Exit condition:
 *    None.
 */
void EthernetIP::SetAttribute(UINT32 ulClass, UINT32 ulInstance, UINT32 ulAttribute, UINT32 value, CIFXHANDLE HChannel, BOOLEAN AtReset)
{
	UINT32 RetVal = 0;
	PCHANNELINSTANCE PtChannel = (PCHANNELINSTANCE) HChannel;
	EIP_OBJECT_PACKET_CIP_SERVICE_CNF_T CnfReg;
	EIP_OBJECT_PACKET_CIP_SERVICE_REQ_T * ServicReqFromHost = (EIP_OBJECT_PACKET_CIP_SERVICE_REQ_T *)&TempPkt;

	ServicReqFromHost->tHead.ulDest = HOST_TO_LE32(0x00000020);
	ServicReqFromHost->tHead.ulSrc = HOST_TO_LE32(0);
	ServicReqFromHost->tHead.ulDestId = HOST_TO_LE32(0);
	ServicReqFromHost->tHead.ulSrcId = HOST_TO_LE32(0);

	ServicReqFromHost->tHead.ulLen = EIP_OBJECT_CIP_SERVICE_REQ_SIZE;

	ServicReqFromHost->tHead.ulId = HOST_TO_LE32(0x00000000);
	ServicReqFromHost->tHead.ulSta = HOST_TO_LE32(0);
	ServicReqFromHost->tHead.ulCmd = HOST_TO_LE32(EIP_OBJECT_CIP_SERVICE_REQ);
	ServicReqFromHost->tHead.ulExt = HOST_TO_LE32(0);
	ServicReqFromHost->tHead.ulRout = HOST_TO_LE32(0);

	ServicReqFromHost->tData.ulClass = HOST_TO_LE32(ulClass);
	ServicReqFromHost->tData.ulInstance = HOST_TO_LE32(ulInstance);
	ServicReqFromHost->tData.ulAttribute = HOST_TO_LE32(ulAttribute);
	ServicReqFromHost->tData.ulService = HOST_TO_LE32(SET_ATTRIBUTE_SINGLE);


	memcpy(&ServicReqFromHost->tData.abData, &value, sizeof(value));

	ServicReqFromHost->tHead.ulLen += (sizeof(value));
	ServicReqFromHost->tHead.ulLen = HOST_TO_LE32(ServicReqFromHost->tHead.ulLen);

	if(AtReset)
	{
		FieldbusTask::thisPtr->CifxPkt.Write(0, TempPkt, 0);
	}
	else
	{
		if (CIFX_NO_ERROR != (RetVal = DEV_PutPacket(PtChannel, (CIFX_PACKET*) &TempPkt, MAILBOX_PACKET_TIMEOUT))) {
#ifdef FIELDBUS_DEBUG
			printf("Error sending register OBJ MR!\r\n");
#endif
		} else {
			RTOS::DelayMs(1);
			if (CIFX_NO_ERROR != (RetVal = DEV_GetPacket(PtChannel, (CIFX_PACKET*) &CnfReg,
				  sizeof(EIP_OBJECT_PACKET_CIP_SERVICE_CNF_Ttag), MAILBOX_PACKET_TIMEOUT))) {
#ifdef FIELDBUS_DEBUG
				printf("Error getting register OBJ MR!\r\n");
#endif
			} else {
				if (LE32_TO_HOST(CnfReg.tHead.ulCmd) == (EIP_OBJECT_CIP_SERVICE_REQ + 1)) {
					if (RCX_S_OK == LE32_TO_HOST(CnfReg.tHead.ulSta)) {
#ifdef FIELDBUS_DEBUG
						printf("Received CNF reg OBJ MR! %08X:\r\n", LE32_TO_HOST(CnfReg.tHead.ulCmd));
#endif
					} else{
#ifdef FIELDBUS_DEBUG
						printf("Received CNF reg OBJ MR! %08X error %08X :\r\n", LE32_TO_HOST(CnfReg.tHead.ulCmd), LE32_TO_HOST(CnfReg.tHead.ulSta));
#endif
					}
				}
			}
		}
	}

}



/*  void EthernetIP::HandleResetServiceIndication(EIP_OBJECT_PACKET_RESET_IND_T *)
 *
 *   Purpose:
 *    This function is called by ProcessMailBox function when reset service indication
 *    is received on identity object.
 *
 *   Entry condition:
 *    Reset indication received.
 *
 *   Exit condition:
 *    None.
 */
void EthernetIP::HandleResetServiceIndication(EIP_OBJECT_PACKET_RESET_IND_T * IndPkt)
{
//	 Handling of EIP_OBJECT_RESET_IND
//		1. Send a response packet (Command code 0x00001A25)
//		2. Send a EIP_APS_SET_CONFIGURATION_REQ packet (Command code 0x00003608)
//		Note: TCP parameters ulTcpFlag, ulIpAddr, ulNetMask and ulGateway might
//		have been changed previously.
//		3. Send a EIP_OBJECT_CFG_QOS_REQ packet (Command code 0x00001A42)
//		Note: Parameters might have been changed previously.
//		4. Send a EIP_OBJECT_RESET_REQ packet (Command code 0x00001A26)
	//Point 1 is covered from in the ProcessmailBox function
	GetWarmStartParams(&TempPkt);
	//Point 2 and 3 are covered here. Note: 3 is part of extended configuration packet.
	FieldbusTask::thisPtr->CifxPkt.Write(0, TempPkt, 0);
	//point 4 is covered here
	EIP_OBJECT_PACKET_RESET_REQ_T * ResetReq = (EIP_OBJECT_PACKET_RESET_REQ_T *)&TempPkt;
	memset(ResetReq , 0, sizeof(EIP_OBJECT_PACKET_RESET_REQ_T));
	ResetReq->tHead.ulDest = HOST_TO_LE32(0x00000020);
	ResetReq->tHead.ulLen = HOST_TO_LE32(sizeof(ResetReq->tData));
	ResetReq->tHead.ulCmd = HOST_TO_LE32(EIP_OBJECT_RESET_REQ);

	ResetReq->tData.ulDataIdx = IndPkt->Data.ulDataIdx;
	ResetReq->tData.ulResetMode = IndPkt->Data.ulResetType;
	FieldbusTask::thisPtr->CifxPkt.Write(0, TempPkt, 0);
	ResetCompleted = TRUE;

}

/*  void EthernetIP::ReadExtendedStatus(CIFXHANDLE HChannel)
 *
 *   Purpose:
 *    This function is called by FiledBusTask::Run() function
 *    when Fieldbus Diagnostic page polls for the status
 *    of slave. Extended status in cae of EthernetIP is not supported by Hilscher.
 *   Entry condition:
 *    HChannel:Handle to the communication channel(returned by XChannelOpen API)
 *
 *   Exit condition:
 *    None.
 */
void EthernetIP::ReadExtendedStatus(CIFXHANDLE HChannel)
{
	HChannel = 0;
}

void EthernetIP::RegisterAssembly(CIFXHANDLE HChannel, UINT32 Instance, UINT32 Size, UINT32 DpmOffset, UINT32 AssemFlags)
{
	/*Output Data Image0 0x1300 5760 Bytes Cyclic Output Process Data Image
	 Input Data Image0    0x2980 5760 Bytes Cyclic Input Process Data Image
	 Register input Assembly (adapter to scanner)(T -> O)(Produce data on network)
	 //Register Output Assembly(scanner to adapter)(O -> T)(Consumes data from network)*/
	PCHANNELINSTANCE PtChannel = (PCHANNELINSTANCE) HChannel;
	EIP_OBJECT_PACKET_AS_REGISTER_REQ_T Pkt;
	EIP_OBJECT_PACKET_AS_REGISTER_CNF_T CnfReg;
	SINT32 RetVal;

	Pkt.tHead.ulDest = HOST_TO_LE32(0x00000020);
	Pkt.tHead.ulSrc = HOST_TO_LE32(0);
	Pkt.tHead.ulDestId = HOST_TO_LE32(0);
	Pkt.tHead.ulSrcId = HOST_TO_LE32(0);
	Pkt.tHead.ulLen = HOST_TO_LE32(EIP_OBJECT_AS_REGISTER_REQ_SIZE);
	Pkt.tHead.ulId = HOST_TO_LE32(0x00000000);
	Pkt.tHead.ulSta = HOST_TO_LE32(0);
	Pkt.tHead.ulCmd = HOST_TO_LE32(EIP_OBJECT_AS_REGISTER_REQ);
	Pkt.tHead.ulExt = HOST_TO_LE32(0);
	Pkt.tHead.ulRout = HOST_TO_LE32(0);


	Pkt.tData.ulInstance = HOST_TO_LE32(Instance);
	Pkt.tData.ulSize = HOST_TO_LE32(Size);
	Pkt.tData.ulDPMOffset = HOST_TO_LE32(DpmOffset);
	Pkt.tData.ulFlags = HOST_TO_LE32(AssemFlags);
	if (CIFX_NO_ERROR != (RetVal = DEV_PutPacket(PtChannel, (CIFX_PACKET*) &Pkt, MAILBOX_PACKET_TIMEOUT))) {
#ifdef FIELDBUS_DEBUG
		printf("Error sending register ip assem!\r\n");
#endif
	} else {
		RTOS::DelayMs(1);
		if (CIFX_NO_ERROR != (RetVal = DEV_GetPacket(PtChannel, (CIFX_PACKET*) &CnfReg,
		      sizeof(EIP_OBJECT_PACKET_AS_REGISTER_CNF_T), MAILBOX_PACKET_TIMEOUT))) {
#ifdef FIELDBUS_DEBUG
			printf("Error getting register ip assem CNF!\r\n");
#endif
		} else {
			if (LE32_TO_HOST(CnfReg.tHead.ulCmd) == (EIP_OBJECT_AS_REGISTER_REQ + 1)) {
				if (RCX_S_OK == LE32_TO_HOST(CnfReg.tHead.ulSta)) {
#ifdef FIELDBUS_DEBUG
					printf("Received CNF %08X ip reg assem success :\r\n", LE32_TO_HOST(CnfReg.tHead.ulCmd));
#endif
				} else{
#ifdef FIELDBUS_DEBUG
					printf(
					"Received CNF %08X ip reg assem error %08X :\r\n", LE32_TO_HOST(CnfReg.tHead.ulCmd),
					      LE32_TO_HOST(CnfReg.tHead.ulSta));
#endif
				}
			}

		}
	}
}

/*  void EthernetIP::RegisterForMSNSChange(CIFXHANDLE HChannel)
 *
 *  Purpose:
 *    This function is called once at power up from ProtocolSpecificInit() function.
 *    This function registers the Host i.e. WC to get the indication of module and network status
 *    change from Slave card.
 *
 *  Entry condition:
 *    HChannel:Handle to the communication channel(returned by XChannelOpen API)
 *
 *  Exit condition:
 *    None.
 */
void EthernetIP::RegisterForMSNSChange(CIFXHANDLE HChannel)
{
	PCHANNELINSTANCE PtChannel = (PCHANNELINSTANCE) HChannel;
	EIP_APS_PACKET_SET_PARAMETER_REQ_T Pkt;
	EIP_APS_PACKET_SET_PARAMETER_CNF_T CnfReg;
	SINT32 RetVal;

	Pkt.tHead.ulDest = HOST_TO_LE32(0x00000020);
	Pkt.tHead.ulSrc = HOST_TO_LE32(0);
	Pkt.tHead.ulDestId = HOST_TO_LE32(0);
	Pkt.tHead.ulSrcId = HOST_TO_LE32(0);
	Pkt.tHead.ulLen = HOST_TO_LE32(4);
	Pkt.tHead.ulId = HOST_TO_LE32(0x00000000);
	Pkt.tHead.ulSta = HOST_TO_LE32(0);
	Pkt.tHead.ulCmd = HOST_TO_LE32(EIP_APS_SET_PARAMETER_REQ);
	Pkt.tHead.ulExt = HOST_TO_LE32(0);
	Pkt.tHead.ulRout = HOST_TO_LE32(0);

	Pkt.tData.ulParameterFlags = HOST_TO_LE32(EIP_APS_PRM_SIGNAL_MS_NS_CHANGE);
	if (CIFX_NO_ERROR != (RetVal = DEV_PutPacket(PtChannel, (CIFX_PACKET*) &Pkt, MAILBOX_PACKET_TIMEOUT))) {
#ifdef FIELDBUS_DEBUG
		printf("Error sending register MS NS! %08X\r\n", RetVal);
#endif
	} else {
		RTOS::DelayMs(1);
		if (CIFX_NO_ERROR != (RetVal = DEV_GetPacket(PtChannel, (CIFX_PACKET*) &CnfReg,
		      sizeof(EIP_APS_PACKET_SET_PARAMETER_CNF_T), MAILBOX_PACKET_TIMEOUT))) {
#ifdef FIELDBUS_DEBUG
			printf("Error getting register MS NS CNF! %08X\r\n", RetVal);
#endif
		} else {
			if (LE32_TO_HOST(CnfReg.tHead.ulCmd) == (EIP_APS_SET_PARAMETER_REQ + 1)) {
				if (RCX_S_OK == LE32_TO_HOST(CnfReg.tHead.ulSta)) {
#ifdef FIELDBUS_DEBUG
					printf("Received CNF reg MS NS success %08X:\r\n", LE32_TO_HOST(CnfReg.tHead.ulCmd));
#endif
				} else{
#ifdef FIELDBUS_DEBUG
					printf("Received CNF reg MS NS %08X error %08X :\r\n", LE32_TO_HOST(CnfReg.tHead.ulCmd),
					      LE32_TO_HOST(CnfReg.tHead.ulSta));
#endif
				}
			}
		}
	}
}

void EthernetIP::RegisterService(CIFXHANDLE HChannel, UINT32 ServiceCode)
{
	EIP_OBJECT_PACKET_REGISTER_SERVICE_REQ_Ttag Pkt;
	EIP_OBJECT_PACKET_REGISTER_SERVICE_CNF_Ttag CnfReg;
	PCHANNELINSTANCE PtChannel = (PCHANNELINSTANCE) HChannel;
	SINT32 RetVal;

	Pkt.tHead.ulDest = HOST_TO_LE32(0x00000020);
	Pkt.tHead.ulSrc = HOST_TO_LE32(0);
	Pkt.tHead.ulDestId = HOST_TO_LE32(0);
	Pkt.tHead.ulSrcId = HOST_TO_LE32(0);
	Pkt.tHead.ulLen = HOST_TO_LE32(4);
	Pkt.tHead.ulId = HOST_TO_LE32(0x00000000);
	Pkt.tHead.ulSta = HOST_TO_LE32(0);
	Pkt.tHead.ulCmd = HOST_TO_LE32(EIP_OBJECT_REGISTER_SERVICE_REQ);
	Pkt.tHead.ulExt = HOST_TO_LE32(0);
	Pkt.tHead.ulRout = HOST_TO_LE32(0);

	Pkt.tData.ulService = HOST_TO_LE32(ServiceCode);
	if (CIFX_NO_ERROR != (RetVal = DEV_PutPacket(PtChannel, (CIFX_PACKET*) &Pkt, MAILBOX_PACKET_TIMEOUT))) {
#ifdef FIELDBUS_DEBUG
		printf("Error sending register service!\r\n");
#endif
	} else {
		RTOS::DelayMs(1);
		if (CIFX_NO_ERROR != (RetVal = DEV_GetPacket(PtChannel, (CIFX_PACKET*) &CnfReg,
		      sizeof(EIP_OBJECT_PACKET_REGISTER_SERVICE_CNF_Ttag), MAILBOX_PACKET_TIMEOUT))) {
#ifdef FIELDBUS_DEBUG
			printf("Error getting register srvice!\r\n");
#endif
		} else {
			if (LE32_TO_HOST(CnfReg.tHead.ulCmd) == (EIP_OBJECT_REGISTER_SERVICE_REQ + 1)) {
				if (RCX_S_OK == LE32_TO_HOST(CnfReg.tHead.ulSta)) {
#ifdef FIELDBUS_DEBUG
					printf("Received CNF reg srvice! %08X:\r\n", LE32_TO_HOST(CnfReg.tHead.ulCmd));
#endif
				} else{
#ifdef FIELDBUS_DEBUG
					printf("Received CNF reg srvice! %08X error %08X :\r\n", LE32_TO_HOST(CnfReg.tHead.ulCmd), LE32_TO_HOST(CnfReg.tHead.ulSta));
#endif
				}
			} else {
#ifdef FIELDBUS_DEBUG
				printf("Received out of sequence %s %08X:\r\n",__FUNCTION__, LE32_TO_HOST(CnfReg.tHead.ulCmd));
#endif
			}
		}
	}
}


/*  void EthernetIP::RegisterVendorSpecificClass(CIFXHANDLE HChannel, UINT32 VendorObjClass)
 *
 *  Purpose:
 *    This function is called once at power up from ProtocolSpecificInit() function.
 *    function.This function registers the Host i.e. WC to get the object change indication for the class
 *    registered through VendorObjClass parameter. The indication is received when the registered class
 *    is accessed through explicit messaging by master.
 *
 *  Entry condition:
 *    HChannel:Handle to the communication channel(returned by XChannelOpen API)
 *	  VendorObjClass:The vendor specific class for which object change indication is to be enabled.
 *	  OnReset: Flag to tell whether the vendor specific classed are registered on power up or reset service
 *	  indication.
 *  Exit condition:
 *    None.
 */
void EthernetIP::RegisterVendorSpecificClass(CIFXHANDLE HChannel, UINT32 VendorObjClass, BOOLEAN OnReset)
{
	EIP_OBJECT_PACKET_MR_REGISTER_REQ_T Pkt;
	EIP_OBJECT_PACKET_MR_REGISTER_CNF_T CnfReg;
	PCHANNELINSTANCE PtChannel = (PCHANNELINSTANCE) HChannel;
	SINT32 RetVal;

	Pkt.tHead.ulDest = HOST_TO_LE32(0x00000020);
	Pkt.tHead.ulSrc = HOST_TO_LE32(0);
	Pkt.tHead.ulDestId = HOST_TO_LE32(0);
	Pkt.tHead.ulSrcId = HOST_TO_LE32(0);
	Pkt.tHead.ulLen = HOST_TO_LE32(EIP_OBJECT_MR_REGISTER_REQ_SIZE);
	Pkt.tHead.ulId = HOST_TO_LE32(0x00000000);
	Pkt.tHead.ulSta = HOST_TO_LE32(0);
	Pkt.tHead.ulCmd = HOST_TO_LE32(EIP_OBJECT_MR_REGISTER_REQ);
	Pkt.tHead.ulExt = HOST_TO_LE32(0);
	Pkt.tHead.ulRout = HOST_TO_LE32(0);

	Pkt.tData.hObjectQue = (0);
	Pkt.tData.ulAccessTyp = HOST_TO_LE32(0);
	Pkt.tData.ulClass = HOST_TO_LE32(VendorObjClass);
	if(OnReset)
	{
		memcpy(&TempPkt, &Pkt, sizeof(Pkt));
		FieldbusTask::thisPtr->CifxPkt.Write(0, TempPkt, 0);
	}
	else
	{
		if (CIFX_NO_ERROR != (RetVal = DEV_PutPacket(PtChannel, (CIFX_PACKET*) &Pkt, MAILBOX_PACKET_TIMEOUT))) {
#ifdef FIELDBUS_DEBUG
			printf("Error sending register OBJ MR!\r\n");
#endif
		} else {
			RTOS::DelayMs(1);
			if (CIFX_NO_ERROR != (RetVal = DEV_GetPacket(PtChannel, (CIFX_PACKET*) &CnfReg,
				  sizeof(EIP_OBJECT_PACKET_REGISTER_SERVICE_CNF_Ttag), MAILBOX_PACKET_TIMEOUT))) {
#ifdef FIELDBUS_DEBUG
				printf("Error getting register OBJ MR!\r\n");
#endif
			} else {
				if (LE32_TO_HOST(CnfReg.tHead.ulCmd) == (EIP_OBJECT_MR_REGISTER_REQ + 1)) {
					if (RCX_S_OK == LE32_TO_HOST(CnfReg.tHead.ulSta)) {
#ifdef FIELDBUS_DEBUG
						printf("Received CNF reg OBJ MR! %08X:\r\n", LE32_TO_HOST(CnfReg.tHead.ulCmd));
#endif
					} else{
#ifdef FIELDBUS_DEBUG
						printf("Received CNF reg OBJ MR! %08X error %08X :\r\n", LE32_TO_HOST(CnfReg.tHead.ulCmd), LE32_TO_HOST(CnfReg.tHead.ulSta));
#endif
					}
				}
			}
		}
	}
}


/*  void EthernetIP::ProtocolSpecificInit(CIFXHANDLE HChannel)
 *
 *  Purpose:
 *    This function is called once at power up from FieldbusTask::Run() function.
 *    function.This function call the other functions which are required to be called once at powerup.
 *
 *  Entry condition:
 *    HChannel:Handle to the communication channel(returned by XChannelOpen API)
 *
 *  Exit condition:
 *    None.
 */
void EthernetIP::ProtocolSpecificInit(CIFXHANDLE HChannel)
{
	UINT32 configCapabilityBit6 = HOST_TO_LE32(0xD5);	//set bit 6 of configuration capability to tell that device requires a restart after configuration is updated.

	RegisterForMSNSChange(HChannel);
//	RegisterService(HChannel, 0x0E);
//	RegisterService(HChannel, 0x10);
	RegisterVendorSpecificClass(HChannel, ETHIP_VENDORSPECIFIC_CLASS1);
	RegisterVendorSpecificClass(HChannel, ETHIP_VENDORSPECIFIC_CLASS2);
	RegisterVendorSpecificClass(HChannel, ETHIP_VENDORSPECIFIC_CLASS3);
	RegisterVendorSpecificClass(HChannel, ETHIP_VENDORSPECIFIC_CLASS4);
	RegisterVendorSpecificClass(HChannel, ETHIP_VENDORSPECIFIC_CLASS5);
	RegisterVendorSpecificClass(HChannel, ETHIP_VENDORSPECIFIC_CLASS6);
	RegisterVendorSpecificClass(HChannel, ETHIP_VENDORSPECIFIC_CLASS7);

	//update bit 6 of Configuration Control attribute of TCP IP class object.
	SetAttribute(TCPIP_INTERFACEOBJECT_CLASS, TCPIP_INTERFACEOBJECT_INSTANCE, TCPIP_INTERFACEOBJECT_CONFIGCAPABILITY_ATTRIBUTE, configCapabilityBit6, HChannel);
}



/* EthIPStausCode EthernetIP::ProcessExplicitMsgRequest(UINT32 Class, UINT32 Service, UINT32 Instance, UINT32 Attribute,
		UINT8 * Data, UINT32 * DataLen)
 *
 *  Purpose:
 *    This function is called by ProcessMailBox function when an object of registerd vendor specific class
 *    is accessed for read or write through explicit messaging. It validates the access parameters and
 *    and call other functions to handle the explicit message.
 *
 *  Entry condition:
 *    Class:Class code whose object is requested.
 *    Sevice: Service code to check (GET or SET READ/WRITE) access.
 *    Instance: Instance number of the class being accessed.
 *    Attribute:Attribute of class object being accessed.
 *    Data: Pointer to Service data associated with access.
 * 	  DataLen: Pointer to length of service data
 *
 *  Exit condition:
 *    Returns the status of explicit message processing needs to be sent to master.
 */
EthIPStausCode EthernetIP::ProcessExplicitMsgRequest(UINT32 Class, UINT32 Service, UINT32 Instance, UINT32 Attribute,
		UINT8 * Data, UINT32 * DataLen)
{
	EthIPStausCode RetVal;
	RetVal.GSC = STAT_SUCCESS;
	RetVal.ESC = EXTSTAT_SUCCESS;
	UINT8 Access = ACC_READ;

	if ((Service == GET_ATTRIBUTE_SINGLE) || (Service == SET_ATTRIBUTE_SINGLE)) {
		switch (Class) {
		case ETHIP_VENDORSPECIFIC_CLASS1:
			if(Instance > ETHIP_VENDORSPECIFIC_CLASS1_MAXINSTANCE)
				RetVal.GSC = STAT_PATH_DESTINATION_UNKNOWN;
			else if(Attribute < PARA_SET_BASE || Attribute > PARA_SET_LAST)
				RetVal.GSC = STAT_INVALID_ATTRIBUTE;
				break;
		case ETHIP_VENDORSPECIFIC_CLASS2:
			if(Instance > ETHIP_VENDORSPECIFIC_CLASS2_MAXINSTANCE)
				RetVal.GSC = STAT_PATH_DESTINATION_UNKNOWN;
			else if(Attribute < WS_SET_BASE || Attribute > WS_SET_LAST)
				RetVal.GSC = STAT_INVALID_ATTRIBUTE;
			break;
		case ETHIP_VENDORSPECIFIC_CLASS3:
			if(Instance > ETHIP_VENDORSPECIFIC_CLASS3_MAXINSTANCE)
				RetVal.GSC = STAT_PATH_DESTINATION_UNKNOWN;
			else if(Attribute < HSPARA_SET_BASE || Attribute > HSPARA_SET_LAST)
				RetVal.GSC = STAT_INVALID_ATTRIBUTE;
			break;
		case ETHIP_VENDORSPECIFIC_CLASS4:
			if(Instance > ETHIP_VENDORSPECIFIC_CLASS4_MAXINSTANCE)
				RetVal.GSC = STAT_PATH_DESTINATION_UNKNOWN;
			else if(Attribute < HSSS_SET_BASE || Attribute > HSSS_SET_LAST)
				RetVal.GSC = STAT_INVALID_ATTRIBUTE;
			break;
		case ETHIP_VENDORSPECIFIC_CLASS5:
			if(Instance >= ETHIP_VENDORSPECIFIC_CLASS5_MAXINSTANCE)
				RetVal.GSC = STAT_PATH_DESTINATION_UNKNOWN;
			else if(Attribute < ERROR_INFO_BASE || Attribute > ERROR_INFO_LAST)
				RetVal.GSC = STAT_INVALID_ATTRIBUTE;
			break;
		case ETHIP_VENDORSPECIFIC_CLASS6:
			if(Instance >= ETHIP_VENDORSPECIFIC_CLASS6_MAXINSTANCE)
				RetVal.GSC = STAT_PATH_DESTINATION_UNKNOWN;
			else if(Attribute < SYSINFO_BASE || Attribute > SYSINFO_LAST)
				RetVal.GSC = STAT_INVALID_ATTRIBUTE;
			break;
		case ETHIP_VENDORSPECIFIC_CLASS7:
			if(Instance >= ETHIP_VENDORSPECIFIC_CLASS7_MAXINSTANCE)
				RetVal.GSC = STAT_PATH_DESTINATION_UNKNOWN;
			else if((Attribute >= EXTCTRL_INFO_BASE) && (Attribute <= EXTCTRL_INFO_LAST)){
			   if(Service == SET_ATTRIBUTE_SINGLE)
			      RetVal.GSC = STAT_ATTRIBUTE_NOT_SETTABLE;
			   else
			      RetVal.GSC = STAT_SUCCESS;
			}
			else if((Attribute >= TEST_PARA_BASE && Attribute <= TEST_PARA_LAST)
					|| (Attribute >= VERSIONINFO_BASE && Attribute <= VERSIONINFO_LAST)
					|| (Attribute >= RTC_DATE && Attribute <= RTC_TIME)
					|| (Attribute >= CNTR_INFO_BASE && Attribute <= (CNTR_INFO_LAST - 1))
					|| (Attribute >= OS_ACCESS_TOKEN && Attribute <= (OS_ACCESS_TOKEN + 1))
					|| (Attribute >= SYSCONFIG_BASE && Attribute <= SYSCONFIG_LAST))
				RetVal.GSC = STAT_SUCCESS;
			else
				RetVal.GSC = STAT_INVALID_ATTRIBUTE;
			break;
		default:
			*DataLen = 0;
			RetVal.GSC = STAT_PATH_DESTINATION_UNKNOWN;
		};
	} else{
		*DataLen = 0;
		RetVal.GSC = STAT_SERVICE_NOT_SUPPORTED;
	}

	if (RetVal.GSC == STAT_SUCCESS) {
		if (Service == SET_ATTRIBUTE_SINGLE)
			Access = ACC_WRITE;
		RetVal = HandleValueEthIP(Attribute, Instance, Access, Data, DataLen);
	}

	return RetVal;
}


/* EthIPStausCode HandleValueEthIP(UINT32 Attribute, UINT32 InstanceNum, UINT8 Access, void * Value, UINT32 * Len)
 * Purpose:
 *    To process the explicit message command received from EthernetIP.
 *    This function is called by EthernetIP::ProcessExplicitMsgRequest() function.
 *
 * Entry condition:
 *  Attribute:Represents the unique number for the parameter to be accessed. Also represents special commands like
 *         GET/RELEASE access token.
 *  InstanceNum: Index of array accesses by Attribute parameter numbe
 *  Access: Access either read or write.
 *  Value: pointer points to either the service data received from SET_ATTRIBUTE_SINGLE service or the service data to be replied
 *  for GET_ATTTRIBUTE_SINGLE service
 * Len: Pointer to length of service data
 *
 * Exit condition:
 *  Returns EthernetIP standard specific General and Extended code.
 */
EthIPStausCode HandleValueEthIP(UINT32 Attribute, UINT32 InstanceNum, UINT8 Access, void * Value, UINT32 * Len)
{
   EthIPStausCode RetVal;
   BOOLEAN AccessTokenStatus;
   RetVal.GSC = STAT_SUCCESS;
   RetVal.ESC = EXTSTAT_SUCCESS;
   /*Currently Acc Token with interface or not*/
   if (CommIfcAccessToken == COMM_IFC_FBB)
      AccessTokenStatus = TRUE;
   else
      AccessTokenStatus = FALSE;


   switch (Attribute) {
      case OS_ACCESS_TOKEN + 0:
         *Len = 0;
         if (InstanceNum != 0)
            RetVal.GSC = STAT_PATH_DESTINATION_UNKNOWN;
         else if (Access == ACC_READ) {
            if(GetAccessToken(COMM_IFC_FBB))
               *((UINT32*) Value) = 0;
            else
               *((UINT32*) Value) = HOST_TO_LE32(1);

            *Len = ETHIP_PARAM_DATALEN;
            RetVal.GSC = STAT_SUCCESS;
         } else if (Access == ACC_WRITE)
            RetVal.GSC = STAT_ATTRIBUTE_NOT_SETTABLE;
         else {
            //Undefined, Error
            RetVal.GSC = STAT_VENDOR_SPECIFIC_ERROR;
            RetVal.ESC = EXTSTAT_VAL_ERR_MISC;
         }
      break;
      case OS_ACCESS_TOKEN + 1:
         *Len = 0;
         if (InstanceNum != 0)
            RetVal.GSC = STAT_PATH_DESTINATION_UNKNOWN;
         else if (Access == ACC_READ) {
            //Read Value, always 0
            *((UINT32*) Value) = 0;
            *Len = ETHIP_PARAM_DATALEN;
            RetVal.GSC = STAT_SUCCESS;
         } else if (Access == ACC_WRITE) {
            if(ReleaseAccessToken(COMM_IFC_FBB) == FALSE)
               RetVal.GSC = STAT_PRIVILEGE_VIOLATION;
            else
               RetVal.GSC = STAT_SUCCESS;
         } else {
            //Undefined, Error
            RetVal.GSC = STAT_VENDOR_SPECIFIC_ERROR;
            RetVal.ESC = EXTSTAT_VAL_ERR_MISC;
         }
      break;
      default:
         if (Access == ACC_READ)
            RetVal = ReadParamEthIP(Value, Len, InstanceNum, Attribute);
         else if (Access == ACC_WRITE)
            RetVal = WriteParamEthIP(AccessTokenStatus, Value, Len, InstanceNum, Attribute);
         else {
            *Len = 0;
            RetVal.GSC = STAT_VENDOR_SPECIFIC_ERROR;
            RetVal.ESC = EXTSTAT_VAL_ERR_MISC;

         }
      break;
   }

   return RetVal;
}
