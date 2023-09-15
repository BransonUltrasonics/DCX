# $Header: /ProductionCyGOS/CyGOS/BlockDevices/Makefile,v 1.2 2010/05/19 21:17:01 Aare Exp $
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

BLOCKDEVCPP = BlockDevice.cpp CacheBlock.cpp FileSystemManager.cpp \
	RamDiskDevice.cpp FRAMDiskDevice.cpp

# SDCardTask.cpp NandFlashTask.cpp \
	LargeNandFlashTask.cpp BlockDeviceTestTask.cpp

# This is a relative path to Block Devices files from the BSP folder.
BLOCKDEVDIR = ../../../../BlockDevices

BLOCKDEVOBJ = $(addprefix $(OBJ_DIR)/BlkDev_, $(BLOCKDEVCPP:.cpp=_cpp.o))

INC += $(BLOCKDEVDIR)

OBJ += $(BLOCKDEVOBJ)

$(OBJ_DIR)/BlkDev_%_cpp.o : $(BLOCKDEVDIR)/%.cpp $(wildcard $(addsuffix /*.h,$(INC)))
	$(CC) $(CCOPT) -o $@ $<
