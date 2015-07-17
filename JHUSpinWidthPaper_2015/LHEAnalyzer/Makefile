
ROOTCFLAGS    = $(shell root-config --cflags) -Lrootlib -lEG
ROOTGLIBS     = $(shell root-config --glibs) -Lrootlib -lEG

CXX           = g++
CXXFLAGS      = -fPIC -ansi -D_GNU_SOURCE -g -O2

ARCH         := $(shell root-config --arch)

ifeq ($(ARCH),macosx)
# MacOS X with cc (GNU cc 2.95.2 and gcc 3.3)
CXX           = c++ -lm
CXXFLAGS      = -O2 -pipe -Wall -W -Woverloaded-virtual -fPIC -Wno-deprecated -O -ansi -D_GNU_SOURCE  -j 4
# The SOFLAGS will be used to create the .dylib,
# the .so will be created separately
DllSuf        = dylib
UNDEFOPT      = suppress
endif

CXXFLAGS      += $(ROOTCFLAGS) -I$(ROOFITSYS)/include/ -I$(CMSSW_BASE)/src

NGLIBS         = $(ROOTGLIBS) 
# Hack here, because RooFit is removed from ROOT:
NGLIBS        += -L $(ROOFITSYS)/lib/ -lMinuit -lRooFitCore -lRooFit #-lboost_regex
NGLIBS  += -L $(CMSSW_BASE)/lib/$(SCRAM_ARCH)/
GLIBS          = $(filter-out -lNew, $(NGLIBS))

INCLUDEDIR       = $(PWD)/src/

OUTLIB           = $(PWD)/lib/

$(OUTLIB)PDGHelpers.o: $(INCLUDEDIR)PDGHelpers.cc
	@mkdir -p lib
	$(CXX) $(CXXFLAGS) -c -o $(OUTLIB)PDGHelpers.o $(GLIBS) $<

$(OUTLIB)Particle.o: $(INCLUDEDIR)Particle.cc $(OUTLIB)PDGHelpers.o 
	@mkdir -p lib
	$(CXX) $(CXXFLAGS) -c -o $(OUTLIB)Particle.o $(GLIBS) $<

$(OUTLIB)LHEParticleSmear.o: $(INCLUDEDIR)LHEParticleSmear.cc
	@mkdir -p lib
	$(CXX) $(CXXFLAGS) -c -o $(OUTLIB)LHEParticleSmear.o $(GLIBS) $<


$(OUTLIB)ZZCandidate.o: $(INCLUDEDIR)ZZCandidate.cc $(OUTLIB)Particle.o
	$(CXX) $(CXXFLAGS) -c -o $(OUTLIB)ZZCandidate.o $(GLIBS) $<


$(OUTLIB)Event.o: $(INCLUDEDIR)Event.cc $(OUTLIB)ZZCandidate.o
	$(CXX) $(CXXFLAGS) -c -o $(OUTLIB)Event.o $(GLIBS) $<


$(OUTLIB)convertLHE.o: $(INCLUDEDIR)convertLHE.cc $(OUTLIB)Event.o $(OUTLIB)LHEParticleSmear.o
	$(CXX) $(CXXFLAGS) -c -o $(OUTLIB)convertLHE.o $(GLIBS) $<


convertLHE: $(OUTLIB)convertLHE.o
	$(CXX) $(CXXFLAGS) -o convertLHE $(OUTLIB)*.o $(GLIBS)

clean:
	rm -rf $(OUTLIB)
	rm -f $(PWD)*.o
	rm -f convert*