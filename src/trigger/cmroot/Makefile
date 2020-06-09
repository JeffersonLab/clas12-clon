#
# Makefile for clon/src/trigger/cmroot
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = cmroot


#------------------
# generic Makefile
#------------------
include $(CODA)/src/Makefile.include

#temporary because of old root
#CC = /apps/gcc/4.9.4/bin/gcc 
#CXX = /apps/gcc/4.9.4/bin/g++ 
#temporary because of old root


ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs) 
ROOTGLIBS    := $(shell root-config --glibs)

#-----------------------
# customize definitions
#-----------------------
CFLAGS = -g -Wno-deprecated -fPIC -DLinux -DLINUX -DLinux_x86_64  

CLON_FLAGS += -std=c++11

CLON_FLAGS += -I./src.s \
				-I$(CLON)/src/trigger/trigger.s \
 				-I$(CLON)/src/trigger/ec.s \
				-I$(CLON)/src/trigger/pc.s \
				-I$(CLON)/src/cmon/cmon.s \
				-I$(CODA)/src/codatt/codatt.s \
				-I$(CODA)/common/include -I$(CODA)/common/include \
				-I$(IPC_HOME)/include/activemq-cpp-$(IPC_VER) \
				-I$(CODA)/src/ipc/ipc.s \
				-I$(CODA)/src/codautil/codautil.s


ifeq ("$(OSTYPE_MACHINE)","Linux_i686")
  CLON_FLAGS += -I/apps/Xilinx/Vivado_HLS/2013.2/include
else
  CLON_FLAGS += -I/apps/Xilinx/Vivado_HLS/2015.4/include
endif

# for json.hpp
CLON_FLAGS += -I$(RCDB_HOME)/cpp/include

CLON_FLAGS += $(ROOTCFLAGS)

ifeq ("$(OSTYPE_MACHINE)","Linux_i686")
  CLON_FLAGS += -I/usr/local/include/apr-1
else ifeq ("$(OSTYPE_MACHINE)","Linux_i686_vme")
  CLON_FLAGS += -I/usr/local/include/apr-1
else
  CLON_FLAGS += -I/usr/include/apr-1
endif


LIBNAMES += $(CLON)/src/trigger/$(OSTYPE_MACHINE)/lib/libtrigger.a
LIBNAMES += $(CLON)/src/cmon/$(OSTYPE_MACHINE)/lib/libcmon.a

LIBNAMES += $(CODA)/$(OSTYPE_MACHINE)/lib/libipc.a
LIBNAMES += $(CODA)/$(OSTYPE_MACHINE)/lib/libcodautil.a
LIBNAMES += $(CODA)/$(OSTYPE_MACHINE)/lib/libet.a
LIBNAMES += $(CODA)/$(OSTYPE_MACHINE)/lib/libevio.a


LIBNAMES += $(ROOTLIBS) $(ROOTGLIBS)

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

#for old solaris only ???
LIBNAMES += -lresolv

LIBNAMES += $(X11LIBS)
LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_bin

#install: install_lib install_bin
#	rm -rf tmp
#	mkdir tmp
#	cp $(MAIN).s/*.h tmp
#	chmod 664 tmp/*
#	cp tmp/* $(CODA)/common/include
#	rm -rf tmp





















