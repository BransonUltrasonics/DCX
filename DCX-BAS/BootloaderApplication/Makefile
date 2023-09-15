
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
# This is the make file to build WC application.

# First, the global configuration for the project
PRJ = BootLoad

# Target project boards (all in upper case because of Windows).
BOARDS =
BOARDS += WC

# C++ module list
CPP = Main.cpp

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
CyGOS = $(value PRODUCTIONCYGOS-$(BOARD))

# Include directories, relative to this directory.
INC = . $(CyGOS)

# This is an optional variable. If defined, the binary file built by this
# project will be copied into specified file.

BINARY = BootLoad_$(BOARD).bin

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

clean:
	@cmd /c "for %i in ($(BOARDS)) do @cs-make --no-print-directory BOARD=%i board-clean"
