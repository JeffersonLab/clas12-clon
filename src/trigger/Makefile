#
# Makefile for clon/trigger/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = trigger


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include
include $(CODA)/src/Makefile.arch




#-----------------------
# customize definitions
#-----------------------
CFLAGS = -g
CXXFLAGS = -g

CLON_FLAGS += -I./$(MAIN).s \
			-I./ec.s -I./pc.s -I./dc.s -I./http.s \
			-I$(CODA)/common/include \
			-I$(ROOTSYS)/include \
			-D_POSIX_PTHREAD_SEMANTICS \
			-I/apps/Xilinx/Vivado_HLS/2013.2/include

#			-I/apps/Xilinx/Vivado_HLS/2015.4/include

#ccdb
CLON_FLAGS += -I$(CCDB_HOME)/include \
			-I$(CCDB_HOME)/include/SQLite
LIBNAMES += $(CCDB_HOME)/lib/libccdb.a
LIBNAMES += $(CCDB_HOME)/tmp/SQLite/libccdb_sqlite.a


LIBNAMES += $(CODA)/src/evio-4.3/$(OSTYPE_MACHINE)/lib/libevio.a

LIBNAMES += $(ROOTGLIBS)

LIBNAMES += $(X11LIBS)
LIBNAMES += $(SYSLIBS)
LIBNAMES += -lpthread -lrt


#-------------------
# customize targets
#-------------------
install: install_lib install_bin
#	rm -rf tmp
#	mkdir tmp
#	cp sda.inc/map_manager.h tmp
#	cp sda.inc/prlib.h tmp
#	cp sda.inc/dclib.h tmp
#	cp sda.inc/sclib.h tmp
#	cp sda.inc/wcs.h tmp
#	chmod 664 tmp/*
#	cp tmp/* $(CLON)/common/include
#	rm -rf tmp





















