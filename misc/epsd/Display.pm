package ORACC::SE::Display;
use warnings; use strict;
use File::Temp qw/tempfile/;

my $pagesize = 25;
my $page_minsize = 3;
my $maxpage = 8;

sub
summary_list {
    my @disp = @_;
    my @ret = ();
    foreach my $d (@disp) {
	my($index,@records)  = @{$d};
	foreach my $r (@records) {
	    $r =~ s/[usi].*$//;
	    if ($$index{"$r,url"}) {
		push @ret, join('<p><a href="',$$index{"$r,url"},'">');
		push @ret, $$index{"$r,sum"};
		push @ret, "</a></p>\n";
	    } else {
		push @ret, $$index{"$r,sum"},"\n";
	    }
	}
    }
    @ret;
}

sub
html {
    my @disp = @_;
    my @ret = ();
    push @ret, "<div class=\"sehtml\">\n";
    foreach my $d (@disp) {
	my($index,@records)  = @{$d};
	foreach my $r (@records) {
	    $r =~ s/[usi].*$//;
	    if ($$index{"$r,url"}) {
		push @ret, join('<p><a href="',$$index{"$r,url"},'">');
		push @ret, $$index{"$r,sum"};
		push @ret, "</a></p>\n";
	    } else {
		push @ret, $$index{"$r,sum"},"\n";
	    }
	}
    }
    push @ret, "</div>\n";
    join('',@ret);
}

sub
save {
    my ($title_x,$search,$rtotal,@disp) = @_;
    my @pages = ();
    my $page = 0;
    foreach my $d (@disp) {
	my($index,@records) = @$d;
	my $ixname = $$index{'#name'};
	foreach my $r (@records) {
	    if (1+$#{$pages[$page]} == $pagesize) {
		++$page;
	    }
	    push @{$pages[$page]}, "$ixname:$r";
	}
    }

    # if there is more than one page, check that the last page
    # is at least page_minsize entries in size; if it is too short,
    # force it onto the preceding page.
    if ($page && $#{$pages[$page]} < $page_minsize) {
	push @{$pages[$page-1]}, @{$pages[$page]};
	pop @pages;
	--$page;
    }

#    my $tname = tmpnam();
    my($tmp,$tname) = tempfile(SUFFIX=>'.ses');
#    open(TMP,">$tname");
    print $tmp "ORACC SE saveset\n$title_x\n$search\n$rtotal\n$page\n";
    foreach my $p (@pages) {
	print $tmp join(' ', @{$p}), "\n";
    }
#    close TMP;
    ($tname,$page,$pages[0]);
}

sub
show {
    my($title,$base,$tmpfile,$search,$rtotal,$lastpage,$pagenum,$page) = @_;
    $base =~ s/\??$/?/;
    my ($prevpage,$nextpage) = ();
    if ($pagenum > 0) {
	$prevpage = "s=$tmpfile\&amp;p=".($pagenum-1);
    }
    if ($pagenum < $lastpage) {
	$nextpage = "s=$tmpfile\&amp;p=".($pagenum+1);
    }
    my @pagelinks = ();
    if ($lastpage > 0) {
	my ($pwin_first,$pwin_last);
	if ($lastpage <= $maxpage) {
	    $pwin_first = 0;
	    $pwin_last = $maxpage;
	} else {
	    $pwin_first = $pagenum - ($maxpage/2);
	    $pwin_first = 0 if $pwin_first < 0;
	    $pwin_last = $pwin_first + $maxpage;
	}
#	for (my $i = $pwin_first; $i <= $lastpage; ++$i) {
	my $first_page_link = ($lastpage == 2) ? 0 : ($pagenum-1);
	my $last_page_link = ($lastpage == 2) ? 2 : ($pagenum+1);

	if ($first_page_link > 0) {
	    push @pagelinks, ["s=$tmpfile\&amp;p=".($first_page_link), # -1
			      '/epsd/icons/sgl-l-arrow.png',0];
	}

	for (my $i = $first_page_link; $i <= $last_page_link; ++$i) {
	    next unless $i >= 0;
	    last if $i > $lastpage;
	    push @pagelinks, ["s=$tmpfile\&amp;p=".$i,$i+1,$i==$pagenum];
	}

	if ($last_page_link < $lastpage) {
	    push @pagelinks, ["s=$tmpfile\&amp;p=".($last_page_link), # +1
			      '/epsd/icons/sgl-r-arrow.png',0];
	}

    }
    my $plur = ($rtotal == 1) ? '' : 's';
    my @html = ();
    push @html, '<div class="seresult">', "\n";
    push @html, <<EOH;
<h1>$title search: $rtotal hit$plur for <span class="sesearch">$search</span></h1>
EOH
    if ($lastpage) {
#	push @html, '<table><tr>';
#	if ($prevpage) {
#	    push @html, '<td class="sepny"><a href="',$base.$prevpage,'">PREV</a></td>';
#	} else {
#	    push @html, '<td class="sepnn"></td>';
#	}
	push @html, '<h2>';
#	foreach my $pl (@pagelinks) {
#	    my($href,$text,$sel) = @$pl;
##	    push @html, '<td><a href="',$base,$href,'">[',$text,']</a></td>';
##	    push @html, '<a href="',$base,$href,'">[',$text,']</a>';
#	    push @html, '<span class="sesel">' if $sel;
#	    if ($text =~ /\.(?:png|gif|jpg)$/) {
#		push @html, '<a href="',$base,$href,'">', "<img src=\"$text\"/>", '</a>';
#	    } else {
#		push @html, '<a href="',$base,$href,'"> ', $text, ' </a>';
#	    }
#	    push @html, '</span>' if $sel;
#	    push @html, '::';
#	}
#	pop @html;

	if ($pagenum > 0) {
	    push @html, "<a href=\"${base}s=$tmpfile\&amp;p=".($pagenum-1)."\">";
	    push @html, "<span class=\"arrow\">&#xAB;</span>", "</a>";
	}

	push @html, ($pagenum+1)."/".($lastpage+1);

	if ($pagenum < $lastpage) {
	    push @html, "<a href=\"${base}s=$tmpfile\&amp;p=".($pagenum+1)."\">";
	    push @html, "<span class=\"arrow\">&#xBB;</span>", "</a>";
	}

	push @html, '</h2>';
#	if ($nextpage) {
#	    push @html, '<td class="sepny><a href="',$base,$nextpage,'">NEXT</a></td>';
#	} else {
#	    push @html, '<td class="sepnn"></td>';
#	}
#	push @html, '</tr></table>';
    }

    my $ix_name = '';
    my $ix = undef;
    foreach my $r (@$page) {
	my($iname,$rid,$sid) = ($r =~ /^(.*?):(.*?)(?:[usi](.*))?$/);
	if ($iname ne $ix_name) {
	    $ix_name = $iname;
	    $ix = ORACC::SE::DBM::load($ix_name);
	}
	push @html, ORACC::SE::DBM::html($ix,$rid), "\n";
    }

    push @html, '</div>',"\n";
    join('',@html);
}

sub
retrieve_page {
    my($tmpfile,$page) = @_;
    open(IN,$tmpfile) || return ();
    my @file = (<IN>);
    close(IN);
    my($title_x,$search,$rtotal,$lastpage) = ($file[1],$file[2],$file[3],$file[4]);
    my $pagetext = $file[5+$page];
    chomp($title_x,$search,$rtotal,$lastpage,$pagetext);
    ($title_x,$search,$rtotal,$lastpage,[split(/\s/,$pagetext)]);
}

sub
expired {
    "<div><p>Sorry, your previous search has expired.  "
	. "Please enter a new search.</p></div>";
}

1;
