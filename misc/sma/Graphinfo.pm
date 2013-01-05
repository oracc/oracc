package ORACC::SMA::Graphinfo;

require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/g_breaks g_data g_reg/;

use warnings; use strict; use utf8; use open 'utf8';

my @data = ();

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
    return [ @data ];
}

sub
g_reg {
    my($at,@g) = @_;
    my @gx = ();
    foreach my $g (@g) {
	my $s = compose_g_str($$g[0],$$g[1]);
	if ($$g[2] == -1) {
	    $s = "($s)";
	} else {
	    my @s = grep(defined&&length, split(/(.)/,$s));
	    my($s_i,$s_m,$s_f) = ('','','');
	    $$g[3] = length($s)-1 unless $$g[3];
	    #initial skip
	    if ($$g[2] > 0) {
		$s_i = join('','(',@s[0..$$g[2]-1],')');
	    }
	    #final skip
	    my $end = length($s)-1;
	    if ($$g[3] < $end) {
		$s_f = join('','(',@s[$$g[3]+1..$end],')');
	    }
	    #'medial' non-skip
	    $s_m = join('',@s[$$g[2]..$$g[3]]);
	    $s = join('',$s_i,$s_m,$s_f);
	}
	push @gx,$s;
    }
    $data[$at] = join('=',@gx);
}

1;
