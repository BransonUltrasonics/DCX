# $Header: /ProductionCyGOS/CyGOS/FileSystems/Makefile,v 1.2 2010/05/25 23:28:17 Aare Exp $
#
# Copyright (c) Cyber Group, Inc 2007
# This program is the property of Cyber Group Inc
# Copying of this software is expressly forbidden, without the prior written
# consent of Cyber Group Inc
#
# This file is part of CyGOS-3.
#
# This is the Block Devices directory make file.
#
# This makefile should be included from the board-specific Makefile.

FILESYSCPP = FileSystem.cpp TarFileSystem.cpp FRAMFileSystem.cpp

# This is a relative path to Block Devices files from the BSP folder.
FILESYSDIR = ../../../../FileSystems

FILESYSOBJ = $(addprefix $(OBJ_DIR)/FileSys_, $(FILESYSCPP:.cpp=_cpp.o))

INC += $(FILESYSDIR)

OBJ += $(FILESYSOBJ)

$(OBJ_DIR)/FileSys_%_cpp.o : $(FILESYSDIR)/%.cpp $(wildcard $(addsuffix /*.h,$(INC)))
	$(CC) $(CCOPT) -o $@ $<
