package ORACC::XMD::CSV;
use Text::CSV_XS;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::NS;
use Data::Dumper;

my $fieldnames = '00lib/fieldnames.row';
my @fields = ();
my %fields = ();
my $file;
my $id_text = -1;
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
    open(F,$fieldnames) 
	|| die "XMD::CSV: can't open fieldnames file '$fieldnames'\n";
    initialize_fields(<F>);
    close(F);
    my @rows = ();
    my $csv = Text::CSV_XS->new({ binary=>1 });
    
#    open my $fh, "<:encoding(utf8)", $file 
#	or die "XMD::CSV: can't open CSV file '$file'\n";
    my @xmd = ();
    my $i = 0;
    open(F, $file);
    while (1) {
	#	my $xmd = $csv->getline($fh);
	my $ln = <F>;
	last unless $ln;
	my $res = $csv->parse($ln);
	my $xmd = undef;
	if ($res) {
	    $xmd = [ $csv->fields() ];
	} else {
	    warn "$.: ", $csv->error_input, "\n";
	    $csv->setDiag(0);
	}
	++$i;
	if ($xmd) {
	    my $id = $$xmd[$id_text];
	    if ($id =~ /^P\d\d\d\d\d\d$/) {
		# good ID
	    } else {
		my $sid = sprintf("%s", $xmd[$id_text]);
		$id =~ s/^P0*//;
		$xmd[$id_text] = sprintf("P%06d", $id);
		$$xmd[$period_index] =~ s/\s+\([^\(]+\)?\s*$//;
		warn "$file:$i: id_text=$id; $sid => $xmd[$id_text]\n";
	    }
	    push @xmd, $xmd;
	} else {
	    warn "$.: parse failed\n";
	}
    }

#    if ($period_index >= 0) {
#	clean_periods($xmd);
#    } else {
#	warn "ORACC::XMD::CSV: no period_index found in $file\n";
#    }

    ([@fields], {%fields}, [@xmd]);
}

sub
initialize_fields {
    my $f = shift;
    chomp $f;
    $f =~ s/^\x{ef}\x{bb}\x{bf}//; # remove BOM
    $f =~ tr/"//d;
    @fields = split(/,/, $f);
    %fields = ();
    for (my $i = 0; $i <= $#fields; ++$i) {
	$period_index = $i if $fields[$i] eq 'period';
	$id_text = $i if $fields[$i] eq 'id_text';
	$fields{$fields[$i]} = $i;
    }
    die "ORACC::XMD::CSV: no id_text found in $file\n" unless $id_text >= 0;
}

1;
