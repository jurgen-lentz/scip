# $Id: Makefile,v 1.2 2002/10/23 14:35:54 bzfpfend Exp $
#* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#*                                                                           *
#*   File....: Makefile                                                      *
#*   Name....: SCIP Makefile                                                  *
#*   Author..: Thorsten Koch                                                 *
#*   Copyright by Author, All rights reserved                                *
#*                                                                           *
#* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#
# Several symlinks are nesseccary:
#
# lib/cpxinc                         -> directory with cplex.h
# lib/spxinc                         -> directory with SoPlex *.h
# lib/libcplex.$(OSTYPE).$(ARCH).a   -> libcplex.a
# lib/libsoplex.$(BASE).a            -> itself
#
#
.PHONY:		depend clean lint doc test testcplex

ARCH            :=      $(shell uname -m | \
                        sed \
			-e s/sun../sparc/ \
			-e s/i.86/x86/ \
			-e s/IP../mips/ \
			-e s/9000..../hppa/ \
			-e s/00........../pwr4/)
OSTYPE		:=	$(shell uname -s | tr A-Z a-z)

OPT		=	dbg
LPS		=	cpx
PAR		=	seq

COMP		=	gnu
CC		=	gcc
CXX		=	g++
DCC		=	gcc
DCXX		=	g++
LINT		=	flexelint
AR		=	ar
RANLIB		=	ranlib
DOXY		=	doxygen

SETTINGS        =       hybridhistory

SRCDIR		=	src
BINDIR		=	bin
LIBDIR		=	lib

CPPFLAGS	=	-I$(SRCDIR)
CFLAGS		=	-O
CXXFLAGS	=	-O
LDFLAGS		=	-lpthread -lm
ARFLAGS		=	cr
DFLAGS		=	-MM

GCCWARN		=	-Wall -W -Wpointer-arith -Wcast-align -Wwrite-strings \
			-Wstrict-prototypes -Wmissing-prototypes \
			-Wmissing-declarations -Wno-unknown-pragmas \
			-Wno-unused

GXXWARN		=	-Wall -W -Wpointer-arith -Wbad-function-cast \
			-Wcast-align -Wwrite-strings -Wconversion \
			-Wstrict-prototypes -Wmissing-prototypes \
			-Wmissing-declarations -Wno-unknown-pragmas \
			-Wctor-dtor-privacy -Wnon-virtual-dtor -Wreorder \
			-Woverloaded-virtual -Wsign-promo -Wsynth -Wundef \
			-Wcast-qual -Wold-style-cast -Wshadow 

BASE		=	$(PAR).$(OSTYPE).$(ARCH).$(COMP).$(OPT)
OBJDIR		=	obj/O.$(BASE)
NAME		=	scip

#-----------------------------------------------------------------------------

LIBOBJ		=

#-----------------------------------------------------------------------------
include make/make.$(BASE)
#-----------------------------------------------------------------------------

LIBRARY	=	$(LIBDIR)/lib$(NAME).$(BASE).a
DEPLIB	=	src/depend.scip

LIBXXX	=	$(addprefix $(OBJDIR)/,$(LIBOBJ))
LIBSRC	=	$(addprefix $(SRCDIR)/,$(LIBOBJ:.o=.c))

#-----------------------------------------------------------------------------
# LP Solver CPLEX
#-----------------------------------------------------------------------------
ifeq ($(LPS),cpx)
	CPPFLAGS+=	-I$(LIBDIR)/cpxinc
	OBJECT	=	cmain.o
	LPSLIB	=	cplex.$(OSTYPE).$(ARCH)
	LPIOBJ	=	cpx_lp.o
	LPILIB	=	$(LIBDIR)/libcpxlp.$(BASE).a
	LPIXXX	=	$(addprefix $(OBJDIR)/,$(LPIOBJ))
	LPISRC  =	$(addprefix $(SRCDIR)/,$(LPIOBJ:.o=.c))
	TARGET	=	$(NAME).$(BASE).cpx
	BINARY	=	$(BINDIR)/$(TARGET)
	DEPLPS	=	src/depend.cpx
	OBJXXX	=	$(addprefix $(OBJDIR)/,$(OBJECT))
	OBJSRC	=	$(addprefix $(SRCDIR)/,$(OBJECT:.o=.c))

$(BINARY):	$(OBJDIR) $(BINDIR) $(OBJXXX) $(LIBRARY) $(LPILIB) 
		$(CXX) $(CXXFLAGS) $(OBJXXX) \
		-L$(LIBDIR) -l$(NAME).$(BASE) -lcpxlp.$(BASE) \
		-l$(LPSLIB) $(LDFLAGS) -o $@

endif
#-----------------------------------------------------------------------------
# LP Solver SoPlex
#-----------------------------------------------------------------------------
ifeq ($(LPS),spx)
	CPPFLAGS+=	-I$(LIBDIR)/spxinc 
	OBJECT	=	cppmain.o
	LPSLIB	=	soplex.$(OSTYPE).$(ARCH).$(COMP).$(OPT)
	LPIOBJ	=	spx_lp.o
	LPILIB	=	$(LIBDIR)/libspxlp.$(BASE).a
	LPIXXX	=	$(addprefix $(OBJDIR)/,$(LPIOBJ))
	LPISRC  =	$(addprefix $(SRCDIR)/,$(LPIOBJ:.o=.cpp))
	TARGET	=	$(NAME).$(BASE).spx
	BINARY	=	$(BINDIR)/$(TARGET)
	DEPLPS	=	src/depend.spx
	OBJXXX	=	$(addprefix $(OBJDIR)/,$(OBJECT))
	OBJSRC	=	$(addprefix $(SRCDIR)/,$(OBJECT:.o=.cpp))

$(BINARY):	$(OBJDIR) $(BINDIR) $(OBJXXX) $(LIBRARY) $(LPILIB)
		$(CXX) $(CXXFLAGS) $(OBJXXX) \
		-L$(LIBDIR) -l$(NAME).$(BASE) -lspxlp.$(BASE) \
		-l$(LPSLIB) $(LDFLAGS) -o $@

endif

#-----------------------------------------------------------------------------
# SCIP Library
#-----------------------------------------------------------------------------
$(LIBRARY):	$(OBJDIR) $(LIBDIR) $(LIBXXX) 
		-rm -f $@
		$(AR) $(ARFLAGS) $@ $(LIBXXX) 
		$(RANLIB) $@

$(LPILIB):	$(OBJDIR) $(LIBDIR) $(LPIXXX)
		-rm -f $@
		$(AR) $(ARFLAGS) $@ $(LPIXXX)
		$(RANLIB) $@

#-----------------------------------------------------------------------------

lint:		$(LIBSRC) $(OBJSRC)
		$(LINT) lint/scip.lnt -os\(lint.out\) \
		$(CPPFLAGS) -UNDEBUG $^

doc:		
		cd doc; $(DOXY) scip.dxy

test:		
		cd check; \
		/bin/sh ./check.sh $(TEST).test $(BINARY) $(SETTINGS);

testcplex:		
		cd check; \
		/bin/sh ./check_cplex.sh $(TEST).test $(OSTYPE).$(ARCH);

$(OBJDIR):	
		-mkdir -p $(OBJDIR)

$(LIBDIR):
		-mkdir -p $(LIBDIR)

$(BINDIR):
		-mkdir -p $(BINDIR)

clean:
		-rm -rf $(OBJDIR)/* $(LIBRARY) $(TARGET)

depend:
		$(SHELL) -ec '$(DCC) $(DFLAGS) $(CPPFLAGS) $(LIBSRC) \
		| sed '\''s|^\([0-9A-z\_]\{1,\}\)\.o|$$\(OBJDIR\)/\1.o|g'\'' \
		>$(DEPLIB)'
ifeq ($(LPS),cpx)
		$(SHELL) -ec '$(DCC) $(DFLAGS) $(CPPFLAGS) $(OBJSRC) \
		| sed '\''s|^\([0-9A-z\_]\{1,\}\)\.o|$$\(OBJDIR\)/\1.o|g'\'' \
		>$(DEPLPS)'
endif
ifeq ($(LPS),spx)
		$(SHELL) -ec '$(DCXX) $(DFLAGS) $(CPPFLAGS) $(OBJSRC) \
		| sed '\''s|^\([0-9A-z\_]\{1,\}\)\.o|$$\(OBJDIR\)/\1.o|g'\'' \
		>$(DEPLPS)'
endif

-include	$(DEPLIB)
-include 	$(DEPLPS)

$(OBJDIR)/%.o:	$(SRCDIR)/%.c
		$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o:	$(SRCDIR)/%.cpp
		$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# --- EOF ---------------------------------------------------------------------
