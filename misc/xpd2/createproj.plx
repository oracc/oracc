#!/usr/bin/perl
use warnings; use strict;
use lib "$ENV{'ORACC'}";
use Getopt::Long;

my $project = '';
my $name = '';
my $abbrev = '';
my $init = 0;
my $project_type;
my $content_type = 'corpus';
my $phase = 'create';
my $project_user = '';
my $project_proj = '';
my $projperm = '';
my $textlist = '';
my $user_project = 0;
my $superproject = '';
my $verbose = 0;

sub xsystem;

GetOptions(
    'abbrev:s'=>\$abbrev,
    'init'=>\$init,
    'name:s'=>\$name,
    'project:s'=>\$project,
    'textlist:s'=>\$textlist,
    'user'=>\$user_project,
    );

create_project();

#########################

sub
create_project {
    my @missing = ();
    push @missing, '-abbrev' unless $abbrev;
    push @missing, '-name' unless $name;
    push @missing, '-project' unless $project;

    if ($#missing >= 0) {
	die "createproj.plx: can't create project: missing options @missing\n";
    }

    if ($user_project) {
	$project_type = 'user';
	$project_user = $superproject 
	    = user_project_user($project) unless $superproject;
	$project_proj = user_project_proj($project);
	$projperm = 'userperm';
    } else {
	$project_type = 'oracc';
	$project_user = $project;
	$project_proj = $project;
	$projperm = 'userperm';
    }
    
    if ($ENV{'ORACC_MODE'} ne 'single') {
	my $projuid = getpwnam($project_user);
	if (!$projuid) {
	    die "createproj.plx: no such project user $project_user\n";
	}
	if ($projuid != $>) {
	    die "createproj.plx: ${project_user}'s uid = $projuid but EUID = $>\n";
	}
    }

    my $home_d = ($ENV{'ORACC_MODE'} eq 'multi') ? '/home' : $ENV{'ORACC_HOME'};

    unless (chdir "$home_d/$project") {
	die "createproj.plx: could not change directory to $home_d/$project\n";
    }


    # Projects are now initialized with esp2-create.sh but after creating 00lib/config.xml
##
##    open(INDEX,">00web/index.html");
##    print INDEX <<EOI;
##<html><body onload="location='/$project/corpus'">
##</body></html>
##EOI
##    close(INDEX);
##

    if ($init) {
	system "cp $ENV{'ORACC'}/lib/data/p3colours.txt 00lib"
	    unless -e "00lib/p3colours.txt";
    }

    my $ctype = 'local';
    open(P,">00lib/config.xml");
    print P <<EOP;
<project n="$project" 
    xmlns="http://oracc.org/ns/xpd/1.0"
    xmlns:xpd="http://oracc.org/ns/xpd/1.0">
  <name>$name</name>
  <abbrev>$abbrev</abbrev>
  <project-type>$project_type</project-type>
  <type>$content_type</type>
  <logo type="text">$abbrev</logo>
EOP

if ($init) {
    print P <<EOO;
  <option name="build-approved-policy" value="atf"/>
  <option name="build-outlined-policy" value="atf"/>
  <option name="catalog-build-policy" value="auto"/>
  <option name="cbd-mode" value="mixed"/>
  <option name="outline-default-select" value="true"/>
  <option name="outline-default-sort-fields" value="period,genre,provenience"/>
  <option name="outline-default-sort-labels" value="Time,Genre,Place"/>
EOO
} else {
    copy_options("../00lib/config.xml");
}
    print P '</project>';
    close(P);
    if ($textlist && $textlist ne 'approved.lst') {
	xsystem 'cp',$textlist,'00lib/approved.lst';
    } else {
	xsystem 'touch', '00lib/approved.lst';
    }
    xsystem ("cd 00lib; ln -sf approved.lst outlined.lst");
    xsystem 'touch', '00lib/rejected.lst', '00lib/new.lst';
    xsystem "$ENV{'ORACC'}/bin/esp2-create.sh";
    xsystem "$ENV{'ORACC'}/bin/$projperm.sh", $project;
    exit 0;
}

sub
user_project_proj {
    my $tmp = shift;
    $tmp =~ s#^([^/]+/)##;
    return $tmp;
}

sub
user_project_user {
    $_[0] =~ m#^(.*?)/#;
    return $1;
}

sub
xsystem {
    warn "$phase: system @_\n" if $verbose;
    system @_;
}

sub
copy_options {
    my @opts = `xsltproc $ENV{'ORACC'}/lib/scripts/copy-options.xsl $_[0]`;
    print P @opts;
}
