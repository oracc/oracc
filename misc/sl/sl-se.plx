#!/usr/bin/perl
use warnings; use strict; use open ':utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::SL::Base;
use Getopt::Long;

my $how = 'ids'; # 'tra'=transliteration, 'cun'=cuneiform, 'htm'=html
my $what = 'sign';

my @what = qw/values sign homophones list compounds contains contained cinit clast forms uchar ucode _sig/;
my %what; @what{@what} = ();

my @uc_ext = qw/c cinit clast contains contained m multi/;
my %uc_ext;
@uc_ext{@uc_ext} = ();

GetOptions(
    'how:s'=>\$how,
    'what:s'=>\$what,
);

my @args = @ARGV;

ORACC::SL::Base::init();
if ($#args >= 0) {
    foreach my $arg (@args) {
	my @res = (lookup($arg));
	if ($#res >= 0) {
	    display(@res);
	} else {
	    display("#no $what match for $arg");
	}
    }
} else {
    while (<>) {
	chomp;
	my @res = (lookup($_));
	if ($#res >= 0) {
	    display(@res);
	} else {
	    display("#no $what match for $_");
	}
    }
}
ORACC::SL::Base::term();

##############################################################

sub
display {
    my @disp = @_;
    if (@disp >= 0 && $disp[0] && $disp[0] =~ /^x\d\d\d\d/) {
	@disp = map { ORACC::SL::Base::sign_of($_); } split('\s+', join(' ', @disp));
    }
    if ($#disp < 0 || !$disp[0]) {
	print '#no results for search',"\n";
    } elsif ($disp[0] =~ /^\#/) {
	print $disp[0], "\n";
    } else {
	print join("\n", split(/ +/, join(' ', @disp))), "\n";
    }
#    print join("\n", @disp), "\n";
}

sub
lookup {
    my $x = shift;

    if (exists($uc_ext{$what})) {
	$x =~ tr/a-zšŋ/A-ZŠŊ/;
    }
    my @r = ();

    if ($what eq 'sign') {

	my $r = ORACC::SL::Base::xid($x);
	@r = ($r) if $r;

    } elsif ($what eq 'homophones') {

	@r = ORACC::SL::Base::homophones($x);

    } elsif ($what eq 'forms') {

	@r = ORACC::SL::Base::forms($x);

    } elsif ($what eq 'values') {

	@r = ORACC::SL::Base::values($x);

    } elsif ($what eq 'list') {

	@r = ORACC::SL::Base::list($x);

    } elsif ($what eq '_sig') {

	@r = (ORACC::SL::Base::_signature('',$x));

    } elsif ($what eq 'compounds') {
	
	

    } elsif ($what eq 'contains') {
	
	

    } elsif ($what eq 'contained') {
	
	

    } elsif ($what eq 'cinit') {
	
	@r = ORACC::SL::Base::cinit($x);

    } elsif ($what eq 'clast') {
	
	

    } else {
	die "sl-se.plx: bad 'what' argument: `$what' unknown\n";
    }
    @r;
}

1;
