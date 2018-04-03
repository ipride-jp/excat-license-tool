#!/bin/sh
set prefix=/usr/local
set exec_prefix=${prefix}
set libdir=${exec_prefix}/lib
java -Djava.library.path=${libdir} -classpath .:${libdir}/License.jar Checker $*
# the end of file
