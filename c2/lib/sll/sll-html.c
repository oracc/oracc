
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
    } else {
	$pr =~ s/\s+.*$//;
	html_header();
	sign_frame($pr);
	html_trailer();
    }
} elsif ($grapheme eq '#none') {
    html_header();
    sign_frame('');
    html_trailer();
} else {
    html_header();
    print '<p class="nomatch">No matches</p>';
    html_trailer();
}

##############################################################################

sub
set_ixname {
    my $buildproj = "@@ORACC_BUILDS@@/pub/$_[0]";
    warn "$0: trying $buildproj/sl/corpus.dbi to set ixname\n";
    if (-r "$buildproj/sl/corpus.dbi") {
	$ixname = 'corpus';
	warn "$0: found ixname=corpus\n";
    } else {
	warn "$0: ixname not found\n";
    }
}

sub
sign_frame {
    inter_frame_divs();
    print "<iframe seamless=\"seamless\" class=\"sign-frame\" id=\"signframe\" src=\"/$project/signs/$_[0].html\"> </iframe>";
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
    if ($ext) {
	if ($ext eq 'forms') {
	    $vcat = slse("$vcat\;name");
	} elsif ($signlist_hack) {
	    $ext = 'signlist';
	}
	$vcat = "$ext_pre{$ext}$vcat$ext_post{$ext}";
    }
#    print header(-charset=>'utf-8');
    print <<EOH;
Content-type: text/html; charset=utf-8

<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="sux" xml:lang="sux">
<head><meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>OGSL Results for $vcat</title>
<link rel="shortcut icon" type="image/ico" href="/favicon.ico" />
<link rel="stylesheet" type="text/css" href="/css/cbd.css" />
<script src="/js/cbd.js" type="text/javascript"><![CDATA[ ]]></script>
<!-- Google tag (gtag.js) -->
<script async="async" src="https://www.googletagmanager.com/gtag/js?id=G-0QKC3P5HJ1"></script>
<script>
  window.dataLayer = window.dataLayer || [];
  function gtag(){dataLayer.push(arguments);}
  gtag('js', new Date());

  gtag('config', 'G-0QKC3P5HJ1');
</script>
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
