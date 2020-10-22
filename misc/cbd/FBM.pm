package ORACC::CBD::FBM;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/fbm_init fbm_term fbm_base_in_form/;

use warnings; use strict; use open 'utf8'; use utf8;

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::CBD::PPWarn;
use ORACC::SL::BaseC;

# set this to 1 if you are calling fbm routines with no cf/gw/pos
$ORACC::CBD::FBM::no_warn_incomplete = 0;

sub fbm_init {
    ORACC::SL::BaseC::init();
}

sub fbm_term {
    ORACC::SL::BaseC::term();
}

sub fbm_base_in_form {
    my %data = @_;
    return if fbm_data_setup(\%data);
    my $form_sig = ORACC::SL::BaseC::tlit_sig('',$data{'form'});
    my $base_sig = ORACC::SL::BaseC::tlit_sig('',$data{'base'});
    my $nmatch = 0;
    my $f = $form_sig;
    while ($f) {
	++$nmatch if $f =~ /^$base_sig/;
	$f =~ s/^[^\.]+\.?//;
    }
    pp_warn("base $data{'base'} not found in form $data{'form'}")
	unless $nmatch;
    pp_warn("base $data{'base'} found at $nmatch locations in form $data{'form'}")
	if $nmatch > 1;
}

sub fbm_data_setup {
    my $data = shift;
    unless ($$data{'file'} && defined $$data{'line'}) {
	warn "$0: fbm subroutine called without setting 'file' and 'line' in \%data\n";
	return 1;
    }
    pp_file($$data{'file'});
    pp_line($$data{'line'});
    unless ($$data{'form'} && $$data{'base'} && $$data{'morph'}) {
	pp_warn("fbm subroutine called without one or more of form/base/morph");
	return 1;
    }
    unless ($$data{'cf'} && $$data{'gw'} && $$data{'pos'}) {
	pp_notify("fbm subroutine called with incomplete cf/gw/pos data")
	    unless $ORACC::CBD::FBM::no_warn_incomplete;
    }
    return 0;
}

1;
