#!/usr/bin/perl
use warnings; use strict;
use Getopt::Long;

my $dl = 0;
my $index_files = 0;
my $output_file;
my $stdin = 0;
GetOptions(
    'output:s'=>\$output_file,
    'stdin'=>\$stdin
    );

my $index_base = '';
if ($output_file) {
    $index_base = $output_file;
    $index_base =~ s/\.xml//;
} else {
    $index_base = 'index';
    $output_file = 'index.xml';
}
open(OUT, ">$output_file");
select OUT;

my @xdf = ();

if ($stdin) {
    @xdf = (<>);
    chomp @xdf;
} else {
    @xdf = @ARGV;
    @xdf = <*/*.xdf> unless $#xdf >= 0;
}

print '<xdf xmlns:xi="http://www.w3.org/2001/XInclude">';
foreach my $a (@xdf) {
    next unless length $a;
    next if $a =~ /^\#/;
    if ($a =~ m/^\@/) {
	subpage($a);
    } elsif ($a =~ s/^\*\s+//) {
	if ($dl) {
	    print "</dl>";
	    $dl = 0;
	}
	print "<index-h>$a</index-h>";
    } else {
	if (!$dl) {
	    print "<dl>";
	    $dl = 1;
	}
	if ($a =~ /^(.*?)\#(.*?)$/) {
	    print "<xi:include href=\"$1\" xpointer=\"$2\"/>";
	} else {
	    print "<xi:include href=\"$a\"/>";
	}
    }
}
print "</dl>" if $dl;
print '</xdf>';

sub
subpage {
    my $line = shift;
    chomp $line;
    my $html_file = '';
    my $index_file = '';
    my $order_file = '';
    $line =~ s/^\@(\S+)\s+//;
    $order_file = $1;
    $index_file = sprintf("$index_base%02d.xml", $index_files++);
    $html_file = $index_file;
    $html_file =~ s/xml$/html/;
    print "<index-sub href=\"$html_file\">$line</index-sub>";
    system "../00tools/mk-index.plx -o $index_file -s <$order_file";
}

1;
