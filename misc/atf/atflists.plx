#!/usr/bin/perl
use warnings; use strict;

my $first_list = 1;
my $need_outfile = 0;
my $need_project = 0;
my $op = undef;
my $optional = 0;
my $outfile = '';
my $project = '';
my %runninglist = ();
my $unqualified_ok = 0;
my $unqualified_force = 0;
my $verbose = 0;

foreach my $arg (@ARGV) {
    if ($need_outfile) {
	$outfile = $arg;
	$need_outfile = 0;
    } elsif ($need_project) {
	$project = $arg;
	$need_project = 0;
#	warn "atflists.plx: project=$project\n";
    } elsif ($arg =~ /^-v/) {
	++$verbose;
    } elsif ($arg =~ /^-o/) {
	$arg =~ s/^-o//;
	if (length $arg) {
	    $outfile = $arg;
	} else {
	    $need_outfile = 1;
	}
    } elsif ($arg =~ /^-p/) {
	$arg =~ s/^-p//;
	if (length $arg) {
	    $project = $arg;
	} else {
	    $need_project = 1;
	}
    } elsif ($arg =~ /^-u/) {
	$unqualified_ok = 1;
    } elsif ($arg =~ /^-U/) {
	$unqualified_ok = $unqualified_force = 1;
    } elsif ($arg =~ /^([-+&])(\?)?$/) {
	$op = $1;
	$optional = $2;
	print STDERR "op $arg\n" if $verbose;
    } else {
	my @ln = ();
	if ($arg eq 'stdin') {
	    while (<STDIN>) {
		push @ln, $_;
	    }
	} else {
	    next if $optional && !-r $arg;
	    open(L,$arg) || die "atflists.plx: no such list file $arg\n";
	    @ln = (<L>);
	    close(L);
	}

	@ln = map { tr/\n\r//d; $_ } @ln;
	@ln = grep (/^\&?[PQX]/, @ln); # only use lines that have PQX IDs
	if ($ln[0] && $ln[0] =~ /\@/) {
	    # remove XMD project from proxy lists
	    @ln = map { s/\@.*$//; $_ } @ln;
	}
	if ($unqualified_force) {
	    # Force atflists always to work in unqualified mode
	    @ln = map { s/^.*?://; $_ } @ln;
	} else {
	    # Force atflists always to work in qualified mode
	    @ln = default_project(@ln);
	}

	if ($first_list) {
	    @runninglist{@ln} = ();
	    $first_list = 0;
	    printf(STDERR "%d entries in running list $arg\n", 
		   scalar keys %runninglist)
		if $verbose;
	} else {
	    printf(STDERR "%d entries in current list $arg\n", $#ln + 1)
		if $verbose;
	    if ($op eq '+') { # union of curr runninglist
		@runninglist{@ln} = ();
	    } elsif ($op eq '-') { # drop runninglist members which are in curr
		foreach my $ln (@ln) {
		    delete $runninglist{$ln} if exists $runninglist{$ln};
		}
	    } elsif ($op eq '&') { # keep members common to runninglist and curr
		my %tmp = ();
		@tmp{@ln} = ();
		foreach my $r (keys %runninglist) {
		    delete $runninglist{$r} unless exists $tmp{$r};
		}
	    } else {
		die "atflists.plx: unhandled op $op\n";
	    }
	}
    }
}
printf(STDERR "%d entries in result list\n", scalar keys %runninglist)
    if $verbose;

if (scalar keys %runninglist > 0) {
    if ($outfile) {
	open(O,">$outfile")
	    || die "atflists.plx: can't write to output file `$outfile'\n";
	select O;
    }
    print join("\n", sort { &pqx_sort } keys %runninglist),"\n";
    close O if $outfile;
}

# mention ? on opts

# mention stdin as special file name

sub
default_project {
    my @new_ln = ();
    foreach (@_) {
	if (/:/ || $unqualified_ok) {
	    push @new_ln, $_;
	} else {
	    if ($project) {
		push @new_ln, "$project:$_";
	    } else {
		die "atflists.plx: must give '-p' or '-u' when using unqualifed lists\n";
	    }
	}
    }
    @new_ln;
}

sub
pqx_sort {
    my $tmp_a = $a;
    my $tmp_b = $b;
    $tmp_a =~ s/^.*?://;
    $tmp_b =~ s/^.*?://;
    $tmp_a cmp $tmp_b;
}

1;
