#!/usr/bin/perl
use warnings; use strict; use open ':utf8'; use utf8;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::OID;
use Data::Dumper;
binmode STDIN, ':utf8'; binmode STDERR, ':utf8';  binmode STDOUT, ':utf8';

my $project = 'ogsl';
my $proof = '';

# you can say sl-xml.plx epsd2 01tmp/signdata.asl
if ($#ARGV == 1) {
    $project = shift @ARGV;
}

my %oids = oid_load_domain('sl');

# print Dumper %oids; exit 0;

my $sid = '';

my $asl = shift @ARGV;
if ($asl) {
    unless (-f $asl) {
	$asl = "00lib/$asl.asl";
    }
} else {
    $asl = "00lib/ogsl.asl";
}
my @sortcodes = `sl-sortcodes.plx $asl`; chomp @sortcodes;
my %sortcodes = ();
foreach my $s (@sortcodes) {
    my($sign,@codes) = split(/\t/, $s);
    $sortcodes{$sign} = [ @codes ];
}

#my %actual_signs = ();
#my %actual_forms = ();

#open(C, '>codes.dump');
#print C Dumper \%sortcodes;
#close(C);

my $xid = 'x0000000';
my %sign_ids = (); sign_ids();

open(SL,$asl) || die "sl-xml.plx: can't read signlist `$asl'\n";
my $xl = $asl;
$xl =~ s#00lib#02xml#;
$xl =~ s/\.a?sl$/-sl.xml/;

open(XL,"|gdlme2 -bs>$xl") || die "sl-xml.plx: can't write to $xl";
select XL;

# preload a list of known signs
my %at_signs = ();
#; push @at_signs, `grep \@nosign $asl`
#my @at_signs = `grep '\@sign\\|\@nosign' $asl`; chomp @at_signs;
my @at_signs = `grep \@sign 00lib/ogsl.asl`; chomp @at_signs;
foreach my $a (@at_signs) {
    $a =~ s/\@(?:no)?sign\S*\s+(\S+).*$/$1/;
    ++$at_signs{$a};
}

my %slv_val2sign = ();
my %slv_form_vars = ();
my %v = ();

my %v_closers = ();
my @v_closers = qw/v form end/;
@v_closers{@v_closers} = ();

my %form_closers = ();
my @form_closers = qw/form end/;
@form_closers{@form_closers} = ();

my $in_sign = 0;
my $in_form = 0;
my $in_value = 0;
my $post_form = 0;
my $utf8 = undef;

my $curr_sign = '';
my $curr_form = '';

my $pi_file = $asl;

print '<?xml version="1.0" encoding="utf-8"?>',"\n";
print "<?file $pi_file?>";
print "<signlist project=\"$project\" xmlns:sl=\"http://oracc.org/ns/sl/1.0\" xmlns=\"http://oracc.org/ns/sl/1.0\" xmlns:g=\"http://oracc.org/ns/gdl/1.0\" xmlns:n=\"http://oracc.org/ns/norm/1.0\"\n>";
while (<SL>) {
    next if /^\s*$/ || /^\#/;
    if (/^\@sign\s+(\S+)$/ || /^\@nosign\s+(\S+)$/) {
	my $deprecated = '';
	my $signname = $curr_sign = $1;
	# $actual_signs{$signname} = $.;
	my $n = xmlify($signname);
 
	%slv_form_vars = ();
	%v = ();
	$curr_form = '';
	$post_form = 0;
	
	if (/^\@nosign/) {
	    $deprecated = ' deprecated="1"';
	}
	if ($in_sign) {
	    warn "$asl:$.: missing \@end sign\n";
	}
	$in_sign = 1;
	unless ($deprecated) {
	 #   warn "undefined deprecated\n";
	    $deprecated = '';
	}
	unless ($n) {
	    # warn "undefined n\n";
	    $n = '';
	}
	$sid = $sign_ids{'sl',$signname};
	
	unless ($sid) {
	    warn "sl-xml.plx: internal error: no ID found for $signname\n";
	    $sid = $xid++;
	}
	pi_line();
	if (/\@nosign/) {
	    $sid = "NOT.$sid";
	}
	print "<sign$deprecated n=\"$n\" xml:id=\"$sid\"><name g:me=\"1\">$n</name>";
	if ($sortcodes{$signname}) {
	    print "<sort";
	    foreach my $c (@{$sortcodes{$signname}}) {
		$c =~ s/^(.*?)=(.*?)$/ $1="$2"/;
		$c =~ s/\"\"/\"/g;
		print $c;
	    }
	    print "/>";
	}
    } else {
	unless ($in_sign) {
	    warn "$asl:$.: missing \@sign\n";
	}
	
	my $curr_field = '';
	/^\@([a-z]+)/;
	$curr_field = $1;
	if (exists $v_closers{$curr_field} && $in_value) {
	    print "</v>";
	    $in_value = 0;
	}
	if (exists $form_closers{$curr_field} && $in_form) {
	    if ($curr_field ne 'end' || /sign/) {
#		compute_qualified();
		print "</form>";
		$in_form = 0;
		$curr_form = '';
	    }
	}

	if (s/\s+(\[\s*[A-Za-z0-9].*?\])\s*$//) {
	    $proof = xmlify($1);
	}

	if (/^\@list\s+(\S+)(?:\s+(\S+)?)?\s*$/) {
	    form_check();
	    my ($n,$name) = ($1,$2);
	    my $xn = xmlify($n);
	    if ($curr_form) {
		push @{${$v{$curr_form}}{'#forml'}}, $n;
	    } else {
		push @{$v{'#signl'}}, $n;
	    }
	    if ($name) {
		$name = xmlify($name);
		$name = " name=\"$name\"";
	    } else {
		$name = '';
	    }
	    pi_line();
	    print "<list n=\"$xn\"$name/>";
	} elsif (/^\@form\s+(\S+)\s+(\S+)(?:\s+\S+\s+(\S+))?$/) {
	    if ($in_form) {
		warn "$asl:$.: missing `\@end form'\n";
		$in_form = 0;
	    } else {
		my ($n,$v,$u) = ($2,$1,$3);
		my $formname = $curr_form = $n;
		#$actual_forms{$formname} = $.;

		if (${$v{'#forms'}}{$curr_form}) {
		    warn "$asl:$.: duplicate form $curr_form\n";
		} else {
		    ++${$v{'#forms'}}{$curr_form};
		}
		$n = xmlify($n);
		$v = xmlify($v);
		my $uattr = "";
		$uattr = " utf8=\"$u\"" if $u;
		my $ref = '';
		# if the form is also a sign, make ref point to the sign, otherwise use xml:id here
		## if ($sign_ids{'sl',$formname}) {
		if ($at_signs{$formname}) {
		    $ref = sprintf(" ref=\"%s\"", $sign_ids{'sl',$formname});
		} else {
		    $ref = sprintf(" xml:id=\"%s\"", $sign_ids{'sl',$formname});
		    ++$at_signs{$formname}; # prevent duplicate xml:id
		}
		pi_line();
		my $vv = $v; $vv =~ s/^~//;
		if ($slv_form_vars{$vv}++) {
		    warn "$asl:$.: duplicate var code $vv\n";
		    continue;
		}
		print "<form n=\"$n\" var=\"$v\" varid=\"$sid.$vv\"$uattr$ref><name g:me=\"1\">$n</name>";
		$in_form = 1;
		if ($sortcodes{$n}) {
		    print "<sort";
		    foreach my $c (@{$sortcodes{$n}}) {
			$c =~ s/^(.*?)=(.*?)$/ $1="$2"/;
			$c =~ s/\"\"/\"/g;
			print $c;
		    }
		    print '/>';
		}
		print "<proof>$proof</proof>" if $proof;
		$proof = '';
	    }
	} elsif (s/^\@v(\??)(-?)[\t\s]+//) {
	    form_check();
	    my $query = $1;
	    my $dropped = $2;
	    pi_line();
	    print "<v\n";
	    print " uncertain=\"yes\"" if $query;
	    print " deprecated=\"yes\"" if $dropped;

# what is this for?
	    if (s/^\#(\S+)\s+//) {
		print " comment=\"$1\"";
	    }
	    my $v_lang = '';
	    if (s/^\%(\S+)\s+//) {
		$v_lang = "\%$1 ";
		print " xml:lang=\"$1\"";
	    }

	    if (s/\s+\[(.*?)\]\s*$//) {
		$proof = xmlify($1);
	    }

	    if (/^(\S+)$/) {
		my $n = $1;

		if ($curr_form) {
		    ++${$v{'#forms'}}{$curr_form};
		    ++${$v{$curr_form}}{$n};
		    push @{$v{$n}}, $curr_form;
		} else {
		    ++${$v{'#signv'}}{$n};
		    my $nn = $n; $nn =~ tr/₀-₉ₓ⁻⁺//d;
		    ${$v{'#basev'}}{$nn} = $n;
		    ++${$v{$curr_sign}{$n}};
		    push @{$v{$n}}, $curr_sign;
		}
		
		if ($slv_val2sign{$n} && $slv_val2sign{$n} ne $curr_sign) {
		    warn "$asl:$.: value $n not allowed in $curr_sign because it's in $slv_val2sign{$n}\n";
		    continue;
		}
		
		my $xn = xmlify($n);
		$xn =~ tr/_/ /;
		print " n=\"$xn\">";
		# don't GDL [...]ri and friends
		if ($n =~ /\[\.\.\.\]/ || $n =~ m#^/# || $n =~ /\?/) {
		    print "<name>$xn</name>";
		} else {
		    print "<name g:me=\"1\">$v_lang$xn</name>";
		}
		print "<proof>$proof</proof>" if $proof;
		$proof = '';
		$in_value = 1;
	    } else {
		warn "$asl:$.: syntax error in \@v\n";
	    }
	} elsif (/^\@ucode/) {
	    form_check();
	    my($hex) = (/^\@ucode\s+(\S+)\s*$/);
	    if ($hex) {
		my $utf = '';
		foreach my $h (split(/\./, $hex)) {
		    if ($h =~ /^x[0-9A-F]+/i) {
			$utf .= chr(hex("0$h"));
		    } else {
			$utf .= 'X';
		    }
		}
		$utf =~ s/X/.X./g; $utf =~ s/^\.?(.*?)\.?$/$1/;
		print "<utf8 hex=\"$hex\">$utf</utf8>" if $hex;
	    } else {
		warn "$asl:$.: bad format in \@ucode\n";
	    }
	} elsif (s/^\@pname\s+//) {
	    form_check();
	    chomp;
	    print '<pname n="', xmlify($_), '"/>';
	} elsif (/^\@(?:note|inote|uname|unote|uphase|lit)/) {
	    form_check();
	    $curr_field = 'note' if $curr_field eq 'lit';
	    s/^\S+\s+//;
	    chomp;
	    pi_line();
	    print "<$curr_field>";
	    print xmlify($_);
	    print "</$curr_field>";
	} elsif (/^\@end\s+form\s*$/) {
	    $post_form = 1;
	    if ($in_form) {
		$in_form = 0;
		print "</form\n>";
		$curr_form = '';
	    } else {
		warn "$asl:$.: superfluous '\@end form'\n";
	    }
	} elsif (/^\@end\s+sign\s*$/) {
	    if ($in_form) {
		warn "$asl:$.: missing `\@end form'\n";
		$in_form = 0;
	    }
	    if ($in_sign) {
		$in_sign = 0;
		if (scalar keys %v) {
		    compute_qualified();
		}
		print "</sign\n>";
	    } else {
		warn "$asl:$.: superfluous '\@end sign'\n";
	    }
	} elsif (/^\@fake/) {
	    # TODO
	} else {
	    warn "$asl:$.: syntax error\n";
	}
    }
}

#foreach my $f (sort keys %actual_forms) {
#    if (!$actual_signs{$f}) {
#	my $foid = $sign_ids{'sl',$f};
#	warn "$asl:$actual_forms{$f}: detected \@form $f that is not \@sign; creating xsign $foid\n";
#	my $xf = xmlify($f);
#	print "<xsign n=\"$xf\" xml:id=\"$foid\"><name g:me=\"1\">$xf</name></xsign>";
#    }
#}

print '</signlist>';
close(SL);

if ($project eq 'ogsl') {
    system 'rm', '-f', '02xml/ogsl.xml';
    system 'ln', '-sf', "$ENV{'ORACC_BUILDS'}/ogsl/02xml/ogsl-sl.xml", '02xml/ogsl.xml';
}

##########################################################################################

sub compute_qualified {
#    use Data::Dumper; print STDERR Dumper \%v;
    my @qs = ();

    if ($v{'#signv'}) {
	my $xs = xmlify($curr_sign);
	foreach my $v (grep /ₓ$/, keys %{$v{'#signv'}}) {
	    my $xv = xmlify($v);
	    push @qs, "<q type=\"must\" qn=\"$xv($xs)\"/>";
	    if ($v{'#signl'}) {
		foreach my $l (@{$v{'#signl'}}) {
		    next if $l eq $curr_sign;
		    my $xl = xmlify($l);
		    push @qs, "<q type=\"map\" qn=\"$xv($xl)\" qm=\"$xv($xs)\"/>";
		}
	    }
	}
	foreach my $v (grep !/ₓ$/, keys %{$v{'#signv'}}) {
#	    warn "compute_qualified $v\n";
	    my $xv = xmlify($v);
	    push @qs, "<q type=\"map\" qn=\"$xv($xs)\" qm=\"$xv\"/>";
	    if ($v{'#signl'}) {
#		warn "processing #signl for $v\n";
		foreach my $l (@{$v{'#signl'}}) {
		    next if $l eq $curr_sign;
		    my $xl = xmlify($l);
		    push @qs, "<q type=\"map\" qn=\"$xv($xl)\" qm=\"$xv\"/>";
		}
	    }
	}
    }

    if ($v{'#forms'}) {
	foreach my $f (keys %{$v{'#forms'}}) {
	    my %seen = ();
	    # does the @form have any values
	    if ($v{$f}) {
		my @fv = keys %{$v{$f}};
		push @fv, keys %{$v{'#signv'}} if $v{'#signv'};
		# A value is qualifiable if:
		#    it is in the @sign's values
		#    it applies to more than one @form
		foreach my $fv (@fv) {
		    my $fvbase = $fv; $fvbase =~ tr/₀-₉ₓ//d;
		    next if $seen{$fvbase}++;
		    if ($fv =~ /ₓ$/ || ($v{'#signv'} && ${$v{'#signv'}}{$fv}) || $#{$v{$fv}}) {
			# sign is either x-value or both in form and in sign or applies to more than one @form
			# must be qualified
			$fv = xmlify($fv);
			$f = xmlify($f);
			push @qs, "<q type=\"must\" qn=\"$fv($f)\"/>";
			if (${$v{$f}}{'#forml'}) {
			    foreach my $fl (@{${$v{$f}}{'#forml'}}) {
				next if $fl eq $f;
				my $xfl = xmlify($fl);
				push @qs, "<q type=\"map\" qn=\"$fv($xfl)\" qm=\"$fv($f)\"/>";				
			    }
			}
		    } else {
			# sign may be qualified in corpora but not in @bases
			# if there is a base value with different index, record that also
			my $nn = $fv; $nn =~ tr/₀-₉ₓ⁻⁺//d;
			my $base = '';
			if (${$v{'#basev'}}{$nn}) {
			    my $bn = ${$v{'#basev'}}{$nn};
			    $bn = xmlify($bn);
			    $base = " base=\"$bn\"";
			}
			$fv = xmlify($fv);
			$f = xmlify($f);
			push @qs, "<q type=\"may\" qn=\"$fv($f)\"$base/>";
			if (${$v{$f}}{'#forml'}) {
			    foreach my $fl (@{${$v{$f}}{'#forml'}}) {
				next if $fl eq $f;
				my $xfl = xmlify($fl);
				push @qs, "<q type=\"map\" qn=\"$fv($xfl)\" qm=\"$fv($f)\"/>";				
			    }
			}
		    }
		}
	    } elsif ($v{'#signv'}) {
		my @sv = keys %{$v{'#signv'}};
		# all values are considered to be both in form and in sign; must be qualified
		foreach my $sv (@sv) {
		    $sv = xmlify($sv);
		    $f = xmlify($f);
		    push @qs, "<q type=\"must\" qn=\"$sv($f)\"/>";
		    if (${$v{$f}}{'#forml'}) {
			foreach my $fl (@{${$v{$f}}{'#forml'}}) {
			    next if $fl eq $f;
			    my $xfl = xmlify($fl);
			    push @qs, "<q type=\"map\" qn=\"$sv($xfl)\" qm=\"$sv($f)\"/>";
			}
		    }
		}
	    }
	}
    }
    if ($#qs >= 0) {
	print '<qs>', @qs, '</qs>';
    }
}

sub
form_check {
    if ($post_form && !$in_form) {
	warn "$pi_file:$.: extraneous tags after \@end form\n";
    }
}

sub
pi_line {
    print "<?line $.?>";
}

sub
sign_ids {
    my @nosigns = `grep \@nosign $asl`; chomp @nosigns;
    %sign_ids = %oids;
    foreach (@nosigns) {
	/\s(\S+)\s*$/;
	if ($sign_ids{'sl',$1}) {
#	    warn "sl-xml.plx: duplicate \@nosign $1\n";
	} else {
	    $sign_ids{'sl',$1} = $xid++;
	}	
    }
}

1;
