package ORACC::FXR::Util;
use warnings; use strict; use open 'utf8';
my @fields = ();
my %fields = ();
my $fxr_rnc = '@@ORACC@@/lib/ORACC/FXR/fxr.rnc';
my $fxr_ns = '';

my $verbose = 0;

my %metadata = (
    'http://www.filemaker.com/xml/fmresultset'=>'</metadata>',
    'http://www.filemaker.com/fmpxmlresult'=>'</METADATA>',
    );

my %record_readers = (
    'http://www.filemaker.com/xml/fmresultset'=>\&xmlfmresultset_records,
    'http://www.filemaker.com/fmpxmlresult'=>\&fmpxmlresult_rows,
    );

my $metadata_splitter = undef;
my $record_reader = undef;

# return 1 if the argument checks out OK, 0 otherwise
sub
check {
    my $arg = shift;
    my $tmp = "/tmp/rnv-$$.log";
    system "rnv -q $fxr_rnc $arg 2>$tmp";
    if (-z $tmp) {
	unlink $tmp;
	return 1;
    } else {
	system "cat $tmp; rm $tmp";
	return 0;
    }
}

# Scan for the namespace and morph on it
sub
initialize_xmlns {
    local $/ = '>';
    while (<FXR>) {
	last if /xmlns=\"/;
    }
    if (/xmlns=\"(.*?)\"/) {
	$fxr_ns = $1;
	print STDERR "ORACC::FXR::Util: xmlns=$fxr_ns\n" if $verbose;
	$metadata_splitter = $metadata{$fxr_ns}
	|| die "ORACC::FXR::Util: no metadata splitter for $fxr_ns\n";
	$record_reader = $record_readers{$fxr_ns}
	|| die "ORACC::FXR::Util: no record reader for $fxr_ns\n";
    } else {
	die "ORACC::FXR::Util: no xmlns\n";
    }
}

sub
initialize_fields {
    local $/ = $metadata_splitter;
    my $preamble = <FXR>;
    my $i = 0;
    @fields = ();
    %fields = ();
    foreach my $f (split(/\/>/,$preamble)) {
	if ($f =~ /<field.*?name=\"(.*?)\"/i) {
	    my $ok_name = $1;
	    $ok_name =~ tr/: '/___/;
	    $ok_name =~ s/\&apos;/_/g;
	    $fields[$i] = $ok_name;
	    $fields{$ok_name} = $i++;
	}
    }
    print STDERR "ORACC::FXR::Util: $#fields fields\n" if $verbose;
}

# close and reopen FXR each time we are about to change RS
sub
internalize {
    my $arg = shift;
    die "ORACC::FXR::Util: no such file $arg\n"
	unless open(FXR,$arg);
    initialize_xmlns();
    close(FXR);
    open(FXR,$arg);
    initialize_fields();
    close(FXR);
    open(FXR,$arg);
    my $rows = &$record_reader;
    ([@fields],{%fields},$rows);
}

sub
xmlfmresultset_records {
    local $/ = '</record>';
    my @rows = ();
    my $pacifier = 0;
    print STDERR "ORACC::FXR::Util: reading records " if $verbose;
    my $nfields = $#fields;
    while (<FXR>) {
	last unless /<data/;
	tr/\t\cK\n\r/    /;
	if (tr/\001-\037//d) {
	    warn "FMP XML bad character in row ", $#rows+1,"\n";
	}
	my @row = (m,<data[/>](.*?)(?:>|</data>),g);
	print STDERR and warn("$#row != $nfields\n") unless $#row == $nfields;
	push @rows, [ @row ];
	if ($verbose) {
	    print STDERR '.' unless ($pacifier++ % 10000);
	}
    }
    close(FXR);
    print STDERR " $#rows\n" if $verbose;
    [@rows];
}

sub
fmpxmlresult_rows {
    $/ = '</ROW>';
    my @rows = ();
    my $pacifier = 0;
    print STDERR "ORACC::FXR::Util: reading records " if $verbose;
    my $nfields = $#fields;
    while (<FXR>) {
	last unless /<COL/;
	my @row = (m,<COL>(.*?)</COL>,g);
	print STDERR and warn("$#row != $nfields\n") unless $#row == $nfields;
	@row = map { fix_data($_) } @row;
	push @rows, [ map {tr/\r\n/  /;$_} @row ];
	if ($verbose) {
	    print STDERR '.' unless ($pacifier++ % 10000);
	}
    }
    close(FXR);
    print STDERR " $#rows\n" if $verbose;
    [@rows];
}

sub
fix_data {
    $_[0] =~ s/^<DATA.*?>\s*//;
    $_[0] =~ s#\s*</DATA>$##;
    $_[0] =~ s#\s*</DATA><DATA>\s*# \000 #g; # we'll write this as structured XML
    $_[0];
}

1;
