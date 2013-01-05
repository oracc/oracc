#!/usr/bin/perl
use warnings; use strict;

use Number::Fraction;
use Math::Units qw(convert);

use utf8;
use open ':utf8';
use PSD::SUM::CC;
PSD::SUM::CC::load();

binmode STDIN, ':utf8';
binmode STDOUT, ':utf8';

my $ns_uri="http://psd.museum.upenn.edu/nm";

my @at = qw/nm system newpage step inst inst-system examples ex/;
my %at = ();
@at{@at} = ();

my %children = (
    '#top'	=> { 'nm'=>1, 'system'=>1 },
    'nm'	=> { },
    'newpage'	=> { },
    'system' 	=> { 'newpage'=>1, 'step'=>1, 'examples'=>1 },
    'step' 	=> { 'inst'=>1, 'inst-system'=>1 },
    'inst'	=> { },
    'inst-system'=> { },
    'examples'  => { 'ex'=>1 },
    'ex'	=> { },
);

my @seq = ();
my $i = -1;
my $system_title = '';

my %step_map = ();

#my $mev_base;
#my $step_mev;
my $sys_meb = '';
my $sys_meq = '';
my $sys_meq_frac = '';
my $sys_meu = '';

my %aev = ();
my %mev = ();

while (<>) {
    next if /^\#/ || /^\s*$/;
    chomp;
    if (s/^\@(\S+)\s*//) {
	my $at = $1;
	if (exists $at{$at}) {
	    push @seq, [$at , $_];
	} else {
	    nmwarn("unknown at-command '$at'");
	}
    } else {
	nmwarn("bad start of line");
    }
}

build_step_mevs();

do_seq($children{'#top'});

####################################################

sub
nmwarn {
    warn("$ARGV:$.: ", @_, "\n");
}
sub
do_seq {
    my($childref) = shift;
    my %myChildren = %$childref;
    ++$i;
    while ($i <= $#seq) {
	last unless defined $myChildren{ ${$seq[$i]}[0] };
	do_seq_entry();
    }
}
sub
do_seq_entry {
    my $seq = $seq[$i];
    my($at,$arg) = ($$seq[0],$$seq[1]);
    {
	no strict;
	my $atfunc = $at;
	$atfunc =~ tr/-/_/;
	&$atfunc($arg);
    }
}
sub
nm {
    $system_title = shift;
    do_seq($children{'nm'});
}
sub
newpage {
    print "<page-break\n/>";
    do_seq($children{'newpage'});
}
sub
system {
    my $arg = shift;
    if ($arg =~ s/^(\S+)\s*//) {
	my $system = $1;
	my %args = ();
	foreach my $a (qw/type time place usedfor meb mev/) {
	    my($val) = $arg =~ /$a=(\S+)/;
	    $args{$a} = $val || '';
	}
	$args{'type'} = 'met' unless length $args{'type'};
	print <<EOS;
<system xmlns="$ns_uri" title="$system_title"
        n="$system" time="$args{'time'}" place="$args{'place'}" 
	usedfor="$args{'usedfor'}" type="$args{'type'}"
	meb="$args{'meb'}" mev="$args{'mev'}"
EOS
	    print ">";
	do_seq($children{'system'});
	print "</system>";
    } else {
	nmwarn("bad \@system syntax");
    }
}
sub
step {
    my $arg = shift;
    if ($arg =~ s/^(\S+)\s*//) {
	my $step_ascii = $1;
	my $step = PSD::ATF::Unicode::gconv($step_ascii);
	my %args = ();
	foreach my $a (qw/atf aev/) {
	    my $val = '';
	    if ($arg =~ /$a=\"/) {
		($val) = ($arg =~ /$a=\"(.*?)\"/);
	    } else {
		($val) = ($arg =~ /$a=(\S+)/);
	    }
	    $args{$a} = $val || '';
	}
	if (defined $args{'atf'}) {
	    $step_map{$step} = $args{'atf'};
	}
	$step =~ tr/_/ /;
	if (defined $args{'aev'}) {
	    if ($args{'aev'} =~ /\*/) {
		my $aev_rel = $args{'aev'};
		$args{'aev'} = $aev{$step_ascii} || '';
		warn "no aev found for step $step_ascii\n"
		    unless defined $args{'aev'};
		my($aeq,$aeu) = ($aev_rel =~ /^(.*?)\*(.*)$/);
		$aeu = PSD::ATF::Unicode::gconv($aeu);
		# this is a real hack...
		$args{'aev'} = "$args{'aev'}\" aeq=\"$aeq\" aeu=\"$aeu";
	    }
	}

	my $mev;
	my $mev_disp_str = '';
	if (defined $mev{$step_ascii}) {
	   $mev = $mev{$step_ascii};
	   $mev .= $sys_meu;
	   my $mev_disp = render_mev($mev);
	   $mev_disp_str = " mev=\"$mev\" mev-disp=\"$mev_disp\"";
	}
   
	print "<step n=\"$step\" atf=\"$args{'atf'}\" aev=\"$args{'aev'}\"";
	print "$mev_disp_str>";
	do_seq($children{'step'});
	print "</step>";
    }
}
sub
inst {
    my $arg = shift;
    my($count,$unit,$range,$aev,$glyph)
	= ($arg =~ /^(.*?)\s+(.*?)\s+(.*?)\s+(.*?)\s+(.*?)$/);
    my($exp,$atf,$mod) = ($glyph =~ /^(.*)=(\S+)(.*)$/);
    $exp = $exp || $glyph;
    $atf = $atf || $glyph;
    $exp .= $mod if $mod;
    $atf .= $mod if $mod;
    my(@vals);
    if ($range =~ /^(\d+)\.\.(\d+)$/) {
	my($min,$max) = ($1,$2);
	my $j = 0;
	for (my $i = $min; $i <= $max; ++$i) {
	    $vals[$j++] = $i;
	}
    } elsif ($range =~ /^(\d+)\.\.n=(.*?)$/) {
	my($min,$n) = ($1,$2);
	my $j = 0;
	for (my $i = $min; $i <= $n; ++$i) {
	    $vals[$j++] = $i;
	}
    } elsif ($range =~ /^n=(.*?)$/) {
	my $n = $1;
	my $j = 0;
	for my $i (split(/,/,$n)) {
	    $vals[$j++] = $i;
	}
    } else {
	$vals[0] = $range;
    }
    foreach my $v (@vals) {
	my($patf,$pexp) = ($atf,$exp);
	my $value;
	if ($aev =~ /\*/) {
	    my($aeq,$aeu) = ($aev =~ /^(.*?)\*(.*)$/);
	    $aeu = PSD::ATF::Unicode::gconv($aeu);
#	    $paev = "$aeq $aeu";
	    if ($aev =~ m,(\d+)/(\d+),) {
		my ($top,$bot) = ($1,$2);
		my $val = $top * $v;
#	    	$aeq = normalize_fraction("$val/$bot");
		$aeq = "$val/$bot";
	    } elsif ($v ne 'n') {
		if ($aeq =~ /\/n$/) {
		    $aeq =~ s/n/$v/;
		} else {
		    $aeq = $aeq * $v;
		}
	    }
	    # this is a real hack...
	    $value = "#\" aeq=\"$aeq\" aeu=\"$aeu";
	} elsif ($aev =~ /1\/n/) {
	    my $mult = $aev;
	    $mult =~ s/1\/n(?:×)?//;
	    $mult = $mult || '1';
	    $value = $mult * $v;
	    $value = normalize_fraction("1/$value");
	} elsif ($aev =~ m,(\d+)/(\d+),) {
	    my ($top,$bot) = ($1,$2);
	    $value = $top * $v;
#	    $value = normalize_fraction("$value/$bot");
	    $value = "$value/$bot";
	} else {
	    $value = $aev * $v;
	}
	$patf =~ s/\#/$v/x;
	$pexp =~ s/\#/$v/x;
	my $cc_pexp = $pexp;
	$cc_pexp =~ s/\{(.*?)\}/.$1./g;
	$cc_pexp =~ tr/|//d;
	$cc_pexp =~ s/BAD/ESZE3/g; # ugliest of hacks!
	my $cc = PSD::SUM::CC::atf_xml($cc_pexp) | '';
	print <<EOS;
<step-instance count="$v" aev="$value" atf="$patf" glyph="$pexp" cc="$cc"
EOS
	print "/>";
    }
    do_seq($children{'inst'});
}
sub
inst_system {
    my $arg = shift;
    print "<inst-system n=\"$arg\"\n/>";
    do_seq($children{'inst-system'});
}
sub
examples {
    print "<examples>";
    do_seq($children{'examples'});
    print "</examples>";
}
sub
ex {
    my $arg = shift;
    my $cc_arg = $arg;
#    $cc_arg =~ s/\{(.*?)\}/.$1./g;
#    $cc_arg =~ tr/|//d;
    my $cc = PSD::SUM::CC::atf_xml($cc_arg) || '';
    print "<example cc=\"$cc\" atf=\"$arg\"\n/>";
    do_seq($children{'ex'});
}
sub
normalize_fraction {
    my $f = Number::Fraction->new($_[0]);
    sprintf $f;
}

#########################################################

sub
build_step_mevs {
    my %steps = ();
    my %rsteps = ();
    my @known_steps = ();

    foreach my $s (@seq) {
	if ($$s[0] eq 'system') {
	    my $arg = $$s[1];
	    my $mev;
	    ($sys_meb) = ($arg =~ /meb=(\S+)/);
	    ($mev) = ($arg =~ /mev=(\S+)/);
	    if ($mev =~ /^(.*?\d)([a-z]+(?:\^\d)?)$/) {
		$sys_meq = $1;
		$sys_meu = $2;
	    } else {
		return; # it's a numerical equivalence so nothing to do
	    }
#	    warn "meq = $sys_meq; meu = $sys_meu\n";
	} elsif ($$s[0] eq 'step') {
	    my $arg = $$s[1];
	    my($n) = ($arg =~ /^(\S+)/);
	    my($aev) = ($arg =~ /aev=(\S+)/);
	    my($aeq,$aeu);
	    if (defined $aev) {
		if ($aev =~ /\*/) {
		    ($aeq,$aeu) = ($aev =~ /^(.*?)\*(.*)$/);
		} else {
		    $aeq = $aev;
		    $aeu = '';
		}
	    } else {
		$aeq = $aeu = '';
	    }
	    push @{$steps{$n}}, [ $aeq , $aeu ];
	    push @{$rsteps{$aeu}}, $n;
#	    push @{$rsteps{$n}}, $aeu;
#	    warn "pushed on n=$n; aeq = $aeq; aeu = $aeu\n";
#	    warn "pushed on r=$aeu; $n\n";
	}
    }
    push @known_steps, $sys_meb;
    push @{$rsteps{$sys_meb}}, $sys_meb;
    $aev{$sys_meb} = Number::Fraction->new('1/1');
    $mev{$sys_meb} = $sys_meq;
    my $sys_meq_frac;
    if ($sys_meq =~ m,/,) {
	$sys_meq_frac = Number::Fraction->new($sys_meq);
    } else {
	$sys_meq_frac = Number::Fraction->new("$sys_meq/1");
    }
  DO_STEPS:
    {
	my @newly_known_steps = ();
	foreach my $s (@known_steps) {
	    foreach my $pair (@{$steps{$s}}) {
		my($s_aeq,$s_aeu) = ($$pair[0],$$pair[1]);
		next unless length $s_aeu;
		next if defined $aev{$s_aeu};
		push @newly_known_steps, $s_aeu;
		my $s_aeq_frac;
		if ($s_aeq =~ m,/,) {
		    $s_aeq_frac = Number::Fraction->new($s_aeq);
		} else {
		    $s_aeq_frac = Number::Fraction->new("$s_aeq/1");
		}
		$aev{$s_aeu} = $aev{$s} / $s_aeq_frac;
		my $this_mev = $aev{$s_aeu} * $sys_meq_frac;
		$mev{$s_aeu} = $this_mev;
#		warn "aev for $s_aeu = $aev{$s_aeu} = $this_mev$sys_meu\n";
	    }
	    foreach my $r (@{$rsteps{$s}}) {
		next if defined $aev{$r};
#		warn "resolving $r by reciprocal relationship with $s\n";
		foreach my $p (@{$steps{$r}}) {
		    if ($$p[1] eq $s) {
#			warn "found $r = $$p[0] $$p[1]\n";
			my($s_aeq,$s_aeu) = ($$p[0],$$p[1]);
			my $s_aeq_frac;
			if ($s_aeq =~ m,/,) {
			    $s_aeq_frac = Number::Fraction->new($s_aeq);
			} else {
			    $s_aeq_frac = Number::Fraction->new("$s_aeq/1");
			}
			$aev{$r} = $aev{$s_aeu} * $s_aeq_frac;
			my $this_mev = $aev{$r} * $sys_meq_frac;
			$mev{$r} = $this_mev;
#			warn "aev for $r = $aev{$r} = $this_mev$sys_meu\n";
		    }
		}
	    }
	}
	@known_steps = @newly_known_steps;
	goto DO_STEPS if $#known_steps >= 0;
    }
}

###########################################################

sub
render_mev {
    my $mev = shift;
    my($q,$u) = ($mev =~ /^([\d\/]*)([^\d].*)$/);
#    print STDERR "eval-ing '$q'\n";
    my $qf = eval($q);
    if ($u eq 'mm') {
	if ($qf > 1000000) {
	    $qf = convert($qf,'mm','km');
	    $u = 'km';
	} elsif ($qf > 1000) {
	    $qf = convert($qf,'mm','m');
	    $u = 'm';
	} elsif ($qf > 100) {
	    $qf = convert($qf,'mm','cm');
	    $u = 'cm';
	}
    } elsif ($u eq 'm^2') {
	if ($qf > 100000) {
	    $qf = convert($qf,'m^2','km^2');
	    $u = 'km^2';
	} elsif ($qf > 1000) {
	    $qf = convert($qf,'m^2','h-are');
	    $u = 'ha';
	} elsif ($qf < 0.01) {
	    $qf = convert($qf,'m^2','cm^2');
	    $u = 'cm^2';
	}
    } elsif ($u eq 'g') {
	if ($qf > 1000) {
	    $qf = convert($qf,'g','kg');
	    $u = 'kg';
#	} elsif ($qf < 1) {
#	    $qf = convert($qf,'g','mg');
#	    $u = 'mg';
	}
    } elsif ($u eq 'l') {
	if ($qf > 1000) {
	    $qf = convert($qf,'l','m^3');
	    $u = 'm^3';
	} elsif ($qf < 1) {
	    $qf = convert($qf,'l','ml');
	    $u = 'ml';
	}
    }
    my $i = sprintf("%.2f",$qf);
    $i =~ s/\.00//g;
    $u =~ s/\^2/&#xb2;/;
    $u =~ s/\^3/&#xb3;/;
    "$i$u";
}
