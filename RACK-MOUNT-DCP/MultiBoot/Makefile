# $Header: /OldStuff/2000XS/MultiBoot/Makefile,v 1.1 2013/05/14 15:04:04 Ellery Exp $
#
# Copyright (c) Cyber Group, Inc 2007
# This program is the property of Cyber Group Inc
# Copying of this software is expressly forbidden, without the prior written
# consent of Cyber Group Inc
#
# This is the make file to build the MultiBoot boot loader.

# First, the global configuration for the project
PRJ = MultiBoot

# Target project boards (all in upper case because of Windows, except for those
# that have digits in names).
#BOARDS += DCP1D
BOARDS += DCPRM
# M54455EVB CPU-1 LITEKIT Platform1A

# C++ module list
CPP = Main.cpp

# ASM module list
ASM =

# This conditional will activate if makefile is invoked second time inside this
# make build, with BOARD having one value out of BOARDS list. In main level
# makefile, BOARD is not defined, so board-specific makefile components cannot
# be included in here.
ifneq ($(BOARD),)
BINARY = BootLoad_$(BOARD).bin
# RTOS folder location. This folder includes the RTOS library and all related
# include files for the C++ compiler.
CyGOS = $(value PRODUCTIONCYGOS-$(BOARD))

# Include directories, relative to this directory.
INC = . $(CyGOS)

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
