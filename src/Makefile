#
# Makefile for clas/src/ directory
#

MAKE = gmake

all:
	-mkdir -p $(CLON)/common/include
	cd clonutil;                         $(MAKE)
	- cd clonutil;                       $(MAKE) java
	- cd clonutil;                       $(MAKE) install
##	- cd minuit;                         $(MAKE)
##	- cd minuit;                         $(MAKE) install
##	- cd cmon;                           $(MAKE)
##	- cd cmon;                           $(MAKE) install
	- cd ipc;                            $(MAKE)
	- cd ipc;                            $(MAKE) java
	- cd ipc;                            $(MAKE) install
	- cd infoserver;                     $(MAKE)
	- cd infoserver;                     $(MAKE) install
	- cd timeline;                       $(MAKE)
	- cd timeline;                       $(MAKE) install
	- cd dbutil;                         $(MAKE)
	- cd dbutil;                         $(MAKE) java
	- cd dbutil;                         $(MAKE) install
	- cd epicsutil;                      $(MAKE)
	- cd epicsutil;                      $(MAKE) install
	- cd runlog;                         $(MAKE)
	- cd runlog;                         $(MAKE) install
	- cd alarm;                          $(MAKE)
	- cd alarm;                          $(MAKE) java
	- cd alarm;                          $(MAKE) install
###	- cd scripts;                        $(MAKE) install

#	- cd evmon;                          $(MAKE)
#	- cd evmon;                          $(MAKE) install
#	- cd laser;                          $(MAKE)
#	- cd laser;                          $(MAKE) install
#	- cd level1;                         $(MAKE)
#	- cd level1;                         $(MAKE) install
#	- cd level2;                         $(MAKE)
#	- cd level2;                         $(MAKE) java
#	- cd level2;                         $(MAKE) install
#	- cd pedman;                         $(MAKE)
#	- cd pedman;                         $(MAKE) install
#	- cd prtr;                           $(MAKE)
#	- cd prtr;                           $(MAKE) install
#	- cd cmroot;                         $(MAKE)
#	- cd cmroot;                         $(MAKE) install
#	- cd scalers;                        $(MAKE)
#	- cd scalers;                        $(MAKE) install
#	- cd stadis;                         $(MAKE) install



clean:
	cd clonutil;                         $(MAKE) clean
	- cd minuit;                         $(MAKE) clean
	- cd cmon;                           $(MAKE) clean
	- cd ipc;                            $(MAKE) clean
	- cd infoserver;                     $(MAKE) clean
	- cd timeline;                       $(MAKE) clean
	- cd dbutil;                         $(MAKE) clean
	- cd epicsutil;                      $(MAKE) clean
	- cd runlog;                         $(MAKE) clean
	- cd alarm;                          $(MAKE) clean
###	- cd scripts;                        $(MAKE) clean

distclean:
	rm -rf $(CLON)/common/*
	rm -rf $(CLON)/$(OSTYPE_MACHINE)/*

###########################################



