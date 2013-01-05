#!/usr/bin/perl
use warnings; use strict;
print '<list>';
while (<>) {
    s/\s*$//;
    print "<item>$_</item>";
}
print '</list>';
1;
