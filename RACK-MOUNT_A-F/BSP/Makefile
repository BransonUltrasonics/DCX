# $Header: $
#
# Copyright (c) Cyber Group, Inc 2015
# This program is the property of Cyber Group Inc
# Copying of this software is expressly forbidden, without the prior written
# consent of Cyber Group Inc
#
# This file is part of CyGOS-3.
#
# This is the Makefile for DCX WC BSP.

include Makefile-$(USERNAME)-$(COMPUTERNAME).mk

# C++ module list
CPP = Board.cpp BoardFlash.cpp BoardNet.cpp LED.cpp

# ASM module list
ASM = Board.asm

BOARDS = WC
CPU_FAMILY = MCF5445x

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

CyGOS = $(value PRODUCTIONCYGOS-$(CPU_FAMILY))

include $(CyGOS)/Makefile.app
include $(CyGOS)/Makefile.arch
include $(CyGOS)/Makefile.cpu

# Because of poor syntax highlighting, this option will make Eclipse believe
# we use regular 'new' operator, but in fact C++ compiler will use debugging
# version.
# To use regular 'new' for compiles, comment this option out
DEBUGNEW = -DCYGOS_USE_DEBUGNEW

# These are the output files, one make file makes them both
LIB = $(OBJ_DIR)/libBSP-$(BOARDS)-$(CPU_FAMILY).a
#DLD = $(PRJ).ld

# Include directories, relative to this directory. Board-specific - this
# directory, CPU-specific - one level up, architecture-specific - two levels up.
# This directory must be the first one in the list because 'include' commands
# below expect this and they will not include the make file from the first word
# in this list, assuming that the first word is this directory.
INC = . $(CyGOS)
# Done including things that are defined only for specific board build.
# Build everything.
all : dirs $(STARTOBJ) $(LIB)
	@echo make: built BSP library for $(CPU_FAMILY)

# Build everything and copy the output files.
allcopy: all copy

# BSP image library, startup object file and all necessary header files go into
# this directory. This is not part of the RTOS project but a directory where
# files for separate project are stored, This other project contains only output
# files from this project, so we can provide access to RTOS binaries and headers
# without involving the source project.
#
# This library is going to be copied into directory that is specified in the
# environment variable CYGOS3-P1A. Variable name must be all upper-case defined 
ENV = BSP-$(BOARDS)-$(CPU_FAMILY)
IMAGE_DIR = $(value $(ENV))

copy: checkdir
	@cmd /c if not exist $(subst /,\\,$(IMAGE_DIR)) md $(subst /,\\,$(IMAGE_DIR))
	@cp -fu *.h $(IMAGE_DIR)
	@cp -fu $(LIB) $(IMAGE_DIR)
	@cp -fu *.ld $(IMAGE_DIR)
	@echo Done copying.

checkdir:
ifeq ($(IMAGE_DIR),)
	@echo.exe
	@echo No RTOS image output directory defined	
	@echo.exe
	@echo Create environment variable BSP-$(BOARDS)-$(CPU_FAMILY) and set
	@echo the value to the directory where RTOS for should be
	@echo copied after it is compiled. Use forward slashes '/' for directory
	@echo separators. 
	@echo.exe
	@exit 1
else	
	@echo $(BOARDS)-$(CPU_FAMILY) output directory is defined as '$(IMAGE_DIR)'
endif

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
else
	@echo Project output directory not defined.
endif

# $(CPUOBJ) $(ARCHOBJ) $(RTOSOBJ) $(STDLIBOBJ)
$(LIB) : $(OBJ)
	@cmd /c if exist $@ rm $@
	$(AR) $(AROPT) $@ $^

# Compiling C++ files.
$(OBJ_DIR)/%_cpp.o : %.cpp $(wildcard $(addsuffix /*.h,$(INC))) $(wildcard $(addsuffix /*.h,../../../$(LIBS)))
	$(CC) $(CCOPT) -o $@ $<

# Compiling ASM files.
$(OBJ_DIR)/%_asm.o : %.asm
	$(AS) $(ASOPT) -o $@ $<
