#!/usr/bin/perl
use warnings; use strict;

my $max_n = 0;
my $max_q = 0;
my %ns = (
    c  =>'http://oracc.org/ns/cdf/1.0',
    g  =>'http://oracc.org/ns/gdl/1.0',
    n  =>'http://oracc.org/ns/norm/1.0',
    syn=>'http://oracc.org/ns/syntax/1.0',
    x  =>'http://oracc.org/ns/xtf/1.0',
    xtr=>'http://oracc.org/ns/xtr/1.0',
    xh =>'http://www.w3.org/1999/xhtml',
    xml=>'http://www.w3.org/XML/1998/namespace',
    );
my %a = ();
my @a = ();
my %e = ();
my @e = ();

open(IN,'cdf_rnc.c');
while (<IN>) {
    process_attr($_);
    process_elem($_);
}
close(IN);

@a = sort keys %a;
@e = sort keys %e;

++$max_n;
++$max_q;

open(H,">xmlnames.h");
print H <<H;
#ifndef _XMLNAMES_H
#define _XMLNAMES_H
#define MAX_A_NAME  $max_n
#define MAX_QA_NAME $max_q
struct xname
{
  char pname[MAX_A_NAME];
  char qname[MAX_QA_NAME];
};
enum a_type
{
H
foreach my $n (sort keys %ns) {
    print H "  n_$n,\n";
}
my $i = 0;
foreach my $a (@a) {
    my $a2 = $a;
    $a2 =~ tr/:-/__/;
    print H "  a_$a2,\n";
}
print H <<H;
  a_enum_top
};
struct attr
{
  char *valpair[2];
  char *renpair[2];
};
enum e_type
{
H
$i = 0;
foreach my $e (@e) {
    my $e2 = $e;
    $e2 =~ tr/:-/__/;
    print H "  e_$e2,\n";
}
print H <<H;
  e_enum_top
};

struct nstab { enum a_type prefix; const char *ns; };

extern struct attr abases[];
extern struct nstab nstab[];
extern struct xname anames[];
extern struct xname enames[];

#endif /*_XMLNAMES_H*/
H
close(H);

open(C,">xmlnames.c");
print C <<C;
#ifndef NULL
#define NULL (char *)0
#endif
#include "xmlnames.h"
struct nstab nstab[] = {
C
foreach my $n (sort keys %ns) {
    print C "  { n_$n, \"$ns{$n}\" },\n";
}
print C <<C;
};

struct xname anames[] =
{
C
foreach my $x (sort keys %ns) {
    print C "  { \"xmlns:$x\", \"xmlns:$x\" },\n";
}
for (my $i = 0; $i <= $#a; ++$i) {
    my $a = $a[$i];
    print C "  { \"$a\", \"$a{$a}\" },\n";
}
print C <<C;
};
C

print C <<C;
struct attr abases[] =
{
C
my $top_a_n = (scalar keys %ns) + $#a;
for (my $i = 0; $i <= $top_a_n; ++$i) {
    print C "  { { anames[$i].qname,NULL } , { anames[$i].pname,NULL } },\n";
}
print C <<C;
};
C

print C <<C;
struct xname enames[] =
{
C
for (my $i = 0; $i <= $#e; ++$i) {
    my $e = $e[$i];
    print C "  { \"$e\", \"$e{$e}\" },\n";
}
print C <<C;
};
C
close(C);

sub
a_nsify {
    my $n = shift;
    if ($n =~ s/^(.*?)://) {
	$n = "$ns{$1}:$n";
    }
    $n;
}

sub
e_nsify {
    my $n = shift;
    if ($n =~ s/^(.*?)://) {
	$n = "$ns{$1}:$n";
    } else {
	$n = "$ns{'x'}:$n";
    }
    $n;
}

sub
process_attr {
    my $aline = shift;
    chomp $aline;
    if ($aline =~ /attribute\s+(\S+)\s*/) {
	my $n = $1;
	$a{$n} = a_nsify($n);
	$max_n = length($n) if length($n) > $max_n;
	$max_q = (length($a{$n})+length($n)) 
	    if (length($a{$n})+length($n)) > $max_q;
    }
}

sub
process_elem {
    my $eline = shift;
    chomp $eline;
    if ($eline =~ /element\s+(\S+)\s*/) {
	my $n = $1;
	$e{$n} = e_nsify($n);
	$max_n = length($n) if length($n) > $max_n;
	$max_q = (length($e{$n})+length($n)) 
	    if (length($e{$n})+length($n)) > $max_q;
    }
}

1;
