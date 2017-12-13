#
# Makefile for clon/epicsutil/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = epicsutil


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------
CLON_FLAGS += -std=c++11 -I$(RCDB_HOME)/cpp/include

CLON_FLAGS += -I./$(MAIN).s -I$(EPICS_BASE)/include

ifeq ("$(OSTYPE)","SunOS")
  CFLAGS = -g -DSunOS -DOSTYPE="SunOS" -DSunOS_sun4u
  CLON_FLAGS += -I$(EPICS_BASE)/include/os/solaris
endif
ifeq ("$(OSTYPE)","Linux")
  CLON_FLAGS += -I$(EPICS_BASE)/include -I$(EPICS_BASE)/include/os/Linux -I$(EPICS_BASE)/src/catools
endif

CLON_FLAGS +=	-I$(CODA)/common/include \
				-I$(CLON)/common/include



## smartsockets
##CLON_FLAGS += -I$(RTHOME)/include

## activemq
CLON_FLAGS += -std=c++11 
CLON_FLAGS +=	-I$(IPC_HOME)/src/main -I$(CODA)/src/ipc/ipc.s
ifeq ("$(OSTYPE_MACHINE)","Linux_i686")
  CLON_FLAGS += -I/usr/local/include/apr-1
else
  CLON_FLAGS += -I/usr/include/apr-1
endif





LIBNAMES += $(CODA)/$(OSTYPE_MACHINE)/lib/libipc.a
LIBNAMES += $(CODA)/$(OSTYPE_MACHINE)/lib/libcodautil.a
LIBNAMES += $(CLON)/src/clonutil/$(OSTYPE_MACHINE)/lib/libclonutil.a

## activemq
LIBNAMES += $(CODA)/$(OSTYPE_MACHINE)/lib/libmysql4.1.20.a

LIBNAMES += $(EPICS_BASE)/lib/$(EPICS_HOST_ARCH)/libcas.a
LIBNAMES += $(EPICS_BASE)/lib/$(EPICS_HOST_ARCH)/libca.a
LIBNAMES += $(EPICS_BASE)/lib/$(EPICS_HOST_ARCH)/libgdd.a
LIBNAMES += $(EPICS_BASE)/lib/$(EPICS_HOST_ARCH)/libCom.a

## activemq
ifeq ("$(OSTYPE_MACHINE)","Linux_i686")
  LIBNAMES += /usr/local/lib/libactivemq-cpp.so
  LIBNAMES += /usr/local/lib/libapr-1.so
  LIBNAMES += -luuid -lrt -lcrypt -lssl -lcrypto -ldl -lz
  LIBNAMES += -L/usr/kerberos/lib -lpthread -pthread -Wl,-rpath -Wl,/usr/local/lib
else
  LIBNAMES += /usr/local/lib/libactivemq-cpp.so
  LIBNAMES += -lapr-1
  LIBNAMES += -lrt -lcrypt -lssl -lcrypto -ldl -lz
  LIBNAMES += -L/usr/kerberos/lib -lpthread -pthread -Wl,-rpath -Wl,/usr/local/lib
endif







ifeq ("$(OSTYPE)","Linux")
  LIBNAMES += -lreadline
  LIBNAMES += -lcurses
  LIBNAMES += -lpthread
  LIBNAMES += -lrt
endif

##LIBNAMES += -lmysqlclient
LIBNAMES += -lresolv
##LIBNAMES += -lCrun
LIBNAMES += $(X11LIBS)
LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_lib install_bin
	rm -rf tmp
	mkdir tmp
	cp $(MAIN).s/epics_server.h tmp
	cp $(MAIN).s/epicsutil.h tmp
	cp $(MAIN).s/MessageActionEPICS.h tmp
	chmod 664 tmp/*
	cp tmp/* $(CLON)/common/include
	rm -rf tmp





















