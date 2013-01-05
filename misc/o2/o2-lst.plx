#!/usr/bin/perl
use warnings; use strict; use open 'utf8';

my $listdir = '01bld/lists';

my $atfsources = '01bld/atfsources.lst';
my $bin = "$ENV{'ORACC'}/bin";
my $cat_ids = "$listdir/cat-ids.lst";
my $have_atf = "$listdir/have-atf.lst";
my $have_lem = "$listdir/have-lem.lst";
my $have_xtf = "$listdir/have-xtf.lst";
my $out_approved = "$listdir/approved.lst";
my $out_outlined = "$listdir/outlined.lst";
my $project = `oraccopt`;
my $verbose = 0;

sub xsystem;

atf_sources();
create_have_atf();
proxy_lists();
update_lists();
lemindex_list();

##############################################################

sub
atf_sources {
    system 'o2-atfsources.plx';
}

sub
create_have_atf {
    my $currpqx = '';
    my %have_atf = ();
    my %have_lem = ();

    open(S, $atfsources) || return;
    my @atf_sources = (<S>);
    close(S);
    chomp @atf_sources;

    foreach my $atf (@atf_sources) {
        if ($atf =~ /\.ods$/) {
            open(A,"$ENV{'ORACC'}/bin/ods2atf.sh -s $atf|");
        } else {
            open(A,$atf);
        }
        while (<A>) {
            if (/^\x{feff}?&([PQX]\d+)/) {
                $currpqx = $1;
                ++$have_atf{$currpqx} unless /^\#/;
            } elsif (/^\#lem/) {
		++$have_lem{$currpqx};
            }
	}
        close(A);
    }
    open(L,">$have_atf");
    if (scalar keys %have_atf) {
	print L join("\n",map { "$project:$_" } sort keys %have_atf), "\n";
	close(L);
	if (scalar keys %have_lem) {
	    open(L,">$listdir/have-lem.lst");
	    print L join("\n",map { "$project:$_" } sort keys %have_lem), "\n";
	    close(L);
	}
    } else {
	close(L);
    }
}

sub
drop_proxy_dups {
    my @p = ();
    my %known = ();
    my $status = 0;
    open(P, '01bld/lists/proxy.lst') || return;
    open(A, '01bld/lists/approved.lst') || return;
    while (<A>) {
	chomp;
	my $k = $_;
	$k =~ s/^.*?://;
	$known{$k} = $_;
    }
    close(A);
    use Data::Dumper;
#    warn Dumper \%known;
    while (<P>) {
	chomp;
	my $id = $_;
	$id =~ s/^.*?:(.*?)\@.*$/$1/;
	if ($known{$id}) {
	    warn "o2-lst.plx: dropping proxied $_: already have $known{$id}\n";
	    ++$status;
	} else {
	    push @p, $_;
	}
    }
    close(P);
    if ($status) {
	if ($#p >= 0) {
	    open(P, '>01bld/lists/proxy.lst');
	    print P join("\n", @p), "\n";
	    close(P);
	} else {
	    unlink '01bld/lists/proxy.lst';
	}
    }
}

sub
lemindex_list {
    my %proxy_projects = ();
    if (open(P,'01bld/lists/proxy.lst')) {
	while (<P>) {
	    chomp;
	    s/:.*$//;
	    ++$proxy_projects{$_};
	}
	close(P);
	my @proxy_lem = ();
	xsystem 'rm', '-f', '01bld/lists/proxy-lem.lst';
	my @pp = sort keys %proxy_projects;
#	warn "@pp\n";
	foreach my $p (keys %proxy_projects) {
	    push @proxy_lem, '+?', "$ENV{'ORACC'}/bld/$p/lists/have-lem.lst";
	}
	if ($#proxy_lem >= 0) {
	    shift @proxy_lem; # shift the leading '+?' off so the first list is lead
	    xsystem 
		'atflists.plx', '-o', "$listdir/proxy-lem.lst",
		@proxy_lem,
		'&','01bld/lists/proxy.lst';
	}
    }
    if (-r $have_lem) {
	xsystem 
	    'atflists.plx', '-o', "$listdir/lemindex.lst",
	    $have_lem,
	    '+?',"$listdir/proxy-lem.lst";
    } else {
	xsystem 'cp', "$listdir/proxy-lem.lst", "$listdir/lemindex.lst";
    }
}

sub
proxy_lists {
    if (-r '00lib/proxy.ol') {
	# this will write 01bld/proxy.lst
	warn "o2-lst.plx: proxy.ol not yet implemented\n";
    } elsif (-r '00lib/proxy.lst') {
	open(P,'00lib/proxy.lst');
	my @p = (<P>); chomp(@p);
	close(P);
	open(P, '>01bld/lists/proxy.lst');
	foreach my $p (sort @p) {
	    unless ($p =~ /:/) {
		warn "o2-lst.plx: `$p': bad proxy in 00lib/proxy.lst\n";
	    } else {
		$p =~ tr/\r\n//d;
		unless ($p =~ /\@/) {
		    my $xmd = '';
		    if ($p =~ /:P/) {
			$xmd = 'cdli';
		    } elsif ($p =~ /:Q/) {
			$xmd = 'qcat';
		    } else {
			$xmd = $p;
			$xmd =~ s/:.*$//;
		    }
		    $p .= "\@$xmd";
		}
		print P "$p\n";
	    }
	}
	close(P);
    }
    drop_proxy_dups();
}

sub
update_lists {
    my $opt = `oraccopt . build-approved-policy`;

    if (!$opt || $opt eq 'atf') {
	xsystem 
	    'atflists.plx', '-o', $out_approved, '-p', $project,
	    $have_atf,
	    '-?','00lib/not-approved.lst',
	    '+?','00lib/add-approved.lst';
    } elsif ($opt eq 'catalog') {
	xsystem 'xmd-ids.plx', $cat_ids;
	xsystem 
	    'atflists.plx', '-o', $out_approved, '-p', $project,
	    $cat_ids,
	    '-?','00lib/not-approved.lst',
	    '+?','00lib/add-approved.lst';
    } elsif ($opt eq 'mixed') {
	xsystem 'xmd-ids.plx', $cat_ids;
	xsystem
	    'atflists.plx', '-o', $out_approved, '-p', $project,
	    '00lib/approved.lst',
	    '+', $have_atf,
	    '+', $cat_ids,
	    '-?','00lib/not-approved.lst',
	    '+?','00lib/add-approved.lst';
    } elsif ($opt eq 'umbrella') {
	## automatically include all approved texts from
	## all public subprojects
	my @pubsub = grep(m#/#, `projpublic.sh $project`);
	chomp @pubsub;
	@pubsub = map { s#^$project/## ; 
			"$_/01bld/lists/approved.lst" } @pubsub;
	open(A, '>01bld/lists/proxy.lst')
	    || die "o2-lst.plx: can't write 01bld/lists/approved.lst\n";
	foreach my $p (@pubsub) {
	    open(P, $p);
	    my @l = (<P>);
	    chomp @l;
	    foreach my $p (@l) {
		$p =~ s/^(.*?):(.*?)$/$1:$2\@$1/;
		print A "$p\n";
	    }
	    close(P);
	}
	close(A);
    } elsif ($opt eq 'static') {
	xsystem 'cp', '-a', '00lib/approved.lst', $listdir;
    } elsif ($opt =~ /\.ol$/) {
	## ol('outlined.lst', $opt);
	warn "o2-lst.plx: .ol lists not yet implemented\n";
    } elsif ($opt eq 'approved') {
	warn "o2-lst.plx: `approved' is no longer a valid value for `build-approved-policy'\n";
	warn "o2-lst.plx: please edit 00lib/config.xml and change `approved' to `static'\n";
    } else {
	warn "o2-lst.plx: unknown build-approved-policy value `$opt'\n";
    }

    proxy_lists();

    $opt = `oraccopt . build-outlined-policy`;

    if ($opt) {
	if ($opt eq 'approved') {
	    xsystem 
		'atflists.plx', "-o$out_outlined", '-p', $project,
		$out_approved,
		'-?', '00lib/not-outlined.lst',
		'+?', '00lib/add-outlined.lst';
	} elsif ($opt eq 'P') {
	    xsystem 
		"grep :[PX] $out_approved | atflists.plx -p$project -o$out_outlined stdin -? 00lib/not-outlined.lst +? 00lib/add-outlined.lst";
	} elsif ($opt eq 'Q') {
	    xsystem 
		"grep :Q $out_approved | atflists.plx -p$project -o$out_outlined stdin -? 00lib/not-outlined.lst +? 00lib/add-outlined.lst";
	} elsif ($opt eq 'atf') {
	    xsystem 
		"atflists.plx -p$project -o$out_outlined $have_atf -? 00lib/not-outlined.lst +? 00lib/add-outlined.lst";
	} elsif ($opt eq 'static') {
	    # no action
	} elsif ($opt =~ /\.ol/) {
	    ## ol('outlined.lst', $opt);
	    warn "o2-lst.plx: .ol lists not yet implemented\n";
	} else {
	    warn "o2-lst.plx: unknown build-outlined-policy value `$opt'\n";
	}
    }
    
    if (-r $have_atf && !-z _) {
	xsystem 'cp', '-p', "$have_atf", "$listdir/withatf"; ### FIXME: rationalize these
	xsystem 'cp', '-p', "$listdir/have-lem.lst", "$listdir/withlem";
	xsystem "atflists.plx -p$project $out_approved - $have_atf >$listdir/sansatf";
#	xsystem "atfhavelem.plx >$listdir/withlem";
	xsystem "atflists.plx -p$project $out_approved - $listdir/withlem >$listdir/sanslem";
    }
}

sub
xsystem {
    warn "xsystem @_\n" if $verbose;
    system @_;
}

1;
