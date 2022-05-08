#!/usr/bin/perl
use warnings; use strict;
use Getopt::Long;
use Pod::Usage;
my $ail = '';
my $atf = 0;
my $lst = '';
my $jdf = 0;
my $project = '';
my $qualified = 0;
my $sort = 0;
GetOptions(
	   'ail:s'=>\$ail,
	   'atf'=>\$atf,
           'lst:s'=>\$lst,
	   'jdf'=>\$jdf,
           'project:s'=>\$project,
           'qualified'=>\$qualified,
	   'sort'=>\$sort,
	   ) || pod2usage(1);

my @files = ();
my %known = ();
my @order = ();

if ($lst) {
    open(L,$lst) || die;
    @files = (<L>);
    close(L);
} elsif ($#ARGV >= 0) {
    @files = @ARGV;
}

if ($#files >= 0) {
    foreach my $f (@files) {
	if ($f =~ /\.ods$/) {
	    open(F,"ods2atf.sh -s $f |") || die;
	} else {
	    open(F,$f) || die "$0: failed to open $f\n";
	}
	while (<F>) {
	    next if $atf && !/^(?:\x{ef}\x{bb}\x{bf})?&/;
	    while (s/([PQX][0-9]{6,}[a-z]*)//) {
		my $pq = $1;
		next if $known{$pq}++;
		push @order, $pq;
	    }
	}
	close(F);
    }
} else {
    while (<>) {
	next if $atf && !/^(?:\x{ef}\x{bb}\x{bf})?&/;
	while (s/([PQX][0-9]{6,}[a-z]*)//) {
	    my $pq = $1;
	    next if $known{$pq}++;
	    push @order, $pq;
	}
    }
}

my @pq = ();

if ($sort) {
    @pq = sort @order;
} else {
    @pq = @order;
}

if ($jdf) {
    foreach (@pq) {
	print "<item xml:id=\"$_\"/>\n";
    }
} else {
    my $ailtab = '';
    if ($ail) {
	$ailtab = "\t = ";
	print $ail, "\n$ailtab";
	print join("\n$ailtab", @pq), "\n";
    } elsif ($project) {
	foreach (@pq) {
	    print "$project:$_\n";
	}
    } else {
	print join("\n", @pq), "\n";
    }
}

1;

__END__

=head1 NAME

atfgrabpq.plx -- grab the PQ ids from STDIN and print them to STDOUT

=head1 SYNOPSIS

atfgrabpq.plx [file]

=head1 OPTIONS

=over

=item B<-ail STRING>

Format the PQ's as an ASCII Item List; this enables the output
to be piped to ail2xil.plx to create an XML Item List.

STRING is a header to go at the top of your item list, e.g., 
-ail 'Ur III Drehem'.

=item B<-atf>

Tell atfgrabpq that this is an atf archive; lines not starting
with '&' will not be scanned for PQ ids.

=item B<-jdf>

Format the PQ's as items for inclusion in a JDF file.

=item B<-sort>

Sort the PQ's (default is to retain order in input).

=back

=head1 DESCRIPTION

atfgrabpq is a tool for extracting PQ ids from the input and printing
    them to STDOUT.

=head1 AUTHOR

Steve Tinney (stinney@sas.upenn.edu)

=head1 COPYRIGHT

Copyright(c) Steve Tinney 2005.

Released under the Gnu General Public License
(http://www.gnu.org/copyleft/gpl.html).
