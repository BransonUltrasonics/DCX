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

include ../../../Makefile-$(USERNAME)-$(COMPUTERNAME).mk

# C++ module list
CPP =

# ASM module list
ASM =

# Temporary object directory. All C++ and ASM objects go there. C++ and ASM
# modules will have _cpp and _asm inserted into their file names when compiled,
# so same name can be used for both modules if they implement functions for the
# same object. Also, files from different source folders have the folder name
# inserted into their object file name, so it is safe for multiple source files
# have same file name. However, all include files must have unique file names
# because they will be piled up into one output directory for this platform
# hardware library.
OBJ_DIR	= obj

# Object file list, list is built from C++ and asm files, prefixed with object
# file directory, and extensions replaced with ".o"
OBJ	= $(addprefix $(OBJ_DIR)/,$(CPP:.cpp=_cpp.o) $(ASM:.asm=_asm.o))

include Makefile.cpu

CPUCPP = CPU.cpp Exception.cpp Terminal.cpp PIT.cpp Serial.cpp FEC.cpp \
	MII.cpp MCF5445xTask.cpp Cache.cpp MMU.cpp  DSPI.cpp \
	MultiBootCpu.cpp I2C.cpp WatchDog.cpp

# UsbOtgDevice.cpp SSI.cpp EDMA.cpp CAN.cpp RTC.cpp PCI.cpp

CPUASM = CPU.asm Exception.asm MCF5445xRTOS.asm

CPUOBJ = $(addprefix $(OBJ_DIR)/$(CPU)_, \
	$(CPUCPP:.cpp=_cpp.o) $(CPUASM:.asm=_asm.o))

$(OBJ_DIR)/$(CPU)_%_cpp.o : %.cpp $(wildcard $(addsuffix /*.h,$(INC)))
	$(CC) $(CCOPT) -o $@ $<

$(OBJ_DIR)/$(CPU)_%_asm.o : %.asm
	$(AS) $(ASOPT) -o $@ $<

OBJ += $(CPUOBJ)

# Because of poor syntax highlighting, this option will make Eclipse believe
# we use regular 'new' operator, but in fact C++ compiler will use debugging
# version.
# To use regular 'new' for compiles, comment this option out
DEBUGNEW = -DCYGOS_USE_DEBUGNEW

# These are the output files, one make file makes them both
LIB = $(OBJ_DIR)/libRTOS-$(CPU_FAMILY).a
DLD = $(PRJ).ld

# This is a list of platform-independent libraries that are compiled into this
# version of CyGOS.
LIBS = stdlib API RTOS Net Flash BlockDevices FileSystems Devices
# Display CANopen

# Include directories, relative to this directory. Board-specific - this
# directory, CPU-specific - one level up, architecture-specific - two levels up.
# This directory must be the first one in the list because 'include' commands
# below expect this and they will not include the make file from the first word
# in this list, assuming that the first word is this directory.
INC = . ..

# Now include makefiles from all folders that must go into the platform build.
# Each makefile will add include directories and their output object names to
# the lists that were started in this directory.

# First two are CPU-specific - one level up and architecture-specific - two
# levels up. Actual values are taken from INC variable, ignoring the first
# include directory so that we don't include same make file over and over.
include $(patsubst %,%/Makefile,$(wordlist 2,$(words $(INC)),$(INC)))

# Now the ones built from common libraries...
include $(patsubst %,../../../%/Makefile,$(LIBS))

# Build everything.
all : dirs $(STARTOBJ) $(LIB)
	@echo make: built library for $(TARGET)

# Build everything and copy the output files.
allcopy: all copy

# RTOS image library, startup object file and all necessary header files go into
# this directory. This is not part of the RTOS project but a directory where
# files for separate project are stored, This other project contains only output
# files from this project, so we can provide access to RTOS binaries and headers
# without involving the source project.
#
# This library is going to be copied into directory that is specified in the
# environment variable CYGOS3-P1A. Variable name must be all upper-case defined 
ENV = PRODUCTIONCYGOS-$(CPU_FAMILY)
IMAGE_DIR = $(value $(ENV))

copy: checkdir
	@cmd /c if not exist $(subst /,\\,$(IMAGE_DIR)) md $(subst /,\\,$(IMAGE_DIR))
	@cmd /c "for %i in (. ..) do @cp -fu %i/*.h $(IMAGE_DIR)"
	@cp -fu ../../../stdlib/*.h $(IMAGE_DIR)
	@cp -fu ../../../FileSystems/*.h $(IMAGE_DIR)
	@cp -fu ../../../Net/*.h $(IMAGE_DIR)
	@cp -fu ../../../BlockDevices/*.h $(IMAGE_DIR)
	@cp -fu ../../../API/*.h $(IMAGE_DIR)
	@cp -fu ../../../RTOS/*.h $(IMAGE_DIR)	
	@cp -fu ../../../Flash/*.h $(IMAGE_DIR)
	@cp -fu ../../../Devices/*.h $(IMAGE_DIR)	
	@cp -fu $(COPYHDR) $(IMAGE_DIR)
	@cp -fu ../../../Makefile.app $(IMAGE_DIR)
	@cp -fu ../../../Makefile.boot $(IMAGE_DIR)
	@cp -fu ../Makefile.arch $(IMAGE_DIR)
	@cp -fu Makefile.cpu $(IMAGE_DIR)
	@cp -fu $(LIB) $(IMAGE_DIR)
	@cp -fu libgcc.a $(IMAGE_DIR)
#@cp -fu *.ld $(IMAGE_DIR)
	@cp -fu $(STARTOBJ) $(IMAGE_DIR)/Start.o
	@echo Done copying.

checkdir:
ifeq ($(IMAGE_DIR),)
	@echo.exe
	@echo No RTOS image output directory defined	
	@echo.exe
	@echo Create environment variable PRODUCTIONCYGOS-$(CPU_FAMILY) and set
	@echo the value to the directory where RTOS for should be
	@echo copied after it is compiled. Use forward slashes '/' for directory
	@echo separators. 
	@echo.exe
	@exit 1
else	
	@echo $(PRJ) output directory is defined as '$(IMAGE_DIR)'
endif

# dirs - create an empty directory if we have to
dirs:
	@cmd /c if not exist $(OBJ_DIR) md $(OBJ_DIR)

# Clean - delete objects and other output files.
# Makefiles are copied into output directory because application and boot loader
# makefiles require these makefiles to operate, so technically, 'clean' is not
# 'completely clean'.
clean :
ifneq ($(OBJ_DIR),)
	@echo Removing temporary files from $(TARGET) build
	@cmd /c if not exist $(OBJ_DIR) md $(OBJ_DIR)
	@echo empty >$(OBJ_DIR)/empty
	@rm -fv $(OBJ_DIR)/*
	@rmdir $(OBJ_DIR)
else
	@echo Object directory not defined.
endif
ifneq ($(IMAGE_DIR),)
	@echo Cleaning project output directory for $(TARGET) build
	@echo empty >$(IMAGE_DIR)/empty
	@rm -fv $(IMAGE_DIR)/*
	@cp -fu ../../../Makefile.app $(IMAGE_DIR)
	@cp -fu ../../../Makefile.boot $(IMAGE_DIR)
	@cp -fu ../Makefile.arch $(IMAGE_DIR)
	@cp -fu Makefile.cpu $(IMAGE_DIR)
else
	@echo Project output directory not defined.
endif

# $(CPUOBJ) $(ARCHOBJ) $(RTOSOBJ) $(STDLIBOBJ)
$(LIB) : $(OBJ) $(O)
	@cmd /c if exist $@ rm $@
	$(AR) $(AROPT) $@ $^

# Compiling C++ files.
$(OBJ_DIR)/%_cpp.o : %.cpp $(wildcard $(addsuffix /*.h,$(INC))) $(wildcard $(addsuffix /*.h,../../../$(LIBS)))
	$(CC) $(CCOPT) -o $@ $<

# Compiling ASM files.
$(OBJ_DIR)/%_asm.o : %.asm
	$(AS) $(ASOPT) -o $@ $<

