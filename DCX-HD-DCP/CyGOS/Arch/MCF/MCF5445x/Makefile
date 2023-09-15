# $Header: /ProductionCyGOS/CyGOS/Arch/MCF/MCF5445x/Makefile,v 1.4 2010/10/26 14:43:17 Noor Exp $
#
# Copyright (c) Cyber Group, Inc 2007
# This program is the property of Cyber Group Inc
# Copying of this software is expressly forbidden, without the prior written
# consent of Cyber Group Inc
#
# This file is part of CyGOS-3.
#
# This is the CPU-specific Makefile for ColdFire MCF5445x processors. Makefile
# lists objects built from C++ and assembly sources specific for all processors
# in MCF5445x series.
#
# This makefile should be included from another Makefile one level deeper,
# specific to the board where the code will execute.

include ../Makefile.cpu

CPUCPP = CPU.cpp Exception.cpp Terminal.cpp PIT.cpp Serial.cpp FEC.cpp \
	MII.cpp MCF5445xTask.cpp Cache.cpp MMU.cpp  DSPI.cpp \
	MultiBootCpu.cpp I2C.cpp WatchDog.cpp

# UsbOtgDevice.cpp SSI.cpp EDMA.cpp CAN.cpp RTC.cpp PCI.cpp

CPUASM = CPU.asm Exception.asm MCF5445xRTOS.asm

CPUOBJ = $(addprefix $(OBJ_DIR)/$(CPU)_, \
	$(CPUCPP:.cpp=_cpp.o) $(CPUASM:.asm=_asm.o))

$(OBJ_DIR)/$(CPU)_%_cpp.o : ../%.cpp $(wildcard $(addsuffix /*.h,$(INC)))
	$(CC) $(CCOPT) -o $@ $<

$(OBJ_DIR)/$(CPU)_%_asm.o : ../%.asm
	$(AS) $(ASOPT) -o $@ $<

OBJ += $(CPUOBJ)
