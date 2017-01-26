#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my $ninst = 0;
my $nkeys = 0;

my ($project,$index) = @ARGV; @ARGV=();

print "{\n";

print "\t\"type\": \"glossary\",\n\t\"project\":\"$project\",\n\t\"index\": \"$index\",\n";

print "\t\"keys\": [";

while (<>) {
    if (/^key\s+(\S+)\s+has\s+(\d+)/) {
	if ($nkeys) {
	    print "\n\t\t]},{\n";
	} else {
	    print "{\n";
	    $nkeys = 1;
	}
	print "\t\t\"key\": \"$1\",\n\t\t\"count\": \"$2\",\n\t\t\"instances\": [\n\t\t\t";
	$ninst = 0;
    } else {
	print "," if $ninst;
	m/^\s+t=(.*?)\%/;
	print "\"$1\"";
	++$ninst;
    }
}
print "\n\t\t]}\n\t]\n" if $nkeys++;
print "}\n";

1;
