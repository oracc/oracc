#!/usr/bin/perl
use warnings; use strict;
use open 'utf8'; binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my $project = `oraccopt`;
#my $list = "$ENV{'ORACC_BUILDS'}/bld/$project/lists/lemindex.lst";
my $list = "$ENV{'ORACC_BUILDS'}/bld/$project/lists/have-xtf.lst";
my $proxies = "$ENV{'ORACC_BUILDS'}/bld/$project/lists/proxy-atf.lst";
my $texts = 0;
my $has_corpus = 0;

my $bldjson = "$ENV{'ORACC_BUILDS'}/$project/01bld/json";

my $dir = "$bldjson/corpusjson";
if (-d $dir) {
    system 'rm', '-fr', $dir;
}
system 'mkdir','-p',$dir;

warn "Creating corpus json for $project\n";

open(OUT,">$bldjson/corpus.json");
print OUT "{\n\"type\": \"corpus\",\n\"project\": \"$project\"";

if (-r $list) {
    my @texts = `cat $list`; chomp @texts;
    if ($#texts > 0){
	print OUT ",\n\"members\": {\n";
	$has_corpus = 1;
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
	print OUT "}\n";
    }
}

if (-r $proxies) {
    my @texts = `cat $proxies`; chomp @texts;
    if ($#texts >= 0) {
	my $nprox = 0;
	$has_corpus = 1;
	print OUT ",\n\"proxies\": {\n";
	foreach my $t (@texts) {
	    my ($project,$PQX) = split(/:/, $t);
	    $PQX =~ s/\@.*$//;
	    print OUT "," if $nprox++;
	    print OUT "\"$PQX\": \"$project\"";
	}
	print OUT "}\n";
    }
}

print OUT "\n}\n";
close(OUT);

unlink "$bldjson/corpus.json"
    unless $has_corpus;

warn "\n";

1;
