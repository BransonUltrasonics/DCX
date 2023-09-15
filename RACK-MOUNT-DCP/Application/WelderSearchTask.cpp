/* $Header:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/WelderSearchTask.cpp_v   1.3   15 Sep 2016 17:16:48   EGUAJARDO  $*/
/*****************************************************************************
 $Log:   D:/databases/VMdb/archives/DCX/RackMount/DCP/Application/WelderSearchTask.cpp_v  $
 * 
 *    Rev 1.3   15 Sep 2016 17:16:48   EGUAJARDO
 * Made WRSendFlag to false when ethernet cable is disconnected.
 * 
 *    Rev 1.2   06 Jun 2016 16:53:54   GButron
 * V1.3.7A FT Commands
 * 
 *    Rev 1.1   19 Mar 2015 07:16:04   sesharma
 * Branson Coding Standard Applied
 * 
 *    Rev 1.0   18 Aug 2014 09:17:34   amaurya
 * Initial revision.
 * 
 *    Rev 1.0   18 Aug 2014 09:13:08   amaurya
 * Initial revision.
 */

#include "WelderSearchTask.h"
#include "Socket.h"
#include "errno.h"
#include "stdio.h"
#include "trace.h"
#include "CyGOS.h"
#include "GLOBALS.h"
#include "TYP_DEF.h"

#define WELDERSEARCHTASK_STACKSIZE 512*8
#define WELDERINFO_BROADCAST_INTERVAL 20000000 //in microseconds 20 seconds
#define WELDERINFOPACKET_TIMEOUT 60000000 //in microseconds 1 minute
#define WELDERINFOREFRESH_TIMEOUT 30000000//in microseconds 30 secs
#define WELDERSEARCHMSG_SOCKET_TIMEOUT 100000

WelderSearchTask * WelderSearchTask::thiPtr;
WelderSearchTask::WelderSearchTask(int intf) :
		Task("welder Search", WELDERSEARCHTASK_STACKSIZE), msgQ(10, "msgQ") {
	netif = CyGOS::GetNetworkInterface(intf);
	fd = INVALID_SOCKET;
	thiPtr = this;
	memset(&exc_set, 0, sizeof(fd_set));
	memset(&input_set, 0, sizeof(fd_set));
}

/*This function first checks for data on UDP port WELDERSEARCH_UDP_PORT.
 * and process it accordingly. It then processes the msgQ.
 */
void WelderSearchTask::Run() {
	DelayMs(2000);
	sockaddr_in sa;
	bool running = false;
	int sa_len = sizeof(sa), len = 0, flags = 0;
	WelderInfoParam msg;
	SearchMsg sMsg;
	for (;;) {
		if (!netif->up) {
			DelayMs(100);
			running = false;
			WRSendFlag = FALSE;
		} else {
			if (!running) {
				CloseSocket();
				CreateAndBindSocket();
				NonBlock();
				running = true;
			}
			PrepareFDSets();
			Poll();
			FD_ISSET(fd, &input_set);
			FD_ISSET(fd, &exc_set);
			if ((FD_ISSET(fd, &input_set) != 0) && (FD_ISSET(fd, &exc_set) == 0)) {
				len = recvfrom(fd, &msg.WelderInfo, sizeof(msg.WelderInfo), flags,
				      (sockaddr *) &sa, &sa_len);

				if (len >= 0) {
					if (msg.WelderInfo.checkSum == WELDERSEARCH_CHECKSUM) {
						msg.ipaddr.addr = sa.sin_addr.S_un.S_addr;
						AddWelderInfo(msg);
					}
				}
			}

			if (msgQ.Read(this, sMsg, 0)) {
				switch (sMsg) {
				case broadcastMsg:
					BroadcastwelderInfo();
					break;
				case refreshMsg:
					RefreshWelderInfo();
					break;
				default:
					break;
				}
			}
		}
	}

}

/*
 * This function create and bind a UDP socket
 * on port WELDERSEARCH_UDP_PORT.
 */
void WelderSearchTask::CreateAndBindSocket() {
	fd = socket(AF_INET, SOCK_DGRAM, PF_INET);
	ASSERT(fd != INVALID_SOCKET);
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(WELDERSEARCH_UDP_PORT);
	sa.sin_addr.s_addr = netif->ipaddr.addr;
	if (bind(fd, (sockaddr *) &sa, sizeof(sa)) < 0)
		CRASH("Bind welderSearch server socket");
}

/*
 * This fuction is called from Run() in case the link is lost
 * (i.e network cable is unplugged)
 * It closes the socket initially binded at 49500 UDP port
 */
void WelderSearchTask::CloseSocket() {
	if (fd != INVALID_SOCKET) {
		lwip_close(fd);
		fd = INVALID_SOCKET;
	}
}

/*
 This function makes the socket binded at 49500 nonblocking
 */
int WelderSearchTask::NonBlock() {
	unsigned long val = 1;
	return lwip_ioctl(fd, FIONBIO, &val);
}

/*
 * Prepare input and error FD sets to be used in socket
 * recvfrom function.
 */
void WelderSearchTask::PrepareFDSets() {
	FD_ZERO(&input_set);
	FD_ZERO(&exc_set);
	FD_SET(fd, &input_set);
}

/*
 * This function waits for something to receive on port 49500
 * for WELDERSEARCHMSG_SOCKET_TIMEOUT uSecs. It returns true
 * if select socket API executes without any error otherwise returns
 * false.
 */
bool WelderSearchTask::Poll() {
	timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = WELDERSEARCHMSG_SOCKET_TIMEOUT;
	if (lwip_select(fd + 1, &input_set, 0, &exc_set, &tv) < 0) {
		return false;
	}
	return true;
}

/*
 * This function  is called from Run() in case welder information
 * packet is received on port 49500. The function add new welders
 * IP and serial number to welderInfoList or refreshes the
 * timeout to WELDERINFOPACKET_TIMEOUT in case the welder info is already present
 * in the list.
 */
void WelderSearchTask::AddWelderInfo(WelderInfoParam info) {
	//Todo:Should it be mutex protected because cause we are
	//receiving this list through website by direct function call and not
	//through q processing

	CriticalSection s;
	bool newWelderFound = true;
	WelderInfoParam * wldrInfo = 0;
	POSITION pos = welderInfoList.GetHeadPosition();
	while (pos && newWelderFound) {
		wldrInfo = welderInfoList.GetNext(pos);
		if (wldrInfo->ipaddr.addr == info.ipaddr.addr) {
			newWelderFound = false;
			wldrInfo->searchTimeOut = WELDERINFOPACKET_TIMEOUT;
		}
	}
	if (newWelderFound) {
		wldrInfo = new WelderInfoParam;
		memcpy(wldrInfo->WelderInfo.SeriallNumber, info.WelderInfo.SeriallNumber,
		SIZE_SERAILNUMBER);
		wldrInfo->ipaddr = info.ipaddr;
		wldrInfo->searchTimeOut = WELDERINFOPACKET_TIMEOUT;
		welderInfoList.AddTail(wldrInfo);
	}
}

/*
 * This function is called every WELDERINFOREFRESH_TIMEOUT. It checks the
 * searchTimeOut of all welders in welder information list.
 * If timeout is found to be zero or less than zero function
 * removes the welder information from list.
 */
void WelderSearchTask::RefreshWelderInfo() {
	//Todo:Should it be mutex protected because cause we are
	//receiving this list through website by direct function call and not
	//through q processing
	WelderInfoParam * wldrInfo = 0;
	POSITION pos = welderInfoList.GetHeadPosition(), oldPos;
	while (pos) {
		oldPos = pos;
		wldrInfo = welderInfoList.GetNext(pos);
		if (wldrInfo->searchTimeOut <= 0) {
			welderInfoList.DeleteAt(oldPos);
		}
	}
}

/*
 * This function is called from Run(). It broadcasts the welder information(i.e
 * IP and serial number) on UDP port 495000.
 */
void WelderSearchTask::BroadcastwelderInfo() {
	WelderInfoParam msg;
	sockaddr_in sa;

	memcpy(msg.WelderInfo.SeriallNumber, EPromData.SerialNo, SIZE_SERAILNUMBER);

	//Assuming max serial number of 20 bytes. As it is a string null the last byte
	//if in some accidental case the same contains some garbage.
	msg.WelderInfo.SeriallNumber[SIZE_SERAILNUMBER - 1] = 0;
	msg.WelderInfo.checkSum = htons(WELDERSEARCH_CHECKSUM);	//check sum

	sa.sin_family = AF_INET;
	sa.sin_port = htons(WELDERSEARCH_UDP_PORT);
	sa.sin_addr.s_addr = 0xFFFFFFFF;	//need to broadcast the packet
	//Here we are broadcasting out information. It gets back to us on port
	//49500 if welder is connected to some HUB or switch. As a result input_set gets fired
	//and from Run() function AddWelderInfo() gets called. So in current implementation
	//we may have or may have not our own information on the welderInfoList. Everything in
	//this list is shown in the drop down list of manufacturing web page.
	sendto(fd, &msg.WelderInfo, sizeof(msg.WelderInfo), 0, (sockaddr *) &sa,
	      sizeof(sa));
}

/*
 * This function is called from website module. It returns the number
 * of welders currently in the network.
 * @return: Number of welders in the network
 */
int WelderSearchTask::GetWelderCount() {
	return thiPtr->welderInfoList.GetCount();
}

/*
 * This function is called from website module.It copies the serial number
 * and IPs of all welders currently in welderInfoList to passed InfoPtr.
 * @param InfoPtr: Pointer to the array of WelderInfoParam having size
 * equal to number of welders currently in the network(i.e. welderInfoList.GetCount()).
 */
void WelderSearchTask::GetWeldersInformation(WelderInfoParam * InfoPtr) {
	WelderInfoParam * wldrInfo = 0;
	int indx = 0;
	POSITION pos = thiPtr->welderInfoList.GetHeadPosition();
	while (pos) {
		wldrInfo = thiPtr->welderInfoList.GetNext(pos);
		memcpy(&InfoPtr[indx++], wldrInfo, sizeof(WelderInfoParam));
	}
}

/*
 * This function is called on every RTOS tick.
 * On every WELDERINFO_BROADCAST_INTERVAL it marks the sending of welder
 * information broadcast by writing into msgQ. On every WELDERINFOREFRESH_TIMEOUT
 * interval it marks to refresh the welderInfoList by writing into msgQ.
 */
void WelderSearchTask::Tick() {
	static int usTickBroadcast = 0, usTickRefresh = 0;
	WelderInfoParam * wldrInfo;	//, searchMsg;
	usTickBroadcast += usecPerTick;
	usTickRefresh += usecPerTick;
	if (usTickBroadcast >= WELDERINFO_BROADCAST_INTERVAL) {
		usTickBroadcast -= WELDERINFO_BROADCAST_INTERVAL;
		POSITION pos = welderInfoList.GetHeadPosition();
		while (pos) {
			wldrInfo = welderInfoList.GetNext(pos);
			if (wldrInfo->searchTimeOut > 0)
				wldrInfo->searchTimeOut -= WELDERINFO_BROADCAST_INTERVAL;
		}
		if (netif->up) {
			msgQ.Write(0, broadcastMsg, 0);//20 secs are UP ..mark the broadcast of welder information
		}
	}
	if (usTickRefresh >= WELDERINFOREFRESH_TIMEOUT) {
		usTickRefresh -= WELDERINFOREFRESH_TIMEOUT;
		if (netif->up) {
			msgQ.Write(0, refreshMsg, 0);	//ever 30 secs check whether a welder has been removed from network
		}
	}
}

