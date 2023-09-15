# $Header: /CyGOS3/BransonCyGOSIndia/Makefile,v 1.1 2010/09/29 14:11:25 noor Exp $
#
# Copyright (c) Cyber Group, Inc 2007
# This program is the property of Cyber Group Inc
# Copying of this software is expressly forbidden, without the prior written
# consent of Cyber Group Inc
#
# This file is part of CyGOS-3.
#
# This is the top-level makefile. The makefile builds targets 'all' and 'clean'
# for selected CPU platform hardware.

# This is the list of supported boards for the build. Add more boards as
# necessary, but make the last part (board name) in upper case. 'Make' under
# Windows has problems with looking up environment variables if they are in
# lower case, and make syntax does not allow uppercasing strings.
#
# To build a particular library, set environment variable Build-BOARD to Yes.
# For example: BUILD-LITEKIT = Yes (in advanced system properties).

# Start with empty list and then add whatever boards we support...
#BOARDS =
#BOARDS += Arch/MCF/MCF5445x/DCP1D
BOARDS += Arch/MCF/MCF5445x/DCPRM
# Almost no need to change anything below this line...

.PHONY: all board-all allcopy board-allcopy clean board-clean everything \
	board-everything

# This rule can be used to make every build independent of which builds are
# enabled in environment variables, useful to test if one build did not break
# another.
everything:
	@cmd /c "for %i in ($(BOARDS)) do @cs-make TARGET=%i board-everything"

all:
	@cmd /c "for %i in ($(BOARDS)) do @cs-make --no-print-directory TARGET=%i board-all"

allcopy:
	@cmd /c "for %i in ($(BOARDS)) do @cs-make --no-print-directory TARGET=%i board-allcopy"

clean:
	@cmd /c "for %i in ($(BOARDS)) do @cs-make --no-print-directory TARGET=%i board-clean"

BOARD = $(word 4, $(subst /, , $(TARGET)))
BUILD = $(value BUILD-$(BOARD))

board-all:
ifeq ($(BUILD),Yes)
	@echo Making target $(TARGET)
	cd $(TARGET) && cs-make all
else
	@echo Not making target $(TARGET)
endif

board-everything:
	@echo Making everything, current target $(TARGET)
	cd $(TARGET) && cs-make all

board-allcopy:
ifeq ($(BUILD),Yes)
	@echo Making target $(TARGET)
	cd $(TARGET) && cs-make allcopy
else
	@echo Not making target $(TARGET)
endif

board-clean:
ifeq ($(BUILD),Yes)
	@echo Making target $(TARGET)
	cd $(TARGET) && cs-make clean
else
	@echo Not making target $(TARGET)
endif

