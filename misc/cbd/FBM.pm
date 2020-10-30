package ORACC::CBD::FBM;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/fbm_init fbm_term fbm_base_in_form fbm_morph_check/;

use warnings; use strict; use open 'utf8'; use utf8;

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::CBD::PPWarn;
use ORACC::SL::BaseC;
use ORACC::SMA::MorphData;

# set this to 1 if you are calling fbm routines with no cf/gw/pos
$ORACC::CBD::FBM::no_warn_incomplete = 0;

my $verbose = 0;

sub fbm_init {
    ORACC::SL::BaseC::init();
    ORACC::SMA::MorphData::init();
}

sub fbm_term {
    ORACC::SL::BaseC::term();
    ORACC::SMA::MorphData::term();
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

	    # Detect anteshare and postshare graphemes where base and morphology are
	    # written sharing a grapheme and flag them in the frames.
	    # If there is a cont we don't record that in postshare becuase it
	    # won't be needed for morphology lookup as it's already included in
	    # the post graphemes.
	    my $anteshare = ($$data{'base'} =~ /°/ || 0);
	    my $postshare = ($$data{'base'} =~ /·/ || 0);

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
	warn "fbm_morph_check mframes: @$mf\n" if $verbose;

	my($pre,$base,$post,$anteshare,$postshare) = @$mf;

	my $n_base_g = ($base =~ tr/././ + 1);

	my %mparse = ORACC::SMA::MorphData::mdata_parse($$data{'morph'});
	if ($mparse{'error'}) {
	    foreach my $m (mdata_messages()) {
		pp_warn($m);
	    }
	    return;
	}
	my @mp = %mparse;
	warn "fbm_morph_check mparse: @mp\n" if $verbose;

	# Now we have prefix/suffix substrings in %mparse to use for
	# lookup in morphdata to see if transliteration is allowed

	my $antesig;
	my $postsig;
	my $index = 0;
	my $res;
	# first construct versions of the grapheme sequences that take account of any shared graphemes
	if ($anteshare) {
	    my $share = $base; $share =~ s/\..*$//;
	    $antesig = "$pre.$share";
	    --$n_base_g; # decrement because we just ate the first base grapheme
	} else {
	    $antesig = $pre;
	}
	if ($postshare) {
	    my $share = $base; $share =~ s/^.*?\.([^.]+)$/$1/;
	    $postsig = "$share.$post";
	    # unless base is one grapheme and also has anteshare,
	    # decrement because we just ate the first base grapheme
	    --$n_base_g if $n_base_g;
	} else {
	    $postsig = $post;
	}

	if ($antesig) { # there should be some morphology before the base
	    if ($mparse{'vpr'}) {
		if (($res = mcheck_sub($data,'vpr',$index,$mparse{'vpr'},$antesig))) {
		    $index += ($antesig =~ tr/././);
		} else {
		    return;
		}
	    } else {
		pp_warn("(fbm) FORM $$data{'form'} has medial BASE $$data{'base'} but no prefix in MORPH $$data{'morph'}");
	    }
	}

	# From here on we are working on suffixes
	$index += $n_base_g;

	if ($postsig) {
	    # There could be more than one morpheme subsequence here:
	    # vsf nsf
	    # isf nsf (*possibly never happens)
	    # vsf
	    # isf
	    # nsf
	    if (($mparse{'vsf'} || $mparse{'isf'}) && $mparse{'nsf'}) {
		my @s = split(/\./,$postsig);
		my $splitpoint = 1;
		my @tries = ();
		my $used = 0;
		while ($splitpoint <= $#s) {
		    my @sf1 = @s[0 .. $splitpoint-1];
		    my @sf2 = @s[$splitpoint .. $#s];
		    # print "sf1 == @sf1 :::: sf2 == @sf2\n";
		    my $res1 = '';
		    if ($mparse{'vsf'}) {
			$res1 = mcheck_sub($data, 'vsf', $index, $mparse{'vsf'}, join('.',@sf1));
		    } else {
			$res1 = mcheck_sub($data, 'isf', $index, $mparse{'isf'}, join('.',@sf1));
		    }
		    return unless $res1;
		    $index += $#sf1 + 1;
		    my $res2 = mcheck_sub($data, 'nsf', $index, $mparse{'nsf'}, join('.',@sf2));
		    if ($res2) {
			$index += $#sf2 + 1;
			if ($index > ($$data{'form_tlit'} =~ tr/ / /)) {
			    push @tries, [ join('.',@sf1) , join('.',@sf2) , $res1 , $res2 ];
			} else {
			    pp_warn("(fbm) unused graphemes at end of $$data{'form'}");
			}
		    }
		    ++$splitpoint;
		}
		if ($#tries < 0) {
		    my $morph = ",$mparse{'nsf'}";
		    if ($mparse{'vsf'}) {
			$morph = ";$mparse{'vsf'}$morph";
		    } else {
			$morph = "!$mparse{'isf'}$morph";
		    }
		    pp_warn("(fbm) no match for $morph == $postsig");
		}
	    } elsif ($mparse{'vsf'}) {
		$res = mcheck_sub($data,'vsf',$index,$mparse{'vsf'},$postsig);
		return unless $res;
		$index += ($postsig =~ tr/././ + 1);
		pp_warn("(fbm) unused graphemes at end of $$data{'form'}") unless $index > ($$data{'form'} =~ tr/ / /);
	    } elsif ($mparse{'isf'}) {
		$res = mcheck_sub($data,'isf',$index,$mparse{'isf'},$postsig);
		return unless $res;
		$index += ($postsig =~ tr/././ + 1);
		pp_warn("(fbm) unused graphemes at end of $$data{'form'}") unless $index > ($$data{'form'} =~ tr/ / /);
	    } elsif ($mparse{'nsf'}) {
		$res = mcheck_sub($data,'nsf',$index,$mparse{'nsf'},$postsig);
		return unless $res;
		$index += ($postsig =~ tr/././ + 1);
		pp_warn("(fbm) unused graphemes at end of $$data{'form'}") unless $index > ($$data{'form'} =~ tr/ / /);
	    } else {
		pp_warn("(fbm) FORM $$data{'form'} has medial BASE $$data{'base'} but no postfix in MORPH $$data{'morph'}");
	    } 
	}
    }
}

sub mcheck_sub {
    warn "mcheck_sub args: @_\n" if $verbose;
    my($data,$type,$index,$m1,$msig,$noerr) = @_;
    #    my $mtlit = fbm_tlit($data,0,$msig =~ tr/././ + 1);
    my $mtlit = fbm_tlit($data,$index,$msig =~ tr/././ + 1);
    my $resref = ORACC::SMA::MorphData::mdata_lookup($type,$m1,$msig,$mtlit);
    warn "mcheck_sub return from mdata: @$resref\n" if $verbose;
    my ($res,$val) = @$resref;
    if ($res) {
	if ($res == 1 || $res == 2 || $res == 3) {
	    foreach my $m (mdata_messages()) {
		pp_warn($m);
	    }
	} else {
	    die "$0: unknown return value ($res) from ORACC::SMA::MorphData::is_known($type,$m1,$msig,$mtlit)\n";
	}
    } else {
	pp_warn("(fbm) sig $mtlit/$msig not known for $type prefix $m1") unless $noerr;
    }
    $res;
}

sub fbm_tlit {
    my ($data,$index,$length) = @_;
    warn "$0: null or bad hashref passed to fbm_lit\n" and return ''
	unless $data && ref($data) eq 'HASH' && $$data{'form_tlit'};
    my @t = split(/\s+/,$$data{'form_tlit'});
    my $i = $index; $i = 1 + $#t + $index if $index < 0;
    my $end = $i+$length-1;
    my @g = @t[$i..$end];
    my @dg = grep defined, @g;
    if ($#g != $#dg) {
	warn "$0: @t segment $index .. $end produced null array element\n";
	return join('-',grep(defined,@g));
    } else {
	# This is a grapheme spec where some items contain ^N^ encoding for signature spans
	# and others can contain determinative info.  Need to code the hyphenation better ...
	return join('-',@g);
    }
    
}

1;
