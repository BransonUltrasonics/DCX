/* $Header:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/BroadcastWR.cpp_v   1.1   Mar 15 2017 11:26:56   hasanchez  $*/
/*****************************************************************************
$Log:   D:/databases/VMdb/archives/DCX/DCP/DCP Application/DCX2010/BroadcastWR.cpp_v  $
 * 
 *    Rev 1.1   Mar 15 2017 11:26:56   hasanchez
 * Added comments and descriptions related to the change to be according the coding standard.
 * 
 *    Rev 1.0   Feb 09 2017 15:15:54   hasanchez
 * Initial revision.
 * 
 *    Rev 1.1   22 Sep 2016 11:44:26   EGUAJARDO
 * Changed the conditional structure of the function Run.
 * 
 *    Rev 1.0   15 Sep 2016 17:23:50   EGUAJARDO
 * Initial revision.
 * 
*/

#include "BroadcastWR.h"
#include "Socket.h"
#include "Board.h"
#include "Ethernet.h"
#include "GLOBALS.h"
#include "Version.h"
#include "LOG.h"
#include "FUNC.h"


#define MAX_WRRESPONSE_SIZE 8000
STR WRResponse[MAX_WRRESPONSE_SIZE];
sockaddr_in ServerAddrPtr, ClientAddrPtr;
SINT32 Sockfeed;
int ClientAddrSizeCrt=sizeof(ClientAddrPtr); //can't change data type, creates conflict with recvfrom network function

WRCommandList wrCommandHandlers[] = {
		{ 0 },
		{ BroadcastWR::EstablishConnection },
		{BroadcastWR::StartSendResults},
		{BroadcastWR::KeepAlive}

};

/*  BroadcastWR::BroadcastWR(SINT32 Eth):Task("BroadcastWR", BROADCAST_WR_STACKSIZE)
 *
 *  Purpose:
 *   	This is the constructor of BroadcastWR class.
 *
 *  Entry condition:
 *		Intf - Ethernet interface the task is running on.
 *
 *  Exit condition:
 *  	None.
 */
BroadcastWR::BroadcastWR(SINT32 Eth):Task("BroadcastWR", BROADCAST_WR_STACKSIZE)
{
	// TODO Auto-generated constructor stub
    this->Eth=Eth;
}

/*  BroadcastWR::~BroadcastWR()
 *
 *  Purpose:
 *   	This is the destructor of BroadcastWR class.
 *
 *  Entry condition:
 *		None.
 *
 *  Exit condition:
 *  	None.
 */
BroadcastWR::~BroadcastWR()
{
	// TODO Auto-generated destructor stub
}

/*  void BroadcastWR::Run(void)
 *
 *  Purpose:
 *		This function implements the forever loop listening for UPD clients
 *
 *  Entry condition:None
 *
 *  Exit condition:None.
 */
void BroadcastWR::Run(void)
{
	STR BufferStr[BROADCAST_WR_BUFFERSIZE];
	SINT32 BroadcastPermissionCtr = BROADCASTPERMISSION;
	memset(BufferStr,0,BROADCAST_WR_BUFFERSIZE);
	UtilsWR::Clean(BufferStr);
	Sockfeed = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (Sockfeed < 0) {
	   printf("socket() failed");
    }
	else
	{
		if (setsockopt(Sockfeed,SOL_SOCKET,SO_BROADCAST,(void*)&BroadcastPermissionCtr,sizeof(BroadcastPermissionCtr)) < 0) {
			printf("setsockopt() failed");
		}
		else
			{
				memset(&ServerAddrPtr,0,sizeof(ServerAddrPtr));
				ServerAddrPtr.sin_family = AF_INET;
				ServerAddrPtr.sin_port = BROADCAST_WR_PORT;
				ClientAddrPtr.sin_family = AF_INET;
				ServerAddrPtr.sin_addr.S_un.S_addr=Board::eth[this->Eth]->ipaddr.addr;
				bind(Sockfeed,(sockaddr*)&ServerAddrPtr,sizeof(ServerAddrPtr));
				listen(Sockfeed,QUEUELENGTH);
				while(TRUE) {
					recvfrom(Sockfeed,BufferStr,BROADCAST_WR_BUFFERSIZE, 0,(sockaddr*)&ClientAddrPtr,&ClientAddrSizeCrt);
					Parser(BufferStr);
					BufferStr[0]=0;
			   }
			}
	}
	closesocket(Sockfeed);
}

/*  void BroadcastWR::Parser(STR * ReceiveStringPtr)
 *
 *  Purpose:
 *		This function parses the received string from the DCX ManagerBasic Application
 *
 *  Entry condition: STR * ReceivedStringPtr
 *  This is the command provided by the DCX ManagerBasic App.
 *
 *  Exit condition:None.
 */
void BroadcastWR::Parser(STR * ReceiveStringPtr)
{
   STR * TempStr = 0;
   SINT32 FuncID = 0, WebCmd = 0;
   WRResponse[0]=0;
   TempStr = strstr(ReceiveStringPtr , "func=");
   if(TempStr != NULL){
      TempStr = strchr(TempStr, EQUALSGN) + 1;
      FuncID = atoi(TempStr);
      TempStr = strstr(ReceiveStringPtr , "cmd=");
      if(TempStr != NULL)
      {
         TempStr = strchr(TempStr, EQUALSGN) + 1;
         WebCmd = atoi(TempStr);
         TempStr = strstr(ReceiveStringPtr , "param=");
         if(TempStr != NULL)
         {
            TempStr = strchr(TempStr, EQUALSGN) + 1;
            if(((UINT32)FuncID > 0) && ((UINT32)FuncID < (sizeof(wrCommandHandlers)/sizeof(wrCommandHandlers[0]))))
            {
               if (wrCommandHandlers[FuncID].handler(TempStr , (WRCommand) WebCmd))
               {

               	if (sendto(Sockfeed,WRResponse,BROADCAST_WR_BUFFERSIZE,0,(sockaddr*)&ClientAddrPtr,ClientAddrSizeCrt) < 0)
               	{
               		pprintf("sendto() failed");
               		WRSendFlag = FALSE;
               	}
               }
            }
         }
      }
   }
}

/*  BOOLEAN BroadcastWR::EstablishConnection(STR * ParamStrPtr, WRCommand Cmd)
 *
 *  Purpose:
 *		This function confirms the communication with DCX ManagerBasic APP and sends the Frequency, Power and Software Version
 *		of the P/S.
 *
 *  Entry condition: STR * ParamStrPtr, WRCommand Cmd
 *
 * 	ParamStrPtr: It is the parameter name sent from the DCX ManagerBasic App.
 * 	Cmd: It is the command number sent by DCX ManagerBasic App.
 *
 *  Exit condition:
 *
 *  RetValue: Confirms that the command was sent successfully
 */
BOOLEAN BroadcastWR::EstablishConnection(STR * ParamStrPtr, WRCommand Cmd)
{
	BOOLEAN RetValueFlag = FALSE;
	if (Cmd == wrOpenConnection) {	//restore IP setup
		strcpy(WRResponse, "Func=3cmd=1?Param=Freq:");			// Pre formatted preset data

		 sprintf(&WRResponse[strlen(WRResponse)], "%d,Power:%d,SoftwareVersion:%s",
				ePromData.PSfrequency, ePromData.PSpower, SWVersionLabel);


		RetValueFlag=TRUE;

	}
	else if(Cmd == wrCloseConnection)
	{

		strcpy(WRResponse, "Func=3cmd=2?Param=");
		RetValueFlag=TRUE;
	}
	return RetValueFlag;
}

/*  void BroadcastWR::SendWR()
 *
 *  Purpose:
 *		This function Sends the weld result each weld cycle finishes or after a configured period.
 *		This is called by DupsTask if a sample period is configured or when a cycle ends.
 *
 *  Entry condition: None
 *
 *  Exit condition: None
 *
 */
void BroadcastWR::SendWR()
{
	LogTime Tm;
	UINT32 Temp;
	STR Time[TIMESTRLENGTH];

	Temp = ReadPowerOnSecCounter();
	CalculateTimeSincePowerUp(Temp, &Tm);

	sprintf(Time, "%02d-%02d-%02d %02d-%02d-%02d", Tm.tm_year,Tm.tm_mon, Tm.tm_day,
	          Tm.tm_hour, Tm.tm_min, Tm.tm_sec);

	strcpy(WRResponse,"");

	if (WeldAmpAverageNumber == 0)     //If the number of samples is 0, should be changed to 1 to avoid the zero division.
		WeldAmpAverageNumber=1;

	if (WRPowerNumber == 0)           //If the number of samples is 0, should be changed to 1 to avoid the zero division.
		WRPowerNumber=1;

	strcpy(WRResponse, "Func=5cmd=1?Param=Time:");			// Pre formatted preset data
	sprintf(&WRResponse[strlen(WRResponse)], "%s,CycleCounter:%d,WeldTime:%d,"
			"WeldPower:%d,WeldEnergy:%d,WeldAmplitude:%d,EndFreq:%d,PeakCurrent:%d,Temperature:%d,ErrorReason:%x,ErrorMessage:%s,",
			Time,
			ReadCycleCounter(),
			CONVMICRO_MILLI(WeldTimer),
			((((WRPower/WRPowerNumber) * SCALING_FACTOR) / ADC11BITVALUE)*(ePromData.PSpower/SCALING_FACTOR)),
			( WeldEnergyResult * ePromData.PSpower/ (ENERGY_MULT_FAC*ADC11BITVALUE) ) ,
			((((WeldAmpAverage/WeldAmpAverageNumber)+10) * SCALING_FACTOR) / ADC11BITVALUE),
			((DDSFrequencyFpga * F_Factor) / WELDFREQFACTOR),
			(( WeldPeakCurrentResult * SCALING_FACTOR) / ADC11BITVALUE),
			(((InputTemperature * F_Factor) / ADC11BITVALUE)),
			ErrorReason,
			ErrorMessageResult);


	if (sendto(Sockfeed,WRResponse,BROADCAST_WR_BUFFERSIZE,0,(sockaddr*)&ClientAddrPtr,ClientAddrSizeCrt) < 0)
	{
		pprintf("sendto() failed");
		CRASH("Error sending data");
	     //WRSendFlag = FALSE;
	}
	WRTimer=0;
	WRPower=0;
	WRPowerNumber=0;
	WeldEnergyResult = 0;
	WeldPeakCurrentResult = 0;
	strcpy(ErrorMessageResult,"");

}

/*  BOOLEAN BroadcastWR::StartSendResults(STR * ParamStrPtr, WRCommand Cmd)
 *
 *  Purpose:
 *		This function confirms the communication with DCX ManagerBasic APP in order to start sending the weld results
 *
 *  Entry condition: STR * ParamStrPtr, WRCommand Cmd
 *
 * 	ParamStrPtr: It is the parameter name sent from the DCX ManagerBasic App.
 * 	Cmd: It is the command number sent by DCX ManagerBasic App.
 *
 *  Exit condition:
 *
 *  RetValue: Confirms that the command was sent successfully
 */
BOOLEAN BroadcastWR::StartSendResults(STR * ParamStrPtr, WRCommand Cmd)
{
	BOOLEAN RetValueFlag = FALSE;
	if (Cmd == wrStartSending) {	//restore IP setup
		strcpy(WRResponse, "Func=4cmd=1?Param=WRFlag:True");			// Pre formatted preset data

		WRSendFlag=TRUE;
		RetValueFlag=TRUE;
		WeldEnergyResult = 0;
		WeldPeakCurrentResult = 0;
		strcpy(ErrorMessageResult,"");

	}
	else if (Cmd == wrStopSending)
	{
		strcpy(WRResponse, "Func=4cmd=2?Param=WRFlag:False");
		WRSendFlag=FALSE;
		RetValueFlag=TRUE;
		WeldEnergyResult = 0;
		WeldPeakCurrentResult = 0;
		strcpy(ErrorMessageResult,"");
	}
	else
	{
		WRSendFlag=FALSE;
		RetValueFlag=FALSE;
		}
	return RetValueFlag;
}


/*  BOOLEAN BroadcastWR::KeepAlive(STR * ParamStrPtr, WRCommand Cmd)
 *
 *  Purpose:
 *		This function confirms that the connection still be established
 *
 *  Entry condition: STR * ParamStrPtr, WRCommand Cmd
 *
 * 	ParamStrPtr: It is the parameter name sent from the DCX ManagerBasic App.
 * 	Cmd: It is the command number sent by DCX ManagerBasic App.
 *
 *  Exit condition:
 *
 *  RetValue: Confirms that the command was sent successfully
 */
BOOLEAN BroadcastWR::KeepAlive(STR * ParamStrPtr, WRCommand Cmd)
{
	BOOLEAN RetValueFlag = FALSE;

	if(Cmd == wrKeepAlive)
	{

		strcpy(WRResponse, "Func=6cmd=1?Param=");
		RetValueFlag=TRUE;
	}

	return RetValueFlag;
}
