#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use lib "$ENV{'ORACC'}/lib";
use ORACC::CBD::Util;
use ORACC::CBD::Forms;

my $f = shift @ARGV;
open(F,$f) || die "$0: can't open $f to check forms\n";
while (<F>) {
    warn "$f:$.: bad chars in form $ORACC::CBD::Forms::forms_check_form\n"
	unless ORACC::CBD::Forms::form_chars_ok($_);
}
close(F);
1;
