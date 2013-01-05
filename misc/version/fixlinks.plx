#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
my $oracc = $ENV{'ORACC'};
my($from,$to) = @ARGV;

die "fixlinks.plx: must give from-project and to-project\n"
    unless $from && $to;

system 'mkdir', '-p', '00web/new';
my @html = <00web/*.html>;
foreach my $h (@html) {
    open(H,$h);
    local($/) = undef;
    my $hfile = <H>;
    close(H);
    $hfile =~ s#(href="[^"]*|location='[^']*)$from(?=["/])#$1$to#og;
    $h =~ s#^00web/##;
    open(H,">00web/new/$h") || die "fixlinks.plx: can't open '00web/new/$h' for write\n";
    print H $hfile;
    close(H);
}

system "(cd 00web ; mkdir -p old ; mv *.html old ; mv new/*.html . ; rmdir new)";

1;
