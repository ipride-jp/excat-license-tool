#!/usr/bin/perl

my $CNF = "-config openssl.cnf";
my $RKY = "CA/private/cakey.pem";
my $POL = "-policy policy_excat";
my $RCT = "CA/cacert.pem";


sub gen_root {
    my $IDX = 1;
# generate root certificate
    $ENV{'C'} = 'JP';
    $ENV{'ST'} = 'Tokyo';
    $ENV{'L'} = 'Chuo';
    $ENV{'O'} = 'iPride Co., Ltd.';
    $ENV{'CN'} = 'iPride Co., Ltd.';
    $ENV{'emailAddress'} = 'info@ipride.co.jp';
    $ENV{'info'} = '';
    $ENV{'macAddress'} = '';
    system("openssl genrsa -out $RKY 4096");
    system("openssl req -out careq.pem -new -key $RKY $CNF -batch");
    system("openssl ca $CNF -startdate 060401000000Z -enddate 070401000000Z $POL -keyfile $RKY -selfsign -in careq.pem -out CA/cacert.pem -notext -batch");
    system("openssl x509 -inform PEM -outform DER -in $RCT -out cacert.der");
    $IDX++;
}


sub gen_cli_cer {
    $ENV{'C'} = 'JP';
    $ENV{'ST'} = 'Tokyo';
    $ENV{'L'} = 'Shinjuku';
    $ENV{'O'} = $ENV{'CN'} = "TEST-${IDX}";
    $ENV{'emailAddress'} = '';
    $ENV{'info'} = shift;
    $ENV{'macAddress'} = "MACADDRESS-${IDX}";
    my $st = shift;
    my $ed = shift;

    my $k = "CA/private/c${IDX}key.pem";
    my $r = "test/c-${IDX}req.pem";
    my $c = "test/c-${IDX}cert.pem";

    system("openssl genrsa -out $k 1024");
    system("openssl req -out $r -new -key $k $CNF -batch");
    system("openssl ca $CNF -startdate $st -enddate $ed $POL -keyfile $RKY -cert $RCT -in $r -out $c -notext -batch\n");
    unlink $r;

    $IDX++;
}

sub gen_clients {
    local $IDX = 2;

    &gen_cli_cer("Excat<=1.0", "060401000000Z", "070401000000Z");

    &gen_cli_cer("Excat<=1.0", "050401000000Z", "060401000000Z");

    $ENV{'C'} = 'JP';
    $ENV{'ST'} = 'Hokkaido';
    $ENV{'L'} = 'Sapporo';
    $ENV{'O'} = 'Another Root CA';
    $ENV{'CN'} = 'Another Root CA';
    $ENV{'emailAddress'} = 'administrator@example.org';
    $ENV{'info'} = 'Excat<=1.0';
    $ENV{'macAddress'} = "MACADDRESS-${IDX}";
    system("openssl genrsa -out CA/private/acakey.pem 1024");
    system("openssl req -out acareq.pem -new -key CA/private/acakey.pem $CNF -batch");
    system("openssl ca $CNF -startdate 060401000000Z -enddate 070401000000Z $POL -keyfile CA/private/acakey.pem -selfsign -in acareq.pem -out CA/acacert.pem -notext -batch");
    $IDX++;
}


if ($ARGV[0] eq 'root') {
    &gen_root;
} elsif ($ARGV[0] eq 'clients') {
    &gen_clients;
}

# the end of file
