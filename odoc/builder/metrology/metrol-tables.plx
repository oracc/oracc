#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::NS;
use ORACC::SL::Base;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my $first_h1 = 1;
my $period = 'OB';
my $row = 0;

ORACC::SL::Base::init();

open(C,"unzip -p metrol-tables.ods content.xml|");
my $content = join('',(<C>));
close(C);
my $xml = load_xml_string($content);
my $table = (tags($xml,
		  $ODFTABLE,
		  'table'))[0];

print <<EOH;
<?xml version="1.0" encoding="utf-8"?>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
<title>Oracc Metrology Guidelines</title>
<link rel="stylesheet" type="text/css" href="/css/oraccdoc.css"/>
<link rel="stylesheet" type="text/css" href="/css/cuneify-mixed.css"/>
<script src="/js/oraccdoc.js" type="text/javascript"> </script>
</head>
<body>
<div class="body">
EOH

my @rows = tags($table,$ODFTABLE,'table-row');
my $i = 0;
while ($i <= $#rows) {
    my $text = $rows[$i]->textContent();
    ++$i and next if $text =~ /^\s*$/;
    if ($text =~ /^\*/ && $text !~ /^\*(PI|GAN)/) {
	++$i;
	$text =~ s/\#([a-z0-9_]+)$//;
	my $frag = $1;
	my $fragstr = ($frag ? "<a name=\"$frag\"/>" : '');
	$text =~ s/^(\*+)\s*//;
	my $ast = $1;
	my $lev = length $ast;
	$text = inline($text);
	if ($lev == 1 && $first_h1) {
	    print "<div class=\"doc-banner\"><h$lev class=\"title\">$text</h$lev></div>\n";
	    $first_h1 = 0;
	} else {
	    print "<h$lev>$fragstr$text</h$lev>\n";
	}
    } elsif ($text =~ /^\#/) {
	if ($text =~ /cuneif/ && $text =~ /NA/) {
	    $period = 'NA';
	}
	++$i;
    } else {
	table();
    }
}
print '</div></body></html>';

ORACC::SL::Base::term();

#######################################################################

sub
inline {
    my $tmp = shift;
    $tmp =~ s#(?<![\)0-9])\@([^-\) ]+)#<i>$1</i>#g;
    $tmp;
}

sub
table {
    print "<table>\n<thead><tr>\n";
    my @cells = tags($rows[$i],$ODFTABLE,'table-cell');
    print "<th/>";
    foreach my $c (@cells) {
	next unless $c->hasChildNodes();
	print "<th>", inline($c->textContent()), "</th>\n";
    }
    print "</tr></thead>\n<tbody>\n";
    while ($i < $#rows) {
	my $t = $rows[$i+1]->textContent();
	++$i;
	last if $t =~ /^\s*$/;
	print '<tr>';
	my @cells = tags($rows[$i],$ODFTABLE, 'table-cell');
	my $cuneicell = $cells[0]->textContent();
	$cuneicell =~ s/\s+\(.*$//;
	my $cunei = ORACC::SL::Base::tlit2cunei("metrol_tables.ods:$i",
						ORACC::SL::Base::tlitsplit($cuneicell,1));
	print "<td class=\"cuneiform-$period\">$cunei</td>";
	foreach my $c (@cells) {
	    next unless $c->hasChildNodes();
	    print "<td>",inline($c->textContent()),"</td>\n";
	}
	print "</tr>\n";
    }
    print "</tbody></table>\n";
}

1;
