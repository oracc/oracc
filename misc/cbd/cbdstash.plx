#!/usr/bin/perl
use warnings; use strict;

my %args = (); my @args = qw/bases check done entries fixed help init locdata repeat senses show status update/; @args{@args} = ();
my %lang_args = (); my @lang_args = qw/bases entries fixed senses/; @lang_args{@lang_args} = ();

my %funcs = (
    bases    => \&bases,
    check    => \&check,
    done     => \&done,
    entries  => \&entries,
    help     => \&help,
    fixed    => \&fixed,
    init     => \&init,
    locdata  => \&locdata,
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
    fixed    => 'stash the fixed corpus after aligning entries and senses',
    help     => 'print this help',
    init     => 'initialize a new stash, saving 00atf/*.atf, 00lib/*.glo, and all lemdata',
    locdata  => 'get the path to the current location data',
    repeat   => 'do the last stash again, only allowed when status is bases, entries, or senses',
    senses   => 'stash the senses work',
    show     => 'show the ISODATE of the current stash',
    status   => 'print the status of the current stash, with no trailing newline',
    update   => 'replace the stashed files with a fresh set; only valid if status=init',
    );

my @sequence = qw/notused init entries senses fixed bases done/;
my %sequence = (); @sequence{@sequence} = (0 .. $#sequence+1);

#########################################################################################

my $arg = shift @ARGV;
my $lang = shift @ARGV;
my $argphase = shift @ARGV; # only with repeat sux entries, for example
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
    setstatus('done');
    return;
    my $status = getstatus();
    if ($status eq 'done') {
	my $curriso = stashiso();
	warn "$0: current stash $curriso already marked 'done'\n";
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

sub fixed {
    die "$0: must give LANG with @lang_args action\n"
	unless $lang;
    if (phase_check('fixed')) {
	my $d = stashdir();
	system 'mv', '-v', "$d/00atf", "$d/00atf.orig";
	system 'cp', '-pPR', '00atf', $d;
	system "wid2lem <01bld/lists/have-xtf.lst | xz >$d/fixed-loc-data.xz";
	system 'cp', '-va', "00lib/$lang.glo", $d;
	setstatus('fixed');
    }
}

sub help {
    warn "\n$0: The following arguments are allowed\n\n";
    foreach my $a (sort @args) {
	warn "\t$a\t$helps{$a}\n";
    }
    warn "\n";
}

sub init {
    my $newiso = `date +\%Y-\%m-\%d`;
    chomp $newiso;
    my $newdir = "00etc/stash/$newiso";
    if (-d $newdir) {
	die "$0: stash $newiso already exists; use 'update' if you want to stash a new set of files\n";
    } else {
	system 'mkdir', '-p', $newdir;
	setstatus("init");
	stash($newdir, 'init');
    }
}

sub locdata {
    my $d = stashdir();
    if ($lang) { # overloaded to use 'init' or 'fixed' for locdata
	print "$d/$lang-loc-data.xz";
    } else {
	print "$d/init-loc-data.xz";
    }
}

sub phase_check {
    my $p = shift;
    my $s = getstatus();
    die "$0: unknown status $p\n" unless $sequence{$p};
    if ($repeat || ($sequence{$p} - $sequence{$s}) == 1) {
	unless ($p eq 'fixed') {
	    unless (-r "$lang-$p-aligned.glo" && -r "$lang-$p-edited.glo") {
		die "$0: must have both $lang-$p-aligned.glo and $lang-$p-edited.glo\n";
	    } else {
		# todo: make sure -edited is later than -aligned
	    }
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
	if (-r "$lang-$p-aligned.glo") { # there isn't an aligned glo for bases or fixed
	    system 'cp', '-va', "$lang-$p-aligned.glo", $d;
	}
	if (-r "$lang-$p-edited.glo") { # there isn't an edited glo for fixed
	    system 'cp', '-va', "$lang-$p-edited.glo", $d;
	    system 'cp', '-va', "$lang-$p-edited.glo", "00lib/$lang.glo";
	}
	system 'cp', '-va', "00lib/$lang.glo", $d;
	setstatus($p);
    }
}

sub repeat {
    my $p = getstatus();
    if (exists $lang_args{$p}) {
	$repeat = 1;
	if (-r "00lib/$lang.glo") {
	    if ($argphase) {
		if ($argphase eq 'fixed') {
		    fixed();
		} else {
		    if (exists $lang_args{$argphase}) {
			phase_save($argphase);
		    } else {
			die "$0: argphase=$argphase: repeat LANG PHASE only allowed with @lang_args\n";
		    }
		}
	    } else {
		phase_save($p);
	    }
	} else {
	    die "$0: no such glossary 00lib/$lang.glo\n";
	}
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

sub status {
    my $newstatus = $lang;
    if ($newstatus) {
	setstatus($newstatus);
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
    my($newdir,$p) = @_;
    system 'cp', '-pPR', '00atf', $newdir;
    foreach my $g (<00lib/*.glo>) {
	system 'cp', '-pPR', $g, $newdir;
    }
    system "wid2lem <01bld/lists/have-xtf.lst | xz >$newdir/$p-loc-data.xz";
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
