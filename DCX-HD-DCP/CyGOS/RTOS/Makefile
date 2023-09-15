# $Header: /ProductionCyGOS/CyGOS/RTOS/Makefile,v 1.1 2010/05/09 00:23:18 Aare Exp $
#
# Copyright (c) Cyber Group, Inc 2007
# This program is the property of Cyber Group Inc
# Copying of this software is expressly forbidden, without the prior written
# consent of Cyber Group Inc
#
# This file is part of CyGOS-3.
#
# This is the RTOS directory make file.
#
# This makefile should be included from the board-specific Makefile.

RTOSCPP = FIFO.cpp RTOS.cpp Task.cpp PrintfTask.cpp Debug.cpp Mutex.cpp \
	Event.cpp MailBox.cpp LedTasks.cpp

# This is a list of header files that must be copied into output directory.
RTOSHDR = Task.h FIFO.h Mutex.h

# This is a relative path to RTOS files from the BSP folder.
RTOSDIR = ../../../../RTOS

RTOSOBJ = $(addprefix $(OBJ_DIR)/RTOS_, \
	$(RTOSCPP:.cpp=_cpp.o) $(RTOSASM:.asm=_asm.o))

INC += $(RTOSDIR)

OBJ += $(RTOSOBJ)

$(OBJ_DIR)/RTOS_%_cpp.o : $(RTOSDIR)/%.cpp $(wildcard $(addsuffix /*.h,$(INC)))
	$(CC) $(CCOPT) -o $@ $<

COPYHDR += $(addprefix $(RTOSDIR)/,$(RTOSHDR))
