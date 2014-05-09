ROOTCFLAGS    = $(shell $(ROOTSYS)/bin/root-config --cflags)
ROOTLIBS      = $(shell $(ROOTSYS)/bin/root-config --libs)

BINDIR = ../bin/

#######################################
# -- DEFINE ARCH to something sensible!
#######################################

# -- Linux with egcs
ifeq ($(shell uname),Linux)
CXX           = g++
CXXFLAGS      = -g -Wall -fPIC
LD            = g++
LDFLAGS       = -g
SOFLAGS       = -shared
endif

# -- Mac OS X - Darwin
ifeq ($(shell uname),Darwin)
CXX           = g++
CXXFLAGS      = -g -Wall -fPIC
LD            = g++
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

