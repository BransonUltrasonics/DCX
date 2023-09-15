#/* $Header: $*/
#/*****************************************************************************
#$Log: $
#*/

include Makefile-$(USERNAME)-$(COMPUTERNAME).mk

# First, the global configuration for the project
#PRJ = WCTest
PRJ = WC

# Target project boards (all in upper case because of Windows).
BOARDS = WC
CPU_FAMILY = MCF5445x

# C++ module list
CPP = Main.cpp InputTask.cpp WCTask.cpp MIATask.cpp GLOBALS.cpp LOG.cpp PasscodeConfig.cpp\
		preset.cpp FRAM.cpp WebSite.cpp UploadFirmWareTask.cpp WeldResultHistory.cpp ScanResDetect.cpp\
		FirmWareUpgrade.cpp RTClock.cpp UserIO.cpp StateMachineDrv.cpp ModBusRcvTask.cpp ReceiveCommandHandler.cpp\
		ModBusSendTask.cpp FieldBus.cpp NRTModbusTask.cpp\
		ProfiBus.cpp EthernetIP.cpp WelderSearchTask.cpp  VerifyPreset.cpp ObjDictionary.cpp\
		counter.cpp FieldbusTask.cpp StateFunctions.cpp StateTable.cpp Alarms.cpp FieldbusTable.cpp ShutDownTask.cpp ExternalStatusTask.cpp BDMTask.cpp BDM08.cpp DcxManagerHandler.cpp\
		$(wildcard cifXToolkit/HWInterface/*.cpp) $(wildcard cifXToolkit/OSAbstraction/*.cpp)\
		$(wildcard cifXToolkit/Source/*.cpp) $(wildcard cifXToolkit/User/*.cpp)

#FieldbusTask.cpp DPTest.cpp HilscherCardTestFunctions.cpp TaskManager.cpp
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

#this path can be anything need to be decided by application programmer.
#He/She just need to copy all header and library files in the path mentioned here
CyGOS = $(value PRODUCTIONCYGOS-$(CPU_FAMILY))
BSP = $(value BSP-$(BOARDS)-$(CPU_FAMILY))

# Include directories, relative to this directory.
INC = cifXToolkit/Source cifXToolkit/OSAbstraction cifXToolkit/HWInterface cifXToolkit/User . $(CyGOS) $(BSP)

# This is an optional variable. If defined, the binary file built by this
# project will be copied into specified file.

BINARY = App_$(BOARDS).bin

include $(CyGOS)/Makefile.app
include $(CyGOS)/Makefile.arch
include $(CyGOS)/Makefile.cpu

# Done including things that are defined only for specific board build.
endif

board-Flash:
	#@echo check path: $(shell pwd)  $(BOARD)
	#@cp -fu $(value PRODUCTIONCYGOS-$(CPU_FAMILY))//*.bin "$(shell pwd)"
	@echo Making the Complete Bin File
	@CompleteFlash.exe $(BOARD)
	@echo Complete Bin for $(BOARD) done.
	@echo Making Firmware File with checksum
	@ADDCHECKSUM.exe -b $(BOARD)
	@cmd /c if exist ObjDumpElf.rtf rm ObjDumpElf.rtf
	@cmd /c if exist readelfall.rtf rm readelfall.rtf
#	@cmd /c if exist WCTest-WCSorted.map rm WCTest-WCSorted.map
	@m68k-elf-objdump -D $(OBJ_DIR)/$(PRJ).elf  > ObjDumpElf.rtf 
	@m68k-elf-readelf -a $(OBJ_DIR)/$(PRJ).elf  > readelfall.rtf
#	@SortMap.exe WCTest-WC.map > WCTest-WCSorted.map 

# Web site files: This Makefile assumes that the storage for all this is flat,
# /www is for common files and separate subdirectories with board names are 
# board-specific, but all of them are copied into one output directory www-BOARD
#
# A hack - board-specific web site components are only copied if there is at
# least one html file in that folder, otherwise copy utilities just go nuts...
board-web:
	@echo Copying web pages into output folder www-$(BOARD)
	@cmd /c if exist www-$(BOARD) rm -r www-$(BOARD)
	@cmd /c if not exist www-$(BOARD) md www-$(BOARD)
	@cmd /c copy www www-$(BOARD)
#	@cmd /c if exist www-$(BOARD)\\*.html copy www\\$(BOARD) www-$(BOARD)
#	@if exist www-$(BOARD)/web$(MAC).tar rm www-$(BOARD)/web$(MAC).tar
	@echo Tar-balling web site files
#	@cd www-$(BOARD) && tar cf web.tar *
	@cd www-$(BOARD) && if [ -f "web.tar" ]; then rm web.tar; fi && 7z a -ttar web.tar *
	@cd www-$(BOARD) && cp -f web$(MAC).tar ../web.cyg
	@echo Web pages for $(BOARD) done.
	
#board-compress:
#	@compressLZAR.exe www-$(BOARD)/web$(MAC).tar web$(MAC).cyg
#	@echo compress Web pages for www-$(BOARD)/web$(MAC).tar done.
	
#compress:
#	@echo compressing the TAR archive
#	@cmd /c "for %i in ($(BOARDS)) do @cs-make --no-print-directory BOARD=%i board-compress"

web:
	@echo Making the web sites
	@cmd /c "for %i in ($(BOARDS)) do @cs-make --no-print-directory BOARD=%i board-web"	
	
CompleteFlash:
	@echo Making Complete Bin File
	@cmd /c "for %i in ($(BOARDS)) do @cs-make --no-print-directory BOARD=%i board-Flash"
	
CreateObjDir:	
	@cmd /c if not exist obj-$(BOARDS)\cifXToolkit mkdir obj-$(BOARDS)\cifXToolkit
	@cmd /c if not exist obj-$(BOARDS)\cifXToolkit\Source mkdir obj-$(BOARDS)\cifXToolkit\Source
	@cmd /c if not exist obj-$(BOARDS)\cifXToolkit\HWInterface mkdir obj-$(BOARDS)\cifXToolkit\HWInterface
	@cmd /c if not exist obj-$(BOARDS)\cifXToolkit\OSAbstraction mkdir obj-$(BOARDS)\cifXToolkit\OSAbstraction
	@cmd /c if not exist obj-$(BOARDS)\cifXToolkit\User mkdir obj-$(BOARDS)\cifXToolkit\User
	
all:
	touch Main.cpp
	@cmd /c "for %i in ($(BOARDS)) do @cs-make --no-print-directory BOARD=%i board-all"
	@cs-make web
#	@cs-make compress
	@cs-make CompleteFlash
	
allcopy:
	@cs-make CreateObjDir
	touch Main.cpp
	@cmd /c "for %i in ($(BOARDS)) do @cs-make --no-print-directory BOARD=%i board-allcopy"
	@cs-make web
#	@cs-make compress
	@cs-make CompleteFlash
	
clean:
	@cmd /c if exist www-$(PRJ) rm -r www-$(PRJ)
	@echo www-$(PRJ)
	@cmd /c "for %i in ($(BOARDS)) do @cs-make --no-print-directory BOARD=%i board-clean"
	@cs-make CreateObjDir
