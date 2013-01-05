#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use Getopt::Long;

my $curr_id = undef;
my $order = '';
my @order = ();
my %texts = ();

GetOptions('order:s'=>\$order,
    );

open(O,$order) || die "atforder.plx: order file `$order' not readable\n";
@order = (<O>);
chomp @order;
close(O);

@order = uniq(@order);

while (<>) {
    s/^\x{ef}\x{bb}\x{bf}//; # remove BOMs
    if (/^\&(\S+)/) {
	$curr_id = $1;
	if ($texts{$curr_id}) {
	    @{$texts{$curr_id}} = ();
	}
	push @{$texts{$curr_id}}, $_;
    } elsif (!defined $curr_id) {
	die "atforder.plx: ATF file has junk before first &-line\n";
    } else {
	push @{$texts{$curr_id}}, $_;
    }
}

foreach my $id (@order) {
    if ($texts{$id}) {
	print @{$texts{$id}};
	delete $texts{$id};
    } else {
	print "\&$id\n#(no transliteration)\n\n";
	warn("$id\n");
    }
}

##########################################################################

sub
uniq {
    my %seen = ();
    my @new_order = ();
    foreach (@_) {
	push @new_order, $_ unless $seen{$_}++;
    }
    @new_order;
}

1;
