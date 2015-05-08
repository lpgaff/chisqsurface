BINDIR = ../bin

CXX           = $(shell $(ROOTSYS)/bin/root-config --cxx)
CXXFLAGS      = $(shell $(ROOTSYS)/bin/root-config --cflags) -g -Wall -fPIC
LD            = $(shell $(ROOTSYS)/bin/root-config --ld)
LDFLAGS       = $(shell $(ROOTSYS)/bin/root-config --ldflags) -g
LIBS          = $(shell $(ROOTSYS)/bin/root-config --libs)

#######################################
# -- DEFINE ARCH to something sensible!
#######################################

# -- Linux with egcs
ifeq ($(shell uname),Linux)
SOFLAGS       = -shared
LDFLAGS      += -Wl,--no-as-needed
endif

# -- Mac OS X - Darwin
ifeq ($(shell uname),Darwin)
SOFLAGS       = -dylib
endif


.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $@

all: chisqsurface

chisqsurface:  chisqsurface.cpp
# -----------------------------------------------------------------------------
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIBS) $< -o $(BINDIR)/$@

