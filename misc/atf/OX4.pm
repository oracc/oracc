package ORACC::ATF::OX4;
use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::L2GLO::Util;

#
# Module for working with output from the ATF processor when invoked as 'ox -4 <ATF_FILES>
#
# Output gives the instance info in col1 and the signature in col1 so it's an easy way to
# access what the lemmatizer is getting and what it's doing with it.
#
# 00atf/K_04603_l_t.atf:30:%sux-x-emesal:te-am₃=ta[what?]	@dcclt/nineveh%sux-x-emesal:te-am₃=ta[what?//what?]QP'QP$ta,am/te#~,am
# 00atf/K_04603_l_t.atf:30:%sux:e-ne-am₃=anam[what]	@dcclt/nineveh%sux:e-ne-am₃=anam[what//what]N'N$anam/e-ne-am₃#~
# 00atf/K_04603_l_t.atf:30:%akk-x-stdbab:mi-nu=mīnu[what]QP	@dcclt/nineveh%akk-x-stdbab:mi-nu=mīnu[what//what]QP'QP$mīnu
#
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/ox4_init ox4_next/;

use warnings; use strict; use Encode; use utf8;

my $ox4fh = undef;
my $ox4loc = ''; # this tracks file/line
my $ox4tok = 0; # index of token in curr file/line
my $ox4lem_only = 0; # 1 if we are only returning the useful bits
my $ox4sig_hash = 0; # 1 if we are parsing the sig into a hash
my %ox4_select = ();

# Pass this a file handle to read input from
sub ox4_init {
    ($ox4fh,$ox4lem_only,$ox4sig_hash) = @_;
}

sub ox4_next {
  again:
    my $next = <$ox4fh>;
    return (undef,undef,undef) unless $next;
    my %sig = ();
    my($fl) = ($next =~ /^(.*?:\d+):/);
    if ($fl ne $ox4loc) {
	$ox4loc = $fl;
	$ox4tok = -1;
    }
    ++$ox4tok;
    if ($next) {
	if ($ox4lem_only && $next !~ /\t.*?\[/) {
	    goto again;
	}
    }
    if ($ox4sig_hash) {
	my($l,$r) = split(/\t/,$next);
	chomp $r;
	%sig = parse_sig($r);
    }
    ($next,$ox4tok,\%sig);
}

1;
