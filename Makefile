BINDIR = ./bin
LIBDIR = ./lib

ROOTCFLAGS	:= $(shell root-config --cflags)
ROOTLIBS	:= $(shell root-config --libs)
ROOTVER		:= $(shell root-config --version | head -c1)

ifeq ($(ROOTVER),5)
	ROOTDICT  := rootcint
	DICTEXT   := .h
else
	ROOTDICT  := rootcling
	DICTEXT   := _rdict.pcm
endif

CXX           = $(shell root-config --cxx)
CXXFLAGS      = $(ROOTCFLAGS) -g -Wall -fPIC -I.
LIBS          = $(ROOTLIBS)

all: chisqsurface

OBJECTS = scan.o \
		  rootobjs.o \
		  chisqsurface_dict.o

DEPENDENCIES = scan.hh \
               rootobjs.hh \
               RootLinkDef.h

chisqsurface: chisqsurface.cc $(OBJECTS)
	mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $^ -o $(BINDIR)/$@ $(LIBS)

%.o: %.cc %.hh
	$(CXX) $(CXXFLAGS) -c $< -o $@


%_dict.o: %_dict.cc
	$(CXX) $(CXXFLAGS) -c $<

%_dict.cc: $(DEPENDENCIES)
	$(ROOTDICT) -f $@ -c $(DEPENDENCIES)
	cp $*_dict$(DICTEXT) $(BINDIR)/


clean:
	rm -f *.o *_dict.cc *$(DICTEXT)
