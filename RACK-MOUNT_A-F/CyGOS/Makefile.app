# $Header: /ProductionCyGOS/CyGOS/Makefile.app,v 1.1 2010/05/09 01:01:47 Aare Exp $
#
# Copyright (c) Cyber Group, Inc 2007
# This program is the property of Cyber Group Inc
# Copying of this software is expressly forbidden, without the prior written
# consent of Cyber Group Inc
#
# This is the standard make file to include in the application Makefile.

# Because of poor syntax highlighting, this option will make Eclipse believe
# we use regular 'new' operator, but in fact C++ compiler will use debugging
# version.
# To use regular 'new' for compiles, comment this option out
DEBUGNEW = -DCYGOS_USE_DEBUGNEW

# CyGOS-provided startup object.
STARTOBJ = $(CyGOS)/Start.o
# CyGOS-provided library and linker files.
DLD = RTOS-$(BOARDS).ld
LIB = $(CyGOS)/libRTOS-$(CPU_FAMILY).a $(BSP)/libBSP-$(BOARDS)-$(CPU_FAMILY).a

# Temporary object directory. All C++ and ASM objects go there. C++ and ASM
# modules will have _cpp and _asm inserted into their file names when compiled,
# so same name can be used for both modules if they implement functions for the
# same object.
OBJ_DIR	= obj-$(BOARDS)

# Object file list, list is built from C++ and asm files, prefixed with object
# file directory, and extensions replaced with ".o"
OBJ = $(addprefix $(OBJ_DIR)/,$(CPP:.cpp=_cpp.o) $(ASM:.asm=_asm.o))

HEX = $(PRJ)-$(BOARDS).hex

# This is the output file in ELF format, hidden in objects directory. This is
# the file that needs to be loaded for Workbench debugger. 
ELF = $(OBJ_DIR)/$(PRJ).elf

board-all: dirs $(HEX)

board-allcopy: board-all
#	cp $(HEX) "$(PRJ)-$(BOARDS).txt"
ifdef BINARY
	$(OC) $(OCBIN) $(ELF) "$(BINARY)"
endif

board-clean:
ifneq ($(OBJ_DIR),)
	@echo Cleaning project output directory for $(PRJ)-$(BOARDS) build
	@echo empty >$(OBJ_DIR)/empty
	rm -r $(OBJ_DIR)/*
else
	@echo Project output directory not defined.
endif

# dirs - create an empty directory if we have to
dirs:
	@cmd /c if not exist $(OBJ_DIR) md $(OBJ_DIR)
	
# Linking the ELF target...
$(ELF) : $(STARTOBJ) $(OBJ) $(LIB)
	$(LD) $(LDOPT) -L$(OBJ_DIR) -L$(CyGOS) -L$(BSP) -o $@ $(STARTOBJ) $(OBJ) --start-group $(LIB) --end-group -lgcc -T $(DLD) -Map $(PRJ)-$(BOARDS).map

# Linking the final target...
$(HEX) : $(ELF)
	$(OC) $(OCHEX) $< $@

# Compiling C++ files
$(OBJ_DIR)/%_cpp.o : %.cpp $(wildcard $(addsuffix /*.h,$(INC)))
	$(CC) $(CCOPT) -o $@ $<

# Compiling ASM files
$(OBJ_DIR)/%_asm.o : %.asm
	$(AS) $(ASOPT) -o $@ $<
