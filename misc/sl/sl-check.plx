#!/usr/bin/perl
use warnings; use strict; use open ':utf8'; use utf8;
binmode STDERR, ':utf8'; binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::SL::BaseC;
use ORACC::CBD::PPWarn;
use Getopt::Long;

my $errfile = '';
my $nametab = 0; # use -n to indicate name tab processing
my $output = 0; # use -o to generate a table of input forms and OID signatures
my $sigs = 0; # look for forms in signatures i.e., %sux:a-na=
my $nonum = 0; # don't report errors in number graphemes

GetOptions(
    'e:s'=>\$errfile,
    'n'=>\$nametab,
    'o'=>\$output,
    's'=>\$sigs,
    'u'=>\$nonum,
);

my %cpd = ();
my %known_bad = ();
my %seen = ();

ORACC::SL::BaseC::init();
ORACC::SL::BaseC::pedantic();
$ORACC::SL::report_all = 1;

my $f = '';
if ($errfile) {
    $f = $errfile;
} else {
    $f = shift @ARGV;
}
my $lnum = 0;

$ORACC::CBD::PPWarn::err_file = $f;
pp_file($f);

while (<>) {
    chomp;
    if ($sigs && /^(.*?):(\d+):\s/) {
	pp_file($1);
	pp_line($2);
    } else {
	pp_line(++$lnum);
    }
    my @todo = make_todo_list($_);
    foreach my $t (@todo) {
	next if $t =~ /^\d/;
	if ($known_bad{$t}) {
	    foreach my $m (@${$known_bad{$t}}) {
		pp_warn($m);
	    }
	}
	cpd_todo($t);
	next if $seen{$t}++;
	my $s = ORACC::SL::BaseC::tlit_sig('',$t);
	my @m = ORACC::SL::BaseC::messages();
	if ($#m >= 0) {
	    foreach my $m (@m) {
		pp_warn($m);
	    }
	    ${$known_bad{$t}} = [ @m ];
	}
	print "$t\t$s\n" if $output;
    }
}
ORACC::SL::BaseC::term();

my %args = ();
my $proj;

if (-r '02xml/config.xml') {
    $proj = `oraccopt`;
} else {
    $proj = 'epsd2';
}
cpd_check($proj,'sux');
pp_diagnostics(\%args);

########################################################################################################

sub make_todo_list {
    my $line = shift;
    if ($nametab) {	
	$line =~ /^(?:.*?:)?(\S+)/;
	my $todo = $1;
	return ($todo);
    } elsif ($sigs) {
	$line =~ /(?:sux.*?:)(\S+?)=/;
	my $todo = $1;
	return ($todo) if $todo;
    }
    ()
}
sub cpd_todo {
    my $cpd = $_[0];
    $cpd =~ s/^[^|]//;
    $cpd =~ s/[^|]+$//;
    #	print STDERR "cpd $cpd => ";
    my @cpd = ($cpd =~ m/(\|.*?\|)/g);
    #	print STDERR "@cpd\n";
    foreach my $c (@cpd) {
	cpd_add($c,pp_line());
    }
}

sub cpd_add {
    my ($a,$ln) = @_;
    if ($a) {
	$a =~ tr/·°//d;
	push @{$cpd{$ln}}, $a;
    } else {
	warn("$0: internal error: empty value passed to cpd_add\n");
    }
}

sub cpd_check {
    my($proj,$lang) = @_;

    return unless scalar keys %cpd;

    open(CPD,">01tmp/cpd-$lang.atf");
    print CPD "\&X999999 = Gloss CPD\n#project: $proj\n#atf: use unicode\n";
    print CPD "#atf: lang $lang\n" unless $lang eq 'qpn';
    my @cpd = map { "$_. @{$cpd{$_}}" } sort { $a <=> $b } keys %cpd;
    print CPD uniq_by_line(@cpd);
    close(CPD);
    system 'ox', '-f', '-x=', '-l', "01tmp/$lang-cpd.log", "01tmp/cpd-$lang.atf";
    system 'xsltproc', '-o', "01tmp/cpd-$lang.tab", "$ENV{'ORACC_BUILDS'}/lib/scripts/sl-compounds.xsl",
	"01tmp/cpd-$lang.xml";
    my @cpd_err = `cat 01tmp/cpd-$lang.tab`; chomp @cpd_err;
    foreach my $c (@cpd_err) {
	my($ln,$in,$ret) = split(/\t/,$c);
	pp_line($ln);
	if ($ret eq 'OK') {
#	    warn "compound $in OK\n";
	} elsif ($ret =~ /^OK=(.*?)$/) {	    
	    pp_warn("(bases) compound $in should be $1");
	} elsif ($ret =~ /^OK~(.*?)\s*$/) {
	    pp_warn("(bases) compound $in should be $1");
	} elsif ($ret =~ /^NO:(.*?)\s*$/) {
	    my $m = $1;
	    pp_warn("(bases) compound $in (mapped to $m) not in signlist") unless $m =~ /X/;
	} elsif ($ret && length $ret) {
	    pp_warn("(bases) compound return code $ret unknown");
	}
    }
}

sub
uniq_by_line {
    my @l = @_;
    my @new_l = ();
    foreach my $l (@l) {
	my $lang = '';
	$lang = "$1 " if $l =~ s/(\%[a-z]\S+)\s+//;
	my($lnum,$rest) = ($l =~ /^(\S+)\s+(.*)\s*$/);
	my %u = ();
	@u{split(/\s+/,$rest)} = ();
	push @new_l, $lnum . "\t$lang" . join(' ', keys %u), "\n";
    }
    @new_l;
}

1;
