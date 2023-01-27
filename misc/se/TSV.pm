package ORACC::SE::TSV;
use warnings; use strict;
#use open ':utf8'; 
use utf8;
use Encode;
use lib "$ENV{'ORACC'}/lib";
use ORACC::SE::DBM;

binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

sub
toTSV {
    my $ix = shift;
    my $name = $$ix{'#name'};
    my $tsv = $$ix{'tsv'} ? $$ix{'tsv'} : "02pub/sl/$name-db.tsv";
    open(TSV,">:raw", $tsv) || die "$0: failed to open $tsv. Stop.\n";
    select TSV;

#    my $fldtype = ref($$ix{'#fields'});
#    if ($fldtype eq 'ARRAY') {
#	foreach my $f (@{$$ix{'#fields'}}) {
##	    print_grep($ix,"#${f}_grep");
#	    print_grep($ix,"#${f}:item_grep");
#	}
#    } else {
#	foreach my $f (split(/\s+/,$$ix{'#fields'})) {
##	    print_grep($ix,"#${f}_grep");
#	    print_grep($ix,"#${f}:item_grep");
#	}
#    }

    $ix = ORACC::SE::DBM::flatten($ix);

#    if ($$ix{'#use_grep'}) {
#	foreach my $k (grep /r$/, sort keys %$ix) {
#	    my $cdata = $k;
#	    $cdata =~ s/r$//;
#	    print "<r><key><![CDATA[$cdata]]></key>";
#	    my $pk = xmlify($$ix{$k});
#	    print "<ids>$pk</ids></r>\n";
#	}
#    }

    if ($$ix{'#all_keys'}) {
	foreach my $k (keys %$ix) {
	    next if $k =~ /^\#.*?(item|record)_grep$/;
	    if (defined $$ix{$k}) {
#		my $f = '';
#		if (defined $$ix{$k,'f'}) {
#		    my $f = xmlify($$ix{$k,'f'});
#		    $f = " f=\"$f\"";
#		}
#		my $pk = xmlify($k);
		my $pv = $$ix{$k};
		my $pk = $k;
		$pk =~ tr//;/;
		#		Encode::_utf8_on($pk);
		#		Encode::_utf8_on($pv);
		# warn "printing $pk\t$pv\n";
		Encode::_utf8_off($pv) if $pk =~ /;(map|name|uchar|values)$/;
		print "$pk\t$pv\n"
	    }
	}
    } elsif ($$ix{'#name' eq 'issl-se'}) {
	foreach my $k (keys %$ix) {
	    next if $k =~ /^\#.*?(item|record)_grep$/;
	    if (defined $$ix{$k}) {
		my $pv = $$ix{$k};
		my $pk = $k;
		$pk =~ tr//;/;
		print "$pk\t$pv\n"
	    }
	}
    } else {
	die "TSV for ePSD data not yet implemented\n";
	foreach my $k (grep !//, sort keys %$ix) {
	    next if $k =~ /^\#.*?(item|record)_grep$/;
	    if (defined $$ix{$k}) {
		my $f = '';
		if (defined $$ix{$k,'f'}) {
		    $f = " f=\"$$ix{$k,'f'}\"";
		}
		my $pk = xmlify($k);
		Encode::_utf8_off($pk);
		print "<k n=\"$pk\"$f>";
		Encode::_utf8_off($$ix{$k});
		print $$ix{$k};
		print "</k>\n";
	    }
	}
    }
#    print "</hash>";
#    close(XML);
    close(TSV);
}

