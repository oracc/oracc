package ORACC::SMA2::Graphinfo;

require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/g_breaks g_data g_reg g_skip/;

use warnings; use strict; use utf8; use open 'utf8';

use Data::Dumper;

my @data = ();
my @idata = ();

sub
compose_g_str {
    my($tlit,$bref) = @_;
    my @res = ();
    my @g = grep(defined&&length,split(/-/,$tlit));
    my @b = @$bref;
    for (my $i = 0; $i <= $#g; ++$i) {
	push @res, "$g[$i]$b[$i]";
    }
    join('-',@res);
}

sub
g_breaks {
    my @b = ();
    my @g = ();
    foreach my $g (@_) {
	if ($g =~ s/(['"])$//) {
	    push @b, $1;
	} else {
	    push @b, '';
	}
	push @g, $g;
    }
    ([@g],[@b]);
}

sub
g_data {
    return ([ @data ], [ @idata]);
}

# Args are the VPR slot; string version of graphemes
# (hyphen-separated); ref to a list of grapheme indexes; index of
# first char in graphemes that belongs to the morphem; index of last
# char in graphemes that belongs to the morpheme.
#
# g[3] = -1 means no morpheme correspondence
# g[3] = 0 means entire grapheme corresponds to morpheme
# g[3] and g[4] undefined means same as g[3]==0.
#
sub
g_reg {
    my($at,@g) = @_;
    my @gx = ();
    my @gix = ();
#    warn "greg: $at: ", Dumper \@g, "\n";
    foreach my $g (@g) {
	my $init_flag = 0;
	my $full_flag = 0;
	@gix = @{$$g[2]};
	my $s = compose_g_str($$g[0],$$g[1]);
	if (((defined($$g[3]) && $$g[3] == 0) && !defined($$g[4]))
	     ||	(!defined($$g[3]) && !defined($$g[4]))) {
	    $full_flag = 1;
	} elsif ($$g[3] == 0 && $$g[4] == 0) {
	    $init_flag = 1;
	}
	unless ($full_flag) {
	    if ($$g[3] == -1) {
		$s = "($s)";
	    } else {
		my @s = grep(defined&&length, split(/(.)/,$s));
		my($s_i,$s_m,$s_f) = ('','','');
		my $last_s_i = 0;
		my $first_s_f = $#s;
		$$g[4] = $#s unless $$g[4];
		#initial skip
		if ($$g[3] > 0) {
		    $s_i = join('','(',@s[0..$$g[3]-1],')');
		    $last_s_i = $$g[3];
		}
		#final skip
		if ($init_flag) {
		    # warn "trying s_f 0 .. end\n";
		    $s_i = '';
		    $s_f = join('','(',@s[1..$$g[4]],')');
		    $last_s_i = 0;
		    $first_s_f = 0;
		} else {
		    my $end = $#s;
		    if ($$g[4] < $end) {
			$s_f = join('','(',@s[$$g[4]+1..$end],')');
			$first_s_f = $$g[4]-1;
		    }
		}
		#'medial' non-skip
		$s_m = join('',@s[$last_s_i .. $first_s_f]);
		$s = join('',$s_i,$s_m,$s_f);
	    }
	}
	push @gx,$s;
    }
    $data[$at] = [ join('=',@gx) , @gix ];
}

sub g_skip {
    my($at,$g,$ix) = @_;
    $idata[$at] = [ "₀$g" , $ix ];
}

1;
