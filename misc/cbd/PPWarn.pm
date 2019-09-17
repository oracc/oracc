package ORACC::CBD::PPWarn;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/pp_diagnostics pp_file pp_line pp_notice pp_status pp_trace pp_warn/;

use warnings; use strict; binmode STDERR, ':utf8';

$ORACC::CBD::PPWarn::trace = 0;
$ORACC::CBD::PPWarn::edit_trace = 0;

my %errlist = ();
my $line = 0;
my $efile = '';
my $file = '';
my $status = 0;

sub flcmp {
    my($af,$al) = ($a =~ m/^(.*?)::(.*?)$/);
    my($bf,$bl) = ($b =~ m/^(.*?)::(.*?)$/);
    $af cmp $bf || $al <=> $bl;
}

sub pp_diagnostics {
    my $args = shift;
    return if $ORACC::CBD::nodiagnostics;
    my @e = sort { &flcmp; } keys %errlist;
    my $ret = $#e + 1;
    if ($$args{'log'}) {
	open(STDERR, '>', $$args{'log'}) || die "$0: unable to open $$args{'log'} to save diagnostics\n";
    }
    if ($#e >= 0) {
	foreach my $e (@e) {
	    warn @{$errlist{$e}};
	}	
    } else {
	warn "$file OK.\n" if $$args{'check'} && $$args{'announce'};
	$ret = 0;
    }
    $status;
}

sub pp_file {
    if (defined $_[0]) {
	$file = shift;
    }
    $file;
}

sub pp_line {
    if (defined $_[0]) {
	$line = shift;
    }
    $line;
}

sub pp_notice {
    $efile = ORACC::CBD::Util::errfile($file);
    $efile = pp_file() unless $efile;
    $line = pp_line() unless $line;
    push @{$errlist{"${efile}::$line"}}, "$efile:$line: @_\n";
}

sub pp_status {
    my $arg = shift;
    if (defined($arg)) {
#	warn "status: arg=$arg\n";
	$status = $arg;
	if ($status == 0) {
	    %errlist = ();
	}
    } else {
	$status;
    }
}

sub pp_trace {
    if ($ORACC::CBD::PPWarn::trace) {
	warn pp_line(), ':', @_, "\n";
    }
}

sub pp_warn {
    pp_notice(@_);
    ++$status;
}

1;
