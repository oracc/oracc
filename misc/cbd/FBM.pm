package ORACC::CBD::FBM;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/fbm_init fbm_term fbm_base_in_form fbm_morph_check/;

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
    my $data = shift;
    return unless $data && ref($data) eq 'HASH' && !$$data{'base_in_form'};
    $$data{'base_in_form'} = 1;
    return if fbm_data_setup($data);
    my $form_sig = $$data{'form_sig'} = ORACC::SL::BaseC::tlit_sig('',$$data{'form'});
    $$data{'form_tlit'} = join(' ',@ORACC::SL::BaseC::last_tlit);
    my $base_sig = $$data{'base_sig'} = ORACC::SL::BaseC::tlit_sig('',$$data{'base'});
    $$data{'base_tlit'} = join(' ',@ORACC::SL::BaseC::last_tlit);
    my $nmatch = 0;
    my $f = $form_sig;
    my @mframes = ();
    my $pre = '';
    while ($f) {
	if ($f =~ /^$base_sig(\.\S+)?$/) {
	    ++$nmatch;
	    my $post = $1 || ''; $post =~ s/^\.// if $post;
	    # detect anteshare and postshare graphemes where base and morphology are
	    # written sharing a grapheme and flag them in the frames
	    my $anteshare = ($$data{'base'} =~ /°/ || 0);
	    my $postshare = ($$data{'base'} =~ /·/ || 0);
	    unless ($postshare) {
		$postshare = 2 if $$data{'cont'};
	    }
	    my $tmp = $pre; $tmp =~ s/\.$// if $tmp;
	    push @mframes, [ $tmp, $base_sig, $post, $anteshare, $postshare ];
	}
	$f =~ s/^([^\.]+\.?)//;
	$pre .= $1;
    }
    $$data{'mframes'} = \@mframes;
    pp_warn("base $$data{'base'} not found in form $$data{'form'}")
	unless $nmatch;
    pp_warn("base $$data{'base'} found at $nmatch locations in form $$data{'form'}")
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

sub fbm_morph_check {
    my $data = shift;
    return unless $data && ref($data) eq 'HASH' && !$$data{'morph_check'};
    $$data{'morph_check'} = 1;
    my @mframes_ok = ();
    foreach my $mf (@{$$data{'mframes'}}) {
	# warn "morph_check @$mf\n";
	my($pre,$base,$post) = @$mf;
	my $m = $$data{'morph'};
	if ($pre) {
	    if ($m =~ /^(.*?):/) {
	    } else {
		pp_warn("(fbm) FORM $$data{'form'} has medial BASE $$data{'base'} but no prefix in MORPH $m");
	    }
	} elsif ($post) {
	    if ($m =~ /[,!]\S+$/) {
	    } else {
		pp_warn("(fbm) FORM $$data{'form'} has medial BASE $$data{'base'} but no postfix in MORPH $m");
	    } 
	}
    }
}

1;
