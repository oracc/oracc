#!/usr/bin/perl
use warnings; use strict;

use Getopt::Long;

my $allns = 0; # use -a to output all ns in the nstab
my $prefix = '';

GetOptions(
    allns=>\$allns, 
    'prefix:s'=>\$prefix,
    );

die "$0: must give prefix with -prefix option\n" unless $prefix;

my $cbd = "cbd_$prefix";
my $CBD = "\U$cbd";

#my $max_n = 0;
#my $max_q = 0;
my %ns = (
    xml=>'http://www.w3.org/XML/1998/namespace',
    );
my %zns = (
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

my %ns_omit = (
    'http://www.w3.org/XML/1998/namespace'=>1,
    'http://relaxng.org/ns/compatibility/annotations/1.0'=>1
);

my %a = ();
my @a = ();
my %e = ();
my @e = ();

my $ns = '';

#open(IN,"${cbd}_rnc.c");
open(IN, "$prefix.xnn") || die;
while (<IN>) {
    s/\s*$//; chomp;
    if (/^ns=(.*)$/) {
	$ns = $1;
    } elsif (/^xmlns:(.*?)=(.*)$/) {
	$ns{$1} = $2;
    } elsif (/^attribute=(.*)$/) {
	process_attr($1);
    } elsif (/^element=(.*)$/) {
	process_elem($1);
    } else {
	warn("$0: unprocessed line: $_\n");
    }
}
close(IN);

@a = sort keys %a;
@e = sort keys %e;

#++$max_n;
#++$max_q;

open(H,">${cbd}_xnn.h");
print H <<H;
#ifndef _${CBD}_XNN_H
#define _${CBD}_XNN_H
enum ${cbd}_a_type
{
H
foreach my $n (sort keys %ns) {
    print H "  ${cbd}_n_$n,\n";
}
my $i = 0;
foreach my $a (@a) {
    my $a2 = $a;
    $a2 =~ tr/:-/__/;
    print H "  ${cbd}_a_$a2,\n";
}
print H <<H;
  ${cbd}_a_enum_top
};

enum ${cbd}_e_type
{
H
$i = 0;
foreach my $e (@e) {
    my $e2 = $e;
    $e2 =~ tr/:-/__/;
    print H "  ${cbd}_e_$e2,\n";
}
print H <<H;
  ${cbd}_e_enum_top
};

extern struct xnn_attr ${cbd}_abases[];
extern struct xnn_nstab ${cbd}_nstab[];
extern struct xnn_xname ${cbd}_anames[];
extern struct xnn_xname ${cbd}_enames[];

#endif /*_${CBD}_XMLNAMES_H*/
H
close(H);

open(C,">${cbd}_xnn.c");
print C <<C;
#ifndef NULL
#define NULL (char *)0
#endif
#include "xnn.h"
#include "${cbd}_xnn.h"
struct xnn_nstab ${cbd}_nstab[] = {
C
foreach my $n (sort keys %ns) {
    print C "  { ${cbd}_n_$n, \"$n\", \"$ns{$n}\" },\n"
	unless $allns || exists($ns_omit{$ns{$n}});
}
print C "  { ${cbd}_a_enum_top, NULL, NULL },\n";
print C <<C;
};

struct xnn_xname ${cbd}_anames[] =
{
C
foreach my $x (sort keys %ns) {
    print C "  { \"xmlns:$x\", \"xmlns:$x\" },\n";
}
for (my $i = 0; $i <= $#a; ++$i) {
    my $a = $a[$i];
    print C "  { \"$a\", \"$a{$a}\" },\n";
}
    print C "  { NULL, NULL },\n";
print C <<C;
};
C

print C <<C;
struct xnn_attr ${cbd}_abases[] =
{
C
my $top_a_n = (scalar keys %ns) + $#a;
for (my $i = 0; $i <= $top_a_n; ++$i) {
    #    print C "  { { ${cbd}_anames[$i].qname,NULL } , { ${cbd}_anames[$i].pname,NULL } },\n";
    my $a = $a[$i] || '';
    my $ap = $a{$a} || '';
    if ($a) {
	print C "  { { \"$a\",NULL } , { \"$ap\",NULL } },\n";
    }
}
print  C "  { { NULL,NULL } , { NULL,NULL } },\n";
print C <<C;
};
C

print C <<C;
struct xnn_xname ${cbd}_enames[] =
{
C
for (my $i = 0; $i <= $#e; ++$i) {
    my $e = $e[$i];
    print C "  { \"$e\", \"$e{$e}\" },\n";
}
print C "  { NULL, NULL },\n";
print C <<C;
};
C

print C "struct xnn_data ${cbd}_data = { ${cbd}_enames, ${cbd}_anames, ${cbd}_nstab };\n";

close(C);

############################################################################

sub nsuri {
    my $ns = $1;
    my $uri = '';
    if (($uri = $ns{$ns})) {
	return $uri;
    } else {
	warn "$0: unknown prefix $ns\n";
    }
    return undef;
}

sub
a_nsify {
    my $n = shift;
    if ($n =~ s/^(.*?)://) {
	my $uri = nsuri($1);
	if ($uri) {
	    $n = "$uri:$n";
	}
    }
    $n;
}

sub
e_nsify {
    my $n = shift;
    if ($n =~ s/^(.*?)://) {
	my $uri = nsuri($1);
	if ($uri) {
	    $n = "$uri:$n";
	}
    } else {
	if ($ns) {
	    $n = "$ns:$n";
	}
    }
    $n;
}

sub
process_attr {
    my $a = shift;
    $a{$a} = a_nsify($a);
#    $max_n = length($a) if length($a) > $max_n;
#    $max_q = (length($a{$a})+length($a)) 
#	if (length($a{$a})+length($a)) > $max_q;
}

sub
process_elem {
    my $e = shift;
    $e{$e} = e_nsify($e);
#    $max_n = length($e) if length($e) > $max_n;
#    $max_q = (length($e{$e})+length($e)) 
#	if (length($e{$e})+length($e)) > $max_q;
}

1;
