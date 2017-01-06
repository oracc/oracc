#!/usr/bin/perl
use warnings; use strict; use open 'utf8';

my $full = 1;
my %l = ();
my $project = shift @ARGV;

$full = 0 if $project;

exit 0 unless -s "01bld/normlinks.tab";

open L, "01bld/normlinks.tab";
while (<L>) {
    chomp;
    my($lft,$rel,$rgt) = split(/\t/,$_);
    if ($full) {
        if ($rel eq 'has-witness') {
            push @{${$l{$lft}}{'has-witness'}}, $rgt;
            push @{${$l{$rgt}}{'is-witness-of'}}, $lft;
        } elsif ($rel eq 'parallels') {
            push @{${$l{$lft}}{'parallels'}}, $rgt;
            push @{${$l{$rgt}}{'parallels'}}, $lft;
        }
    } else {
	if ($rel eq 'has-witness') {
	    if ($lft =~ /^$project:(.*?)$/) {
		push @{${$l{$1}}{'has-witness'}}, $rgt;
	    }
	    if ($rgt =~ /^$project:(.*?)$/) {
		push @{${$l{$1}}{'is-witness-of'}}, $lft;
	    }
	} elsif ($rel eq 'parallels') {
	    if ($lft =~ /^$project:(.*?)$/) {
		push @{${$l{$1}}{'parallels'}}, $rgt;
	    }
	    if ($rgt =~ /^$project:(.*?)$/) {
		push @{${$l{$1}}{'parallels'}}, $lft;
	    }	
	}
    }
}
close L;

my $count = 0;
open(J, ">01bld/witnesses.json");
print J "\t\"witnesses\": {\n";
foreach my $id (sort keys %l) {
    print J "\t\t},\n" if $count++;
    print J "\t\t\"$id\": {\n";
    my %data = %{$l{$id}};
    my $ntypes = 0;
    foreach my $type (sort keys %data) {
	my $str = join(',', map { "\"$_\"" } @{$data{$type}});
	print J ",\n" if $ntypes++;
	print J <<EOJ;
			"$type": [ $str ]
EOJ
    }
}
print J "\t\t}\n\t}\n";
close(J);

1;
