package ORACC::Legacy::StyleInfo;
use warnings; use strict; use open 'utf8';
use lib "$ENV{'ORACC'}/lib";

my %styles = ();

sub
info4style {
    my ($style,$key) = @_;
    if ($styles{$style}) {
	return ${$styles{$style}}{$key};
    }
    undef;
}

sub
loadinfo {
    my $f = shift;
    open(S,"unodf.sh $f | xsltproc $ENV{'ORACC'}/lib/scripts/StyleInfo.xsl - |");
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

sub
showinfo{
    foreach (sort keys %styles) {
	my %info = %{$styles{$_}};
	print "$_";
	foreach my $k (keys %info) {
	    print "\t$k=$info{$k}";
	}
	print "\n";
    }
}

1;

