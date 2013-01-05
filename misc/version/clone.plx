#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

# FIXME: this should also fix problems in .ods files

my $oracc = $ENV{'ORACC'};
my($from,$as) = @ARGV;

if (!$as) {
    $as = $from;
    $from = `oraccopt`;
}

if (!-d "$oracc/$as") {
    if (-d "$oracc/$from/$as") {
	$as = "$from/$as";
    }
}

my $fixconfig_xsl = "$oracc/src/misc/version/clone-config.xsl";

if (!$from || !$as) {
    die "clone.plx: usage: clone.plx [FROM-PROJECT] [AS-PROJECT]\n";
}

if (!-e "$oracc/$from/00lib/config.xml") {
    die "clone.plx: no such project '$from' to clone from\n";
} elsif (!-d "$oracc/$as") {
    die "clone.plx: no such project '$as' to clone as\n";
}

system 'cp', '-a', "$oracc/$as/00lib/config.xml", "$oracc/$as/01tmp/config.xml";

my @zerodirs = grep(-d $_, eval("<$oracc/$from/00*>"));
foreach my $d (@zerodirs) {
    system 'rm', '-fr', "$oracc/$as/$d";
    system 'cp', '-a', "$d", "$oracc/$as";
}

system 'find', "$oracc/$as", '-name', '*~', '-exec', 'rm', '{}', ';';
system 'find', "$oracc/$as", '-name', '*.bak', '-exec', 'rm', '{}', ';';

my @atf = eval("<$oracc/$as/00atf/*.*>");
foreach my $a (@atf) {
    fix_atf_project($a,$as);
}

my @glo = eval("<$oracc/$as/00lib/*.glo>");
foreach my $g (@glo) {
    fix_glo_project($g,$as);
}

fix_config_xml();

system "(cd $oracc/$as ; fixlinks.plx $from $as)";

my $owner = $as;
$owner =~ s#/.*$##;
system 'chown', '-R', "$owner:$owner", "$oracc/$as";

1;

########################################################################

sub
dump_file {
    open(A,">$_[0]");
    print A $_;
    close(A);
}

sub
fix_atf_project {
    $_ = load_file($_[0]);
    s/\#project:\s*(.*?)\s*$/#project: $_[1]\n/m;
    dump_file($_[0]);
}

sub
fix_config_xml {
    system 'xsltproc', '-o', "$oracc/$as/01tmp/new-config.xml", $fixconfig_xsl, 
    	"$oracc/$as/00lib/config.xml";
    system 'mv', "$oracc/$as/01tmp/new-config.xml", "$oracc/$as/00lib/config.xml";
}

sub
fix_glo_project {
    $_ = load_file($_[0]);
    s/\@(proj.*?)\s.*$/\@$1 $_[1]\n/m;
    dump_file($_[0]);
}

sub
load_file {
    local($/) = undef;
    warn "loading file $_[0]\n";
    open(A,$_[0]);
    $_ = <A>;
    close(A);
    $_;
}
