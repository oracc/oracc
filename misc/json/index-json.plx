#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my $ninst = 0;
my $nkeys = 0;

my ($project,$index) = @ARGV; @ARGV=();

my $mangletab = "$ENV{'ORACC_BUILDS'}/pub/$project/$index/mangle.tab";

print "{\n";

print "\t\"type\": \"index\",\n\t\"project\":\"$project\",\n\t\"name\": \"$index\",\n";

print "\t\"keys\": [";

while (<>) {
    if (/^key\s+(\S+)\s+has\s+(\d+)/) {
	my $k = $1;
	my $jkey = jsonify($k);
	if ($nkeys) {
	    print "\n\t\t]},{\n";
	} else {
	    print "{\n";
	    $nkeys = 1;
	}
	print "\t\t\"key\": \"$jkey\",\n\t\t\"count\": \"$2\",\n\t\t\"instances\": [\n\t\t\t";
	$ninst = 0;
    } else {
	print "," if $ninst;
	my($id) = (m/^\s+t=(.*?)\%/);
	if ($index =~ /^txt|lem|tra$/) {
	    my($u,$w) = (m/u=(\d+);w=(\d+);/);
	    if ($index eq 'tra') {
		print "\"${id}_project-en.$u.$w\"";
	    } else {
		print "\"$id.$u.$w\"";
	    }
	} else {
	    print "\"$1\"";
	}
	++$ninst;
    }
}
print "\n\t\t]}\n\t]" if $nkeys++;
my $mapentries = 0;
if (open(M, $mangletab)) {
    print ",\n\t\"map\": {\n";
    while (<M>) {
	print ",\n" if $mapentries++;
	chomp;
	my($from,$to) = split(/\t/,$_);
	my $jfrom = jsonify($from);
	my $jto = jsonify($to);
	print "\t\t\"$jfrom\": \"$jto\"";
    }
    print "\n\t}\n";
} else {
    print "\n";
}

print "}\n";

######################################################################

sub
jsonify {
    my $tmp = shift;
    $tmp =~ s/"/\000"/g;
    $tmp =~ s/\\/\000\\/g;
    $tmp =~ tr/\000/\\/;
    $tmp =~ tr/\x80-\x9f//d;
    $tmp =~ tr/\t\xa0/  /;
    $tmp;
}

1;
