#
# Makefile for clon/cmon/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = cmon


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

## # '-fast' invokes '-xvector=yes', as result undefined symbols
## ifeq ("$(OSTYPE)","SunOS")
##   FFLAGS += -xvector=no
## endif

# if debugging
#ifeq ("$(OSTYPE)","SunOS")
#  FFLAGS = -g -KPIC -mt -DSunOS -e -ftrap=no%inexact,no%underflow
#  CFLAGS = -g -KPIC -mt -DSunOS -DOSTYPE="SunOS"
#endif

# use FORTRAN as linker: we'll link *.c and *.F code
ifeq ("$(OSTYPE)","Linux")
  LDCC = $(LDFC)
endif

CLON_FLAGS += -I./$(MAIN).s \
			-I./cmon.s -I./sim.s \
			-I$(CODA)/common/include \
			-I$(CODA)/src/codatt/codatt.s \
			-D_POSIX_PTHREAD_SEMANTICS

LIBNAMES += $(CODA)/src/codatt/$(OSTYPE_MACHINE)/lib/libcodatt.a

LIBNAMES += $(X11LIBS)
LIBNAMES += $(CERNLIBS)
LIBNAMES += $(F77LIBS)
LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_lib install_bin

	rm -rf tmp
	mkdir tmp
	cp sda.inc/map_manager.h tmp
	cp sda.inc/prlib.h tmp
	cp sda.inc/dclib.h tmp
	cp sda.inc/sclib.h tmp
	cp sda.inc/wcs.h tmp
	chmod 664 tmp/*
	cp tmp/* $(CLON)/common/include
	rm -rf tmp





















