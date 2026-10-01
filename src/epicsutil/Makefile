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

#
# EPICS version 7 !!!
#

ifeq ("$(OSTYPE)","Linux")
  CLON_FLAGS += -I$(EPICS_BASE)/include -I$(EPICS_BASE)/include/os/Linux -I$(EPICS_SUPPORT)/pcas/include
endif

CLON_FLAGS += -I/usr/include/mysql

CLON_FLAGS +=	-I$(CODA)/common/include -I$(CLON)/common/include

## activemq
CLON_FLAGS += -std=c++11 
CLON_FLAGS +=	-I$(IPC_HOME)/include/activemq-cpp-$(IPC_VER) -I$(CODA)/src/ipc/ipc.s
ifeq ("$(OSTYPE_MACHINE)","Linux_i686")
  CLON_FLAGS += -I/usr/local/include/apr-1
else
  CLON_FLAGS += -I/usr/include/apr-1
endif

LIBNAMES += $(CODA)/$(OSTYPE_MACHINE)/lib/libipc.a
LIBNAMES += $(CODA)/$(OSTYPE_MACHINE)/lib/libcodautil.a
LIBNAMES += $(CLON)/src/clonutil/$(OSTYPE_MACHINE)/lib/libclonutil.a

LIBNAMES += $(MYSQLLIBS)

LIBNAMES += $(EPICS_SUPPORT)/pcas/lib/$(EPICS_HOST_ARCH)/libcas.a
LIBNAMES += $(EPICS_SUPPORT)/pcas/lib/$(EPICS_HOST_ARCH)/libgdd.a

LIBNAMES += $(EPICS_BASE)/lib/$(EPICS_HOST_ARCH)/libca.a
LIBNAMES += $(EPICS_BASE)/lib/$(EPICS_HOST_ARCH)/libCom.a


## activemq
ifeq ("$(OSTYPE_MACHINE)","Linux_i686")
  LIBNAMES += $(IPC_HOME)/lib/libactivemq-cpp.so
  LIBNAMES += /usr/local/lib/libapr-1.so
  LIBNAMES += -luuid -lrt -lcrypt -lssl -lcrypto -ldl -lz
  LIBNAMES += -L/usr/kerberos/lib -lpthread -pthread -Wl,-rpath -Wl,/usr/local/lib
else
  LIBNAMES += $(IPC_HOME)/lib/libactivemq-cpp.so
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

LIBNAMES += -lresolv
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





















