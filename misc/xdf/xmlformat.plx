#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use Getopt::Long;
use Pod::Usage;

my $no_refill;

GetOptions(
	   'nofill'=>\$no_refill,
	   ) || pod2usage(1);

my $x = shift @ARGV;
$x || pod2usage(1);
binmode STDOUT, ':utf8';
my $incomment = 0;
my $xpp = ($no_refill ? 'cat' : "xmllint -format");

open(X,"$xpp $x|");
while (<X>) {
    chomp;
    # refill attributes if the line starts with an element;
    # we don't refill inline content yet.
    if (!$no_refill && ((length($_)>80) && /=\"/)) {
	my($initws,$elem,$attr,$post) = m,^(\s*)(<[^>]+?\s+)(.*?)(/?>.*)?$,;
	my @refill = "$initws$elem";
	my @attr = ();
	$initws = $initws.(' 'x length($elem));
	my $sofar = $initws;
	while ($attr =~ s/^(.*?=\".*?\")\s*//) {
	    my $next_attr = $1;
	    if (length($sofar)+length($next_attr) > 80) {
		push @refill, $sofar;
		$sofar = "$initws $next_attr";
	    } else {
		$sofar .= " $next_attr";
	    }
	}
	if ($sofar) {
	    if ($#refill == 0) {
		$sofar =~ s/^\s*/ /;
		$refill[0] =~ s/\s*$//;
		$refill[0] .= $sofar;
	    } else {
		push (@refill, $sofar);
	    }
	}
	push @refill, "$initws$post";
	my $res = join("\n", map { &style($_) } @refill);
	$res =~ s/(<.*?\s+?)\n +/$1 /;
	$res =~ s,\n\s+(/?>), $1,;
	print "$res\n";
    } else {
	print style($_), "\n";
    }
}
close(X);

sub
style {
    if (/^<\?/) {
	$_ = span('xml_decl',$_);
    } elsif (/<!--/) {
	if (/<!--.*?-->/) {
	    s/<!--(.*?)-->/<!--\000span class\001"def_Comment">$1\000\/span>-->/g;
	} else {
	    $incomment = 1;
	}
    } elsif ($incomment) {
	if (/-->/) {
	    $incomment = 0;
	} else {
	    $_ = span('def_Comment',$_);
	}
    } else {
	s/xmlns=\"(.*?)\"/defaultns($1)/eg;
	s/xmlns:(.*?)=\"(.*?)\"/xmlns($1,$2)/eg;
	s/\s([-a-zA-Z]+?):([^=]+?)=\"(.*?)\"/nsattr($1,$2,$3)/eg;
	s/\s([-a-zA-Z]+?)=\"(.*?)\"/attr($1,$2)/eg;
	s/(<\/?)([^: ]+?)([ <>])/elem($1,$2,$3)/eg;
	s/(<\/?)([-a-zA-Z]+?):(.*?)(\s|[<>])/nselem($1,$2,$3,$4)/eg;
    }
    s/</&lt;/g;
    s/\000/</g;
    s/\001/=/g;
    $_;
}

sub
defaultns {
    " xmlns\001\"".span('xml_default_ns_uri',$_[0]).'"';
}

sub
xmlns {
    " xmlns:".span('xml_ns_prefix',$_[0])
	."\001\"".span('xml_ns_uri',$_[1]).'"';
}

sub
elem {
    $_[0].span('xml_elem',$_[1]).$_[2];
}

sub
nselem {
    $_[0].span('xml_ns_prefix',$_[1]).':'.span('xml_elem',$_[2]).$_[3];
}

sub
attr {
    ' '.span('xml_attr',$_[0])."\001\"".span('xml_string',$_[1]).'"';
}

sub
nsattr {
    ' '.span('xml_ns_prefix',$_[0]).':'.span('xml_attr',$_[1])."\001\""
	.span('xml_string',$_[2]).'"';
}

sub
span {
    my($class,$text) = @_;
    my($initws,$span) = ($text =~ /^(\s*)(.*)$/);
    "$initws\000span class\001\"$class\">$span\000/span>";
}

1;

__END__

=head1 NAME

xmlformat.plx -- reformat XML files and add syntax colorization tags

=head1 SYNOPSIS

xmlformat.plx [options] FILE

=head1 OPTIONS

=over

=item B<-nofill>

Do not pass the XML file through C<xmllint -format> first.

=back

=head1 DESCRIPTION

The primary purpose of this program is to add syntax colorization
tags; these are generated as HTML spans.  By default, the XML is 
passed through C<xmllint -format> before colorization.

=head1 AUTHOR

Steve Tinney (stinney@sas.upenn.edu)

=head1 COPYRIGHT

Copyright(c) Steve Tinney 2006.

Released under the Gnu General Public License
(http://www.gnu.org/copyleft/gpl.html).
