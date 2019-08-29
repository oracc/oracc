#!/usr/bin/perl
use warnings; use strict;

my %args = (); my @args = qw/bases check done entries help init repeat senses show status update/; @args{@args} = ();
my %lang_args = (); my @lang_args = qw/bases entries senses/; @lang_args{@lang_args} = ();

my %funcs = (
    check    => \&check,
    done     => \&done,
    entries  => \&entries,
    help     => \&help,
    init     => \&init,
    repeat   => \&repeat,
    senses   => \&senses,
    show     => \&show,
    status   => \&status,
    update   => \&update,
    );

my %helps = (
    bases    => 'stash the bases work',
    check    => 'check that no manual edits have been made according to the current stash',
    done     => 'mark the edits for which the current stash was created are complete',
    entries  => 'stash the entries work',
    help     => 'print this help',
    init     => 'initialize a new stash, saving 00atf/*.atf, 00lib/*.glo, and all lemdata',
    repeat   => 'do the last stash again, only allowed when status is bases, entries, or senses',
    senses   => 'stash the senses work',
    show     => 'show the ISODATE of the current stash',
    status   => 'print the status of the current stash, with no trailing newline',
    update   => 'replace the stashed files with a fresh set; only valid if status=init',
    );

my @sequence = qw/notused init entries senses bases done/;
my %sequence = (); @sequence{@sequence} = (0 .. $#sequence+1);

#########################################################################################

my $arg = shift @ARGV;
my $lang = shift @ARGV;
my $repeat = 0;

if ($arg) {
    if (exists $args{$arg}) {
	&{$funcs{$arg}};
    } else {
	warn "$0: unknown argument.\n";
	help();
	exit 1;
    }
} else {
	warn "$0: requires an argument.\n";
	help();
	exit 1;
}

#########################################################################################

sub bases {
    phase_save('bases');
}

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

sub entries {
    phase_save('entries');
}

sub help {
    warn "\n$0: The following arguments are allowed\n\n";
    foreach my $a (sort @args) {
	warn "\t$a\t$helps{$a}\n";
    }
    warn "\n";
}

sub phase_check {
    my $p = shift;
    my $s = getstatus();
    die "$0: unknown status $p\n" unless $sequence{$p};
    if (($sequence{$p} - $sequence{$s}) == 1) {
	unless (-r "$lang-$p-aligned.glo" && -r "$lang-$p-edited.glo") {
	    die "$0: must have both $lang-$p-aligned.glo and $lang-$p-edited.glo\n";
	} else {
	    # todo: make sure -edited is later than -aligned
	}
    } else {
	shift @sequence;
	warn "$0: you can't stash $p right after $s.\n";
	warn "$0: correct sequence is @sequence. Stop.\n";
	exit 1;
    }
    return 1;
}

sub phase_save {
    my $p = shift;
    die "$0: must give LANG with @lang_args action\n"
	unless $lang;
    if ($repeat || phase_check($p)) {
	my $d = stashdir();
	my $s = getstatus();
	system 'cp', '-va', "$d/$lang.glo", "$d/$lang.glo.$s"
	    unless $repeat;
	system 'cp', '-va', "$lang-$p-aligned.glo", $d;
	system 'cp', '-va', "$lang-$p-edited.glo", $d;
	system 'cp', '-va', "$lang-$p-edited.glo", "00lib/$lang.glo";
	system 'cp', '-va', "00lib/$lang.glo", $d;
	setstatus($p);
    }
}

sub repeat {
    my $p = getstatus();
    if (exists $lang_args{$p}) {
	$repeat = 1;
	phase_save($p);
    } else {
	die "$0: repeat only allowed with @lang_args\n";
    }
}

sub senses {
    phase_save('senses');
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
