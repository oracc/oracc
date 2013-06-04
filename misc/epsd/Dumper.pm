package ORACC::SE::Dumper;
use warnings; use strict; use open ':utf8';

my %jtype = (
	     ' '=>'span',
	     '_'=>'span',
	     '^'=>'start',
	     '$'=>'end',
	     '-'=>'item',
	     '~'=>'any',
	     );

my $dumpfh;

sub
dump_init {
    my $fname = shift;
    if ($fname) {
	open(DUMP,">$fname") || die;
	$dumpfh = \*DUMP;
    } else {
	$dumpfh = \*STDOUT;
    }
    print $dumpfh '<?xml version="1.0" encoding="utf-8"?>',"\n";
    print $dumpfh "<dump>";
}

sub
dump_term {
    print $dumpfh "</dump>";
    close $dumpfh;
}

sub
dump {
    my ($term,$state) = @_;
    print $dumpfh '<compiled_term';
    if ($state) {
	print $dumpfh " state=\"$state\"";
    }
    print $dumpfh ">\n";
    print $dumpfh "<string>$$term{'string'}</string>";
    print $dumpfh "<tokens>", join('|',@{$$term{'tokens'}}),"</tokens>";
    if ($$term{'bad'}) {
	print $dumpfh "<bad-search>", $$term{'bad'}, "</bad-search>";
    } else {
	dump_list($$term{$state || 'parsed'});
    }
    print $dumpfh '</compiled_term>';
}

sub
dump_list {
    my @terms = @_;
    foreach my $t (@terms) {
	my @l = @$t;
	my $name = shift @l;
	if ($name eq 'group') {
	    print $dumpfh "<$name>";
	    if ($l[0] && ref($l[0]) eq 'HASH') {
		my $info = shift @l;
		my $needed = '';
		if (defined $$info{'nonterms'}) {
		    my $needs_re = $$info{'nonterms'} ? 1 : 0;
		    $needed = " needed=\"$needs_re\"";
		}
		print $dumpfh "<re$needed>", xmlify($$info{'re'}), "</re>";
		print $dumpfh "<frq>$$info{'freq'}</frq>";
		print $dumpfh "<try>", join(' ', keys %{$$info{'try'}}), "</try>";
	    }
	    dump_list(@l);
	    print $dumpfh "</$name>";
	} elsif ($name eq 'join') {
	    print STDERR "no jtype for $l[0]\n" unless $jtype{$l[0]};
	    print $dumpfh "<join type=\"$jtype{$l[0]}\"/>";
	} elsif ($name eq 'op') {
	    print $dumpfh "<op type=\"$l[0]\"/>";
	} elsif ($name eq 'term') {
#	    my($fuzzy,$field,$type,$raw) = (splice @l,0,4);
#	    print $dumpfh "<term fuzzy=\"$fuzzy\" field=\"$field\" type=\"$type\">";
	    my $raw = shift @l;
	    $raw =~ tr/\cA\cB//d;
	    print $dumpfh "<term>";
	    print $dumpfh "<raw>$raw</raw>";
	    if ($#l >= 0) {
		my $term_info = shift @l;
		my $needs_re = shift @l;
		my $re = shift @l;
		$re = '' unless $re;
		print $dumpfh "<lowest-freq d=\"$$term_info[0]\"/>";
		print $dumpfh "<field d=\"$$term_info[1]\"/>";
		print $dumpfh "<type d=\"$$term_info[2]\"/>";
		print $dumpfh "<re needed=\"$needs_re\">$re</re>";
		print $dumpfh 
		    "<selected-records>",
		    join(' ',@{$$term_info[3]}),
		    "</selected-records>";
		foreach my $g (@l) {
		    my($name,$pat,$freq,$tryref,$recref) = @$g;
		    print $dumpfh "<$name>";
		    if ($name eq 'key') {
			print $dumpfh "<pat>$pat</pat>";
			print $dumpfh "<freq d=\"$freq\"/>";
			foreach my $t (@$tryref) {
			    print $dumpfh "<try d=\"$t\"/>";
			}
			print $dumpfh "<records>",join(' ', @$recref), "</records>";
		    } elsif ($name eq 'join') {
			print $dumpfh "<join type=\"$pat\"/>";
		    } else {
			die "unknown name '$name'\n";
		    }
		    print $dumpfh "</$name>";
		}
	    }
	    print $dumpfh "</$name>";
	} else {
	    die "unhandled name $name";
	}
    }
}

sub
xmlify {
    my $tmp = shift;
    $tmp =~ s/\&/&amp;/g;
    $tmp =~ s/</&lt;/g;
    $tmp;
}

1;
