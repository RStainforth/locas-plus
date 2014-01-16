# Makefile for LOCAS
#
# Do 'make clean' at the command line after modifications to this makefile
#
# LOCAS depends solely on ROOT. It therefore tries to get as
# much information from ROOT as possible through 'root-config'

# Compile and link flags imported from ROOT 
ROOTCFLAGS  := $(shell root-config --cflags)
ROOTLIB     := $(shell root-config --libs) -lMinuit

# Compile and link flags imported from GEANT4 
GEANT4CFLAGS  := $(shell geant4-config --cflags)
GEANT4LLIB    := $(shell geant4-config --libs)

# Compile and link flags imported from CLHEP 
CLHEPCFLAGS := $(shell clhep-config --include)
CLHEPLIB    := $(shell clhep-config --libs)

# Compile and link flags imported from CURL 
CURLCFLAGS  := $(shell curl-config --cflags)
CURLLIB     := $(shell curl-config --libs)

DEPCFLAGS   := ${ROOTCFLAGS} ${GEANT4CFLAGS} ${CLHEPCFLAGS} ${CURLCFLAGS}
DEPLIBS     := ${ROOTLIB} ${GEANT4LIB} ${CLHEPLIB} ${CURLLIB}

# The name of the library depends on the value of RATSYSTEM, which must be setup to link against rat anyway.
# Compile and link flags imported from RAT
RATCFLAGS   := -I$(RATROOT)/include -I$(RATROOT)/include/RAT -I$(RATROOT)/include/RAT/DS
RATLIB		:= -L$(RATROOT)/lib -lRATEvent_$(RATSYSTEM) -lrat_$(RATSYSTEM)

# Compile and link flags for LOCAS

LOCASCFLAGS := -I./
LOCASLIB    := -L. -lLOCAS

PICFLAG	:= -fPIC

# Defines the archictecture of your machine. This is used to decide the linking command
ARCH	    := $(shell root-config --arch)

# This flag in ROOT is only specified for the architecture. Use it to make sure that consistenty is kept
BUILD		:=  $(shell root-config --ldflags)
LDOPTS		:= 
LINKCMD		:=

ifneq ($(findstring macosx, $(ARCH)),)
	LDOPTS  := -dynamiclib -single_module -undefined dynamic_lookup

# Shouldn't need the below statement anymore - will keep it here for legacy control.
# LINKCMD := install_name_tool -change lib/libRATEvent_$(RATSYSTEM).so @rpath/libRATEvent_$(RATSYSTEM).so libLOCAS.so
endif
ifneq ($(findstring $(ARCH), $(strip linux debian redhat)),)
	LDOPTS := -shared
endif
# If LDOPTS is still empty at this point use the linux flag
ifeq ($(strip $(LDOPTS)),)
	LDOPTS := -shared
endif

### Some options for this package
LOCAS_VERSION:= "1.0.0" # The LOCAS version
LOCAS_COMPATIBILITY:= "1.0.0" 
LOCAS_ROOT 	:= $(LOCAS_ROOT)
LOCAS_LIB	:= $(LOCAS_ROOT)/lib 
LOCAS_SRC	:= $(LOCAS_ROOT)/src
LOCAS_INC	:= $(LOCAS_ROOT)/include
LOCAS_BIN	:= $(LOCAS_ROOT)/bin

# Dictionary
DICT_H	:= LOCAS_LinkDef.h
DICT_C	:= LOCAS_Dict.C
DICT_O	:= $(patsubst %.C,%.o,$(DICT_C))


LOCAS_EXE	:= soc2locas db2soc locas2fit soc2soc

# N. Barros - 22-11-2011 : Removed from the targets as the source file seems to be missing. Maybe a missing commit?
# sjmptest

# Creates a list of all of the *.hh files in the current directory (same directory as the Makefile) EXCEPT the filenames
# defined by the values of $DICT_H, and $DICT_C but with the .C extension replaced by .h
# i.e. Not the files LOCAS_LinkDef.h and LOCAS_Dict.C
LOCASH		:= $(filter-out $(DICT_H) $(patsubst %.C,%.h,$(DICT_C)),$(notdir $(shell find $(LOCAS_SRC) -name "*.hh")))

# Creates a list of all of the *.cc files in the current directory (same directory as the Makefile) EXCEPT the filenames
# defined by the values of $LOCAS_EXE, with .cc attached i.e. Not locas.cc
LOCASC		:= $(filter-out $(addsuffix .cc,$(LOCAS_EXE)),$(notdir $(shell find $(LOCAS_SRC) -name "*.cc")))

# List of the LOCAS objects, i.e. all those files attached in $LOCASC but with the extensions .cc replaced
# with .o
LOCASO	    := $(patsubst %.cc,%.o,$(LOCASC))
LOCASL		:= libLOCAS.so

### Below are the building commands. Don't touch if you don't know what you're doing.

.PHONY: setup all install_libraries executables install_executables


all: show headers libraries executables
#LOCAS_positionfit tdiff reflecttime qpath_runner LOCASfit_runner LOCASextract_runner LOCASplots_runner LOCAStitles_runner

dirs: 
	@mkdir -p $(LOCAS_LIB)
	@mkdir -p $(LOCAS_BIN)
	@mkdir -p $(LOCAS_INC)

show:
	@echo "Build options:"
	@echo " * ARCH  	: $(ARCH)"
	@echo " * LDOPT 	: $(LDOPTS)"

headers: $(LOCASH)
	@echo " == Executing $@ =="

libraries: $(LOCASL)
		@echo " == Executing $@ =="

executables: $(LOCAS_EXE)
		@echo " == Executing $@ =="

install: install_libraries install_executables install_headers

install_headers: headers
		@echo " == Executing $@ =="
		@echo "copying LOCAS headers to $(LOCAS_INC)"
		@cp $(LOCASH) $(LOCAS_INC)
		@echo "Done."

install_libraries: libraries dirs
		@echo " == Executing $@ =="
		@echo "copying $(LOCASL) to $(LOCAS_LIB)"
		@cp $(LOCASL) $(LOCAS_LIB)
		@echo "Done."

install_executables: executables dirs
		@echo " == Executing $@ =="
		@echo "copying LOCAS executables to $(LOCAS_BIN)"
		@cd $(LOCAS_SRC)
		@cp $(LOCAS_EXE) $(LOCAS_ROOT)/bin
		@cd $(LOCAS_ROOT)
		@echo "Done"

clean:
	@echo " == Executing $@ =="
	@rm -f *Dict.C
	@rm -f *Dict.h
	@rm -f *.o
	@rm -f *.so
	@rm -f $(LOCAS_EXE)
	@echo "Done."

soc2locas: soc2locas.o $(LOCASL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
	 $< ${DEPLIBS} ${RATLIB} ${LOCASLIB}

db2soc: db2soc.o $(LOCASL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
	 $< ${DEPLIBS} ${RATLIB} ${LOCASLIB}

locas2fit: locas2fit.o $(LOCASL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
	 $< ${DEPLIBS} ${RATLIB} ${LOCASLIB}

soc2soc: soc2soc.o $(LOCASL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
	 $< ${DEPLIBS} ${RATLIB} ${LOCASLIB}

$(LOCASL): $(LOCASO) $(DICT_O)
	@echo " == Executing $@ =="
	@echo "linking $(LOCASL)"
	$(CXX) $(LDFLAGS) $(LDOPTS) \
	-o $@ $(LOCASO) ${DEPLIBS} ${RATLIB} $(DICT_O) $(LINKCMD)

$(DICT_C): $(DICT_H) $(LOCASO) $(LOCASH)
	@echo " == Executing $@ =="
	@rm -f $@
	@echo "Generating $@ dictionary"
	rootcint -f $@ -c -p $(RATCFLAGS) $(LOCASH) $<
	@echo "Done with $@ ..."


%.o:	%.cc %.hh
#	@echo Special rule for .cc files with headers
	$(CXX) ${PICFLAG} ${DEPCFLAGS} ${RATCFLAGS} ${LOCASCFLAGS} -DCOMPILED_ROOT -c $<

%.o:	%.C %.h
#	@echo Special rule for .C files with headers
	$(CXX) ${PICFLAG} ${DEPCFLAGS} ${RATCFLAGS} ${LOCASCFLAGS} -DCOMPILED_ROOT -c $<

%.o:	%.C
#	@echo Rule for .C files
	$(CXX) ${PICFLAG} ${DEPCFLAGS} ${RATCFLAGS} ${LOCASCFLAGS} -DCOMPILED_ROOT -c $<

%.o:	%.cc
#	@echo Rule for .cc files
	$(CXX) ${PICFLAG} ${DEPCFLAGS} ${RATCFLAGS} ${LOCASCFLAGS} -DCOMPILED_ROOT -c $<
