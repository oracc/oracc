#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; binmode STDOUT, ':utf8';
binmode STDERR, ':utf8';

my $inst_field = 0;
my %prunes = ();
open(P,'01bld/lists/prune.lst');
my @p = (<P>); chomp(@p);
close(P);
@prunes{@p} = ();

my @sig = <01bld/*.sig>;

foreach my $s (@sig) {
    prune_sigs($s);
}

######################################################################

sub prune {
    my @p = split(/\s+/, $_[0]);
    my @newp = ();
    for (my $i = 0; $i <= $#p; ++$i) {
	my $p = $p[$i];
	$p =~ s/\..*$//;
	push @newp, $p[$i] unless exists $prunes{$p};
    }
    join(' ', @newp);
}

sub prune_sigs {
    my $s = shift;
    open(S, $s) || die "l2p1-prune-sigs.plx: no permission to read $s\n";
    my @s = (<S>); chomp(@s);
    close(S);
    open(S, ">$s") || die "l2p1-prune-sigs.plx: no permission to write $s\n";
    do {
	set_inst_fields() and next if /^\@fields/;
	print "$_\n" and next if /^\@(proj|lang|name)/;
    } while (/^\@(proj|lang|name|fields)/);
    warn "l2p1-prune-sigs.plx: $s: no insts field\n" and return unless $inst_field;
    foreach (@s) {
	my @f = split(/\t/,$_);
	$f[$inst_field] = prune($f[$inst_field]);
	print join("\t", @f), "\n";
    }
}

sub set_inst_field {
    s/^\@fields\s+//;
    my @f = split(/\s+/,$_);
    
}
1;
