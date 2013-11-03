package ORACC::XMD::TSV;
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

##########################################################################

sub
clean_periods {
    my $xmd = shift;
    for (my $i = 0; $i <= $#$xmd; ++$i) {
	$$xmd[$i][$period_index] =~ s/\s+\([^\(]+\)?\s*$//;
    }
}
sub
clean_id_text {
    my $xmd = shift;
    for (my $i = 0; $i <= $#$xmd; ++$i) {
	$$xmd[$i][$id_text_index] = fixup_P_id($$xmd[$i][$id_text_index]);
    }
}
sub
fixup_P_id {
    my $pqx = shift;
    if ($pqx =~ /^\d/) {
	my $nid = $1;
	$nid = 'P'.('0'x(6-length($nid))).$nid;
#	print STDERR "TSV.pm: repairing P-ID from $pqx to $nid\n";
	$pqx = $nid;
    }
    $pqx;
}
sub
internalize {
    $file = shift;
    my @rows = ();
    my $csv = Text::CSV_XS->new({ sep_char=>"\t" , auto_diag=>1 , allow_loose_quotes=>1 , 
				  quote_char=>'' , escape_char=>'' });

    open my $fh, "<:encoding(utf8)", $file 
	or die "XMD::TSV: can't open CSV file '$file'\n";

    my $xmd = $csv->getline_all($fh);
    my @xmd = @$xmd;
    my $fields_row = shift @xmd;
    $xmd = [ @xmd ];

    initialize_fields($fields_row);

    if ($id_text_index >= 0) {
	clean_id_text($xmd);
    } else {
	warn "ORACC::XMD::TSV: no id_text_index found in $file\n";
    }
    if ($period_index >= 0) {
	clean_periods($xmd);
    } else {
	warn "ORACC::XMD::TSV: no period_index found in $file\n";
    }

    ([@fields],{%fields},$xmd);
}

sub
initialize_fields {
    @fields = @{$_[0]};
    %fields = ();
    for (my $i = 0; $i <= $#fields; ++$i) {
	$period_index = $i if $fields[$i] eq 'period';
	$id_text_index = $i if $fields[$i] eq 'id_text';
	$fields{$fields[$i]} = $i;
    }
}

1;
