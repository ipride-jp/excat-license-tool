#!/bin/sh

#rm -fr CA
#tar -xvf CA.tar
openssl x509 -inform PEM -outform DER -in CA/cacert.pem -out cacert.der
./der2hex.pl
