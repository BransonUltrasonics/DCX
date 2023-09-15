#/* $Header: $*/
#/*****************************************************************************
#$Log: $
#*/


# First, the global configuration for the project
PRJ = Dcp1bTest

# Target project boards (all in upper case because of Windows).
#BOARDS =
#BOARDS += DCP1D
BOARDS += DCPRM

# C++ module list
CPP = Main.cpp InputTask.cpp DupsTask.cpp IOTask.cpp GLOBALS.cpp SEEK.cpp WELD.cpp \
		ScanResDetect.cpp StateMachineDrv.cpp SCAN.cpp ReceiveCommandHandler.cpp \
		LOG.cpp preset.cpp UserIO.cpp counter.cpp  FRAM.cpp WebSite.cpp UploadFirmWareTask.cpp \
		WelderSearchTask.cpp ShutDownTask.cpp BDM08.cpp BDMTask.cpp FirmwareUpgrade.cpp \
		BroadcastFT.cpp UtilsFT.cpp CommandsFT.cpp CommandsParametersFT.cpp SR232FT.cpp \
		TerminalFT.cpp WebSiteFT.cpp BroadcastWR.cpp UtilsWR.cpp

# ASM module list WebSite.cpp SysConfig.cpp
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

BINARY = App_$(BOARD).bin

include $(CyGOS)/Makefile.app
include $(CyGOS)/Makefile.arch
include $(CyGOS)/Makefile.cpu

# Done including things that are defined only for specific board build.
endif

board-Flash:
	@echo Making the Complete Bin File
	@CompleteFlash.exe $(BOARD)
	@echo Complete Bin for $(BOARD) done.
	@echo Making Firmware File with checksum
	@ADDCHECKSUM.exe -b $(BOARD)
	@cp Complete_$(BOARD).bin Complete_$(BOARD)_Dongle.bin
	@rm -f Complete_$(BOARD).bin
	@cp Complete_$(BOARD)Firmware.bin Complete_$(BOARD)_RJ45.bin
	@rm -f Complete_$(BOARD)Firmware.bin
	@cp App_$(BOARD).bin App_$(BOARD)_Dongle.bin
	@rm -f App_$(BOARD).bin
	@cp App_$(BOARD)Firmware.bin App_$(BOARD)_RJ45.bin
	@rm -f App_$(BOARD)Firmware.bin


# Web site files: This Makefile assumes that the storage for all this is flat,
# /www is for common files and separate subdirectories with board names are 
# board-specific, but all of them are copied into one output directory www-BOARD
#
# A hack - board-specific web site components are only copied if there is at
# least one html file in that folder, otherwise copy utilities just go nuts...
board-web:
	@echo Copying web pages into output folder www-$(BOARD)
	@cmd /c if not exist www-$(BOARD) md www-$(BOARD)
	@cmd /c copy www www-$(BOARD)
#	@cmd /c if exist www-$(BOARD)\\*.html copy www\\$(BOARD) www-$(BOARD)
#	@if exist www-$(BOARD)/web$(MAC).tar rm www-$(BOARD)/web$(MAC).tar
	@echo Tar-balling web site files
	@cd www-$(BOARD) && tar cf web.tar *
	@echo Web pages for $(BOARD) done.
	
board-compress:
	@compressLZAR.exe www-$(BOARD)/web$(MAC).tar web$(MAC).cyg
	@echo compress Web pages for www-$(BOARD)/web$(MAC).tar done.
	
compress:
	@echo compressing the TAR archive
	@cmd /c "for %i in ($(BOARDS)) do @cs-make --no-print-directory BOARD=%i board-compress"

web:
	@echo Making the web sites
	@cmd /c "for %i in ($(BOARDS)) do @cs-make --no-print-directory BOARD=%i board-web"	
	
CompleteFlash:
	@echo Making Complete Bin File
	@cmd /c "for %i in ($(BOARDS)) do @cs-make --no-print-directory BOARD=%i board-Flash"
	
all:
	touch Main.cpp
	@cmd /c "for %i in ($(BOARDS)) do @cs-make --no-print-directory BOARD=%i board-all"
	@cs-make web
	@cs-make compress
	@cs-make CompleteFlash
	
allcopy:
	touch Main.cpp
	@cmd /c "for %i in ($(BOARDS)) do @cs-make --no-print-directory BOARD=%i board-allcopy"
	@cs-make web
	@cs-make compress
	@cs-make CompleteFlash
	
clean:
	@cmd /c "for %i in ($(BOARDS)) do @cs-make --no-print-directory BOARD=%i board-clean"
