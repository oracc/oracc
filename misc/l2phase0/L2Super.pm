package ORACC::L2P0::L2Super;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/chatty super_warn super_die glo_compare 
	     map_drop_act map_dump map_unload/;
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Builtins;
use ORACC::L2GLO::Util;
use POSIX qw(strftime);
use File::Copy "cp";

use Data::Dumper;

$ORACC::L2P0::L2Super::chatty = 1;
$ORACC::L2GLO::Builtins::bare = 1;
my $EMULT = 1000;

use constant {
    CL_WORD    => 1,
    CL_SENSE   => 2,
    CL_FULL    => 4,
    CL_NONE    => 8,
    FI_BASE_NO => 16,
    FI_BASE_YES=> 32,
};

use Getopt::Long;

my $baseproj = `proj-from-conf.sh`;

my $argmap = '';
my $argbase = '';
my $argsrc = '';

my $dryrun = 0;
my $force = 0;
my $lang = '';
my $level = '';
my $project = '';

my %function_info = (
    induct =>[ FI_BASE_NO,  '00raw', 'glo'],
    compare=>[ FI_BASE_YES, '00src', 'glo'],
    prepare=>[ FI_BASE_YES, '00map', 'map'],
    map    =>[ FI_BASE_YES, '00map', 'map'],
    merge  =>[ FI_BASE_YES, '01tmp', 'new'],
    getsigs=>[ FI_BASE_NO,  '00map', 'map'],
    );

$ORACC::L2GLO::Builtins::O2_cancel = 0;

my $function = undef;
my $last_outputdate;
my %return_data = ();

my $basedata = undef;
my $basefile = undef;
my %basehash = ();
my $baselang = undef;

my %map = ();
my %map_comments = ();
my %map_glo = ();
my %map_line = ();
my %map_sort = ();
my $mapfile = '';

my $newfile = '';

my $srcdata = undef;
my $srcfile = undef;
my %srchash = ();


sub
init {
    GetOptions(
	'base=s'=>\$argbase,
	'compare=s'=>\$argsrc,
	'dryrun'=>\$dryrun,
	force=>\$force,
	'lang=s'=>\$lang,
	'level=i'=>\$level,
	'map'=>\$argmap,
	'project=s'=>\$project,
	);

    $return_data{'dryrun'} = $dryrun;
    $return_data{'force'} = $force;

    $function = $0; $function =~ s/^(.*?)super-//; $function =~ s/\.plx$//;
    super_die("unsupported function $function")
	unless $function_info{$function};
    my($flags, $dir, $type) = @{$function_info{$function}};

    super_die("failed to get function info") 
	unless defined($flags) && $dir && $type;

    my $argfile = shift @ARGV;
    if ($argfile) {
	super_die("can't use command line options when giving argument file")
	    if $project || $lang || $argbase || $argsrc || $argmap;
	super_die("file '$argfile' does not exist")
	    unless -e $argfile;
	super_die("file '$argfile' exists but cannot be read")
	    unless -r $argfile;
	my ($argfile_dir, $argfile_project, $argfile_lang, $argfile_type) 
	    = ($argfile =~ m#(.*?)/(.*?)~(.*?)\.(.*?)$#);

	if ($argfile_dir) {
	    super_die("expected file to be in $dir, but it is in $argfile_dir")
		unless $dir eq $argfile_dir;
	    super_die("expected file to have extension .$type but it has .$argfile_type")
		unless $type eq $argfile_type;
	} else {
	    super_die("expected filename to have format DIR/PROJECT~LANG.EXT, not $argfile")
		unless $function eq 'induct';
	}

	unless ($project && $lang) {
	    if ($function eq 'induct') {
		($argfile_project,$argfile_lang) = projlang_from_glo_header($argfile);
	    }
	}

	$return_data{'project'} = $project = $argfile_project unless $project;
	$return_data{'lang'} = $lang = $argfile_lang unless $lang;
	
    } else {
	if ($argbase || $argsrc) {
	    super_die("must give both -b and -c and optional -m")
		unless $argbase && $argsrc;
	    $basefile = $argbase;
	    $srcfile = $argsrc;
	    $mapfile = $argmap;
	} else {
	    super_die("must use both of -project and -lang or give .$type file on command line")
		unless $project && $lang;
	    $argfile = "$dir/$project~$lang.$type";
	    super_die("expected to use file $argfile but it doesn't exist")
		unless -e $argfile;
	    super_die("expected to use file $argfile but it can't be read")
		unless -e $argfile;
	}
    }
    
    # Make sure all the necessary file names are set

    unless ($basefile) {
	my @glo = <00lib/*.glo>;
	super_die("a super-glossary must have a .glo file")
	    unless $#glo >= 0;
	super_die("a super-glossary is only allowed one .glo file")
	    unless $#glo == 0;
	$basefile = shift @glo;
    }

    unless ($srcfile) {
	if ($function eq 'compare') {
	    $srcfile = $argfile;
	} elsif ($function eq 'merge') {
	    $srcfile = $newfile = $argfile;
	    $srcfile =~ s/^...../00src/; $srcfile =~ s/\....$/.glo/;
	} elsif ($function eq 'prepare' || $function eq 'map' || $function eq 'getsigs') {
	    $srcfile = $argfile;
	    $srcfile =~ s/^00map/00src/;
	    $srcfile =~ s/map$/glo/;
	}
    }

    unless ($mapfile) {
	if ($function eq 'prepare' || $function eq 'map' || $function eq 'getsigs') {
	    $mapfile = $argfile;
	} elsif ($function eq 'compare' || $function eq 'merge') {
	    $mapfile = $srcfile;
	    $mapfile =~ s/^00src/00map/;
	    $mapfile =~ s/\....$/.map/;
	}
    }

    # now check and internalize all the necessary files

    if ($basefile) {
	if ($flags & FI_BASE_YES) {
	    super_die("base glossary $basefile does not exist")
		unless -e $basefile;
	    super_die("base glossary $basefile can't be read")
		unless -r $basefile;
	    chatty("checking $basefile ... ");
	    my $arg_xml = ORACC::L2GLO::Builtins::acd2xml($basefile);
	    super_die("errors in $basefile") unless $arg_xml;
#	    chatty_ok();
	    undef $arg_xml;
	    $basedata = ORACC::L2GLO::Builtins::input_acd($basefile);
	    %basehash = %{$$basedata{'ehash'}};
	    glo_add_senses(\%basehash);
	}

    	$baselang = $basefile; 
	$baselang =~ s#^.*?/([^/]+?)\.glo$#$1#;
	$return_data{'basefile'} = $basefile;
	$return_data{'baselang'} = $baselang;
	$return_data{'baseproj'} = $baseproj;
	$return_data{'basehash'} = \%basehash;
    }

    if ($srcfile) {
	chatty("checking $srcfile ... ");
	my $arg_xml = ORACC::L2GLO::Builtins::acd2xml($srcfile);
	super_die("errors in $srcfile") unless $arg_xml;
#	chatty_ok();
	undef $arg_xml;
	$srcdata = ORACC::L2GLO::Builtins::input_acd($srcfile);
	%srchash = %{$$srcdata{'ehash'}};
	glo_add_senses(\%srchash);
	$return_data{'srcfile'} = $srcfile;
	$return_data{'srchash'} = \%srchash;
    }

    if ($mapfile) {
	if (-r $mapfile) {
	    map_load($mapfile);
	    chatty("checking $mapfile ... ");
	    super_die("errors checking map file $mapfile")
		if map_check();
	}
    }

    if ($newfile) {
	chatty("checking $newfile ... ");
	my $arg_xml = ORACC::L2GLO::Builtins::acd2xml($newfile);
	super_die("errors in $newfile") unless $arg_xml;
	undef $arg_xml;
	$return_data{'newfile'} = $newfile;
    }

    if ($function eq 'induct') {
	($return_data{'input'},$return_data{'input_fh'}) 
	    = setup_argfile($argfile);
	($return_data{'output'},$return_data{'output_fh'}) 
	    = setup_file('>', '00src', $project, $lang, 'glo');
    } elsif ($function eq 'compare') {
 	backup_file($mapfile);
	($return_data{'output'}) = setup_file(undef, '00map', $project, $lang, 'map');
    } elsif ($function eq 'prepare') {
	($return_data{'outmap'}) = setup_file(undef, '01tmp', $project, $lang, 'map');
	($return_data{'outglo'},$return_data{'outglo_fh'}) = setup_file('>', '01tmp', $project, $lang, 'new');
    } elsif ($function eq 'merge') {
	backup_file($basefile);
	backup_file($mapfile);
	($return_data{'outmap'}) = setup_file(undef, '01tmp', $project, $lang, 'map');
	($return_data{'outglo'}) = setup_file(undef, '01tmp', '', $baselang, 'glo');
    } elsif ($function eq 'getsigs') {
	($return_data{'output'}) = setup_file(undef, '00sig', $project, $lang, 'sig');
	$return_data{'outputdate'} = $last_outputdate;
    }

    if ($function eq 'prepare') {
	map_add2glo();
	%map = map_drop_act('add');
	map_sort_by_lines();
	map_dump($return_data{'outmap'});
    }
 
    $return_data{'mapfile'} = $mapfile;
    $return_data{'map'} = \%map;
    $return_data{'map_comments'} = \%map_comments;
    $return_data{'map_glo'} = \%map_glo;
    $return_data{'map_line'} = \%map_line;
    $return_data{'map_sort'} = \%map_sort;

    return %return_data;
}

##########################################################################################################

sub
backup_file {
    my($from) = shift;
    my $to = $from;

    # this is not an error--it just means the file being backed up doesn't exist yet
#    super_die("no such file $from to back up")
#	unless -r $from;
    return unless -r $from;

    $to =~ s/^(0[01]...)/00bak/;

    my $isodate = strftime("%Y%m%d", gmtime());
    $to =~ s/\./-$isodate./;
    my $to_wild = $to;
    $to_wild =~ s/\./-\*./;
    my @bak = eval("<$to_wild>");
#    warn "to_wild = $to_wild => @bak\n";
    my $version = '000';
    if ($#bak >= 0) {
	my @sbak = sort @bak;
	$version = pop @sbak;
	($version) = ($version =~ /(\d\d\d)\./);
	++$version;
    }
    $to =~ s/\./-$version./;
    super_warn("backing up $from to $to");
    cp($from,$to) or super_die("backup of $to failed");
}

sub
chatty {
    if ($ORACC::L2P0::L2Super::chatty) {
	print STDERR "super $function: ", @_, "\n";
    }
}

sub
projlang_from_glo_header {
    my $glo = shift;
    my $project = '';
    my $lang = '';
    open(G,$glo) || super_die("can't read glossary $glo");
    while (<G>) {
	next if /^\s*$/;
	if (/^\@project\s+(.*?)\s*$/) {
	    $project = $1;
	} elsif (/^\@lang\s+(.*?)\s*$/) {
	    $lang = $1;
	}
	last if /^\@entry/;
    }
    close(G);
    super_die("failed to obtain project and lang from glossary $glo")
	unless $project && $lang;
    ($project,$lang);
}

sub
setup_argfile {
    my $file = shift @_;
    my $fh = undef;
    unless (open($fh,$file)) {
	super_die("cannot open $file for reading");
    }
    return ($file,$fh);
}

sub
setup_file {
    my($io,$dir,$proj,$lang,$type) = @_;
    $proj =~ tr#/#-#;
    my $file = ($proj ? "$dir/$proj~$lang.$type" : "$dir/$lang.$type");
    my $fh = undef;
    $last_outputdate = (stat($file))[9];
    system 'mkdir', '-p', $dir
	unless -d $dir;
    if ($io) {
	unless (open($fh,$io,$file)) {
	    my $rw = ($io eq '<' ? 'read' : 'write');
	    super_die("cannot open $file for $rw");
	}
    }
    return ($file,$fh);
}

sub
super_die {
    die "super $function: @_. Stop.\n";
}

sub
super_warn {
    warn "super $function: @_.\n";
}

#########################################################################################################

sub
glo_add_senses {
    my $hashref = shift;
    foreach my $e (keys %$hashref) {
	next if $e =~ m#//#;
	my %b = %${$$hashref{$e}};
	foreach my $s (@{$b{'sense'}}) {
	    my($epos,$sense) = ($s =~ /^(\S+)\s+(.*)$/);
	    my $newsense = $e;
	    $newsense =~ s#\]#//$sense]#;
	    $newsense .= "'$epos";
	    $$hashref{$newsense} = $$hashref{$e};
	}
    }
}

sub
glo_compare {
    %map = map_drop_act('new');
    foreach my $e (keys %srchash) {
	next if $e =~ m,//,;
	my $mapped = 0;
	my %e = %${$srchash{$e}};
	my $eline = $e{'#line'} * $EMULT;
	$map_sort{$e} = $eline;

	unless (defined $basehash{$e}) {
	    $map{$e} = [ 'new', 'entry', $e , '' ] unless $map{$e};
	} else {
	    my %s = ();
	    my %b = %${$basehash{$e}};
	    @s{@{$b{'sense'}}} = ();
	    foreach my $s (sort @{$e{'sense'}}) {
		unless (exists $s{$s}) {
		    my($epos,$sense) = ($s =~ /^(\S+)\s+(.*)$/);
		    my $newsense = $e;
		    $newsense =~ s#\]#//$sense]#;
		    $newsense .= "'$epos";
		    $map_sort{$newsense} = $eline;
		    $map{$newsense} = [ 'new', 'sense', $newsense, '' ] unless $map{$newsense};
		}
	    }
	}
    }
}

##########################################################################################

sub
map_add2glo {
    my $map_status = 0;
    foreach my $v (values %map) {
	if ($$v[0] eq 'add') {
	    my ($cfgwpos, $partsref, $senseref) = map_parse_add($v);
	    if ($cfgwpos) {
		if ($partsref) {
		    my @parts = @$partsref;
		    if ($#parts >= 0) {
			push @{$map_glo{$cfgwpos}}, [ 0 , "\@parts $parts[0]\n" ];
		    }
		}
		my @senses = @$senseref;
		if ($#senses >= 0) {
		    foreach my $s (@senses) {
			push @{$map_glo{$cfgwpos}}, [ 1 + scalar keys %map_glo, $s ];
		    }
		} else {
		    unless ($map_glo{$cfgwpos}) { # silently ignore add entry that comes after add sense for same word
			@{$map_glo{$cfgwpos}} = ();
		    }
		}
	    } else {
		++$map_status;
	    }
	}
    }
}

sub
map_check {
    my $status = 0;
    if (!scalar keys %map_sort) {
	map_sort_by_lines();
    }
    foreach my $m (sort { $map_sort{$$a[2]} <=> $map_sort{$$b[2]} } values %map) {
	my($act,$type,$sig,$map) = @$m;
	unless ($srchash{$sig}) {
	    my $mapentry = map_line(@$m);
	    super_warn "$mapfile:$map_line{$mapentry}: $sig not in source glossary";
	    ++$status;
	}
	if ($act eq 'map' || $act eq 'fix') {
	    unless ($basehash{$map}) {
		my $mapentry = map_line(@$m);
		super_warn "$mapfile:$map_line{$mapentry}: => $map not in base glossary";
		++$status;
	    }
	}
    }
    return $status;
}

sub
map_drop_act {
    my $act = shift;
    my %map_no_new = %map;
    my @deletiae = ();
    foreach my $m (keys %map) {
	my $mm = $map{$m};
	push(@deletiae,$m) 
	    if $$mm[0] eq $act;
    }
    foreach my $d (@deletiae) {
	delete $map_no_new{$d};
    }
    %map_no_new;
}

sub
map_drop_new {
    return map_drop_act('new');
}

sub
map_dump {
    my $mapout = shift;
    $mapout = $mapfile unless $mapout;
    my @v = values %map;
    foreach my $v (@v) {
	warn "no sort code for $$v[2]\n" unless $map_sort{$$v[2]};
    }
    chatty("writing new map file $mapout");
    open(M,">$mapout") || super_die("can't write to map file $mapout"); select M;
    foreach my $w (sort { $map_sort{$$a[2]} <=> $map_sort{$$b[2]} } values %map) {
	my $line = "$$w[0] $$w[1] $$w[2]";
	$line .= " => $$w[3]" if $$w[3];
	print "$line\n";
	print join("\n", @{$map_comments{$line}}), "\n"
	    if $map_comments{$line};
    }
    close(M);
}

sub
map_line {
    my($act,$type,$sig,$map) = @_;
    my $ret = "$act $type $sig";
    $ret .=  " => $map" if $map;
    $ret;
}

sub
map_load {
    my $m = shift;
    chatty("loading map file $m");
    open(M, $m) || die "super compare: can't read map file $m\n";
    my $map_curr = '';
    my $status = 0;
    while (<M>) {
	chomp;
	if (/^\#/) {
	    push @{$map_comments{$map_curr}}, $_;
	    next;
	} else {
	    $map_curr = $_;
	}
	s/\s+/ /g;
	$map_line{$_} = $.;
        my($act,$type,$sig) = (/(\S+)\s+(\S+)\s+(.*?)$/);
        my $map = '';
	unless ($sig) {
	    super_warn "$m:$.: bad map entry";
	    ++$status;
	    next;
	}
        # next if $act eq 'new';
        if ($sig =~ /^(.*?)\s*=>\s*(.*?)$/) {
            ($sig,$map) = ($1,$2);
        }
	if ($map{$sig}) {
	    super_warn "$m:$.: $sig has more than one action" and next unless $sig;
	} else {
	    $map{$sig} = [ $act, $type, $sig, $map ];
	}
    }
    close(M);
    super_die("errors loading mapfile $m") 
	if $status;
}

sub
map_parse_add {
    my $v = shift;
    $_ = $$v[2];
    if ($$v[1] eq 'entry') {
	if ($srchash{$_}) {
	    my %e = %${$srchash{$_}};
	    s/\[/ [/; s/\]/] /;
	    return ($_, $e{'parts'}, $e{'sense'})
	} else {
	    warn "super prepare: entry $_ not found in source glossary $srcfile\n";
	    return ();
	}
    } else {
	my($cf,$gw,$sense,$pos,$epos) = (m#^(.*?)\[(.*?)//(.*?)\](.*?)\'(.*?)$#);
	if ($cf) {
	    return ("$cf [$gw] $pos", [ ], [ "$epos $sense" ]);
	} else {
	    super_warn("$_ bad sense specification in add");
	    return ();
	}
    }
}

sub
map_sort_by_lines {
    foreach my $v (values %map) {
	my $l = map_line(@$v);
	$map_sort{$$v[2]} = $map_line{$l};
    }
}

sub
map_unload {
    %map = ();
    %map_comments = ();
}

1;
