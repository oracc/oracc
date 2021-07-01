#!/usr/bin/perl
use warnings; use strict;

use Data::Dumper;

my @nn = qw/n AN CN DN EN FN GN LN MN ON PN QN RN SN TN WN YN/;

my @nn_long = qw/number Agricultural Celestial Divine Ethnos Field
    Geographical Lineage Month Object Personal Quarter Royal Temple
    Watercourse Year/;

my %nn_colours = ();
my %nn_long = (); @nn_long{@nn} = @nn_long;

open(N,'colour_nn.css') || die "$0: can't open colour_nn.css\n";
while (<N>) {
    if (/^\.(n|(?:[A-Z]N))/) {
	my $nn = $1;
	/color:\s*(\S+)/;
	$nn_colours{$nn} = $1;
    }
}
close(N);

open(H,'>colour_nn.html'); select H;
print <<EOF;
<!doctype html>
<html><head><title>Oracc NN Colourization</title></head>
<body><table style="width: 400px; margin:auto;">
<h2 style="text-align:center">Oracc Colours for Proper Nouns</h2>

<tr><td><b>Abbreviation</b></td><td><b>Meaning</b></td><td><b>HTML Colour</b></td></tr>
EOF
    
foreach my $n (sort keys %nn_colours) {
    my $Name = '';
    $Name = ' Name' if $n =~ /^[A-Z]/;
    print "<tr><td>$n</td><td>$nn_long{$n}$Name</td><td style=\"color:$nn_colours{$n}\">$nn_colours{$n}</td></tr>\n"
}
print "<tr><td colspan=\"2\">Emesal Sumerian</td><td style=\"color:mediumorchid\">mediumorchid</td></tr>\n";
print '</table></body></html>';
close(H);

1;
