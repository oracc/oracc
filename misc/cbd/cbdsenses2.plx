#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

use String::Similarity::Group qw/groups_hard/;

use Data::Dumper;

my $multi = 0;
my $first_sense = '';
my @senses = ();
my %seen = ();
my @entry = ();

while (<>) {
    chomp;
    push @entry, $_;
    if (/^\@end\s+entry/) {
	edit_and_dump(@entry);
	@entry = ();
    }
}

edit_and_dump(@entry);

sub edit_and_dump {
    my @senses = ();
    
    for (my $i = 0; $i <= $#entry; ++$i) {
	if ($entry[$i] =~ /^$acd_rx\@sense/) {
	    push @senses, [ $i , $entry[$i] ];
	}
    }
    my @nsenses = edit_senses(@senses);
#    print Dumper \@nsenses;
    foreach my $s (@nsenses) {
	next unless $s;
#	print "dollar-s= ", Dumper \$s;
	my($i,$m) = @{$s};
	$entry[$i] = $m;
    }
    print join("\n", @entry), "\n";
}

sub edit_senses {
    my @s = @_;
    my %m = ();
    my %order = ();

    foreach my $s (@s) {
	my($i,$m) = @$s;
	$m =~ s/^\S+\s+(\S+)\s+//;
	my $pos = $1;
	$m = trim_mng($m);
	$m = "$m $pos";
	push @{$m{$m}}, $s;
	$order{$m} = $$s[0] unless defined $order{$m};
    }

    my @m = keys %m;
    my @g = groups_hard(0.8,\@m);

#    print Dumper \@g;

    my %g = ();
    my %keep = ();
    my %drop = ();

    # index the grouped key meanings by key
    foreach my $g (@g) {
	my $gkey = join(':',@$g);
	foreach my $gm (@$g) {
	    $keep{$gm} = $gkey;
	}
    }
#    print Dumper \%keep;

    # now iterate over the meanings and keep or drop them
    my @new_senses = ();
    my %g_seen = ();
    foreach my $m (sort { $order{$a} <=> $order{$b} } keys %m) {
	if ($keep{$m}) {
	    # this is a candidate to keep; find its gkey and see if we found a match for that yet;
	    # if not, use this candidate and set the seen flag for its key
	    if ($g_seen{$keep{$m}}++) {
		foreach my $s (@{$m{$m}}) {
		    $$s[1] =~ s/^/##/;
		    push @new_senses, $s;
		}
	    } else {
		my @s = @{$m{$m}};
		if ($#s > 0) {
		    @s = sort { length($$b[1]) <=> length($$a[1]) } @s;
		}
		push @new_senses, $s[0]; shift @s;
		foreach my $s (@s) {
		    $$s[1] =~ s/^/##/;
		    push @new_senses, $s;
		}
	    }
	} else {
	    my @s = @{$m{$m}};
	    if ($#s > 0) {
		@s = sort { length($$b[1]) <=> length($$a[1]) } @s;
	    }
	    push @new_senses, $s[0]; shift @s;
	    foreach my $s (@s) {
		$$s[1] =~ s/^/##/;
		push @new_senses, $s;
	    } 
	}
    }
#    print Dumper @new_senses;
    
    @new_senses;
}

sub trim_mng {
    my $t = shift;
    $t =~ s/^an?\s+//;
    $t =~ s/^to\s+//;
    $t =~ s/^\(to be\)\s+//;
    $t;
}


1;
