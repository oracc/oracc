package ORACC::CBD::Sigs;

require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/sigs_simple/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Forms;

use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Util;

my $COF_NONE = 0;
my $COF_HEAD = 1;
my $COF_TAIL = 2;

my $basesig = '';
my @compound_parts = ();
my %cof_lines = ();
my $coresig = '';
my %coresigs = ();
my $currsig = '';
my $in_sense = '';
my @instsigs = ();
my $out = '';
my $lang = '';
my $project = `oraccopt`;
my @sigs_simple = ();
my $sigs_simple_file = '';
my %sig = ();
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
my $simple = 0;
my $verbose = 0;
my $cof_verbose = 0;
my $psu_verbose = 0;
my $rank = 0;
my $entrybang = '';

my $load_simple_counter = 0;

my $simple_loaded = 0;
my %simple;

my $ignorable = '(?:\(to be\)|a|to|the|\s)*?';
my $finalparens = '(?:\s+\(.*?\)\s*)?';

$verbose = $psu_verbose = 1;

my %data = ();
my $trace = 0;
my @global_cbd = ();

######################################################################################

sub sigs_simple {
    my($args,@cbd) = @_;
    %data = %ORACC::CBD::Util::data;
    $trace = $ORACC::CBD::PPWarn::trace;
    @global_cbd = @cbd;
    ($project,$lang) = @$args{qw/project lang/};

    my $nsense = 0;
    
    for (my $i = 0; $i <= $#cbd; ++$i) {
	next if $cbd[$i] =~ /^\000$/ || $cbd[$i] =~ /^\#/;
	pp_line($i+1);

	$_ = $cbd[$i];

#	warn "input: $_\n" if $verbose;

	if (s/^\@entry\*?(!?)\s+//) {
	    $curr_cfgw = $_; $curr_cfgw =~ s/\s*$//;
	    $compound = $in_sense = 0;
	    @instsigs = ();
	    $basesig = undef;
	    $coresig = undef;
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
	    #
	    # NOT GOOD ENOUGH: should emit sig for each primary base, and should
	    # check sigs to see if they already included it, supplying it if not.
	    #
	    if (!$compound && $simple && !$found_simple_sig && $current_first_base) {
		my $lang = ($lang =~ /^qpn/ ? 'sux' : $lang);
		my $instsig1 = "\@$project'}\%$lang:$current_first_base=";
		my $xsig = "\$$sig{'cf'}/$current_first_base#~";
		++$noprintsigs{ "$instsig1$coresig\t0\n" };
	    }

	    $curr_cfgw = '';
	    
	} elsif (s/^\@parts\s+//) {

	    do { ++$i; } 
	    while ($cbd[$i] !~ /^\@end\s+entry/ && $i < $#cbd);

	} elsif (/^\@sense(!?)/) {

	    my $sensebang = $1 || '';

	    if (!$nsense && $ORACC::CBD::Forms::external) {

		my @f = forms_by_cfgw($curr_cfgw);
		if ($#f >= 0) {
#		    warn "processing $#f+1 external forms for $curr_cfgw\n";
		    foreach my $f (@f) {
			# $$f[3] is just the form line after @form; there is
			# no provision for ! on external forms yet so the first
			# arg is always empty
			my $f3 = $$f[3];
			$f3 =~ s/^(\S+)\s*//;
			my $orth = $1;
			sigs_form('',$orth,$f3);
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
	    ++$coresigs{$coresig};

	    if ($#instsigs >= 0) {
		foreach my $instsig (@instsigs) {
		    if ($$instsig[0] =~ s/^!//) {
			$rank = 4;
		    }
		    if ($sensebang) {
			$rank |= 2;
		    } elsif ($nsense == 1) {
			$rank |= 1;
		    }
			
		    ++$printsigs{ "$$instsig[0]$coresig$$instsig[1]\t$rank\n" };
		    $found_simple_sig = 1;
		}
	    } else {
		unless ($sig{'cf'} =~ / /) {
		    if ($current_first_base) {
			my $lang = ($lang =~ /^qpn/ ? 'sux' : $lang);
			my $instsig0 = "\@$project\%$lang:$current_first_base";
			++$printsigs{ "$instsig0=$coresig\$$sig{'cf'}/$current_first_base#~\t0\n" };
			$found_simple_sig = 1;
		    }
		}
	    }
	    
	} elsif (/\@bases\s+(\S+)/) {

	    #
	    # Need to index primary bases here and autogenerate sigs for all of them
	    #
	    
	    $current_first_base = $1;
	    $current_first_base =~ s/;$//;
	    $current_first_base =~ s/^\*//;
	    
	} elsif (s/^\@form(!?)\s+(\S+)//) {

	    sigs_form($1,$2,$_) unless
		$ORACC::CBD::Forms::external;

	}
    }
    sigs_simple_dump();
}

sub sigs_form {
    my $formbang = $_[0] || '';
    $sig{'form'} = $_[1];
    local($_) = $_[2];
    my @cof_tails = ();
    my $cof = $COF_NONE;
    my $cof_part = '';
    my $cof_sig = '';
    my $cof_core = '';
    my $cof_template = '';
    
    $formbang = '!' if $entrybang;
    
    @sig{qw/norm morph morph2 base cont stem root/} = ();
    
    my $cof_nth = 0;
    
    if (s/\s+\%(\S+)//) {
	$sig{'lang'} = $1;
	$sig{'lang'} =~ s#/n$#-949# if $sig{'lang'};
    } else {
	$sig{'lang'} = $lang;
    }
    
    if (/\$\(/) {
	warn "identified COF\n" if $verbose;
	
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
	    warn "found COF TAIL\n" if $verbose;
	} else {
	    $cof = $COF_HEAD;
	    $cof_nth = 1;
	    warn "found COF HEAD\n" if $verbose;
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
    
    if (s/\s\@(\S+)//) {
	my $rws = $1;
	if (defined $ORACC::L2GLO::Util::rws_map{$rws}) {
	    $sig{'lang'} = $ORACC::L2GLO::Util::rws_map{$rws};
	    if ($rws eq 'EG' 
		&& ($lang && $lang ne 'sux')) {
		$sig{'lang'} = 'sux';
	    } else {
		pp_warn("(sigs) unknown RWS code \@$rws has been ignored");
	    }
	}
    }
    
    if ($sig{'form'}) {
	my $instsig1 = "\@$project\%$lang:$sig{'form'}=";
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
	    ++$printsigs{ "$instsig1$coresig$instsig2\t$srank\n" };
	} else {
	    $found_simple_sig = 1;
	    push @instsigs, [ $formbang.$instsig1, $instsig2 ];
	}
	
    }
    
}

sub sigs_simple_dump {
    mkdir -p "01bld/$lang";
    @sigs_simple = sort (keys %printsigs, keys %noprintsigs);
    $sigs_simple_file = "01bld/$lang/simple.sig";
    open(SIMPLE, ">$sigs_simple_file");
    print SIMPLE "\@fields sig rank\n", @sigs_simple;
    close(SIMPLE);
    open(CORESIGS, ">01bld/$lang/coresigs.txt");
    print CORESIGS join("\n", sort keys %coresigs), "\n";
    close(CORESIGS);
}

######################################################################

sub sigs_cofs {
    my %cofs = ();
    my i = 0;
    open(C,">01bld/$lang/cofs.sig");
    foreach my $c (@sigs_simple) {
	++$i;
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
		push @{${$cofs{"$lang\:$key"}}[$index]}, $v;
	    } else {
		pp_file($sigs_simple_file);
		pp_line($i);
		pp_warn "(cof) syntax error: $_";
	    }
	}
    }
    foreach my $c (keys %cofs) {
	my @parts = @{$cofs{$c}};
	permute(@parts);
    }
    close(C);
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
	    print C "$_\t0\n";
	}
    }
}

######################################################################



1;
