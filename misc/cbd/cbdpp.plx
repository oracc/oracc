#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::ATF;
use ORACC::CBD::PPWarn;
use ORACC::CBD::SuxNorm;
use ORACC::CBD::Validate;

use Getopt::Long;

my $bare = 0; # no need for a header
my $dry = 0; # no output files
my $filter = 0; # read from STDIN, write to CBD result to STDOUT
my $trace = 0;
my $vfields = '';

GetOptions(
    'bare'=>\$bare,
    'dry'=>\$dry,
    'filter'=>\$filter,
    'trace'=>\$trace,
    'validate:s'=>\$vfields,
    ) || die "unknown arg";

#GetOptions('f'=>\$filter); 
#print "filter=$filter\n"; exit 0;

$ORACC::CBD::PPWarn::trace = $trace;

my %validators = (
    entry=>\&v_entry,
    parts=>\&v_parts,
    bases=>\&v_bases,
    conts=>\&v_conts,
    prefs=>\&v_prefs,
    root =>\&v_root,
    form =>\&v_form,
    norms=>\&v_norms,
    sense=>\&v_sense,
    stems=>\&v_stems,
    bib=>\&v_bib,
    isslp=>\&v_isslp,
    equiv=>\&v_equiv,
    note=>\&v_note,
    inote=>\&v_inote,
    end=>\&v_end,
    project=>\&v_project,
    lang=>\&v_lang,
    name=>\&v_name,
    was=>\&v_deprecated,
    moved=>\&v_deprecated,
    bff=>\&v_bff,
    collo=>\&v_collo,
    geo=>\&v_geo,
    usage=>\&v_usage,
    );

my %rws_map = (
    EG => 'sux',
    ES => 'sux-x-emesal',
    CF => 'akk',
    CA => 'akk-x-conakk',
    OA => 'akk-x-oldass',
    OB => 'akk-x-oldbab',
    MA => 'akk-x-midass',
    MB => 'akk-x-midbab',
    NA => 'akk-x-neoass',
    NB => 'akk-x-neobab',
    SB => 'akk-x-stdbab',
    );

my $acd_chars = '->+=';
my $acd_rx = '['.$acd_chars.']';

my @funcs = qw/free impf perf Pl PlObj PlSubj Sg SgObj SgSubj/;
my %funcs = (); @funcs{@funcs} = ();

my @tags = qw/entry parts bff bases stems phon root form length norms
              sense equiv inote prop end isslp bib defn note pl_coord
              pl_id pl_uid was moved project lang name collo/;

my %tags = (); @tags{@tags} = ();

my @data = qw/usage collo sense/;

my %data = (); @data{@data} = ();

my %ppfunc = (
    usage=>\&pp_usage,
    collo=>\&pp_collo,
    sense=>\&pp_geo,
);

my $lng = '';
my $cbd = '';

unless ($filter) {
    $cbd = shift @ARGV;
    if ($cbd) {
	$lng = $cbd; $lng =~ s/\.glo$//; $lng =~ s#.*?/([^/]+)$#$1#;
    } else {
	die "cbdpp.plx: must give glossary on command line\n";
    }
} else {
    $cbd = '<stdin>';
}
pp_file($cbd);

my @acd = ();
my @cbd = ();
my @ngm = ();

my %bases = ();
my $bid = 0;
my $cbdlang = '';
my $in_entry = 0;
my $init_acd = 0;
my $is_compound = 0;
my $mixed_morph = 0;
my $project = '';
my $projdir = '';
my $seen_bases = 0;
my %seen_forms = ();
my $seen_morph2 = 0;
my $status = 0;
my %tag_lists = ();

my %vfields = ();
my %arg_vfields = (); 
if ($vfields) {
    @arg_vfields{split(/,/,$vfields)} = ();
    %vfields = %arg_vfields;
    @vfields{qw/lang entry end/} = ();
} else {
    %vfields = %tags;
    if ($trace) {
	%arg_vfields = %tags;
    }
}

###############################################################
#
# Program Body
#
###############################################################

pp_load();

# Allow files of bare glossary bits for testing
if ($bare) {
    $cbdlang = 'sux' unless $cbdlang;
    $project = 'test' unless $project;
}

die "cbdpp.plx: $cbd: can't continue without project and language\n"
    unless $project && $cbdlang;

if ($cbdlang =~ /sux|qpn/) {
    @cbd = ORACC::CBD::SuxNorm::normify($cbd, @cbd);
}

$projdir = "$ENV{'ORACC_BUILDS'}/$project";
pp_validate();

if ($status) {
    die("cbdpp.plx: errors in glossary $cbd. Stop.\n");
} else {    
    foreach my $f (keys %ppfunc) {
	if ($#{$data{$f}} >= 0) {
	    &{$ppfunc{$f}}();
	}
    }
}

atf_check($project,$cbdlang);

pp_cbd();

pp_diagnostics();

#######################################################################

sub pp_validate {
    for (my $i = 0; $i <= $#cbd; ++$i) {
	next if $cbd[$i] =~ /^\000$/ || $cbd[$i] =~ /^\#/;
	pp_line($i+1);
	if ($cbd[$i] =~ /^\s*$/) {
	    pp_warn("blank lines not allowed in \@entry")
		if $in_entry;
	} elsif ($cbd[$i] =~ /^\@([A-Z]+)\s*$/) {
	    my $rws = $1;
	    pp_warn("\@$1 unknown register/writing-system/dialect")
		unless $rws_map{$rws};
	    #	} elsif ($cbd[$i] =~ /^$acd_rx?@([a-z]+)\s+(.*)\s*$/o
	} elsif ($cbd[$i] =~ /@([a-z]+)/) {
	    my($tag,$line) = ($1,$2);
	    if (exists $tags{$tag}) {
		push @{$tag_lists{$tag}}, $i;
		if ($validators{$tag}) {
		    if (exists $vfields{$tag}) {
			if ($cbd[$i] =~ m/^(\S+)\s+(.*?)\s*$/) {
			    my($t,$l) = ($1,$2);
			    &{$validators{$tag}}($t,$l);
			} else {
			    &{$validators{$tag}}($cbd[$i],'');
			}
		    }
		} else {
		    pp_warn("internal error: no validator function defined for tag `$tag'");
		}
	    } else {
		pp_warn("\@$1 unknown tag");
	    }
	} else {
	    pp_warn("invalid line in glossary");
	}
    }
}

################################################
#
# Utility routines
#
################################################

sub pp_entry_of {
    my $i = shift;
    while ($cbd[$i] !~ /\@entry/) {
	--$i;
    }
    $i;
}

sub pp_load {
    if ($filter) {
	@cbd = (<>); chomp @cbd;
    } else {
	open(C,$cbd) || die "cbdpp.plx: unable to open $cbd. Stop.\n";
	@cbd = (<C>); chomp @cbd;
	close(C);
    }

    my $insert = -1;
    for (my $i = 0; $i <= $#cbd; ++$i) {
	if ($cbd[$i] =~ /^$acd_rx?\@([a-z]+)/) {
	    my $tag = $1;
	    if ($tag ne 'end') {
		if ($tag eq 'project') {
		    pp_line($i+1);
		    v_project($cbd[$i]);
		} elsif ($tag eq 'lang') {
		    pp_line($i+1);
		    v_lang($cbd[$i]);
		}
		$insert = $i;
		push(@{$data{$tag}}, $i) if exists $data{$tag};
	    } else {
		$insert = -1;
	    }
	} elsif ($cbd[$i] =~ s/^\s+(\S)/ $1/) {
	    if ($insert >= 0) {
		$cbd[$insert] .= $cbd[$i];
		$cbd[$i] = "\000";
	    } else {
		pp_warn("indented lines only allowed within \@entry");
	    }
	}
    }
}

################################################
#
# CBDPP Operational Functions
#
################################################

sub pp_acd {
    open(ACD, '>pp.acd');
    foreach my $i (@{$data{'acd'}}) {
	print ACD $cbd[$i], "\n";
	$cbd[$i] = "\000";
    }
    close(ACD);
}

sub pp_cbd {
    return if pp_status();
    if ($filter) {
	foreach (@cbd) {
	    print "$_\n" unless /^\000$/;
	}
    } else {
	my $ldir = "$projdir/01tmp";
	system 'mkdir', '-p', $ldir;
	open(CBD, ">$ldir/$cbdlang.glo") 
	    || die "cbdpp.plx: can't write to $ldir/$cbdlang.glo";
	foreach (@cbd) {
	    print CBD "$_\n" unless /^\000$/;
	}
	close(CBD);
    }
}
sub pp_collo {
    my $ndir = "$projdir/02pub";
    system 'mkdir', '-p', $ndir;
    open(COLLO, ">$ndir/coll-$cbdlang.ngm");
    foreach my $i (@{$data{'collo'}}) {
	my $e = pp_entry_of($i);
	my $c = $cbd[$e];
	$c =~ s/^\S*//;
	$c =~ s/\].*$/\]/;
	$c =~ s/\s+\[/\[/;
	my $cc = $cbd[$i];
	$cc =~ s/\s+-(\s+|$)/ $c /;
	$cc =~ s/\s+$//;
	$cc =~ s/^\S+\s+//;
	print COLLO $cc, "\n";
	$cbd[$i] = "\000";
    }
    close(COLLO);
}
sub pp_geo {
    open(GEOS, '>pp.geos') || die "cbdpp.plx: can't write to pp.glo";
    foreach my $i (@{$data{'geos'}}) {
	print GEOS $cbd[$i], "\n";
	$cbd[$i] = "\000";
    }
    close(GEOS);
}
sub pp_usage {
    open(USAGE,'>pp.usage');
    foreach my $i (@{$data{'collo'}}) {
	print USAGE $cbd[$i], "\n";
	$cbd[$i] = "\000";
    }
    close(USAGE);
}

1;
