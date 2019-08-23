#!/usr/bin/perl
use warnings; use strict;

my %args = (); my @args = qw/check done help show start status update/; @args{@args} = ();

my %funcs = (
    check    => \&check,
    done     => \&done,
    help     => \&help,
    show     => \&show,
    start    => \&start,
    status   => \&status,
    update   => \&update,
    );

my %helps = (
    check    => 'check that no manual edits have been made according to the current stash',
    done     => 'mark the edits for which the current stash was created are complete',
    help     => 'print this help',
    show     => 'show the ISODATE of the current stash',
    start    => 'start a new stash, saving 00atf/*.atf, 00lib/*.glo, and all lemdata',
    status   => 'print the status of the current stash, with no trailing newline',
    update   => 'replace the stashed files with a fresh set; only valid if status=init',
    );

#########################################################################################

my $arg = shift @ARGV;

if (exists $args{$arg}) {
    &{$funcs{$arg}};
} else {
    warn "$0: unknown argument.\n";
    help();
    exit 1;
}

#########################################################################################

sub check {
    my $dir = stashdir();
    foreach my $g (<$dir/*.glo>) {
	my $og = $g;
	$og =~ s#^.*?/([^/]+)$#00lib/$1#;
	warn "checking $og vs $g\n";
	my $minglo = `cbdminglo.sh $og`; chomp $minglo;
	system "cbdminglo.sh <$g | diff - $minglo";
	if ($? == 0) {
	    warn "$0: $g and $og have same entries/senses\n";
	} else {
	    warn "$0: $og has been edited manually since being stashed.\n";
	}
    }
}

sub done {
    my $status = getstatus();
    if ($status eq 'done') {
	my $curriso = stashiso();
	warn "$0: current stash $curriso already marked 'done'\n";
    } elsif ($status eq 'edit') {
	setstatus('done');
    } elsif ($status eq 'init') {
	my $curriso = stashiso();
	warn "$0: current stash $curriso has no edits.\n";
    } else {
	warn "$0: unknown status '$status'\n";
    }
}

sub help {
    warn "\n$0: The following arguments are allowed\n\n";
    foreach my $a (sort @args) {
	warn "\t$a\t$helps{$a}\n";
    }
    warn "\n";
}

sub show {
    print stashiso();
}

sub start {
    my $newiso = `date +\%Y-\%m-\%d`;
    chomp $newiso;
    my $newdir = "00etc/stash/$newiso";
    if (-d $newdir) {
	die "$0: stash $newiso already exists; use 'update' if you want to stash a new set of files\n";
    } else {
	system 'mkdir', '-p', $newdir;
	setstatus("init");
	stash($newdir);
    }
}

sub status {
    my $newstatus = shift @ARGV;
    if ($newstatus) {
	if ($newstatus eq 'edit') { 
	    setstatus('edit');
	} else {
	    die "$0: only allowed value for status is 'edit'. Stop.\n";
	}
    } else {
	print getstatus();
    }
}

sub update {
    my $status = getstatus();
    if ($status eq 'init') {
	stash(stashdir());
    } else {
	die "$0: update only allowed when status==init. Stop.\n";
    }
}

########################################################################################

sub getstatus {
    my $stashdir = stashdir();
    `cat $stashdir/status`;
}

sub setstatus {
    my $stashdir = stashdir();
    system "/bin/echo -n $_[0] >$stashdir/status";
}

sub stash {
    my $newdir = shift;
    system 'cp', '-pPR', '00atf', $newdir;
    foreach my $g (<00lib/*.glo>) {
	system 'cp', '-pPR', $g, $newdir;
    }
    system "wid2lem <01bld/lists/have-xtf.lst | xz >$newdir/loc-data.xz";
}

sub stashdir {
    my $iso = stashiso();
    "00etc/stash/$iso";
}

sub stashiso {
    my $iso = `ls 00etc/stash | tail -1`;
    chomp $iso;
    return $iso;
}

1;
