package ORACC::CBD::PPWarn;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/pp_diagnostics pp_file pp_line pp_status pp_trace pp_warn/;

use warnings; use strict; binmode STDERR, ':utf8';

$ORACC::CBD::PPWarn::trace = 0;

my %errlist = ();
my $line = 0;
my $file = '';
my $status = 0;

sub pp_diagnostics {
    foreach my $e (sort { $a <=> $b } keys %errlist) {
	warn @{$errlist{$e}};
    }
}

sub pp_file {
    $file = shift;
}

sub pp_line {
    if (defined $_[0]) {
	$line = shift;
    }
    $line;
}

sub pp_status {
    $status;
}

sub pp_trace {
    if ($ORACC::CBD::PPWarn::trace) {
	warn @_, "\n";
    }
}

sub pp_warn {
    push @{$errlist{$line}}, "$file:$line: @_\n";
    ++$status;
}

1;
