lib_LTLIBRARIES=liblicense.la
liblicense_la_SOURCES = \
       certificate.c\
       license.c\
       version.c\
       der2hex.c\
       java/jp_co_ipride_License.c

LIBS=$LIBS  -lssl -lcrypto  
liblicense_la_LDFLAGS = -version-info @LT_VERSION@  $(LIBS) -shrext .so
liblicensedir=$(includedir)
liblicense_HEADERS=license.h
AM_CPPFLAGS =-I$(JAVA_HOME)/include/ -I$(JAVA_HOME)/include/solaris
