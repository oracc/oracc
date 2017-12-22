package ORACC::CBD::PPWarn;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/pp_diagnostics pp_file pp_line pp_notice pp_status pp_trace pp_warn/;

use warnings; use strict; binmode STDERR, ':utf8';

$ORACC::CBD::PPWarn::trace = 0;

my %errlist = ();
my $line = 0;
my $file = '';
my $status = 0;

sub pp_diagnostics {
    my $args = shift;
    my @e = sort { $a <=> $b } keys %errlist;
    if ($#e >= 0) {
	foreach my $e (@e) {
	    warn @{$errlist{$e}};
	}
    } else {
	warn "$file OK.\n" if $$args{'check'};
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

sub pp_notice {
    push @{$errlist{$line}}, "$file:$line: @_\n";
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
    pp_notice(@_);
    ++$status;
}

1;
