#!/usr/bin/perl
use warnings; use strict;

my $max_n = 0;
my $max_q = 0;
my %ns = (
    c  =>'http://oracc.org/ns/cbd/2.0',
    g  =>'http://oracc.org/ns/gdl/1.0',
    n  =>'http://oracc.org/ns/norm/1.0',
    note=>'http://oracc.org/ns/note/1.0',
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

open(IN,'cbd_rnc.c');
while (<IN>) {
    process_attr($_);
    process_elem($_);
}
close(IN);

@a = sort keys %a;
@e = sort keys %e;

++$max_n;
++$max_q;

open(H,">cbd_xmlnames.h");
print H <<H;
#ifndef _CBD_XMLNAMES_H
#define _CBD_XMLNAMES_H
#define CBD_MAX_A_NAME  $max_n
#define CBD_MAX_QA_NAME $max_q
struct cbd_xname
{
  char pname[CBD_MAX_A_NAME];
  char qname[CBD_MAX_QA_NAME];
};
enum cbd_a_type
{
H
foreach my $n (sort keys %ns) {
    print H "  cbd_n_$n,\n";
}
my $i = 0;
foreach my $a (@a) {
    my $a2 = $a;
    $a2 =~ tr/:-/__/;
    print H "  cbd_a_$a2,\n";
}
print H <<H;
  cbd_a_enum_top
};
struct cbd_attr
{
  char *valpair[2];
  char *renpair[2];
};
enum cbd_e_type
{
H
$i = 0;
foreach my $e (@e) {
    my $e2 = $e;
    $e2 =~ tr/:-/__/;
    print H "  e_$e2,\n";
}
print H <<H;
  cbd_e_enum_top
};

struct cbd_nstab { enum cbd_a_type prefix; const char *ns; };

extern struct cbd_attr cbd_abases[];
extern struct cbd_nstab cbd_nstab[];
extern struct cbd_xname cbd_anames[];
extern struct cbd_xname cbd_enames[];

#endif /*_CBD_XMLNAMES_H*/
H
close(H);

open(C,">cbd_xmlnames.c");
print C <<C;
#ifndef NULL
#define NULL (char *)0
#endif
#include "cbd_xmlnames.h"
struct cbd_nstab cbd_nstab[] = {
C
foreach my $n (sort keys %ns) {
    print C "  { cbd_n_$n, \"$ns{$n}\" },\n";
}
print C <<C;
};

struct cbd_xname cbd_anames[] =
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
struct cbd_attr cbd_abases[] =
{
C
my $top_a_n = (scalar keys %ns) + $#a;
for (my $i = 0; $i <= $top_a_n; ++$i) {
    print C "  { { cbd_anames[$i].qname,NULL } , { cbd_anames[$i].pname,NULL } },\n";
}
print C <<C;
};
C

print C <<C;
struct cbd_xname cbd_enames[] =
{
C
for (my $i = 0; $i <= $#e; ++$i) {
    my $e = $e[$i];
    print C "  { \"$e\", \"$e{$e}\" },\n";
}
    print C "  { \"\", \"\" },\n";
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
	$n = "$ns{'c'}:$n";
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
