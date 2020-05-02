package ORACC::CBD::Sigs;

require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/sigs_check sigs_from_glo/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Forms;
use ORACC::CBD::Util;

my $acd_rx = '[-+>=]?';

use Data::Dumper;

use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Util;

my $COF_NONE = 0;
my $COF_HEAD = 1;
my $COF_TAIL = 2;

my $basesig = '';
my @compound_parts = ();
my %cof_lines = ();
my $coresig = '';
#my %coresigs = ();
my $currsig = '';
my $in_sense = '';
my @instsigs = ();
my $out = '';
my $lang = '';

my %pref_bases = ();

my $sigorder = 0;

my @sigs_cofs = ();
my @sigs_coresigs = ();
my @sigs_psus = ();
my @sigs_simple = ();

my %psu_indexes = ();
my @psu_parts = ();

my %sig = ();
my $sigs_glo_file = '';

my @cof_template_heads = ();
my @cof_template_tails = ();
my %cof_heads = ();
my %cof_tails = ();
my $compound = 0;
my $compound_cfgw = '';
my %compounds = ();
my $curr_cfgw = '';
my $current_first_base = '';
my $debug_type = '';
my $debug_arg = '';
my %default_senses = ();
my @externals = ();
my %externals = ();
my $found_simple_sig = 0;
my %noprintsigs = ();
my $parts_line = 0;
my %partsigs = ();
my %parts_map = ();
my %printsigs = ();
my %simple_bases = ();
my $verbose = 0;
my $cof_verbose = 0;
my $psu_verbose = 0;
my $rank = 0;
my $entrybang = '';

my $load_simple_counter = 0;

#my %simple = ();

my $ignorable = '(?:\(to be\)|a|to|the|\s)*?';
my $finalparens = '(?:\s+\(.*?\)\s*)?';

#$verbose = $psu_verbose = 1;

my $trace = 0;
my @global_cbd = ();

#my %psu_cfs = ();

my %e = ();
my $err_glo = '';
my %forms_in_entry = ();
my $glo = '';
my @parts_errors = ();
my $psu_parts = '';
my @entries_parts_lines = ();

my %validated_parts_lines = ();
my %bad_parts_lines = ();

my @no_sense_forms = ();
my @sense_forms = ();

######################################################################################

sub sigs_init_2 {
    my $basesig = '';
    @compound_parts = ();
    %cof_lines = ();
    $coresig = '';
#    %coresigs = ();
    $currsig = '';
    $in_sense = '';
    @instsigs = ();
    $out = '';
    $lang = '';

    @sigs_cofs = ();
    @sigs_coresigs = ();
    @sigs_psus = ();
    @sigs_simple = ();
    @psu_parts = ();

    %sig = ();
    $sigs_glo_file = '';

    @cof_template_heads = ();
    @cof_template_tails = ();
    %cof_heads = ();
    %cof_tails = ();
    $compound = 0;
    $compound_cfgw = '';
    %compounds = ();
    $curr_cfgw = '';
    $current_first_base = '';
    $debug_type = '';
    $debug_arg = '';
    %default_senses = ();
    @externals = ();
    %externals = ();
    $found_simple_sig = 0;
    %noprintsigs = ();
    $parts_line = 0;
    %partsigs = ();
    %parts_map = ();
    %printsigs = ();
    $verbose = 0;
    $cof_verbose = 0;
    $psu_verbose = 0;
    $rank = 0;
    $entrybang = '';

    $load_simple_counter = 0;

#    %simple = ();

    #     %psu_cfs = ();

    %psu_indexes = ();

    %e = ();
    $err_glo = '';
    $glo = '';
    @parts_errors = ();
    $psu_parts = '';
    @entries_parts_lines = ();
    
    %validated_parts_lines = ();
    %bad_parts_lines = ();
    
    @no_sense_forms = ();
    @sense_forms = ();
  
}

sub sigs_init {
    sigs_init_2();
    @sigs_cofs = ();
    @sigs_coresigs = ();
    @sigs_psus = ();
    @sigs_simple = ();
    @psu_parts = ();

    %sig = ();
    $sigs_glo_file = '';

    @cof_template_heads = ();
    @cof_template_tails = ();
    %cof_heads = ();
    %cof_tails = ();
    $compound = 0;
    $compound_cfgw = '';
    %compounds = ();
    %partsigs = ();
    %parts_map = ();
    %printsigs = ();
    @global_cbd = ();
}

my $passnumber = 0;

sub sigs_check {
    my($glo,$args,@cbd) = @_;
    ++$passnumber;
#    warn "sigs_check [$passnumber]\n";
    $lang = ORACC::CBD::Util::lang();
    sigs_init();
    sigs_simple($args,@cbd);
    sigs_cofs();
    sigs_psus($args,@cbd) unless $$args{'nopsus'};
#    my $cbdname = ORACC::CBD::Util::cbdname();
#    if ($glo) {
#	@{$$glo{'sigs'}} = @sigs_simple;
#	@{$$glo{'cofs'}} = @sigs_cofs;
#	@{$$glo{'psus'}} = @sigs_psus;
#	@{$$glo{'psu_parts'}} = @psu_parts;
#   } else {
#	warn "$0: internal error: CBD data for $cbdname not yet set\n";
#    }
}

sub sigs_from_glo {
    ++$passnumber;
#    warn "sigs_from_glo [$passnumber]\n";
    my($args,@cbd) = @_;
    $lang = ORACC::CBD::Util::lang();
    sigs_init();
    sigs_simple($args,@cbd);
    sigs_cofs();
    sigs_psus($args,@cbd) unless $$args{'nopsus'};
    sigs_dump($args);
}

######################################################################################

sub sigs_simple {
    my($args,@cbd) = @_;
    $trace = $ORACC::CBD::PPWarn::trace;
    @global_cbd = @cbd;
    my($project,$lang) = (ORACC::CBD::Util::project(),ORACC::CBD::Util::lang());

    my $nsense = 0;
    my $coresig1 = '';
    $sigorder = 0;

    my %coresigs = ();

    if (-r '00lib/preferred-bases.tab') {
	# FIXME: preferred-bases.tab should only be tried for lang=~ /^sux/, should be publically available and should index via OID
	open(P,'00lib/preferred-bases.tab') || die;
	while (<P>) {
	    chomp;
	    my($cfgw,$base) = split(/\t/,$_);
	    $pref_bases{$cfgw} = $base;
	}
	close(P);
    }
    
    for (my $i = 0; $i <= $#cbd; ++$i) {
	next if $cbd[$i] =~ /^\000$/ || $cbd[$i] =~ /^\#/;
	pp_line($i+1);

	local($_) = $cbd[$i];

#	warn "input: $_\n" if $verbose;

	if (s/^$acd_rx?\@entry\*?(!?)\s+//) {
	    $curr_cfgw = $_; $curr_cfgw =~ s/\s*$//;
	    $compound = $in_sense = 0;
	    @instsigs = ();
	    %forms_in_entry = ();
	    $basesig = undef;
	    $coresig = undef;
	    $coresig1 = undef;
	    $nsense = 0;
	    $entrybang = $1 || '';
	    @compound_parts = ();
	    $parts_line = 0;
	    $current_first_base = '';
	    %sig = ();
	    @sig{qw/cf gw pos/} = /^(.*?)\s+\[(.*?)\]\s*(\S+)\s*$/;
	    if ($sig{'cf'}) {
		$basesig = "$sig{'cf'}\[$sig{'gw'}\]$sig{'pos'}";
	    } else {
		pp_warn("(sigs) bad syntax in \@entry; entry ignored");
		do { ++$i; } 
		while ($cbd[$i] !~ /^\@end\s+entry/ && $i < $#cbd);
	    }
	} elsif (/^\@end\s+entry/) {
	    unless (defined $coresig) {
		pp_warn("(sigs) undefined coresig--bad entry\n");
		next;
	    }
	    if (!$compound && $current_first_base) {
		my $lng = ($lang =~ /^qpn/ ? $ORACC::CBD::qpn_base_lang : $lang);
		foreach my $b (split(/\s+/, $current_first_base)) {
		    my $f = $b; $f =~ tr/·°//d;
		    next if $forms_in_entry{$f};
		    my $instsig1 = "\@$project\%$lng:$f=";
		    my $xsig = "\$$sig{'cf'}/$b#~";
		    ++$noprintsigs{ "$instsig1$coresig1/$f\t0\n" };
		    $printsigs{ "$instsig1$coresig1$xsig\t0\n" } = ++$sigorder;
#		    warn "base-form $b => $instsig1$coresig1$xsig\t0\n";
		}
	    }

	    $curr_cfgw = '';
	    
	} elsif (s/^\@parts\s+//) {

	    do { ++$i; } 
	    while ($cbd[$i] !~ /^\@end\s+entry/ && $i < $#cbd);

	} elsif (/^[-+>]?\@sense(!?)/) {

	    my $sensebang = $1 || '';

	    s/\s\*\S+//; # remove stems

	    if (!$nsense && $ORACC::CBD::Forms::external) {

		my @f = ORACC::CBD::Forms::forms_by_cfgw($curr_cfgw);
		if ($#f >= 0) {
#		    warn "processing $#f+1 external forms for $curr_cfgw\n";
		    foreach my $f (@f) {
			# $$f[3] is just the form line after @form; there is
			# no provision for ! on external forms yet so the first
			# arg is always empty
			my $f3 = $$f[3];
			$f3 =~ s/^(\S+)\s*//;
			my $orth = $1;
#			warn "passing sigs_form $orth=$f3\n";
			sigs_form($args,'',$orth,$f3);
		    }
		}
	    }
		
	    ++$nsense;

	    $in_sense = 1;
	    $rank = 0;
	    @sig{qw/epos sense/} = /\s+([A-Z]\S*)\s+(.*?)\s*$/;
	    if (!$sig{'epos'}) {
		pp_warn("(sigs) no POS in SENSE");
		/\s(.*?)\s*$/;
		$sig{'sense'} = $1;
		$sig{'epos'} = $sig{'pos'};
	    }
	    
#	    if (!defined $default_senses{$basesig}) {
#		$default_senses{$basesig} = [ $sig{'sense'} , $sig{'epos'} ];
#	    }
	    
	    $coresig = "$sig{'cf'}\[$sig{'gw'}//$sig{'sense'}\]$sig{'pos'}'$sig{'epos'}";
	    $coresig1 = $coresig unless $coresig1;
	    $coresigs{$coresig} = ++$sigorder
		unless $coresigs{$coresig};

	    if ($#instsigs >= 0) {
		foreach my $instsig (@instsigs) {
		    my $instsig0 = $$instsig[0];
		    if ($instsig0 =~ s/^!//) {
			$rank = 4;
		    }
		    if ($sensebang) {
			$rank |= 2;
		    } elsif ($nsense == 1) {
			$rank |= 1;
		    }
			
		    $printsigs{ "$instsig0$coresig$$instsig[1]\t$rank\n" } = ++$sigorder;
		    $found_simple_sig = 1;
		}
	    } else {
		unless ($sig{'cf'} =~ / /) {
		    if ($current_first_base) {
			my $lng = ($lang =~ /^qpn/ ? $ORACC::CBD::qpn_base_lang : $lang);
			foreach my $b (split(/\s+/, $current_first_base)) {
			    my $f = $b; $f =~ tr/·°//d;
			    my $instsig1 = "\@$project\%$lng:$f=";
			    my $xsig = "\$$sig{'cf'}/$b#~";
			    ++$noprintsigs{ "$instsig1$coresig/$f\t0\n" };
			    $printsigs{ "$instsig1$coresig$xsig\t0\n" } = ++$sigorder;
			}
		    }
		}
	    }
	    
	} elsif (/^\@bases\s+(.*)\s*$/) {

	    $current_first_base = $1;
	    $current_first_base =~ s/\{d\}/\001/g;
	    my @bits = split(/;\s+/,$current_first_base);
	    @bits = map { s/\s+\(.*$//; s/\{-/{/g; s/\{\+.*?\}//g; s/\001/{d}/g; $_ } @bits;
	    my %bits = (); @bits { @bits } = ();
	    $current_first_base = join(' ', @bits);
	    
#	    warn "cfb=$current_first_base\n";

	    my $basekey = $curr_cfgw; $basekey =~ s/\s+\[/[/; $basekey =~ s/\]\s+/]/;
	    my $p = $pref_bases{$basekey};
	    if ($p) {
		$p =~ s/^\!//;
		$p =~ s/\@[ox].*$//;
		$p =~ s/\{\+.*?\}//g;
		if (exists $bits{$p}) {
		    $simple_bases{$basekey} = $p;
		} else {
		    my $bits0 = $bits[0] || '<no default>';
		    pp_notice("$basekey: preferred base $p unknown; defaulting to $bits0");
		    $simple_bases{$basekey} = $bits[0];
		}
	    } else {
		$simple_bases{$basekey} = $bits[0];
	    }
 
	} elsif (s/^\@form(!?)\s+(\S+)//) {

	    sigs_form($args,$1,$2,$_) unless
		$ORACC::CBD::Forms::external;

	}
    }

    @sigs_simple = sort ( { $printsigs{$a} <=> $printsigs{$b} } keys %printsigs); #, keys %noprintsigs);
    open(S,'>sigs_simple.dump'); print S @sigs_simple; close(S);
#    print Dumper \%coresigs;
    @sigs_coresigs = sort { $coresigs{$a} <=> $coresigs{$b} } keys %coresigs;
#    print Dumper \@sigs_coresigs;
}

sub sigs_form {
    my $args = shift @_;
    my $formbang = $_[0] || '';
    $sig{'form'} = $_[1];
    ++$forms_in_entry{$_[1]};
    local($_) = $_[2];
    my @cof_tails = ();
    my $cof = $COF_NONE;
    my $cof_part = '';
    my $cof_sig = '';
    my $cof_core = '';
    my $cof_template = '';

    my($project,$lang) = (ORACC::CBD::Util::project(),ORACC::CBD::Util::lang());
    
    $formbang = '!' if $entrybang;
    
    @sig{qw/norm morph morph2 base cont stem root/} = ();
    
    my $cof_nth = 0;
    
    if (s/\s+\%(\S+)//) {
	$sig{'lang'} = $1;
	$sig{'lang'} =~ s#/n$#-949# if $sig{'lang'};
#	warn "sig{lang} = $sig{'lang'}\n";
    } else {
	$sig{'lang'} = $lang;
    }
    
    if (/\$\(/) {
#	warn "COF: $_\n" if $verbose;
	
	if (/(^|\s)\$[^\(\s]+?\[/) {
	    pp_warn("(sigs) COF head should not have CF[GW]POS form; use only NORM");
	}
	
	my $cof_epos = $sig{'epos'} || $sig{'pos'};
	if ($coresig) {
	    # this happens if the COF FORM occurs within a @sense
	    $cof_core = $coresig;
	} else {
	    # this is a COF FORM before any @sense
	    $cof_core 
		= "$sig{'cf'}\[$sig{'gw'}//$sig{'gw'}\]$sig{'pos'}'$cof_epos";
	    $cof_template = "$sig{'cf'}\[$sig{'gw'}//\]$sig{'pos'}'";
	}
	
	my @cofvals = ();
	
	if (/^\s*\$\(/) {
	    $cof = $COF_TAIL;
	    s/^\s*//;
	    my @d = (m/\s+(\$\(?)/g);
	    for ($cof_nth = 2, my $i = 0; $i <= $#d; ++$i) {
		if ($d[$i] =~ /^\$\(/) {
		    ++$cof_nth;
		} else {
		    last;
		}
	    }
	    warn "COF TAIL: $_\n" if $verbose;
	} else {
	    $cof = $COF_HEAD;
	    $cof_nth = 1;
	    warn "COF HEAD: $_\n" if $verbose;
	}
	
	# get the form/sig belonging to the current word
	my $tmp = $_;
	$tmp =~ s/\$\(.*?\)//g;
	$tmp =~ s/^\s*(.*?)\s*$/$1/;
	$tmp =~ s/\s+/ /g;
	$tmp =~ s/^/ /;
	
	$_ = $cof_part = $tmp;
	
    } else {
	$cof_part = '';
    }
    s/^/ /; # so "/BASE" etc get trapped in initial position
    $sig{'norm'} = $1 if s/\s+\$(\S+)//;
    $sig{'morph2'} = $1 if s/\s+\#\#(\S+)//;
    $sig{'morph'} = $1 if s/\s+\#(\S+)//;
    $sig{'base'} = $1 if s/\s+\/(\S+)//;
    $sig{'cont'} = $1 if s/\s+\+(\S+)//;
    $sig{'stem'} = $1 if s/\s+\*(\S+)//;

#    warn "stem=$sig{'stem'}\n" if $sig{'stem'};
    
    if (s/\s\@(\S+)//) {
	my $rws = $1;
	if (defined $ORACC::L2GLO::Util::rws_map{$rws}) {
	    $sig{'lang'} = $ORACC::L2GLO::Util::rws_map{$rws};
	    if ($rws eq 'EG' 
		&& ($lang && $lang ne 'sux')) {
		$sig{'lang'} = 'sux';
	    }
	} else {
	    pp_warn("(sigs) unknown RWS code \@$rws has been ignored");
	}
    }
    
    if ($sig{'form'}) {
	my $l = ($sig{'lang'} ? $sig{'lang'} : $lang);
	my $instsig1 = "\@$project\%$l:$sig{'form'}=";
	my $instsig2 = '';
	foreach my $field (@ORACC::L2GLO::Util::instfields) {
	    $instsig2 .= "$ORACC::L2GLO::Util::fieldchars{$field}$sig{$field}" 
		if $sig{$field};
	}
	
	if ($cof == $COF_HEAD) {
	    $instsig2 .= '!0x01';
	} elsif ($cof == $COF_TAIL) {
	    $instsig2 .= sprintf("\!0x%02d", $cof_nth);
	}
	
	if ($in_sense) {
	    my $srank = $rank;
	    if ($formbang) {
		$srank |= 4;
	    }
	    $printsigs{ "$instsig1$coresig$instsig2\t$srank\n" } = ++$sigorder;
	} else {
	    $found_simple_sig = 1;
	    push @instsigs, [ $formbang.$instsig1, $instsig2 ];
	}
	
    }
}

######################################################################

sub cofs_marshall {
    my @smpl = grep(/\!0x0/, @_); chomp @smpl;
    my @extl = `grep -h -F '!0x0' 01bld/*/from_glo.sig | grep -v -F '!0x01'`; chomp @extl;
#    warn "cofs_marshall: ", Dumper(\@smpl), Dumper(\@extl), "\n===\n";
    uniq(@smpl, @extl);
}

sub sigs_cofs {
    my %cofs = ();
    my $i = 0;
    my @cof_sigs = sort { &cof_cmp; } cofs_marshall(@sigs_simple);
#    print STDERR 'marshalled cofs: ', Dumper \@cof_sigs;
    foreach my $c (@cof_sigs) {
	++$i;
	local($_) = $c;
	if (/\!0x/) {
	    chomp;
	    #	s/\t.*$//; # not sure what this was doing but it makes COF with RANK fail
	    # so move instead to not anchoring the following regex with $, this means it can
	    # have fields beyond rank without breaking anything (don't think this can happen
	    # though)
	    my($pre,$key,$sig,$nth,$rank) = (/^(.*?):(.*?)=(.*?)\!0x0*(\d+)\t(\d+)/);
	    if ($pre) {
		my $index = $nth - 1;
		my $v = '';
		my ($lang) = ($pre =~ /\%(.*?)$/);
		if ($index) {
		    $v = "$pre:=$sig";
		} else {
		    $v = "$pre:$key=$sig";
		}
		my $ckey = "$lang\:$key";
		if ($nth == 1 || $cofs{$ckey}) {
		    push @{${$cofs{$ckey}}[$index]}, $v;
		} else {
		    # this is not an error--it just means the COF head is not in the glossary
		    # currently being processed
		    # pp_warn("missing element in COF before $_");
		}
	    } else {
		pp_file('<simple sigs list>');
		pp_line($i);
		pp_warn "(cof) syntax error: $_";
	    }
	}
    }
#    print STDERR Dumper \%cofs;
    foreach my $c (keys %cofs) {
	my @parts = @{$cofs{$c}};
	permute(@parts);
#	warn Dumper \@parts;
#	warn Dumper \@sigs_cofs;
    }
}

sub cof_cmp {
    my ($aa) = ($a =~ /\!0x0(\d)/);
    my ($bb) = ($b =~ /\!0x0(\d)/);
    $aa <=> $bb;
}

# an array of arrays; the first contains the heads, the remainder
# tails.  We join the first two lists, then call recursively to 
# join additional ones.
sub
permute {
    my @parts = @_;
    my @newheads = ();
    foreach my $h (@{$parts[0]}) {
	foreach my $t (@{$parts[1]}) {
	    push @newheads, "$h\&\&$t";
	}
    }
    shift @parts; shift @parts;
    if ($#parts >= 0) {
	permute( [@newheads] , @parts);
    } else {
	foreach (@newheads) {
	    push @sigs_cofs, "$_\t0\n";
	}
    }
}

######################################################################

sub sigs_psus {
    my($args,@cbd) = @_;
    $err_glo = $$args{'file'};
    psu_index(ORACC::CBD::Util::lang());
    psu_index_coresigs();
    psu_index_simple();
    psu_glo(@cbd);
#    psu_dump() unless $$args{'check'};
}

sub psu_marshall {
    my $l = shift;
    my $p = ORACC::CBD::Util::project();
    my @core = ();
    my @smpl = ();
    my $coresigs_txt = "$ENV{'ORACC_BUILDS'}/$p/01bld/$l/coresigs.txt";
    my $from_glos = "$ENV{'ORACC_BUILDS'}/$p/01bld/$l/from_glo.sig";
    if (-r $coresigs_txt) {
	warn "psu_marshall: loading $coresigs_txt\n";
	@core = `cat $coresigs_txt`; chomp @core;
    } else {
	warn "psu_marshall: can't find $coresigs_txt\n";
	return (undef,undef);
    }
    if (-r $from_glos) {
	warn "psu_marshall: loading $from_glos\n";
	open(S,$from_glos) || die;
	while (<S>) {
	    next if /^\{/;
	    s/\t.*$//;
	    push @smpl, $_;
	}
    }
    return (\@core, \@smpl);
}

sub psu_index {
    my $l = shift;
    my %ix = ();
    $ix{'lang'} = $l;
    if ($l eq ORACC::CBD::Util::lang()) {
	%{$ix{'core'}} = psu_index_coresigs(@sigs_coresigs);
	%{$ix{'smpl'}} = psu_index_simple(@sigs_simple);
#	open(D,'>D'); print D Dumper \@sigs_coresigs; print D Dumper \@sigs_simple; print D Dumper \%ix; close(D);
	$ix{'ok'} = 'yes';
    } else {
	my($core,$smpl) = psu_marshall($l);
	if ($core) {
	    %{$ix{'core'}} = psu_index_coresigs(@$core);
	    %{$ix{'smpl'}} = psu_index_simple(@$smpl);
	    $ix{'ok'} = 'yes';
	}
    }
    $psu_indexes{$l} = { %ix };
#    print Dumper \%psu_indexes;
}

sub remove_stem {
    # B G Bt Gt Btn Gtn D ŠD Dt Dtn Š Št Štn N Nt Ntn
    my $tmp = shift;
    $tmp =~ s/\s*\*[BGDŠN][Dt]?n?//;
    $tmp;
}

sub psu_index_coresigs {
    my @input = @_;
    my %psu_cfs = ();
    foreach my $c (@input) {	
	my($cf,$gw) = ($c =~ m#^(.*?)\[(.*?)//#);
	$c =~ s/\!0x.*$//;
	$c = remove_stem($c);
	push @{${$psu_cfs{$cf}}{$gw}}, $c;
    }
#    print Dumper \%psu_cfs;
    %psu_cfs;
}

sub psu_index_simple {
    my @input = @_;
    my %simple = ();
    foreach my $s (@input) {
	local($_) = $s;
	s/\t.*$//;
	m#^.*?=(.*\'(?:V(?:/[it])?|[A-Z]+)).*$#;
	my $keysig = $1;
	if ($keysig) {
	    s#\000#//#;
	    s#\001#V/#g;
	    $keysig =~ s#\000#//#;
	    $keysig =~ s#\001#V/i#g;
	    $keysig =~ s#\002#V/t#g;
	    s/\!0x.*$//; # is this the right time to kill COF markers? Or before keysig assignment?
	    $_ = remove_stem($_);
	    push(@{$simple{$keysig}}, $_);
	} else {
	    chomp($s);
	    pp_warn "internal error: null keysig from $s";
	}
    }
    %simple;
}

sub psu_glo {
    my $i = 0;
    my $nsense = 0;
    foreach my $c (@_) {
	local($_) = $c;
	++$i; pp_line($i);
	pp_file($err_glo);
	if (m/^[-+>]?\@entry[\*!]*\s+(.*?)\s*\[(.*?)\]\s*(\S+)\s*$/) {
	    @e{qw/cf gw pos/} = ($1,$2,$3);
	    $compound = ($e{'cf'} =~ / /);
	    $in_sense = $nsense = 0;
	    @no_sense_forms = ();
	    if ($compound) {
		$curr_cfgw = "$e{'cf'} \[$e{'gw'}\] $e{'pos'}";
	    }
	} elsif (/^\@parts/) {
	    $psu_parts = $_;
	    $psu_parts =~ s/^\@parts\s+//;
	    chomp $psu_parts;
	    push @entries_parts_lines, [ pp_line()-1 , $_ ];
	} elsif (/^\@form/ && $compound) {
	    if ($in_sense) {
		my $formlang = '';
		if (s/\s\%([a-z]\S+)//) {
		    $formlang = $1 || '';
		}
		do_psu($formlang || ORACC::CBD::Util::lang(), $_);
	    } else {
		push @no_sense_forms, $_;
	    }
	} elsif (/^[-+>]?\@sense/ && $compound) {
	    s/\s\*\S+//;
	    if ($nsense == 0) {
		if ($ORACC::CBD::Forms::external) {
		    @no_sense_forms = ORACC::CBD::Forms::forms_by_cfgw($curr_cfgw);		    
		}
		if ($#no_sense_forms < 0) {
		    foreach my $epl (@entries_parts_lines) {
			# create a form from primary bases
			my $pline = $$epl[1];
			if ($pline) {
			    $pline =~ s/\@parts\s+//;
			    $pline =~ s/(\]\S*)\s+/$1\cA/g;
			    $pline =~ s/\cAn\sn\s/\cAn\cAn\cA/g;
			    $pline =~ s/\cAn\s/\cAn\cA/g;
			    $pline =~ s/^n\s+/n\cA/g;
			    my @nsf = ();
			    foreach my $p (split(/\cA/,$pline)) {
				$p =~ s#//.*?\]#]#;
				$p =~ s/\'.*//;
				$p =~ s/<.*?>$//; # remove ngram predicates in <...>
				if ($simple_bases{$p}) {
				    # warn "$p => $simple_bases{$p}\n";
				    my $f = $simple_bases{$p}; $f =~ tr/·°//d;
				    push @nsf, $f;
				} elsif ($p =~ /^n$/) {
				    push @nsf, 'n';
				} else {
				    pp_warn "$p not in simple_bases";
				}
			    }
			    push @no_sense_forms, '@form '.join('_',@nsf);
			    #		    warn "@no_sense_forms\n";
			} else {
			    if ($#entries_parts_lines
				&& $entries_parts_lines[0]
				&& ${$entries_parts_lines[0]}[1]) {
				pp_warn "no parts line info for ${$entries_parts_lines[0]}[1]";
			    } else {
				pp_warn "no parts line info in entry";
			    }
			}
		    }
		}
	    }
	    $in_sense = 1;
	    my($epos,$sense) = (/\s(\S+)\s+(.*?)\s*$/);
	    if ($epos) {
		@e{qw/epos sense/} = ($epos,$sense);
		foreach my $f (@no_sense_forms) {
		    my $formlang = '';
		    my $f3 = '';
		    if (ref($f) eq 'ARRAY') {
			$f3 = $$f[3];
		    } else {
			$f3 = $f;
		    }
		    if ($f3 =~ s/\s\%([a-z]\S+)//) {
			$formlang = $1;
		    }
#		    warn "f3=$f3\n";
		    do_psu($formlang || ORACC::CBD::Util::lang(), $f3);
		}
	    } else {
		chomp;
		pp_warn("malformed SENSE: $_");
	    }
	} elsif (/^\@end\s+entry/) {
	    $compound = 0;
	    %e = ();
	    @entries_parts_lines = ();
	} elsif (/\@entry/) {
	    pp_warn("suspicious entry $_") unless /^\@i?note/;
	}
    }
}

####################################################################

sub
do_psu {
    my($psulang,$formline) = @_;
#    warn "do_psu: psulang=$psulang; formline=$formline\n" if $verbose;
    $formline =~ s/^\@form\s+//;
#    my ($forms, $norms) = ($formline =~ /^(\S+)\s*(\S.*)\s*$/);
    my ($forms, $rest) = ($formline =~ /^(\S+)\s*(.*)\s*$/);
    unless ($forms) {
	pp_warn("syntax error, no forms in \@forms line");
	return;
    }

    my $psu_form = $forms; $psu_form =~ s/_0/ /g; $psu_form =~ tr/_/ /;
    
    my @forms = ($forms ? split(/_/,$forms) : ());
    my @rest = grep defined&&length, (split(/\s+/, $rest));
    my @norms = map { s/^\$//; $_ } grep (/^\$/, @rest);  # split(/(?:^|\s+)\$/,$norms);
    if ($#norms < 0) {
	@norms = ('*')x($#forms+1);
    }

    if ($#forms < $#norms && $#forms > 1 && $#norms > 1 && $forms[0] ne '*') {
	pp_warn("PSU $psu_parts FORM $formline: please use '_0' to indicate empty FORM elements\n");
	return;
    } elsif ($#forms < $#norms) {
	if ($forms[0] eq '*') {
	    push(@forms, '*') while $#forms < $#norms;
	} else {
	    push(@forms, '0') while $#forms < $#norms;
	}
    }

    ## This is necessary to match the parts but should not be used as the output form of a PSU;
    ## use the version computed in f2_sig.c instead
    
    # rewrite '0' entries in @forms
    my $last_non_zero_i = 0;
    for (my $i = 0; $i <= $#forms; ++$i) {
	if ($forms[$i] eq '0') {
	    $forms[$i] = $last_non_zero_i;
	} else {
	    $last_non_zero_i = $forms[$i];
	}
   }

    my $matched_parts = 0;
#    foreach my $p (@{$e{'parts'}}) {
    foreach my $p (@entries_parts_lines) {
	my ($res_p,@matches) = parts_match(\@forms, \@norms, $psulang, $p);
	if ($#matches > 0) {
	    print_psu_sig(\%e, $psu_form, $res_p, @matches);
	    ++$matched_parts;
	    last;
	}
    }
    unless ($matched_parts) {
	if ($#parts_errors >= 0) {
	    my $err = pp_line();
	    pp_line($err - 1);
	    unless ($#parts_errors == 0 && $parts_errors[0] eq '#nowarn#') {
		foreach (@parts_errors) {
#		    pp_warn("psulang=$psulang; formline=$formline");
		    pp_warn($_)
		}
	    }
	} else {
	    my($xcfgw) = "$e{'cf'} \[$e{'gw'}\] $e{'pos'}";
	    unless (ORACC::CBD::Validate::is_bad_compound($xcfgw)) {
		warn '=== at ',pp_file(),':',pp_line(),"\n";
		warn Dumper \%e;
		warn "====\n";
		pp_warn("unknown parts processing failure");
	    }
	}
    }
    @parts_errors = ();
}

sub find_in_coresigs_sub {
    my($cf,$xgw,$l) = @_;    
    psu_index($l) unless $psu_indexes{$l};
    if ($psu_indexes{$l}) {
	my $ix = $psu_indexes{$l};
	if ($$ix{'ok'}) {
	    my %psu_cfs = %{$$ix{'core'}};
	    if (defined ${$psu_cfs{$cf}}{$xgw}) {
		$$ix{'sig'} = $xgw;
		return $ix;
	    } elsif (defined $psu_cfs{$cf}) {
		foreach my $gw (keys %{$psu_cfs{$cf}}) {
		    my @sigs = @{${$psu_cfs{$cf}}{$gw}};
		    foreach my $s (@sigs) {
			my $qxgw = quotemeta($xgw);
			if ($s =~ m#//[^\]]*$qxgw#) {
			    # warn "matched $cf\[$xgw] in $s\n";
			    $$ix{'sig'} = $s;
			    return $ix;
			}
		    }
		}
	    }
	}
    }
    undef;
}

sub find_in_coresigs {
    my($cf,$xgw,$l) = @_;    
    $l =~ s/-\d+$//;
    $l =~ s#/n##;
    my $res = find_in_coresigs_sub($cf,$xgw,$l);
    return $res if $res;
    my $this_l = ORACC::CBD::Util::lang();
    if ($l ne $this_l) {
	my $res = find_in_coresigs_sub($cf,$xgw,$this_l);
	return $res if $res;
    }
    if ($l ne 'qpn') {
	my $res = find_in_coresigs_sub($cf,$xgw,'qpn');
	return $res if $res;
    }
    undef;
}

sub
match_sense {
    my($sense,@sigs) = @_;
    my $qs = quotemeta($sense);
    my @m = grep(m#//.*?$qs.*?\]#, @sigs);
    if ($#m < 0) {
	@m = grep(m#\[.*?$qs.*?//#, @sigs);
    }
    if ($#m > 0) {
	my %s = ();
	foreach my $s (@m) {
	    if ($s eq 'n') {
		++$s{'n'};
	    } else {
		my $tmp = $s;
		$tmp =~ s#^.*?//(.*?)\].*$#$1#;
		++$s{$tmp};
	    }
	}
	if (scalar keys %s > 1) {
	    my @new_m = ();
	    foreach my $s (@m) {
		if ($s =~ m#//$qs\]# || $s =~ m#\[$qs//#) {
		    push @new_m, $s;
		}
	    }
	    if ($#m < 0) {
		pp_warn("$sense matches more than one sense: ", join(', ', keys %s));
	    } else {
		@m = @new_m;
	    }
	}
    }
    @m;
}

sub
parts_match {
    my ($forms_ref, $norms_ref, $psulang, $parts_line_ref) = @_;
    my @forms = @$forms_ref;
    my @norms = @$norms_ref;
    my @parts_data = ();
    my $partskey = "$psulang\:$$parts_line_ref[1]";

    my $parts_ref = $validated_parts_lines{$partskey};
    if (!$parts_ref) {
	if ($bad_parts_lines{$partskey}) {
	    @parts_errors = ('#nowarn#');
	    return ();
	}
	if ((@parts_data = validate_parts($psulang, @$parts_line_ref))) {
	    $validated_parts_lines{$partskey} = [ @parts_data ];
	} else {
	    ++$bad_parts_lines{$partskey};
	    @parts_errors = ('#nowarn#');
	    return ();
	}	
    } else {
	@parts_data = @$parts_ref;
    }

#    print Dumper \@parts_data;
    
    if ($#forms > $#parts_data) {
	push @parts_errors, "`@forms' has too many forms for $psu_parts";
	return ();
    }

    if ($#norms != $#parts_data) {
	my $nnorms = $#parts_data + 1;
	push @parts_errors, "`@norms' should have $nnorms members; missing '\$' in \@form line?";
	return ();
    }

    my $matched = 1;
    my @matched_candidates = ();
    # Iterate over the parts of the compound, i.e., @forms/@norms/@parts 
    # and look for a FORM/NORM match--we already know the @candidates match
    # as far as SENSE.  Just stop at the first match and take that; we 
    # don't care about ambiguity because there shouldn't be any.
    for (my $i = 0; $i <= $#forms; ++$i) {
	if (!$parts_data[$i]) {
	    push @parts_errors, "PSU $psu_parts: no parts data for form $forms[$i]"
		unless $#parts_errors == 0;
	    return ();
	}
	my ($pt, $csig, @candidates) = @{$parts_data[$i]}; chomp @candidates;
	my $this_form_matched = 0;
	my @tmp_matches = ();
	my $pass_1 = 1;
      match_pass_1:
	{
	    for (my $j = 0; $j <= $#candidates; ++$j) {
		my($form,$norm) = ();
		if ($candidates[$j] =~ /\$/) {
		    #		    ($form,$norm) = ($candidates[$j] =~ m#:(.*?)=.*?\$(.*?)(?:$|[/+\#\@\*])#);
		    ($form,$norm) = ($candidates[$j] =~ m#:(.*?)=.*?\$(.*?)(?:$|[/+\#\@])#); # no STEM here
		} else {
		    $candidates[$j] =~ m#:(.*?)=#;
		    ($form,$norm) = ($1,'*');
		}
		#		warn "form=$form; norm=$norm; form[i]=$forms[$i]; norm[i]=$norms[$i]\n";
		if ($form eq 'n') {
		    $this_form_matched = 1;
		    $matched_candidates[$i] = $candidates[0];
		    last match_pass_1;		    
		} elsif ($form && $form eq $forms[$i] 
		    && ($norm eq '*' || $norms[$i] eq '*' || 
		    $norm eq $norms[$i])) {
		    if ($pass_1) {
			if ($candidates[$j] =~ /\%$psulang\:/) {
			    warn "pass_1_matched $form eq $forms[$i]/$norm eq $norms[$i] in $candidates[$j]\n"
				if $verbose;
			    $this_form_matched = 1;
			    $matched_candidates[$i] = $candidates[$j];
			    last match_pass_1;
			}
		    } else {
			warn "pass_2_matched $form eq $forms[$i]/$norm eq $norms[$i] in $candidates[$j]\n"
			    if $verbose;
			$this_form_matched = 1;
			$matched_candidates[$i] = $candidates[$j];
			last match_pass_1;
		    }
		}
	    }
	    if ($pass_1) {
		$pass_1 = 0;
		goto match_pass_1;
	    }
	}

	unless ($this_form_matched) {
	    $matched = 0;
	    unless ($#parts_errors == 0) {
		push @parts_errors, "no form/norm match on '$forms[$i]' = simple sig '$csig\$$norms[$i]' in `$psulang.glo'";
#		print STDERR Dumper \@candidates;
	    }
	    last;
	}
    }
#    use Data::Dumper; print "matched_candidates[$passnumber]: ", Dumper \@matched_candidates;
    return ( [@parts_data] , @matched_candidates );
}

sub
print_psu_sig {
    my ($eref, $psuform, $res_ref, @sigs) = @_;
    my $glolang = ORACC::CBD::Util::lang();
    my @p = ();
    foreach my $r (@$res_ref) {
	push @p, $$r[0];
    }
    # rewrite the form to make sure we get any necessary language switches into it
    my %f = ();
    foreach my $s (@sigs) {
	my($flang,$form) = ($s =~ m/\%(.*?):(.*?)=/);
	if ($flang && $flang ne $glolang) {
	    $f{$form} = $flang;
	}
    }
    my @f = ();
    my $curr_lang = $glolang;
    foreach my $p (split(/\s+/, $psuform)) {
	if ($f{$p}) {
	    if ($curr_lang ne $f{$p}) {
		push @f, "\%$f{$p} $p";
		$curr_lang = $f{$p};
	    } else {
		push @f, $p;
	    }
	} else {
	    if ($curr_lang ne $glolang) {
		push @f, "\%$glolang $p";
		$curr_lang = $glolang;
	    } else {
		push @f, $p;
	    }
	}
    }
    $psuform = join(' ', @f);
    my $psusig = join('++', @sigs);
#    $psuform =~ s/\s*$//;
    push (@sigs_psus, 
	  "{$psuform = @p += $$eref{'cf'}\[$$eref{'gw'}//$$eref{'sense'}\]$$eref{'pos'}'$$eref{'epos'}}"
	  .'::'.$psusig."\t0\n");
    push (@psu_parts, "@p\t".$psusig."\n");
}

sub
validate_parts {
    my($psulang, $lnum, $p) = @_;
    my $status = 0;
    $p =~ s/^\@parts\s+//;
    $p =~ s/\s[\%\#\@\/\+]\S+/ /g;
    $p =~ s/\s+/ /g;
    $p =~ s/(\]\S*)\s+/$1\cA/g;
    $p =~ s/\cAn\sn\s/\cAn\cAn\cA/g;
    $p =~ s/\cAn\s+/\cAn\cA/g;
    $p =~ s/^n\s+/n\cA/g;
    my @parts = grep defined&&length, split(/\cA/,$p);
    my @ret = ();
    foreach my $pt (@parts) {
	my($cf,$gw) = ($pt =~ /^(.*?)\[(.*?)(?:\/\/|\])/);
	my($pos,$epos) = ('','');
	my $sense = '';
	my $ix = undef;
	if ($cf && $gw && ($ix = find_in_coresigs($cf,$gw,$psulang))) {
	    my $gw = $$ix{'sig'};
	    my %psu_cfs = %{$$ix{'core'}};
	    my %simple = %{$$ix{'smpl'}};
	    if ($gw =~ /\[/) { # the return was a matched coresig, not a simple GW
		my($xgw,$xsense,$xpos,$xepos) = ($gw =~ m#^.*?\[(.*?)//(.*?)\](.*?)'(.*?)#);
		# Ensure the part has all elements of a coresig
		($gw,$sense,$pos,$epos) = ($xgw,$xsense,$xpos,$xepos);
	    } else {
		if ($pt =~ /\](.*?)'([A-Za-z\/]+)/) {
		    ($pos,$epos) = ($1,$2);
		} else {
		    $pt =~ /\]([A-Za-z\/]+)/;
		    $pos = $1;
		}
	    }

	    # this is a list of coresigs that match the psu part
	    my @pt_matches = @{${$psu_cfs{$cf}}{$gw}};
	    if ($epos) {
		pp_line($lnum);
		pp_warn("if you give EPOS in \@parts you must also give POS")
		    unless $pos;
		my @pos_matches = grep(/\]$pos'$epos/, @pt_matches);
		if ($#pos_matches < 0) {
		    pp_line($lnum);
		    pp_warn("$pt has wrong POS or EPOS in `$psulang.glo'");
		} else {
		    @pt_matches = @pos_matches;
		}
	    } else {
		my $qpos = quotemeta($pos);
		my @pos_matches = grep(/\]$qpos\'/, @pt_matches);
		if ($#pos_matches < 0) {
		    @pos_matches = grep /\].*?\'$qpos$/, @pt_matches;
		    if ($#pos_matches < 0) {
			pp_line($lnum);
			pp_warn("$pt has wrong POS in `$psulang.glo'");
		    } else {
			$epos = $pos;
			$pos_matches[0] =~ /\](.*?)'/;
			$pos = $1;
		    }
		} else {
		    @pt_matches = @pos_matches;
		}
	    }
	    if ($pt =~ m#//(.*?)\]#) {
		$sense = $1;
#	    } else {
#		$sense = $gw;
	    }
	    my @sense_matches = ();
	    if ($sense) {
		@sense_matches = match_sense($sense, @pt_matches);
		if ($#sense_matches < 0) {
		    pp_warn("$pt should match a SENSE of $cf\[$gw\] in `$psulang.glo' but doesn't");
		} else {
		    @pt_matches = @sense_matches;
		}
	    } else {
		@sense_matches = @pt_matches;
	    }
	    
	    # Now create the canonical base sig for the part; we require 
	    # at least CF[GW] so we may have to supply SENSE/POS/EPOS

	    unless ($sense && $pos && $epos) {
#		warn "pt_matches[0] = $pt_matches[0]\n";
		$pt_matches[0] =~ m#//(.*?)\](.*?)'([A-Za-z/]+)#;
		$sense = $1 unless $sense;
		$pos = $2 unless $pos;
		$epos = $3 unless $epos;
	    }
	    $epos = $pos unless $epos;

	    my $csig = "$cf\[$gw//$sense\]$pos'$epos";
#	    warn "csig = $csig\n";
	    my @simple_matches = ();
	    foreach my $ptm (@pt_matches) {
#		if ($ptm =~ /teŋ/) {
#		    warn "teŋ $ptm\n";
#		}
		if ($ptm && $simple{$ptm}) {
		    push(@simple_matches, @{$simple{$ptm}});
#		    print "simple_matches[$passnumber]: ", Dumper \@simple_matches;
		} else {
		    if ($ptm) {
			warn "l2p1-psus.plx: no match for $ptm in 01tmp/l2p1-simple.sig\n";
		    } else {
			chomp;
			warn "01tmp/l2p1-simple.sig:$.: l2p1-psus.plx: undefined part in $_\n";
		    }
		}
	    }
	    push @ret, [ $pt , $csig , @simple_matches ];
	} elsif ($pt =~ /^n$/) {
	    my($project,$lang) = (ORACC::CBD::Util::project(),ORACC::CBD::Util::lang());
	    push @ret, [ $pt , "n[n//n]NU'NU", "\@$project%$lang:n=n[n//n]NU'NU" ];
	} else {
	    pp_line($lnum+1);
	    pp_warn("$pt does not match a known CF[GW] in `$psulang.glo'");
	    $status = 1;
	}
    }
    if ($status) {
	();
    } else {
	@ret;
    }
}

sub psu_dump {
    open(P, ">01bld/$lang/parts.map") || die "psu_dump failed to open 01bld/$lang/parts.map\n";
    print P @psu_parts;
    close(P);
}

######################################################################

sub sigs_dump {
    my($args) = @_;
    if ($$args{'stdout'}) {
	*SIGS = *STDOUT;
    } else {
	$sigs_glo_file = "01bld/$$args{'lang'}/from_glo.sig";
	open(SIGS, ">$sigs_glo_file") || die "sigs_dump failed to open $sigs_glo_file\n";
    }
#    my $cbdname = ORACC::CBD::Util::cbdname();
    #    my %g = %{$ORACC::CBD::data{$cbdname}};
#    print STDERR Dumper \@sigs_cofs;
    pp_diagnostics();
    print SIGS "\@fields sig rank\n";
    print SIGS uniq(@sigs_simple);
    print SIGS uniq(@sigs_cofs);
    print SIGS uniq(@sigs_psus) unless $$args{'nopsus'};
#    print SIGS @{$g{'sigs'}};
#    print SIGS @{$g{'cofs'}};
#    print SIGS @{$g{'psus'}};
    close(SIGS);

#    warn "cbdpp: sigs written to $sigs_glo_file\n"
#	if $$args{'announce'};
    unless ($$args{'stdout'}) {
	open(CORESIGS, ">01bld/$$args{'lang'}/coresigs.txt");
	print CORESIGS join("\n", uniq(@sigs_coresigs)), "\n";
	close(CORESIGS);
    }
}

# slower uniq with stable order w/r/t incoming array
sub uniq  {
    my %u = ();
    my @u = ();
    foreach my $s (@_) {
	my $s1 = $s; $s1 =~ s/\t.*$//;
	push @u, $s unless $u{$s1}++;
    }
    @u;
}
    
1;
