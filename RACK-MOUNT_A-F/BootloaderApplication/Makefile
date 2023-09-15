
#*
#$Header:$
#*/
#/*****************************************************************************
#$Log:$

#******************************************************************************/
# Copyright (c) Cyber Group, Inc 2007
# This program is the property of Cyber Group Inc
# Copying of this software is expressly forbidden, without the prior written
# consent of Cyber Group Inc
#
# This is the make file to build WC bootloader application.

include Makefile-$(USERNAME)-$(COMPUTERNAME).mk
# First, the global configuration for the project
PRJ = BootLoad

# Target project boards (all in upper case because of Windows).
BOARDS = WC
CPU_FAMILY = MCF5445x

# C++ module list
CPP = $(wildcard *.cpp)

# ASM module list
ASM =

# This conditional will activate if makefile is invoked second time inside this
# make build, with BOARD having one value out of BOARDS list. In main level
# makefile, BOARD is not defined, so board-specific makefile components cannot
# be included in here.
ifneq ($(BOARD),)
BINARY = $(BOARD).bin
# RTOS folder location. This folder includes the RTOS library and all related
# include files for the C++ compiler.

#this path can be anything need to be decided by application programmer.
#He/She just need to copy all header and library files in the path mentioned here
CyGOS = $(value PRODUCTIONCYGOS-$(CPU_FAMILY))
BSP = $(value BSP-$(BOARDS)-$(CPU_FAMILY))

# Include directories, relative to this directory.
INC = . $(CyGOS) $(BSP)

# This is an optional variable. If defined, the binary file built by this
# project will be copied into specified file.

BINARY = BootLoad_$(BOARDS).bin

include $(CyGOS)/Makefile.boot
include $(CyGOS)/Makefile.arch
include $(CyGOS)/Makefile.cpu

# Done including things that are defined only for specific board build.
endif

all:
	touch Main.cpp
	@cmd /c "for %i in ($(BOARDS)) do @cs-make --no-print-directory BOARD=%i board-all"

allcopy:
	touch Main.cpp
	@cmd /c "for %i in ($(BOARDS)) do @cs-make --no-print-directory BOARD=%i board-allcopy"
	@cp -fu *.bin $(value PRODUCTIONCYGOS-$(CPU_FAMILY))

clean:
	@cmd /c "for %i in ($(BOARDS)) do @cs-make --no-print-directory BOARD=%i board-clean"
