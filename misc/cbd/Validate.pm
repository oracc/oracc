package ORACC::CBD::Validate;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/pp_validate v_project v_lang v_form v_is_entry v_set_cfgw/;

use warnings; use strict; use open 'utf8'; use utf8;

my @tags = qw/letter entry oid parts bff bases stems phon root form length norms
    sense equiv inote prop end isslp bib defn note alias
    pl_coord pl_id pl_uid was moved project lang name collo proplist prop ok
    allow file/;

my %tags = (); @tags{@tags} = ();

my %validators = (
    letter=>\&v_letter,
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
    oid=>\&v_oid,
    was=>\&v_deprecated,
    moved=>\&v_deprecated,
    bff=>\&v_bff,
    collo=>\&v_collo,
    geo=>\&v_geo,
    usage=>\&v_usage,
    proplist=>\&v_proplist,
    prop=>\&v_prop,
    alias=>\&v_alias,
    pl_id=>\&v_pl_id,
    pl_uid=>\&v_pl_uid,
    pl_coord=>\&v_pl_coord,
    length=>\&v_length,
    file=>\&v_file,
    ok=>\&v_ok,
    );

use ORACC::CBD::Util;
use ORACC::L2GLO::Langcore;
use ORACC::CBD::ATF;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Props;
use ORACC::CBD::Sigs;
use ORACC::CBD::Bases;
use ORACC::SL::BaseC;
use Data::Dumper;

#################################################
#
#  VALIDATION
#
#################################################

my $bid = 'b000001';
my $eid = 'x000001';

my $acd_chars = '->+=!';
my $acd_rx = '['.$acd_chars.']';
$ORACC::CBD::Edit::acd_rx = $acd_rx;

my @acd_ok_tags = qw/entry sense/;
my %acd_ok_tags = (); @acd_ok_tags{@acd_ok_tags} = ();

my @poss = qw/AJ AV N V DP IP PP CNJ J MA O QP RP DET PRP POS PRT PSP PTC
    SBJ NP M MOD REL XP NU AN BN CN DN EN FN GN HN IN JN KN LN MN NN
    ON PN QN PNF RN SN TN U UN VN WN X XN YN ZN/; 

push @poss, ('V/t', 'V/i'); 
my %poss = (); @poss{@poss} = ();

my %entry_map = ();
my %sense_map = ();

my @geo_pos = qw/AN EN FN GN ON SN QN TN WN/;
my %geo_pos = (); @geo_pos{@geo_pos} = ();

my @stems = qw/B rr RR rR Rr rrr RRR rrrr RRRR S₁ S₂ S₃ S₄/;
my %stems = (); @stems{@stems} = ();

my %rws_map = (
    EG => 'sux',
    ES => 'sux-x-emesal',
    UGN => 'sux-x-udganu',
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

my @funcs = qw/free impf perf Pl PlObj PlSubj Sg SgObj SgSubj/;
my %funcs = (); @funcs{@funcs} = ();

# semantic {e} and {ga} are marked in sux.glo w {-e} and {-ga}
my @known_det = qw/
    anše bad₃ buru₅ d dug e₂ gada gana₂ geme₂ gi gud 
    hašhur ŋeš id₂ iku im iri itud ki ku₆ kug-babbar kug-sig₁₇ 
    kaš kur kuš lu₂ m mul munus muš mušen na₄ ninda peš₂
    sar siki su-din šah šah₂ še šim šum₂ tug₂ tumu u₂ 
    udu urud uzu zabar zabar₃ zid₂/;
my %known_det = (); @known_det{@known_det} = ();

my $stem_validator;

my %vfields = ();
my %arg_vfields = ();

my %allow = ();
my %bad_compounds = ();
my %bases = ();
my %basedata = ();
my %basesigs = ();
my @bffs = ();
my $curr_cf = '';
my $curr_cfgw = '';
my $curr_id = '';
my $detcheck = 0;
my @global_cbd = ();
my %entries = ();
my %entries_nopos = ();
my %entries_cfmng = ();
my %gdlsigs = ();
my $in_entry = 0;
my $init_acd = 0;
my $is_compound = 0;
my $lang = '';
my $entry_lang = ''; 	# this is set when RWS tokens @EG, @ES @UGN are encountered;
			# the semantics of Akk RWS are different--the RWS gives the
			# CF for the dialect
my $mixed_morph = 0;
my %ok = ();
my @parts = ();
my $project = '';
my $status = 0;
#my %tag_lists = ();
my %seen_entries = ();
my $seen_bases = 0;
my $seen_sense = 0;
my $seen_morph2 = 0;
my %tlit_sigs = ();
my $trace = 0;
my $vfields = '';
my $always_check_base = 0;
my %data = ();

sub init {
    my $vfields = shift;
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
}
sub reinit {
    %allow = ();
    %bad_compounds = ();
    %bases = ();
    %basedata = ();
    @bffs = ();
    $curr_cf = '';
    $curr_cfgw = '';
    $curr_id = '';
    $detcheck = 0;
    @global_cbd = ();
    %entries = ();
    %entries_nopos = ();
    %entries_cfmng = ();
    $in_entry = 0;
    $init_acd = 0;
    $is_compound = 0;
    $lang = '';
    $mixed_morph = 0;
    %ok = ();
    @parts = ();
    $project = '';
    $status = 0;
    %seen_entries = ();
    $seen_bases = 0;
    $seen_sense = 0;
    $seen_morph2 = 0;
    %tlit_sigs = ();
    %data = ();
}

sub pp_validate {
    my($args,@cbd) = @_;

#    warn "validate: entering pp_validate for ".ORACC::CBD::Util::cbdname()."\n";
    
    %data = %ORACC::CBD::data;

    $detcheck = 1 if $$args{'dets'};
    
    my %glodata = (); # entries, bffs, psu, etc., for this cbd
    $trace = $ORACC::CBD::PPWarn::trace;
    @global_cbd = @cbd;
    ($project,$lang,$vfields) = (ORACC::CBD::Util::project(), ORACC::CBD::Util::lang(), $$args{'vfields'});
    init($vfields);

    %entry_map = ();
    %sense_map = ();
    
    $bid = 'b000001';
    $eid = 'x000001';

    %entries = ();
    %entries_nopos = ();
    %entries_cfmng = ();
    %seen_entries = ();
    %bases = ();
    %basedata = ();
    @bffs = ();
    @parts = ();
    %tlit_sigs = ();
    %ORACC::CBD::forms = ();
	
    ORACC::SL::BaseC::init();
    $ORACC::SL::report_all = 1;
    if ($lang =~ /^(sux|qpn)/ && $project =~ /epsd|dcclt|blms|gkab/) {
	ORACC::SL::BaseC::pedantic(1);
	$always_check_base = 1;
	$detcheck = 1 if pp_file() eq '00src/sux.glo';
    }

    if ($lang =~ /^akk/) {
	$stem_validator = \&v_stem_akk;
    } elsif ($lang =~ /^sux/) {
	$stem_validator = \&v_stem_sux;
    } else {
	$stem_validator = \&v_stem_bad;
    }

    if ($lang =~ /emesal/ && $detcheck) {
	++$known_det{'mu'};
    }
    
    for (my $i = 0; $i <= $#cbd; ++$i) {
	next if $cbd[$i] =~ /^\000$/;
	if ($cbd[$i] =~ /^\#/) {
	    if ($cbd[$i] =~ /^\#\@ok/) {
		++$ok{$curr_id};
	    }
	    next;
	}
	pp_line($i+1);

	if ($cbd[$i] =~ /^\s*$/) {
	    if ($in_entry) {
		pp_warn("blank lines not allowed in \@entry");
	    }
	    next;
	} else {
	    unless ($in_entry || $cbd[$i] =~ /^$acd_rx*\@(?:project|lang|name|entry|letter|proplist)/) {
		$i = out_entry($i, @cbd);
		next;
	    }
	}

	if ($cbd[$i] =~ /^\@([A-Z]+)(?:\s*(\S.*))?$/) {
	    my $rws = $1;
	    if ($seen_bases) {
		pp_warn("\@$1 should come before \@bases");
	    }
	    unless ($rws_map{$rws}) {
		pp_warn("\@$1 unknown register/writing-system/dialect");
	    } else {
		$entry_lang = $rws_map{$rws} if $rws =~ /^(?:EG|ES|UGN) /
	    }
	} elsif ($cbd[$i] =~ /^($acd_rx*)@([a-z_]+)/) { # \s+(.*)\s*$/o) {
	    my ($pre,$tag,$post) = ($1,$2);
	    if (exists $tags{$tag}) {
		# push @{$tag_lists{$tag}}, $i;		
		if ($validators{$tag}) {
		    if (exists $vfields{$tag}) {
			if ($cbd[$i] =~ m/^(\S+)\s+(.*?)\s*$/) {
			    my($t,$l) = ($1,$2);
			    &{$validators{$tag}}($t,$l,$i,\@cbd,$pre);
			} else {
			    &{$validators{$tag}}($cbd[$i],'',$i,\@cbd,$pre);
			}
		    }
		} else {
		    if ($tag eq 'allow') {
			my $a = $cbd[$i];
			if ($cbd[$i] =~ /(\S+)\s+=\s+(\S+)/) {
			    ++${$allow{$1}}{$2};
			    ++${$allow{$2}}{$1};
			} else {
			    pp_warn("\@allow must have the form PRIMARY_BASE = PRIMARY_BASE");
			}
#			$cbd[$i] = "\000";
		    } else {
			pp_warn("internal error: no validator function defined for tag `$tag'");
		    }
		}
		if ($pre) {
		    if (exists $acd_ok_tags{$tag}) {
			push @{$data{'edit'}}, pp_line()-1;
			if ($pre =~ /^>/) {
			    if ($tag eq 'entry') {
				$cbd[$i] =~ /entry\S*\s+(.*)\s*$/;
				$entry_map{$curr_cfgw} = $1;
			    } elsif ($tag eq 'sense') {
				$cbd[$i] =~ /sense\S*\s+(.*)\s*$/;
				my $to_sense = $1;
				my $from_sense = '';
				if ($cbd[$i-1] =~ /^\@sense\S*\s+(.*?)\s*$/) {
				    $from_sense = $1;
				}
				if ($from_sense) {
				    ${$sense_map{$curr_cfgw}}{$from_sense} = $to_sense;
				} else {
				    pp_warn("no previous SENSE corresponding to edit mark > (nothing allowed between \@sense and >\@sense)");
				}
			    } else {
				pp_warn("internal error: tag `$tag': edit mark processing not defined in Validate.pm");
			    }
			}
		    } else {
			pp_warn("edit mark '$pre' doesn't belong on \@$tag");
		    }
		}
	    } else {
		pp_warn("\@$tag unknown tag");
	    }
	} elsif ($cbd[$i] =~ /^($acd_rx+)/o) {

	    my $x = $1;
	    push @{$data{'edit'}}, pp_line()-1;
	    if ($x eq '>' && $cbd[$i-1] =~ /^$acd_rx*\@entry/) {
		$cbd[$i] =~ /^>\s*(.*?)\s*$/;
		my $ncfgw = $1; $ncfgw =~ s/\s*(\[.*?\])\s*/ $1 /;
		pp_warn("syntax error in > $ncfgw") unless $ncfgw =~ s/^(.*?)\s+(\[.*?\])\s+(\S+)$/$1 $2 $3/;
		$entry_map{$curr_cfgw} = $1;
	    } elsif ($x eq '>' && $cbd[$i-1] =~ /^\@sense/) {
		$cbd[$i] =~ /^>\s*(.*?)\s*$/;
		${$sense_map{$curr_cfgw}}{$cbd[$i-1]} = "\@sense $1"; # should validate tok1 of $1 for legal POS
	    } elsif ($x eq '>') {
		pp_warn("misplaced edit marker '>' (preceding line must be \@entry or \@sense)");
	    }

	} else {
	    pp_warn("invalid line in glossary: $cbd[$i]");
	}
    }

#    open(D,'>cfmng.dump');
#    print D Dumper \%entries;
#    close(D);
    
    foreach my $p (@parts) {
	v_part($p);
    }

    pp_trace("calling atf_check at pp_line()==", pp_line());
    atf_check($project,$lang);
    ORACC::SL::BaseC::pedantic(1);
    cpd_check($project,$lang, $$args{'file'});

    %{$glodata{'bffs'}} = bff_check();
    %{$glodata{'entries'}} = %entries;
    %{$glodata{'entry_map'}} = %entry_map; # warn 'Validate: ', Dumper \%entry_map;
    %{$glodata{'sense_map'}} = %sense_map; # warn 'Validate: ', Dumper \%sense_map;
    %{$glodata{'basedata'}} = %basedata;
    @{$glodata{'parts'}} = @parts;
    %{$glodata{'ok'}} = %ok;

    sigs_check(\%glodata,$args,@cbd) if $$args{'check'};

    pp_status(0) if $ORACC::CBD::novalidate;

    my $cbdname = ORACC::CBD::Util::cbdname();
#    warn "$0: validate: setting cbdname to $cbdname\n";
    push @{$data{'cbds'}}, $cbdname;
    ${$data{'cbdmap'}}{pp_file()} = $cbdname;
    %{$data{$cbdname}} = %glodata;
    
    %ORACC::CBD::data = %data;

    ORACC::CBD::Bases::bases_sigs($args, \%basesigs) if $$args{'bases'} || $project eq 'epsd2';

#    warn "validate: exiting pp_validate status=".pp_status()."\n";
#    warn "pp_status now=".pp_status()."\n";

    @cbd;
}

sub out_entry {
    my($i,@c) = @_;
    pp_warn("missing \@entry");
    while ($i < $#c) {
	last if $c[$i] =~ /^\@end/;
	++$i;
    }
    $i;
}

sub v_project {
    my($line,$arg) = @_;
    return if $arg;
    my $proj = '';
    if ($line =~ /\@project\s+(\S+)\s*$/) {
	$proj = $1;
    } else {
	pp_warn("project empty or malformatted");
    }
    $proj;
}

sub v_lang {
    my($line,$arg) = @_;
    return if $arg;
    my $vlang = '';
    if ($line =~ /\@(?:qpnbase)?lang\s+(\S+)\s*$/) {
	$vlang = $1;
	if ($vlang =~ s/^\%//) {
	    pp_warn("language in header should not start with `%'");
	}
	unless (lang_known($vlang)) {
	    pp_warn("unknown language `$vlang' in header");
	    $vlang = '';
	}
	
    } else {
	pp_warn("language empty or malformatted");
    }
    $vlang;
}

sub v_name { 
    my($tag,$arg) = @_;
    $arg;
}

sub v_letter {
    my($tag,$arg) = @_;
}

sub v_entry {
    my($tag,$arg,$i,$cbdref) = @_;
    if ($trace && exists $arg_vfields{'entry'}) {
	pp_trace("v_entry: tag=$tag; arg=$arg");
    }

    if ($arg =~ s/\s+>\s+(.*?)\s*$//) {
	$entry_map{$arg} = $1;
#	warn " > $$cbdref[$i]\n";
	$$cbdref[$i] =~ s/\s+>\s+.*$//;
#	warn " > $$cbdref[$i]\n";
    }
    
    $seen_sense = 0;
    my($pre,$etag,$pst) = ($tag =~ /^($acd_rx)?\@(\S+?)(\*?\!?)$/);
    my ($cf,$gw,$pos) = ();
    if ($etag && $etag eq 'entry') {
	$pre = '' unless $pre;
	$pst = '' unless $pst;
	if ($in_entry) {
	    if ($pre) {
		if ($in_entry > 1) {
		    pp_warn("multiple acd \@entry fields not permitted");
		} else {
		    # push @{$data{'edit'}}, pp_line()-1;
		}
	    } else {
		pp_warn("multiple \@entry fields not permitted");
	    }
	    ++$in_entry;
	} elsif ($in_entry > 1) {
	    pp_warn("max two \@entry fields allowed");
	} else {
	    ++$in_entry;

	    # $curr_cfgw =~ tr/_/ /; #map _ to &nbsp;

	    $curr_cfgw = $arg;

	    $curr_id = $entries{$curr_cfgw} = $eid++;
	    my $nopos = $curr_cfgw; $nopos =~ s/\].*$/]/;
	    warn "$i: $nopos has no [...]\n" unless $nopos =~ /\[.*?\]/;
	    $curr_cf = $nopos; $curr_cf =~ s/\s*\[.*$//;
	    push @{$entries_nopos{$nopos}}, $curr_cfgw;
	    $entries{$curr_id} = $curr_cfgw;
	    $entries{$curr_id,'line'} = pp_line()-1;
	    
	    ($cf,$gw,$pos) = ($arg =~ /^([^\[]+)\s+(\[[^\]]+\])\s+(\S+)\s*$/);
	    if (!$cf) {
		if ($arg =~ /\[/ && $arg !~ /\s\[/) {
		    pp_warn("missing space before [ in CF [GW] POS");
		} elsif ($arg =~ /\]/ && $arg !~ /\]\s/) {
		    pp_warn("missing space after ] in CF [GW] POS");
		} elsif ($arg =~ /\]/ && $arg !~ /\]\s*$/) {
		    pp_warn("missing POS in CF [GW] POS");
		} else {
		    pp_warn("syntax error in \@entry's CF [GW] POS");
		}
	    } else {
		if ($cf =~ tr/'/'/) {
		    pp_warn("quote mark ' in citation form should be raised right half circle, ʾ");
		}
		if ($cf =~ tr/\$//) {
		    pp_warn("dollar sign '\$' not allowed in citation form");
		}
		$is_compound = ($cf =~ /\s/);
		if ($is_compound && !has_parts($i,$cbdref)) {
		    pp_warn("compound or usage without \@parts entry");
		    ++$bad_compounds{$curr_cfgw};
		}
		if (exists $poss{$pos}) {
		    if (exists $geo_pos{$pos}) {
			push @{$data{'geo'}}, pp_line()-1;
		    }
		} else {
		    pp_warn("unknown POS '$pos'");
		}
		my $ee = "$cf $gw $pos";
		if ($seen_entries{$ee}++) {
		    pp_warn("duplicate entry `$ee'");
		}
	    }
	}
	if ($pre) {
	    v_acd_ok($pre);
	}
	if ($pst) {
	    if ($pst !~ /^\*|\!$/) {
		pp_warn("bad \@entry suffix: allowed sequences are '*', '!'");
	    }
	}
	if ($trace && exists $arg_vfields{'entry'}) {
	    $cf = '' unless $cf;
	    $gw = '' unless $gw;
	    $pos = '' unless $pos;
	    pp_trace "entry: cf=$cf; gw=$gw; pos=$pos; pre=$pre, pst=$pst";
	}
    } else {
	pp_warn("bad format in \@entry '$tag'. (acd=$acd_rx)");
    }
}

sub v_acd_ok {
    my $pre = shift;
    if ($pre !~ /^$acd_rx$/) {
	pp_warn("(acd) only $acd_rx allowed");
    } else {
	if (length($pre) > 1) {
	    pp_warn("(acd) only one of $acd_rx allowed");
	}		 
    }
}

sub gdlsigs {
    my %s = @_;

    foreach my $core (keys %s) {
	1 while $core =~ s/\{[^\}]+?\{.*?\}.*?\}//; # remove { ... {...} ... } first
	1 while $core =~ s/\{[^}]+\}//;
	++$s{$core};
    }

    fopen(S,">$$.gsig") || die "$.: unable to write to $$.gsig\n";
    print join("\n", keys %s), "\n";
    fclose(S);

    my @s = `gdlx -d -s -f $$.gsig`; chomp @s;
    foreach my $s (@s) {
	my($gdl,$sig) = split(/\t/, $s);
	$gdlsigs{$gdl} = $sig;
    }
}

sub v_bases {
    my($tag,$arg) = @_;

    if ($trace && exists $arg_vfields{'bases'}) {
	pp_trace "v_bases: tag=$tag; arg=$arg";
    }

    if ($arg =~ s/^\s*;//) {
	pp_warn("bases entry begins with semi-colon--please remove it");
    }

    if ($arg =~ s/;\s*;/;/g) {
	pp_warn("bases entry contains '; ;' or similar--please fix this");
    }

    if ($arg =~ s/;\s*$//) {
	pp_warn("bases entry ends with semi-colon--please remove it");
    }

    my @bits = split(/;\s+/, $arg);
    if ($trace && exists $arg_vfields{'bases'}) {
	pp_trace "v_bases: \@bits=@bits";
    }

    if ($seen_bases++) {
	pp_warn("\@bases can only be given once");
	return;
    }

    ++$ORACC::CBD::det_minus if $arg =~ /\{-/;
    
    my $alt = '';
    my $stem = '';
    my $blang = '';
    my $pri = '';
    my %vbases = (); # this one is just for validation of the current @bases field
    my $pricode = 0;

    my %gdlsigs_needed = ();
    
    foreach my $b (@bits) {
#	warn "base bit :$b:\n";
	if ($b =~ s/^\*(\S+)\s+//) {
	    $stem = $1;
	} elsif ($b =~ /^\*/) {
	    $b =~ s/^\*\s*//;
	    pp_warn("misplaced '*' in \@bases");
	}

	if ($b =~ s/^\%(\S+)\s+//) {
	    $blang = xflang($1);
	} elsif ($b =~ /^\%/) {
	    $b =~ s/^\%\s*//;
	    pp_warn("misplaced '%' in \@bases");
	}
	$blang = $entry_lang unless $blang;
	
	if ($b =~ /\s+\(/) {
	    my $tmp = $b;
	    pp_warn("malformed alt-base in `$b'")
		if ($tmp =~ tr/()// % 2);
	    ($pri,$alt) = ($b =~ /^(.*?)\s+\((.*?)\)\s*$/);
	    if ($pri) {
		if ($pri =~ s/>.*$//) {
		    push @{$data{'edit'}}, pp_line()-1;
		}
		if ($pri =~ /\s/ && !$is_compound) {
		    pp_warn("space in base `$pri'");
		    $pri = $alt = '';
		} else {
		    ++$bases{$pri};
		    ++$gdlsigs_needed{$pri} unless $gdlsigs{$pri};
		    $bases{$pri,'*'} = $stem
			if $stem;
		    $bases{$pri,'%'} = $blang;
		}
		if ($pri) {
		    det_check($pri) if $detcheck;
		    if (defined $vbases{$pri}) {
			pp_warn("repeated base $pri");
		    } else {
#			qualcheck($pri) if $pri =~ /\(/;
			%{$vbases{$pri}} = ();
			$vbases{"$pri#code"} = ++$pricode;
		    }
		    foreach my $a (split(/,\s+/,$alt)) {
			if ($a =~ /\s/ && !$is_compound) {
			    pp_warn("space in alt-base `$a'");
			    $pri = $alt = '';
			} else {
			    if ($a) {
				if (${$vbases{$pri}}{$a}++) {
				    pp_warn("$pri has repeated alternate base $a");
				}
				# all alternates for this primary
				++${$vbases{"$pri#alt"}}{$a};
				$bases{"#$a"} = $pri;
				++$gdlsigs_needed{$a} unless $gdlsigs{$a};
				# all alternates in this @bases
				if ($vbases{'#alt'} && ${$vbases{'#alt'}}{$a}) {
				    if (${$vbases{'#alt'}}{$a} eq '1') {
					pp_warn("detected error in alt base--check other error messages");
				    } else {
					if (defined ${${$vbases{'#alt'}}{$a}}) {
					    my $prevpri =  ${${$vbases{'#alt'}}{$a}};
					    pp_warn("alt $a already defined for primary $prevpri");
					} else {
					    ${${$vbases{'#alt'}}{$a}} = $pri;
					}
				    }
				}
			    }
			}
		    }
		}
	    } else {
		pp_warn("syntax error in base with (...) [missing paren?]");
	    }
	} else {
	    if ($b =~ /\s/ && !$is_compound) {
		pp_warn("space in base `$b'");
		$pri = $alt = '';
	    } else {
		++$bases{$b};
		$bases{$b,'*'} = $stem
		    if $stem;
		$pri = $b;
		$alt = '';
		det_check($pri) if $detcheck;
		if (defined $vbases{$pri}) {
		    pp_warn("repeated base $pri");
		} else {
#		    qualcheck($pri) if $pri =~ /\(/;
		    %{$vbases{$pri}} = ();
		    $vbases{"$pri#code"} = ++$pricode;
		}
	    }
	}
    }

    # Now that we have all the primary and alternate bases syntactically validated
    # and captured in %vbases we can do some more validation ...

#    warn "second phase base checking\n";

    # get sigs for bases not previously looked up via gdlsig
    gdlsigs(%gdlsigs_needed);
    
    # 1. Does more than one primary have the same signs?
    my %prisigs = ();
    my %altsigs = ();
    foreach my $p (sort keys %vbases) {
	next if $p =~ /\#/;
	pp_trace("BaseC::check: $p");
	#	my $px = $p; $px =~ s/ₓ\(//g; pp_warn("(bases) $px has x-value with no qualifier") if $px =~ /ₓ/;
	###	my $psig = ORACC::SL::BaseC::check(undef,$p, 1);

	my $psig = $gdlsigs($p);

	#	warn ORACC::SL::BaseC::messages(), "\n";
	if ($psig eq 'q00') {
	    pp_warn("(bases) primary base $p not in OGSL");
#	} elsif (@ORACC::SL::fixes_needed >= 0) {
#	    foreach my $f_n (@ORACC::SL::fixes_needed) {
#		my $new_p = bases_fix_base($p,$f_n[0], $f_n[1]);
#		if ($new_p) {
#		    $p = $new_p;
#		} else {
#		    pp_warn("(bases) autofix of $p ($f_n[0] -> $f_n[1]) failed");
#		}
#	    }
	}
	unless ($lang =~ /qpn/) {
	    atf_add($p,$lang) if $p;
	}
	unless (pp_sl_messages($p)) {
	    if (defined $prisigs{$psig} && !is_allowed($p,$prisigs{$psig})) {
		pp_warn("(bases) primary bases '$p' and '$prisigs{$psig}' are the same (both = $psig)");
	    } else {
#		warn "adding $psig to prisigs for $p\n";
		$prisigs{$psig} = $p;
		$prisigs{$p} = $psig;
		if ($p =~ /\{/) {
		    my $core = $p;
		    1 while $core =~ s/\{[^\}]+?\{.*?\}.*?\}//; # remove { ... {...} ... } first
		    1 while $core =~ s/\{[^}]+\}//;
		    if (!defined $vbases{$core}) {
			# my $csig = ORACC::SL::BaseC::check(undef,$core,1);
			my $csig = $gdlsigs{$core};
			if (defined $prisigs{$csig} && !is_allowed($core,$prisigs{$csig})) {
			    pp_warn("(bases) core $core of base $p should be $prisigs{$csig}");
			}
		    }
		}
	    }
	    register_base_sig($p,$psig);
	}
    }

    # 2. Does each alternate have the same signs as its primary?
    foreach my $p (sort keys %vbases) {
	next if $p =~ /\#/;
	my $prisig = $prisigs{$p}; # if this is empty there was an error earlier
	if ($prisig && defined $vbases{"$p#alt"}) {
	    my @alts = sort keys %{$vbases{"$p#alt"}};
	    my $pcode = $vbases{"$p#code"};
	    foreach my $a (@alts) {
		my $ped = ORACC::SL::BaseC::pedantic(0);
		pp_trace("BaseC::check: $a");
		# my $asig = ORACC::SL::BaseC::check(undef,$a, 1);
		# ORACC::SL::BaseC::pedantic($ped) if $ped;
		my $asig = gdlsigs{$a};
		unless ($lang =~ /qpn/) {
		    atf_add($a,$lang) if $a;
		}
		unless (pp_sl_messages()) {
		    if ($prisig ne $asig) {
			pp_warn("(bases) primary '$p' and alt '$a' have different signs ($prisig ne $asig)");
		    }
		    $altsigs{$asig} = $a;
		    $altsigs{"$asig#code"} = $pcode;
		    $altsigs{$a} = $asig;
		}
	    }
	}
    }

    # 3. Does a primary occur as an alternate?
    foreach my $p (sort keys %vbases) {
	next if $p =~ /\#/;
	my $prisig = $prisigs{$p};
	if ($prisig && $altsigs{$prisig}) {
	    my $vbpc = $vbases{"$p#code"};
	    my $aspc = $altsigs{"$prisig#code"};
	    pp_warn("(bases) primary '$p' is also an alt of '$altsigs{$prisig}'")
		unless  $vbpc == $aspc;
	}
    }

    # 4. For compounds, if it isn't in the sign list does it use the right component names?
    #
    # We do this one by collecting all the bases that contain elements with compounds and
    # then using an external script to do the heavy lifting
    foreach my $p (sort keys %vbases) {
	next if $p =~ /\#/;
	if ($p =~ /\|/) {
	    if ($p =~ tr/|/|/ % 2) {
		pp_warn("(bases) odd number of pipes in compound");
	    } else {
		# warn "cpd_add $p\n";
		cpd_add($p) unless $p =~ /X/;
	    }
	    # warn "#4: $p\n";
	    # while ($p =~ s/^.*?(\|[^|]+\|)//) {
	    # 	my $c = $1;
	    # 	warn "c10e: $c\n";
	    # 	my $shouldbe = ORACC::SL::BaseC::c10e_compound($c);
	    # 	pp_sl_messages();
	    # }
	}
    }
    
    if ($trace && exists $arg_vfields{'bases'}) {
	pp_trace "v_bases: dump of \%ORACC::CBD::bases:";
	pp_trace Dumper \%ORACC::CBD::bases;
    }

    unless ($ORACC::CBD::Forms::external) {
	# get bases registered now so they are available for @form processing
	foreach my $b (keys %bases) {
	    if ($b =~ /^#/) {
		${$ORACC::CBD::bases{$curr_cfgw}}{$b} = $bases{$b};
	    } else {
		++${$ORACC::CBD::bases{$curr_cfgw}}{$b}
		unless ${$ORACC::CBD::bases{$curr_cfgw}}{$b};
	    }
	}
    }
}

sub xflang {
    my %xl = (s=>'sux',e=>'sux-x-emesal',u=>'sux-x-udganu');
    my $l = shift;
    if ($l && $xl{$l}) {
	return $xl{$l};
    }
    $l;
}

sub pp_sl_messages {
    my $p = shift || '';
    my @m = ORACC::SL::BaseC::messages();
#    warn "pp_sl_messages p=$p; m = @m\n";
    if ($#m >= 0) {
	foreach my $m (@m) {
	    if ($p eq 'Q') {
		next unless $m =~ /\[Q/;
	    }
	    if ($p =~ /^\|(.*?)\|$/) {
		my $novb = $1;
#		warn "m=$m; novb=$novb\n";
		next if $m =~ $novb;
	    }
	    next if $p =~ /X/ && $m =~ /X/;
#	    warn join(':', caller()), "\n";
#	    $ORACC::CBD::PPWarn::trace = 1;
	    pp_trace("pp_sl_messages adding message `$m' with pp_line()==",pp_line());
	    pp_warn("(bases) ".$m);
#	    $ORACC::CBD::PPWarn::trace = 0;
	}
	1
    } else {
	0
    }
}

sub v_form {
    my($tag,$arg) = @_;

    $arg = '' unless $arg;
    my $f = $arg;
    my $flang = '';
    
    if ($trace) {
	pp_trace "v_form: tag=$tag; arg='$arg'; lang=$lang";
    }
    
    unless ($arg) {
	pp_warn("empty \@form");
	return;
    }

    if ($arg =~ /^[\%\$\#\@\+\/\*]\S/) {
	unless ($arg =~ /^\$[A-ZḪŊṢŠṬʾ]/) {
	    pp_warn("\@form must begin with writing of form (arg=$arg)");
	    return;
	}
    }

    if ($ORACC::CBD::Forms::external) {
	&ORACC::CBD::Forms::forms_register_inline(pp_file(), pp_line(), $curr_cfgw, $arg);
	return;
    }

    if ($f =~ s/(?:^|\s+)\%(\S+)//) {
	$flang = $1;
	$f =~ s/^\s*//;
    } elsif ($lang =~ /^qpn/) {
	pp_warn("no %LANG in QPN glossary \@form entry");
    } else {
	$flang = $lang;
    }
    
    my $barecheck = $arg;
    $barecheck =~ s/^(\S+)\s*//;
    my $formform = $1;

    my $tmpform = $formform; $tmpform =~ tr/_/ /;
    atf_add($tmpform,$flang) if $tmpform;

    if ($formform =~ tr/°·//d) {
	$tmpform =~ tr/ /_/;
	pp_warn("FORM $tmpform may not contain degree (°) or centre-dot (·)");
    }
    
    if ($formform =~ /[áéíúàèìùÁÉÍÚÀÈÌÙ]/) {
	pp_warn("accented vowels not allowed in \@form");
    }

    if ($formform =~ /[<>]/) {
	pp_warn("angle brackets not allowed in \@form");
    }

    my($fo) = ($f =~ /^(\S+)/); # warn "fo=$fo; flang=$flang\n";
    if (!$ORACC::CBD::Forms::external
	&& $ORACC::CBD::forms{$curr_cfgw,$fo,$flang}++) {
	# can't do this for all langs: it's legit to have al-pi $alpi and al-pi $alpī
	pp_warn("duplicate form in `$curr_cfgw': $fo")
	    if $flang =~ /^sux/;
	# return;
    }

    if ($fo =~ tr/_/ / && !$is_compound) {
	pp_warn("underscore (_) not allowed in form except in compounds");
    }
    
    if (($lang =~ /^akk/ 
	 || ($lang =~ /^qpn/ && $flang =~ /akk/))) {
	pp_warn("no normalization in form")
	    unless $f =~ m#(?:^|\s)\$\S#;
    }

    if (($ORACC::CBD::bases # $lang =~ /^sux/ 
	 || ($lang =~ /^qpn/ && $flang =~ /^$ORACC::CBD::qpn_base_lang/))) {
	if ($f =~ m#(?:^|\s)/(\S+)#) {
	    my $b = $1;
	    my $blang; # ignored when validating
	    if ($b =~ s/^\%(\S+?)://) {
		$blang = $1;
	    }
	    if ($b) {
		if ($always_check_base) {
		    ORACC::SL::BaseC::check('',$b);
		    pp_sl_messages('Q');
		}
		if ($is_compound) {
		    pp_warn("/BASE not allowed in \@form belonging to compound word (b=$b)");
		} else {
		    if (!$bases{$b}) { ###  || $ORACC::SL::report_all
			if (!${$ORACC::CBD::bases{$curr_cfgw}}{$b}) { ### || $ORACC::SL::report_all) {
			    my $warned = 0;
			    my $a = $bases{"#$b"} || ${$ORACC::CBD::bases{$curr_cfgw}}{"#$b"};
			    # warn "alt for $b == $a\n";
			    if ($a) {
				$a =~ s/^\#//;
				pp_warn("alt BASE $b should be primary $a");
				$warned = 1;
			    } else {
				# slow but effective check for base match by tlit signature
				atf_add($b,$lang) if $b;
				my $tsig = $tlit_sigs{$b};
				$tsig = $tlit_sigs{$b} = ORACC::SL::BaseC::tlit_sig('',$b)
				    unless $tsig;
				# warn "tsig for $b == $tsig\n";
				# my $nkeys = scalar keys %{$ORACC::CBD::bases{$curr_cfgw}};
				# warn "curr_cfgw == $curr_cfgw; nkeys = $nkeys\n";
				foreach my $c (keys %{$ORACC::CBD::bases{$curr_cfgw}}) {
				    $c =~ s/^\%.*?://;
				    my $csig = $tlit_sigs{$c};
				    $csig = $tlit_sigs{$c} = ORACC::SL::BaseC::tlit_sig('',$c)
					unless $csig;
				    # warn "csig for $c == $csig\n";
				    if ($tsig eq $csig && $b ne $c) {
					$c =~ s/^\#//;
					pp_warn "form's BASE $b should be $c";
					$warned = 1;
					last;
				    }
				}
				pp_sl_messages();
			    }
			    pp_warn("BASE $b not known or findable for `$curr_cfgw'")
				unless $bases{$b} || $warned;
			}
		    }
		}
	    }
	} else {
	    pp_warn("no BASE entry in form") unless $is_compound;
	}
    }

    if ($f =~ /\s\+(\S+)/) {
	my $c = $1;
	pp_warn("malformed CONT '$c'")
	    unless $c =~ /^-(.*?)=(.*?)$/;
    }

    my $morph = '';
    if ($f =~ /\s\#([^\#]\S*)/) {
	$morph = $1;
	$morph =~ tr/*/✻/; #map * to U+273B TEARDROP-SPOKED ASTERISK
	v_morph($formform, $morph);
	# hook into ORACC::SMA::MorphData::mdata_validate() when epsd2 is cleaned
    } elsif (($ORACC::CBD::bases # || $lang =~ /^sux/ 
	      || ($lang =~ /^qpn/ && $flang =~ /^$ORACC::CBD::qpn_base_lang/))
	     && !$is_compound
	) {
	pp_warn("no MORPH in form");
    }
    
    if ($f =~ /\s\#\#(\S+)/) {
	++$seen_morph2;
	my $morph2 = $1;
	pp_warn("morph2 `$morph2' has no morph1")
	    unless $morph;
    } elsif ($morph && $seen_morph2) {
	if ($f =~ s/\s\#//g > 1) {
	    pp_warn("repeated `$morph' field (missing '#' on morph2?)");
	} else {
	    pp_warn("morph has no morph2")
		unless $mixed_morph;
	}
    }

    if ($f =~ /\s\*(\S+)/) {
	&$stem_validator($1);
    }

    1 while $barecheck =~ s#(^|\s)[\%\$\#\@\+\/\*!]\S+#$1#g;

    if ($barecheck =~ /\S/) {
	pp_warn("bare word in \@form. barecheck=$barecheck; arg=$arg");
    } else {
	my $tmp = $arg;
	$tmp =~ s#\s/(\S+)##; # remove BASE because it may contain '$'s.
	$tmp =~ s/^\S+\s+//; # remove FORM because it may contain '$'s.
	my $ndoll = 0;
	if (($ndoll = ($tmp =~ tr/$/$/)) > 1 
	    && !$is_compound) {
	    my $nparen = ($tmp =~ s/\$\(//g);
	    pp_trace "v_form COF: ndoll=$ndoll; nparen=$nparen";
	    if ($ndoll - $nparen > 1) {
		pp_warn("COFs must have only one NORM without parens (found more than 1)");
	    } elsif ($ndoll == $nparen) {
		pp_warn("COFs must have one NORM without parens (found none)");
	    }
	}
    }
}

sub v_morph {
    return;
    my ($f, $m) = @_;
    if ($m !~ /~/ && $m ne 'X') {
	pp_warn("no ~ in MORPH $f = $m");
    } elsif ($m =~ /[:;,.!]$/) {
	pp_warn("MORPH $m ends with punct");
    } elsif (($m =~ tr/:/:/ > 1)
	     || ($m =~ tr/;/;/ > 1)
	     || ($m =~ tr/,/,/ > 1)
	     || ($m =~ tr/!/!/ > 1)) {
	pp_warn("duplicate segment delimiter (one of :;.!) in $m");
    }
}

sub v_parts {
    my($tag,$arg) = @_;
    if ($arg =~ /\](\s|$)/) {
	pp_warn("parts component has no POS after ']'");
    }
    $arg =~ s/\s+n\s+/ n\cA/g;
    $arg =~ s/(\]\S*)\s+/$1\cA/g;
    my @p = split(/\cA/,$arg);
    foreach my $p (@p) {
	my $cf = $p;
	$cf =~ s/\[.*$//;
	if ($cf =~ tr/'/'/) {
	    pp_warn("quote mark ' in citation form should be raised right half circle, ʾ");
	}
	push @parts, [ $curr_cfgw, $p, pp_file(), pp_line() ];
    }
    $_[0];
}

# We process this for each of the parts recorded in v_parts but after
# all the entries have been read
sub v_part {
    my $pref = shift;
    my($cmpd,$part,$file,$line) = @$pref;
    my $cfgwpos = $part;
    $cfgwpos =~ s#//.*?\]#]#;
    $cfgwpos =~ s/'\S+//;
    $cfgwpos =~ s/\s*(\[.*?\])\s*/ $1 /;

#    warn "cfgwpos=$cfgwpos\n";
    
    return if $entries{$cfgwpos} || $cfgwpos =~ /^n \[/;
    
    my $fsave = pp_file();
    my $lsave = pp_line();
    pp_file($file);
    pp_line($line);
    my $tmp = $cfgwpos;
    $tmp =~ s/\].*$/]/;
    if ($entries_nopos{$tmp}) {
	my $b = best_list_string(@{$entries_nopos{$tmp}});
	$part =~ s/<.*?>$//;
	pp_warn("part $part better $b") unless $part eq $b;
	pp_file($fsave);
	pp_line($lsave);
	return;
    }
#    warn "tmp = $tmp\n";
    my($cf,$gw) = ($tmp =~ /^(.*?) \[(.*?)\]$/);
    my @best_list = ();
    if (defined($gw)) {
	$gw =~ tr/a-zA-Z0-9 \t//cd;
	foreach my $m (split(/\s+/, $gw)) {
	    if ($entries_cfmng{"$cf$m"}) {
		my @l = @{$entries_cfmng{"$cf$m"}};
		#	    warn "found entries_cfmng for $cf$m = @l\n";
		if ($#best_list < 0) {
		    @best_list = @l;
		    last if $#best_list == 0;
		} else {
		    if ($#l < $#best_list) {
			@best_list = @l;
			last if $#best_list == 0;
		    }
		}
	    }
	}
    } else {
	if ($tmp eq 'n') {
	    @best_list = ('n');
	} else {
	    warn "$file:$line: no [] in part $tmp of $cmpd\n" unless $gw;
	}
    }
    
    if ($#best_list == 0) {
	my $b = best_list_string(@best_list);
	pp_warn("part $part better $b") unless $part eq $b;
	pp_file($fsave);
	pp_line($lsave);
	return;
    } elsif ($#best_list > 0) {
	my $b = best_list_string(@best_list);
	pp_warn("part $part better $b") unless $part eq $b;
	pp_file($fsave);
	pp_line($lsave);
	return;	
    }
    
    # This message duplicates XXX does not match a known CF[GW] in XXX.glo
    #    pp_warn("$cmpd: part $part is not an \@entry in this glossary");
    
    pp_file($fsave);
    pp_line($lsave);
}

sub best_list_string {
    my $b = $_[0];
    $b =~ s/\s+\[/[/; $b =~ s/\]\s+/]/;
    $b;
}

sub v_sense {
    my($tag,$arg) = @_;

    ++$seen_sense;
    
    if ($arg =~ s/^\[(.*?)\]\s+//) {
#	$sgw = $1;
    }

    my($pre,$etag,$pst) = ($tag =~ /^($acd_rx)?\@(\S+?)(\!?)$/);

    if ($etag =~ s/\+$//) {
	pp_warn("please review \@sense+ before proceeding");
    }
    
    if ($pre) {
	if ($pre eq '>' && $global_cbd[pp_line()-2] =~ /^$acd_rx/) {
	    pp_warn("edit command '>' can't follow another edit command");
	    pp_warn('prev='.$global_cbd[pp_line()-2]);
	    pp_warn('curr='.$global_cbd[pp_line()-1]);
	} else {
	    # push @{$data{'edit'}}, pp_line()-1;
	}
    }

    my $stem = undef;
    if ($arg =~ s/\s\*(\S+)//) {
	$stem = $1;
	&$stem_validator($stem);
    }
    
    my($pos,$mng) = ();
    if ($arg =~ /^[A-Z]+(?:\/[it])?\s/) {
	($pos,$mng) = ($arg =~ /^([A-Z]+(?:\/[it])?)\s+(.*)\s*$/);
    } else {
	$mng = $arg;
	$mng =~ s/^\s*(.*?)\s*$/$1/;
    }
    if ($pos) {
	if (!exists $poss{$pos} && $pos !~ /^V\/[ti]/) {
	    if ($pos =~ /^[A-Z]+$/) {
		pp_warn("$pos not in known POS list");
	    } else {
		$mng = "$pos $mng";
	    }
	}
    } else {
	$pos = '';
    }
    if (!defined($mng)) { # allow mng="0"
	pp_warn("no content in SENSE");
	$mng = '';
    }

    if ($arg =~ tr/"//d) {
	pp_warn("double quotes not allowed in SENSE; use Unicode quotes instead");
    }
    if ($arg =~ tr/[]//d) {
	pp_warn("square brackets not allowed in SENSE; use Unicode U+27E6/U+27E7 instead");
    }
    if ($arg =~ tr/;//d) {
	pp_warn("semi-colons not allowed in SENSE; use comma or split into multiple SENSEs");
    }
    my($tok1) = ($arg =~ /^(\S+)/);
    if (!$tok1) {
	pp_warn("empty SENSE");
    } else {
	pp_warn("$tok1: unknown POS in SENSE") unless exists $poss{$tok1};
	pp_warn("no content in SENSE") unless $arg =~ /\s\S/;
    }

    $arg =~ s/^\S+\s+//;
    $arg =~ tr/a-zA-Z0-9 \t//cd;
    foreach my $m (split(/\s+/, $arg)) {
	push @{$entries_cfmng{"$curr_cf$m"}}, $curr_cfgw;
    }

    $_[0];
}

sub v_bff {
    my($tag,$arg) = @_;
    my($class,$code,$label,$link,$target) = ();
    $arg =~ s/\s*$//;
    if ($arg =~ /^["<]/) {
	pp_warn("missing CLASS in \@bff");
	push @bffs, {
	    curr_id=>pp_line()-1,
	    line=>$.,
	    link=>''
	};
    } else {
	($arg =~ s/^(\S+)\s*//) && ($class = $1);
#	unless ($bff_class{$class}) {
#	    pp_warn( "unknown bff CLASS: $class\n");
#	}
	if ($arg !~ /^["<]/) {
	    ($arg =~ s/^(\S+)\s*//) && ($code = $1);
	}
	if ($arg =~ /^"/) {
	    ($arg =~ s/^"(.*?)\"\s+//) && ($label = $1);
	}
	if ($arg =~ /<[^>]*$/) {
	    pp_warn("missing close '>' on bff link");
	    return;
	}
	if ($arg =~ /^[^<]*$/) {
	    pp_warn("missing open '<' on bff link");
	    return;
	}
	($arg =~ s/\s*<(.*?)>\s*$//) && ($link = $1);
	if ($arg) {
	    #	    pp_warn("bff is CLASS CODE \"LABEL\" <LINK> where CODE and \"LABEL\" are optional");
	    pp_warn("bff leftovers=$arg (out of order components?)");
	}
	push @bffs, {
	    bid=>$bid++,
	    class=>$class,
	    code=>$code,
	    label=>$label,
	    link=>$link,
	    line=>pp_line(),
	    ref=>pp_line()-1,
	};
    }
}

sub v_stems {
    my($tag,$arg) = @_;
    if ($lang =~ /^akk/) {
	# possibly do nothing
    } elsif ($lang =~ /^sux/) {
	# this is actually used a few times in epsd2; need rules for it
    } else {
	pp_warn("STEM not yet implemented for lang=$lang");
    }
}

sub v_bib {
    my($tag,$arg) = @_;
}

sub v_isslp {
    my($tag,$arg) = @_;
    pp_warn("\@isslp must start with year or 'nd'")
	unless $arg =~ /^\d\d\d\d\S*\s/ || $arg =~ /^nd\s/;
}

sub v_equiv {
    my($tag,$arg) = @_;
}

sub v_file {
    my($tag,$arg) = @_;
    if (-r $arg) {
	pp_notice("FILE $arg is present and readable");
    } else {
	pp_warn("FILE $arg not found");
    }
}

sub v_inote {
    my($tag,$arg) = @_;
}

sub v_note {
    my($tag,$arg) = @_;
}

sub v_root {
    my($tag,$arg) = @_;
}

sub v_norms {
    my($tag,$arg) = @_;
}

sub v_conts {
    my($tag,$arg) = @_;
}

sub v_prefs {
    my($tag,$arg) = @_;
}

sub v_allow {
    my($tag,$arg) = @_;
}

sub v_collo {
    my($tag,$arg) = @_;
}

sub v_geos {
    my($tag,$arg) = @_;
}

sub v_usage {
    my($tag,$arg) = @_;
}

sub v_end {
    my($tag,$arg) = @_;
    pp_warn("malformed \@end entry")
	unless $arg =~ /^\s*entry\s*$/;
    pp_warn("no SENSE in \@entry") unless $seen_sense;
    if ($ORACC::CBD::Forms::external) {
	foreach my $b (keys %bases) {
	    if ($b =~ /^#/) {
		${$ORACC::CBD::bases{$curr_cfgw}}{$b} = $bases{$b};
	    } else {
		++${$ORACC::CBD::bases{$curr_cfgw}}{$b}
		unless ${$ORACC::CBD::bases{$curr_cfgw}}{$b};
	    }
	}
    }
    $curr_cfgw = '';
    $in_entry = $seen_bases = $seen_morph2 = $seen_sense = 0;
    %allow = ();
    %bases = ();
}

sub v_deprecated {
    pp_warn("$_[0] is deprecated, please remove from glossary");
}

sub is_proper {
    $_[0] && $_[0] =~ /^[A-Z]N$/;
}

sub v_proplist {
    my($tag,$arg) = @_;
    push @{$data{'proplist'}}, pp_line()-1;
    proplist($arg);
}

sub v_prop {
    my($tag,$arg) = @_;
    prop($arg);
}

sub v_alias {
    my($tag,$arg) = @_;
}

sub v_pl_id {
    my($tag,$arg) = @_;
}

sub v_pl_uid {
    my($tag,$arg) = @_;
}

sub v_pl_coord {
    my($tag,$arg) = @_;
}

sub v_length {
    my($tag,$arg) = @_;
}

sub v_is_entry {
    $seen_entries{$_[0]};
}
sub v_ok {
    pp_warn("\@ok is deprecated; use \#\@ok instead");
}
sub v_set_cfgw {
    $curr_cfgw = $_[0];
    my($cf) = ($curr_cfgw =~ /^(.*?)\s*\[/);
    $is_compound = ($cf =~ /\s/);
}

sub bff_check {
    my %bffs = ();
    foreach my $bff (@bffs) {
	my %bff = %$bff;
	pp_line($bff{'line'}+1);
	if ($bff{'link'}) {
	    if ($entries{$bff{'link'}}) {
		if ($entries{$bff{'link'}}) { # SOMETHING WRONG HERE: IS THIS SUPPOSED TO TEST A DIFFERENT MEMBER OF %bff?
		    $bff{'target'} = $entries{$bff{'link'}};
		    my $target_id =  $entries{$bff{'target'}};
		    if ($target_id) {
			$bffs{$bff{'bid'}} = $bff;
			push @{$bffs{$target_id}}, $bff{'bid'};
			push @{$entries{$bff{'ref'},'bffs-listed'}}, $bff{'bid'};
			push @{$entries{$bff{'target'},'bffs'}}, { %bff };
		    } else {
			pp_warn("bff target <$bff{'target'}> has no ID");
		    }
		} else {
		    pp_warn("bff link <$bff{'link'}> not known as entry");
		}
	    } else {
		pp_warn("unresolved bff link <$bff{'link'}>");
	    }
	} else {
	    pp_warn("no <link> found in bff");
	}
    }
    %bffs;
}

sub register_base_sig {
    my($base,$tsig) = @_;
    push @{$basedata{$tsig}}, [ $curr_id , $base ];
    push @{$basesigs{$curr_cfgw}}, [ $base , $tsig ];
}

sub has_parts {
    my($i,$cbdref) = @_;
    while ($i < $#$cbdref) {
	if ($$cbdref[$i] =~ /^\@end\s+entry/) {
	    return 0;
	} elsif ($$cbdref[$i] =~ /^\@parts/) {
	    return 1;
	} else {
	    ++$i;
	}
    }
    return 0;
}

sub is_allowed {
    my($p1,$p2) = @_;
#    warn "is_allowed($p1,$p2)\n";
#    warn Dumper \%allow;
#    my $ret = 
#    warn "is_allowed == $ret\n";
    return ${$allow{$p1}}{$p2} if defined($allow{$p1});
    return 0;
}

sub is_bad_compound {
#    print Dumper \%bad_compounds;
    $bad_compounds{$_[0]};
}

my %akk_stem = (); 
my @akk_stem = qw/B G Bt Gt Btn Gtn D ŠD Dt Dtn Š Št Štn N Nt Ntn/;
@akk_stem{@akk_stem} = ();

sub v_stem_akk {
    my $stems = shift;
    foreach my $s (split(/,/,$stems)) {
	pp_warn("unknown stem $s") unless exists $akk_stem{$s};
    }
}

sub v_stem_sux {
}

sub v_stem_bad {
    pp_warn("no stem validator for lang=$lang");
}

sub v_oid {
    my($tag,$arg) = @_;
    if ($arg !~ /^[ox]\d+$/) {
	pp_warn("bad OID $arg");
    }    
}

sub det_check {
    my $b = shift;
    my $orig_b = $b;
    my $b2 = $b;
    my $fixes = 0;
    while ($b2 =~ s/\{(.*?)\}//) {
	my $d = $1;
	my $plus = ($d =~ s/^\+//);
	if ($plus) {
	    # pp_warn("det in base $b :: $d");
	} else {
	    # {-ga} is a @bases only convention to mark semantic dets
	    unless (exists $known_det{$d} || $d =~ s/^-//) {
		$fixes += $b =~ s/\{$d\}/{+$d}/;
	    }
	}
    }
    pp_warn("(bases) phonetically determined BASE $orig_b should be $b") if $fixes;
}

1;
