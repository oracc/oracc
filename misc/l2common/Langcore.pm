package ORACC::L2GLO::Langcore;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/lang_uses_base/;

use warnings; use strict; use open 'utf8'; use utf8;

my %langcore = ();
my $loaded = 0;

sub lang_uses_base {
    my $lang = shift;
    $lang =~ s/-.*$//;
    load_langcore() unless $loaded;
    ${$langcore{$lang}}{'BASE'};
}

sub load_langcore {
    my $langcore_g = "$ENV{'ORACC_BUILDS'}/lib/data/lang_core.g";
    my $inlangs = 0;
    open(L,$langcore_g) || die "Langcore.pm: can't open $langcore_g\n";
    while (<L>) {
	if (/%%/) {
	    $inlangs = !$inlangs;
	} elsif ($inlangs) {
	    chomp;
	    my %l = ();
	    @l{qw/lang script logolang upper uscore enum feat/} = split(/,\s+/, $_);
	    $l{'script'} =~ s/^\"(.*?)\"/$1/;
	    $l{'logolang'} =~ s/^\"(.*?)\"/$1/;
	    foreach my $f (split(/\|/, $l{'feat'})) {
		$f =~ s/^LF_(.*?)$/$1/;
		$l{$f} = 1;
	    }
	    %{$langcore{$l{'lang'}}} = (%l);
	}
    }
    close(L);
#    use Data::Dumper;
#    print Dumper \%langcore;
}

1;
