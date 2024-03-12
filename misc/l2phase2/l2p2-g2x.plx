#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use Data::Dumper;

use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::OID;
use ORACC::L2GLO::Builtins;
use ORACC::L2GLO::Util;
use ORACC::SL::Tlitsig;
use Getopt::Long;

use constant {
    IDENTITY => 0,
    LITERAL  => 1,
    WILDCARD => 2,
    SENSEPOS => 3,
    CFGWPOS  => 4,
};

my $baselang = '';
my %compound_parts = ();
my $curr_periods_int = 0;
my %data = ();
my $done = 0;
my %entry_ids = ();
my %entry_langs = ();
my %entry_lines = ();
my %entry_xis = ();
my %entryformstems = ();
my %entrynormstems = ();
my %f = ();
my %forms = ();
my %form_ids = ();
my %header = ();
my $header_vars = 0;
my $have_disamb = 0;
my $id_base = '';
my $id_counter = -1; # next_xid pre-increments
my $lang = '';
my %letter_ids = ();
my $name = '';
my $nox = 0; # if 1 don't make .xis, .lst, .map
my %norms = ();
my $outfile = '';
my %pre_ids = ();
my $proj = '';
my $seen_morph2 = 0;
my $verbose = 0;
my $with_zero_freqs = 0;
my %xis_periods = ();

GetOptions(
    'all+'=>\$with_zero_freqs,
    'header'=>\$header_vars,
    'lang:s'=>\$lang,
    'name:s'=>\$name,
    'nox'=>\$nox,
    'out:s'=>\$outfile,
    'proj:s'=>\$proj,
    'verb'=>\$verbose,
    );

die "l2p2-g2x.plx: must give -lang LANG/-name NAME/-project PROJ on command line\n"
    unless $header_vars || ($lang && $name && $proj);

$header{'lang'} = $lang;
$header{'name'} = $name;
$header{'project'} = $proj;
my $input = shift @ARGV;

die "l2p2-g2x.plx: must give name of .sig file on command line\n"
    unless $input;

my $cbd_oid = 0;
my %form_cofs = ();
my %m1m2 = ();
my %norm_form_cofs = ();
my %norm_form_cof_insts = ();
my %norm_form_cof_freqs = ();
my %norm_form_freqs = ();
my %norm_form_insts = ();
my %parts = ();
my $parts_loaded = 0;
my $psu_debug = 0;
my $sid = 0;
my @sigfields = qw/form norm base cont morph morph2 stem prefix/;
my %sigmap = ();
my %sigmap_ids = ();
my %sigrefs = ();
my %sig_rewrites = ();
my %s_field_freqs = ();
my %s_field_insts = ();
my %entry_freqs = ();
my %entry_sigrefs = ();
my @ns = (
    'xmlns="http://oracc.org/ns/cbd/1.0"',
    'xmlns:cbd="http://oracc.org/ns/cbd/1.0"',
    'xmlns:g="http://oracc.org/ns/gdl/1.0"',
    'xmlns:n="http://oracc.org/ns/norm/1.0"',
    );
my $ns = join(' ', @ns);
my %cofs = ();
my %cof_index = ();
my $curr_cof_form = '';
my $project = ($proj ? $proj : `oraccopt`);

my $cbd_oid_opt = `oraccopt $project cbd-oid`;
if ($cbd_oid_opt eq 'yes' || $lang =~ /^sux/) {
    $cbd_oid = 1;
}

my %scriptcode = (
    arc=>'950',
    );
my %seen = ();
my %sense_freqs = ();
my %sense_sigrefs = ();
my %sig_lines = ();
my $xrefid = '';
my $xrefid_counter = 0;
my %xrefs = ();
my %xrefs_infos = ();

my %gdlme = (); my @gdlme = qw/base form/; push @gdlme, 'form-sans'; @gdlme{@gdlme} = ();

ORACC::SL::Tlitsig::init();

$ORACC::L2GLO::Util::project = $project;

my $dirname = `dirname $input`;
chomp $dirname;
if ($dirname)  {
    $dirname .= '/' unless $dirname =~ m#/$#;
}

my $rewrites = $input;
$rewrites =~ s/\..*$/.rew/;
load_rewrites($rewrites) if -r $rewrites;

read_input($input);

sort_forms();
sort_norms();

#use Data::Dumper;
#warn Dumper(\%s_field_freqs), "\n";

# open(D,">$header{'lang'}-entry.log"); print D Dumper(\%entry_ids); close(D);

if ($have_disamb) {
    push @sigfields, 'form-sans';
}

system 'mkdir', '-p', "02pub/cbd/$header{'lang'}";
my $tisout = "02pub/cbd/$header{'lang'}/$header{'lang'}.tis";
open(XISTAB,">$tisout") || die "$0: can't open $tisout for write\n";
my $xisout = "$dirname$header{'lang'}.xis";
open(XIS,">$xisout") || die "$0: can't open $xisout for write\n";
print XIS '<?xml version="1.0" encoding="utf-8"?>', "\n";
print XIS '<xisses xmlns="http://oracc.org/ns/xis/1.0" xmlns:xis="http://oracc.org/ns/xis/1.0">';

#open(G, "|gdlme2 -s>$dirname$header{'name'}.g2x") 
if ($outfile) {
    open(G, ">$outfile")
	|| die "l2p2-g2x.plx: can't open $outfile for output\n";
} else {
    open(G, ">$dirname$header{'lang'}.g2x")
	|| die "l2p2-g2x.plx: can't open $dirname$header{'name'}.g2x for output\n";
}

my $versionattr = '';
my $dateattr = '';
if (-r 'VERSION') {
    $versionattr = ' version="'.`cat VERSION`.'"';
    $dateattr = ' date="'.`cat DATE`.'"';
}

select G;
print '<?xml version="1.0" encoding="utf-8"?>', "\n";
print "<entries $ns project=\"$header{'project'}\" xml:lang=\"$header{'lang'}\" n=\"$header{'name'}\"$versionattr$dateattr>";

foreach my $lang (sort keys %data) {
    $baselang = $lang; $baselang =~ s/-.*$//;
    foreach my $entry (sort keys %{$data{$lang}}) {
	foreach my $sense (keys %{${$data{$lang}}{$entry}}) {
	    my $sense_freq = 0;
	    foreach my $sig (@{${${$data{$lang}}{$entry}}{$sense}}) {
		$sense_freqs{$sense} += $$sig[2];
		push @{$sense_sigrefs{$sense}}, $$sig[0];
	    }
	    $entry_freqs{$entry} += $sense_freqs{$sense}
	    	if $sense_freqs{$sense};
	    push @{$entry_sigrefs{$entry}}, @{$sense_sigrefs{$sense}};
	}
	my $entry_xid = $entry_ids{$entry};
	my $oid = '';
	my $siglang = $baselang;
	if ('qpn' eq $baselang) {
	    $siglang = $entry_langs{$entry};
	}
	warn "oid_lookup($siglang,$entry)\n";
	$oid = oid_lookup($siglang, $entry) if $cbd_oid; # $baselang =~ /^sux/
	my $xid = ($oid ? $oid : $entry_xid);
	my ($letter) = ($entry =~ /^(.)/);
#	push @{$letter_ids{&ORACC::L2GLO::Builtins::first_letter($letter)}}, $entry_xid;
	my %xis_info = xis($lang,$entry_xid,$xid,$entry_freqs{$entry},'100',@{$entry_sigrefs{$entry}});
	
	$entry_xis{$entry_xid} = { %xis_info };

	my $xentry = xmlify($entry);
	my $p_icount = $entry_freqs{$entry} || -1;
	print "<entry xml:id=\"$xid\" n=\"$xentry\"";
	print " oid=\"$oid\"" if $oid;
	xis_attr(%xis_info);
	print '>';
	my $entry_periods = $xis_info{'periods'};
	my($cf,$gw,$pos) = ($entry =~ m/^(.*?)\[(.*?)\](.*?)$/);
	$cf = xmlify($cf);
	$gw = xmlify($gw);
	print "<cf>$cf</cf><gw xml:lang=\"en\">$gw</gw><pos>$pos</pos>";

	if ($compound_parts{$entry}) {
	    print '<compound>';
	    my $primary = 1;
	    my @parts = @{$compound_parts{$entry}};
	    my @sparts = @{$compound_parts{$entry,'senses'}};
	    for (my $p_i = 0; $p_i <= $#parts; ++$p_i) {
		my $p = $parts[$p_i];
		my $eid = '';
		if ($cbd_oid) {
		    $eid = oid_lookup($baselang,$p);
		}
		$eid = $entry_ids{$p} unless $eid;
		unless ($eid) {
		    # This block is where we set up refs for cross-glossary PSUs
		    load_parts() unless $parts_loaded;
		    if ($parts{$p}) {
			my @p = keys %{$parts{$p}};
			my $p0 = $p[0];
			$p0 =~ s/-.*$//; # remove sub-lang tag, e.g., from sux-x-emesal to sux -- questionable?
			$p0 = 'qpn' if $p =~ /\][A-Z]N$/; # coerce NN to 'qpn' glossary
			my $xp = xmlify($p);
			$eid = "\#$p0\:$xp";
			$eid =~ s/\s+:/:/;
#			warn "eid=$eid\n";
		    } else {
			$eid = '';
			$p =~ s/\[/ [/; $p =~ s/\]/] /;
			warn "00lib/$lang.glo: PSU $entry needs \@entry $p\n";
		    }
		}
		my($pcf,$pgw,$psense,$ppos,$pepos) = ($sparts[$p_i] =~ /^(.*?)\[(.*?)\/\/(.*?)\](.*?)'(.*?)$/);
		$pcf = xmlify($pcf);
		$pgw = xmlify($pgw);
		$psense = xmlify($psense);
		print '<cpd ';
		print ' primary="1"' if $primary-- > 0;
		print " partsig=\"$pcf\[$pgw\]$ppos\"";
		print " ref=\"$eid\">";
		print "<cf>$pcf</cf><gw>$pgw</gw><mng>$psense</mng><pos>$ppos</pos><epos>$pepos</epos></cpd>";
	    }
	    print '</compound>';
	}

	compute_and_print_entry_data($lang, $entry, %{${$data{$lang}}{$entry}});

	print '<senses>';
	foreach my $sense (sort keys %{${$data{$lang}}{$entry}}) {
	    my $pct = ipct($sense_freqs{$sense},$entry_freqs{$entry});
	    $xid = next_xid();
	    my %xis_info = xis($lang,$entry_xid,$xid,$sense_freqs{$sense},$pct,@{$sense_sigrefs{$sense}});
	    my $p_icount = $sense_freqs{$sense} || -1;
	    my $sense_no_norm = $sense;
	    $sense_no_norm =~ s/\$.*$//;
	    my $oid = '';
	    $oid = oid_lookup($baselang,$sense_no_norm) if $cbd_oid;
	    my $xsense = xmlify($sense_no_norm);
	    print "<sense xml:id=\"$xid\" n=\"$xsense\"";
	    print " oid=\"$oid\"" if $oid;
	    xis_attr(%xis_info);
	    print '>';
	    my($mng,$pos) = ($sense_no_norm =~ m#//(.*?)\].*?'(.*?)$#);
	    $mng = xmlify($mng);
	    warn "$input:$entry_lines{$entry}: no mng in $entry :: $sense_no_norm\n" unless defined $mng;
	    warn "$input:$entry_lines{$entry}: no epos in $entry :: $sense_no_norm\n" unless $pos;
	    print "<pos>$pos</pos><mng xml:lang=\"en\">$mng</mng>";
	    foreach my $f (@sigfields) {
		if (${$s_field_freqs{$sense}}{$f}) {
		    print "<${f}s>";
		    foreach my $k (sort keys %{${$s_field_freqs{$sense}}{$f}}) {
			my $pct = ipct(${${$s_field_freqs{$sense}}{$f}}{$k}, $sense_freqs{$sense});
			$xid = next_xid();
			%xis_info = xis($lang,$entry_xid,$xid,${${$s_field_freqs{$sense}}{$f}}{$k},$pct,
					@{${${$s_field_insts{$sense}}{$f}}{$k}});
			my $kn = $k;
			$kn =~ s/^\%.*?://;
			$kn = xmlify($kn);
			my $m2attr = '';
			if ($f eq 'morph') {
			    if ($m1m2{$entry_ids{$entry},$k}) {
				$m2attr = ' morph2="'.join(' ',@{$m1m2{$entry_ids{$entry},$k}}).'"';
			    }
			}
			if ($f eq 'form') {
			    print "<$f cbd:id=\"$xid\"$m2attr n=\"$kn\"";
			} else {
			    print "<$f xml:id=\"$xid\"$m2attr n=\"$kn\"";
			}
			if ($f eq 'form') {
			    if ($form_ids{$k}) {
				print " ref=\"$form_ids{$k}\"";
			    }
			}
			xis_attr(%xis_info);
			print '/>';
		    }
		    print "</${f}s>";
		}
	    }
	    print '<sigs>';
	    foreach my $sig (@{${${$data{$lang}}{$entry}}{$sense}}) {
		my $ipct = ipct($$sig[2],$sense_freqs{$sense});
		$xid = next_xid();
		my %xis_info = xis($lang,$entry_xid,$xid,$$sig[2],$ipct, $$sig[0]);
		my $psig = $cof_index{$$sig[0]} || $$sig[1];
		my $xsig = xmlify($psig);
		my $p_icount = $$sig[2] || -1;
		my $print_ngram = '';
		if ($$sig[3]) {
		    $print_ngram = sprintf(" ngram=\"%s\"", xmlify($$sig[3]));
		}
		print "<sig xml:id=\"$xid\" sig=\"$xsig\"";
		xis_attr(%xis_info);
		if ($cof_index{$$sig[0]}) {
		    my @cof = @{$cofs{$cof_index{$$sig[0]}}};
		    my $index = $cof_index{$$sig[0],'#'};
		    print '><cof-data>';
		    for (my $j = 0; $j <= $#cof; ++$j) {
			my $tag = ($j ? 'cof-tail' : 'cof-head');
			my $cur = (($j==$index) ? " curr=\"yes\"" : '');
			print "<$tag$cur>$cof[$j]</$tag>";
		    }
		    print '</cof-data></sig>';
		} else {
		    print "/>";
		}
		$sigmap_ids{$$sig[1]} = $entry_xid;
	    }
	    print '</sigs>';
	    print '</sense>';
	}
	print '</senses>';
#	print "<overview periods=\"$entry_periods\"/>";
	print '</entry>';
    }
}

xis_dump_periods() unless $nox;
print '</entries>';

xis_rr() unless $nox;
print XIS '</xisses>'; close XIS; close XISTAB;
close(G);

unless ($nox) {
    open(M,">$dirname$header{'lang'}.map") 
	|| die "l2p2-g2x.plx: can't open map file `$dirname$header{'lang'}.map'\n";
    foreach my $m (keys %sigmap) {
	if (defined $sigmap{$m}) {
	    if ($sigmap{$m}) {
		print M "$m\t$sigmap_ids{$sigmap{$m}}\n";
	    } elsif ($sigmap_ids{$m}) {
		print M "$m\t$sigmap_ids{$m}\n";
	    } else {
		warn "no sigmap for $m\n";
	    }
	}
    }
    close(M);
}

# unless ($nox) {
#     my $lid = 'L000';
#     my @l = ();
#     foreach my $l (sort keys %letter_ids) {
# 	open(L,">$dirname$lid.lst");
# 	foreach my $id (@{$letter_ids{$l}}) {
# 	    print L "$id\n";
# 	}
# 	close(L);
# 	push @l, [ $l, $lid++ ];
#     }
#     open(L,">${dirname}letter_ids.tab");
#     foreach my $l (@l) {
# 	print L "$$l[0]\t$$l[1]\t";
#     }
#     close(L);
# }

###################################################################

sub
add_sig {
    my ($sig, $freq, $refs, $full_cof_sig, $cof_index) = @_;

    my $psu_form = '';
    my $psu_ngram = '';
    if ($sig =~ s/^{(.*?)\s+\+=\s+/{/) {
	my $psutmp = $1;
	if ($psutmp =~ /^(.*?) = (.*?)$/) {
	    $psu_form = $1;
	    $psu_ngram = $2;
	} else {
	    $psu_ngram = $psutmp;
	}
#	warn "psu_ngram = $psu_ngram\n";
    }

    # We do sig rewriting very early in the process so the data from
    # the incoming sig cannot contaminate the processed sig
    $sig = rewrite_sig($sig);

#    if ($baselang =~ /^qpn/) {
#	$esig =~ s/%(.*?):/%qpn:/;
#    }

    my $cof_norm = '';
    my %sig = ();
    if ($sig =~ /^\{/) {
	%sig = (ORACC::L2GLO::Util::parse_psu($sig));
    } else {
	%sig = (ORACC::L2GLO::Util::parse_sig($sig));
	if ($sig{'morph'} && $header{'lang'} =~ /^sux/) {
	    my $p = $sig{'morph'};
	    if ($p =~ /:/) {
		$p =~ s/:.*$//;
		$sig{'prefix'} = $p;
	    }
	}
	if ($full_cof_sig) {
	    if (!$cof_index) {
		$curr_cof_form = $sig{'form'};
	    } else {
		$sig{'form'} = $curr_cof_form;
	    }
	    $cof_norm = make_cof_norm($full_cof_sig);
	}
#	print Dumper \%sig;
    }
    if ($psu_ngram) {
	$sig{'psu_ngram'} = $psu_ngram;
	$sig{'form'} = $psu_form;  ## put this back because it gets removed before calling parse_psu
    }
    
    $sig{'sid'} = sprintf("sig%06x",$sid++);

    if ($sig{'stem'}) {
	#	    warn "found stem\n";
	++${$entryformstems{"$sig{'cf'}\[$sig{'gw'}\]$sig{'pos'}",$sig{'form'}}{$sig{'stem'}}};
	$entrynormstems{"$sig{'cf'}\[$sig{'gw'}\]$sig{'pos'}",$sig{'norm'}} = $sig{'stem'};
	#	    print Dumper \%entryformstems;
    }
    
    ++$seen_morph2 if $sig{'morph2'};

    ++$forms{$sig{'form'}} if $sig{'form'};
    ++$norms{$sig{'norm'}} if $sig{'norm'};

    my $datalang = '';
    if ($baselang =~ /^qpn/) {
	$datalang = 'qpn';
    } else {
	$datalang = $sig{'lang'}; 
	$datalang =~ s/-.*$//;
    }
    if (!$sig{'cf'}) {
	warn "$input:$.: no CF in sig $sig\n";
    }
    $sig{'cf'} = 'X' unless $sig{'cf'};
    $sig{'gw'} = 'X' unless defined $sig{'gw'};
    $sig{'sense'} = 'X' unless defined $sig{'sense'};
    $sig{'pos'} = 'X' unless $sig{'pos'};
    $sig{'epos'} = 'X' unless $sig{'epos'};
    my $entry = "$sig{'cf'}\[$sig{'gw'}\]$sig{'pos'}";
    unless ($entry_ids{$entry}) {
	$entry_ids{$entry} = next_xid();
	$entry_lines{$entry} = $.;
	$entry_langs{$entry} = $sig{'lang'};
    }
    if ($cof_norm) {
#	warn "registering form_cof value $cof_norm for key $entry_ids{$entry}\:$sig{'form'}\n";
	++${$form_cofs{"$entry_ids{$entry}\:$sig{'form'}"}}{$cof_norm};
    }
    my $sense = "$sig{'cf'}\[$sig{'gw'}//$sig{'sense'}\]$sig{'pos'}'$sig{'epos'}";
    $sigrefs{$sig{'sid'}} = $refs;
    $sig_lines{$sig{'sid'}} = $.;
    if ($sig{'parts'}) {
	@{$compound_parts{$entry}} = @{$sig{'parts'}};
	@{$compound_parts{$entry,'senses'}} = @{$sig{'sense-parts'}};
    }
    # ADD RANK
    push @{${${$data{$datalang}}{$entry}}{$sense}} , [ $sig{'sid'} , $sig , $freq , $sig{'psu_ngram'} ];
    if ($sig{'norm'} && $sig{'form'}) {
	    ${$norm_form_freqs{"$entry\$$sig{'norm'}"}}{$sig{'form'}} += $freq;
	    push (@{$norm_form_insts{"$entry\$$sig{'norm'}\:$sig{'form'}"}}, $sig{'sid'});
	    if ($full_cof_sig) {
		push (@{$norm_form_cofs{"$entry\$$sig{'norm'}\:$sig{'form'}"}}, $cof_norm);
		${$norm_form_cof_freqs{"$entry\$$sig{'norm'}"}}{"$sig{'form'}\:$cof_norm"} += $freq;
		push (@{$norm_form_cof_insts{"$entry\$$sig{'norm'}\:$sig{'form'}\:$cof_norm"}}, $sig{'sid'});
	    }
    }

    if ($sig{'morph'}) {
	if ($sig{'morph2'}) {
	    my $tmp_id = $pre_ids{$entry_ids{$entry},'morph2',$sig{'morph2'}};
	    unless ($tmp_id) {
		$tmp_id = $pre_ids{$entry_ids{$entry},'morph2',$sig{'morph2'}} = next_xid();
	    }
	    push @{$m1m2{$entry_ids{$entry},$sig{'morph'}}}, $tmp_id;
	} elsif ($seen_morph2) {
	    warn "$input:$.: morph has no morph2\n";
	}
    }

    foreach my $f (@sigfields) {
	if ($sig{$f}) {
	    my $key = $sig{$f};
	    ${${$s_field_freqs{$sense}}{$f}}{$key} += $freq;
	    push (@{${${$s_field_insts{$sense}}{$f}}{$key}}, $sig{'sid'});

	    if ($f eq 'form') {
		# it is unnecessary to have forms_sans_disamb for most projects so this
		# should be tested more
		my $f2 = 'form-sans';
		my $k2 = $key;
#		warn "sig{lang}=$sig{'lang'}\n";
#		if ($sig{'lang'} =~ /-949/) {
#		    $k2 = "$sig{'lang'}:$key";
#		}
		if ($k2 =~ s/\\\S+//g) {
		    ++$have_disamb;
		    $k2 =~ s/\s+/ /g;
		}
		${${$s_field_freqs{$sense}}{$f2}}{$k2} += $freq;
		push (@{${${$s_field_insts{$sense}}{$f2}}{$k2}}, $sig{'sid'});
	    }
	}
    }

    if ($full_cof_sig) {
	my $key = "$sig{'form'}\:$cof_norm";
	${${$s_field_freqs{$sense}}{'cof_norm'}}{$key} += $freq;
	push (@{${${$s_field_insts{$sense}}{'cof_norm'}}{$key}}, $sig{'sid'});
	$cof_index{$sig{'sid'}} = $full_cof_sig;
	$cof_index{$sig{'sid'},'#'} = $cof_index;
	my $cret = $sense;
	$sense .= "\$sig{'norm'}" if $sig{'norm'};
	return $cret;
    }
}

sub
compute_and_print_entry_data {
    my $lang = shift;
    my $entry = shift;
    my %entry = @_;
    my %freqs = ();
    my %insts = ();
    my $entry_xid = $entry_ids{$entry};

    %form_ids = ();

    foreach my $sense (keys %entry) {
	foreach my $f (@sigfields) {
	    if (${$s_field_freqs{$sense}}{$f}) {
		foreach my $k (keys %{${$s_field_freqs{$sense}}{$f}}) {
		    ${$freqs{$f}}{$k} += ${${$s_field_freqs{$sense}}{$f}}{$k};
		    @{${$insts{$f}}{$k}}{@{${${$s_field_insts{$sense}}{$f}}{$k}}} = ();
		    if ($f eq 'form') {
			foreach my $fc (sort keys %{$form_cofs{"$entry_xid\:$k"}}) {
			    my $fkc .= "$k\:$fc";
			    if (${${$s_field_freqs{$sense}}{'cof_norm'}}{$fkc}) {
				${$freqs{'cof_norm'}}{$fkc} += ${${$s_field_freqs{$sense}}{'cof_norm'}}{$fkc};
#				warn "storing data with fkc = $fkc; freq=${${$s_field_freqs{$sense}}{'cof_norm'}}{$fkc} => ${$freqs{'cof_norm'}}{$fkc}\n";
				@{${$insts{'cof_norm'}}{$fkc}}{@{${${$s_field_insts{$sense}}{'cof_norm'}}{$fkc}}} = ();
			    }
			}
		    }
		}
	    }
	}	
    }

    # use Data::Dumper; warn Dumper(\%freqs), "\n";

    foreach my $f (@sigfields) {
	if ($freqs{$f} || $with_zero_freqs) {
	    print "<${f}s>";
	    if ($f eq 'base') {
		rewrite_bases($entry_lines{$entry}, \%freqs, \%insts);
#		%freqs = { %$nfreqs };
#		%insts = { %$ninsts };
	    }
	    my @k = ();
	    if ($f eq 'norm') {
		@k = sort { $norms{$a} <=> $norms{$b} } keys %{$freqs{$f}};
	    } elsif ($f eq 'form') {
		@k = sort { $forms{$a} <=> $forms{$b} } keys %{$freqs{$f}};
	    } else {
		@k = sort keys %{$freqs{$f}};
	    }
	    foreach my $k (@k) {
		my @sigs = sort keys %{${$insts{$f}}{$k}};
		my $icount = ${$freqs{$f}}{$k};
		my $ipct = ipct($icount, $entry_freqs{$entry});
		my $xid = $pre_ids{$entry_ids{$entry},$f,$k} || next_xid();
		my %xis_info = xis($lang,$entry_xid,$xid,$icount,$ipct,@sigs);
		my $sort_k = $k;

		if (exists $gdlme{$f}) {
		    my $gme = ' g:me="1"';
		    ### GRAPHETIC MARKERS ARE NOT ALLOWED INTO FORM SO WE SHOULD NOT NEED
		    ### TO DELETE THEM HERE.
		    ###
		    $k =~ tr/°·//d;
		    $k =~ /^\%(.*?):/;
		    my $formlang = $1 || '';
		    my $xk = $k;
		    $xk =~ s/^(\%\S+?):/$1 /;
		    $xk = xmlify($xk);
		    my $xk_n = $xk;
		    $xk_n =~ s/^\%.*?\s+//g;
		    $xk =~ s/(\s\%[-a-z0-9]+):/$1 /g;
		    $xk =~ s/ \+/ /;

		    # FIXME: check the script code if we start using it any way
		    # except internally
		    #		    if ($scriptcode{$formlang}) {
		    #			$gme = '';
		    #		    }
		    if ($f eq 'form') {
			print "<$f xml:id=\"$xid\" n=\"$xk_n\" c=\"$forms{$sort_k}\"";
		    } else {
			print "<$f cbd:id=\"$xid\" n=\"$xk_n\"";
		    }
		    if ($entryformstems{$entry,$k}) {
			my $efs = entry_form_string($entry,$k);
			print " stem=\"$efs\"";
		    }
		    xis_attr(%xis_info);
		    print '>';
#		    icount=\"$icount\" ipct=\"$ipct\" xis=\"$xrefid\">";
#		    $xrefid_counter; #  sigs=\"@sigs\";
		    print "<t$gme g:file=\"$input\" g:line=\"$sig_lines{$sigs[0]}\">$xk</t>";

		    if ($f eq 'form') {
			$form_ids{$k} = $xid;
#			warn "processing form $k in entry $entry_ids{$entry}\n";
			if ($form_cofs{"$entry_ids{$entry}\:$k"}) {
#			    warn "found form_cofs for $entry_ids{$entry}\:$k\n";
			    foreach my $fc (sort keys %{$form_cofs{"$entry_ids{$entry}\:$k"}}) {
				my $fkc = "$k\:$fc";
				my @xsigs = sort keys %{${$insts{'cof_norm'}}{$fkc}};
				my $xcount = ${$freqs{'cof_norm'}}{$fkc};
				my $xpct = ipct($xcount, $entry_freqs{$entry});
#				warn "retrieving data with fkc = $fkc; xcount=$xcount\n";
				my $n = $fc;
				$n =~ s/^\%.*?://; $n =~ s/^.*?://;
				$n = xmlify($n);
				my $xid = next_xid();

				my %xis_info = xis($lang,$entry_xid,$xid,$xcount,$xpct,@xsigs);
				$form_ids{"$k\:$fc"} = $xid;
				print "<cof-form-norm xml:id=\"$xid\" n=\"$n\"";
				xis_attr(%xis_info);
				print '/>';
				#icount=\"$xcount\" ipct=\"$xpct\" xis=\"$xrefid\"/>";
			    }
			}
		    }

		    print "</$f>";
		} else {
		    if ($f eq 'norm') {
			my $nk = xmlify($k);
			$nk =~ tr/ /_/;
			printf "<norm xml:id=\"$xid\" n=\"$nk\" icount=\"$icount\" ipct=\"$ipct\" xis=\"$xrefid\"";
			if ($entrynormstems{$entry,$k}) {
			    printf " stem=\"$entrynormstems{$entry,$k}\"";
			}
			    printf(">");
			if ($k =~ /\s/) {
			    print '<n>';
			    foreach my $kk (split(/\s+/,$k)) {
				my $xkk = xmlify($kk);
				print "<n>$xkk</n>";
			    }
			    print '</n>';
			} else {
			    my $xk = xmlify($k);
			    print "<n>$xk</n>";
			}
			my $normform = "$entry\$$k";
			print '<forms>';
			foreach my $f (sort keys %{$norm_form_freqs{$normform}}) {
			    my $freq = ${$norm_form_freqs{$normform}}{$f};
			    $xid = next_xid();
			    my $ipct = ipct($freq,$entry_freqs{$entry});
			    my %xis_info = xis($lang,$entry_xid,$xid,$freq,$ipct,@{$norm_form_insts{"$normform\:$f"}});
#			    if ($f =~ /\s/) {
#				print "<f xml:id=\"$xid\" icount=\"$freq\" ipct=\"$ipct\" xis=\"$xrefid\">";
#				foreach my $ff (split(/\s+/,$f)) {
#				    print "<f ref=\"$form_ids{$ff}\"/>";
#				} 
#
#				print '</f>';
#			    } else {
			    if ($form_ids{$f} && defined($freq) && defined($ipct) && $xrefid) {
				my $fx = xmlify($f);
				$fx =~ s/^\%.*?://;
				print "<f n=\"$fx\" cbd:id=\"$xid\" ref=\"$form_ids{$f}\"";
				xis_attr(%xis_info);
				print '/>';
#				icount=\"$freq\" ipct=\"$ipct\" xis=\"$xrefid\"/>";
			    } else {
				if (!defined($freq) || !defined($ipct) || !$xrefid) {
				    warn "$input:$entry_lines{$entry}: no freqs/instances when printing form $f\n";
				    print "<f cbd:id=\"$xid\"/>";
				} elsif (!$form_ids{$f}) {
				    warn "$input:$entry_lines{$entry}: no form_id when printing form $f\n";
#				    use Data::Dumper; warn Dumper(\%form_ids);
				    print "<f cbd:id=\"$xid\" bad-form=\"$f\"/>";
				} else {
				    warn "$input:$entry_lines{$entry}: undef when printing form $f\n";
#				    print "<f cbd:id=\"$xid\" bad-form=\"$f\"/>";
				}
			    }
#			    }
			}
			print '</forms></norm>';
		    } else {
			my $m2attr = '';
			if ($f eq 'morph') {
			    if ($m1m2{$entry_ids{$entry},$k}) {
				$m2attr = ' morph2="'.join(' ',@{$m1m2{$entry_ids{$entry},$k}}).'"';
			    }
			}
			# 20190825 changed xml:id to cbd:id because overflowing LibXML2's xml:ids again :(
			print "<$f cbd:id=\"$xid\" n=\"$k\"$m2attr icount=\"$icount\" ipct=\"$ipct\" xis=\"$xrefid\"/>";
		    }
		}
	    }
	    print "</${f}s>";
	}
    }
}

sub
ipct {
    my($amount,$total) = @_;
    return 0 unless defined $amount && $total;
    if ($total) {
	my $pct = 100 * ($amount/$total);
	return int($pct+.5);
    } else {
	return 0;
    }
}

sub
load_parts {
    ++$parts_loaded;
    my $parts_file = '01tmp/l2p1-simple.sig';
    if (open(P,$parts_file)) {
	my $fields = <P>;
	while (<P>) {
	    chomp;
	    #	    my($lang,$entry) = (/\%(.*?):.*?=(.*?)\t\d+\$/);
	    my($lang,$entry) = (/\%(.*?):.*?=(.*?)\t/);
	    if ($lang) {
		$entry =~ s,//.*?\],],;
		$entry =~ s/'.*$//;
		$lang =~ s/-.*$//; ## is this too early to reduce to top-level lang?
		++${$parts{$entry}}{$lang};
	    }
	}
	close(P);
#	warn "dumping parts\n";
#	open(P,'>parts.dump'); use Data::Dumper; print P Dumper(\%parts); close(P);
    } else {
	warn "l2p2-g2x.plx: no $parts_file\n";
	return;
    }
}

sub
xload_parts {
    ++$parts_loaded;
    if (open(P,'01bld/parts.map')) {
	while (<P>) {
	    chomp;
	    my($e,$lang) = split(/\t/,$_);
	    $parts{$e} = $lang;
	}
	close(P);
	warn "dumping parts\n";
#	open(P,'>parts.dump'); use Data::Dumper; print P Dumper(\%parts); close(P);
    } else {
	warn "l2p2-g2x.plx: no 01bld/parts.map\n";
	return;
    }
}

#not quite right: we will have one rewrites map for the project rather than
#one per language.
sub
load_rewrites {
    my $rew = shift;
    open(REW,$rew) || die "l2p2-g2x.plx: can't open rewrites file `$rew'\n";
    while (<REW>) {
	# format is INCOMING => OUTGOING; partial incoming
    }
    close(REW);
}

sub
next_xid {
    sprintf("$id_base.x%08d",++$id_counter);
}

sub
rewrite_sig {
    my $sig = shift;
    # first look for the literal
    return sig_map($sig,$sig_rewrites{$sig},LITERAL) if $sig_rewrites{$sig};
    
    # now try the wildcards
    my $tmp = $sig;

    # first the full sig in any project
    $tmp =~ s/\@[a-z][a-z0-9][a-z0-9].*?://g;
    return sig_map($sig,$sig_rewrites{$tmp},WILDCARD) if $sig_rewrites{$sig};

    # now up to sense/epos granularity
    $tmp =~ s/\@[a-z][a-z0-9][a-z0-9].*(?:\+\+|$)//g;
    return sig_map($sig,$sig_rewrites{$tmp},SENSEPOS) if $sig_rewrites{$sig};

    # now cf[gw]pos granularity
    $tmp =~ s#//.*?](.*?)'.*(?:\+\+|$)#]$1#g;
    return sig_map($sig,$sig_rewrites{$tmp},CFGWPOS) if $sig_rewrites{$sig};

    # now we are just mapping this incoming sig to the new owner
    # project's version
    return sig_map($sig,$sig,IDENTITY);
}

## FIXME: This doesn't handle compounds yet.
sub
sig_map {
    my($from,$to,$level) = @_;
    if ($level == SENSEPOS) {
	# supply FORM and any peripherals from $from
	my $tmp = $from;
	$tmp =~ s/^\@[a-z][a-z0-9][a-z0-9\/]//;
	$to =~ s/\@[a-z][a-z0-9][a-z0-9\/].*$//;
	$to .= $tmp;
    } elsif ($level == CFGWPOS) {
	# supply FORM, SENSE, EPOS and any peripherals from $from
	my $tmp = $from;
	$tmp =~ s/^\@[a-z][a-z0-9][a-z0-9\/]//;
	$to =~ s/\@[a-z][a-z0-9][a-z0-9\/].*$//;
	$to .= $tmp;
	my($sense,$epos) = ($tmp =~ m#//(.*?)\].*?'(.*?)\@[a-z][a-z0-9][a-z0-9]#);
	$to =~ s#//(.*?)\]#//$sense]#;
	## REALLY ??? WHY IS @PROJECT AFTER EPOS ???
	$to =~ s/'(.*?)\@[a-z][a-z0-9][a-z0-9\/]/;$epos/;
    }
    # If this is a proper noun glossary, inherit the sig lang from
    # the QPN's sig lang, which gives the context environment in 
    # which the QPN was found.
    if ($header{'lang'} =~ /^qpn/) {
	$to =~ s/\@([a-z][a-z0-9][a-z0-9\/]*\%)/\@$project\%/g;
    } else {
	# otherwise ensure the $to has the project/lang of the glossary
	# being created.
	my $s949 = (/-949:/ ? "-949" : '');
	$to =~ s/\@([a-z][a-z0-9][a-z0-9\/]*?)\%(.*?):/\@$project\%$header{'lang'}$s949:/g;
    }
    if ($from ne $to) {
	$sigmap{$from} = $to;
    } else {
	$sigmap{$to} = '';
    }
    $to;
}

sub
xis {
    my($lang,$entry_id,$xid,$icount,$ipct,@refs) = @_;
    my @xrefs = xis_refs(@refs);
    my %xis_info = ();
    @xis_info{'xid','icount','ipct'} = ($xid,$icount,$ipct);
    $xis_info{'p_icount'} = (defined($icount) ? $icount : -1);

    my $xis_pair = undef;
    if (($xis_pair = $xrefs{"@xrefs"})) {
	$xrefid = $$xis_pair[0];
    } else {
	$xrefid = sprintf("%s.r%06x", $lang, ++$xrefid_counter);
	$xrefs{"@xrefs"} = [ $xrefid , $entry_id ];
    }

    $xis_info{'xrefid'} = $xrefid;

    ( %xis_info );
}

sub
xis_attr {
    my %xi = @_;
    $xi{'ipct'} = 0 unless $xi{'ipct'};
    my $xpicount = (($xi{'p_icount'} >= 0) ? $xi{'p_icount'} : 0);
#    print " icount=\"$xi{'p_icount'}\" ipct=\"$xi{'ipct'}\" xis=\"$xi{'xrefid'}\" xis-periods=\"$xi{'periods-id'}\" xis-percents=\"$xi{'periods-pct-id'}\"";
    print " icount=\"$xpicount\" ipct=\"$xi{'ipct'}\" xis=\"$xi{'xrefid'}\"";
}

sub
xis_dump_periods {
    print '<xis:periods xmlns:xis="http://oracc.org/ns/xis/1.0">';
    foreach (sort { $xis_periods{$a} cmp $xis_periods{$b} } keys %xis_periods) {
	my $p = join '', map { "<i>$_</i>" } split(/:/, $_);
	print "<xis:pd xml:id=\"$xis_periods{$_}\" key=\"$_\">$p</xis:pd>";
    }
    print '</xis:periods>';
}

sub
xis_rr {
    foreach my $rr (sort { $xrefs{$a} cmp $xrefs{$b} } keys %xrefs) {
	my $xispair = $xrefs{$rr};
	my $xid = $$xispair[0];
	if ($xid =~ /^[^a-zA-Z]/) {
	    warn("bad xid $xid, fixing to XXX.$xid");
	    $xid = "XXX.$xid";
	}
	my %exis = %{$entry_xis{$$xispair[1]}};
	my $icount = $exis{'icount'} || 0;
	printf XIS "<xis xml:id=\"$xid\" efreq=\"$icount\">";
	foreach my $r (split(/\s+/,$rr)) {
	    print XIS "<r>$r</r>";
	}
	print XIS '</xis>';
	print XISTAB "$xid\t$icount\t$rr\n";
    }
}

sub
xis_refs {
    my @sigrefs = @_;
    my %r = ();
    foreach my $s (@sigrefs) {
	@r{split(/\s+/,$sigrefs{$s})} = ()
	    if $sigrefs{$s};
    }
    sort keys %r;
}

sub
make_cof_norm {
    my $f = shift;
    my @n = ();
    foreach my $fbit (split(/\&\&/, $f)) {
	my %s = parse_sig ($fbit);
	if ($baselang eq 'sux') {
	    my $m = $s{'morph'};
	    $m =~ s/~/$s{'base'}/;
	    $m =~ s/\%.*?://g;
	    push @n, $m;
	} else {
	    if ($s{'norm'}) {
		push @n, $s{'norm'};
	    } else {
		warn "$input:$.: bad NORM in COF\n";
	    }
	}
    }
    my $n = join(' ', @n);
#    warn "make_cof_norm: $f => $n\n";
    $n;
}

sub
rewrite_bases {
    my($lnum,$origf,$originst) = @_;
    my %new_b = ();
    my %new_i = ();
#    use Data::Dumper;

    return unless $$origf{'base'};

    my %b = %{$$origf{'base'}};
    my %i = %{$$originst{'base'}};

#    warn "b=", Dumper(\%b);
#    warn "i=", Dumper(\%i);

    my %sig = ();
    foreach my $b (keys %b) {
	my $tmpb = $b;
	$tmpb =~ s/%[-a-z0-9]+://g;
	my $sig = ORACC::SL::Tlitsig::tlitsigx("$project/$input\:$tmpb", $tmpb);
	push @{$sig{$sig}}, $b;
    }
 
#    warn "sigs=", Dumper(\%sig);

    foreach my $s (keys %sig) {
	my @b = @{$sig{$s}};
	if ($#b > 0) {
	    @b = sort { $b{$b} <=> $b{$a} } @b;
	    for (my $i = 0; $i <= $#b; ++$i) {
		$new_b{$b[0]} += $b{$b[$i]}; # total freqs
		@{$new_i{$b[0]}}{keys %{$i{$b[$i]}}} = ();
	    }
	} else {
	    $new_b{$b[0]} = $b{$b[0]};
	    $new_i{$b[0]} = $i{$b[0]};
	}
    }

#    warn "new_b=", Dumper(\%new_b);
#    warn "new_i=", Dumper(\%new_i);

#    exit 0 if ++$done == 10;

    $$origf{'base'} = { %new_b };
    $$originst{'base'} = { %new_i };
}

sub
set_f {
    my $fields = shift;
    %f = ();
    my @f = split(/\s+/, $fields);
    shift @f; # drop '@field';
    for (my $i = 0; $i <= $#f; ++$i) {
	$f{$f[$i]} = $i;
    }
}

sub
read_input {
#    warn "read_input\n";
    if ($input eq '-') {
	while (<>) {
	    read_input_line();
	}
    } else {
	open(S,$input) || die "l2p2-g2x.plx: can't open `$input'\n";
	while (<S>) {
	    read_input_line();
	}
	close(S);
    }
}

sub
read_input_line {
    return if /^\s*$/;
    
    return if /\t0$/ && !$with_zero_freqs;
    
    if (/^\@(project|name|lang)/) {
	my $h = $1;
	if (/^\S+\s+(\S+)/) {
	    $header{$h} = $1
		unless $header{$h};
	} else {
	    warn "l2p2-g2x.plx:$input:$.: malformed '$h' in header\n";
	}
	return;
    }

    chomp;
    
    if (/^\@fields/) {
	set_f($_);
	return;
    }
    
#    warn "l2p2-g2x.plx: processing $input ($lang/$name/$proj)\n" if $verbose;
    
    my ($sig,$rank,$freq,$refs) = ();
    my @t = split(/\t/,$_);
    $sig = $t[0]; # sig is always first
    $rank = $t[$f{'rank'}] if exists $f{'rank'};
    $freq = $t[$f{'freq'}] if exists $f{'freq'};
    $refs = $t[$f{'inst'}] if exists $f{'inst'};
    return if $seen{$sig}++;
    
    $baselang = $header{'lang'};
    $baselang =~ s/-.*$//;
    $id_base = "$header{'lang'}"; # $header{'project'}.
    $id_base =~ tr#/#_#;
    $refs = '' unless $refs;
    $freq = ($refs =~ tr/ / /) + ($refs ? 1 : 0);
    
    if ($sig =~ /\&\&/) {
	my @cof = ();
	my $index = 0;
	foreach my $c (split(/\&\&/, $sig)) {
	    push(@cof, add_sig($c, $freq, $refs, $sig, $index++));
	}
	$cofs{$sig} = [ @cof ];
    } else {
	add_sig($sig, $freq, $refs, 0);
    }
}

sub
sort_forms {
    my %cgc = ();
    my $tmpname = '';
    if (open(TMP,">01tmp/$$-forms.cgc")) {
	$tmpname = "01tmp/$$-forms.cgc";
    } elsif (open(TMP,">/tmp/$$-forms.cgc")) {
	$tmpname = "/tmp/$$-forms.cgc";
    } else {
	die "set_cgc: can't write /tmp/$$-forms.cgc or tmp/$$-forms.cgc\n";
    }
    foreach my $t (keys %forms) {
	my $k = $t;
	$t =~ s/^\%.*?://;
	my $t2 = "\L$t";
	if ($t eq '*') {
	    $t = '0';
	} else {
	    $t =~ s/\{.*?\}//g;
	    $t =~ tr/./-/;
	    $k =~ tr/ /_/;
	    $t =~ tr/ /_/;
	    $t2 =~ tr/ /_/;
	}
	print TMP "${t} \t$t2\t$k\n";
    }
    close TMP;
    system 'cp', "01tmp/$$-forms.cgc", "01tmp/$$-forms.bak";
#    system 'msort', '-j', '--out', $tmpname, '-ql', '-n1', '-n2', '-s',
    #    "$ENV{'ORACC'}/lib/config/msort.order", '-x', "$ENV{'ORACC'}/lib/config/msort.exclude", $tmpname;
    system 'psdsort', '-i', '-G', '-o', $tmpname, $tmpname;
    open(TMP,$tmpname);
    my @cgc = (<TMP>);
    close(TMP);
#    unlink $tmpname;
    chomp @cgc;
    @cgc = map { tr/_/ /; m/^.*?\t.*?\t(.*)$/; $1 } @cgc;
    @cgc{@cgc} = (1..($#cgc+1));
#    use Data::Dumper; warn Dumper \%cgc;
    foreach my $n (keys %forms) {
	warn "sort forms failure: form $n has no sort code\n"
	    unless defined $cgc{$n};
	$forms{$n} = $cgc{$n};
    }
}

sub
sort_norms {
    my %cgc = ();
    my $tmpname = '';
    if (open(TMP,">01tmp/$$-norms.cgc")) {
	$tmpname = "01tmp/$$-norms.cgc";
    } elsif (open(TMP,">/tmp/$$-norms.cgc")) {
	$tmpname = "/tmp/$$-norms.cgc";
    } else {
	die "set_cgc: can't write /tmp/$$-norms.cgc or tmp/$$-norms.cgc\n";
    }
    foreach my $t (keys %norms) {
	$t =~ tr/_/—/; ### HACK !!!
	$t =~ tr/ /_/;
	print TMP "${t}_\n";
    }
    close TMP;
    system 'msort', '-j', '--out', $tmpname, '-ql', '-n1', '-s', 
    "$ENV{'ORACC'}/lib/config/msort.order", '-x', "$ENV{'ORACC'}/lib/config/msort.exclude", $tmpname;
    open(TMP,$tmpname);
    my @cgc = (<TMP>);
    close(TMP);
#    unlink $tmpname;
    chomp @cgc;
    @cgc = map { s/_$//; tr/_—/ _/; $_ } @cgc;
    @cgc{@cgc} = (0..$#cgc);
    foreach my $n (keys %norms) {
	warn "sort norms failure: norm $n has no sort code\n"
	    unless defined $cgc{$n};
	$norms{$n} = $cgc{$n};
    }
}

sub entry_form_string {
    my @k = keys %{$entryformstems{$_[0],$_[1]}};
    join(',',@k);
}

1;
