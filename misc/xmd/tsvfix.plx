#!/usr/bin/perl
use warnings; use strict;
use Text::CSV_XS;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::NS;
use Data::Dumper;

my @fields = ();
my %fields = ();
my $file;
my $id_text_index = -1;
my $period_index = -1;

$file = shift;
my @rows = ();
my $csv = Text::CSV_XS->new({ sep_char=>"\t" , auto_diag=>1 , allow_loose_quotes=>1 , 
			      quote_char=>'' , escape_char=>'' });

open my $fh, "<:encoding(utf8)", $file 
    or die "XMD::TSV: can't open CSV file '$file'\n";

my $xmd = $csv->getline_all($fh);
my @xmd = @$xmd;
my $fields_row = shift @xmd;

my @newfields = initialize_fields($fields_row);

print join("\t", @newfields), "\n";

foreach my $r (@xmd) {
    my @r = @$r;
    for (my $i = 0; $i <= $#fields; ++$i) {
	my $f = $fields[$i];
	if ($f eq 'id_text') {
	    print fixup_P_id($r[$i]);
	} elsif ($f eq 'period') {
	    $r[$i] =~ s/\s+\(.*$//;
	    print fixup_P_id($r[$i]);
	} elsif ($f =~ /internal_only/) {
	} else {
	    $r[$i] =~ s/\cK/ /g;
	    print $r[$i];
	}
	print "\t" if $i < $#fields;
    }
    print "\n";
}

##########################################################################

sub
fixup_P_id {
    my $pqx = shift;
    if ($pqx =~ /^\d/) {
	my $nid = '';
	$nid = 'P'.('0'x(6-length($pqx))).$pqx;
#	print STDERR "TSV.pm: repairing P-ID from $pqx to $nid\n";
	$pqx = $nid;
    }
    $pqx;
}

sub
initialize_fields {
    @fields = @{$_[0]};
    my @newfields = ();
    %fields = ();
    for (my $i = 0; $i <= $#fields; ++$i) {
	$period_index = $i if $fields[$i] eq 'period';
	$id_text_index = $i if $fields[$i] eq 'id_text';
	$fields{$fields[$i]} = $i;
	push @newfields, $fields[$i] unless $fields[$i] =~ /internal_only/;
    }
    @newfields;
}

1;
