#!/usr/bin/perl
use warnings; use strict;
use Encode; use utf8;
use open 'utf8'; binmode STDIN, 'utf8'; binmode STDOUT, 'utf8'; binmode STDERR, 'utf8'; 
use CGI qw/:standard/;
use lib '@@ORACC@@/lib';
use ORACC::ATF::Unicode;

my $db_file = "@@ORACC@@/pub/ogsl";
my $db_name = 'ogsl';

my %ext_pre = (
    'c'=>'',
    'cinit'=>'Initial ',
    'clast'=>'Final ',
    'contains'=>'',
    'contained'=>'Contained ',
    'h'=>'',
    'm'=>'Modified ',
    'multi'=>'',
    );

my %ext_post = (
    'aka'=>' Aliases',
    'c'=>' Compounds',
    'cinit'=>'',
    'clast'=>'',
    'contains'=>' Container',
    'contained'=>'',
    'forms'=>' Forms',
    'h'=>' Homophones',
    'signlist'=>' Numbers',
    'm'=>'',
    'multi'=>' Repeated',
    );

my @id_ext = qw/forms/;
my %id_ext; @id_ext{@id_ext} = ();

my @uc_ext = qw/c cinit clast contains contained forms m multi/;
my %uc_ext; @uc_ext{@uc_ext} = ();

my $v = decode utf8=>param('k1');
my $ext = param('ext') || '';

my $project = param('project') || 'ogsl';

if ($v =~ s/([a-z])0$/$1/) {
    $ext = 'h';
}

my $signlist_hack = 0;

$v = ORACC::ATF::Unicode::gconv($v);
if ($ext) {
    if (exists($id_ext{$ext})) {
	my $v_orig = $v;
	$v = slse($v);
	if ($v) {
	    warn "remapping $v_orig to $v\n";
	}
    } elsif (exists($uc_ext{$ext})) {
	$v =~ tr/a-zšŋ/A-ZŠŊ/;
    } else {
	if ($ext eq 'h') {
	    $v =~ s/[0-9₀-₉₊]+$//;
	    if (is_signlist($v)) {
		$signlist_hack = 1;
	    } else {
		$v =~ tr/A-ZŠŊ/a-zšŋ/;
	    }
	} else {
	    $v =~ tr/A-ZŠŊ/a-zšŋ/;
	}
    }
}

# Map a bad sign name to its proper known one if possible
if ($v =~ /[AEIU]/) {
    unless (slse($v)) {
	my $nv = $v;
	$nv =~ tr/A-ZŠŊ/a-zšŋ/;
	my $vtmp = slse($nv);
	if ($vtmp) {
	    $v = slse("$vtmp\;name");
	}
    }
}

my $pr;

if ($ext) {
    $pr = slse("$v\;$ext") || '';
    warn "$v => $pr\n";
} else {
    $pr = slse($v) || '';
    warn "$v => $pr\n";
}

if ($pr) {
    if ($ext) {
	html_header();
	my %known = ();
	my $first_id = undef;
	my @ss = split(/\s+/,$pr);
	for (my $i = 0; $i <= $#ss; ++$i) {
	    my $s = $ss[$i];
	    my $punct = ($i < $#ss ? ';' : '');
	    next if $known{$s}++;
	    if ($s =~ m,/,) {
		my ($id,$n) = ($s =~ m,^(.*?)/(.*?)$,);
		$first_id = $id unless $first_id;
		if ($n =~ /^~[a-z]+/) {
		    # do nothing
		} else {
		    if ($n >= 1000) {
			$n = 'x';
		    } elsif ($n == 0) {
			$n = '';
		    }
		}
		my $sn = slse("$id\;name");
		my $vh = $v;
		if (is_signlist($vh)) {
		    printhtml($id,$sn,"$vh$n",$punct);
		} else {
		    printhtml($id,$sn,"$vh<sub>$n</sub>",$punct);
		}
	    } else {
		my $sn = slse("$s;name");
		printhtml($s,$sn);
	    }
	}
	sign_frame($first_id) unless $project eq 'epsd2';
	html_trailer();
    }else {
	$pr =~ s/\s+.*$//;
	if ($project ne 'ogsl') {
	    print redirect("/$project/sl/brief/$pr.html");
	} else {
	    print redirect("/ogsl/signs/$pr.html");
	}
    }
} else {
    html_header();
    print '<p class="nomatch">No matches</p>';
    html_trailer();
}

##############################################################################

sub
sign_frame {
    inter_frame_divs();
    print "<iframe seamless=\"seamless\" class=\"sign-frame\" id=\"signframe\" src=\"/ogsl/signs/$_[0].html\"> </iframe>";
    close_frame_divs();
}

sub
open_frame_divs {
    print '<div name="ogsltop" id="ogsltop">';
}
sub
inter_frame_divs {
    print '</div><div name="ogsl-body" id="ogslbody">';
}
sub
close_frame_divs {
    print '</div>';
}
sub
html_header {
    my $vcat = $v;
    Encode::_utf8_on($vcat);
    if ($ext) {
	if ($ext eq 'forms') {
	    $vcat = slse("$vcat\;name");
	} elsif ($signlist_hack) {
	    $ext = 'signlist';
	}
	$vcat = "$ext_pre{$ext}$vcat$ext_post{$ext}";
    }
    print header(-charset=>'utf-8');
    print <<EOH;
<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="sux" xml:lang="sux">
<head><meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>OGSL Results for $vcat</title>
<link rel="shortcut icon" type="image/ico" href="/favicon.ico" />
<link rel="stylesheet" type="text/css" href="/css/cbd.css" />
<script src="/js/cbd.js" type="text/javascript"><![CDATA[ ]]></script>
</head><body>
EOH
    if ($ext) {
	open_frame_divs() unless $project eq 'epsd2';
	print "<h1 class=\"ogslres\">$vcat</h1>\n";
    }
}

sub
html_trailer {
    print '</body></html>',"\n";
}

sub
printhtml {
    my($id,$sn,$v,$p) = @_;
    if ($v) {
	$v .= '&#xa0;=&#xa0;';
    } else {
	$v = '';
    }
    my $pspan = '';
    if ($p) {
	if ($project eq 'epsd2') {
	    $pspan = '<br/>';
	} else {
	    $pspan = '<span class="ogsl-punct">;</span>';
	}
    } else {
	$p = '';
    }
    print "<a href=\"javascript:showsign('$project','$id')\">$v<span class=\"sign\">$sn</span></a>$pspan\n";
}

sub
subify {
    my $sub = shift;
    $sub =~ tr/₀-₉₊/0-9x/;
    "<sub>$sub</sub>";
}

sub
is_signlist {
    $_[0] =~ /^[A-Z]SL/;
}

sub
slse {
    `@@ORACC@@/bin/sl $db_name $db_file '$_[0]'`
}

1;
