#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use lib '/usr/local/oracc/lib';
use ORACC::Expand2;
use ORACC::XML;
my $PQ = '';
my $project = `oraccopt`;
die unless $project;
while (<>) {
    chomp;
    $PQ = $_;
    my $txt = expand_in_project(undef,"$PQ.psd", $project);
    my $html = $txt;
    $html =~ s/psd$/html/;
    wraptxt($txt,$html);
}

exec '/usr/local/oracc/bin/pubfiles.sh', $project;

############################################################################

sub
strip_lemma_and_code {
    chomp;
    s/\(LEMMA .*?\)//;
    s/\(CODE .*?\)//;
    s/\s+\)/)/;
    s/\s*$/\n/;
    $_;
}

sub
wraptxt {
    my($t,$h) = @_;
    open(H,">$h") 
	|| (warn "ptb2html.plx: can't open $h for output\n" and return);
    open(T,$t) 
	|| (warn "ptb2html.plx: can't open $t for input\n" and return);
    print H <<EOH;
<html><head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
</head>
<body onload="pcsepsd('$PQ')">
<pre class="pcs">
EOH
    print H xmlify(strip_lemma_and_code($_)) while (<T>);
    print H <<EOH;
</pre></body></html>
EOH
}
