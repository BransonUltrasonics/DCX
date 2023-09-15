/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/Main.cpp_v   1.6   15 Sep 2016 17:13:32   EGUAJARDO  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/Main.cpp_v  $
 * 
 *    Rev 1.6   15 Sep 2016 17:13:32   EGUAJARDO
 * Created the task to BroadcastWR to communicate DCX with Manager Basic app.
 * 
 *    Rev 1.5   06 Jun 2016 15:04:20   GButron
 * V1.3.7A FT Commands
 * 
 *    Rev 1.4   04 Jun 2015 07:48:00   sesharma
 * Type casting for Data1 & Data2 fields of event log changed to SINT32 when writing an event. Branson coding standard formatter applied.
 * 
 *    Rev 1.3   19 Mar 2015 06:55:38   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.2   14 Nov 2014 13:49:50   ewomack
 * Do not decompress TARFS before CyGOS starts.
 * 
 *    Rev 1.1   17 Sep 2014 08:53:28   amaurya
 * Removed unused comments.
 * 
 *    Rev 1.0   18 Aug 2014 09:17:26   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:02   amaurya
 * Initial revision.
 */

#include "CyGOS.h"
#include "LED.h"
#include "InputTask.h"
#include "DebugNew.h"
#include "stdio.h"
#include "Board.h"
#include "IOTask.h"
#include "DupsTask.h"
#include "FUNC.h"
#include "Log.h"
#include "UserIO.h"
#include "ReceiveCommandHandler.h"
#include "SysConfig.h"
#include "Website.h"
#include "UploadFirmWareTask.h"
#include "WelderSearchTask.h"
#include "GLOBALS.h"
#include "CpuDefs.h"
#include "ShutDownTask.h"
#include "FirmwareUpgrade.h"
#include "SR232FT.h"
#include "WebSiteFT.h"
#include "BroadcastWR.h"
#include "RTOS.h"

#define PRIORITYOFFSET                                   1

void SetLeds(int slow, int fast)
{
   LED::Set(1, slow);
   LED::Set(2, fast);
}


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  Lookup Tables.  The "hardware" method of computing CRCs involves bit   */
/*  manipulations, which is very inefficient for a software computation.   */
/*  Instead of computing the CRC bit-by-bit, a 256-element lookup table    */
/*  can be used to perform the equivalent of 8 bit operations at a time.   */
/*  (This is described in "Byte-wise CRC Calculations" in IEEE Micro,      */
/*  June 1983, pp. 40-50.)  For a CRC-16, the lookup table consists of 256 */
/*  2-byte WORDs (see below, or the CRC16.PAS unit for the actual table,   */
/*  or the CRCTable program for computation of the lookup table for the    */
/*  x16 + x15 + x2 + 1 generator polynomial):                              */
/*                                                                         */
/*-------------------------------------------------------------------------*/

const UINT16 CRCTable[256] = { 0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0,
      0x0280, 0xC241, 0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481,
      0x0440, 0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
      0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841, 0xD801,
      0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40, 0x1E00, 0xDEC1,
      0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41, 0x1400, 0xD4C1, 0xD581,
      0x1540, 0xD701, 0x17C0, 0x1680, 0xD641, 0xD201, 0x12C0, 0x1380, 0xD341,
      0x1100, 0xD1C1, 0xD081, 0x1040, 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300,
      0xF3C1, 0xF281, 0x3240, 0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0,
      0x3480, 0xF441, 0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80,
      0xFE41, 0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
      0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41, 0xEE01,
      0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40, 0xE401, 0x24C0,
      0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640, 0x2200, 0xE2C1, 0xE381,
      0x2340, 0xE101, 0x21C0, 0x2080, 0xE041, 0xA001, 0x60C0, 0x6180, 0xA141,
      0x6300, 0xA3C1, 0xA281, 0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501,
      0x65C0, 0x6480, 0xA441, 0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0,
      0x6E80, 0xAE41, 0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881,
      0x6840, 0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
      0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40, 0xB401,
      0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640, 0x7200, 0xB2C1,
      0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041, 0x5000, 0x90C1, 0x9181,
      0x5140, 0x9301, 0x53C0, 0x5280, 0x9241, 0x9601, 0x56C0, 0x5780, 0x9741,
      0x5500, 0x95C1, 0x9481, 0x5440, 0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00,
      0x9FC1, 0x9E81, 0x5E40, 0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0,
      0x5880, 0x9841, 0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81,
      0x4A40, 0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
      0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641, 0x8201,
      0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040 };

UINT16 ByteCRC(UINT8 *Ptr, UINT32 Length)
/*****************************************************************************/
/*                                                                           */
/*   The following is a little cryptic (but executes very quickly).          */
/*   The algorithm is as follows:                                            */
/*   1. exclusive-or the input byte with the low-order byte of               */
/*      the CRC register to get an INDEX                                     */
/*   2. shift the CRC register eight bits to the right                       */
/*   3. exclusive-or the CRC register with the contents of                   */
/*      Table[INDEX]                                                         */
/*   4. repeat steps 1 through 3 for all bytes}                              */
/*                                                                           */
/*****************************************************************************/
{
   UINT16 CRCRegister = 0;
   UINT8 Index;
   while (Length-- > 0) {
      Index = (UINT8) ((CRCRegister ^ *Ptr++) & 0xff);
      CRCRegister = CRCRegister >> 8;
      CRCRegister = CRCRegister ^ CRCTable[Index];
   }
   return (CRCRegister);
}
void SetDHCPServerIPDefaults()
{
   DhcpServerConfiguration config;
   memset(&config, 0, sizeof(config));
   memcpy(config.id, "DHSV", DEFDHCPSERVERID_LEN);
   config.length = sizeof(config);
   config.poolSize = DEFAULTPOOLSIZE;
   config.poolStart = MakeIP(DEFAULTPOOLSTART);
   config.netmask = ClassC();
   config.gateway = MakeIP(DEFAULTGATEWAY);
   config.wins = 0;
   config.dns = MakeIP(DEFAULTIP);
   config.leaseTime = DEFAULTLEASETIME;
   config.renewTime = DEFAULTRENEWTIME;
   config.rebindTime = DEFAULTREBINDTIME;
   sysConfig.systemIP = MakeIP(DEFAULTIP);
   sysConfig.dhcpConfig.dns = MakeIP(DEFAULTIP);
   //Save Itin FRAM
   memcpy(&sysConfig.dhcpConfig, &config, sizeof(config));
}

void SetStaticIPDefaults()
{
   sysConfig.currNetworkConfig = configStatic;
   sysConfig.systemIPStatic.addr = MakeIP(DEFAULTIPSTATIC);
   sysConfig.gw.addr = MakeIP(DEFAULTGATEWAYSTATIC);
   sysConfig.netmask.addr = ClassC();
}

void InitializeDhcpServerConfiguration(int intf, int & priority)
/*************************************************************************************/
/* Description:-  																	 */
/*This function Initializes DHCP Server Configuration.                               */
/* @intf:	Interface used.															 */
/* @priority:	priority	used													 */
/*************************************************************************************/
{
   sysConfig.dhcpConfig.dns = sysConfig.systemIP;
   sysConfig.dhcpConfig.poolStart.addr = (sysConfig.systemIP.addr
         & sysConfig.dhcpConfig.netmask.addr) + htonl(1);
   CyGOS::SetupAddress(intf, sysConfig.systemIP.addr,
         sysConfig.dhcpConfig.netmask.addr);
   CyGOS::SetGW(intf, sysConfig.dhcpConfig.gateway.addr);
   CyGOS::CreateDhcpServerTask(intf, sysConfig.dhcpConfig, priority);
   CyGOS::CreateDNSServerTask(intf, priority);
}

/*************************************************************************************/
/* Description:-  																	 */
/*This function check for the jumper between j3 pin 10 and 4						 */
/*@return: In case the jumper is found returns TRUE else FALSE						 */
/*************************************************************************************/
BOOLEAN CheckForColdStart()
{
   //local variables
   BOOLEAN coldStart = FALSE;
   volatile int delayCnt;
   UINT32 nToggled = 0, toggleCount = 2, indx;
   UINT32 expectedResult = toggleCount * 2;
   UINT8 temp, outVal = 0;
   unsigned char outValArr[NUM_DOUT] = { DOUT1_BIT, DOUT2_BIT, DOUT3_BIT,
         DOUT4_BIT };

   //get current value of op's
   //something that boot loader has written on outputs
   //TODO:Directly reading the DIGITALIOs is giving 0x00??
   temp = (UINT8) __FRAM_START;
   for (indx = 0; indx < 4; indx++) {
      if ((temp & BIT(indx)) == 0)   //FRAM saves op 1 to 4 on bit o - 3
         outVal |= outValArr[indx];   //op data line has bit 7 to 4 of op 1 to 4
   }
   temp = outVal;
   for (indx = 0; indx < toggleCount; indx++) {
      //write 1 on Dig Out 4
      outVal |= DOUT4_BIT;
      DIGITAL_IO_VAL = outVal;
      //insert some random delay before reading back
      for (delayCnt = 0; delayCnt < 10000; delayCnt++)
         ;

      //Read digital input 4
      if (DIGITAL_IO_VAL & ((UINT8) DIN4_BIT))   //If read 1
         nToggled++;   //toggled successfully

      //write 0 on Dig Out 4
      outVal &= ~DOUT4_BIT;
      DIGITAL_IO_VAL = outVal;

      //insert some random delay before reading back
      for (delayCnt = 0; delayCnt < 10000; delayCnt++)
         ;

      //Read digital input 4
      if (!(DIGITAL_IO_VAL & ((UINT8) DIN4_BIT)))   //If read 0
         nToggled++;   //toggled successfully

   }

   //restore outputs that includes dig out 4
   DIGITAL_IO_VAL = temp;
   pprintf("\n restoring ops to value %02X", temp);
   if (nToggled == expectedResult)   //check if toggled successfully
      coldStart = TRUE;

   //print diagnostic
   if (coldStart)
      pprintf("\n jumper found %d", nToggled);
   else
      pprintf("\n jumper not found %d", nToggled);

   return coldStart;
}

/*  void SetupEportForSensingPowerDown(void)
 *
 *  Purpose:
 *     This function sets up edge port module registers to sense 24 Volt
 *     i.e the power for DCP board(From External Source) and 15V from
 *     Instrumentation board.
 *     The sensing line for 24V is connected external interrupt 4 pin of CPU
 *     and  for 15V is connected to external interrupt 7 pin.
 *     The registers setting are done to read the port bit and not configured
 *     as an interrupt.
 *     This function is called by Main() function.
 *
 *  Entry condition: None.
 *
 *  Exit condition: None.
 */
void SetupEportForSensingPowerDown(void)
{
   //Eport pin assignment register settings
   //set pin IRQ4 & IRQ7 to both rising and falling edge trigger.
   //MCF_EPORT_EPPAR |= (MCF_EPORT_EPPAR_EPPA4_BOTH | MCF_EPORT_EPPAR_EPPA7_BOTH);

   //IRQ Pin Assignment Register settings
   MCF_PAD_PAR_IRQ &= ~(MCF_PAD_PAR_IRQ_PAR_IRQ4);   // configure pin as external interrupt/GPIO
   //Eport data direction register settings
   MCF_EPORT_EPDDR &= ~(MCF_EPORT_EPDR_EPD4 | MCF_EPORT_EPDR_EPD7);   // configure as input
   //Eport interrupt enable register settings
   MCF_EPORT_EPIER &= ~(MCF_EPORT_EPIER_EPIE4 | MCF_EPORT_EPIER_EPIE7);   // disable interrupts
}

int main()
{
	UINT8 MacAddr[MAC_ADDRESS_LEN] = {0};
	const UINT8 EthernetInterfaceNumber = 0;
	const UINT16 RtosDelayMilliSecondsTmr = 1000;
	const UINT16 RtosDelayMicroSecondsTmr = 10000;

//	char hostName[50];
   int Priority = 20;

//	printf("\n\n\n\nBuild Date: %s %s\n\n", buildDate, buildTime);
   CyGOS::Init();
   SetupEportForSensingPowerDown();
   PowerUpInitilization();
   if (CheckForColdStart() == TRUE) ColdStart();

   IOTask * AdcIO = new IOTask();
   DupsTask * DupsPtr = new DupsTask(AdcIO);
   CyGOS::CreateTask(AdcIO, Priority);
   CyGOS::CreateTask(DupsPtr, Priority);
   CyGOS::CreateTcpipTasks(Priority);
   CyGOS::CreateTask(new ReceiveCommandHandler(0), Priority);

   ReadMACAddressFromEEPRom(MacAddr);
   for (int IntfCtr = 0; IntfCtr < Board::EthCount(); IntfCtr++) {
      CyGOS::AddTcpInterface(IntfCtr, Priority, MacAddr);
      //TODO below is not valid for more than one ethernet interface
      CyGOS::SetupHostName(IntfCtr, "welder");
      if (sysConfig.systemConfigurationValidCheckSum
            == SYSTEMCONFIGVALIDCHECKSUM) {
         pprintf("\nsys config is valid. current network config = %d ",
               sysConfig.currNetworkConfig);

         if (sysConfig.currNetworkConfig == configDhcpClient) {
            //for future
            //sprintf(hostName,"welder-%02X%02X%02X%02X%02X%02X",
            //	macAddr[0], macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
            //CyGOS::SetupHostName(intf , hostName);
            CyGOS::SetupInterface(IntfCtr, TRUE, TRUE);
         }
         else if (sysConfig.currNetworkConfig == configStatic) {
            CyGOS::SetupAddress(IntfCtr, sysConfig.systemIPStatic.addr,
                  sysConfig.netmask.addr);
            CyGOS::SetGW(IntfCtr, sysConfig.gw.addr);
         }
         else if (sysConfig.currNetworkConfig == configDhcpServer)
            InitializeDhcpServerConfiguration(IntfCtr, Priority);
         else				//Force default again.
         {
            Log::WriteEventLog(CRC_ERROR, (SINT32) sysConfig.currNetworkConfig,
                  (SINT32) configStatic, 5, true);
            SetStaticIPDefaults();
            CyGOS::SetupAddress(IntfCtr, sysConfig.systemIPStatic.addr,
                  sysConfig.netmask.addr);
            CyGOS::SetGW(IntfCtr, sysConfig.gw.addr);
            //something was wrong so set the DHCP server deafults also
            SetDHCPServerIPDefaults();
         }
      }
      else				//default case Static
      {
         pprintf("\n sys config is invalid. restoring default IP setup");
         Log::WriteEventLog(CRC_ERROR, (SINT32) SYSTEMCONFIGVALIDCHECKSUM,
               (SINT32) sysConfig.systemConfigurationValidCheckSum, 5, true);
         //To handle the where user will just go to the LCD and change the configuration to DHCP server
         //we should have right values in FRAM for DHCP server configuration.
         SetDHCPServerIPDefaults();
         SetStaticIPDefaults();
         CyGOS::SetupAddress(IntfCtr, sysConfig.systemIPStatic.addr,
               sysConfig.netmask.addr);
         CyGOS::SetGW(IntfCtr, sysConfig.gw.addr);
         sysConfig.systemConfigurationValidCheckSum = SYSTEMCONFIGVALIDCHECKSUM;
      }

      CyGOS::CreateHttpServerTask(IntfCtr, DCXWEBSITE_PORT, Priority);
      CyGOS::CreateTftpClientTask(IntfCtr, Priority);
   }
   CyGOS::InitFileSystems(Priority);
   CyGOS::CreateBlockDevice(CreateRamDisk(), "A", Priority);
   RegisterTarFS(Priority, Priority + PRIORITYOFFSET);
   Priority += PRIORITYOFFSET + PRIORITYOFFSET;
   //	DecompressTarRegionFlash(0);
   WebSite::InitWebSite(0, DCXWEBSITE_PORT);
   CyGOS::CreateTask(new UploadFirmware(), Priority);
   CyGOS::CreateTask(new FirmwareUpgradeTask(0), Priority);   // to create FirmwareUpgarde task which deals with firmware file transfer from DCP to LCD.
   Task * WldrSearch = new WelderSearchTask(0);
   CyGOS::CreateTask(WldrSearch, Priority);
   CyGOS::CreateTask(new ShutDownTask(), Priority);

   //CyGOS::CreateTask(new InputTask(), Priority);
	CyGOS::CreateHttpServerTask(EthernetInterfaceNumber,WEBSITE_FT_PORT,Priority);
	WebSiteFT::InitWebSite(EthernetInterfaceNumber,WEBSITE_FT_PORT);
	CyGOS::CreateTask(new SR232FT(), Priority);
	CommandsFT::Priority=Priority;
	RTOS::DelayMs(RtosDelayMilliSecondsTmr);
	Task * Eth1Ptr = new BroadcastWR(0);
	RTOS::Create(Eth1Ptr, Priority);
   pprintf("\n timer is set to %d us", DCPTIME);
   CyGOS::InstallTimer(DCPTIME, MSTickFunc, 1);
	pprintf("\n CyGOS::Start\n");
	CyGOS::Start(RtosDelayMicroSecondsTmr);
   return 0;
}
