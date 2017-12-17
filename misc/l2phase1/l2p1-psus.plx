#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;

my $compound = 0;
my %cfs = ();
my %e = ();
my $err_glo = '';
my $glo = '';
my @parts_errors = ();
my $psu_parts = '';
my $verbose = 0;
my $lang = '';
my @entries_parts_lines = ();

my %validated_parts_lines = ();
my %bad_parts_lines = ();

my @no_sense_forms = ();
my @sense_forms = ();
my $in_sense = 0;

open(S, '01bld/coresigs.txt');
while (<S>) {
    chomp;
    my($cf,$gw) = (m#^(.*?)\[(.*?)//#);
    s/\!0x.*$//;
    push @{${$cfs{$cf}}{$gw}}, $_;
}
close(S);

my %simple = ();
open(S, '01tmp/l2p1-simple.sig');
my $fields = <S>;
while (<S>) {
    next if /^\@fields/;
    chomp;
    s/\t.*$//;
    m#=(.*?)\$#;
    my $keysig = $1;
    s/\!0x.*$//;
    if (defined $keysig) {
	push(@{$simple{$keysig}}, $_);
    }
#	if $keysig && $simple{$keysig};
}
close(S);

#warn Dumper \%cfs;

my $g2 = `oraccopt . g2`;

open(PARTSMAP, '>01bld/parts.map');
open(P, '>01tmp/l2p1-psus.sig'); select P;
# print $fields; ### NO: simple cofs psus are cat'ed together so no @fields on cofs/psus
if ($g2 eq 'yes') {
    foreach my $g (<00lib/*.glo>) {
	$err_glo = $g;
	$lang = $glo = $g;
	$lang =~ s#^.*?/([^/]+)\..*$#$1#;
	psu_glo()
    }
} else {
    foreach my $g (<00lib/*.glo>) {
    $err_glo = $g;
    $lang = $glo = $g;
    $lang =~ s#^.*?/([^/]+)\..*$#$1#;
    if ($lang =~ /^(?:sux|qpn)/) {
	$glo =~ s/00lib/01bld/;
	$glo .= '.norm';
    }
    psu_glo();
}

sub psu_glo {
    open(G, $glo);
    while (<G>) {
	if (m/^\@entry\s+(.*?)\s*\[(.*?)\]\s*(\S+)\s*$/) {
	    @e{qw/cf gw pos/} = ($1,$2,$3);
	    $compound = ($e{'cf'} =~ / /);
	    $in_sense = 0;
	    @no_sense_forms = ();
	} elsif (/^\@parts/) {
	    $psu_parts = $_;
	    $psu_parts =~ s/^\@parts\s+//;
	    chomp $psu_parts;
	    push @entries_parts_lines, [ $. , $_ ];
	} elsif (/^\@form/ && $compound) {
	    if ($in_sense) {
		my $formlang = '';
		if (s/\s\%([a-z]\S+)//) {
		    $formlang = $1 || '';
		}
		do_psu($formlang || $lang, $_);
	    } else {
		push @no_sense_forms, $_;
	    }
	} elsif (/^\@sense/ && $compound) {
	    $in_sense = 1;
	    my($epos,$sense) = (/\s(\S+)\s+(.*?)\s*$/);
	    if ($epos) {
		@e{qw/epos sense/} = ($epos,$sense);
		foreach my $f (@no_sense_forms) {
		    my $formlang = '';
		    if ($f =~ s/\s\%([a-z]\S+)//) {
			$formlang = $1;
		    }
		    do_psu($formlang || $lang, $f);
		}
	    } else {
		chomp;
		bad("malformed SENSE: $_");
	    }
	} elsif (/^\@end\s+entry/) {
	    $compound = 0;
	    %e = ();
	    @entries_parts_lines = ();
	}
    }
    close(G);
}

close(P);
close(PARTSMAP);

####################################################################

sub
bad {
    warn "$err_glo:$.: (psu) ", @_, "\n";
}

sub
bad2 {
    my($lnum,@err) = @_;
    warn "$err_glo:$lnum: (psu) ", @err, "\n";
}

sub
do_psu {
    my($psulang,$formline) = @_;
    warn "do_psu: psulang=$psulang; formline=$formline" if $verbose;
    $formline =~ s/^\@form\s+//;
#    my ($forms, $norms) = ($formline =~ /^(\S+)\s*(\S.*)\s*$/);
    my ($forms, $rest) = ($formline =~ /^(\S+)\s*(.*)\s*$/);
    unless ($forms) {
	bad "syntax error, no forms in \@forms line";
    }

    my $psu_form = $forms; $psu_form =~ s/_0/ /g; $psu_form =~ tr/_/ /;
    
    my @forms = ($forms ? split(/_/,$forms) : ());
    my @rest = grep defined&&length, (split(/\s+/, $rest));
    my @norms = map { s/^\$//; $_ } grep (/^\$/, @rest);  # split(/(?:^|\s+)\$/,$norms);
    if ($#norms < 0) {
	@norms = ('*')x($#forms+1);
    }

    if ($#forms < $#norms && $#forms > 1 && $#norms > 1 && $forms[0] ne '*') {
	bad "PSU $psu_parts FORM $formline: please use '_0' to indicate empty FORM elements\n";
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
	    bad @parts_errors
		unless $#parts_errors == 0 && $parts_errors[0] eq '#nowarn#';
	} else {
	    bad "unknown parts processing failure";
	    warn Dumper \%e;
	    warn "====\n";
	}
    }
    @parts_errors = ();
}

sub
find_in_coresigs {
    my($cf,$xgw) = @_;
    if (defined ${$cfs{$cf}}{$xgw}) {
	return $xgw;
    } elsif (defined $cfs{$cf}) {
	use Data::Dumper;
#	warn Dumper \$cfs{$cf};
	foreach my $gw (keys %{$cfs{$cf}}) {
	    my @sigs = @{${$cfs{$cf}}{$gw}};
	    foreach my $s (@sigs) {
		my $qxgw = quotemeta($xgw);
		if ($s =~ m#//[^\]]*$qxgw#) {
		    # warn "matched $cf\[$xgw] in $s\n";
		    return $s;
		}
	    }
	}
    }
    warn "never matched $cf\[$xgw\]\n";
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
	    my $tmp = $s;
	    $tmp =~ s#^.*?//(.*?)\].*$#$1#;
	    ++$s{$tmp};
	}
	if (scalar keys %s > 1) {
	    my @new_m = ();
	    foreach my $s (@m) {
		if ($s =~ m#//$qs\]# || $s =~ m#\[$qs//#) {
		    push @new_m, $s;
		}
	    }
	    if ($#m < 0) {
		bad "$sense matches more than one sense: ", join(', ', keys %s);
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
	my ($pt, $csig, @candidates) = @{$parts_data[$i]};
	my $this_form_matched = 0;
	my @tmp_matches = ();
	my $pass_1 = 1;
      match_pass_1:
	{
	    for (my $j = 0; $j <= $#candidates; ++$j) {
		my($form,$norm) = ($candidates[$j] =~ m#:(.*?)=.*?\$(.*?)(?:$|[/+\#\@])#);
		if ($form && $form eq $forms[$i] 
		    && ($norm eq '*' || 
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
	    push @parts_errors, "no form/norm match for $forms[$i]=$csig\$$norms[$i] in `$psulang.glo'"
		unless $#parts_errors == 0;
	    last;
	}
    }
    return ( [@parts_data] , @matched_candidates );
}

sub
print_psu_sig {
    my ($eref, $psuform, $res_ref, @sigs) = @_;
    my @p = ();
    foreach my $r (@$res_ref) {
	push @p, $$r[0];
    }
    my $psusig = join('++', @sigs);
    print("{$psuform = @p += $$eref{'cf'}\[$$eref{'gw'}//$$eref{'sense'}\]$$eref{'pos'}'$$eref{'epos'}}::", 
	  $psusig, "\t0\n");
    print PARTSMAP "@p\t", $psusig, "\n";
}

sub
validate_parts {
    my($psulang, $lnum, $p) = @_;
    my $status = 0;
    $p =~ s/^\@parts\s+//;
    $p =~ s/\s[\%\#\@\/\+]\S+/ /g;
    $p =~ s/\s+/ /g;
    $p =~ s/(\]\S+)\s+/$1\cA/g;
    my @parts = grep defined&&length, split(/\cA/,$p);
    my @ret = ();
    foreach my $pt (@parts) {
	my($cf,$gw) = ($pt =~ /^(.*?)\[(.*?)(?:\/\/|\])/);
	my($pos,$epos) = ('','');
	my $sense = '';
	if (($gw = find_in_coresigs($cf,$gw))) {
	    if ($gw =~ /\[/) { # the return was a matched coresig, not a simple GW
		my($xgw,$xsense,$xpos,$xepos) = ($gw =~ m#^.*?\[(.*?)//(.*?)\](.*?)'(.*?)#);
		if ($pos && $pos != $xpos && $pos != $xepos) {
		    bad2($lnum,"$pt has wrong POS or EPOS in `$psulang.glo'");
		    $status = 1;
		} else {
		    # Now it will be as though the part contained a full coresig
		    ($gw,$sense,$pos,$epos) = ($xgw,$xsense,$xpos,$xepos);
		}
	    } else {
		if ($pt =~ /\](.*?)'([A-Za-z\/]+)/) {
		    ($pos,$epos) = ($1,$2);
		} else {
		    $pt =~ /\]([A-Za-z\/]+)/;
		    $pos = $1;
		}
	    }

	    my @pt_matches = @{${$cfs{$cf}}{$gw}};
	    if ($epos) {
		bad2($lnum,"if you give EPOS in \@parts you must also give POS")
		    unless $pos;
		my @pos_matches = grep(/\]$pos'$epos/, @pt_matches);
		if ($#pos_matches < 0) {
		    bad2($lnum,"$pt has wrong POS or EPOS in `$psulang.glo'");
		} else {
		    @pt_matches = @pos_matches;
		}
	    } else {
		my $qpos = quotemeta($pos);
		my @pos_matches = grep(/\]$qpos\'/, @pt_matches);
		if ($#pos_matches < 0) {
		    @pos_matches = grep /\].*?\'$qpos$/, @pt_matches;
		    if ($#pos_matches < 0) {
			bad2($lnum,"$pt has wrong POS in `$psulang.glo'");
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
	    } else {
		$sense = $gw;
	    }
	    my @sense_matches = match_sense($sense, @pt_matches);
	    if ($#sense_matches < 0) {
		bad "$pt should match a SENSE of $cf\[$gw\] in `$psulang.glo' but doesn't";
	    } else {
		@pt_matches = @sense_matches;
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

	    my $csig = "$cf\[$gw//$sense\]$pos'$epos";
#	    warn "csig = $csig\n";
	    my @simple_matches = ();
	    foreach my $ptm (@pt_matches) {
		if ($ptm && $simple{$ptm}) {
		    push(@simple_matches, @{$simple{$ptm}});
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
	} else {
	    bad2($lnum,"$pt does not match a known CF[GW] in `$psulang.glo'");
	    $status = 1;
	}
    }
    if ($status) {
	();
    } else {
	@ret;
    }
}

1;
