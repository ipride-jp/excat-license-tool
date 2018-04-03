#!/bin/sh

COMM="./test-main"

# OK, c-2cert.pem is certified to Excat<=1.0
$COMM c-2cert.pem "Excat=0.9"
if [ $? -eq 0 ];
then
    echo "OK";
else
    echo "BROKEN!"
    exit -1;
fi

# OK, c-2cert.pem is certified to Excat<=1.0
$COMM c-2cert.pem "Excat=1.0"
if [ $? -eq 0 ];
then
    echo "OK";
else
    echo "BROKEN!"
    exit -1;
fi

# FAIL, c-2cert.pem is certified to Excat<=1.0
$COMM c-2cert.pem "Excat=1.1"
if [ $? -ne 0 ];
then
    echo "OK";
else
    echo "BROKEN!"
    exit -1;
fi

# FAIL, c-3cert.pem is out of date
$COMM c-3cert.pem "Excat=1.0"
if [ $? -ne 0 ];
then
    echo "OK";
else
    echo "BROKEN!"
    exit -1;
fi

# FAIL, c-4cert.pem is not certified by iPride's CA
$COMM c-4cert.pem "Excat=1.0"
if [ $? -ne 0 ];
then
    echo "OK";
else
    echo "BROKEN!"
    exit -1;
fi

# the end of file
