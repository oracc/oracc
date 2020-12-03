package ORACC::SMA::StripVPR;

require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/strip_vpr/;

use warnings;
use strict;

use ORACC::SMA::VPR;
use ORACC::SMA::VSF;
use ORACC::SMA::NSF;

$ORACC::SMA::StripVPR::prefix = '';

sub
strip_vpr {
    my @g = @_;

    # get the longest run of initial graphemes that parses as a VPR
    my $vpr_end = $#g;
    while ($vpr_end > 0) {
	my $tmp;
	if ($tmp = is_vpr(@g[0..$vpr_end])) {	    
	    $vpr_end = $tmp;
	    last;
	} else {
	    --$vpr_end;
	}
    }

    if ($vpr_end > 0) {
	# take the following graphemes as a verb base
	$ORACC::SMA::StripVPR::prefix = join('-',@g[0..$vpr_end-1]);
	return join('-',@g[$vpr_end..$#g]);
    } else {
	return undef;
    }
}

1;
