# $Header: /ProductionCyGOS/CyGOS/Net/Makefile,v 1.3 2010/09/16 16:43:25 Aare Exp $
#
# Copyright (c) Cyber Group, Inc 2007
# This program is the property of Cyber Group Inc
# Copying of this software is expressly forbidden, without the prior written
# consent of Cyber Group Inc
#
# This file is part of CyGOS-3.
#
# This is the Net directory make file.
#
# This makefile should be included from the board-specific Makefile.

NETCPP = Ethernet.cpp LAN8701.cpp TcpIp.cpp PktBuf.cpp \
	ARP.cpp IPv4.cpp Raw.cpp TCP.cpp UDP.cpp ICMP.cpp IGMP.cpp \
	DhcpClient.cpp AutoIP.cpp DnsClient.cpp \
	Socket.cpp NetBuf.cpp NetConn.cpp \
	TcpServer.cpp TcpConnection.cpp \
	HttpServer.cpp HttpConnection.cpp \
	TftpClient.cpp DNSServer.cpp DhcpServer.cpp KSZ8863RLL.cpp
# HTTPd.cpp	

# This is a list of header files that must be copied into output directory.
NETHDR = Socket.h

# This is a relative path to RTOS files from the BSP folder.
NETDIR = ../../../../Net

NETOBJ = $(addprefix $(OBJ_DIR)/Net_, $(NETCPP:.cpp=_cpp.o))

INC += $(NETDIR)

OBJ += $(NETOBJ)

$(OBJ_DIR)/Net_%_cpp.o : $(NETDIR)/%.cpp $(wildcard $(addsuffix /*.h,$(INC)))
	$(CC) $(CCOPT) -o $@ $<

COPYHDR += $(addprefix $(NETDIR)/,$(NETHDR))
