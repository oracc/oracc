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
	# warn "morph_check @$mf\n";
	my($pre,$base,$post,$anteshare,$postshare) = @$mf;

	my %mparse = mdata_parse($$data{'morph'});
	if ($mparse{'error'}) {
	    foreach my $m (mdata_messages()) {
		pp_warn($m);
	    }
	    return;
	}
	
	# Now we have prefix/suffix substrings in %mparse to use for
	# lookup in morphdata to see if transliteration is allowed

	my $antesig;
	my $postsig;
	# first construct versions of the grapheme sequences that take account of any shared graphemes
	if ($anteshare) {
	    my $share = $base; $share =~ s/\..*$//;
	    $antesig = "$pre.$share";
	} else {
	    $antesig = $pre;
	}
	if ($postshare) {
	    my $share = $base; $share =~ s/^.*?\.([^.]+)$/$1/;
	    $postsig = "$share.$post";
	} else {
	    $postsig = $post;
	}
	
	if ($antesig) { # there should be some morphology before the base
	    if ($mparse{'vpr'}) {
		mcheck_sub('vpr',$antesig);
	    } else {
		pp_warn("(fbm) FORM $$data{'form'} has medial BASE $$data{'base'} but no prefix in MORPH $m");
	    }
	} elsif ($postsig) {
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
		while ($splitpoint <= $#s) {
		    my @sf1 = @s[0 .. $splitpoint-1];
		    my @sf2 = @s[$splitpoint .. $#s];
		    # print "sf1 == @sf1 :::: sf2 == @sf2\n";
		    my $res1 = mcheck_sub($mparse{'vsf'} ? 'vsf' : 'isf', join('.',@sf1));
		    my $res2 = mcheck_sub('nsf', join('.',@sf2));
		    if ($res1 && $res2) {
			push @tries, [ join('.',@sf1) , join('.',@sf2) , $res1 , $res2 ];
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
		mcheck_sub('vsf',$postsig);
	    } elsif ($mparse{'isf'}) {
		mcheck_sub('isf',$postsig);
	    } elsif ($mparse{'nsf'}) {
		mcheck_sub('nsf',$postsig);
	    }
	    if ($m =~ /[,!]\S+$/) {
		if ($$data{'postshare'}) {
		    my $share = $base; $share =~ s/\..*$//;
		    my $mlook = "$pre.$share";
		}
		
	    } else {
		pp_warn("(fbm) FORM $$data{'form'} has medial BASE $$data{'base'} but no postfix in MORPH $m");
	    } 
	}
    }
}

sub mcheck_sub {
    my($type,$sig,$noerr) = @_;
    my $mtlit = fbm_tlit($data,0,$type =~ tr/././ + 1);
    my $res = undef;
    if (($res = ORACC::SMA::MorphData::mdata($type,$m1,$msig,$mtlit))) {
	if ($res == 1) { # match at mtlit level
	} elsif ($res == 2) { # match at msig level
	} elsif ($res == 3) { # match at m1 level
	} else {
	    die "$0: unknown return value from ORACC::SMA::MorphData::is_known($type,$m1,$msig,$mtlit)\n";
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
    # This is a grapheme spec where some items contain ^N^ encoding for signature spans
    # and others can contain determinative info.  Need to code the hyphenation better ...
    join('-',@g);
}

1;
