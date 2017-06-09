#!/usr/bin/perl
use warnings; use strict;

my %have = ();
my $prefix='';

if (-r "${prefix}01bld/lists/withatf") {
    @{$have{'atf'}} = `cat ${prefix}01bld/lists/withatf`; chomp @{$have{'atf'}};
}
if (-r "${prefix}01bld/lists/xtfindex.lst") {
    @{$have{'xtf'}} = `cat ${prefix}01bld/lists/xtfindex.lst`; chomp @{$have{'xtf'}};
}
if (-r "${prefix}01bld/lists/withlem") {
    @{$have{'lem'}} = `cat ${prefix}01bld/lists/withlem`; chomp @{$have{'lem'}};
}

if (-r "${prefix}01bld/lists/have-xtr.tab") {
    open(TR,"${prefix}01bld/lists/have-xtr.tab");
    while (<TR>) {
	chomp;
	my($pqx,$trs) = (/^(.*)\t(.*?)$/);
	foreach my $tr (split(/\s+/, $trs)) {
	    push @{$have{"tr-$tr"}}, $pqx;
	}
    }
    close(TR);
}

my $first_row = 0;
print "\t\"formats\": {\n";
foreach my $h (sort keys %have) {
    print ",\n" if $first_row++;
    print "\t\t\"$h\": [ ", join(',', map { s/^.*?://; "\"$_\"" } @{$have{$h}} ), " ]";
}
print "\n\t}";

1;
