package ORACC::CBD::XML;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/pp_xml/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::XML;
use ORACC::NS;
use ORACC::CBD::Hash;
use ORACC::CBD::PPWarn;

my %parts_map = ();
my $parts_map_loaded = 0;

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
my $curr_id = '';
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
my $lang = '';
my $last_char = undef;
my $mnglang = 'en';
my %seen = ();
my $seen_morph2 = '';
my %xids = ();
my $last_xid;
my %line_of = ();
my $usage_flag = 0;
my $ebang_flag = 0;
my %sense_props = ();

my @funcs = qw/free impf perf Pl PlObj PlSubj Sg SgObj SgSubj/;
my %funcs = (); @funcs{@funcs} = ();

my @poss = qw/AJ AV N V DP IP PP CNJ J MA O QP RP DET PRP POS PRT PSP SBJ NP M MOD REL XP NU
	AN BN CN DN EN FN GN HN IN JN KN LN MN NN ON PN QN PNF RN SN TN U UN VN WN X XN YN ZN/;
push @poss, ('V/t', 'V/i');
my %poss = (); @poss{@poss} = ();

my @stems = qw/B rr RR rR Rr rrr RRR rrrr RRRR S₁ S₂ S₃ S₄/;
my %stems = (); @stems{@stems} = ();

my @tags = qw/entry alias parts bases bff conts morphs morph2s moved phon prefs root 
	      form length norms sense stems equiv inote prop end isslp bib was
	      defn note pl_coord pl_id pl_uid/;
my %tags = (); @tags{@tags} = ();

my %fseq = ();
foreach my $f (@tags) {
    $fseq{$f} = 0 + scalar keys %fseq;
}

my %bffs_index = ();
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

sub pp_xml {
    #    my($input,$n,$arglang,$title) = @_;
    my ($args) = @_;
    my $cbdname = "$$args{'project'}:$$args{'lang'}";
    my $hash = $ORACC::CBD::data{$cbdname};
    if (!$hash) {
	if (pp_hash($args)) {
	    $hash = \%{$ORACC::CBD::data{$cbdname}};
	}
    }
    return undef unless $hash;
    
    my $project = $$args{'project'};
    my $last_tag = '';

    %seen = ();

    $cbdlang = $lang = $$args{'lang'};

    $cbdid = $lang;
    $cbdid =~ tr/-/_/;

    my ($title,$n) = ();
    ($project, $n, $title, $lang) = (@$args{qw/project name name lang/});
    $n =~ tr#/#_#;
    $n =~ s/_[^_]+$//;
    my @xml = (xmldecl(),"<entries xmlns=\"http://oracc.org/ns/cbd/1.0\" xmlns:cbd=\"http://oracc.org/ns/cbd/1.0\" xmlns:g=\"http://oracc.org/ns/gdl/1.0\" xmlns:n=\"http://oracc.org/ns/norm/1.0\" xml:lang=\"$lang\" g:file=\"$lang.glo\" project=\"$project\" n=\"$n\" name=\"$title\">");

    my %entries = %{$$hash{'entries'}};
    foreach my $id (@{$$hash{'ids'}}) {
	if ($entries{$id,'e'}) {
	    push(@xml, acdentry($hash, $id, %{$entries{$id,'e'}}));
	} else {
	    warn "$0: weird: no hash for entry $id\n";
	}
    }
    
    push(@xml,'</entries>');

    if (pp_status()) {
	return undef;
    }

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
acdentry {
    my ($hash,$id, %e) = @_;
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

    my %entries = %{$$hash{'entries'}};
    my %line_of = %{$entries{$id,'l'}};
    
    if ($e{'entry'}) {
	$sid = 0;
	if ($#{$e{'entry'}} > 0) {
	    pp_warn("multiple \@entry' fields");
	} else {
	    $e = ${$e{'entry'}}[0];
	    unless ($eid = $entries{$e}) {
		pp_warn("no entries eid for `$e' (this can't happen)");
		return;
	    }
	    if ($entries{$eid,'bffs'}) {
		@bffs = @{$entries{$eid,'bffs'}};
	    }
	    if ($entries{$eid,'bffs-listed'}) {
		@bffs_listed = @{$entries{$eid,'bffs-listed'}};
	    }
	    if ($#bffs >= 0 && $#bffs_listed >= 0) {
		pp_warn("entry with BFFs is also owner of BFFs");
	    }
 	    if ($seen{$e}++) {
		pp_warn("duplicate cf [gw]: $e");
	    } else {
		my($cf,$gd,$pos) = ($e =~ /^(.*?)\s+\[(.*?)\]\s*([A-Za-z0-9]+(?:\/[a-z])?)?$/);
		$curr_cf = $cf;
		$curr_pos = $pos;
		if ($e =~ /\]\S/) {
		    pp_warn("missing space after ]");
		}
		if ($cf) {
		    if ($gdtag eq 'gw' && !$gd) {
			pp_warn("no guideword in entry field");
		    } else {
			my $cacf = '';
			my $usattr = '';
			my $defattr = '';
			$gd = '' unless $gd;
			
			if ($gd =~ tr/"//d) {
			    pp_warn("double quotes not allowed in GW\n");
			}

			++$form_norms{$cf} if $use_norms && !defined($e{'parts'});
			$cf = xmlify($cf);
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
			push @ret, "<entry xml:id=\"$cbdid.$eid\" n=\"$e_sig\"$usattr$defattr>",
			    make_file_pi(pp_file()), 
			    make_line_pi($line_of{'entry'}), 
			    "<cf$cacf>$cf</cf>";
			if ($e{'alias'}) {
			    foreach my $alias (@{$e{'alias'}}) {
				push @ret, "<alias>$alias</alias>";
			    }
			}
			foreach my $dialect (keys %{$e{'rws_cfs'}}) {
			    if ($rws_map{$dialect}) {
				push @ret, "<dcf xml:lang=\"$rws_map{$dialect}\" n=\"$dialect\">${$e{'rws_cfs'}}{$dialect}</dcf>";
			    } else {
				pp_warn("unknown dialect abbreviation `$dialect'\n");
			    }
			}

			push @ret, "<$gdtag xml:lang=\"$mnglang\">$gd</$gdtag>";
		    }
		} else {
		    pp_warn("no citation form in entry field");
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
			    pp_warn("$pos not in list of known POS");
			}
		    } else {
			push @ret, "<pos>$pos</pos>";
		    }
		}
	    }
	}
    } else {
	pp_warn("no 'entry' field");
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
		    $v = "$cbdid.$v" if $k eq 'ref';
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
	my $field_index = 0;
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

	    if ($fo =~ tr/_/ / && !$e{'parts'}) {
		pp_warn("underscore (_) not allowed in form except in compounds");
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
		pp_warn("no normalization in form");
	    }

	    if ($f =~ /\s\+(\S+)/) {
		my $c = $1;
		if ($c =~ /^-(.*?)=(.*?)$/) {
		    ++$conts{$cont=$1};
		} else {
		    pp_warn("malformed CONT '$c'");
		}
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
		    pp_warn("morph2 `$morph2' has no morph1");
		}
	    } elsif ($morph && $seen_morph2) {
		my $tmp = $_;
		if (s/\s\#//g > 1) {
		    pp_warn("repeated `$morph' field (missing '#' on morph2?)");
		} else {
		    pp_warn("morph has no morph2");
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
			pp_warn("base stem $bstem not declared in \@stems");
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
		$xattr .= " stem=\"$stems{$stem}\"" if $stem;
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
	my ($sid,$sigs,$sgw,$pos,$mng) = ();
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
	if ($s =~ /^[A-Z]+(?:\/[it])?\s/) {
	    ($pos,$mng) = ($s =~ /^([A-Z]+(?:\/[it])?)\s+(.*)\s*$/);
	} else {
	    $mng = $s;
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
	if (!$mng) {
	    pp_warn("meaning not given");
	    $mng = '';
	}
	my $sgwTag = '';
	$sgwTag = "<sgw>$sgw</sgw>" if $sgw;
	my $posTag = '';
	$posTag = "<pos>$pos</pos>" if $pos;
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
	push @ret, xidify("<sense n=\"$s_sig\"$defattr>$sgwTag$posTag<mng xml:lang=\"$mnglang\">$mng</mng>");
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
		pp_warn("%lang required after \@equiv");
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
    push @ret, '</entry>';
    ++$eid;
    @ret;
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
setup_list_field {
    my($e,$hash,$field,$xhash,$xsplit) = @_;
    if ($$e{$field}) {
	warn Dumper($e) if $ORACC::L2GLO::debug && $field eq 'bases';
	my @bits = split(/;\s+/, join(' ', @{$$e{$field}}));
	warn "bits = @bits\n" if $ORACC::L2GLO::debug;
	if ($xsplit) {
	    foreach my $b (@bits) {
		my($head,$tail) = &$xsplit($b,$xhash);
		++$$hash{$head};
		push @{$$hash{'#order#'}}, $head;
		$$xhash{$head} = $tail if $tail;
	    }
	} else {
	    @{$$hash{@bits}} = ();
	}
    }
}

sub
xbase_split {
    my ($b,$xh) = @_;
    my ($pri,$alt) = ();
    my $bstar;
    if ($b =~ s/^\*(\S+)\s+//) {
	$bstar = $1;
    } else {
	$b =~ s/^\*\s*//;
    }
    if ($b =~ /\s+\(/) {
	my $tmp = $b;
	($pri,$alt) = ($b =~ /^(\S+)\s+\((.*?)\)\s*$/);
	$$xh{$pri,'*'} = $bstar;
    } else {
	$$xh{$b,'*'} = $bstar;
	$pri = $b;
	$alt = '';
    }
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
is_proper {
    $_[0] && $_[0] =~ /^[A-Z]N$/;
}

sub
render_parts {
    my $cpd = shift;
    my @ret = ();
    my @part_sigs = ();
    my $primary = 0;
    my $pindex = 0;

    my $POSrx = '[A-Za-z0-9]+(?:\/[a-z])?';

#    load_parts_map() unless $parts_map_loaded;

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
	$tmp =~ s#(/?>)# xml:id="$cbdid.$eid.$sid"$1#;
	$xids{"$tag\:$val"} = $last_xid = "$cbdid.$eid.$sid";
    } elsif ($tmp =~ /^<([a-z]+)[^>]*>(.*?)</) {
	($tag,$val) = ($1,$2);
	$tmp =~ s#(/?>)# xml:id="$cbdid.$eid.$sid"$1#;
	$last_xid = "$cbdid.$eid.$sid";
    } else {
	$tmp =~ /^<([a-z]+)/;
	($tag,$val) = ($1,'');
	$tmp =~ s#( )# xml:id="$cbdid.$eid.$sid"$1#;
	$last_xid = "$cbdid.$eid.$sid";
    }
    ++$sid;
    $tmp;
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
	    pp_warn("(stems) second norm $s is not a stem or function");
	} else {
	    $norm = $s;
	}
    }
    ($norm, [$stem,$func,$restrict]);
}

1;
