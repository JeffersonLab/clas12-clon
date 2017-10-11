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



#RCDB
CLON_FLAGS += -std=c++11 -DRCDB_MYSQL -I/usr/include/mysql -I$(RCDB_HOME)/cpp/include


#CLON_FLAGS += -I./$(MAIN).s -I$(CODA)/common/include -I$(CLON)/common/include \
#				-I$(RTHOME)/include

CLON_FLAGS += -I./$(MAIN).s -I$(CODA)/common/include -I$(CLON)/common/include \
				-I$(IPC_HOME)/src/main

#epics
#CLON_FLAGS += -I$(EPICS_BASE)/include
#ifeq ("$(OSTYPE)","SunOS")
#  CFLAGS = -g -DSunOS -DOSTYPE="SunOS" -DSunOS_sun4u
#  CLON_FLAGS += -I$(EPICS_BASE)/include/os/solaris
#endif
#ifeq ("$(OSTYPE)","Linux")
#  CLON_FLAGS += -I$(EPICS_BASE)/include -I$(EPICS_BASE)/include/os/Linux -I$(EPICS_BASE)/src/catools
#endif
#epics

ifeq ("$(OSTYPE_MACHINE)","Linux_i686")
  CLON_FLAGS += -I/usr/local/include/apr-1
else
  CLON_FLAGS += -I/usr/include/apr-1
endif




#epics
#LIBNAMES += $(EPICS_BASE)/lib/$(EPICS_HOST_ARCH)/libcas.a
#LIBNAMES += $(EPICS_BASE)/lib/$(EPICS_HOST_ARCH)/libca.a
#LIBNAMES += $(EPICS_BASE)/lib/$(EPICS_HOST_ARCH)/libgdd.a
#LIBNAMES += $(EPICS_BASE)/lib/$(EPICS_HOST_ARCH)/libCom.a
#epics


LIBNAMES += $(CLON)/$(OSTYPE_MACHINE)/lib/libclonutil.a
#LIBNAMES += $(CLON)/$(OSTYPE_MACHINE)/lib/libepicsutil.a
LIBNAMES += $(CODA)/$(OSTYPE_MACHINE)/lib/libmysql4.1.20.a
LIBNAMES += $(CODA)/$(OSTYPE_MACHINE)/lib/libet.a
LIBNAMES += $(CODA)/$(OSTYPE_MACHINE)/lib/libevio.a



#LIBNAMES += -L$(RTHOME)/lib/i86_linux -lrtipc -lrtipcl -lrtutil -lrtcxxipc50

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






#######JFLAGS = -classpath $(CLON)/common/jar/clonutil.jar:$(IMPORT_JAR)/ss.jar



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





















