#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Util;
use Getopt::Long;

my $lang = '';
my $autovars = 0;
my $coresig = '';
my $drop_zero = 1;
my %header = ();
my $in_sense = '';
my @instsigs = ();
my $name = '';
my $oracc = "$ENV{'ORACC'}";
my $out = '';
my $project = `oracc2 project`;
my %sig = ();
my @sigfiles = ();
my @sigprojects = ();

GetOptions (
    'lang:s'=> \$lang,
    'name:s'=> \$name,
    'out:s' => \$out,
    'project:s'=>\$project,
    'sig:s' => \@sigfiles,
    );

die "sig-merger.plx: must give '-lang' attribute with lang to process\n"
    unless $lang;

if ($#sigfiles < 0) {
    if (open(P,'00lib/projects.lst')) {
	my @p = (<P>); close(P);
	chomp @p;
	@sigfiles = map { "$oracc/bld/$_/$lang/glossary.sig" } @p;
    }
    die "sig-merge.plx: no sig-files given for merging\n"
	unless $#sigfiles >= 0;
}

$out = "01bld/$lang/merged.sig" unless $out;
open(O,">$out") || die "sig-merger.plx: can't open `$out' for output\n";
select O;

load_sigfile("01bld/$lang/cbd.sig", 1);
foreach my $s (@sigfiles) {
    load_sigfile($s, 0);
}

dump_sigs();

######################################################################################

sub
dump_sigs {
    foreach (sort keys %sig) {
	my $i = mysum(map { /;(\d+)/ } @{$sig{$_}});
	# The correct format for this is no-percent column; this is what sig-g2x expects
	print "$_\t$i\t@{$sig{$_}}\n";
    }
}

sub
lang_of {
    $_[0] =~ /^(.*?)-/;
    return $1;
}

sub
load_sigfile {
    my($file,$first) = @_;
    open(S,$file) || die "sig-merge.plx: can't open sigs `$file' for input\n";
    while (<S>) {
	next if /^\s*$/;
	if (/^\@?(proj(?:ect)?|name|lang)\s+(\S+)/) {
	    if ($first) {
		$header{$1} = $2;
	    } else {
		my($field,$value) = ($1,$2);
		if ($field eq 'project') {
		    if (lang_of($header{'project'}) ne lang_of($value)) {
			my $main_lang = lang_of($header{'project'});
			my $curr_lang = lang_of($value);
			die "sig-merge.plx: can't merge $curr_lang with $main_lang\n"; 
		    }
		}
	    }
	    next;
	}
	chomp;
	my($sig,$freq,$pct,$refs,$pers) = split(/\t/,$_);
	
	if ($freq && $refs) {
	    foreach my $ref (split(/\s+/,$refs)) {
		unless ($ref =~ /:/) {
		    my($sproj) = (m/(\@.*?):/);
		    $ref = "\@$sproj\:$ref;$freq;\%$pct;$pers";
		}
		$sig =~ s/\@(.*?)\%/\@$project\%/g;
		$sig =~ s/\#.*$//;
		$sig =~ s#\\.*?=#=#;
		push @{$sig{$sig}}, $ref;
	    }
	} elsif (!$drop_zero) {
	    push @{$sig{$sig}}, "$project\:0" unless $first;
	}
    }
    close(S);
}

sub
mysum {
    my $x = 0;
    foreach (@_) {
	$x += $_;
    }
    $x;
}

1;
