#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use lib "$ENV{'ORACC'}/lib";
use ORACC::Texts::Util;

my ($w2l_file,$sig_file) = @ARGV;

open(W,$w2l_file); my @w = (<W>); chomp @w; close(W);
open(S,$sig_file); my @s = (<S>); chomp @s; close(S);

wid2lem_by_sig($w2l_file,\@w,$sig_file,\@s);

1;
