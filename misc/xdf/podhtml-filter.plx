#!/usr/bin/perl
use warnings; use strict;
my $title = '';
my $author = '';
my @file = ();

while (<>) {
    next if /^<!DOCTYPE/;
    if (/^\s*<link/) {
	push(@file, 
	     '<link rel="stylesheet" type="text/css" href="/xdf.css" />', 
	     "\n");
    } elsif (/^\s*<title>/) {
	$title = $_;
	push @file, $_;
	$title =~ s/^\s*<title>//;
	$title =~ s/\s*--.*$//;
	$title =~ s/\s*$//;
    } elsif (/^\s*<body/) {
	s/<body.*$/<body class="pod">/;
	push @file, $_;
    } elsif (/name="__index__"/) {
	s/<p>/<h1>/;
	s,</a></p>,CONTENTS</a></h1>,;
	push @file, $_;
    } elsif (/^\s*<h1><a name="author"/) {
	$author = <>;
	push @file, $_, $author;
	$author =~ s/\s*$//;
    } else {
	push @file, $_;
    }
}

foreach my $l (@file) {
    print $l;
    if ($l =~ /^\s*<body/) {
	print "<h1 class=\"title\">$title</h1>\n";
	print "<h2 class=\"author\">$author</h2>\n";
    }
}

1;

__END__

=head1 NAME

podhtml-filter.plx -- add an XDF-style header to pod2html output

=head1 SYNOPSIS

podhtml-filter.plx

=head1 DESCRIPTION

Reads input from STDIN and writes to STDOUT, adding a CSS link to xdf.css
and supplying the XDF documentation's title and author headings.

=head1 AUTHOR

Steve Tinney (stinney@sas.upenn.edu)

=head1 COPYRIGHT

Copyright(c) Steve Tinney 2006.

Released under the Gnu General Public License
(http://www.gnu.org/copyleft/gpl.html).
