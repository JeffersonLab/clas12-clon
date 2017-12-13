#
# Makefile for clon/dbutil/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = dbutil


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------

#RCDB
CLON_FLAGS += -std=c++11 -DRCDB_MYSQL -I/usr/include/mysql -I$(RCDB_HOME)/cpp/include

CLON_FLAGS += -Imain -Idbutil.s \
				-I$(CODA)/common/include -I$(CLON)/common/include \
				-I$(IPC_HOME)/src/main 

ifeq ("$(OSTYPE_MACHINE)","Linux_i686")
  CLON_FLAGS += -I/usr/local/include/apr-1
else
  CLON_FLAGS += -I/usr/include/apr-1
endif

##JFLAGS = -classpath ./:$(CLON)/common/jar/clonutil.jar:$(IMPORT_JAR)/klg.jar:$(IMPORT_JAR)/symclass.zip:$(IMPORT_JAR)/symbeans.jar:$(IMPORT_JAR)/ss.jar


LIBNAMES += $(CODA)/$(OSTYPE_MACHINE)/lib/libcodautil.a
LIBNAMES += $(CODA)/$(OSTYPE_MACHINE)/lib/libipc.a
LIBNAMES += $(CODA)/$(OSTYPE_MACHINE)/lib/libmysql4.1.20.a


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
  LIBNAMES += -lpthread
endif

##############################LIBNAMES += -lmysqlclient
LIBNAMES += -lresolv
LIBNAMES += $(X11LIBS)
LIBNAMES += $(SYSLIBS)



#-------------------
# customize targets
#-------------------
install: install_bin
# install_java
#	rm -rf tmp
#	mkdir tmp
#	cp $(MAIN).s/*.h tmp
#	chmod 664 tmp/*
#	cp tmp/* $(CLON)/common/include
#	rm -rf tmp





















