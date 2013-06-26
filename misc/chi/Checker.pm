package ORACC::CHI::Checker;
use warnings; use strict;

$ORACC::CHI::Checker::error = '';

my $NCName = '[_A-Za-z][-._a-zA-Z0-9\p{L}]*';

# Return 1 if the CHI is OK.
sub
check {
    my ($c,$ix) = @_;
    my @c = split(/\./, $c);
    foreach (@c) {
	if (/^$NCName$/o) {
	    if (/^[a-z]_/) {
		return 0;
	    }
	} else {
	    return 0;
	}
    }
    if ($ix) {
	if (ORACC::CHI::Indexer::is_known($c, $ix)) {
	    unless (ORACC::CHI::Indexer::is_terminal($c, $ix)) {
		my $t = ORACC::CHI::Indexer::terminal_of($c, $ix);
		if ($t =~ /^bad terminal: (\S+)/) {
		    $ORACC::CHI::Checker::error = "can't remap: $1 remaps";
		    return 0;
		} else {
		    $ORACC::CHI::Checker::error = "needs-remap\t$t";
		    return -1;
		}
	    }
	} else {
	    $ORACC::CHI::Checker::error = 'unknown';
	    return 0;
	}
    }
    1;
}

1;
