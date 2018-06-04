package ORACC::CBD::Fix;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/pp_fix pp_fix_load_fixes/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Bases;
use ORACC::CBD::Hash;

use Data::Dumper;

my %acd = ();
my @cfgws = ();
my %cfgws = ();
my %fixes = ();

my %basemap = ();
my $is_compound = 0;

sub pp_fix {
    my($args,$h) = @_;
    if ($$args{'check'}) {
	@cfgws = pp_hash_cfgws($h);
	@cfgws{@cfgws} = ();
	die unless load_fixes($args, $h);
	exit 0;
    } else {
	die unless load_fixes($args, undef);
    }
    %acd = pp_hash_acd($h);
    foreach my $e (@cfgws) {
	if ($fixes{$e}) {
	    fix_entry($e); # FIXME: need notation for mapping simple to usage/compound and vice-versa?
	}
    }
    fix_refs();
    pp_acd_sort(\%acd);
    pp_serialize($h,\%acd);
}

sub pp_fix_load_fixes {
    my($args,$h) = @_;
    die unless load_fixes($args, $h);
    %fixes;
}

sub fix_entry {
    my $fix = shift;
    my $new = $fixes{$fix};
    if (exists $cfgws{$new}) {
	# merge the to-fix item with the target
	fix_merge($fix,$new);
	delete(${$acd{'ehash'}}{$fix});
    } else {
	# rename and update ehash
	my $newname = $fixes{$fix};
	my $f = ${$acd{'ehash'}}{$fix};
	$$$f{'entry'} = [ $new ];
	${$acd{'ehash'}}{$new} = $f;
	delete(${$acd{'ehash'}}{$fix});
    }    
}

sub fix_merge {
    my($fix,$new) = @_;
    my $f = ${$acd{'ehash'}}{$fix};
    my $i = ${$acd{'ehash'}}{$new};
    my $i_bases = '';
    foreach my $fld (fields_of(keys %{$$$f{'fields'}})) {
	next if $fld eq 'entry';
	# if $f = 'form', build an index of 'form's in %known
	# this should use canonicalized versions as returned by the
	# parse_xxx routines, but they are not done yet ...
	my %known = ();
	
	if ($$$f{'rws_cfs'}) {
	    my %r = ();
	    if ($$$i{'rws_cfs'} && ref($$$i{'rws_cfs'}) eq 'HASH') {
		my %i = %{$$$i{'rws_cfs'}};
		foreach my $k (keys %{$$$i{'rws_cfs'}}) {
		    $r{$k} = ${$$$i{'rws_cfs'}}{$k}
		}
	    }
	    delete($$$i{'rws_cfs'});
	    my %from_rws = %{$$$f{'rws_cfs'}};
	    foreach my $k (keys %from_rws) {
		unless (defined ${$$$i{'rws_cfs'}}{$k}) {
		    $r{$k} = $from_rws{$k};
		}
	    }
	    foreach my $r (keys %r) {
		${$$$i{'rws_cfs'}}{$r} = $r{$r};
	    }
	}
	$i_bases = '';
	foreach my $l (@{$$$i{$fld}}) {
	    my $tmp = $l;
	    $tmp =~ s/\s+\@\S+\s*//;
	    if ($fld eq 'bases') {
		$i_bases = $tmp;
	    } else {
		++$known{$tmp};
	    }
	}
	foreach my $l (@{$$$f{$fld}}) {
	    my $tmp = $l;
	    $tmp =~ s/\s+\@\S+\s*//;
	    if ($fld eq 'bases') {
		my $b = bases_merge($i_bases, $tmp, $is_compound);
		if ($$b{'map'}) {
		    %basemap = %{$$b{'map'}};
		}
		${$$$i{'bases'}}[0] = bases_string($b);
	    } else {
		if ($fld eq 'form' && $fld =~ m#/(\S+)#) {
		    my $fb = $1;
		    if ($basemap{$fb}) {
			my $nb = $basemap{$fb};
			warn "fixing form base $fb to $nb\n";
			$fld =~ s#/(\S+)#/$fb#;
		    } 
		}
		if (!defined $known{$tmp}) {
		    ++${$$$i{'fields'}}{$fld} unless ${$$$i{'fields'}}{$fld};
		    if ($fld eq 'sense') {
			warn "setting +$l\n";
			push @{$$$i{$fld}}, "+$l";
		    } else {
			push @{$$$i{$fld}}, "$l";
		    }
#		    ++${$$$i{'fields'}}{$fld} unless ${$$$i{'fields'}}{$fld};
#		    push @{$$$i{$fld}}, $l;

		}
	    }
	}
    }
}
    
sub fix_refs {
    foreach my $e (values %{$acd{'ehash'}}) {
	if (${$$e}{'parts'}) {
	    my @parts = @{$$e}{'parts'};
	    my @newparts = ();
	    foreach my $partsref (@parts) {
		foreach my $parts (@$partsref) {
		    my @p = ();
		    while ($parts =~ s/^\s*(.*?\]\S+)//) {
			my $p = $1;
			my $save = $p;
			$p =~ s/\[(.*?)\]/ \[$1\] /;
			if ($fixes{$p}) {
			    my $f = $fixes{$p};
			    $f =~ s/\s+\[(.*?)\]\s+/\[$1\]/;
			    push @p, $f;
			} else {
			    push @p, $save;
			}
		    }
		    push @newparts, join(' ', @p);
		}
	    }
	    ${$$e}{'parts'} = [ @newparts ];
	}
#          'bffs' => {
#                      'a [tome] N' => [
#                                      'b000001'
#                                    ],
#                      'b000001' => {
#                                     'link' => 'a [tome] N',
#                                     'ref' => 18,
#                                     'code' => undef,
#                                     'class' => 'COMP',
#                                     'line' => 19,
#                                     'bid' => 'b000001',
#                                     'label' => undef
#                                   }
#                    },

	if (${$$e}{'bff'}) {
	    my @bff = @{$$e}{'bff'};
	    my @newbff = ();
	    foreach my $bffref (@bff) {
		foreach my $bff (@$bffref) {
		    $bff =~ /<(.*?)>/;
		    my $bffxref = $1;
		    if ($fixes{$bffxref}) {
			$bff =~ s/<(.*?)>/<$fixes{$bffxref}>/;
		    }
		    push @newbff, $bff;
		}
	    }
	    ${$$e}{'parts'} = [ @newbff ];
	}
    }
}

sub load_fixes {
    my ($args,$h) = @_;
    my $file = '';
    my $fixfile = $$args{'fix'} || '';
    my @l = ();
    my %seen = ();
    
    if ($fixfile eq '-') {
	@l = (<>);
	$file = '<stdin>';
    } else {
	die "$0: must give fixes file with -fix arg\n" unless $fixfile;
	die "$0: no $fixfile\n" unless -r  $fixfile;
	$file = $fixfile;
	open(F,$fixfile) || die "$0: can't open $fixfile for read\n";
	@l = (<F>);
	close(F);
    }

    for (my $i = 0; $i <= $#l; ++$i) {
	local($_) = $l[$i];
	my $line = $i+1;
	my($from,$to) = ();

	if (/=>/) {
	    warn "$file:$line: ignoring old => format, please replace with tab\n";
	    next;
	}
	unless (/\t/) {
	    warn "$file:$line: ignoring line without <TAB>\n";
	    next;
	}

	my $lang = ''; s/^%(\S+)\s+// && ($lang = "$1:");

	if (/^(.*?)\s+=>\s+(.*?)$/) {
	    ($from,$to) = ($1,$2); # do nothing
	} else {
	    ($from,$to) = (/^(.*?)\t(.*?)$/);
	}

	if ($seen{$from}++) {
	    warn "$file:$line: ignoring repeated from specification '$from'\n";
	} elsif ($from eq $to) {
	    warn "$file:$line: ignoring identity map '$from' == '$to'\n";
	} elsif ($$args{'mode'} eq 'glossary') {
	    # if mode=glossary we want to assert LHS is in glossary and we
	    # don't need to use the lang prefix
	    if (!$h || exists $cfgws{short_form($from)}) {
		$fixes{$from} = $to;
	    } else {
		warn "$file:$line: from entry '$from' not in $$args{'cbd'}\n";
	    }
	} elsif ($$args{'mode'} eq 'corpus') {
	    # if mode=corpus we want to assert RHS is in glossary
	    # and we can safely prepend lang to LHS
	    if (!$h || exists $cfgws{short_form($to)}) {
		$fixes{$lang.$from} = $to;
	    } else {
		warn "$file:$line: to entry '$to' not in $$args{'cbd'}\n"
		    unless $to =~ /^-/; # these are deleted entries so no warning
	    }
	} else {
	    ; # can't happen
	}
    }

    1;
}

# remove //SENSE and anything after POS, but it's OK if there isn't a POS
# also expand to glossary form of cfgw with spaces before/after [/]
sub short_form {
    my $x = shift;
    $x =~ s#//.*?\]#]#;
    $x =~ s/\](\s+\S+).*$/]$1/;
    $x =~ s/\s*\[/ [/;
    $x =~ s/\]\s*/] /;
    $x;
}

1;
