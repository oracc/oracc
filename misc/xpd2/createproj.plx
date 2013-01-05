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

    my $projuid = getpwnam($project_user);
    if (!$projuid) {
	die "createproj.plx: no such project user $project_user\n";
    }
    if ($projuid != $>) {
	die "createproj.plx: ${project_user}'s uid = $projuid but EUID = $>\n";
    }

    use Cwd;
    my $cdir = getcwd();

    if ($cdir ne "$ENV{'ORACC_HOME'}/$project") {
	die "createproj.plx: must run createproj.plx -create from $ENV{'ORACC_HOME'}/$project not $cdir\n";
    }

    open(INDEX,">00web/index.html");
    print INDEX <<EOI;
<html><body onload="location='/$project/corpus'">
</body></html>
EOI
    close(INDEX);

    if ($init) {
	system "cp $ENV{'ORACC'}/lib/data/p2colours.txt 00web"
	    unless -e "00web/colours.txt";
    } else {
	system "cp ../00web/p2colours.txt 00web"
	    if -r "00web/p2colours.txt";
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
  <option name="pager-glossaries" value="akk sux qpn"/>
  <option name="pager-outline" value="yes"/>
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
