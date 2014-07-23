#!/usr/bin/perl
use warnings; use strict; use utf8;
binmode STDOUT, ':utf8';

my %kb = (
    'š' => [ ',c' , '' ],
    'Š' => [ ',c' , '' ],
    'ŋ' => [ ',j' , '' ],
    'Ŋ' => [ ',J' , '' ],
    'ḫ' => [ ',h' , '' ],
    'Ḫ' => [ ',H' , '' ],
    'ṣ' => [ ',s' , '' ],
    'Ṣ' => [ ',S' , '' ],
    'ṭ' => [ ',t' , '' ],
    'Ṭ' => [ ',T' , '' ],
    'ś' => [ 'S,s', '' ],
    'Ś' => [ 'S,S', '' ],
    '₀' => [ ',0' , '' ],
    '₁' => [ ',1' , '' ],
    '₂' => [ ',2' , '' ],
    '₃' => [ ',3' , '' ],
    '₄' => [ ',4' , '' ],
    '₅' => [ ',5' , '' ],
    '₆' => [ ',6' , '' ],
    '₇' => [ ',7' , '' ],
    '₈' => [ ',8' , '' ],
    '₉' => [ ',9' , '' ],
    'ₓ' => [ ',x' , '' ],
    '×' => [ ',X' , '' ],
    'ʾ' => [ ',)' , '' ],
    'ʿ' => [ ',(' , '' ],
    '⸢' => [ ',[' , '' ],
    '⸣' => [ ',]' , '' ],
    'ʳ' => [ ',r' , '' ],
    );

my %names = ();
open(N,"@@ORACC@@/lib/data/NamesList.txt") || die;
while (<N>) {
    if (/^(\S+)\s+(.*?)$/) {
	chomp;
	my($u,$n) = ($1,$2);
	$n =~ s/\s+\(Sami\)\s*//;
	$names{$u} = $n;
    }
}
close(N);

my $chars = <<EOC;
áéíúÁÉÍÚśŚ
àèìùÀÈÌÙ
āēīūĀĒĪŪ
âêîûÂÊÎÛ
šŠ
ṣṢṭṬ
ḫḪ
ŋŊ
×
ʾ
ʿ
₀₁₂₃₄₅₆₇₈₉ₓ
⸢⸣⸤⸥
ʳ
EOC

$chars =~ tr/\n//d;

print <<EOT;
<?xml version="1.0" encoding="utf-8"?>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
<title>Unicode for Cuneiform Transliteration</title>
<link rel="stylesheet" type="text/css" href="/cdlsite.css"/>
<link rel="stylesheet" type="text/css" href="/xdf.css"/>
<script src="/xdf.js" type="text/javascript"> </script>
</head>
<body>
<div class="sanstoc">
<div class="body">

<h1>Unicode Characters for Cuneiform Transliteration</h1>

<p>This page lists the essential Unicode characters for
transliterating Sumero-Akkadian cuneiform.  Several families of fonts
containing all of these characters are available from <a
href="/fonts.html">the fonts page</a>.

<table>
<thead><tr><th class="center ungkam">LITERAL</th><th>U+</th><th>UNICODE NAME</th><th>KEYS<sup>1</sup></th></tr></thead>
EOT
foreach my $c (split(/(.)/,$chars)) {
    if ($c) {
	my $u = sprintf "%04X", ord $c;
	my $k = $kb{$c};
	my $kbtd = '';
	if ($k && $$k[0]) {
	    $kbtd = "<td class=\"center\">$$k[0]</td>";
	}
	print "<tr class=\"vcenter\"><td class=\"center ungkam\">$c</td><td>$u</td><td class=\"fixed\">$names{$u}</td>$kbtd</tr>\n";
    }
}
print <<EOT;
</table>
</div>
<p><sup>1</sup>A collection of keyboards supporting Unicode transliterations
implements a consistent set of input conventions across operating
systems.  The keystrokes in the 'KEYS' column above are composed of
the comma character (<code>,</code>) or the shifted comma character
(<code>S,</code>).  In each case the comma or shift-comma is followed
by the character typed to produce a unicode result.</p>
<p>For complete details please see <a href="keyboards.html">the keyboards
page</a>.</p>
</div>
</body>
</html>
EOT

1;
