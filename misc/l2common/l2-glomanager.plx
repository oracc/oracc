#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use lib "$ENV{'ORACC'}/lib";
use Getopt::Long;
use Pod::Usage;
use ORACC::Log::Util;
use ORACC::XML;
use ORACC::NS;
use ORACC::XCF::Util;
use ORACC::XPD::Util;
use ORACC::L2GLO::Builtins;
use Encode;

binmode STDIN, ':utf8';
binmode STDOUT, ':utf8';
binmode STDERR, ':utf8';

my %builtins = (
    add_ids =>\&ORACC::L2GLO::Builtins::add_ids,
    input   =>\&ORACC::L2GLO::Builtins::input,
    link    =>\&ORACC::L2GLO::Builtins::link,
    load_acd=>\&ORACC::L2GLO::Builtins::load_acd,
    merge   =>\&ORACC::L2GLO::Builtins::merge,
    output  =>\&ORACC::L2GLO::Builtins::output,
    sort    =>\&ORACC::L2GLO::Builtins::sort,
    web     =>\&ORACC::L2GLO::Builtins::web,
    );

my @sys_builtins = qw/mkdir chown chmod/;
my %sys_builtins = ();
@sys_builtins{@sys_builtins} = ();

my $abbrev = '';
my $argpid = 0;
my $auto = 0;
my $bare = 0;
my $cbd = undef;
my $cbdlang = '';
my $check = 0;
my $config = undef;
my $clang = '';
my $debug = 0;
my $force = 0; # not used; just for oraccproject
my $glofile = '';
my $inplace = 0;
my $merge = 0;
my $mlist = 0;
my $mlang = 'en';
my $noconfig = 0;
my $noletters = 0;
my $output = '';
my $project = '';
my $projectDir = '';
my %project_params = ();
my %extra_params = ();
my %results = ();
my $rws = '';
my $scripts = "$ENV{'ORACC'}/share/scripts/config";
my $sort = 0;
my $title = '';
my $validate = 0;
my $verbose = 0;
my $xff = 0;
my $xml = '';
my $web = '';
my $webdir = '';
my %cfg = ();

GetOptions(
    'abbrev:s'=>\$abbrev,
    'argpid:s'=>\$argpid,
    'auto'=>\$auto,
    'bare'=>\$bare,
    'check:s'=>\$check,
    'config:s'=>\$config,
    'cbdlang:s'=>\$clang,
    'debug'=>\$debug,
    'force'=>\$force,
    'inplace'=>\$inplace,
    'merge'=>\$merge,
    'mlist:s'=>\$mlist,
    'mnglang:s'=>\$mlang,
    'noconfig'=>\$noconfig,
    'noletters'=>\$noletters,
    'output:s'=>\$output,
    'project:s'=>\$project,
    'rws:s'=>\$rws,
    'sort:s'=>\$sort,
    'title:s'=>\$title,
    'validate'=>\$validate,
    'verbose' =>\$verbose,
    'webdir:s'=>\$webdir,
    'xml:s'=>\$xml,
    'web:s'=>\$web,
    ) || pod2usage(1);

#    'xff+'=>\$xff,

$ORACC::L2GLO::Builtins::bare = $bare;
$ORACC::L2GLO::debug = $debug;
$ORACC::L2GLO::Builtins::noletters = $noletters;
$ORACC::L2GLO::verbose = $verbose;

if (!$project && !$sort && !$xml) {
    $project = `oraccopt`;
    if (!$project) {
	die "l2-glomanager.plx: must run in project directory with valid 00lib/config.xml\n";
	exit(1);
    }
}

my $cbd_post_process = undef;
$cbd_post_process = `oraccopt $project cbd-post-process`
    if $project;
if ($cbd_post_process && !$clang) {
    if ($xml =~ m#(?:^|/)([^/.]).glo$#) {
	$clang = $xml;
    }
}

my $projectPath = "$ENV{'ORACC_HOME'}/$project";

$output =~ s/glo\.normcbd/cbd/ if $output;

if ($sort) {
    my @header = ();
    open(S, $sort);
    while (<S>) {
	if (/^\@(?:project|lang|name)/) {
	    push @header, $_;
	}
    }
    close(S);
    my $s = ORACC::L2GLO::Builtins::input_acd($sort,$clang,undef,undef);
    ORACC::L2GLO::Builtins::acd_sort($s);
    if ($inplace) {
	open(O,">$sort") || die "l2-glomanager.plx: can't open $sort for sort output\n";
	select O;
    }
    print @header, "\n";
    foreach my $e (@{$$s{'entries'}}) {
	ORACC::L2GLO::Builtins::acd_dump_entry($e);
    }
    close(O) if $inplace;
    exit 0;
} elsif ($merge) {
    my $d = `date +%Y-%m-%d`;
    $d =~ tr/-//d;
    chomp $d;
    if ($mlist) {
	die "l2-glomanager.plx: must give -cbdlang with -mlist\n"
	    unless $clang;
	my @m = split(/,/,$mlist);
	my $m1 = shift @m;
	ORACC::L2GLO::Builtins::merge_exec($clang, $m1, [@m], $output);
    } else {
	foreach my $lang (<00lib/*.new>) {
	    $lang =~ s#.*?/(.*?).new#$1#;
	    if (-e "00lib/$lang.glo") {
		my $save = "00bak/$lang-$d.glo";
		my($orig,$mrg,$out) = ("00lib/$lang.glo","00lib/$lang.new","00lib/$lang.mrg");
		system 'cp', $orig, $save;
		print "merge_exec $lang $orig $mrg $out\n";
		ORACC::L2GLO::Builtins::merge_exec($lang,$orig, [ $mrg ], $out);
		system 'mv', '-f', $out, $orig;
	    } else {
		system 'mv', '-f', "00lib/$lang.new", "00lib/$lang.glo";
	    }
	}
    }
    exit(0);
} elsif ($xml || $check) {
    $glofile = $xml || $check;
    $cbdlang = $glofile;
    $cbdlang =~ s#^00lib/(.*?)\.glo$#$1#;

    if ($cbdlang =~ /^(sux|qpn)/) {
#	system('l2-sux-norm.plx', $glofile); # this is forced in g2a-simple now
	$glofile =~ s/00lib/01bld/;
	$glofile .= ".norm";
    }
    my $xdoc = ORACC::L2GLO::Builtins::acd2xml($glofile,$clang||'sux',$title || 'Glossary',$title || 'Glossary');
    if ($check) {
	#  on error we died in acd2xml
	if ($glofile =~ /sux/) {
	    if (-r '00src/sux.glo') {
		$glofile =~ s/01bld/00src/;
		$glofile =~ s/\.norm$//;		
	    } else {
		$glofile =~ s/01bld/00lib/;
		$glofile =~ s/\.norm$//;
	    }
	    system 'cbdbases.plx', '-check', $glofile;
	}
	warn "Glossary $check OK\n";
	exit 0;
    } elsif ($output) {
	open(O,">$output") || die "l2-glomanager.plx: can't open $output for xml\n";
	binmode O, ':raw';
	print O $xdoc->toString();
    } else {
	binmode STDOUT,':raw';
	print $xdoc->toString();
    }
    maybe_cbd_post_process();
    exit 0;
} elsif ($web) {
    my @verbose = ();
    push @verbose, '-verbose' if $verbose;
    system 
	"$ENV{'ORACC'}/bin/g2manager.plx",
	'-cbdlang',$web,
	'-project',$project,
	'-config',"$ENV{'ORACC'}/lib/config/cbd-phase2.xcf",
	@verbose;
    exit 0;
}

if ($config && !-r $config) {
    $config = "$ENV{'ORACC'}/lib/config/$config";
}
$config = '00lib/cbd.xcf' unless $config;
$config = '00lib/glossary.xcf' unless -r $config;
$config = undef unless -r $config;

unless ($abbrev) {
    $abbrev = "$project $cbdlang";
}

if ($config) {
    my $ret = ORACC::XCF::Util::load($config,0,
				   abbrev=>$abbrev,
				   basename=>$project,
				   projectDir=>$projectPath,
				   cbdlang=>$clang,
				   mnglang=>$mlang,
				   title=>$title,
				   argpid=>$argpid,
				   webdir=>$webdir,
	);
    %cfg = %$ret;
    setprojparm('title','abbrev','basename');
    $extra_params{'subdir'} = "'${$cfg{'cbdlang'}}[0]'";
    $extra_params{'webdir'} = "'$webdir'";
} else {
    die "l2-glomanager.plx: no -config option and no file 00lib/cbd.xcf\n"
      unless $check;
}

mkdir ${$cfg{'webglodir'}}[0] if $cfg{'webglodir'};

#use Time::HiRes qw/gettimeofday tv_interval/;
#my $t0 = $ORACC::L2GLO::Builtins::t0 = [gettimeofday];
foreach my $p (@{$cfg{'process'}}) {
    my %p = %$p;
    my $args = $p{'args'} ? " $p{'args'}" : '';
#    if ($verbose) {
#	my $elapsed = tv_interval($t0);
#	my $elstr = sprintf("%.02fs",$elapsed);
#	warn "$elstr: process $p{'action'} $args...\n";
#    }
    if ($p{'action'} =~ /\.xsl/) {
	my $styler = load_xsl($p{'action'});
	die "l2-glomanager.plx: no such XSL $p{'action'}\n" unless $styler;
	my $subject = $cbd;
	if ($p{'input'}) {
	    if ($results{$p{'input'}}) {
		$subject = $results{$p{'input'}};
		warn "using cached input $p{'input'}\n" if $verbose;
	    } else {
		warn "loading input $p{'input'}\n" if $verbose;
		$subject = load_xml($p{'input'});
		die "l2-glomanager.plx: load_xml failed on $p{'input'}\n"
		    unless $subject;
	    }
	}
	my %process_params = ();
	if ($p{'params'}) {
	    foreach my $param (split(/;/,$p{'params'})) {
		my($par,$val) = ($param =~ /^(.*?)=(.*)$/);
		$process_params{$par} = "'$val'";
	    }
	}
	if ($verbose) {
	    warn("\tproject_params:\n");
	    foreach my $p (sort keys %project_params) {
		warn("\t\t$p=$project_params{$p}\n");
	    }
	    warn("\textra_params:\n");
	    foreach my $p (sort keys %extra_params) {
		warn("\t\t$p=$extra_params{$p}\n");
	    }
	    warn("\tprocess_params:\n");
	    foreach my $p (sort keys %process_params) {
		warn("\t\t$p=$process_params{$p}\n");
	    }
	}
	my $result = eval {
	    $styler->transform($subject, 
			       %project_params, 
			       %extra_params,
			       %process_params
	    );
	};
	if ($@) {
	    die "XML::LibXSLT: $@\n";
	}
	$cbd = $result if $p{'cbdresult'};
	
	if ($p{'output'}) {
	    $results{$p{'output'}} = $result;
	    warn ("writing output $p{'output'}\n") if $verbose;
	    open(O,">$p{'output'}") || die "l2-glomanager.plx: $p{'output'}: open failed\n";
	    binmode O, ':raw'; # work around a XML::LibXSLT encoding bug
#	    print O $styler->output_string($result);
	    if ($p{'text'} && $p{'text'} eq 'yes') {
		my $x = $result->toString();
		$x =~ s/^<\?xml.*?\?>\n?//;
		print O $x;
	    } else {
		print O $result->toString();
	    }
#	    my $ures = $styler->output_as_bytes($result);
#	    Encode::_utf8_on($ures);
#	    print O $ures;
	    close(O);
	} elsif ($p{'result'}) {
	    $results{$p{'result'}} = $result;
	}
	undef $styler;
    } elsif ($builtins{$p{'action'}}) {
	$cbd = &{$builtins{$p{'action'}}}($p,\%cfg,$cbd);
    } elsif (-x $p{'action'}) {
	if ($p{'cbdresult'}) {
	    my @res = `$p{'action'} $args`;
	    $cbd = load_xml_string(join('',@res));
	} else {
	    system "$p{'action'} $args";
	}
    } elsif ($p{'action'} =~ /^(\S+)/ && exists $sys_builtins{$1}) {
	system $p{'action'};
    } else {
	die "l2-glomanager.plx: unknown/nonexistent action $p{'action'}\n";
    }
}

maybe_cbd_post_process();

#if ($verbose) {
#    my $elapsed = tv_interval($t0);
#    my $elstr = sprintf("%.02fs",$elapsed);
#    warn "$elstr: completed.\n";
#}

#########################################################################

sub
add_project_options {
    my @opts = ORACC::XPD::Util::all_options();
    foreach my $o (@opts) {
	$project_params{$o} = ORACC::XPD::Util::option($o);
	$ORACC::L2GLO::Builtins::accents = 1 
	    if $o eq 'render-accents' && $project_params{$o} eq 'yes';
    }
}

sub
maybe_cbd_post_process {
    if ($cbd_post_process) {
	system "$ENV{'ORACC'}/bin/$cbd_post_process", $clang;
    }
}

sub
setprojparm {
    foreach my $p (@_) {
	if ($#{$cfg{$p}} == 0) {
	    $project_params{$p} = "'${$cfg{$p}}[0]'";
	} elsif ($#{$cfg{$p}} > 0) {
	    warn "l2-glomanager.plx: project parameter '$p' given more than once\n";
	} else {
	    warn "l2-glomanager.plx: missing project parameter '$p'\n";
	}
    }
    $project_params{'dateversion'} = "'${$cfg{'dateversion'}}[0]'";
    $project_params{'project'} = "'$project'";
    $project_params{'projectDir'} = "'$projectPath'";
    $project_params{'lang'} = "'$clang'";
    $project_params{'abbrev'} = "'$abbrev'";

    add_project_options() unless $noconfig;
}

1;

__END__

=head1 NAME

g2manager.plx -- manage Corpus-Based Dictionaries

=head1 SYNOPSIS

g2manager.plx [options]

=head1 OPTIONS

=over

=item B<-config>

Give the name of the configuration file; see 
http://oracc.museum.upenn.edu/doc/CBD/#config
for configuration info.

=item B<-validate>

Perform validation phases and exit.

=item B<-verbose>

Make noise about what is happening.

=back

=head1 DESCRIPTION

Program to manage Corpus-Based Dictionaries.

=head1 AUTHOR

Steve Tinney (stinney@sas.upenn.edu)

=head1 COPYRIGHT

Copyright(c) Steve Tinney 2007.

Released under the Gnu General Public License
(http://www.gnu.org/copyleft/gpl.html).

