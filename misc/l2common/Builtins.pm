package ORACC::L2GLO::Builtins;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw//;

use warnings; use strict; use open 'utf8'; use utf8;
use lib '@@ORACC@@/lib';
use ORACC::XML;
use ORACC::OID;
use ORACC::NS;
use ORACC::XMD::Pleiades;
use Data::Dumper;

binmode STDIN, ':utf8';
binmode STDOUT, ':utf8';
binmode STDERR, ':utf8';

$ORACC::L2GLO::Builtins::accents = 0;
$ORACC::L2GLO::Builtins::bare = 0;
$ORACC::L2GLO::Builtins::debug = 0;
$ORACC::L2GLO::Builtins::noletters = 0;
$ORACC::L2GLO::Builtins::O2_cancel = 1;
$ORACC::L2GLO::Builtins::t0 = 0;

my $cbd_project = '';
my $pleiades_initialized = 0;
my @pleiades_data = ();
my %pleiades = ();

my %sense_props = ();

my $cgctmp = '';
my $early_debug = 1;

my %rws_map = (
    EG => 'sux',
    CF => 'akk',
    OA => 'akk-x-oldass',
    OB => 'akk-x-oldbab',
    MA => 'akk-x-midass',
    MB => 'akk-x-midbab',
    NA => 'akk-x-neoass',
    NB => 'akk-x-neobab',
    SB => 'akk-x-stdbab',
    );

my %acc = (
	   'á'=>['a','₂'],
	   'é'=>['e','₂'],
	   'í'=>['i','₂'],
	   'ú'=>['u','₂'],
	   'à'=>['a','₃'],
	   'è'=>['e','₃'],
	   'ì'=>['i','₃'],
	   'ù'=>['u','₃'],
	   'Á'=>['A','₂'],
	   'É'=>['E','₂'],
	   'Í'=>['I','₂'],
	   'Ú'=>['U','₂'],
	   'À'=>['A','₃'],
	   'È'=>['E','₃'],
	   'Ì'=>['I','₃'],
	   'Ù'=>['U','₃'],
	   );

my %recip_acc = ();
foreach my $k (keys %acc) {
    $recip_acc{${$acc{$k}}[0],${$acc{$k}}[1]} = $k;
}

my %errlist = ();

my %merge_header = ();

my %sigs = ();
my %signatures = ();
my $sense_id = 0;

my $bad_action;
my $bad_input;
my $cbdid;
my $cbdlang;
my %cgc = ();
my $curr_cf = '';
my $curr_sense_id = 0;
my $curr_sig_id = '';
my $bid = 'b000001';
my $eid = 'x000001';
my $e_sig = '';
my $sid = 0;
my %formattr = ();
my $gdtag = 'gw';
my %header = ();
my $is_compound = 0;
my $last_char = undef;
my $mnglang = 'en';
my $oid = '';
my %seen = ();
my $seen_morph2 = '';
my $status = 0;
my %xids = ();
my $last_xid;
my $usage_flag = 0;
my $ebang_flag = 0;

my @funcs = qw/free impf perf Pl PlObj PlSubj Sg SgObj SgSubj/;
my %funcs = (); @funcs{@funcs} = ();

my @poss = qw/AJ AV N V DP IP PP CNJ J MA O QP RP DET PRP POS PRT PSP SBJ NP M MOD REL XP NU
	AN BN CN DN EN FN GN HN IN JN KN LN MN NN ON PN QN PNF RN SN TN U UN VN WN X XN YN ZN/;
push @poss, ('V/t', 'V/i');
my %poss = (); @poss{@poss} = ();

my @stems = qw/B rr RR rR Rr rrr RRR rrrr RRRR S₁ S₂ S₃ S₄/;
my %stems = (); @stems{@stems} = ();

my @tags = qw/entry oid alias parts allow bases bff collo conts morphs morph2s moved phon prefs root 
	      form length norms sense stems equiv inote prop end isslp bib was
	      defn note pl_coord pl_id pl_uid/;
my %tags = (); @tags{@tags} = ();

my %fseq = ();
foreach my $f (@tags) {
    $fseq{$f} = 0 + scalar keys %fseq;
}

my %header_fields = ();
my @header_fields = qw/project name lang/;
@header_fields{@header_fields} = ();

my %langnorms = (
    akk=>1,
    sux=>0,
    );
my $use_norms;

my %vowel_of = (
    'Ā'=>'A',
    'Ē'=>'E',
    'Ī'=>'I',
    'Ū'=>'U',
    'Â'=>'A',
    'Ê'=>'E',
    'Î'=>'I',
    'Û'=>'U',
    );

my %parsers = (
    entry=>\&parse_entry,
    parts=>\&parse_parts,
    bases=>\&parse_bases,
    conts=>\&parse_conts,
    collo=>\&parse_collo,
    prefs=>\&parse_prefs,
    root =>\&parse_root,
    form =>\&parse_form,
    norms=>\&parse_norms,
    sense=>\&parse_sense,
    stems=>\&parse_stems,
    equiv=>\&parse_equiv,
    inote=>\&parse_inote,
    );

my %line_of = ();
my $curr_line = 1;
my $curr_file = undef;
my $field_index = 0;
my $bstar = '';

my $POSrx = '[A-Za-z0-9]+(?:\/[a-z])?';

my $lang = undef;

my @glo_atf = ();
my $bases_atf = '';

my %entries = ();

# This stores a list of all the bffs that refer to the
# keys; keys are entry ids
my %bffs_index = ();

# This maps the CF [GW] POS in the @entry line to and ID

my $curr_id = '';
my %entries_index = ();

my $blang = '';

sub
acd2xml {
    my($input,$n,$arglang,$title) = @_;
    my $project = undef;
    my $last_tag = '';

    %seen = ();

    $lang = $arglang;

    if (($input =~ /sux/ || $input =~ /qpn/) && (-r "$input.norm")) {
	$input = "$input.norm";
    }

    # index entries quickly to validate xrefs in @bff etc.
    open(IN,$input) || die "acd2xml: unable to open glossary $input\n";
    my @bffs = ();
    while (<IN>) {
	if (s/^[+->]?\@entry[*!]*\s+//) {
	    chomp;
	    s/\s*$//;
	    my $cfgw = $_;
	    my $oid_ln = <IN>; chomp $oid_ln;
	    if ($oid_ln =~ /^\@oid\s+(\S+)\s*/) {
		$curr_id = $entries_index{$cfgw} = $1;
	    } else {
		$curr_id = $entries_index{$cfgw} = $eid++;
	    }
	    $entries_index{$curr_id} = $cfgw;
	} elsif (s/^\@bff\s+//) {
	    my $b = parse_bff($curr_id, $_);
	    push @bffs, $b;
	}
    }
    close(IN);

    foreach my $bff (@bffs) {
	my %bff = %$bff;
	if ($bff{'link'}) {
	    if ($entries_index{$bff{'link'}}) {
		if ($entries_index{$bff{'link'}}) {
		    $bff{'target'} = $entries_index{$bff{'link'}};
		    my $target_id = $entries_index{$bff{'target'}};
		    if ($target_id) {
			$bffs_index{$bff{'bid'}} = $bff;
			push @{$bffs_index{$target_id}}, $bff{'bid'};
			push @{$entries_index{$bff{'ref'},'bffs-listed'}}, $bff{'bid'};
			push @{$entries_index{$bff{'target'},'bffs'}}, { %bff };
		    } else {
			warn("$input:$bff{'line'}: bff target <$bff{'target'}> has no ID\n");
		    }
		} else {
		    warn("$input:$bff{'line'}: bff link <$bff{'link'}> not known as entry\n");
		}
	    } else {
		warn("$input:$bff{'line'}: unresolved bff link <$bff{'link'}>\n");
	    }
	} else {
	    warn("$input:$bff{'line'}: no <link> found in bff");
	}
    }

    open(IN,$input);

    while (<IN>) {
	if (/^\@(project|name|lang)\s+(.*?)\s*$/) {
	    $header{$1} = $2;
	} else {
	    if (/^\#/ || /^\s*$/) {
		last;
	    } else {
		die "$input: first line after header must be blank or comment\n";
	    }
	}
    }
    foreach my $h (qw/project name lang/) {
	if (!$header{$h}) {
	    $input =~ s/\.norm$//;
	    warn "$input: must give project/name/lang in header\n";
	    exit 1;
	}
    }
    ($project, $n, $title, $lang) = (@header{qw/project name name lang/});
    $cbd_project = $project;
    $n =~ tr#/#_#;
    $n =~ s/_[^_]+$//;
    my @xml = (xmldecl(),"<entries xmlns=\"http://oracc.org/ns/cbd/1.0\" xmlns:cbd=\"http://oracc.org/ns/cbd/1.0\" xmlns:g=\"http://oracc.org/ns/gdl/1.0\" xmlns:n=\"http://oracc.org/ns/norm/1.0\" xml:lang=\"$lang\" g:file=\"$lang.glo\" project=\"$project\" n=\"$n\" name=\"$title\">");
    $status = 0;
    $bad_action = 'acd2xml';
    $bad_input = $input; $bad_input =~ s/\.norm$//;
    $cbdlang = $lang if $lang;
    $blang = $cbdlang; $blang =~ s/-.*//;
    $cbdid = $cbdlang;
    $cbdid =~ tr/-/_/;
    my $currtag = undef;
    my $currarg = undef;
    $curr_file = $input;
    $curr_line = 1;
    $line_of{'#init'} = 1;
    my %e = ();
    $use_norms = $langnorms{$lang};
#    open(L,">dumper.log");
#    load_sigs($input);
    while (<IN>) {
	++$curr_line;
	next if /^\#/ || /^\@letter/;
	chomp;
	s/\s+/ /g;
	if (/^[+->]?\@([a-z_]+[-*!]*)\s+(.*?)\s*$/) {
	    ($currtag,$currarg) = ($1,$2);
	    my $defn_minus = 0;
	    my $default = $currtag =~ s/!//;
	    my $starred = $currtag =~ s/\*//;
	    my $linetag = $currtag;
	    $linetag =~ s/\*$//;
	    next if exists $header_fields{$currtag}; # ignore header for now

	    if ($currtag eq 'oid') {
		## handled in indexing phase
		# $curr_id = $cbdid
		# $curr_id = $oid = $currarg;
		next;
	    }
	    
	    if ($last_tag eq $currtag) {
		++$field_index;
	    } else {
		$field_index = 0;
		$last_tag = $currtag;
	    }

	    $line_of{$linetag} = $.
		unless defined $line_of{$linetag};
	    if ($currtag =~ /^entry/) {
		$ebang_flag = $default || '';
		$usage_flag = $starred;
		$currarg =~ /^(\S+)/;
		$curr_cf = $1;
		$currarg =~ s/^\s+//; $currarg =~ s/\s*$//;
		$curr_id = $entries_index{$currarg};
		unless ($curr_id) {
		    bad($currtag, "weird; no entries_index entry for `$currarg' (this can't happen)\n");
		    $curr_id = '';
		}
	    } elsif ($currtag =~ /^defn/) {
		$defn_minus = ($currtag =~ s/-$//);
	    }
	    if (!exists($tags{$currtag})) {
		bad(undef,"unknown tag '$currtag'");
	    } elsif ($currtag eq 'end') {
		if ($currarg eq 'entry') {
		    push @xml, acdentry(%e);
		    %e = ();
		    $bstar = '';
		} else {
		    bad($currtag,"malformed end tag: \@end $currarg");
		}
		%line_of = ();
		$line_of{'#init'} = $.; # $curr_line;
	    } else {
		if ($currtag eq 'bases' && $e{'bases'}) {
		    bad('bases',"`bases' can only be given once");
		} else {
		    if ($currtag eq 'sense') {
			if ($currarg =~ tr/"//d) {
			    bad($currtag, "double quotes not allowed in SENSE; use Unicode quotes instead");
			}
			if ($currarg =~ tr/[]//d) {
			    bad($currtag, "square brackets not allowed in SENSE; use Unicode U+27E6/U+27E7 instead");
			}
			if ($currarg =~ tr/;//d) {
			    bad($currtag, "semi-colons not allowed in SENSE; use comma or split into multiple SENSEs");
			}
			my($tok1) = ($currarg =~ /^(\S+)/);
			if (!$tok1) {
			    bad($currtag, "empty SENSE");
			} else {
			    bad($currtag, "$tok1: unknown POS in SENSE") unless exists $poss{$tok1};
			    bad($currtag, "no content in SENSE") unless $currarg =~ /\s\S/;
			}
			$curr_sense_id = sprintf("\#%06d",$sense_id++);
			my $defbang = ($default ? '!' : '');
			$currarg = "$curr_sense_id$defbang\t$currarg";
		    } elsif ($currtag eq 'form') {
			my $barecheck = $currarg;
			$barecheck =~ s/^(\S+)\s*//;
			my $formform = $1;
			if ($formform =~ /[áéíúàèìùÁÉÍÚÀÈÌÙ]/) {
			    bad($currtag, "accented grapheme in FORM");
			}
			if ($formform =~ /[<>]/) {
			    bad($currtag, "angle brackets are not allowed in FORM");
			}
			1 while $barecheck =~ s#(^|\s)[\%\$\#\@\+\/\*!]\S+#$1#g;
			if ($barecheck =~ /\S/) {
			    bad($currtag, "bare word in FORM. barecheck=$barecheck; currarg=$currarg");
			} else {
			    my $tmp = $currarg;
			    $tmp =~ s#\s/(\S+)##; # remove BASE because it may contain '$'s.
			    $tmp =~ s/^\S+\s+//; # remove FORM because it may contain '$'s.
			    my $ndoll = 0;
			    if (($ndoll = ($tmp =~ tr/$/$/)) > 1 
				&& !defined($e{'parts'})) {
				my $nparen = ($tmp =~ s/\$\(//g);
				if ($ndoll - $nparen != 1) {
				    bad($currtag, "COFs must have exactly one NORM without parens");
				} elsif ($ndoll < 2) {
				    bad($currtag, "COFs must have exactly one NORM without parens");
				}
			    }
			    if ($currarg =~ s/^\s*<(.*?)>\s+//) {
				push @{$sigs{$curr_sense_id}}, $1;
			    }
			    if ($default || $ebang_flag) {
				$currarg = "!$currarg";
			    }
			}
			my $atf = "$..\t";
			/\s(\%\S+)/ && ($atf .= "$1 ");
			/\s(\S+)/ && ($atf .= "$1 ");
			m#\s/(\S+)# && ($atf .= $1);
			$atf =~ s/\\\S+//g;
			$atf .= "\n";
			$atf =~ tr/_/ /;
			push @glo_atf, $atf;
		    } elsif ($currtag eq 'bff') {
## we already processed these
#			my $bff = parse_bff($curr_id, $currarg);
#			if ($bff) {
#			    my %bff = %$bff;
#			    ${$entries{$curr_cf}}{'bff'} = { %bff };
#			}
		    }
		    if ($currtag eq 'prop' && $curr_sense_id) {
#			warn "pushing props to sense $curr_sense_id\n";
			push @{$sense_props{$curr_sense_id}}, $currarg;
		    } else {
			push @{$e{$currtag}}, $currarg
			    unless $currtag eq 'inote';
		    }
		}
	    }
	} elsif (/^\@([A-Z]+)\s+(.*?)\s*$/) {
	    ${$e{'rws_cfs'}}{$1} = $2;
	} elsif (/^\s+\S/) {
	    if ($currtag) {
		s/^\s+/ /;
		${$e{$currtag}}[$#{$e{$currtag}}] .= $_;
	    } else {
		bad($currtag,"continuation line before tags");
	    }
	} elsif (/^\s*$/) {
	    # if %e is empty this was multiple blank lines between entries
	    if (scalar %e) {
		# if it's non-empty the @end entry is missing
		bad($currtag, "missing \@end entry");
		push(@xml, acdentry(%e));
		%e = ();
		$bstar = '';
	    }
	} elsif (/^>/) {
	    # ignore edit map commands
	} else {
	    chomp;
	    $line_of{'#init'} = $.; # $curr_line;
	    bad($currtag,"syntax error near '$_'");
	}
    }
    close(IN);
    push(@xml, acdentry(%e)) if $e{'entry'};

    open(ATF,">01tmp/$cbdlang.atf");
    print ATF "\&X999999 = Gloss ATF\n#project: $project\n#atf: use unicode\n#atf: use math\n";
    print ATF "#atf: lang $cbdlang\n" unless $cbdlang eq 'qpn';
    print ATF uniq_by_line(@glo_atf);
    close(ATF);
    system 'ox', '-cQ', '-l', "01tmp/$cbdlang-atf.log", "01tmp/$cbdlang.atf";

    if (open(OX,"01tmp/$cbdlang-atf.log")) {
	while (<OX>) {
	    if (/^(\d+):/) {
		if (non_status_error($_)) {
		    s#^(.*?):\s+#00lib/$cbdlang.glo:$1: (atf warning) #;
		} else {
		    ++$status;
		    s#^(.*?):\s+#00lib/$cbdlang.glo:$1: (atf error) #;
		}
		m/:(.*?):/;
		push @{$errlist{$1}}, $_;
	    }
	}
	close(OX);
    }

    foreach my $e (sort { $a <=> $b } keys %errlist) {
	warn @{$errlist{$e}};
    }

    if ($status) {
	if ($ORACC::L2GLO::Builtins::O2_cancel) {
	    system 'touch', "01bld/cancel";
	    die "$status errors; stop\n" if $status;
	} else {
	    return undef;
	}
    }
    push(@xml,'</entries>');

    if (scalar keys %pleiades > 0) {
	open(P,'>01bld/pleiades.tab');
	foreach my $o (sort keys %pleiades) {
	    my %p = %{$pleiades{$o}};
	    print P "$o\t$p{'cf'}\t$p{'alias'}\t$p{'id'}\t$p{'coord'}\t$p{'uid'}\n";
	}
	close(P);
    }

#    open(X,">/tmp/geonames-$$.xml");
#    print X @xml;
#    close(X);
#    system("@@ORACC@@/bin/gdlme /tmp/$$.xml -o /tmp/$$.out");
#    load_xml("/tmp/$$.out");

#    open(X,">XX.log"); print X @xml; close(X);

#    if ($early_debug && open(R,">01tmp/$cbdlang-raw.xml")) {
#	print R join("\n", @xml);
#	close(R);
#    }
    #    check_defined(@xml);
    my $xload = undef;
    my $str = join('',@xml);
    eval { $xload = load_xml_string($str) };
    if ($@) {
	warn "load_xml_string: failed: $@\n" if $@;
	open(BADLOAD,'>01tmp/bad-load'); print BADLOAD $str; close(BADLOAD);
    }
    $xload;
}

sub
non_status_error {
    /unknown sign-name/ || /cuneify/;
}

sub
parse_bff {
    my ($curr_id, $currarg) = @_;
    my($class,$code,$label,$link,$target) = ();
    $currarg =~ s/\s*$//;
    if ($currarg =~ /^["<]/) {
	bad('bff', "missing CLASS");
	return {
	    curr_id=>$curr_id,
	    line=>$.,
	    link=>''
	};
    } else {
	($currarg =~ s/^(\S+)\s*//) && ($class = $1);
#	unless ($bff_class{$class}) {
#	    bad('bff', "unknown bff CLASS: $class\n");
#	}
	if ($currarg !~ /^["<]/) {
	    ($currarg =~ s/^(\S+)\s*//) && ($code = $1);
	}
	if ($currarg =~ /^"/) {
	    ($currarg =~ s/^"(.*?)\"\s+//) && ($label = $1);
	}
	if ($currarg =~ /^</) {
	    ($currarg =~ s/<(.*?)>\s*$//) && ($link = $1);
	}
	if ($currarg) {
	    bad('bff', "bff is CODE \"LABEL\" <LINK> where CODE and \"LABEL\" are optional");
	}
	return {
	    bid=>$bid++,
	    class=>$class,
	    code=>$code,
	    label=>$label,
	    link=>$link,
	    line=>$.,
	    ref=>$curr_id,
	};
    }
}

sub
uniq_by_line {
    my @l = @_;
    my @new_l = ();
    foreach my $l (@l) {
	my $lang = '';
	$lang = "$1 " if $l =~ s/(\%[a-z]\S+)\s+//;
	my($lnum,$rest) = ($l =~ /^(\S+)\s+(.*)\s*$/);
	my %u = ();
	@u{split(/\s+/,$rest)} = ();
	push @new_l, $lnum . "\t$lang" . join(' ', keys %u), "\n";
    }
    @new_l;
}

sub
check_defined {
    my $near = '';
    my $last_def = '';
    foreach my $x (@_) {
	if (defined $x) {
	    if ($x =~ /xml:id=\"(.*?)\"/) {
		$near = $1;
	    }
	    $last_def = $x;
	} else {
	    warn "undefined elements near $near; last defined node: $last_def\n";
	}
    }
}

sub
acdentry {
    my %e = @_;
    my @ret = ();
    my %bases = ();
    my %xbases = ();
    my %stems = ();
    my %xstems = ();
    my %conts = ();
    my %morphs = ();
    my %morph_m2refs = ();
    my %morph2s = ();
    my %prefs = ();
    my @norms = ();
    my %forms = ();
    my %form_norms = ();
    my %norm_forms = ();
    my %cfgw_forms = ();
    my %lffmap = ();
    my $curr_cf = '';
    my $curr_pos = '';
    my $use_stemrefs = 0;
    my $e;
    my $elapsed;
    my $elstr;
    my @bffs = ();
    my @bffs_listed = ();
    my $bff_index = 0;

    %xids = ();

    if ($e{'entry'}) {
	$sid = 0;
	if ($#{$e{'entry'}} > 0) {
	    bad('entry',"multiple \@entry' fields");
	} else {
	    $e = ${$e{'entry'}}[0];
	    unless ($eid = $entries_index{$e}) {
		bad('entry', "no entries_index for `$e' (this can't happen)");
		return;
	    }
	    if ($entries_index{$eid,'bffs'}) {
		@bffs = @{$entries_index{$eid,'bffs'}};
	    }
	    if ($entries_index{$eid,'bffs-listed'}) {
		@bffs_listed = @{$entries_index{$eid,'bffs-listed'}};
	    }
	    if ($#bffs >= 0 && $#bffs_listed >= 0) {
		bad('entry', "entry with BFFs is also owner of BFFs");
	    }
 	    if ($seen{$e}++) {
		bad('entry',"duplicate cf [gw]: $e");
	    } else {
		my($cf,$gd,$pos) = ($e =~ /^(.*?)\s+\[(.*?)\]\s*([A-Za-z0-9]+(?:\/[a-z])?)?$/);
		$curr_cf = $cf;
		$curr_pos = $pos;
		if ($e =~ /\]\S/) {
		    bad('entry', "missing space after ]");
		}
		if ($cf) {
		    if ($gdtag eq 'gw' && !$gd) {
			bad('entry',"no guideword in entry field");
		    } else {
			my $cacf = '';
			my $usattr = '';
			my $defattr = '';
			$gd = '' unless $gd;
			
			if ($gd =~ tr/"//d) {
			    bad('entry', "double quotes not allowed in GW\n");
			}

			++$form_norms{$cf} if $use_norms && !defined($e{'parts'});
#			if ($ORACC::L2GLO::rws && ${$e{'rws_cfs'}}{$ORACC::L2GLO::rws}) {
#			    $cacf = xmlify($cf);
#			    $cf = xmlify(${$e{'rws_cfs'}}{$ORACC::L2GLO::rws});
#			    $cacf = " cacf=\"$cacf\"";
#			} else {
			    $cf = xmlify($cf);
#			}
			if ($usage_flag) {
			    $usattr = " usage=\"1\"";
			}
			if ($ebang_flag) {
			    $defattr = " default=\"yes\"";
			}
			$cf = '' unless $pos;
			$gd = '' unless $pos;
			$pos = '' unless $pos;
			$e_sig = "$cf\[$gd\]$pos";
			my $oidattr = '';
			if ($blang =~ /^sux/) {
			    my $oid = oid_lookup('sux',$e_sig);
			    if ($oid) {
				$oidattr = " oid=\"$oid\"";
			    }
			}
			# $cbdid.$eid
			push @ret, "<entry xml:id=\"$eid\" n=\"$e_sig\"$oidattr$usattr$defattr>",make_file_pi($curr_file), make_line_pi($line_of{'entry'}), "<cf$cacf>$cf</cf>";
			if ($e{'alias'}) {
			    foreach my $alias (@{$e{'alias'}}) {
				push @ret, "<alias>$alias</alias>";
			    }
			}
			foreach my $dialect (keys %{$e{'rws_cfs'}}) {
			    if ($rws_map{$dialect}) {
				push @ret, "<dcf xml:lang=\"$rws_map{$dialect}\" n=\"$dialect\">${$e{'rws_cfs'}}{$dialect}</dcf>";
			    } else {
				bad('entry', "unknown dialect abbreviation `$dialect'\n");
			    }
			}

			push @ret, "<$gdtag xml:lang=\"$mnglang\">$gd</$gdtag>";
		    }
		} else {
		    bad('entry',"no citation form in entry field");
		}
		if ($pos) {
		    my $testpos = $pos;
		    if ($pos =~ m#/[a-z]#) {
			$testpos =~ s#/[a-z]##;
		    }
		    if (!exists $poss{$testpos}) {
			if ($pos eq 'X') {
			    warn("new item has POS = X\n");
			} else {
			    bad('entry',"$pos not in list of known POS");
			}
		    } else {
			push @ret, "<pos>$pos</pos>";
		    }
		}
	    }
	}
    } else {
	bad('#init',"no 'entry' field");
    }

    if ($e{'root'}) {
	push @ret, '<root>', ${$e{'root'}}[0],'</root>';
    }

    if ($e{'parts'}) {
	foreach my $p (@{$e{'parts'}}) {
	    push @ret, render_parts($p);
	}
    } else {
	setup_list_field(\%e,\%stems,'stems',\%xstems,\&xstem_split);
	setup_list_field(\%e,\%bases,'bases',\%xbases,\&xbase_split);
	setup_list_field(\%e,\%conts,'conts');
	setup_list_field(\%e,\%prefs,'prefs');
	setup_list_field(\%e,\%morphs,'morphs');
	setup_list_field(\%e,\%morph2s,'morph2s');
    }

    if ($#bffs >= 0) {
	push @ret, '<bffs>';
	foreach my $b (@bffs) {
	    push @ret, '<bff';
#	    warn Dumper $b;
	    foreach my $k (keys %$b) {
		next if $k eq 'target' || $k eq 'link';
		if ($$b{$k}) {
		    my $v = xmlify($$b{$k});
		    $k = 'xml:id' if $k eq 'bid';
		    # $cbdid.$v
		    $v = "$v" if $k eq 'ref';
		    push @ret, " $k=\"$v\"";
		}
	    }
	    push @ret, '/>';
	}
	push @ret, '</bffs>';
    }

    if ($#bffs_listed >= 0) {
	push @ret, '<bffs-listed>';
	foreach my $b (@bffs_listed) {
	    push @ret, "<bffl ref=\"$b\"/>";
	}
	push @ret, '</bffs-listed>';
    }

    if ($e{'form'}) {
	# form is different because for the CBD it's partly a source to 
	# harvest bases and norms from, and partly a source of data to validated
	# form/cf/gw in the lemmatized files
#	my %localfids = ();
	my $localfid = '000';
	$field_index = 0;
	my %seen_forms = ();
	foreach my $f (@{$e{'form'}}) {
	    my ($base,$cont,$stem,$morph,$pref,$rws,$morph2) = ();
	    my $flang = '';
	    my $lfid = $localfid++;
	    my $defbang = '';
	    if ($f =~ s/^!//) {
		$defbang = '!';
	    }
	    
	    if ($f =~ s/(?:^|\s+)\%(\S+)//) {
		$flang = $1;
		$f =~ s/^\s*//;
	    }
	    my($fo) = ($f =~ /^(\S+)/);

#	    if ($seen_forms{$fo,$flang}++) {
#		bad('form',"duplicate form: $fo");
#		next;
# 	    }

	    if ($fo =~ tr/_/ / && !$e{'parts'}) {
		bad('form', "underscore (_) not allowed in form except in compounds");
	    }
	    if ($f =~ s/\s+\@(\S+)//) {
		$rws = $1;
	    }

	    if ($f =~ /\s\$(.*?)(?=$|\s+[^\$])/) {
		my $no = $1;
		++$form_norms{$no};
		if (!exists ${$norm_forms{$no}}{$fo}) {
		    ${$norm_forms{$no}}{$fo} = [$flang,$fo,$lfid];
		}
	    } elsif ($f =~ /\s\#(\S+)/) { # default to morph1 if no norm
		my $no = $1;
		++$form_norms{$no};
		if (!exists ${$norm_forms{$no}}{$fo}) {
		    ${$norm_forms{$no}}{$fo} = [$flang,$fo,$lfid];
		}
	    } elsif (($cbdlang =~ /^akk/ 
		      || ($cbdlang =~ /^qpn/ && $flang =~ /akk/))) {
		bad('form',"no normalization in form");
	    }

	    if ($f =~ /\s\/(\S+)/) {
		my $base = $1;
		if (check_base($base, %bases)) {
		    ++$bases{$base=$1};
		}
	    } elsif (($cbdlang =~ /^sux/ 
		      || ($cbdlang =~ /^qpn/ && $flang =~ /sux/))
		     && !$e{'parts'}) {
		bad('form',"no base entry in form");
	    }
	    if ($f =~ /\s\+(\S+)/) {
		my $c = $1;
		if ($c =~ /^-(.*?)=(.*?)$/) {
		    ++$conts{$cont=$1};
		} else {
		    bad('form', "malformed CONT '$c'");
		}
	    }
	    if ($f =~ /\s\*(\S*)/) {
		# do we need ++$stems{$stem=$1} as with other attr?
		$stem = $1;
	    }
	    if ($f =~ /\s\#([^\#]\S*)/) {
		my $m = $1;
		++$morphs{$morph=$m};
		if ($m =~ /^(\S+?):/) {
		    ++$prefs{$pref=$1};
		}
	    }
	    if ($f =~ /\s\#\#(\S+)/) {
		++$seen_morph2;
		my $m = $1;
		++$morph2s{$morph2=$m};
		if ($morph) {
		    push @{$morph_m2refs{$morph}}, $morph2;
		} else {
		    bad('form', "morph2 `$morph2' has no morph1");
		}
	    } elsif ($morph && $seen_morph2) {
		my $tmp = $_;
		if (s/\s\#//g > 1) {
		    bad('form', "repeated `$morph' field (missing '#' on morph2?)");
		} else {
		    bad('form', "morph has no morph2");
		}
	    }
	    if (!exists $cfgw_forms{$fo}) {
		$cfgw_forms{$fo} = [$defbang,$flang,$fo,$lfid,
				    $base,$cont,$morph,$pref,$stem,$rws,$morph2,
				    $line_of{'form'}+$field_index];
	    }
	    ++$field_index;
	}
	$field_index = 0;
#	open(L,">dumper.log");
#	print L Dumper(\%norm_forms);
#	print L "===============\n";
    }

    if ($e{'bffs'}) {
	
    } elsif ($e{'bff'}) {
	
    }

    if (scalar %stems) {
	push @ret, '<stems>';
	foreach my $s (@{$stems{'#order#'}}) {
	    $s = xmlify($s);
	    push @ret, xidify("<stem n=\"$s\"");
	    $stems{$s} = $last_xid;
	    $xstems{$s,'xid'} = $last_xid;
	    my($name,$func,$restrict) = @{$xstems{$s}};
	    push @ret, " stem=\"$name\"" if $name;
	    push @ret, " func=\"$func\"" if $func;
	    push @ret, " restrict=\"$restrict\"" if $restrict;
	    push @ret, "/>";
	}
	push @ret, '</stems>';
	$use_stemrefs = 1;
    } elsif (!is_proper($curr_pos)) {
	my $s = xmlify($curr_cf);
	push @ret, "<stems defaulted=\"yes\"><stem n=\"$s\"/></stems>";
	$use_stemrefs = 0;
    }

    if (scalar %bases) {
	push @ret, '<bases>';
	foreach my $b (@{$bases{'#order#'}}) {
	    next if $b eq '#order#';
	    my($pri,$alt) = ($b,$xbases{$b}||'');
	    my $lit = $ORACC::L2GLO::Builtins::accents ? accentify($pri) : $pri;
	    $pri = xmlify($pri);
	    my $stemattr = '';
	    if ($use_stemrefs) {
		if ($xbases{$b,'*'}) {
		    my $bstem = $xbases{$b,'*'};
		    if ($xstems{$bstem}) {
			$stemattr = " stemref=\"$xstems{$bstem,'xid'}\"";
		    } else {
			bad('bases', "base stem $bstem not declared in \@stems");
		    }
		}
	    }
            $lit = xmlify($lit);
	    push @ret, xidify("<base literal=\"$lit\"$stemattr>", $pri);
	    $bases{$pri} = $last_xid;
	    my $gline = $line_of{'bases'} || 0;
	    push @ret, "<t g:file=\"$lang.glo\" g:line=\"$gline\" g:me=\"1\">$pri</t>";
	    foreach my $a (split(/,\s+/, $alt)) {
		$a = xmlify($a);
		push @ret, "<t g:me=\"1\">$a</t>";
	    }
	    push @ret, '</base>';
	}
	push @ret, '</bases>';
    }

    if (scalar %conts) {
	push @ret, '<conts>';
	foreach my $c (keys %conts) {
	    next if $c eq '#order#';
	    push @ret, xidify("<cont>$c</cont>");
	    $conts{$c} = $last_xid;
	}
	push @ret, '</conts>';
    }

    my @m2_xml = ();
    if (scalar %morph2s) {
	push @m2_xml, '<morph2s>';
	foreach my $m (keys %morph2s) {
	    next if $m eq '#order#';
	    push @m2_xml, xidify("<morph2>$m</morph2>");
	    $morph2s{$m} = $last_xid;
	}
	push @m2_xml, '</morph2s>';
    }

    if (scalar %morphs) {
	push @ret, '<morphs>';
	foreach my $m (keys %morphs) {
	    next if $m eq '#order#';
	    my @m2refs = ();
	    if ($m && $morph_m2refs{$m}) {
		@m2refs = @{$morph_m2refs{$m}};
	    }
	    my $m2ref_attr = '';
	    if ($#m2refs >= 0) {
		my @m2ids = map { $morph2s{$_} } @m2refs;
		$m2ref_attr = " morph2=\"@m2ids\"";
	    }
	    push @ret, xidify("<morph$m2ref_attr>$m</morph>");
	    $morphs{$m} = $last_xid;
	}
	push @ret, '</morphs>';
    }

    if ($#m2_xml >= 0) {
	push @ret, @m2_xml;
    }

    if (scalar %prefs) {
	push @ret, '<prefs>';
	foreach my $p (keys %prefs) {
	    next if $p eq '#order#';
	    push @ret, xidify("<pref>$p</pref>");
	    $prefs{$p} = $last_xid;
	}
	push @ret, '</prefs>';
    }

#    print Dumper(\%conts);

    if ($e{'form'}) {
	push @ret, '<forms>';
	foreach my $fo (sort keys %cfgw_forms) {
	    my($defbang,$l,$f,$lfid,$base,$cont,$morph,$pref,$stem,$rws,$morph2,$fline) 
		= @{$cfgw_forms{$fo}};
	    $l = $cbdlang unless $l;
	    $l =~ s#/n#-949#;
	    $f = xmlify($f);
	    my $xattr = ($defbang ? ' default="yes"' : '');
	    if ($base || $cont || $morph || $pref || $stem || $rws) {
#		if ($base && $bases{$base} == 1) {
#		    warn "iffy $base\n";
#		}
		$xattr .= " base=\"$bases{$base}\"" if $base;
		$xattr .= " cont=\"$conts{$cont}\"" if $cont;
		#		$xattr .= " stem=\"$stems{$stem}\"" if $stem;
		$xattr .= " stem=\"$stem\"" if $stem;
		$xattr .= " morph=\"$morphs{$morph}\"" if $morph;
		$xattr .= " morph2=\"$morph2s{$morph2}\"" if $morph2;
		$xattr .= " pref=\"$prefs{$pref}\"" if $pref;
		$xattr .= " rws=\"$rws\"" if $rws;
	    }
#	    my $gline = $line_of{'form'} || 0;
	    my $f_no_slash = $f;
	    $f_no_slash =~ s/\\.*$//;
	    push @ret, xidify("<form literal=\"$f\" g:file=\"$lang.glo\" g:line=\"$fline\" g:me=\"1\"$xattr>%$l $f_no_slash</form>",$f);
	    $lffmap{$f} = $last_xid;
	}
	push @ret, '</forms>';
    }

    if ($e{'norms'} || scalar(keys %form_norms) > 0) {
	my %known_n = ();
	foreach my $ns (@{$e{'norms'}}) {
	    foreach my $n (split(/,\s+/,$ns)) {
		next if $known_n{$n}++;
		++$form_norms{$n};
	    }
	}

	push @ret, '<norms>';
	foreach my $n (sort keys %form_norms) {
	    if (!$known_n{$n}++) {
		my $n2 = xmlify($n); # WATCHME: if @form can have # for morph it breaks...
		if ($n =~ /\s\$/) {
#		    print STDERR "$n\n";
		    my @n = map { s/^\$//; $_ } split(/\s+(?=\$)/,$n);
		    my @new_n = ();
		    for (my $i=0; $i <= $#n; ++$i) {
			my $each_n = $n[$i];
			if ($each_n =~ /^\(/) {
			    $each_n =~ s#^\((.*?)\)$#<n secondary="1">$1</n>#;
			} else {
			    $each_n = "<n>$each_n</n>";
			}
			if (!defined $e{'parts'}) { # compounds cannot be COFs
			    if ($i) {
				$each_n =~ s#^<n#<n cof-tail="1"#;
			    } else {
				$each_n =~ s#^<n#<n cof-head="1"#;
			    }
			}
			$each_n =~ s/>%(.*?):/ xml:lang="$1">/;
			unless ($each_n =~ s/>(.*?)\$(.*?)</ literal="$1\$$2">$2</) {
			    $each_n =~ s/>(.*?)(\[.*?)</ literal="$1$2">$1</;
			}
			$each_n =~ s/^\$\((.*?)\)$//;
			push @new_n, $each_n;
		    }
		    $n2 = join('',@new_n);
#		} else {
#		    $n2 = xmlify($n2);
		}
		push @ret, xidify("<norm><n>$n2</n>",$n2);
#		print L "norm_forms\n", Dumper(\%norm_forms);
#		close L;
		if (scalar keys %{$norm_forms{$n}}) {
		    my @lfrefs = ();
		    foreach my $fo (keys %{$norm_forms{$n}}) {
			my($l,$f,$lfid) = @{${$norm_forms{$n}}{$fo}};
			push @lfrefs, $lffmap{$f};
		    }
		    push @ret, "<forms>"; # />
		    foreach my $fo (keys %{$norm_forms{$n}}) {
			my($l,$f,$lfid) = @{${$norm_forms{$n}}{$fo}};
			$l = $cbdlang unless $l;
			$f = xmlify($f);
			push @ret, xidify("<f ref=\"$lffmap{$f}\"/>");
		    }
		    push @ret, '</forms>';
		}
		push @ret, '</norm>';
	    }
	}
	push @ret, '</norms>';
    }
    push @ret, '<senses>';
    foreach my $s (@{$e{'sense'}}) {
	my ($sid,$sigs,$sgw,$pos,$mng,$stem) = ();
	my $defattr = '';

	if ($s =~ s/^(\S+)\s+//) {
	    $sid = $1;
	    if ($sid =~ s/!$//) {
		$defattr = ' default="yes"';
	    }
	    $sigs = $sigs{$sid};
	}
	if ($s =~ s/^\[(.*?)\]\s+//) {
	    $sgw = $1;
#	    warn "found sgw = $sgw\n";
	}
	if ($s =~ s/\s+\*(\S+)//) {
	    $stem = $1;
	}
	if ($s =~ /^[A-Z]+(?:\/[it])?\s/) {
	    ($pos,$mng) = ($s =~ /^([A-Z]+(?:\/[it])?)\s+(.*)\s*$/);
	} else {
	    $mng = $s;
	    $mng =~ s/^\s*(.*?)\s*$/$1/;
	}
	if ($pos) {
	    if (!exists $poss{$pos} && $pos !~ /^V\/[ti]/) {
		if ($pos =~ /^[A-Z]+$/) {
		    bad('sense',"$pos not in known POS list");
		} else {
		    $mng = "$pos $mng";
		}
	    }
	} else {
	    $pos = '';
	}
	if (!$mng) {
	    bad('sense',"meaning not given");
	    $mng = '';
	}
	my $sgwTag = '';
	$sgwTag = "<sgw>$sgw</sgw>" if $sgw;
	my $posTag = '';
	$posTag = "<pos>$pos</pos>" if $pos;
	my $stemTag = '';
	$stemTag = "<stem>$stem</stem>" if $stem;
	my $s_sig = $e_sig;
	if ($sgw) {
	    $s_sig =~ s#\]#//$sgw\]#;
	} else {
	    $s_sig =~ s#\]#//$mng\]#;
	}
	if ($pos) {
	    $s_sig .= "'$pos";
	} else {
	    $s_sig =~ s/\](.*)$/]$1'$1/;
	}
	my $oidattr = '';
	if ($blang =~ /^sux/) {
	    my $oid = oid_lookup('sux',$s_sig);
	    if ($oid) {
		$oidattr = " oid=\"$oid\"";
	    }
	}
	push @ret, xidify("<sense n=\"$s_sig\"$defattr>$sgwTag$posTag$stemTag<mng xml:lang=\"$mnglang\">$mng</mng>");
	if (defined $sense_props{$sid}) {
	    push @ret, '<props>';
	    foreach my $p (@{$sense_props{$sid}}) {
		my($n,$v) = ($p =~ /^(\S+)\s+(.*?)\s*$/);
		die "$p\n" unless $n;
		push @ret, "<prop key=\"$n\">"; # ,'!'
		foreach my $vv (split(/\s+/,$v)) {
		    push @ret, "<v>$vv</v>";
		}
		push @ret, '</prop>';
	    }
	    push @ret, '</props>';
	}
	push @ret, ("</sense>"); # ,'!'
    }
    push @ret, '</senses>';
    %sense_props = ();
    
    if ($e{'prop'}) {
	push @ret, '<props>';
	foreach my $p (@{$e{'prop'}}) {
	    my($n,$v) = ($p =~ /^(\S+)\s+(.*?)\s*$/);
	    die "$p\n" unless $n;
	    push @ret, "<prop key=\"$n\">";
	    foreach my $vv (split(/\s+/,$v)) {
		push @ret, "<v>$vv</v>";
	    }
	    push @ret, '</prop>';
	}
	push @ret, '</props>';
    }
    if ($e{'equiv'}) {
	push @ret, '<equivs>';
	foreach my $s (@{$e{'equiv'}}) {
	    my ($lang,$eq) = ($s =~ /^%(\S+)\s+(.*)\s*$/);
	    if (!$lang) {
		bad('equiv',"%lang required after \@equiv");
	    } else {
		push @ret, xidify("<equiv xml:lang=\"$lang\">$eq</equiv>",'!');
	    }
	}
	push @ret, '</equivs>';
    }
    if ($e{'isslp'}) {
	push @ret, '<bib>';
	foreach my $i (@{$e{'isslp'}}) {
	    my $date = '';
	    $i =~ s/(\S+)\.?\s+//;
	    $date = $1;
	    push @ret, "<ref year=\"$date\">$i</ref>";
	}
	push @ret, '</bib>';
    }
    if ($e{'bib'}) {
	push @ret, '<bib>';
	foreach my $i (@{$e{'bib'}}) {
	    my $date = '';
	    $i =~ s/(\S+)\s+//;
	    $date = $1;
	    push @ret, "<ref year=\"$date\">$i</ref>";
	}
	push @ret, '</bib>';
    }
    if ($e{'pl_id'}) {
	my %pl = ();
        foreach my $p (@{$e{'pl_id'}}) {
	    $pl{'id'} = $p;
            push @ret, "<pl_id>$p</pl_id>";
        }
	if ($e{'pl_coord'}) {
	    foreach my $p (@{$e{'pl_coord'}}) {
#		warn "Builtins: using hard-coded pl_coord $p for $pl_id\n";
		push @ret, "<pl_coord>$p</pl_coord>";
		$pl{'coord'} = $p;
	    }
	} else {
	    pleiades_init($cbd_project)
		unless $pleiades_initialized;
	    my $pl_coord = ORACC::XMD::Pleiades::coords($pl{'id'}, 
							@pleiades_data);
	    if ($pl_coord) {
		$pl{'coord'} = $pl_coord;
		push @ret, "<pl_coord>$pl_coord</pl_coord>";
	    } else {
		$pl{'coord'} = '';
	    }
	}
	if ($e{'pl_uid'}) {
	    foreach my $p (@{$e{'pl_uid'}}) {
		$pl{'uid'} = $p;
		push @ret, "<pl_uid>$p</pl_uid>";
	    }
	} else {
	    $pl{'uid'} = '';
	}
	if ($e{'alias'}) {
	    $pl{'alias'} = join(',',@{$e{'alias'}});
	} else {
	    $pl{'alias'} = '';
	}
	my $cfgw = ${$e{'entry'}}[0];
	#	$cfgw =~ s/\s*\[.*//;
	$cfgw =~ s/\s*\[(.*?)\]\s*/\[$1\]/;
	$cfgw =~ m/^(.*?)\[/;
	my $cf = $1;
	$pl{'cf'} = $cf;
	$pleiades{$cfgw} = { %pl };
    } else {
	# nothing happens I think
    }
    push @ret, '</entry>';
    ++$eid;
    @ret;
}

my %parts_map = ();
my $parts_map_loaded = 0;

## FIXME: this should be deprecated in favour of using 01bld/projectx.sig.
## This would make the .cbd build depend on .sig build, but that's OK now
## that sig-building is l2p1 and cbd-building is l2p2
sub
load_parts_map {
    $parts_map_loaded = 1;
    open(M,"01bld/$cbdlang/parts.map") || return;
#    warn "loading parts.map\n" if $verbose;
    while (<M>) {
	/^(.*?)\t(.*?)$/;
	$parts_map{$1} = $2;
    }
    close(M);
}

sub
render_parts {
    my $cpd = shift;
    my @ret = ();
    my @part_sigs = ();
    my $primary = 0;
    my $pindex = 0;

    load_parts_map() unless $parts_map_loaded;

    if ($parts_map{$cpd}) {
	@part_sigs = split(/\+\+/, $parts_map{$cpd});
    }

    $pindex = 0;
    $cpd =~ s/(\]\S+)\s+/$1\cA/g;
    push @ret, '<compound>';
    my @c = split(/\cA/,$cpd);
    for (my $i = 0; $i <= $#c; ++$i) {
	my $c = $c[$i];
	my $xmorph = '';
	$c =~ s/<.*?>$//;
	my $part_attr = " partsig=\"$c\"";
	if ($part_sigs[$i]) {
	    my $esig = $part_sigs[$i];
	    $esig =~ s#^.*?=(.*?)//.*?(\]\S+?)'.*$#$1$2#;
	    $part_attr .= " esig=\"$esig\"";
	}
	if ($c =~ s/#(.*)$//) {
	    $xmorph = "<morph>$1</morph>";
	}
	my($ccf,$cgw,$cpos) 
	    = ($c =~ /^(\S+)\[(.*?)\]($POSrx(?:'$POSrx)?)?$/);
	bad('compound', "malformed parts component `$c'") and next
	    unless $ccf;
	$cpos = 'n' if $c =~ /\]n/;
	$cgw = '' if $c =~ /\[\]/;
	$ccf = '' unless $ccf;
	my $pattr = '';
	my $lattr = '';
	bad('compound', "missing POS on compound part") unless $cpos;
	my($xgw,$xpos) = ();
	if ($cgw =~ m#^(.*?)//(.*?)$#) {
	    $xgw = "<gw>$1</gw><sense>$2</sense>";
	    $cgw = $1;
	} else {
	    $xgw = "<gw>$cgw</gw>";
	}
	if ($cpos =~ /^(.*?)'(.*?)$/) {
	    $xpos = "<pos>$1</pos><epos>$2</epos>";
	    $cpos = $1;
	} else {
	    $xpos = "<pos>$cpos</pos>";
	}
	if (is_proper($cpos)) {
	    if ($cbdlang ne 'qpn') {
		$lattr = " xml:lang=\"qpn\"";
	    }
	} elsif ($cbdlang eq 'qpn') {
#		warn "langof.sh \@entry $ccf \[$cgw\] $cpos\n";
	    my (@xlang) 
		= `fgrep -m1 -l "\@entry $ccf \[$cgw\] $cpos" 00lib/*.glo`;
	    
	    @xlang = grep(!/qpn/,@xlang);
	    if ($#xlang >= 0) {
#		    warn "xlang = $xlang\n";
		my $xlang = $xlang[0]; # FIXME: what if word is in two GLO's?
		$xlang =~ s#^00lib/(.*?).glo$#$1#;
		$lattr = " xml:lang=\"$xlang\"";
	    } else {
		bad('compound', "unknown external compound part '$ccf\[$cgw\]$cpos'");
	    }
	}
	if ($pindex == $primary) {
	    $pattr = " primary=\"1\"";
	}
	if ($cpos eq 'n') {
	    push @ret, "<cpd$part_attr$pattr$lattr><pos>n</pos></cpd>";
	} else {
	    bad('compound',"malformed compound '$c': should be CF[GW]POS")
		unless $ccf && defined($cgw) && $cpos;
	    push @ret, "<cpd$part_attr$pattr$lattr><cf>$ccf</cf>$xgw$xpos</cpd>";
	    ++$primary;
	}
    }
    push @ret, '</compound>';
    @ret;
}

sub
xidify {
    my ($tmp,$content) = @_;
    my($tag,$val) = ();
    if ($content && $content ne '!') {
	my($tag,$val) = ();
	if ($content) {
	    $tmp =~ /^<([a-z]+)/;
	    $tag = $1;
	    $val = $content;
	} else {
	    $tmp =~ /^<([a-z]+)[^>]*>(.*?)</;
	    ($tag,$val) = ($1,$2);
	}
	# $cbdid.$eid
	$tmp =~ s#(/?>)# xml:id="$eid.$sid"$1#;
	# $cbdid.$eid
	$xids{"$tag\:$val"} = $last_xid = "$eid.$sid";
    } elsif ($tmp =~ /^<([a-z]+)[^>]*>(.*?)</) {
	($tag,$val) = ($1,$2);
	# $cbdid.$eid
	$tmp =~ s#(/?>)# xml:id="$eid.$sid"$1#;
	# $cbdid.$eid
	$last_xid = "$eid.$sid";
    } else {
	$tmp =~ /^<([a-z]+)/;
	($tag,$val) = ($1,'');
	# $cbdid.$eid
	$tmp =~ s#( )# xml:id="$eid.$sid"$1#;
	# $cbdid.$eid
	$last_xid = "$eid.$sid";
    }
    ++$sid;
    $tmp;
}

sub
setup_list_field {
    my($e,$hash,$field,$xhash,$xsplit) = @_;
    if ($$e{$field}) {
	warn Dumper($e) if $ORACC::L2GLO::debug && $field eq 'bases';
	my @bits = split(/;\s+/, join(' ', @{$$e{$field}}));
	warn "bits = @bits\n" if $ORACC::L2GLO::debug;
	if ($xsplit) {
	    $bases_atf = '';
	    foreach my $b (@bits) {
		my($head,$tail) = &$xsplit($b,$xhash);
		++$$hash{$head};
		push @{$$hash{'#order#'}}, $head;
		$$xhash{$head} = $tail if $tail;
	    }
	    if ($field eq 'bases') {
		$bases_atf =~ tr/·°//d;
		push @glo_atf, "$line_of{'bases'}.\t%sux $bases_atf\n";
		$bases_atf = '';
	    }
	    warn Dumper($hash) if $ORACC::L2GLO::debug;
	} else {
	    @{$$hash{@bits}} = ();
	}
    }
}

sub
xbase_split {
    my ($b,$xh) = @_;
    my ($pri,$alt) = ();
    if ($b =~ s/^\*(\S+)\s+//) {
	$bstar = $1;
    } else {
	$b =~ s/^\*\s*//;
    }
    if ($b =~ /\s+\(/) {
	my $tmp = $b;
	bad('bases',"malformed alt-base in `$b'")
	    if ($tmp =~ tr/()// % 2);
	($pri,$alt) = ($b =~ /^(\S+)\s+\((.*?)\)\s*$/);
	$$xh{$pri,'*'} = $bstar;
	$bases_atf .= " $pri ";
	foreach my $t (split(/,\s+/,$alt)) {
	    bad('bases',"space in alt-base `$t'")
		if $t =~ /\s/;
	    $bases_atf .= "$t ";
	}
    } else {
	$$xh{$b,'*'} = $bstar;
	$bases_atf .= "$b ";
	$pri = $b;
	$alt = '';
    }
    bad('bases',"space in base `$pri'") if $pri =~ /\s/;
    ($pri,$alt);
}

sub
funcs_ok {
    foreach my $s (split(/\+/,$_[0])) {
	return 0 unless exists $funcs{$s};
    }
    1;
}

sub
xstem_split {
    my @s = split(/\s+/, $_[0]);
    my $restrict = ($s[$#s] =~ s/^\@// ? pop(@s) : '');
    my ($norm,$stem,$func) = ();
    foreach my $s (@s) {
	if (exists $stems{$s}) {
	    $stem = $s;
	} elsif (funcs_ok($s)) {
	    $func = $s;
	} elsif ($norm) {
	    bad('stems',"second norm $s is not a stem or function");
	} else {
	    $norm = $s;
	}
    }
    ($norm, [$stem,$func,$restrict]);
}

sub
make_file_pi {
    my $f = shift;
    $f =~ s/\.norm$//;
    "<?file $f?>";
}
sub
make_line_pi {
    if ($_[0]) {
	"<?line $_[0]?>";
    } else {
	warn "line num undef'd\n";
    }
}

sub
accentify {
    my $tmp = shift;
    $tmp =~ s/([AEIUaeiu])([A-Za-zŠšḪḫṢṣṬṭŊŋʾ]*)([₀-₉]+)/acc($1,$3,$2)/eg;
    $tmp;
}

sub
acc {
    my($vow,$num,$rest) = @_;
    my $acc = $recip_acc{$vow,$num};
    if ($acc) {
	"$acc$rest";
    } else {
	"$vow$rest$num";
    }
}

sub
add_ids {
    my($set,$cfg,$cbd) = @_;
    foreach my $c ($cbd->getDocumentElement()->childNodes()) {
	$c->setAttributeNS($XML,'id',$eid);
	my $sid = 1;
	my %form_ids = ();
	foreach my $sc ($c->findnodes('.//cbd:base|.//cbd:sense|.//cbd:norm|.//cbd:form|.//cbd:norm//cbd:f')) {
	    $sc->setAttributeNS($XML,'id',"$eid.$sid");
	    if ($sc->localName() eq 'form') {
		$form_ids{$sc->textContent()} = "$eid.$sid";
	    }
	    ++$sid;
	}
	foreach my $r ($c->findnodes('.//cbd:f')) {
	    $r->setAttribute('ref',$form_ids{$r->textContent()});
	}
	++$eid;
    }
    $cbd;
}

sub
bad {
    my $lfield = shift || '';
    $field_index = 0 unless defined $field_index;
    my $lineof = $line_of{$lfield} || 0;
    my $eline = $lfield ? ($lineof + $field_index) : $.;
    $eline = $line_of{'#init'} unless $eline;
    push @{$errlist{$eline}}, "$bad_input:$eline: during $bad_action: ", @_, " [fi=$field_index]\n";
    ++$status;
}

sub
input {
    my($set,$cfg,$cbd) = @_;
    my $input = $$set{'input'};
    warn ("loading input $input\n") if $ORACC::L2GLO::verbose;
    $cbd = load_xml($input) || die "ORACC::L2GLO::input: can't read '$input'\n";
    return $cbd;
}

sub
load_acd {
    my($set,$cfg,$cbd) = @_;
    my $proj_type = ${$$cfg{'proj_type'}}[0];
    my $input = $$set{'input'};
    $cbdlang = ${$$cfg{'cbdlang'}}[0];
    $cbdid = $cbdlang;
    $cbdid =~ tr/-/_/;
    my $basename = ${$$cfg{'basename'}}[0];
    my $acd_dir = ($proj_type && $proj_type eq 'corpus') ? ${$$cfg{'lib'}}[0] : ${$$cfg{'sources'}}[0];
    my $acd_base = ($proj_type && $proj_type eq 'corpus') ? 'glossary' : $basename;
    $bad_input = $input = "00lib/$cbdlang.glo" unless $input;
    $bad_action = 'load_acd';
    if (-r $input) {
	$gdtag = ${$$cfg{'use_gw'}}[0] ? 'gw' : 'dt';
	warn "loading acd $input (gdtag = $gdtag)\n" if $ORACC::L2GLO::verbose;
	if ($cbdlang) {
	    if ($$cfg{'mnglang'}) {
		$mnglang = ${$$cfg{'mnglang'}}[0];
	    }
	    return acd2xml($input,$cbdlang,$basename.'_cbd_'.$cbdlang, ${$$cfg{'title'}}[0]);
	} else {
	    $. = 0;
	    bad(undef,"cbdlang must be given in config file");
	    return undef;
	}
    } else {
	die "nonexistent acd file $input\n";
    }
}

sub
output {
    my($set,$cfg,$cbd) = @_;
    my $output = $$set{'output'};
    warn ("writing output $output\n") if $ORACC::L2GLO::verbose;
    my $x = $cbd->toString();
    if ($$set{'text'} && $$set{'text'} eq 'yes') {
	$x =~ s/^<?xml.*?\?>\n?//;
    }
    open(OUT,">$output") || die "ORACC::L2GLO::output: can't write '$output'\n";
    binmode(OUT,':raw');
    print OUT $x;
    close(OUT);
    # must return cbd in case output is called in a pipeline
    $cbd;
}

sub
sort {
    my($set,$cfg,$cbd) = @_;
    my %cf = ();
    my %dt = ();
    my @e = ();
    my @x = ();
    my $doc = $cbd->getDocumentElement();
    foreach my $e ($doc->childNodes()) {
	my $cf_node = (tags($e,$CBD,'cf'))[0];
	if ($cf_node) {
	    my $cf = $cf_node->textContent();
	    my $dt = $cf_node->nextSibling()->textContent();
	    if (!$cf || !$dt) {
		$cf = '' unless $cf;
		$dt = '' unless $dt;
	    } else {
		$cf =~ tr/*?()//d;
		++$cf{$cf};
		++$dt{$dt};
		push @e, [ $cf, $dt, $e];
	    }
	} else {
	    push @x, $e;
	}
    }
    if ($#e >= 2) {
	set_cgc(@e);
	$doc->removeChildNodes();
	foreach my $s (sort { $cgc{$$a[0]} <=> $cgc{$$b[0]} 
			      || $$a[1] cmp $$b[1] } 
		       @e) {
	    $doc->appendChild($$s[2]);
	}
	foreach (@x) {
	    $doc->appendChild($_);
	}
    }
    $cbd;
}

sub
set_cgc {
    %cgc = ();
    my $tmpname = '';
    if (open(TMP,">01tmp/$$.cgc")) {
	$tmpname = "01tmp/$$.cgc";
    } elsif (open(TMP,">/tmp/$$.cgc")) {
	$tmpname = "/tmp/$$.cgc";
    } else {
	die "set_cgc: can't write /tmp/$$.cgc or tmp/$$.cgc\n";
    }
    $cgctmp = $tmpname;
    my %t = (); @t{@_} = (); # uniq the sort keys
    foreach my $t (@_) {
	my $tx = ${$t}[0];
	$tx =~ tr/_/—/; ### HACK !!!
	$tx =~ tr/ /_/;
	print TMP "${tx}_\n";
    }
    close TMP;
    system 'msort', '-j', '--out', $tmpname, '-ql', '-n1', '-s', '@@ORACC@@/lib/config/msort.order', '-x', '@@ORACC@@/lib/config/msort.exclude', $tmpname;
    open(TMP,$tmpname);
    my @cgc = (<TMP>);
    close(TMP);
    chomp @cgc;
    @cgc = map { s/_$//; tr/_—/ _/; $_ } @cgc;
    @cgc{@cgc} = (0..$#cgc);
    foreach my $e (@_) {
	warn "$$e[0]: not in cgc\n" unless exists $cgc{$$e[0]};
    }
}

# Read the ACD into memory; the only parsing done at this stage is to
# break the input into entries and reduce multi-line fields to single
# lines.
sub
input_acd {
    my($input,$lang,$n,$title,$save_header) = @_;
    my %acd = ();
    %seen = ();
    $status = 0;
    $bad_action = 'input_acd';
    $bad_input = $input; $bad_input =~ s/\.norm$//; $bad_input =~ s/01bld/00lib/;
    my $currtag = undef;
    $curr_file = $input;
    $curr_line = 1;
    $line_of{'#init'} = 1;
    open(IN,$input) || die "cbdmanager.plx: input_acd: can't open `$input'\n";
    my %e = ();
    $lang = '' unless $lang;
    $use_norms = $langnorms{$lang};
    while (<IN>) {
	++$curr_line;
	next if /^\#/ || m/^\@letter/;
	chomp;
	if (/^\@(project|name|lang)\s+(.*?)\s*$/) {
	    if ($save_header) {
		$merge_header{$1} = $2;
	    }
	    next;
	}
	s/\s+/ /g;
#	warn "$_\n" if /^\@entry\*/;
	my $usage_flag = s/^\@entry\*/\@entry/;
	if (/^\@([a-z]+)\s+(.*?)\s*$/) {
	    $currtag = $1;
	    my $tagarg = $2;
	    $line_of{$currtag} = $curr_line;
	    if (!exists($tags{$currtag})) {
		bad(undef,"unknown tag '$currtag'");
	    } elsif ($currtag eq 'end') {
		if ($tagarg eq 'entry') {
		    my $eref = { %e };
#		    warn "storing usage_flag\n" if $$eref{'usage_flag'};
		    push @{$acd{'entries'}}, $eref;
		    ${$acd{'ehash'}{parse_entry($e{'entry'})}} = $eref;
		    if ($is_compound) {
			bad('entry',"compound entries must have a '\@parts' field")
			    unless ${$e{'fields'}}{'parts'} || $ORACC::L2GLO::Builtins::bare;
			--$status if $status;
			# $is_compound = 0;
		    }
		    %e = ();
		} else {
		    bad($currtag,"malformed \@end entry");
		}
		$line_of{'#init'} = $curr_line;
	    } else {
		if ($currtag eq 'bases' && $e{'bases'}) {
		    bad('bases',"`bases' can only be given once");
		    --$status;
		}
		push @{$e{$currtag}}, $tagarg;
		++${$e{'fields'}}{$currtag};
		if ($currtag eq 'entry') {
		    $e{'usage_flag'} = $usage_flag;
		    $e{'#line'} = $curr_line;
#		    warn "setting usage_flag on $_\n" if $e{'usage_flag'};
		}
	    }
	} elsif (/^\@([A-Z]+)\s+(.*?)\s*$/) {
	    ${$e{'rws_cfs'}}{$1} = $2;
	} elsif (/^\s+\S/) {
	    if ($currtag) {
		s/^\s+/ /;
		${$e{$currtag}}[$#{$e{$currtag}}] .= $_;
	    } else {
		bad($currtag,"continuation line before tags");
	    }
	} elsif (/^\s*$/) {
	    bad($currtag,"blank lines not allowed within entries") if scalar %e;
	} else {
	    chomp;
	    bad($currtag,"syntax error near '$_'");
	}
    }
    close(IN);
    foreach my $e (sort { $a <=> $b } keys %errlist) {
	warn @{$errlist{$e}};
    }
    die "$status errors; stop\n" if $status;
    return { %acd };
}

sub
first_letter {
    my($first) = ($_[0] =~ /^(.)/);
    $first = "\U$first";
    $first = $vowel_of{$first} if $vowel_of{$first};
    $first;
}

sub
acd_dump_entry {
    my %e = %{$_[0]};
    my $cfgw = ${$e{'entry'}}[0];
    my $init_char = first_letter($cfgw);
    unless ($ORACC::L2GLO::Builtins::noletters) {
	if (!$last_char || $last_char ne $init_char) {
	    $last_char = $init_char;
	    print "\@letter $last_char\n\n";
	}
    }
    my $ustar = ($e{'usage_flag'} ? '*' : '');
    print "\@entry$ustar $cfgw\n";
    foreach my $rws (sort keys %{$e{'rws_cfs'}}) {
	print "\@$rws ${$e{'rws_cfs'}}{$rws}\n";
    }
    foreach my $f (sort {$fseq{$a}<=>$fseq{$b}} keys %{$e{'fields'}}) {
	next if $f eq 'entry' || $f eq 'rws_cf';
	foreach my $l (@{$e{$f}}) {
	    print "\@$f $l\n";
	}
    }
    print "\@end entry\n\n";
}

sub
acd_sort {
    my $acd = shift;
    my @esort = ();
    foreach my $e (keys %{$$acd{'ehash'}}) {
	my($cf,$dt) = ($e =~ /^(.*?)\[([^\[]+)\]/);
	push @esort, [ $cf, $dt, $e];
    }
    if ($#esort > 1) {
	set_cgc(@esort);
	@{$$acd{'entries'}} = ();
	foreach my $s (sort { $cgc{$$a[0]} <=> $cgc{$$b[0]} 
			      || $$a[1] cmp $$b[1] } 
		       @esort) {
	    my $ehash = ${$$acd{'ehash'}}{$$s[2]};
	    my $eref = { %{$$ehash} };
	    push @{$$acd{'entries'}}, $eref;
	}
	unlink $cgctmp;
    }
}

sub
merge {
    my($p,$c,$x) = @_;
    merge_exec(${$$c{'cbdlang'}}[0],$$p{'input'},$$p{'merge'},$$p{'output'});
}

sub
merge_exec {
    my($lang,$input,$merge,$output) = @_;
    $cbdlang = $lang;
    $cbdid = $cbdlang;
    $cbdid =~ tr/-/_/;
    my $into = input_acd($input,$cbdlang,'','', 1);
    foreach my $m (@$merge) {
	my $from = input_acd($m,$cbdlang,'','', 0);
	foreach my $e (keys %{$$from{'ehash'}}) {
	    if (!defined ${$$into{'ehash'}}{$e}) {
		my $ehash = ${$$from{'ehash'}}{$e};
		my $eref = { %{$$ehash} };
		push @{$$into{'entries'}}, $eref;
		${$$into{'ehash'}{$e}} = $eref;
	    } else {
		my $f = ${$$from{'ehash'}}{$e};
		my $i = ${$$into{'ehash'}}{$e};
		foreach my $fld (sort {$fseq{$a}<=>$fseq{$b}} keys %{$$$f{'fields'}}) {
		    next if $fld eq 'entry';
		    # if $f = 'form', build an index of 'form's in %known
		    # this should use canonicalized versions as returned by the
		    # parse_xxx routines, but they are not done yet ...
		    my %known = ();
		    foreach my $l (@{$$$i{$fld}}) {
			my $tmp = $l;
			$tmp =~ s/\s+\@\S+\s*//;
			if ($fld eq 'bases') {
			    foreach my $b (split(/;\s+/, $tmp)) {
				++$known{$b};
			    }
			    my @b = sort keys %known;
#			    warn "known bases = @b\n";
			} else {
			    ++$known{$tmp};
			}
		    }
		    foreach my $l (@{$$$f{$fld}}) {
			my $tmp = $l;
			$tmp =~ s/\s+\@\S+\s*//;
			if ($fld eq 'bases') {
			    foreach my $b (split(/;\s+/, $tmp)) {
				if (!defined $known{$b}) {
				    ++${$$$i{'fields'}}{$fld} unless ${$$$i{'fields'}}{$fld};
				    ${$$$i{'bases'}}[0] .= "; $b";
				}
			    }
			} else {
			    if (!defined $known{$tmp}) {
				++${$$$i{'fields'}}{$fld} unless ${$$$i{'fields'}}{$fld};
				push @{$$$i{$fld}}, $l;
			    }
			}
		    }
		}
	    }
	}
    }
    acd_sort($into);
#    print Dumper $into;
    my $o_out;
    if ($output) {
	open(O,">$output"); 
	$o_out = select O;
    }
    print <<EOH;
\@project $merge_header{'project'}
\@lang $merge_header{'lang'}
\@name $merge_header{'name'}

EOH
    foreach my $e (@{$$into{'entries'}}) {
	acd_dump_entry($e);
    }
    select $o_out if $o_out;
    close(O);
}

sub
link {
    my($set,$cfg,$cbd) = @_;
    system "touch $$set{'from'}" unless -e $$set{'from'};
    symlink $$set{'from'}, $$set{'to'};
}

sub
load_sigs {
    my $g2s = shift;
    $g2s =~ s/g2a$/g2s/;
    open(S,$g2s) || return;
    while (<S>) {
	/^(\S+)\s+(.*)$/;
	$signatures{$1} = $2;
    }
    close(S);
}

###########################################################################
#
# parse_xxx routines all validate and canonicalize the relevant @-field in
# an acd file; the canonicalized form is returned on success, the empty
# string on failure.

sub
parse_entry {
    my @e = @{$_[0]};
    if ($#e != 0) {
	bad('entry',"multiple \@entry fields not permitted");
	'';
    } else {
	$e[0] =~ tr/\n\r/  /;
	my($cf,$gw,$pos) = ($e[0] =~ /^([^\[]+)\s+(\[[^\]]+\])\s+(\S+)\s*/);
	if (!$cf) {
	    bad('entry',"syntax error in \@entry line");
	    '';
	} else {
	    $is_compound = $cf =~ /\s/;
	    "$cf$gw$pos";
	}
    }
}

sub
parse_form {
    $formattr{'base'} = $1 if $_[0] =~ s#\s+/(\S+)##;
    $formattr{'cont'} = $1 if $_[0] =~ s#\s+\+(\S+)##;
    $formattr{'morph'} = $1 if $_[0] =~ s/\s+\#(\S+)//;
    if ($formattr{'morph'} =~ /^(\S+?):/) {
	$formattr{'pref'} = $1;
    }
    $formattr{'rws'} = $1 if $_[0] =~ s/\s+\@(\S+)//;
    $_[0];
}

sub
parse_parts {
    $_[0];
}

sub
parse_norms {
    $_[0];
}

sub
parse_sense {
    $_[0];
}

sub
parse_equiv {
    $_[0];
}

sub
parse_inote {
    $_[0];
}

sub
parse_root {
    $_[0];
}

sub
parse_bases {
#    my @b = split(/;\s+/, $_[0])
    $_[0];
}

sub
parse_conts {
    $_[0];
}

sub
parse_collo {
    $_[0];
}

sub
parse_prefs {
    $_[0];
}

sub
is_proper {
    $_[0] && $_[0] =~ /^[A-Z]N$/;
}

sub
check_base {
    my($b,%b) = @_;
    my @b = keys %b;
    unless ($b{$b}) {
	bad('form', "unknown base `$b'");
	0;
    }
    1;
}

sub
pleiades_init {
    my $proj = shift;
    $pleiades_initialized = 1;
    warn("Builtins: pleiades_init triggered\n");
    my $p = ORACC::XMD::Pleiades::load('00lib/pleiades.tab');
    push(@pleiades_data, $p) if $p;
    unless ($proj eq 'geonames') {
	my $p = ORACC::XMD::Pleiades::load("$ENV{'ORACC'}/pub/geonames/pleiades.tab");
	push(@pleiades_data, $p) if $p;
    }
}

1;
