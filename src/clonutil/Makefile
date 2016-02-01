#
# Makefile for clon/clonutil/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = clonutil


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

CLON_FLAGS += -I./$(MAIN).s -I$(EPICS_BASE)/include \
				-I$(EPICS_BASE)/include/os/solaris \
				-I$(EPICS_EXTENSIONS)/src/ezca \
				-I$(CODA)/common/include -I$(CLON)/common/include

###				-I$(RTHOME)/include \

JFLAGS = -classpath $(CLAS)/jar/ss.jar


# 32bit !!!
ifeq ("$(OSTYPE)","Linux")
  LIBNAMES += $(CODA)/Linux_i686/lib/libmysql4.1.20.a
else
  LIBNAMES += $(CODA_LIB)/libmysql4.1.20.a
endif

#for old solaris only ???
LIBNAMES += -lresolv

LIBNAMES += $(X11LIBS)
LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_lib install_bin install_java
	rm -rf tmp
	mkdir tmp
	cp $(MAIN).s/*.h tmp
	chmod 664 tmp/*
	cp tmp/* $(CLON)/common/include
	rm -rf tmp





















