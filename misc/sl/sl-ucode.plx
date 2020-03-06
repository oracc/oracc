#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use lib "$ENV{'ORACC'}/lib";
use ORACC::SL::BaseC;

ORACC::SL::BaseC::init();

my @sl = `cat 00lib/ogsl.asl`; chomp @sl;

for (my $i = 0; $i <= $#sl; ++$i) {
    if (($sl[$i] =~ /^\@sign/ || $sl[$i] =~ /^\@form/) && $sl[$i] =~ /\|/) {
	unless (has_ucode($i)) {
	    my($s) = ($sl[$i] =~ /(\S+)\s*$/);
	    my $u = ORACC::SL::BaseC::tlit2uni('',$s);
	    my @m = ORACC::SL::BaseC::messages();
	    warn(join("\n", @m), "\n") if $#m >= 0;
	    $u =~ s/\&#//g;
	    $u =~ s/;\s*$//;
	    $u =~ tr/;/./;
	    print "\@ucode $u\n"
		unless !$u || $u =~ /X/;
	}
    }
}

ORACC::SL::BaseC::term();

#####################################################################################

sub has_ucode {
    my $i = shift;
    until ($sl[$i] =~ /^\@form/ || $sl[$i] =~ /^\@end\s/) {
	return 1 if $sl[$i] =~ /^\@ucode/;
	++$i;
    }
    0;
}

1;
