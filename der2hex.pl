#!/usr/bin/perl

open(FR, "cacert.der") || die;
open(FW, ">cacert.hex") || die;

while (read(FR, $bs, 10)) {
    for $b (unpack('C*', $bs)) {
	printf FW " 0x%02x,", $b;
    }
    printf FW "\n";
}

close(FW);
close(FR);

# the end of file
