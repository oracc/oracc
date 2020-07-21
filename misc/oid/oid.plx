#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8'; binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

use Data::Dumper;
use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::OID;

oid_args();

oid_load();

if (oid_check_mode()) {
    oid_check();
} elsif (oid_dump_mode()) {
    # fall through to oid_finish
} elsif (oid_edit_mode()) {
    oid_edit();
} else {
    oid_keys();
}

oid_finish();

1;

#######################################################################
# print Dumper \%oid_keys;
# print Dumper \%oid_doms;
# print Dumper \%oid_ext;
# exit 1;
