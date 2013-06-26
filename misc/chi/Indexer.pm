package ORACC::CHI::Indexer;
use warnings; use strict;

sub
index {
    my $chis = shift;
    my %ix = ();
    my @pending = ();

    foreach my $chi (keys %$chis) {
	next if $chi eq '@prefixes';
	my $chihash = $$chis{$chi};
	${$ix{$chi}}{$chihash} = $chihash;
	$$chihash{'head'} = $chi;
	foreach my $k (keys %$chihash) {
	    next if $k eq 'head';
	    if ($k eq '_terminal') {
		$ix{$chi,'_terminal'} = 1;
		my $v = $$chihash{$k};
		if ($v ne '-') {
		    my @bits = split(/\.+-?/, $v);
		    shift @bits; # don't index head because we did already
		    foreach my $b (@bits) {
			push @pending, [ $b, $chihash ];
		    }
		}
	    } else {
		my @x = @{$$chihash{$k}};
		foreach my $x (@x) {
		    ${$ix{$x}}{$chihash} = $chihash;
		}
	    }
	}
    }

    foreach my $p (@pending) {
	my($c,$h) = @$p;
	${$ix{$c}}{$h} = $h;
    }

    return { %ix }
}

sub
is_known {
    my($c,$x) = @_;
    $$x{$c};
}

sub
is_terminal {
    my($c,$x) = @_;
    $$x{$c,'_terminal'};
}

sub
terminal_of {
    my($c,$x) = @_;
    if ($$x{$c,'_terminal'}) {
	return $c;
    } else {
	my @chirefs = keys %{$$x{$c}};
	my @t = ();
	foreach my $cc (@chirefs) {
	    my $ccref = ${$$x{$c}}{$cc};
	    if ($$ccref{'_terminal'}) {
		push @t, $$ccref{'head'};
	    }
	}
	if ($#t == 0) {
	    return $t[0];
	} else {
	    "bad terminal: $#t";
	}
    }
}

1;
