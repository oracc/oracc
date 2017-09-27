#!/usr/bin/perl
use warnings; use strict;
use Getopt::Long;

my %forms = ();
my $table = '';

GetOptions (
    'table:s'=>\$table,
    );

if ($table) {
    fail("no such table file '$table'") unless -r $table;
    load_table($table);
}

open(X, ">xforms.tab") || die "can't open xforms.tab\n";

while (<>) {
    if (/^\s*$/ || /^[&\@\$]/ || /^[:=]/) {
	print;
	next;
    } elsif (/^\#/) {
	print;
	next;
    } else {
	print; # get rid of the line before hacking it up
	s/^\S*\s+//;
	s/\s$//;
	s/<<.*?>>//g;
	s/--/-/;
	tr/<>[]\#\*\!\?//d;
	s/\s+/ /g;
	my @line = grep(defined&&length , split(/\s+/, $_));
	my @lem = ();
	for (my $i = 0; $i <= $#line; ++$i) {
	    my $w = $line[$i];
	    if ($forms{$w}) {
		push @lem, $forms{$w};
		print X "X\t$w\n" if $forms{$w} eq 'X';
#		warn "X\n" if $forms{$w} eq 'X';
	    } elsif ($w =~ /\.\.|-x|x-/) {
		push @lem, 'u';
	    } else {
		push @lem, 'X';
		print X "X\t$w\n";
#		warn "X\n";
	    }
	}
	print "#lem: ", join('; ', @lem), "\n";
    }
}

close(X);

1;

#######################################################

sub
bad {
    print STDERR 'atfquicklem.plx: ', join('', @_), "\n";
}

sub
fail {
    bad(@_);
    die "atfquicklem.plx: failed\n";
}

sub
load_table {
    my $t = shift;
    open(T,$t) || die;
    while (<T>) {
	next if /^\#/ || /^\s*$/;
	my($form,$lem) = (/^(\S+)\t(.*?)$/);
	die unless $form && $lem;
	$forms{$form} = $lem;
    }
    close(T);
}

1;
