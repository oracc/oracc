#!/usr/bin/perl
# @SUMMARY@= simple template generator for XDF files
use warnings; use strict;

my $template = shift @ARGV;

die "xdftemplate.plx: must give name of file to create\n"
    unless $template;

die "xdftemplate.plx: will not overwrite $template\n"
    if -r $template;

open(T,">$template") 
    || die "xdftemplate.plx: can't write $template\n";

print T <<EOT;
<?xml version='1.0' encoding='utf-8'?>
<d:doc  xmlns="http://www.w3.org/1999/xhtml" 
	xmlns:d="http://oracc.org/ns/xdf/1.0"
	xmlns:dc="http://purl.org/dc/elements/1.1"
	xmlns:dcterms="http://purl.org/dc/terms/"
	xmlns:h="http://www.w3.org/1999/xhtml" 
   	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">

<d:meta>
  <dc:title></dc:title>
  <dcterms:alternative></dcterms:alternative>
  <dcterms:identifier 
      xsi:type="dcterms:URI"></dcterms:identifier>
  <dc:creator></dc:creator>
  <dc:date></dc:date>
  <dc:publisher></dc:publisher>
  <dc:description></dc:description>
</d:meta>

<d:schema name="" uri="">
</d:schema>

<d:resources/>

</d:doc>
EOT

close(T);

1;
