package ORACC::CBD::Senses;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/senses_collect senses_merge senses_string/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;
use Data::Dumper;

my @common1 = qw/a an in is of or the to/;
my %common1 = (); @common1{@common1} = ();

my @common2 = qw/and be by designation down for form make off on out part status type up/;
my %common2 = (); @common2{@common2} = ();

sub senses_collect {
    my @cbd = @_;
    my %s = ();
    my $curr_entry = '';
    my @s = ();
    for (my $i = 0; $i <= $#cbd; ++$i) {
	if ($cbd[$i] =~ /^\@entry\S*\s+(.*?)\s*$/) {
	    $curr_entry = $1;
	} elsif ($cbd[$i] =~ /^\@sense/) {
	    push @s, $cbd[$i];
	} elsif ($cbd[$i] =~ /^\@end\s+entry/) {
	    $s{$curr_entry} = [ @s ];
	    @s = ();
	}
    }
    %s;
}

sub senses_merge {
    my($i,$b) = @_;
    my %map = ();
    my @newb = (@$b);
    my %b = index_senses(@$b);
#    print Dumper \%b;
    foreach my $s (@$i) {
	my @matches = ();
	# does s occur in @b?
	foreach my $b (@$b) {
	    if ($b =~ /$s/ || $s =~ /$b/) {
		push @matches, $b;
	    }
	}
	if ($#matches == 0) {
	    map_sense('1', $s, $matches[0]);	    
	    next;
	} else {
	    my %i = index_senses($s);
	    my %bs = ();
	    # create a hash where they keys are the index into @$b
	    # and the values are the number of tokens that occur in
	    # each of the keys--this means that the highest token
	    # count is the best match
	    foreach my $i (keys %i) {
		if ($b{$i}) {
		    foreach my $bs (@{$b{$i}}) {			
			++$bs{"#$bs"};
		    }
		}
	    }
	    if (scalar keys %bs) {
		my @m = sort { $bs{$b} <=> $bs{$a} || $a cmp $b } keys %bs;
		my $index = $m[0]; $index =~ s/^#//;
		map_sense('2', $s, ${$b}[$index]);
	    } else {
		$s =~ s/sense/sense+/;
		warn "Senses[3]: adding $s\n";
		push @newb, $s;
	    }
	}	
    }
    [ @newb ];
}

sub map_sense {
    my($code,$in,$base) = @_;
    warn "Senses[$code]: mapping $in => base $base\n";
}

sub senses_string {
    join ("\n\t", @{$_[0]});
}

sub index_senses {
    my %t = ();
    for (my $i = 0; $i <= $#_; ++$i) {
	my $s = $_[$i];
	$s =~ s/^\@sense\S*\s+\S+\s+//;
	$s =~ s/^\S+\s+\S+\s+//; # remove sense-id and POS
	$s =~ tr/a-zA-Z0-9 //cd; # reduce to alphanumerics
	foreach my $t (split(/\s/,$s)) {
	    push @{$t{$t}}, $i; # register token $t as occurring in sense $i
	}
    }
    %t = de_common(%t);
}

sub de_common {
    my %t = @_;
    my %n = ();
    foreach my $t (keys %t) {
	$n{$t} = $t{$t} unless exists($common1{$t}) || exists($common2{$t});
    }
    if (scalar(keys(%n)) == 0) {
	foreach my $t (keys %t) {
	    $n{$t} = $t{$t} unless exists($common1{$t});
	}	
    }
    if (scalar(keys(%n)) == 0) {
	%t;
    } else {
	%n;
    }
}

1;
