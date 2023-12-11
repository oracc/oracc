package ORACC::CBD::ATF;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/atf_add atf_check atf_reset cpd_add cpd_check/;

use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use Data::Dumper;
use ORACC::CBD::PPWarn;
use ORACC::Legacy::Sexify;

my %atf = ();
my %cpd = ();

sub atf_add {
    my ($a,$l) = @_;
#    warn "atf_add: $a\n";
    if ($a) {
	my @a = split(/\s+/, $a);
	my @aa = ();
	foreach my $aa (@a) {
	    if ($aa eq '0') {
		# skip
	    } elsif ($aa =~ /(^|[-} ])([0-9]+)(-|$)/) {
		my $z = $1 || '';
		my $x = $2;
		my $y = $3 || '';
		$x = ORACC::Legacy::Sexify::sexify($x,0,1,0);
		$aa =~ s/((?:^|[-} ])[0-9]+(?:-|$))/$z$x$y/;
		push @aa, $aa;
	    } else {
		push @aa, $aa;
	    }
	}
	$a = join(' ', @aa);
	$a =~ tr/·°//d;
	$a =~ s/\{-/{/g;
	$a =~ s/\\.*$//; # remove disambiguators for checking
	my $add = $l ? "\%$l $a" : $a;
#	warn pp_line().": $add\n";
#	warn "atf_add: actually adding $add\n";
	push @{$atf{pp_line()}}, $add;
    } else {
	pp_warn("internal error: empty value passed to atf_add")
    }
}

sub atf_reset {
    %atf = ();
    %cpd = ();
}

sub atf_check {
    my($proj,$lang) = @_;

    return unless scalar keys %atf;

    my $compound_K = ($ORACC::CBD::check_compounds ? 'K' : '');
    
    open(ATF,">01tmp/$lang.atf");
    print ATF "\&X999999 = Gloss ATF\n#project: $proj\n#atf: use unicode\n#atf: use math\n";
    print ATF "#atf: lang $lang\n" unless $lang eq 'qpn';
    my @atf = map { "$_. @{$atf{$_}}" } sort { $a <=> $b } keys %atf;
    print ATF uniq_by_line(@atf);
    close(ATF);
    system 'oxx', '-c6Q'.$compound_K, '-l', "01tmp/$lang-atf.log", "01tmp/$lang.atf";

    if (open(OX,"01tmp/$lang-atf.log") || die "cbdpp.plx: can't open 01tmp/$lang-atf.log\n") {
	my $save = pp_line();
	while (<OX>) {
	    if (/^(\d+):/) {
		my $cat = non_status_error($_) ? 'warning' : 'error';
		s#^(.*?):\s+#(atf $cat) #;
		my $ln = $1;
		pp_line($1);
		chomp; s/ To request adding it please visit://;
		if ($cat eq 'warning') {
		    pp_notice $_;
		} else {
		    pp_warn $_;
		}
	    }
	}
	close(OX);
	pp_line($save);
    }
}

sub cpd_add {
    my $a = shift;
    if ($a) {
	$a =~ tr/·°//d;
	$a =~ s/\{-/{/g;
	push @{$cpd{pp_line()}}, $a;
    } else {
	pp_warn("internal error: empty value passed to cpd_add")
    }
}

sub cpd_check {
    my($proj,$lang,$err_file) = @_;

    my $pedantic = 0; # no longer necessary
#    my $pedantic = ORACC::SL::BaseC::pedantic();
#    warn "called cpd_check with ORACC::SL::BaseC::Pedantic = $pedantic\n";
#    warn Dumper \%cpd;
    
    return unless scalar keys %cpd;

    my $save = pp_line();
    open(CPD,">01tmp/cpd-$lang.atf");
    print CPD "\&X999999 = Gloss CPD\n#project: $proj\n#atf: use unicode\n";
    print CPD "#atf: lang $lang\n" unless $lang eq 'qpn';
    my @cpd = map { "$_. @{$cpd{$_}}" } sort { $a <=> $b } keys %cpd;
    print CPD uniq_by_line(@cpd);
    close(CPD);
    system 'oxx', '-f', '-x=', '-l', "01tmp/$lang-cpd.log", "01tmp/cpd-$lang.atf";
    system 'xsltproc', '-o', "01tmp/cpd-$lang.tab", "$ENV{'ORACC_BUILDS'}/lib/scripts/sl-compounds.xsl",
	"01tmp/cpd-$lang.xml";
    my @cpd_err = `cat 01tmp/cpd-$lang.tab`; 
    chomp @cpd_err;
    foreach my $c (@cpd_err) {
	my($ln,$in,$ret) = split(/\t/,$c);
	pp_file($err_file);
	pp_line($ln);
	if ($ret eq 'OK') {
#	    warn "compound $in OK\n";
	} elsif ($ret =~ /^OK=(.*?)$/) {
	    pp_warn("(bases) compound $in should be $1") if $pedantic;
	} elsif ($ret =~ /^OK~(.*?)\s*$/) {
	    pp_warn("(bases) compound $in should be $1") if $pedantic;
	} elsif ($ret =~ /^NO:(.*?)\s*$/) {
	    my $m = $1;
	    pp_warn("(bases) compound $in (mapped to $m) not in signlist") unless $m =~ /X/;
	} elsif ($ret && length $ret) {
	    pp_warn("(bases) compound return code $ret unknown");
	}
    }
    pp_line($save);
}

sub
non_status_error {
    /unknown sign-name/ || /cuneify/;
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
#	warn "uniq_by_line: $rest\n";
	push @new_l, $lnum . "\t$lang" . join(' ', keys %u), "\n";
    }
    @new_l;
}

1;
