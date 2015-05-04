ROOTCFLAGS    = $(shell $(ROOTSYS)/bin/root-config --cflags)
ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --libs)

BINDIR = ../bin/

#######################################
# -- DEFINE ARCH to something sensible!
#######################################

# -- Linux with egcs
ifeq ($(shell uname),Linux)
CXX           = $(shell $(ROOTSYS)/bin/root-config --cxx)
CXXFLAGS      = -g -Wall -fPIC
LD            = $(shell $(ROOTSYS)/bin/root-config --cxx)
LDFLAGS       = -g
SOFLAGS       = -shared
endif

# -- Mac OS X - Darwin
ifeq ($(shell uname),Darwin)
CXX           = $(shell $(ROOTSYS)/bin/root-config --cxx)
CXXFLAGS      = -g -Wall -fPIC
LD            = $(shell $(ROOTSYS)/bin/root-config --cxx)
LDFLAGS       = -g
SOFLAGS       = -dylib
endif

LIBS += $(ROOTLIBS)

.SUFFIXES: .cc,.C

.cc.o:
	$(CXX) $(CXXFLAGS) -c $<

all: chisqsurface

chisqsurface:  chisqsurface.cpp
# -----------------------------------------------------------------------------
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(LIBS) $< -o $(BINDIR)/$@

