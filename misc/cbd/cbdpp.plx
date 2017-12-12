#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
use Getopt::Long;

my $check = 0;
my $ngm = 0;
my $trace = 0;
my $vfields = '';

GetOptions(
    'check'=>\$check,
    'ngm'=>\$ngm,
    'trace'=>\$trace,
    'validate:s'=>\$vfields,
    );

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

my @bases_atf = ();
my $curr_ln = -1;

my @funcs = qw/free impf perf Pl PlObj PlSubj Sg SgObj SgSubj/;
my %funcs = (); @funcs{@funcs} = ();

my @poss = qw/AJ AV N V DP IP PP CNJ J MA O QP RP DET PRP POS PRT PSP
    SBJ NP M MOD REL XP NU AN BN CN DN EN FN GN HN IN JN KN LN MN NN
    ON PN QN PNF RN SN TN U UN VN WN X XN YN ZN/; 

push @poss, ('V/t', 'V/i'); 
my %poss = (); @poss{@poss} = ();

my @stems = qw/B rr RR rR Rr rrr RRR rrrr RRRR S₁ S₂ S₃ S₄/;
my %stems = (); @stems{@stems} = ();

my @tags = qw/entry parts bff bases stems phon root form length norms
              sense equiv inote prop end isslp bib defn note pl_coord
              pl_id pl_uid ngm was moved project lang name/;

my %tags = (); @tags{@tags} = ();

my $lng = '';
my $cbd = shift @ARGV;
if ($cbd) {
    $lng = $cbd; $lng =~ s/\.glo$//; $lng =~ s#.*?/([^/]+)$#$1#;
} else {
    die "cbdpp.plx: must give glossary on command line\n";
}

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
my $seen_bases = 0;
my %seen_forms = ();
my $seen_morph2 = 0;
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
# Program functions
#
###############################################################

cbd_load();
cbd_validate();

dump_ngm() if $#ngm >= 0;
dump_acd() if $#acd >= 0; 
dump_cbd();

#######################################################################

sub cbd_validate {
    for (my $i = 0; $i <= $#cbd; ++$i) {
	next if $cbd[$i] =~ /^\000$/ || $cbd[$i] =~ /^\#/;
	$curr_ln = $i+1;
	if ($cbd[$i] =~ /^\s*$/) {
	    ppwarn("blank lines not allowed in \@entry")
		if $in_entry;
	} elsif ($cbd[$i] =~ /^\@([A-Z]+)\s*$/) {
	    my $rws = $1;
	    ppwarn("\@$1 unknown register/writing-system/dialect")
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
		    ppwarn("internal error: no validator function defined for tag `$tag'");
		}
	    } else {
		ppwarn("\@$1 unknown tag");
	    }
	} else {
	    ppwarn("invalid line in glossary");
	}
    }
}

sub dump_acd {
    
}

sub dump_cbd {
    if ($init_acd) {
	open(C, '>01bld/$lng.glo');
	foreach (@cbd) {
	    print C $_, "\n";
	}
	close(C);
    }
}

sub dump_ngm {
    open(N, '>01bld/$lng.ngm');
    foreach (@ngm) {
	print N $_, "\n";
    }
    close(N);
}

#################################################
#
#  VALIDATION
#
#################################################

sub v_project { 
    my($tag,$arg) = @_;
};

sub v_lang {
    my($tag,$arg) = @_;
    if ($trace && exists $arg_vfields{'lang'}) {
	warn "v_lang: tag=$tag; arg=$arg\n";
    }
    if ($arg =~ /(\S+)/) {
	$cbdlang = $1;
    }
};

sub v_name { 
    my($tag,$arg) = @_;
};

sub v_entry {
    my($tag,$arg) = @_;
    if ($trace && exists $arg_vfields{'entry'}) {
	warn("$curr_ln: v_entry: tag=$tag; arg=$arg\n");
    }
    my($pre,$etag,$pst) = ($tag =~ /^($acd_rx)?\@(\S+?)(\*?\!?)$/);
    my ($cf,$gw,$pos) = ();
    if ($etag && $etag eq 'entry') {
	$pre = '' unless $pre;
	$pst = '' unless $pst;
	if ($in_entry) {
	    if ($pre) {
		if ($in_entry > 1) {
		    ppwarn("multiple acd \@entry fields not permitted");
		}
	    } else {
		ppwarn("multiple \@entry fields not permitted");
	    }
	    ++$in_entry;
	} elsif ($in_entry > 1 ) {
	    ppwarn("max two \@entry fields allowed");
	} else {
	    ++$in_entry;
	    ($cf,$gw,$pos) = ($arg =~ /^([^\[]+)\s+(\[[^\]]+\])\s+(\S+)\s*$/);
	    if (!$cf) {
		ppwarn("syntax error in \@entry's CF [GW] POS");
	    } else {
		$is_compound = $cf =~ /\s/;
	    }
	}
	if ($pre) {
	    v_acd_ok($pre);
	}
	if ($pst) {
	    if ($pst !~ /^\*|\!$/) {
		ppwarn("bad \@entry suffix: allowed sequences are '*', '!'");
	    }
	}
	if ($trace && exists $arg_vfields{'entry'}) {
	    $cf = '' unless $cf;
	    $gw = '' unless $gw;
	    $pos = '' unless $pos;
	    warn "entry: cf=$cf; gw=$gw; pos=$pos; pre=$pre, pst=$pst\n";
	}
    } else {
	ppwarn("bad format in \@entry");
    }
}

sub v_acd_ok {
    my $pre = shift;
    if ($pre !~ /^$acd_rx$/) {
	ppwarn("(acd) only $acd_rx allowed");
    } else {
	if (length($pre) > 1) {
	    ppwarn("(acd) only one of $acd_rx allowed");
	}		 
    }
}

sub v_bases {
    my($tag,$arg) = @_;
    if ($trace && exists $arg_vfields{'bases'}) {
	warn "v_bases: tag=$tag; arg=$arg\n";
    }
    my @bits = split(/;\s+/, $arg);
    if ($trace && exists $arg_vfields{'bases'}) {
	warn "v_bases: \@bits=@bits\n";
    }

    if ($seen_bases++) {
	ppwarn("\@bases can only be given once");
	return;
    }
    
    my $alt = '';
    my $stem = '';
    my $pri = '';
    my $bases_atf = '';
    foreach my $b (@bits) {
	if ($b =~ s/^\*(\S+)\s+//) {
	    $stem = $1;
	} elsif ($b =~ /^\*/) {
	    $b =~ s/^\*\s*//;
	    ppwarn("misplaced '*' in \@bases");
	}
	if ($b =~ /\s+\(/) {
	    my $tmp = $b;
	    ppwarn("malformed alt-base in `$b'")
		if ($tmp =~ tr/()// % 2);
	    ($pri,$alt) = ($b =~ /^(\S+)\s+\((.*?)\)\s*$/);
	    if ($pri =~ /\s/) {
		ppwarn("space in base `$pri'")
	    } else {
		++$bases{$pri};
		$bases{$pri,'*'} = $stem
		    if $stem;
	    }
	    $bases_atf .= " $pri ";
	    foreach my $t (split(/,\s+/,$alt)) {
		if ($t =~ /\s/) {
		    ppwarn("space in alt-base `$t'");
		} else {
		    $bases_atf .= "$t ";
		}
	    }
	} else {
	    if ($b =~ /\s/) {
		ppwarn("space in base `$b'");
		$pri = $alt = '';
	    } else {
		++$bases{$b};
		$bases{$b,'*'} = $stem
		    if $stem;
		$bases_atf .= "$b ";
		$pri = $b;
		$alt = '';
	    }
	}
    }
    if ($trace && exists $arg_vfields{'bases'}) {
	warn "v_bases: dump of \%bases:\n";
	warn Dumper \%bases;
    }
    push @bases_atf, "$curr_ln. $bases_atf\n";
}

sub v_form {
    my($tag,$arg) = @_;

    $arg = '' unless $arg;
    
    if ($trace) {
	warn "v_form: tag=$tag; arg='$arg'; cbdlang=$cbdlang\n";
    }
    
    unless ($arg) {
	ppwarn("empty \@form");
	return;
    }
    if ($arg =~ /^[\%\$\#\@\+\/\*]/) {
	ppwarn("\@form must begin with writing of form");
	return;
    }
    
    my $barecheck = $arg;
    $barecheck =~ s/^(\S+)\s*//;
    my $formform = $1;

    if ($formform =~ /[áéíúàèìùÁÉÍÚÀÈÌÙ]/) {
	ppwarn("accented vowels not allowed in \@form");
    }

    if ($formform =~ /[<>]/) {
	ppwarn("angle brackets not allowed in \@form");
    }

    my $f = $arg;
    my $flang = '';
    if ($f =~ s/(?:^|\s+)\%(\S+)//) {
	$flang = $1;
	$f =~ s/^\s*//;
    } elsif ($cbdlang =~ /^qpn/) {
	ppwarn("no %LANG in QPN glossary \@form entry");
    }

    my($fo) = ($f =~ /^(\S+)/);
    if ($seen_forms{$fo,$flang}++) {
	ppwarn("duplicate form: $fo");
	return;
    }

    if ($fo =~ tr/_/ / && !$is_compound) {
	ppwarn("underscore (_) not allowed in form except in compounds");
    }
    
    if (($cbdlang =~ /^akk/ 
	 || ($cbdlang =~ /^qpn/ && $flang =~ /akk/))) {
	ppwarn("no normalization in form")
	    unless $f =~ m#(?:^|\s)\$\S#;
    }

    if (($cbdlang =~ /^sux/ 
	 || ($cbdlang =~ /^qpn/ && $flang =~ /sux/))
	&& !$is_compound) {
	ppwarn("no BASE entry in form")
	    unless $f =~ m#(?:^|\s)/\S#;
    }

    if ($f =~ /\s\+(\S+)/) {
	my $c = $1;
	ppwarn("malformed CONT '$c'")
	    unless $c =~ /^-(.*?)=(.*?)$/;
    }

    my $morph = '';
    if ($f =~ /\s\#([^\#]\S*)/) {
	$morph = $1;
    }
    if ($f =~ /\s\#\#(\S+)/) {
	++$seen_morph2;
	my $morph2 = $1;
	ppwarn("morph2 `$morph2' has no morph1")
	    unless $morph;
    } elsif ($morph && $seen_morph2) {
	if ($f =~ s/\s\#//g > 1) {
	    ppwarn("repeated `$morph' field (missing '#' on morph2?)");
	} else {
	    ppwarn("morph has no morph2")
		unless $mixed_morph;
	}
    }
    
    1 while $barecheck =~ s#(^|\s)[\%\$\#\@\+\/\*!]\S+#$1#g;

    if ($barecheck =~ /\S/) {
	ppwarn("bare word in \@form. barecheck=$barecheck; arg=$arg");
    } else {
	my $tmp = $arg;
	$tmp =~ s#\s/(\S+)##; # remove BASE because it may contain '$'s.
	$tmp =~ s/^\S+\s+//; # remove FORM because it may contain '$'s.
	my $ndoll = 0;
	if (($ndoll = ($tmp =~ tr/$/$/)) > 1 
	    && !$is_compound) {
	    my $nparen = ($tmp =~ s/\$\(//g);
	    if ($trace) {
		warn "v_form COF: ndoll=$ndoll; nparen=$nparen\n";
	    }
	    if ($ndoll - $nparen > 1) {
		ppwarn("COFs must have only one NORM without parens (found more than 1)");
	    } elsif ($ndoll == $nparen) {
		ppwarn("COFs must have one NORM without parens (found none)");
	    }
	}
    }
}

sub v_parts {
    my($tag,$arg) = @_;
    $_[0];
}

sub v_sense {
    my($tag,$arg) = @_;
#    if ($s =~ s/^(\S+)\s+//) {
#	$sid = $1;
#	if ($sid =~ s/!$//) {
#	    $defattr = ' default="yes"';
#	}
#	$sigs = $sigs{$sid};
#    }

    if ($arg =~ s/^\[(.*?)\]\s+//) {
#	$sgw = $1;
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
		ppwarn("$pos not in known POS list");
	    } else {
		$mng = "$pos $mng";
	    }
	}
    } else {
	$pos = '';
    }
    if (!$mng) {
	ppwarn("no content in SENSE");
	$mng = '';
    }
    if ($arg =~ tr/[]//d) {
	ppwarn("square brackets not allowed in SENSE; use Unicode U+27E6/U+27E7 instead");
    }
    if ($arg =~ tr/;//d) {
	ppwarn("semi-colons not allowed in SENSE; use comma or split into multiple SENSEs");
    }
    my($tok1) = ($arg =~ /^(\S+)/);
    if (!$tok1) {
	ppwarn("empty SENSE");
    } else {
	ppwarn("$tok1: unknown POS in SENSE") unless exists $poss{$tok1};
	ppwarn("no content in SENSE") unless $arg =~ /\s\S/;
    }
    
    $_[0];
}

sub v_bff {
    my($tag,$arg) = @_;
    my($class,$code,$label,$link,$target) = ();
    $arg =~ s/\s*$//;
    if ($arg =~ /^["<]/) {
	ppwarn("missing CLASS in \@bff");
	return {
	    curr_id=>$curr_ln-1,
	    line=>$.,
	    link=>''
	};
    } else {
	($arg =~ s/^(\S+)\s*//) && ($class = $1);
#	unless ($bff_class{$class}) {
#	    ppwarn( "unknown bff CLASS: $class\n");
#	}
	if ($arg !~ /^["<]/) {
	    ($arg =~ s/^(\S+)\s*//) && ($code = $1);
	}
	if ($arg =~ /^"/) {
	    ($arg =~ s/^"(.*?)\"\s+//) && ($label = $1);
	}
	if ($arg =~ /<[^>]*$/) {
	    ppwarn("missing close '>' on bff link");
	    return;
	}
	if ($arg =~ /^[^<]*$/) {
	    ppwarn("missing open '<' on bff link");
	    return;
	}
	($arg =~ s/\s*<(.*?)>\s*$//) && ($link = $1);
	if ($arg) {
	    #	    ppwarn("bff is CLASS CODE \"LABEL\" <LINK> where CODE and \"LABEL\" are optional");
	    ppwarn("bff leftovers=$arg (out of order components?)");
	}
	return {
	    bid=>$bid++,
	    class=>$class,
	    code=>$code,
	    label=>$label,
	    link=>$link,
	    line=>$curr_ln,
	    ref=>$curr_ln-1,
	};
    }
}

sub v_bib {
    my($tag,$arg) = @_;
}

sub v_isslp {
    my($tag,$arg) = @_;
}

sub v_equiv {
    my($tag,$arg) = @_;
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

sub v_end {
    my($tag,$arg) = @_;
    ppwarn("malformed \@end entry")
	unless $arg =~ /^\s*entry\s*$/;
    $in_entry = $seen_bases = 0;
    %bases = ();
    %seen_forms = ();
}

sub v_deprecated {
    ppwarn("$_[0] is deprecated, please remove from glossary");
}

sub is_proper {
    $_[0] && $_[0] =~ /^[A-Z]N$/;
}

sub check_base {
    my($b,%b) = @_;
    my @b = keys %b;
    unless ($b{$b}) {
	bad('form', "unknown base `$b'");
	0;
    }
    1;
}

################################################
#
# Utility routines
#
################################################

sub cbd_load {
    open(C,$cbd) || die "cbdpp.plx: unable to open $cbd. Stop.\n";
    @cbd = (<C>); chomp @cbd;
    close(C);

    my $insert = -1;
    for (my $i = 0; $i <= $#cbd; ++$i) {
	if ($cbd[$i] =~ /^$acd_rx?\@([a-z]+)/) {
	    my $tag = $1;
	    if ($tag ne 'end') {
		$insert = $i;
	    } else {
		$insert = -1;
	    }
	} elsif ($cbd[$i] =~ s/^\s+(\S)/ $1/) {
	    if ($insert >= 0) {
		$cbd[$insert] .= $cbd[$i];
		$cbd[$i] = '\000';
	    } else {
		ppwarn("indented lines only allowed within \@entry");
	    }
	}
    }
}

sub ppwarn {
    warn "$cbd:$curr_ln: ", @_, "\n";
}

1;
