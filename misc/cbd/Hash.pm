package ORACC::CBD::Hash;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/pp_hash pp_hash_cfgws pp_hash_acd pp_acd_merge pp_acd_sort pp_serialize fields_of/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Bases;

use Data::Dumper;

my $field_index = 0;
my $sense_id = 0;

my $cbdid;
my $cbdlang;
my %cgc = ();
my $cgctmp = '';
my $curr_id = '';
my $curr_cf = '';
my $curr_sense_id = 0;
my $curr_sig_id = '';
my $ebang_flag = '';
my %entries = ();
my $last_char = undef;
my %line_of = ();
my %sense_props = ();
my %sigs = ();
my $usage_flag = 0;

my @tags = qw/entry alias parts bases bff conts morphs morph2s moved phon prefs root 
	      form length norms sense stems equiv inote prop end isslp bib was
	      defn note collo pl_coord pl_id pl_uid/;
my %tags = (); @tags{@tags} = ();

my %fseq = ();
foreach my $f (@tags) {
    $fseq{$f} = 0 + scalar keys %fseq;
}

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

# Create an old-style ACD-hash from a new-style PP-hash
#
# An ACD-hash is just a hash with CFGWPOS as key and the entry content hash as value
sub pp_hash_acd {
    my $h = shift;
    my %acd = ();
    my @ids = @{$$h{'ids'}};
    my %ent = %{$$h{'entries'}};
    my %ehash = ();
    my @entries = ();
    foreach my $id (@ids) {
	my $cf = $ent{$id};
	my %e = %{$ent{$id,'e'}};
	my %f = ();
	foreach my $f (keys %e) {
	    ++$f{$f};
	}
	%{$e{'fields'}} = %f;
	my $eref = { %e };
	$ehash{$cf} = \$eref;
	push @entries, \$eref;
    }
    %{$acd{'ehash'}} = %ehash;
    @{$acd{'entries'}} = @entries;
    #    use Data::Dumper;
    #    print Dumper \%acd; exit 0;
    %acd;
}

sub pp_hash_cfgws {
    my $h = shift;
    my @ids = @{$$h{'ids'}};
    my %e = %{$$h{'entries'}};
    sort map { $e{$_} } @ids;
}

sub pp_hash {
    my ($args,@cbd) = @_;
    my $defn_minus = 0;
    my %h = ();
    my %langnorms = ();
    my $last_tag = '';
    my $use_norms = 0;
    my @ee = ();

    if ($#cbd < 0) {
	@cbd = setup_cbd($args);
	return 0 if pp_status();
	pp_file($$args{'file'});

    } else {
	# pp_file() should be set already
    }
    my %entries = %{${$ORACC::CBD::data{cbdname()}}{'entries'}};

    $cbdlang = ORACC::CBD::Util::lang();
    $cbdid = $cbdlang;
    $cbdid =~ tr/-/_/;
    my $currtag = undef;
    my $currarg = undef;

    $use_norms = $langnorms{$cbdlang}; ## langcore

    my %e = ();
    my %rws_cfs = ();

    for (my $i = 0; $i <= $#cbd; ++$i) {

	next if $cbd[$i] =~ /^[#\000]/;

	pp_line($i+1);
	local($_) = $cbd[$i];

	if (/^\@(project|name|lang)\s+(.*?)\s*$/) {
	    my($key,$val) = ($1,$2);
	    $h{$key} = $val;
	    next;
	}

	next if /^\#/ || /^\@letter/;

	if (/^\@([a-z_]+[-*!]*)\s+(.*?)\s*$/) {
	    ($currtag,$currarg) = ($1,$2);
	    my $default = $currtag =~ s/!//;
	    my $starred = $currtag =~ s/\*//;
	    my $flags = '';
#	    $flags .= '*' if $starred;
#	    $flags .= '!' if $default;

#	    push (@{$e{$currtag,'flags'}}, "$flags");

	    my $linetag = $currtag;
	    $linetag =~ s/\*$//;

	    if ($last_tag eq $currtag) {
		++$field_index;
	    } else {
		$field_index = 0;
		$last_tag = $currtag;
	    }

	    $line_of{$linetag} = pp_line()-1
		unless defined $line_of{$linetag};
	    if ($currtag =~ /^entry/) {
		#		$ebang_flag = $default || '';
		#		$usage_flag = $starred;
		$currarg =~ /^(\S+)/;
		$curr_cf = $1;
		$currarg =~ s/^\s+//; $currarg =~ s/\s*$//;
		$curr_id = $entries{$currarg};
		unless ($curr_id) {
		    pp_warn("weird; no entries_index entry for `$currarg' (this can't happen)\n");
		    $curr_id = '';
		}
		$line_of{'entry'} = pp_line()-1;
		$e{'usage_flag'} = '*' if $starred;
	    } elsif ($currtag =~ /^defn/) {
		$defn_minus = ($currtag =~ s/-$//);
	    }
	    if ($currtag eq 'end') {
		if ($currarg eq 'entry') {
		    if (scalar keys %rws_cfs) {
			%{$e{'rws_cfs'}} = %rws_cfs; %rws_cfs = ();
		    }
		    %{$entries{$curr_id,'e'}} = %e;
		    %{$entries{$curr_id,'l'}} = %line_of;
		    push @ee, $curr_id;
		    %e = ();
		} else {
		    pp_warn("malformed end tag: \@end $currarg");
		}
		%line_of = ();
	    } else {
		if ($currtag eq 'sense') {
		    my($tok1) = ($currarg =~ /^(\S+)/);
		    $curr_sense_id = sprintf("\#%06d",$sense_id++);
		    my $defbang = ($default ? '!' : '');
		    $currarg = "$curr_sense_id $defbang\t$currarg";
		} elsif ($currtag eq 'form') {
		    my $barecheck = $currarg;
		    $barecheck =~ s/^(\S+)\s*//;
		    my $formform = $1;
		    my $tmp = $currarg;
		    $tmp =~ s#\s/(\S+)##; # remove BASE because it may contain '$'s.
		    $tmp =~ s/^\S+\s+//; # remove FORM because it may contain '$'s.
		    if ($currarg =~ s/^\s*<(.*?)>\s+//) {
			push @{$sigs{$curr_sense_id}}, $1;
		    }
		    if ($default || $ebang_flag) {
			$currarg = "!$currarg";
		    }
		} elsif ($currtag eq 'bff') {
		}
		if ($currtag eq 'prop' && $curr_sense_id) {
		    push @{$sense_props{$curr_sense_id}}, $currarg;
		} else {
		    push @{$e{$currtag}}, $currarg;
#			unless $currtag eq 'inote';
		}
	    }
	} elsif (/^\@([A-Z]+)\s*(\S*)\s*$/) {
	    my($k,$v) = ($1,$2);
	    $rws_cfs{$k} = $v || '';
	} else {
	    chomp;
	    pp_warn("(hash) syntax error near '$_'") if /\S/;
	}
    }
    my $cbdname = ORACC::CBD::Util::cbdname();
    ${$ORACC::CBD::data{$cbdname}}{'file'} = pp_file();
    ${${$ORACC::CBD::data{$cbdname}}{'cbdname'}} = $cbdname;
    %{${$ORACC::CBD::data{$cbdname}}{'header'}} = %h;
    @{${$ORACC::CBD::data{$cbdname}}{'ids'}} = @ee;
    %{${$ORACC::CBD::data{$cbdname}}{'entries'}} = %entries;
    $ORACC::CBD::data{$cbdname};
}

sub tags_of {
    my @ret = ();
    foreach my $k (@_) {
	if (exists $tags{$k}) {
	    push @ret, $k;
	}
    }
    @ret;
}

sub pp_acd_merge {
    my($into,$from) = @_;
    foreach my $e (keys %{$$from{'ehash'}}) {
	my $x = $e; $x =~ s/^.*?\s+\[//;
	my $is_compound = ($x =~ /\s/);
	my %basemap = ();
	if (!defined ${$$into{'ehash'}}{$e}) {
	    my $ehash = ${$$from{'ehash'}}{$e};
	    my $eref = { %{$$ehash} };
	    push @{$$into{'entries'}}, $eref;
	    ${$$into{'ehash'}{$e}} = $eref;
	} else {
	    my $f = ${$$from{'ehash'}}{$e};
	    my $i = ${$$into{'ehash'}}{$e};
	    my $i_bases = '';
	    foreach my $fld (sort {$fseq{$a}<=>$fseq{$b}} tags_of(keys %{$$$f{'fields'}})) {
		next if $fld eq 'entry';
		# if $f = 'form', build an index of 'form's in %known
		# this should use canonicalized versions as returned by the
		# parse_xxx routines, but they are not done yet ...
		my %known = ();

		if ($$$f{'rws_cfs'}) {
		    my %r = ();
#		    print STDERR "(0) r at start= ", Dumper(\%r);
		    if ($$$i{'rws_cfs'} && ref($$$i{'rws_cfs'}) eq 'HASH') {
			my %i = %{$$$i{'rws_cfs'}};
#			print STDERR "i= ", Dumper(\%i);
			foreach my $k (keys %{$$$i{'rws_cfs'}}) {
#			    warn "(1)setting $k to ${$$$i{'rws_cfs'}}{$k}\n";
			    $r{$k} = ${$$$i{'rws_cfs'}}{$k}
			}
		    }
		    delete($$$i{'rws_cfs'});
		    my %from_rws = %{$$$f{'rws_cfs'}};
#		    print STDERR "(2)from_rws= ", Dumper \%from_rws;
		    foreach my $k (keys %from_rws) {
#			warn "(3)k=$k val=$from_rws{$k}\n";
			unless (defined ${$$$i{'rws_cfs'}}{$k}) {
#			    warn "(3a)setting $k to $from_rws{$k}\n";
#			    print STDERR "(3b)r= ", Dumper \%r;
			    $r{$k} = $from_rws{$k};
#			    print STDERR "(3c)r= ", Dumper \%r;
			}
		    }
#		    print STDERR "(4)r= ", Dumper \%r;
#		    print STDERR "(5)r-keys= ", join(':', keys %r), "\n";
		    foreach my $r (keys %r) {
#			warn "(6)setting $r to $r{$r}\n";
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
			my $b = bases_merge($i_bases, $tmp, $is_compound); # we want the hash back to map bases in forms
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
			    unless (${$$$i{'fields'}}{$fld}) {
				++${$$$i{'fields'}}{$fld};
				if ($fld eq 'sense') {
				    push @{$$$i{$fld}}, "+$l";
				} else {
				    push @{$$$i{$fld}}, "$l";
				}
			    }
			}
		    }
		}
	    }
	}
    }
    $into;
}

sub pp_serialize {
    my ($pph,$acd) = @_;
    my %h = %{$$pph{'header'}};
    use Data::Dumper;
    print <<EOH;
\@project $h{'project'}
\@lang    $h{'lang'}
\@name    $h{'name'}

EOH
    if ($acd) {
#	print Dumper @{$$acd{'entries'}};
	foreach my $e (@{$$acd{'entries'}}) {
	    pp_acd_serialize_entry($e);
	}
    }
}

sub pp_acd_serialize_entry {
    my $eref = shift;
    my %e = %{$eref};
    my $cfgw = ${$e{'entry'}}[0];
    if (!$cfgw) {
	warn "no cfgw in: ", Dumper $eref;
    }
    my $init_char = first_letter($cfgw);
    unless ($ORACC::CBD::noletters) {
	if (!$last_char || $last_char ne $init_char) {
	    $last_char = $init_char;
	    print "\@letter $last_char\n\n";
	}
    }
    my $ustar = ($e{'usage_flag'} ? '*' : '');
    print "\@entry$ustar $cfgw\n";
    if ($e{'rws_cfs'}) {
	foreach my $rws (sort keys %{$e{'rws_cfs'}}) {
#	    warn "rws key = $rws\n";
	    my $val =  ${$e{'rws_cfs'}}{$rws};
	    if ($val) {
		$val = ' '.$val;
	    } else {
		$val = '';
	    }

	    print "\@$rws$val\n";
	}
    }
    foreach my $f (sort {$fseq{$a}<=>$fseq{$b}} tags_of(keys %{$e{'fields'}})) {
	next if $f eq 'entry' || $f eq 'rws_cf';
	foreach my $l (@{$e{$f}}) {
	    if ($f eq 'sense') {
		$l =~ s/\#\S+\s+//;
	    }
	    my $defbang = '';
	    if ($l =~ s/^!\s*//) {
		$defbang = '!';
	    }
	    if ($l =~ s/^\+//) {
		$defbang = "+$defbang";
	    }
	    print "\@$f$defbang $l\n";
	}
    }
    print "\@end entry\n\n";
}

sub
pp_acd_sort {
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
first_letter {
    my($first) = ($_[0] =~ /^(.)/);
    $first = "\U$first";
    $first = $vowel_of{$first} if $vowel_of{$first};
    $first;
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

sub fields_of {
    sort {$fseq{$a}<=>$fseq{$b}} tags_of(@_);
}


1;

