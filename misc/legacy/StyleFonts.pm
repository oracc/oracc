package ORACC::Legacy::StyleFonts;
use warnings; use strict; use open 'utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;

my %styles = ();

sub
font4style {
    $stylefonts{$_[0]};
}

sub
showfonts {
    my %f = ();
    @f{values %stylefonts} = ();
    foreach (sort keys %f) {
	print "$_\n";
    }
}

sub
harvestfonts {
    my $f = shift;
    open(S,"xsltproc $ENV{'ORACC'}/lib/scripts/StyleInfo.xsl $f |");
    while (<S>) {
	my %s = ();
	chomp;
	my($style,@data) = split(/\t/, $_);
	foreach my $d (@data) {
	    my($key,$val) = ($d =~ /^(.*?)=(.*)$/);
	    $s{$key} = $val;
	}
	$styles{$style} = { %s };
    }
    close(S);
}

1;

