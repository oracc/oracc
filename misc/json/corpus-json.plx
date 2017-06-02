#!/usr/bin/perl
use warnings; use strict;
use open 'utf8'; binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my $project = `oraccopt`;
#my $list = "$ENV{'ORACC_BUILDS'}/bld/$project/lists/lemindex.lst";
my $list = "$ENV{'ORACC_BUILDS'}/bld/$project/lists/xtfindex.lst";
my $texts = 0;

my $bldjson = "$ENV{'ORACC_BUILDS'}/$project/01bld/json";

my $dir = "$bldjson/corpusjson";
if (-d $dir) {
    system 'rm', '-fr', $dir;
}
system 'mkdir','-p',$dir;

warn "Creating corpus json for $project\n";

open(OUT,">$bldjson/corpus.json");
print OUT "{\n\"type\": \"corpus\",\n\"project\": \"$project\",\n\"members\": {\n";

if (-r $list) {
    my @texts = `cat $list`; chomp @texts;
    foreach my $t (@texts) {
	my ($project,$PQX) = split(/:/, $t);
	my ($four) = ($PQX =~ /^(....)/);
	my $xtf = "$ENV{'ORACC_BUILDS'}/bld/$project/$four/$PQX/$PQX.xtf";
	if (-r $xtf) {
	    print OUT "," if $texts++;
	    my $json = "corpusjson/$PQX.json";
	    print OUT "\"$PQX\": \"$json\"";
	    system("$ENV{'ORACC'}/bin/xcl-json.plx -s $t $dir/$PQX.json");
	} else {
	    warn "corpus-json.plx: no such file $xtf\n"
	}

    }
}

print OUT "\n}\n}\n";
close(OUT);

warn "\n";

1;
