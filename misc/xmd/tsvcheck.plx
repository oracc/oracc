#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDERR, ':utf8';

my $tsv = shift @ARGV;
die "$0: must give .tsv file name on commandline\n" unless $tsv;
die "$0: $tsv: file not readable\n" unless -r $tsv;

undef $/;

my $fields = `head -1 $tsv`; chomp $fields; $fields =~ tr/\cM//d;
my $nftab = ($fields =~ tr/\t/\t/);
my @fields = split(/\t/, $fields);
my $nfields = $#fields;
my $bad_line = 0;
my $curr_id = -1;
my $id = -1;
my $ln = 0;
my @tsv = ();
my $status = 0;

for (my $i = 0; $i <= $#fields; ++$i) {
    $fields[$i] =~ s/\s*$//;
    if ($fields[$i] eq 'id_text' || $fields[$i] eq 'id_composite') {
	$id = $i;
    }
    if ($fields[$i] =~ /\s/) {
	warn "$tsv:1: '$fields[$i]': spaces not allowed in field names\n";
    }
    if ($fields[$i] =~ /:/) {
	warn "$tsv:1: '$fields[$i]': colon not allowed in field names\n";
    }
}
die "$0: $tsv: no id_text or id_composite found in field names\n" unless $id >= 0;

# warn "$0: tsv=$tsv; lastfield=$nfields (nftab=$nftab); ID field=$id\n";

system 'cp', $tsv, "$tsv.bak";

open(TSV,$tsv);
my $tsvlines = <TSV>;
close(TSV);

if ($tsvlines =~ /\r\n/) {
    $tsvlines =~ tr/\r//d;
}

process($tsvlines);

exit $status;

################################################################################

sub process {
    local($_) = shift;
    $|++;
    my @lines = split(/\n/, $_);
    shift @lines; # remove fields
    for (my $i = 0; $i <= $#lines; ++$i) {
	$ln = $i+1;
	my $ntab = ($lines[$i] =~ tr/\t/\t/);
	warn "$tsv:$ln: wrong field count for text $curr_id: should be $nftab found $ntab\n" unless $nftab == $ntab;
	my @f = split(/\t/,$lines[$i]);
	$curr_id = $f[$id];
	$bad_line = 0;
	for (my $j = 0; $j <= $#f; ++$j) {
	    $f[$j] = check_field($f[$j],$fields[$j]);
	}
	++$status if $bad_line;
    }
}

sub check_field {
    my ($t,$fld) = @_;
    my $nctk = ($t =~ tr/\cA\cB\cK/   /);
    if ($nctk) {
	warn "$tsv:$ln; bad control characters in record $curr_id, field '$fld'\n";
    }
    my $bad = ($t =~ tr/"/"/ %2);
    if ($bad) {
	warn "$tsv:$ln: mismatched quotes in record $curr_id, field '$fld'\n";
	++$bad_line;
    }
    $t;
}

1;
