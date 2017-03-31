# Makefile for the ROOT test programs.
# This Makefile shows nicely how to compile and link applications
# using the ROOT libraries on all supported platforms.
#
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Fons Rademakers, 29/2/2000

RC     := root-config
ifeq ($(shell which $(RC) 2>&1 | sed -ne "s@.*/$(RC)@$(RC)@p"),$(RC))
MKARCH := $(wildcard $(shell $(RC) --etcdir)/Makefile.arch)
RCONFIG := $(wildcard $(shell $(RC) --incdir)/RConfigure.h)
endif
ifneq ($(MKARCH),)
include $(MKARCH)
else
ifeq ($(ROOTSYS),)
ROOTSYS = ..
endif
include $(ROOTSYS)/etc/Makefile.arch
endif

#------------------------------------------------------------------------------

SEGMDICTO    = segmdict.$(ObjSuf) segmdictDict.$(ObjSuf)
SEGMDICTS    = segmdict.$(SrcSuf) segmdictDict.$(SrcSuf)
SEGMDICT     = segmdict$(ExeSuf)

#------------------------------------------------------------------------------

.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)
.PHONY:    Aclock Hello Tetris



ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so
		$(LD) $(SOFLAGS)$@ $(LDFLAGS) $^ $(OutPutOpt) $@ $(EXPLLINKLIBS)
ifneq ($(subst $(MACOSX_MINOR),,1234),1234)
ifeq ($(MACOSX_MINOR),4)
		ln -sf $@ $(subst .$(DllSuf),.so,$@)
endif
endif
else
		$(LD) $(SOFLAGS) $(LDFLAGS) $^ $(OutPutOpt) $@ $(EXPLLINKLIBS)
endif
		@echo "$@ done"




#sergey
CXXFLAGS += -I$(CODA)/src/evio-4.3/src.s \
			-I$(CLON)/src/trigger/trigger.s \
			-I$(CLON)/src/trigger/ec.s \
			-I$(CLON)/src/trigger/pc.s


GLIBS +=	$(CLON)/src/trigger/$(OSTYPE_MACHINE)/lib/libtrigger.a \
			$(CODA)/src/evio-4.3/$(OSTYPE_MACHINE)/lib/libevio.a \
			-lexpat -lpthread -lrt


OBJS         = $(SEGMDICTO)
PROGRAMS     = $(SEGMDICT)





all: $(PROGRAMS)


$(SEGMDICT):   $(SEGMDICTO)
		$(LD) $(LDFLAGS) $^ $(GLIBS) $(OutPutOpt)$@
		$(MT_EXE)
		@echo "$@ done"






clean:
		@rm -f $(OBJS) $(TRACKMATHSRC) core *Dict.*

distclean:      clean
		-@(mv -f stressRooStats_ref.root stressRooStats_ref.root- >/dev/null 2>&1;true)
		@rm -f $(PROGRAMS) *Dict.* *.def *.exp \
		   *.root *.ps *.so *.lib *.dll *.d *.log .def so_locations \
		   files/*

.SUFFIXES: .$(SrcSuf)

###

segmdict.$(ObjSuf): segmdict.h
segmdictDict.$(SrcSuf): segmdict.h segmdictLinkDef.h
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ -c $^

.$(SrcSuf).$(ObjSuf):
	$(CXX)  $(CXXFLAGS) -c $<

