# $Header: /Aaredcx2010/BransonCyGOS/Arch/MCF/MCF5445x/DCP1C/Makefile,v 1.1 2010/12/03 17:06:38 Aare Exp $
#
# Copyright (c) Cyber Group, Inc 2007
# This program is the property of Cyber Group Inc
# Copying of this software is expressly forbidden, without the prior written
# consent of Cyber Group Inc
#
# This file is part of CyGOS-3.
#
# This is the make file to build the board-specific RTOS image library.

# This is the target hardware board name. Name must be in upper case even if
# appropriate folders can be in mixed case.
BOARD = DCPRM

# C++ module list
CPP = Board.cpp LED.cpp BoardNet.cpp BoardFlash.cpp MultiBootBoard.cpp

# ASM module list
ASM = Board.asm

# Because of poor syntax highlighting, this option will make Eclipse believe
# we use regular 'new' operator, but in fact C++ compiler will use debugging
# version.
# To use regular 'new' for compiles, comment this option out
DEBUGNEW = -DCYGOS_USE_DEBUGNEW

# From here on things don't need to be changed frequently...

# First, the project name. Project name is used to specify the library name.
PRJ = RTOS-$(BOARD)

# RTOS image library, startup object file and all necessary header files go into
# this directory. This is not part of the RTOS project but a directory where
# files for separate project are stored, This other project contains only output
# files from this project, so we can provide access to RTOS binaries and headers
# without involving the source project.
#
# This library is going to be copied into directory that is specified in the
# environment variable CYGOS3-P1A. Variable name must be all upper-case defined 
ENV = PRODUCTIONCYGOS-$(BOARD)
IMAGE_DIR = $(value $(ENV))

# These are the output files, one make file makes them both
LIB = $(OBJ_DIR)/lib$(PRJ).a
DLD = $(PRJ).ld

# Temporary object directory. All C++ and ASM objects go there. C++ and ASM
# modules will have _cpp and _asm inserted into their file names when compiled,
# so same name can be used for both modules if they implement functions for the
# same object. Also, files from different source folders have the folder name
# inserted into their object file name, so it is safe for multiple source files
# have same file name. However, all include files must have unique file names
# because they will be piled up into one output directory for this platform
# hardware library.
OBJ_DIR	= obj

# This is a list of platform-independent libraries that are compiled into this
# version of CyGOS.
LIBS = stdlib API RTOS Net Flash BlockDevices FileSystems MultiBoot Devices
# Display CANopen

# Object file list, list is built from C++ and asm files, prefixed with object
# file directory, and extensions replaced with ".o"
OBJ	= $(addprefix $(OBJ_DIR)/,$(CPP:.cpp=_cpp.o) $(ASM:.asm=_asm.o))

# Include directories, relative to this directory. Board-specific - this
# directory, CPU-specific - one level up, architecture-specific - two levels up.
# This directory must be the first one in the list because 'include' commands
# below expect this and they will not include the make file from the first word
# in this list, assuming that the first word is this directory.
INC = . .. ../..

# Now include makefiles from all folders that must go into the platform build.
# Each makefile will add include directories and their output object names to
# the lists that were started in this directory.

# First two are CPU-specific - one level up and architecture-specific - two
# levels up. Actual values are taken from INC variable, ignoring the first
# include directory so that we don't include same make file over and over.
include $(patsubst %,%/Makefile,$(wordlist 2,$(words $(INC)),$(INC)))

# Now the ones built from common libraries...
include $(patsubst %,../../../../%/Makefile,$(LIBS))

# dirs - create an empty directory if we have to
dirs:
	@cmd /c if not exist $(OBJ_DIR) md $(OBJ_DIR)

# Build everything.
all : dirs $(STARTOBJ) $(LIB)
	@echo make: built targets for $(BOARD)

# Build everything and copy the output files.
allcopy: all copy

# Compiling C++ files.
$(OBJ_DIR)/%_cpp.o : %.cpp $(wildcard $(addsuffix /*.h,$(INC))) $(wildcard $(addsuffix /*.h,../../../../$(LIBS)))
	$(CC) $(CCOPT) -o $@ $<

# Compiling ASM files.
$(OBJ_DIR)/%_asm.o : %.asm
	$(AS) $(ASOPT) -o $@ $<

# Clean - delete objects and other output files.
# Makefiles are copied into output directory because application and boot loader
# makefiles require these makefiles to operate, so technically, 'clean' is not
# 'completely clean'.
clean :
ifneq ($(OBJ_DIR),)
	@echo Removing temporary files from $(BOARD) build
	@echo empty >$(OBJ_DIR)/empty
	rm -fv $(OBJ_DIR)/*
else
	@echo Object directory not defined.
endif
ifneq ($(IMAGE_DIR),)
	@echo Cleaning project output directory for $(BOARD) build
	@echo empty >$(IMAGE_DIR)/empty
	rm -fv $(IMAGE_DIR)/*
	@cp -fu ../../../../Makefile.app $(IMAGE_DIR)
	@cp -fu ../../../../Makefile.boot $(IMAGE_DIR)
	@cp -fu ../../Makefile.arch $(IMAGE_DIR)
	@cp -fu ../Makefile.cpu $(IMAGE_DIR)
else
	@echo Project output directory not defined.
endif

# $(CPUOBJ) $(ARCHOBJ) $(RTOSOBJ) $(STDLIBOBJ)
$(LIB) : $(OBJ) $(O)
	@cmd /c if exist $@ rm $@
	$(AR) $(AROPT) $@ $^

copy: checkdir
	@cmd /c if not exist $(subst /,\\,$(IMAGE_DIR)) md $(subst /,\\,$(IMAGE_DIR))
	@cmd /c "for %i in (. .. ../..) do @cp -fu %i/*.h $(IMAGE_DIR)"
	@cp -fu ../../../../stdlib/*.h $(IMAGE_DIR)
	@cp -fu ../../../../FileSystems/*.h $(IMAGE_DIR)
	@cp -fu ../../../../Net/*.h $(IMAGE_DIR)
	@cp -fu ../../../../BlockDevices/*.h $(IMAGE_DIR)
	@cp -fu ../../../../API/*.h $(IMAGE_DIR)
	@cp -fu ../../../../RTOS/*.h $(IMAGE_DIR)
	@cp -fu $(COPYHDR) $(IMAGE_DIR)
	@cp -fu ../../../../Makefile.app $(IMAGE_DIR)
	@cp -fu ../../../../Makefile.boot $(IMAGE_DIR)
	@cp -fu ../../Makefile.arch $(IMAGE_DIR)
	@cp -fu ../Makefile.cpu $(IMAGE_DIR)
	@cp -fu $(LIB) $(IMAGE_DIR)
	@cp -fu *.ld $(IMAGE_DIR)
	@cp -fu $(STARTOBJ) $(IMAGE_DIR)/Start.o
	@echo Done copying.

checkdir:
ifeq ($(IMAGE_DIR),)
	@echo.exe
	@echo No RTOS image output directory defined
	@echo.exe
	@echo Create environment variable ProductionCYGOS3-$(BOARD) and set
	@echo the value to the directory where RTOS for Platform1A board should be
	@echo copied after it is compiled. Use forward slashes '/' for directory
	@echo separators. 
	@echo.exe
	@exit 1
else
	@echo $(PRJ) output directory is defined as '$(IMAGE_DIR)'
endif

# This batch file creates documentation for everything in the source tree and
# also creates source web pages for every source file in the project. This is
# the 'developer' documentation. Separate target must be used for 'user'
# documentation that only includes comments for public and protected items in
# the files, and no source code.
doc: ../../../../MakeDoc.bat
#	@echo $(foreach var,$(LIBS),include ../../../../$(var)/Makefile)
	@cmd /c $(subst /,\\\\,$< "CyGOS-3 - Platform1A" INCLUDE_PRIVATE \
		$(abspath $(IMAGE_DIR))/doc/index.htm \
		$(abspath ../../../..)/Arch/MCF/MCF5445x/M54455EVB \
		$(abspath ../../../..)/Arch/MCF/MCF5445x \
		$(abspath ../../../..)/Arch/MCF \
		$(abspath ../../../..)/Arch \
		$(abspath ../../../..)/BlockDevices \
		$(abspath ../../../..)/Devices \
		$(abspath ../../../..)/CANOpen \
		$(abspath ../../../..)/FileSystems \
		$(abspath ../../../..)/RTOS \
		$(abspath ../../../..)/stdlib \
		)
