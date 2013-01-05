#!/usr/bin/perl
use warnings; use strict;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XVL::AVL;

ORACC::XVL::AVL::load_class('period');
print "Uruk V = ", ORACC::XVL::AVL::get('period','Uruk V'), "\n";
print "Nuzi = ", ORACC::XVL::AVL::get('period','Nuzi'), "\n";

1;
