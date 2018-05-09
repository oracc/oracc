package ORACC::CBD::ATF;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/atf_add atf_check/;

use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use ORACC::CBD::PPWarn;

my %atf = ();

sub atf_add {
    my $a = shift;
    if ($a) {
	$a =~ tr/·°//d;
	push @{$atf{pp_line()}}, $a;
    } else {
	pp_warn("internal error: empty value passed to atf_add")
    }
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
    system 'ox', '-cQ'.$compound_K, '-l', "01tmp/$lang-atf.log", "01tmp/$lang.atf";

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
	push @new_l, $lnum . "\t$lang" . join(' ', keys %u), "\n";
    }
    @new_l;
}

1;
