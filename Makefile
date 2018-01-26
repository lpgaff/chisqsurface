BINDIR = ./bin

CXX           = $(shell root-config --cxx)
CXXFLAGS      = $(shell root-config --cflags) -g -Wall -fPIC
LD            = $(shell root-config --ld)
LDFLAGS       = $(shell root-config --ldflags) -g
LIBS          = $(shell root-config --libs)

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

