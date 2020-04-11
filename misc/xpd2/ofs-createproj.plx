#!/usr/bin/perl
use warnings; use strict;
use POSIX qw(strftime);
use File::Copy "cp";
use integer;

sub xsystem;

my($project,$command,$file,@args) = @ARGV;

die "$0: must give 'init' or 'subproject' then short and long project names\n"
    unless $command;

my $pwd = `pwd`; chomp $pwd;
my $begin = `date +%s`;
warn "Begin command $command in $pwd\n";

## Now we give the project name as arg1 on the cli
#my $project = `osc-proj-from-conf.sh`;
#unless ($project) {
#   if ($command eq 'init') {
#	$project = `id -u -n`;
#	chomp $project;
#   } else {
#   	die "ofs-createproj.plx: unable to determine project name from 00lib/config.xml. Stop.\n";
#   }
#}

my $bin = "$ENV{'ORACC'}/bin";

my($short,$long) = ($file, join(' ',@args));
subproject_die("Must give short name and long name") unless $short && $long;
my $lsub = "\L$short";
if (length $lsub < 4) {
    die "oracc: subproject must be at least four characters long\n";
}
if ($lsub !~ m/^[a-z][a-z0-9]+/) {
    die "oracc: subproject must contain only letters and numbers\n";
}
if ($command eq 'subproject') {
    if (-r '00lib/config.xml') {
	if ($project) {
	    my $argproj = "$project/$lsub";
	    xsystem "$bin/subproject.sh $lsub";
	    xsystem "(cd $lsub; $bin/createproj.plx -abbr $project/$short -proj $argproj -name '$project/$long' -user)";
	} else {
	    subproject_die("no project name found in 00lib/config.xml");
	}
    } else {
	subproject_die("No 00lib/config.xml in parent project");
    }
} else {
    $project = $lsub unless $project;
    xsystem "$bin/createproj.plx -abbr $short -proj $project -name '$long' -init";
}

sub
subproject_die {
    warn "oracc $command failed: ", @_, "\n\n";

    print <<E;
Use

    oracc init ABBREVIATED_NAME FULL NAME MULTI WORD


To initialize a new top-level project (eop-level projects must be
created by Oracc administrators, but subprojects can be created by
project staff).

Use

    oracc subproject ABBREVIATED_NAME FULL NAME MULTI WORD

To create and initialize a new subproject.

In both cases, give the abbreviated name and the full name of the
project, leaving out the name of the parent project for subprojects.

A subproject name may not contain forward slashes, i.e., you must
create nested subprojects as subprojects of the immediate parent 
project.

You may want to use uppercase or mixed case in the abbreviation--as
ePSD does in its top-level project, for example.  This is fine: the
abbreviation will retain its uppercase characters, but the subproject
will be created by first converting all uppercase letters to lowercase
and then creating directories using the all-lowercase version.

E
    exit(1);
}

sub
xsystem {
#    warn "system @_\n";
    system @_;
}
1;

