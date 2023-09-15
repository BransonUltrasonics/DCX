# $Header: /ProductionCyGOS/CyGOS/Arch/MCF/Makefile,v 1.2 2010/10/26 14:42:07 Noor Exp $
#
# Copyright (c) Cyber Group, Inc 2007
# This program is the property of Cyber Group Inc
# Copying of this software is expressly forbidden, without the prior written
# consent of Cyber Group Inc
#
# This file is part of CyGOS-3.
#
# This is architecture-specific Makefile for ColdFire processors. Makefile
# lists objects built from C++ and assembly sources specific for all processors
# of this architecture.
# Makefile also includes some object files that are extracted from libgcc.a
# that are required by gcc to implement features that are not directly
# supported by Coldfire processors, such as multiply, divide etc.
#
# This makefile should be included from another Makefile two levels deeper,
# specific to the board where the code will execute.

include ../../Makefile.arch

# C++ files.
ARCHCPP = ArchDefs.cpp FecBase.cpp QspiBase.cpp
# FlexCAN.cpp

# ASM files.
ARCHASM = MemoryInit.asm ArchDefs.asm

# Startup ASM file.
STARTASM = Vectors.asm

# Object files extracted from libgcc.a so we have them in here and we won't get
# any baggage if we don't need it...
ARCH_O = *.o
#__floatsidf __adddf3 __muldf3 __divdf3 __subdf3 __fixunsdfsi
# _divsi3.o _udivsi3.o _dvmd_tls.o

# Objects from these files will have $(ARCH)_ prefix so they will not interfere
# with objects compiled from other sources.
ARCHOBJ = $(addprefix $(OBJ_DIR)/$(ARCH)_, \
	$(ARCHCPP:.cpp=_cpp.o) $(ARCHASM:.asm=_asm.o))

OBJ += $(ARCHOBJ)

O += $(addprefix ../../,$(ARCH_O))

# This is the startup object. ld cannot get it from the library even if linker
# script requires a label from this object file, so this object is separated
# from the rest.
STARTOBJ = $(addprefix $(OBJ_DIR)/$(ARCH)_,$(STARTASM:.asm=_asm.o))

# This is how we compile C++ files from this directory. Sources are expected to
# be two directories up because this makefile is included from another makefile.
$(OBJ_DIR)/$(ARCH)_%_cpp.o : ../../%.cpp $(wildcard $(addsuffix /*.h,$(INC)))
	$(CC) $(CCOPT) -o $@ $<

# This is how we compile ASM files from this directory. Sources are expected to
# be two directories up because this makefile is included from another makefile.
$(OBJ_DIR)/$(ARCH)_%_asm.o : ../../%.asm
	$(AS) $(ASOPT) -o $@ $<
