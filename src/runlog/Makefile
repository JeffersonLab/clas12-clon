#
# Makefile for clon/runlog/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = runlog


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include

# if want debugging mode, uncomment following lines
ifeq ("$(OSTYPE)","SunOS")
    CFLAGS = -g -KPIC -mt -DSunOS -DOSTYPE="SunOS"
    LDFLAGS = -g -KPIC -mt
endif
ifeq ("$(OSTYPE_MACHINE)","SunOS_i86pc")
    CFLAGS += -DSunOS_i86pc
endif
ifeq ("$(OSTYPE_MACHINE)","SunOS_sun4u")
    CFLAGS += -DSunOS_sun4u
endif
ifdef RTPC_RUN_LOG
    CFLAGS += -D_RTPC_RUN_LOG_
endif

#-----------------------
# customize definitions
#-----------------------

#RCDB
CLON_FLAGS += -std=c++11 -DRCDB_MYSQL -I/usr/include/mysql -I$(RCDB_HOME)/cpp/include


CLON_FLAGS += -I./$(MAIN).s -I$(CODA)/common/include -I$(CLON)/common/include \
				-I$(CODA)/src/ipc/ipc.s \
				-I$(CLON)/src/epicsutil/epicsutil.s \
				-I$(IPC_HOME)/include/activemq-cpp-$(IPC_VER)

ifeq ("$(OSTYPE_MACHINE)","Linux_i686")
  CLON_FLAGS += -I/usr/local/include/apr-1
else
  CLON_FLAGS += -I/usr/include/apr-1
endif


LIBNAMES += $(CLON)/src/epicsutil/$(OSTYPE_MACHINE)/lib/libepicsutil.a
LIBNAMES += $(CODA)/$(OSTYPE_MACHINE)/lib/libcodautil.a
LIBNAMES += $(CLON)/src/clonutil/$(OSTYPE_MACHINE)/lib/libclonutil.a
LIBNAMES += $(CODA)/$(OSTYPE_MACHINE)/lib/libipc.a

ifeq ("$(OSTYPE_MACHINE)","Linux_armv7l")
  LIBNAMES += /usr/lib/libmysqlclient.so
else ifeq ("$(OSTYPE_MACHINE)","Linux_x86_64")
  LIBNAMES += /usr/lib64/mysql/libmysqlclient.so
else ifeq ("$(OSTYPE_MACHINE)","Linux_i686_vme")
    LIBNAMES += /usr/lib/mysql/libmysqlclient.so
else ifeq ("$(OSTYPE_MACHINE)","Linux_i686")
    LIBNAMES += /usr/lib/mysql/libmysqlclient.so
endif

# EPICS libraries
##LIBNAMES += $(EPICS_BASE)/lib/linux-x86/libca.a
##LIBNAMES += $(EPICS_BASE)/lib/linux-x86/libCom.a
LIBNAMES += $(EPICS_BASE)/lib/linux-x86_64/libca.a
LIBNAMES += $(EPICS_BASE)/lib/linux-x86_64/libCom.a

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

LIBNAMES += -lresolv

ifeq ("$(OSTYPE)","Linux")
  LIBNAMES += -lreadline
  LIBNAMES += -lcurses
  LIBNAMES += -lpthread
  LIBNAMES += -lrt
endif

LIBNAMES += $(X11LIBS)
LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_bin
#	rm -rf tmp
#	mkdir tmp
#	cp $(MAIN).s/*.h tmp
#	chmod 664 tmp/*
#	cp tmp/* $(CLON)/common/include
#	rm -rf tmp





















