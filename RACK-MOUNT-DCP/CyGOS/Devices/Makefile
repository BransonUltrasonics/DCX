# $Header: /ProductionCyGOS/CyGOS/Devices/Makefile,v 1.1 2010/09/13 15:54:23 Aare Exp $
#
# Copyright (c) Cyber Group, Inc 2007
# This program is the property of Cyber Group Inc
# Copying of this software is expressly forbidden, without the prior written
# consent of Cyber Group Inc
#
# This file is part of CyGOS-3.
#
# This is the Flash directory make file.
#
# This makefile should be included from the board-specific Makefile.

DEVCPP = TLV5604.cpp MCP3208.cpp AT24C64.cpp 24AA025E48.cpp I2CDevice.cpp AD7689.cpp PCF8563.cpp

DEVHDR = ADC.h DAC.h

# This is a relative path to RTOS files from the BSP folder.
DEVDIR = ../../../../Devices

DEVOBJ = $(addprefix $(OBJ_DIR)/Dev_, $(DEVCPP:.cpp=_cpp.o))

INC += $(DEVDIR)

OBJ += $(DEVOBJ)

$(OBJ_DIR)/Dev_%_cpp.o : $(DEVDIR)/%.cpp $(wildcard $(addsuffix /*.h,$(INC)))
	$(CC) $(CCOPT) -o $@ $<

COPYHDR += $(addprefix $(DEVDIR)/,$(DEVHDR) $(DEVCPP:.cpp=.h))
