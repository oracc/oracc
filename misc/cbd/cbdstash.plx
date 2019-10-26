#!/usr/bin/perl
use warnings; use strict;

my %args = (); my @args = qw/bases check culled dir done entries fixed help init 
    locdata repeat senses show status update work/; @args{@args} = ();
my %lang_args = (); my @lang_args = qw/bases entries fixed senses/; @lang_args{@lang_args} = ();

my %status = ();

my %funcs = (
    bases    => \&bases,
    check    => \&check,
    culled   => \&culled,
    dir      => \&dir,
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
    work     => \&work,
    );

my %helps = (
    bases    => 'stash the bases work',
    check    => 'check that no manual edits have been made according to the current stash',
    culled   => 'stash glossary from which unreferenced forms and bases have been culled',
    dir	     => 'print the name of the working directory for the current stash',
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
    work     => 'set the current working language to LANG'
    );

my @sequence = qw/notused init entries senses fixed culled bases done/;
my %sequence = (); @sequence{@sequence} = (0 .. $#sequence+1);

#########################################################################################

my $arg1 = shift @ARGV;
my $arg2 = shift @ARGV;
my $arg3 = shift @ARGV;
my $lang = '';
my $argphase = shift @ARGV; # only with repeat sux entries, for example
my $repeat = 0;

if ($arg1) {
    if (exists $args{$arg1}) {
	if ($arg1 eq 'status') {
	    # arg2 is optional lang, arg3 is optional phase
	    if ($arg2) {
		$lang = $arg2;
		if ($arg3) {
		    $arg2 = $arg3;
		} else {
		    $arg2 = '' unless $arg2 eq '#current';
		}
	    }
	}
	unless ($arg1 eq 'dir' || $arg1 eq 'init' || $arg1 eq 'help') {
	    status_load();
	    $lang = $status{'#current'} unless $arg1 eq 'status' && $arg2 && $arg2 eq '#current';
	}
	&{$funcs{$arg1}};
	status_dump() unless $arg1 eq 'status' || $arg1 eq 'help';
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

sub culled {
    phase_save('culled');   
}

sub dir {
    print stashdir();
    exit 0;
}

sub done {
#    fixed();
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
#    die "$0: must give LANG with @lang_args action\n"
#	unless $lang;
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
    if ($arg2) {
	$lang = $arg2;
	die "$0: LANG=arg2 but no glossary 00lib/$lang.glo. Stop.\n"
	    unless -r "00lib/$lang.glo";
    } else {
	die "$0: must give LANG with 'init', e.g., cbdstatus.plx init sux\n";
    }
    my $newiso = `date +\%Y-\%m-\%d`;
    chomp $newiso;
    my $newdir = "00etc/stash/$newiso";
    if (-d $newdir) {
	die "$0: stash $newiso already exists; use 'update' if you want to stash a new set of files\n";
    } else {
	system 'mkdir', '-p', $newdir;
	setstatus("init");
	setstatus($lang,'#current');
	stash($newdir, 'init');
    }
}

sub locdata {
    my $d = stashdir();
    if ($arg2) {
	print "$d/$arg2-loc-data.xz";
    } else {
	print "$d/init-loc-data.xz";
    }
}

sub phase_check {
    my $p = shift;
    my $s = getstatus();
    die "$0: unknown status $p\n" unless $sequence{$p};
    if ($repeat || ($sequence{$p} - $sequence{$s}) == 1) {
	if ($p eq 'entries' || $p eq 'senses' ) {
	    unless (-r "$lang-$p-aligned.glo" && -r "$lang-$p-edited.glo") {
		die "$0: must have both $lang-$p-aligned.glo and $lang-$p-edited.glo\n";
	    } else {
		# todo: make sure -edited is later than -aligned
	    }
	} elsif ($p eq 'culled') {
	    unless (-r "$lang-culled-forms.glo" && -r "$lang-culled-bases.glo") {
		die "$0: must have both $lang-forms-$p.glo and $lang-bases-$p.glo\n";
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
	if ($p eq 'culled') {
	    if (-r "$lang-culled-forms.glo" && -r "$lang-culled-bases.glo") {
		if (-s "$lang-culled-forms.glo" && -s "$lang-culled-bases.glo") {
		    system 'cp', '-va', "$lang-culled-forms.glo", $d;
		    system 'cp', '-va', "$lang-culled-bases.glo", $d;
		    system 'cp', '-va', "$lang-culled-bases.glo", "00lib/$lang.glo";
		} else {
		    die "$0: either of $lang-culled-forms.glo or $lang-culled-bases.glo is empty. Stop\n";
		}
	    }
	} else {
	    if (-r "$lang-$p-aligned.glo") { # there isn't an aligned glo for bases or fixed
		if (-s "$lang-$p-edited.glo") {
		    system 'cp', '-va', "$lang-$p-aligned.glo", $d;
		} else {
		    die "$0: $lang-$p-aligned.glo is empty. Stop.\n";
		}
	    }		
	    if (-r "$lang-$p-edited.glo") { # there isn't an edited glo for fixed
		if (-s "$lang-$p-edited.glo") {
		    system 'cp', '-va', "$lang-$p-edited.glo", $d;
		    system 'cp', '-va', "$lang-$p-edited.glo", "00lib/$lang.glo";
		} else {
		    die "$0: $lang-$p-edited.glo is empty. Stop.\n";
		}
	    }
	    system 'cp', '-va', "00lib/$lang.glo", $d;
	}
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
    my $newstatus = $arg2;
    if ($newstatus && $newstatus ne '#current') {
	setstatus($newstatus,$lang);
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

sub status_load {
    %status = ();
    my $stashdir = stashdir();
    die "$0: can't read status from $stashdir/status\n" unless
	open(S,"$stashdir/status");
    while (<S>) {
	my($l,$s) = (/(\S+)\s+(\S+)\s*$/);
	die "$0: corrupt status file $stashdir/status\n" unless $l && $s;
	$status{$l} = $s;
    }
    close(S);
}

sub status_dump {
    my $stashdir = stashdir();
    open(S,">$stashdir/status") || die "$0: unable to save status to $stashdir/status\n";
    foreach my $l (sort keys %status) {
	print S "$l\t$status{$l}\n";
    }
    close(S);
}

sub getstatus {
    my $l = shift @_;
    $l = $lang unless $l;
    $status{$l};
}

sub setstatus {
    my ($s,$l) = @_;
    $l = $lang unless $l;
    $status{$l} = $s;
    status_dump();
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
    if ($iso) {
	"00etc/stash/$iso";
    } else {
	'';
    }
}

sub stashiso {
    my $iso = undef;
    if (-r '00etc/stash') {
	$iso = `ls 00etc/stash | tail -1`;
	chomp $iso;
    }
    return $iso;
}

1;
