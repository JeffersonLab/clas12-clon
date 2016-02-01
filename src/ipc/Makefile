#
# Makefile for clon/ipc/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = ipc


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

# use C++ as linker instead of CC: we'll link main *.c program with
# SmartSocket's C++ libraries, and using CC will not resolve references
#ifeq ("$(OSTYPE)","Linux")
#  LDCC = $(CXX)
#endif
# default now !!!



CLON_FLAGS += -I./$(MAIN).s -I$(CODA)/common/include -I$(CLON)/common/include \
				-I$(RTHOME)/include

LIBNAMES += $(globallib)/libclonutil.a

### 32-bit !!!
ifeq ("$(OSTYPE)","Linux")
  LIBNAMES += $(CODA)/Linux_i686/lib/libet.a
  LIBNAMES += $(CODA)/Linux_i686/lib/libevio.a
  LIBNAMES += $(CODA)/Linux_i686/lib/libmysql4.1.20.a
  LIBNAMES += -L$(RTHOME)/lib/i86_linux -lrtipc -lrtipcl -lrtutil -lrtcxxipc50
else
  LIBNAMES += $(CODA_LIB)/libet.a
  LIBNAMES += $(CODA_LIB)/libevio.a
  LIBNAMES += $(CODA_LIB)/libmysql4.1.20.a
  LIBNAMES += -L$(RTHOME)/lib/$(OSTYPE_MACHINE) -lrtipc -lrtipcl -lrtutil -lrtcxxipc50
endif




JFLAGS = -classpath $(CLON)/common/jar/clonutil.jar:$(CLAS)/jar/ss.jar



#for 'inet_aton'
LIBNAMES += -lresolv

ifeq ("$(OSTYPE)","SunOS")
  LIBNAMES += -lCrun
endif

ifeq ("$(OSTYPE)","Linux")
  LIBNAMES += -lpthread
endif


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
	-mkdir -p $(CLON)/common/include
	cp tmp/* $(CLON)/common/include
	rm -rf tmp





















