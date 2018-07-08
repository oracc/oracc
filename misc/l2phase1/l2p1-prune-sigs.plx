#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; binmode STDOUT, ':utf8';
binmode STDERR, ':utf8';

my $inst_field = 0;
my %prunes = ();
open(P,'01bld/lists/prune.lst');
my @p = (<P>); chomp(@p);
close(P);
@prunes{@p} = ();
my $pruned = 0;

my @sig = <01bld/*.sig>;

foreach my $s (@sig) {
    $pruned = 0;
    prune_sigs($s);
    if ($pruned) {
	warn "l2p1-prune-sigs.plx: pruned $pruned bad refs from $s\n";
	system 'mv', "$s.new", $s;
    }
}

######################################################################

sub prune {
    my @p = split(/\s+/, $_[0]);
    my @newp = ();
    for (my $i = 0; $i <= $#p; ++$i) {
	my $p = $p[$i];
	$p =~ s/^.*?://;
	$p =~ s/\..*$//;
	if (exists $prunes{$p}) {
	    ++$pruned;
	} else {
	    push @newp, $p[$i];
	}
    }
    join(' ', @newp);
}

sub prune_sigs {
    my $s = shift;
    open(S, $s) || die "l2p1-prune-sigs.plx: no permission to read $s\n";
    my @s = (<S>); chomp(@s);
    close(S);
    my $i;
    open(S, ">$s.new") 
	|| die "l2p1-prune-sigs.plx: no permission to write $s\n";
    for (my $i = 0; $i <= $#s; ++$i) {
	if ($s[$i] =~ /^\@fields/) {
	    print S $s[$i], "\n";
	    $inst_field = set_inst_field($s[$i]);
	} elsif ($s[$i] =~ /^\@(?:proj|lang|name)/ || $s[$i] =~ /^\s*$/) {
	    print S $s[$i], "\n";
	} else {
	    last;
	}
    }
    while ($i++ <= $#s) {
	next unless $s[$i];
	my @f = split(/\t/,$s[$i]);
	if ($f[$inst_field]) {
	    $f[$inst_field] = prune($f[$inst_field]);
	} else {
	    warn "$s:$.: inst_field=$inst_field: no inst in: $s[$i]\n";
	}
	print S join("\t", @f), "\n";
    }
}

sub set_inst_field {
    $_[0] =~ s/^\@fields\s+//;
    my @f = split(/\s+/,$_[0]);
    for (my $i = 0; $i <= $#f; ++$i) {
	if ($f[$i] eq 'inst') {
	    return $i;
	}
    } 
}

1;
