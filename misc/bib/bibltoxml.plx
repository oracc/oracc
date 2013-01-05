#!/usr/bin/perl
use warnings; use strict;
undef $/;
$_ = <>;
s/([[{(])\.(.*?)\.([\]})])/&hack_cites($1,$2)/seg;
print;

1;

#############################################################

sub
hack_cites {
    my ($type,$text) = @_;
    my $ret = '';
    $ret = "<cites type=\"$type\">";
    foreach my $c (split(/(?<!\\);\s*/,$text)) {
	my ($keys,$note) = ($c =~ /^(.*?)(?::\s*(.*))?$/s);
	if (defined $note) {
	    $note =~ s/^\s*//;
	    $note =~ s/\s*$//;
	} else {
	    $note = '';
	}
	$ret .= "<cite keys=\"$keys\">$note</cite>";
    }
    $ret .= "</cites>";
    $ret;
}
