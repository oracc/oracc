package ORACC::XMD::TSV;
use Text::CSV_XS;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::NS;
use Data::Dumper;

my @fields = ();
my %fields = ();
my $file;
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
internalize {
    $file = shift;
    my @rows = ();
    my $csv = Text::CSV_XS->new({ binary=>1 , eol=>'\r' , sep_char=>'\t' });
    
    open my $fh, "<:encoding(utf8)", $file 
	or die "XMD::CSV: can't open CSV file '$file'\n";
    my $xmd = $csv->getline_all($fh);

    my @xmd = @$xmd;
    my $fields_row = shift @xmd;
    $xmd = [ @xmd ];

    initialize_fields($fields_row);
    
    if ($period_index >= 0) {
	clean_periods($xmd);
    } else {
	warn "ORACC::XMD::CSV: no period_index found in $file\n";
    }

    ([@fields],{%fields},$xmd);
}

sub
initialize_fields {
    @fields = @{$_[0]};
    %fields = ();
    for (my $i = 0; $i <= $#fields; ++$i) {
	$period_index = $i if $fields[$i] eq 'period';
	$fields{$fields[$i]} = $i;
    }
}

1;
