BINDIR = ./bin
LIBDIR = ./lib

ROOTCFLAGS	:= $(shell root-config --cflags)
ROOTLIBS	:= $(shell root-config --libs)
ROOTINC 	:= $(shell root-config --incdir)
ROOTVER		:= $(shell root-config --version | head -c1)

ifeq ($(ROOTVER),5)
	ROOTDICT  := rootcint
	DICTEXT   := .h
else
	ROOTDICT  := rootcling
	DICTEXT   := _rdict.pcm
endif

CXX      = $(shell root-config --cxx)
CFLAGS   = $(ROOTCFLAGS) -g -Wall -fPIC -I.
INCLUDES = -I. -I$(ROOTINC)
LIBS     = $(ROOTLIBS)

PLATFORM:=$(shell uname)
ifeq ($(PLATFORM),Darwin)
	export __APPLE__:= 1
	INCLUDES += -I/opt/local/include
	LIBS     += -L/opt/local/lib
endif
ifeq ($(PLATFORM),Linux)
	export __LINUX__:= 1
endif


all: chisqsurface

OBJECTS = scan.o \
		  rootobjs.o \
		  chisqsurface_dict.o

DEPENDENCIES = scan.hh \
               rootobjs.hh \
               RootLinkDef.h

chisqsurface: chisqsurface.cc $(OBJECTS)
	mkdir -p $(BINDIR)
	$(CXX) $(CFLAGS) $(INCLUDES) $^ -o $(BINDIR)/$@ $(LIBS)

%.o: %.cc %.hh
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

%_dict.o: %_dict.cc
	$(CXX) $(CFLAGS) $(INCLUDES) -c $<

%_dict.cc: $(DEPENDENCIES)
	$(ROOTDICT) -f $@ -c $(INCLUDES) $(DEPENDENCIES)
	cp $*_dict$(DICTEXT) $(BINDIR)/


clean:
	rm -f *.o *_dict.cc *$(DICTEXT)
