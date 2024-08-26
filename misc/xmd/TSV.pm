package ORACC::XMD::TSV;
use warnings; use strict;
use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

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
my $xmdline = 0;

my $id_type = 'id_text';
my $last_id = '';

##########################################################################

sub
clean_periods {
    my $xmd = shift;
    for (my $i = 0; $i <= $#$xmd; ++$i) {
	
	$$xmd[$i][$period_index] =~ s/\s+\([^\(]+\)?\s*$//
					   if $$xmd[$i][$period_index];
    }
}
sub
clean_id_text {
    my $xmd = shift;
    my @new = ();
    $xmdline = 1;
    for (my $i = 0; $i <= $#$xmd; ++$i) {
	++$xmdline;
	if ($$xmd[$i][$id_text_index]) {
	    $$xmd[$i][$id_text_index] = fixup_P_id($$xmd[$i][$id_text_index]);
	    push @new, $$xmd[$i];
	} # silently delete records with empty id_text
    }
    $xmd = [ @new ];
}
sub
fixup_P_id {
    my $pqx = shift;
    if ($pqx !~ /^[PQX]\d{6}$/) {
	my $nid = '';
	if ($pqx !~ /^\d+$/) {
	    warn "TSV.pm: invalid $id_type '$pqx' at input line $xmdline; last valid id was $last_id\n";
	} elsif ($pqx) {
	    $last_id = $pqx;
	    $nid = 'P'.('0'x(6-length($pqx))).$pqx;
	    # warn "TSV.pm: repairing P-ID from $pqx to $nid\n";
	    $pqx = $nid;
	}
    }
    $pqx;
}
sub
internalize {
    $file = shift;
    my @rows = ();
    my $csv = ($file =~ /cdli/ ? tsv_cdli() : tsv_default());

    open my $fh, "<:encoding(utf8)", $file 
	or die "XMD::TSV: can't open CSV file '$file'\n";

    my $xmd = $csv->getline_all($fh);
    my @xmd = @$xmd;
    my $fields_row = shift @xmd;
    if ($$fields_row[0] eq 'artifact_id') {
	warn "ORACC::XMD::TSV: remapping artifact_id to id_text\n";
	$id_type = 'artifact_id';
	$$fields_row[0] = 'id_text';
	warn "ORACC::XMD::TSV: value of fields_row[0] is now $$fields_row[0]\n";
    }
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
	$id_text_index = $i if ($fields[$i] eq 'id_text'
				|| $fields[$i] eq 'id_composite');
	$fields{$fields[$i]} = $i;
    }
}

sub tsv_cdli {
    Text::CSV_XS->new({ sep_char=>"\t" , auto_diag=>1 , allow_loose_quotes=>1 , 
			quote_char=>undef , escape_char=>undef , binary=>1 });
}

sub tsv_default {
    Text::CSV_XS->new({ sep_char=>"\t" , auto_diag=>1 , allow_loose_quotes=>1 , 
			quote_char=>'' , escape_char=>'' });
}

1;
