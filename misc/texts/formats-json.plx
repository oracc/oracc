#!/usr/bin/perl
use warnings; use strict;

my %have = ();
#my $prefix = "/home/oracc/rimanum/";
my $prefix='';

@{$have{'atf'}} = `cat ${prefix}01bld/lists/withatf`; chomp @{$have{'atf'}};
@{$have{'xtf'}} = `cat ${prefix}01bld/lists/xtfindex.lst`; chomp @{$have{'xtf'}};
@{$have{'lem'}} = `cat ${prefix}01bld/lists/withlem`; chomp @{$have{'lem'}};

open(TR,"${prefix}01bld/lists/have-xtr.tab");
while (<TR>) {
    chomp;
    my($pqx,$trs) = (/^(.*)\t(.*?)$/);
    foreach my $tr (split(/\s+/, $trs)) {
	push @{$have{"tr-$tr"}}, $pqx;
    }
}
close(TR);

my $first_row = 0;
print "\t\"formats\": {\n";
foreach my $h (sort keys %have) {
    print ",\n" if $first_row++;
    print "\t\t\"$h\": [ ", join(',', map { s/^.*?://; "\"$_\"" } @{$have{$h}} ), " ]";
}
print "\n\t}";

1;
