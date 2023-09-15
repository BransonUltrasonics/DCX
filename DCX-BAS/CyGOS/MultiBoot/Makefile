# $Header: /ProductionCyGOS/CyGOS/MultiBoot/Makefile,v 1.1 2010/05/11 22:32:11 Aare Exp $
#
# Copyright (c) Cyber Group, Inc 2007
# This program is the property of Cyber Group Inc
# Copying of this software is expressly forbidden, without the prior written
# consent of Cyber Group Inc
#
# This file is part of CyGOS-3 MultiBoot loader.

MBOOTCPP = SerialBootTask.cpp

# This is a list of header files that must be copied into output directory.
MBOOTHDR = SerialBootTask.h

# This is a relative path to RTOS files from the BSP folder.
MBOOTDIR = ../../../../MultiBoot

MBOOTOBJ = $(addprefix $(OBJ_DIR)/MBoot_, \
	$(MBOOTCPP:.cpp=_cpp.o))

$(OBJ_DIR)/MBoot_%_cpp.o : ../../../../MultiBoot/%.cpp $(wildcard $(addsuffix /*.h,$(INC)))
	$(CC) $(CCOPT) -o $@ $<

OBJ += $(MBOOTOBJ)
COPYHDR += $(addprefix $(MBOOTDIR)/,$(MBOOTHDR))