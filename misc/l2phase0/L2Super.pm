package ORACC::L2P0::L2Super;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/chatty super_warn super_die/;
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Builtins;
use ORACC::L2GLO::Util;
use POSIX qw(strftime);


# use Data::Dumper;

$ORACC::L2P0::L2Super::chatty = 1;

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

my $base = '';
my $level = '';
my $lang = '';
my $force = 0;
my $project = '';

my %function_info = (
    induct =>[ FI_BASE_NO,  '00raw', 'glo'],
    compare=>[ FI_BASE_YES, '00src', 'glo'],
    prepare=>[ FI_BASE_NO,  '00map', 'map'],
    merge  =>[ FI_BASE_YES, '01tmp', 'new'],
    getsigs=>[ FI_BASE_NO,  '00map', 'map'],
    );

$ORACC::L2GLO::Builtins::O2_cancel = 0;

my $function = undef;
my $last_outputdate;
my %return_data = ();

sub
init {
    GetOptions(
	'base=s'=>\$base,
	force=>\$force,
	'lang=s'=>\$lang,
	'level=i'=>\$level,
	'project=s'=>\$project,
	);

    $return_data{'force'} = $force;

    $function = $0; $function =~ s/^(.*?)super-//; $function =~ s/\.plx$//;
    super_die("unsupported function $function")
	unless $function_info{$function};
    my($flags, $dir, $type) = @{$function_info{$function}};

    super_die("failed to get function info") 
	unless defined($flags) && $dir && $type;

    my $argfile = shift @ARGV;
    if ($argfile) {
	super_die("file '$argfile' does not exist")
	    unless -e $argfile;
	super_die("file '$argfile' exists but cannot be read")
	    unless -r $argfile;
	my ($argfile_dir, $argfile_project, $argfile_lang, $argfile_type) 
	    = ($argfile =~ m#(.*?)/(.*?)~(.*?)\.(.*?)$#);

	super_die("expected filename to have format DIR/PROJECT~LANG.EXT, not $argfile")
	    unless $argfile_dir;
	super_die("expected file to be in $dir, but it is in $argfile_dir")
	    unless $dir eq $argfile_dir;
	super_die("expected file to have extension .$type but it has .$argfile_type")
	    unless $type eq $argfile_type;
	super_die("can't use -project/-lang when giving argument file")
	    if $project || $lang;

	$project = $argfile_project;
	$lang = $argfile_lang;
    } else {
	super_die("must use both of -project and -lang or give .$type file on command line")
	    unless $project && $lang;
	$argfile = "$dir/$project~$lang.$type";
	super_die("expected to use file $argfile but it doesn't exist")
	    unless -e $argfile;
	super_die("expected to use file $argfile but it can't be read")
	    unless -e $argfile;
    }

    $return_data{'project'} = $project;
    $return_data{'project'} =~ tr#-#/#;
    $return_data{'lang'} = $lang;

    if ($base) {
	super_die("base glossary $base does not exist\n")
	    unless -e $base;
	super_die("base glossary $base can't be read\n")
	    unless -r $base;
    } else {
	my @glo = <00lib/*.glo>;
	super_die("a super-glossary must have a .glo file\n")
	    unless $#glo >= 0;
	super_die("a super-glossary is only allowed one .glo file\n")
	    unless $#glo == 0;
	$base = shift @glo;
	super_die("base glossary $base does not exist\n")
	    unless -e $base;
	super_die("base glossary $base can't be read\n")
	    unless -r $base;
    }

    my $baselang = $base; $baselang =~ s#^.*?/(.*?)\.glo$#$1#;
    $return_data{'base'} = $base;
    $return_data{'baselang'} = $baselang;
    $return_data{'baseproj'} = $baseproj;

    if ($flags & FI_BASE_YES) {
	my $arg_xml = ORACC::L2GLO::Builtins::acd2xml($base);
	super_die("errors in $base") unless $arg_xml;
	undef $arg_xml;
    }

    if ($type eq 'glo' || $type eq 'new') {
	my $arg_xml = ORACC::L2GLO::Builtins::acd2xml($argfile);
	super_die("errors in $argfile") unless $arg_xml;
	undef $arg_xml;
    } elsif ($type eq 'map') {
	my($mapref,$mapgloref) = parse_mapfile($argfile);
	super_die("errors in $argfile")
	    unless $mapref;
	$return_data{'mapref'} = $mapref;
	$return_data{'mapgloref'} = $mapgloref;
    }

    # always open the argfile because it's the most reliable way of checking for existence;
    # caller can just close $return_data{'input_fn'} if it's unwanted.
    ($return_data{'input'},$return_data{'input_fh'})  = setup_argfile($argfile);
    if ($function eq 'induct') {
	($return_data{'output'},$return_data{'output_fh'}) 
	    = setup_file('>', '00src', $project, $lang, 'glo');
    } elsif ($function eq 'compare') {
	($return_data{'output'},$return_data{'output_fh'}) 
	    = setup_file('>', '00map', $project, $lang, 'map');
    } elsif ($function eq 'prepare') {
	$return_data{'outmap'} = setup_file('>', '01tmp', $project, $lang, 'map');
	$return_data{'outglo'} = setup_file('>', '01tmp', $project, $lang, 'new');
    } elsif ($function eq 'merge') {
	my $argmap = $argfile;
	$argmap =~ s/new$/map/;
	backup_file($argfile, '00bak', $project, $lang, 'glo');
	backup_file($argmap, '00bak', $project, $lang, 'map');
	($return_data{'map'},$return_data{'map_fh'})   = setup_file('<', '01tmp', $project, $lang, 'map');
	($return_data{'outmap'},$return_data{'outmap_fh'}) 
	    = setup_file('>', '00map', $project, $lang, 'map');
	($return_data{'outglo'},$return_data{'outglo_fh'}) 
	    = setup_file('>', '00lib', '', $baselang, 'glo');
    } elsif ($function eq 'getsigs') {
	($return_data{'output'},$return_data{'output_fh'}) 
	    = setup_file(undef, '00sig', $project, $lang, 'sig');
	$return_data{'outputdate'} = $last_outputdate;
    }

    return %return_data;
}

##########################################################################################################

sub
backup_file {
    my($from) = shift @ARGV;
    my $to = $from;
    super_die("no such file $from to back up")
	unless -r $from;
    $to =~ s/^(00lib|01tmp)/00bak/;
    super_die("can't write backup $to when trying to back up $from")
	unless -w $to;

    my $isodate = strftime("%Y%m%d", gmtime());
    $to =~ s/\./-$isodate./;
    my $to_wild = $to;
    $to_wild =~ s/\./-*./;
    my @bak;
    eval "@bak = <$to_wild>";
    my $version = '000';
    if ($#bak >= 0) {
	my @sbak = sort @bak;
	$version = pop @sbak;
	($version) = ($version =~ /(\d\d\d)\./);
	++$version;
    }
    $to =~ s/\./-$version./;
    super_warn("backing up $from to $to");
    system "mv $from $to";
}

sub
parse_mapfile {
    my %glo = ();
    my @map = ();
    my $map_status = 0;
    open(M, $_[0]) || super_die("weird error: unable to open map file $_[0]");
    while (<M>) {
	if (/^add/) {
	    my ($cfgwpos, $partsref, $senseref) = parse_map($_);
	    if ($cfgwpos) {
		if ($partsref) {
		    my @parts = @$partsref;
		    if ($#parts >= 0) {
			push @{$glo{$cfgwpos}}, [ 0 , "\@parts $parts[0]\n" ];
		    }
		}
		my @senses = @$senseref;
		if ($#senses >= 0) {
		    foreach my $s (@senses) {
			push @{$glo{$cfgwpos}}, [ 1 + scalar keys %glo, $s ];
		    }
		} else {
		    unless ($glo{$cfgwpos}) { # silently ignore add entry that comes after add sense for same word
			@{$glo{$cfgwpos}} = ();
		    }
		}
	    } else {
		++$map_status;
	    }
	} else {
	    push @map, $_;
	}
    }
    close(M);
    (\@map, \%glo);
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
    my $file = "$dir/$proj~$lang.$type";
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
chatty {
    if ($ORACC::L2P0::L2Super::chatty) {
	warn "super $function: ", @_, "\n";
    }
}

sub
super_die {
    die "super $function: @_. Stop.\n";
}
sub
super_warn {
    warn "super $function: @_.\n";
}

1;
