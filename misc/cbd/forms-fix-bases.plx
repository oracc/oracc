#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use ORACC::CBD::Bases;
use ORACC::CBD::BaseSigs;

use Getopt::Long;

my $auto = 1;
my $form = '';
my $lang = 'sux';
my $oracc = $ENV{'ORACC_BUILDS'};
my $proj = 'epsd2';

GetOptions(
    'auto'=>\$auto,
    'form:s'=>\$form,
    'lang:s'=>\$lang,
    'proj:s'=>\$proj,
    );

die "$0: ORACC environment not set\n" unless $oracc;

die "$0: -forms FILE required; -lang and -proj optional (defaults epsd2 and sux).\n"
    unless $form && $lang && $proj;

#my $basesigs = "$oracc/$proj/01bld/$lang/base-sigs.tab";
#die "$0: no $basesigs\n\nYou can create this file with:\n\t(cd $oracc/$proj ; cbdpp.plx -c -bases 00lib/$lang.glo)\n"
#    unless -r $basesigs;

my %t = (); 
# my %t = basesigs_load($basesigs);

my $cancel = 0;
collect_bases();

if ($cancel) {
    die "$0: run commands listed before this line and try again\n";
}

my $auto_str = " (autofixed)" if $auto;
ORACC::SL::BaseC::init();
open(F,$form) || die "$0: strange; can't open readable file $form\n";
my $lnum = 0;
while (<F>) {
    ++$lnum;
    if (/\s\%(\S+)/) {
	print and next unless $1 eq $lang;
    }
    if (/^(.*?)\t\@form/) {
	my $c = $1;
	my $cfgw_ok = ($c =~ s/^\+//); 
	if ($t{$c}) {
	    my $b = '';
	    if (m#\s/(\S+)#) {
		$b = $1;
		my $sig = ORACC::SL::BaseC::check(undef,$b,1);
		my $base_ok = ($b =~ s/^\+//);
		# warn "checking $b with sig $sig\n";
		my %csigs = %{$t{$c}};
		if ($csigs{$sig}) {
		    my %sigtlits = %{$csigs{$sig}};
		    my @tlits = keys %sigtlits;
		    my $match = 0;
		    foreach my $t (@tlits) {
			if ($b eq $t) {
			    $match = 1;
			    last;
			}
		    }
		    unless ($match) {
			if ($#tlits > 0) {
			    warn "$form:$lnum: $b should be one of @tlits\n";
			} else {
			    warn "$form:$lnum: $b is really $tlits[0]$auto_str\n";
			    s#\s/(\S+)# /$tlits[0]# if $auto;
			}
		    }
		} else {
		    warn "$form:$lnum: unknown base, write /+$b to okay it for $c [sig=$sig]\n"
			unless $base_ok || $cfgw_ok;
		}
	    }
	} else {
	    warn "$form:$lnum: unknown CFGW, write +$c to okay it\n"
		unless $cfgw_ok;
	}
    } else {
	warn "$form:$lnum: malformed line in .forms file\n";
    }
    s/^\+//; s#\s/\+# /#;
    print;
}
ORACC::SL::BaseC::term();

#############################################################################################

sub collect_bases {
    my @b = ();
    if ($lang eq 'sux') {
	push @b, 'epsd2:sux', 'epsd2/names:qpn';
	push @b, '.:sux' if -r '00lib/sux.glo';
	push @b, '.:qpn' if -r '00lib/qpn.glo';
    } else {
	push @b, 'epsd2/emesal:sux-x-emesal';
	push @b, '.:sux-x-emesal' if -r '00lib/sux-x-emesal.glo';
	push @b, '.:qpn' if -r '00lib/qpn.glo';
    }
    my @sigtabs = ();
    foreach my $b (@b) {
	my($p,$l) = ($b =~ /^(.*?):(.*?)$/);
	$p = "$oracc/$p" unless $p eq '.';
	my $sigtab = "$p/01bld/$l/base-sigs.tab";
	if (-r $sigtab) {
	    warn "using $sigtab\n";
	    push @sigtabs, $sigtab;
	} else {
	    ++$cancel;
	    if ($p eq '.') {
		warn "cbdpp.plx -c -bases 00lib/$l.glo\n";
	    } else {
		warn "(cd $p ; cbdpp.plx -c -bases 00lib/$l.glo)\n";
	    }
	}
    }
    unless ($cancel) {
	%t = basesigs_load_files(@sigtabs);
    }
}

1;
