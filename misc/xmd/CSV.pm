package ORACC::XMD::CSV;
use Text::CSV_XS;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::NS;
use Data::Dumper;

my $fieldnames = '00any/cdli.ucla.edu/fieldnames.row';
my @fields = ();
my %fields = ();
my $file;
my $period_index = -1;

#my @db = internalize('00any/cdli.ucla.edu/test.csv');
#print Dumper \@db;

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
    open(F,$fieldnames) 
	|| die "XMD::CSV: can't open fieldnames file '$fieldnames'\n";
    initialize_fields(<F>);
    close(F);
    my @rows = ();
    my $csv = Text::CSV_XS->new({ binary=>1 });
    
    open my $fh, "<:encoding(utf8)", $file 
	or die "XMD::CSV: can't open CSV file '$file'\n";
    my $xmd = $csv->getline_all($fh);
    
    if ($period_index >= 0) {
	clean_periods($xmd);
    } else {
	warn "ORACC::XMD::CSV: no period_index found in $file\n";
    }

    ([@fields],{%fields},$xmd);
}

sub
initialize_fields {
    my $f = shift;
    chomp $f;
    @fields = split(/,/, $f);
    %fields = ();
    for (my $i = 0; $i <= $#fields; ++$i) {
	$period_index = $i if $fields[$i] eq 'period';
	$fields{$fields[$i]} = $i;
    }
}

1;
