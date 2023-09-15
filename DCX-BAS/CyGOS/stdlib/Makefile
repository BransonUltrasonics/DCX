# $Header: /ProductionCyGOS/CyGOS/stdlib/Makefile,v 1.1 2010/05/09 00:31:05 Aare Exp $
#
# Copyright (c) Cyber Group, Inc 2007
# This program is the property of Cyber Group Inc
# Copying of this software is expressly forbidden, without the prior written
# consent of Cyber Group Inc
#
# This file is part of CyGOS-3.
#
# This is the stdlib directory make file.
#
# This makefile should be included from the board-specific Makefile.

STDLIBCPP = Assert.cpp Alloc.cpp PrintF.cpp String.cpp StdLib.cpp StdIO.cpp \
	IO.cpp

STDLIBDIR = ../../../../stdlib

STDLIBOBJ = $(addprefix $(OBJ_DIR)/stdlib_, \
	$(STDLIBCPP:.cpp=_cpp.o) $(STDLIBASM:.asm=_asm.o))

INC += $(STDLIBDIR)

OBJ += $(STDLIBOBJ)

$(OBJ_DIR)/stdlib_%_cpp.o : $(STDLIBDIR)/%.cpp $(wildcard $(addsuffix /*.h,$(INC)))
	$(CC) $(CCOPT) -o $@ $<
