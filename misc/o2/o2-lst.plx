#!/usr/bin/perl
use warnings; use strict; use open 'utf8';

my $globalA = '';

BEGIN {
    $SIG{'__WARN__'} = sub {
	if (/<A> line (\d+)/) {
	    $_[0] =~ s/at .*?,/,/;
	    $_[0] =~ s/^(.*?),\s+.*?(\d+)/$globalA:$2: $1\n"/;
	}
	warn $_[0];
    } 
}

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
	$globalA = $atf;
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
    }
    close(L);
    open(L,">$listdir/have-lem.lst");
    if (scalar keys %have_lem) {
	print L join("\n",map { "$project:$_" } sort keys %have_lem), "\n";
    }
    close(L);
}

sub
lemindex_list {
    my %proxy_lem_atfs = ();

    if (open(P,'01bld/lists/proxy-atf.lst')) {
	open(PL, '>01bld/lists/proxy-lem.lst');
	while (<P>) {
	    chomp;
	    my $line = $_;
	    my($proj,$id) = (/^(.*?):(.*?)(?:\@|$)/);
	    unless ($proxy_lem_atfs{$proj}) {
		my $projlem = "$ENV{'ORACC'}/bld/$proj/lists/have-lem.lst";
		if (-r $projlem) {
		    open(HAVELEM, $projlem) || die;
		    while (<HAVELEM>) {
			chomp;
			/:(.*?)(?:\@|$)/;
			${$proxy_lem_atfs{$proj}}{$1} = 1;
		    }
		    close(HAVELEM);
		} else {
		    $proxy_lem_atfs{$proj} = '#none#'
		}
	    }
#	    use Data::Dumper; print Dumper(\%proxy_lem_atfs);
#	    warn "proj = $proj\n";
	    if (ref($proxy_lem_atfs{$proj}) eq 'HASH') {
		if (scalar %{$proxy_lem_atfs{$proj}}) {
		    if (${$proxy_lem_atfs{$proj}}{$id}) {
			print PL "$line\n";
		    }
		}
	    }
	}
	close(PL);
	close(P);
    }
    if (-r $have_lem) {
	xsystem 
	    'atflists.plx', '-o', "$listdir/lemindex.lst",
	    $have_lem,
	    '+?',"$listdir/proxy-lem.lst";
    } elsif (-r "$listdir/proxy-lem.lst") {
	xsystem "cut -d\@ -f1 $listdir/proxy-lem.lst >$listdir/lemindex.lst";
    } else {
	unlink "$listdir/lemindex.lst";
	xsystem 'touch', "$listdir/lemindex.lst";
    }
}

sub
proxy_lists {
    my %host_atf = ();
    my %host_cat = ();
    my $proxy_lst = undef;

    if (open(ATF, '01bld/lists/have-atf.lst')) {
	while (<ATF>) {
	    chomp;
	    my $k = $_;
	    $k =~ s/^.*?://;
	    $host_atf{$k} = $_;
	}
	close(ATF);
    }

    if (open(CAT, '01bld/lists/cat-ids.lst')) {
	while (<CAT>) {
	    chomp;
	    my $k = $_;
	    $k =~ s/^.*?://;
	    $host_cat{$k} = $_;
	}
	close(CAT);
    }

    if (-r '00lib/proxy.ol') {
	# this will write 01bld/proxy-ol.lst
	warn "o2-lst.plx: proxy.ol not yet implemented\n";
	$proxy_lst = "01bld/proxy-ol.lst";
    } elsif (-r '00lib/proxy.lst') {
	$proxy_lst = "00lib/proxy.lst";
    }

    if ($proxy_lst) {

	open(P,$proxy_lst);
	my @p = (<P>); chomp(@p);
	close(P);

	my %pa_seen = ();
	my %px_seen = ();
	open(PX, '>01bld/lists/proxy-cat.lst');
	open(PA, '>01bld/lists/proxy-atf.lst');
	my $lnum = 0;
	foreach my $p (@p) {
	    ++$lnum;
	    $p =~ tr/\r\n//d;
	    # if the proxy has a ':' we want it for its atf:
	    # if it has no catalogue, default it to the source project
	    if ($p =~ /^(.*?):/) {
		my $proxproj = $1;
		$p .= "\@$proxproj" unless $p =~ /\@/;
		my($id) = ($p =~ /:(.*?)\@/);
		if ($host_atf{$id}) {
		    warn "$proxy_lst:$lnum: ignoring proxy of $id because $project already has that ATF\n";
		} elsif ($pa_seen{$id}++) {
		    warn "$proxy_lst:$lnum: ignoring duplicate ATF proxy for $id\n";
		} else {
		    print PA "$p\n";
		}
	    }
	    # if the proxy has a '@' we want it for its cat unless the project eq host
	    if ($p =~ /\@(.*)$/) {
		my $xc = $1;
		if ($xc ne $project) {
		    # if there is no ':' default it to host project
		    $p = "$project:$p" unless $p =~ /:/;
		    my($id) = ($p =~ /:(.*?)\@/);
		    if ($host_cat{$id}) {
			warn "$proxy_lst:$lnum: ignoring proxy of $id because $project CAT already has it\n";
		    } elsif ($px_seen{$id}++) {
			warn "$proxy_lst:$lnum: ignoring duplicate CAT proxy for $id\n";
		    } else {
			print PX "$p\n";
		    }
		}
	    }
	    # just ignore bare proxies (i.e., those with no : or @) for now.
	}
	close(PA);
	close(PX);
    }
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
	my @pubsub = ();
	if (-r '00lib/umbrella.lst') {
	    ## either use the list provided by the project
	    open(U,'00lib/umbrella.lst') || die "o2-lst.plx: strange: can't open readable file '00lib/umbrella.lst'\n";
	    while (<U>) {
		chomp;
		push @pubsub, split(/\s+/,$_);
	    }
	    close(U);
	    @pubsub = map { "$_/01bld/lists/approved.lst" } @pubsub;
	} else {
	    ## or automatically include all approved texts from
	    ## all public subprojects
	    grep(m#/#, `projpublic.sh $project`);	    
	    chomp @pubsub;
	    @pubsub = map { s#^$project/## ; 
			    "$_/01bld/lists/approved.lst" } @pubsub;
	}
	open(A, '>01bld/lists/proxy-atf.lst')
	    || die "o2-lst.plx: can't write 01bld/lists/proxy-atf.lst\n";
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
	link '01bld/lists/proxy-atf.lst', '01bld/lists/proxy-cat.lst';
    } elsif ($opt eq 'static') {
	if (-r '00lib/approved.lst') {
	    xsystem 'cp', '-a', '00lib/approved.lst', $listdir;
	} else {
	    xsystem 'touch', "$listdir/approved.lst";
	}
    } elsif ($opt =~ /\.ol$/) {
	## ol('outlined.lst', $opt);
	warn "o2-lst.plx: .ol lists not yet implemented\n";
    } elsif ($opt eq 'approved') {
	warn "o2-lst.plx: `approved' is no longer a valid value for `build-approved-policy'\n";
	warn "o2-lst.plx: please edit 00lib/config.xml and change `approved' to `static'\n";
    } else {
	warn "o2-lst.plx: unknown build-approved-policy value `$opt'\n";
    }

# WATCHME: this was called before update_lists() and also here in the middle of it--which is right?
#    proxy_lists();

    qualify_approved();

    $opt = `oraccopt . build-outlined-policy`;

    if (!$opt || $opt eq 'approved') {
	unless (-s $out_approved) {
	    system 'touch', $out_approved;
	}
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
    
    if (-r $have_atf && !-z _) {
	xsystem 'cp', '-p', "$have_atf", "$listdir/withatf"; ### FIXME: rationalize these
	xsystem "atflists.plx -p$project $out_approved - $have_atf >$listdir/sansatf";
	if (-r "$listdir/have-lem.lst") {
	    xsystem 'cp', '-p', "$listdir/have-lem.lst", "$listdir/withlem";
	    xsystem "atflists.plx -p$project $out_approved - $listdir/withlem >$listdir/sanslem";
	} else {
	    xsystem 'touch', "$listdir/withlem", "$listdir/sanslem";
	}
    }
}

sub
qualify_approved {
    my @a = ();
    open(A, $out_approved) || return; # just ignore missing lists/approved.lst
    @a = (<A>); chomp @a;
    close(A);
    open(A, ">$out_approved") || die;
    foreach my $a (@a) {
	$a = "$project:$a" unless $a =~ /:/;
	print A $a, "\n";
    }
    close(A);
}

sub
xsystem {
    warn "xsystem @_\n" if $verbose;
    system @_;
}

1;
