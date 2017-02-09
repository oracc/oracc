#!/usr/bin/perl
use warnings; use strict;

my $mode = 'singles';

my $project = shift;
die "corpus-json.plx: must give project on command line\n"
    unless $project;

my $list = "$ENV{'ORACC_BUILDS'}/bld/$project/lists/lemindex.lst";
my $texts = 0;

my $dir = "$ENV{'ORACC_BUILDS'}/www/$project/corpusjson";
if (-d $dir) {
    system 'rm', '-fr', $dir;
}
system 'mkdir','-p',$dir;

if ($mode eq 'singles') {
    open(OUT,"| jq . >$ENV{'ORACC_BUILDS'}/www/$project/corpus.json");
    select(OUT);
}

print "{\n\"type\": \"corpus\",\n\"project\": \"$project\",\n\"members\": ";

if ($mode eq 'singles') {
    print "{\n";
} else {
    print "[\n";
}

if (-r $list) {
    my @texts = `cat $list`; chomp @texts;
    foreach my $t (@texts) {
	my ($project,$PQX) = split(/:/, $t);
	my ($four) = ($PQX =~ /^(....)/);
	my $xtf = "$ENV{'ORACC_BUILDS'}/bld/$project/$four/$PQX/$PQX.xtf";
	if (-r $xtf) {
	    print "," if $texts++;
	    if ($mode eq 'singles') {
		my $json = "$ENV{'ORACC_BUILDS'}/www/$project/corpusjson/$PQX.json";
		system("$ENV{'ORACC'}/bin/xcl-json.plx -s $t");
		$json =~ s/^.*?corpusjson/corpusjson/;
		print "\"$PQX\": \"$json\"";
	    } else {
		system "$ENV{'ORACC'}/bin/xcl-json.plx", $t;
	    }
	} else {
	    warn "corpus-json.plx: no such file $xtf\n"
	}

    }
}

if ($mode eq 'singles') {
    print "\n}\n}\n";
} else {
    print "\n]\n}\n";
}

1;
