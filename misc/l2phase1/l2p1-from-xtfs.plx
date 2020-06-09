#!/usr/bin/perl
use warnings; use strict;
use open 'utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::NS;
use Getopt::Long;
use ORACC::XPD::Util;

binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my $base = 'xtf';
my %char = (norm0=>'$',base=>'/',cont=>'+',morph=>'#',morph2=>'##');
my $cbd_mode = ORACC::XPD::Util::option('cbd-mode');
my $dyn_mode = ($cbd_mode && $cbd_mode eq 'dynamic');
my %exos = ();
my $lang = '';
my $new_mode = 0;
my %news = ();
my $output = undef;
my $project = `oraccopt`;
my $cbd_no_harvest = '';
my $proxy_mode = 0;
my %rws_map = (
    EG => '',
    OA => 'x-oldass',
    OB => 'x-oldbab',
    MA => 'x-midass',
    MB => 'x-midbab',
    NA => 'x-neoass',
    NB => 'x-neobab',
    SB => 'x-stdbab',
    );
my %sigs = ();
my %wordrefs = ();
my $textlist = ();
my $xtf_project = '';
my $verbose = 0;

GetOptions (
    'new'=>\$new_mode,
    'output:s'=>\$output,
    'proxy'=>\$proxy_mode,
    'textlist:s'=>\$textlist,
    'verbose'=>\$verbose
    );

#$output = '01bld/from-xtfs.sig'
#   unless $output;

if ($new_mode) {
    if (($cbd_no_harvest = `oraccopt . cbd-no-harvest`)) {
	cbd_no_harvest_setup();
    }
}

$base = 'prx' if $proxy_mode;

$textlist = '01bld/lists/have-lem.lst' unless $textlist;

open(T,$textlist) || die "l2p1-from-xtfs.plx: can't open '$textlist'\n";
while (<T>) {
    chomp;
#    warn "l2p1-from-xtfs.plx: processing $_\n";
    my $p = '';
    s/\@.*$//; # remove cat element from proxy.lst
    if (/^(.*?):(.*?)$/) {
	my($proj,$text) = ($1,$2);
	$p = sprintf("$ENV{'ORACC'}/bld/$proj/%s/$text/$text.xtf",four($text));
    } else {
	$p = sprintf("01bld/%s/$_/$_.xtf",four($_));
    }
    if (-r $p) {
	# if there is a .xsf read sigs from that because then we also harvest
	# instances from lemmatized exemplars
	my $xsf = $p;
	$xsf =~ s/xtf$/xsf/;
	if (-r $xsf) {
	    loadsigs($xsf);
	    warn("l2p1-from-xtfs.plx: reading $p\n")
		if $verbose;
	} else {
	    loadsigs($p);
	    warn("l2p1-from-xtfs.plx: reading $p\n")
		if $verbose;
	}
    } else {
	warn("l2p1-from-xtfs.plx: can't read $p\n")
	    unless $proxy_mode;
    }
}
close(T);

#if ($output) {
#    open O, ">$output"; select O;
#}

my @glores = ();
my @exores = ();
my @newres = ();

if ($new_mode) {
    foreach my $s (keys %news) {
	push @newres, "$s\t@{$news{$s}}\n";
    }
#    warn "newres has ", $#newres+1, " entries\n";
} else {
    foreach my $s (sort keys %sigs) {
	push @glores, "$s\t@{$sigs{$s}}\n";
    }
}

## if there is a lemm-path for the language in config.xml
## and . is not in the path, don't include the exosigs because
## we are harvesting for inclusion in the external glossary

# set up non-exo hash
# for each option if %option split value on space and look for '.' entry
# if . entry non-exo-hash for lang option = 1
## use Data::Dumper;
my %no_exo = ();
## foreach my $l (ORACC::XPD::Util::lang_options()) {
##    my @v = split(/\s+/, ORACC::XPD::Util::option($l));
##    my $nodot = 1;
##    foreach my $v (@v) {
##	if ($v eq '.') {
##	    $nodot = 0;
##	    last;
##	}
##    }
##    $l =~ s/^\%//;
##    ++$no_exo{$l} if $nodot;
##}

#warn Dumper \%no_exo;
foreach my $s (keys %exos) {
    my ($sl) = ($s =~ /\%(.*?):/);
    $sl =~ s/-\d\d\d//;
#    warn "found lang $sl in $s\n";
    if ($no_exo{$sl}) {
#	warn "skipping $s\n";
    } else {
	push @exores, "$s\t@{$exos{$s}}\n";
    }
}

my $fields = "\@fields\tsig\tinst\n";

if ($new_mode) {
    open(O,">01bld/from-$base-glo.sig"); print O $fields, sort (@glores); close(O);
    open(O,">01bld/from-$base-new.sig"); print O $fields, sort (@newres, @exores); close(O);
} else {
    open(L,">xlog"); print L @glores; print L @exores; close(L);
    open(O,">01bld/from-$base-glo.sig"); print O $fields, sort (@glores, @exores); close(O);
    open(O,">01bld/from-$base-new.sig"); print O $fields, sort @newres; close(O);
}

######################################################################

sub
attr {
    my($node,$attr) = @_;
    $node->getAttribute($attr);
}

sub
four {
    my $tmp = shift;
    $tmp =~ s/^(....).*$/$1/;
    $tmp;
}

sub
lang {
    my $rws = attr($_[0], "rws");
    if ($rws) {
	if ($rws_map{$rws}) {
	    "$lang-$rws_map{$rws}";
	} else {
	    warn "sig-form-harvester.plx: undefined langtag shorthand '$rws'\n"
		unless defined $rws_map{$rws};
	    $lang;
	}
    } else {
	$lang;
    }
}

sub showattr {
    my $n = shift @_;
    my @a = $n->attributes();
    foreach my $a (@a) {
	my $nm = $a->nodeName();
	my $vl = $a->getValue();
	warn "\t$nm=$vl\n";
    }
}

sub
loadsigs {
    my $x = load_xml($_[0]);
    $lang = $x->getDocumentElement()->getAttribute('xml:lang');
    $xtf_project = $x->getDocumentElement()->getAttribute('project');
    my @nodes = tags($x,$XCL,'l');
    if ($_[0] =~ /xsf$/) {
	push @nodes, tags($x,$XCL,'v');
    }
    foreach my $l (@nodes) {
	my $bad = $l->getAttribute('bad');
	if (!$bad || $bad ne 'yes') {
	    my $xid = xid($l);
	    my $ref = $l->getAttribute('ref');
	    my $sig = $l->getAttribute('sig');
	    my $exo = $l->getAttribute('exosig');
	    my $new = $l->getAttribute('newsig');
	    $new = '' unless $new;
#	    warn "processing node with xml:id $xid and ref $ref; new=$new\n";
#	    showattr($l);
	    $wordrefs{$xid} = $ref;
	    if ($sig && $sig =~ /^.+\[/) {
#		warn "$ref: found sig=$sig\n";
		push(@{$sigs{$sig}},"$xtf_project\:$ref");
	    } elsif ($exo) { # && $base ne 'prx') {
#		warn "$ref: found exo=$exo\n";
		my $xp = $l->getAttribute('exoprj');
		my $xl = $l->getAttribute('exolng');
		if (!$cbd_no_harvest || !cbd_no_harvest($xp,$xl)) {
		    push(@{$exos{$exo}},"$xtf_project\:$ref");
		}
	    } elsif ($new && $base ne 'prx') {
#		warn "found new=$new\n";
		push(@{$news{$new}},"$xtf_project\:$ref");
	    }
	}
    }
    foreach my $l (tags($x,$XCL,'linkset')) {
	next unless $l->getAttributeNS($XLINK,'role') eq 'psu';
	my $ref = $l->getAttribute('ref');
	my $sig = $l->getAttribute('sig');
	# warn "psu sig = $sig\n" if $sig =~ /command/;
	if (!$ref) {
	    $ref = psuref($l);
	}
	if ($ref && $sig && $sig =~ /^.+\[/) {
	    push(@{$sigs{$sig}},"$xtf_project\:$ref");
	}
    }
    undef $x;
}

sub
psuref {
    my @refs = ();
    my %seen = ();
    foreach my $c ($_[0]->childNodes()) {
	next unless $c->isa('XML::LibXML::Element');
	my $href = $c->getAttributeNS($XLINK,'href');
	if ($href) {
	    $href =~ s/^\#//;
	    my $wref = $wordrefs{$href};
	    if ($wref) {
		push(@refs,$wref) unless $seen{$wref}++;
	    } else {
		return '';
	    }
	}
    }
    join('+',@refs);
}

my %cbd_nh_proj = ();
my %cbd_nh_lang = ();

sub cbd_no_harvest_setup {
    foreach my $nh (split(/\s+/, $cbd_no_harvest)) {
	if ($nh =~ /:/) {
	    ++$cbd_nh_lang{$nh};
	} else {
	    ++$cbd_nh_proj{$nh};
	}
    }
}

sub cbd_no_harvest {
    my($xproj,$xlang) = @_;
    if ($xproj && $xlang) {
	if ($cbd_nh_proj{$xproj}) {
	    return 1;
	}
	if ($cbd_nh_lang{"$xproj:$xlang"}) {
	    return 1;
	}
    }
    0;
}

1;
