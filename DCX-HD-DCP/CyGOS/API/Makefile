# $Header: /ProductionCyGOS/CyGOS/API/Makefile,v 1.4 2010/09/21 21:39:17 Aare Exp $
#
# Copyright (c) Cyber Group, Inc 2007
# This program is the property of Cyber Group Inc
# Copying of this software is expressly forbidden, without the prior written
# consent of Cyber Group Inc
#
# This file is part of CyGOS-3.
#
# This is the API directory make file.
#
# This makefile should be included from the board-specific Makefile.

APICPP = CyGOS.cpp TCPIP.cpp LinkedList.cpp CRC.cpp FileSystems.cpp TFTP.cpp \
	HTTP.cpp FileSystems.cpp Decode.cpp

# This is a list of header files that must be copied into output directory.
APIHDR = CyGOS.h DebugNew.h SPI.h

# This is a relative path to RTOS files from the BSP folder.
APIDIR = ../../../../API

APIOBJ = $(addprefix $(OBJ_DIR)/API_, \
	$(APICPP:.cpp=_cpp.o) $(APIASM:.asm=_asm.o))

INC += $(APIDIR)

OBJ += $(APIOBJ)

$(OBJ_DIR)/API_%_cpp.o : $(APIDIR)/%.cpp $(wildcard $(addsuffix /*.h,$(INC)))
	$(CC) $(CCOPT) -o $@ $<

COPYHDR += $(addprefix $(APIDIR)/,$(APIHDR))
