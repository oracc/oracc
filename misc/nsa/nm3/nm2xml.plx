#!/usr/bin/perl
use warnings; use strict;

use Number::Fraction;
use Math::Units qw(convert);

#### NO, STEVE, DO NOT MOVE THIS PRAGMAS BEFORE THE TWO use STATEMENTS!
#### UTF8 CAUSES WARNINGS FROM THOSE MODULES
use utf8; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my $ns_uri="http://oracc.org/ns/nsa/1.0";

my @at = qw/system end step code time 
	    place meb mev sysdet comh comm type 
	    nm newpage inst inst-system examples ex/;
my %at = ();
@at{@at} = ();

my @listable = qw/time place step/;
my %listable = ();
@listable{@listable} = ();

my %system = ();
my %step_map = ();

#my $mev_base;
#my $step_mev;
my $sys_meb = '';
my $sys_meq = '';
my $sys_meq_frac = '';
my $sys_meu = '';

my %aev = ();
my %mev = ();

print '<?xml version="1.0" encoding="utf-8"?>', "\n";
print "<systems xmlns=\"http://oracc.org/ns/nsa/1\"\n>";
while (<>) {
    next if /^\#/ || /^\s*$/;
    chomp;
    if (s/^\@(\S+)\s*//) {
	my $at = $1;
	s/^\s*(.*?)\s*$/$1/;
	if (exists $at{$at}) {
	    if ($at eq 'end') {
		if (/system/) {
		    process_system();
		} else {
		    nmwarn("bad '\@end' command: $_");
		}
	    } else {
		if (exists $listable{$at}) {
		    if ($at eq 'time' || $at eq 'place') {
			push @{$system{$at}}, $_;
		    } elsif ($at eq 'step') {
			push @{$system{$at}}, step_parse($_);
		    } else {
			nmwarn("unhandled listable field `$at'");
		    }
		} else {
		    if ($system{$at}) {
			nmwarn("\@$at can only be given once in each system");
		    } else {
			$system{$at} = $_;
		    }
		}
	    }
	} else {
	    nmwarn("unknown at-command '$at'");
	}
    } else {
	nmwarn("bad start of line");
    }
}
print '</systems>';

####################################################

sub
nmwarn {
    warn("$ARGV:$.: ", @_, "\n");
}

sub
process_system {
    build_step_mevs(%system);
    xml_printer(%system) unless $system{'type'} && $system{'type'} eq 'num';
    %system = ();
}

sub
step_parse {
    my($name,$aev,$form) = (/^(\S+)\s+(\S+)\s+(.+?)\s*$/);
    my($aeq,$aeu) = ('','');
    if ($aev =~ /\*/) {
#	    $aev = $aev{$step_ascii} || '';
#	    warn "no aev found for step $step_ascii\n"
#		unless defined $aev;
	($aeq,$aeu) = ($aev =~ /^(.*?)\*(.*)$/);
    }
    
    # We are going to reset aev during build_step_mevs
    if ($name) {
	return { name=>$name , aev=>$aev, aeu=>$aeu, aeq=>$aeq, form=>$form };
    } else {
	nmwarn("bad step syntax");
    }
}

sub
xml_printer {
    my %s = @_;
    my $sysdet = '';
    if ($s{'sysdet'}) {
	$sysdet=" det=\"$s{'sysdet'}\"";
    }
    my $type = $s{'type'} || 'met';
    print "<system title=\"$s{'system'}\" n=\"$s{'code'}\" type=\"$type\" meb=\"$s{'meb'}\" mev=\"$s{'mev'}\"\n>";
    foreach my $step (@{$s{'step'}}) {
	my $f = $$step{'form'};
	my $overt = '';
	if ($f =~ /\*\s*$/) {
	    $overt = " overt=\"no\"";
	}
	my $sex = '';
	my $sexcode = '*';
	if ($f =~ /^(S[ADP])/) {
	    $sexcode = ($1 eq 'SA' ? 'aš' : ($1 eq 'SD' ? 'diš' : 'pos'));
	}
	my $mev;
	my $mev_disp_str = '';
	if (defined $mev{$$step{'name'}}) {
	    $mev = $mev{$$step{'name'}};
	    $mev .= $sys_meu;
	    my $mev_disp = render_mev($mev);
	    $mev_disp_str = " mev=\"$mev\" mev-disp=\"$mev_disp\"";
	}

	if ($f =~ /^\#/) {
	    foreach my $g (split(/\|/,$f)) {
		$g =~ s/^.*?\((.*?)\).*$/$1/;
		my $aev = ($$step{'aev'} ? " aev=\"$$step{'aev'}\"" : '');
		my $aeq = ($$step{'aeq'} ? " aeq=\"$$step{'aeq'}\"" : '');
		my $aeu = ($$step{'aeu'} ? " aeu=\"$$step{'aeu'}\"" : '');
		print "<step n=\"$$step{'name'}\" utf=\"$g\" sex=\"$sexcode\"$overt$aev$aeq$aeu$mev_disp_str\n/>";
	    }
	} else {
	    my $aev = ($$step{'aev'} ? " aev=\"$$step{'aev'}\"" : '');
	    my $aeq = ($$step{'aeq'} ? " aeq=\"$$step{'aeq'}\"" : '');
	    my $aeu = ($$step{'aeu'} ? " aeu=\"$$step{'aeu'}\"" : '');
	    print "<step n=\"$$step{'name'}\" utf=\"$$step{'name'}\" sex=\"$sexcode\"$overt$aev$aeq$aeu$mev_disp_str\n/>";
	}
    }
    print "</system>";
}

#############################################################################

sub
step {
    my $arg = shift;
    my %step = ();
    @step{qw/name aev notation/} = ($arg =~ /^(\S+)\s+(\S+)\s+(.*?)\s*$/);
#    if (0) { # defined $args{'atf'}) {
#	$step_map{$step} = $args{'atf'};
#    }
    if (defined $step{'aev'}) {
	my $aev = $step{'aev'};
	if ($aev =~ /\*/) {
	    my $aev_rel = $step{'aev'};
#	    $aev = $aev{$step_ascii} || '';
#	    warn "no aev found for step $step_ascii\n"
#		unless defined $aev;
	    my($aeq,$aeu) = ($aev_rel =~ /^(.*?)\*(.*)$/);
	    $aeu = PSD::ATF::Unicode::gconv($aeu);
	    # this is a real hack...
	    $aev = "$aev\" aeq=\"$aeq\" aeu=\"$aeu";
	}
    }

    my $mev;
    my $mev_disp_str = '';
    if (defined $mev{$step{'name'}}) { # $step{$step_ascii}
	$mev = $mev{$step{'name'}}; # $step{$step_ascii}
	$mev .= $sys_meu;
	my $mev_disp = render_mev($mev);
	$mev_disp_str = " mev=\"$mev\" mev-disp=\"$mev_disp\"";
    }
    
    print "<step n=\"$step{'name'}\" aev=\"$step{'aev'}\""; # atf=\"$args{'atf'}\" 
    print "$mev_disp_str>";
#    do_seq($children{'step'});
    print "</step>";
}

sub
normalize_fraction {
    my $f = Number::Fraction->new($_[0]);
    sprintf $f;
}

#########################################################

sub
build_step_mevs {
    my %system = @_;
    my %steps = ();
    my %rsteps = ();
    my @known_steps = ();

    $sys_meb = $system{'meb'};
    my $mev = $system{'mev'};
    if ($mev =~ /^(.*?\d)([a-z]+(?:\^\d)?)$/) {
	$sys_meq = $1;
	$sys_meu = $2;
    } else {
	return; # it's a numerical equivalence so nothing to do
    }

    foreach my $s (@{$system{'step'}}) {
	my $n = $$s{'name'};
	my($aeq,$aeu) = ($$s{'aeq'}, $$s{'aeu'});
	push @{$steps{$n}}, [ $aeq , $aeu ];
	push @{$rsteps{$aeu}}, $n;
#	    push @{$rsteps{$n}}, $aeu;
#	    warn "pushed on n=$n; aeq = $aeq; aeu = $aeu\n";
#	    warn "pushed on r=$aeu; $n\n";
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
		warn "aev for $s_aeu = $aev{$s_aeu} = $this_mev$sys_meu\n";
	    }
	    foreach my $r (@{$rsteps{$s}}) {
		next if defined $aev{$r};
		warn "resolving $r by reciprocal relationship with $s\n";
		foreach my $p (@{$steps{$r}}) {
		    if ($$p[1] eq $s) {
			warn "found $r = $$p[0] $$p[1]\n";
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
			warn "aev for $r = $aev{$r} = $this_mev$sys_meu\n";
		    }
		}
	    }
	}
	@known_steps = @newly_known_steps;
	goto DO_STEPS if $#known_steps >= 0;
    }
    
    foreach my $s (@{$system{'step'}}) {
	if ($aev{$$s{'name'}}) {
	    $$s{'aev'} = "$aev{$$s{'name'}}";
	    $$s{'aev'} .= "/1" unless $$s{'aev'} =~ m,/,;
	}
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

1;

