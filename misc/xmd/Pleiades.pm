package ORACC::XMD::Pleiades;
use warnings; use strict; use open 'utf8';

my @ignored_places = qw/unclear/;
my %ignored_places = (); @ignored_places{@ignored_places} = ();

sub
pleiades_id {
    my($xmd_id,$prov,@pdata) = @_;
#    warn "pleiades_id: $xmd_id, $prov\n";
    my @places = split(/,\s*/,$prov);
    my @ids = ();
    foreach my $p (@places) {
	if ($p eq 'uncertain') {
	    push @ids, 0;
	} else {
	    my $id = undef;
	    foreach my $pdata (@pdata) {
		last if $id = $$pdata{$p};
	    }
	    if ($id) {
		push @ids, $id;
	    } else {
		warn("pleiades_id:$xmd_id: no ID for place $p in @pdata\n")
		    unless exists($ignored_places{$p});
		push @ids, 0;
	    }
	}
    }
    return join(',', @ids);
}

sub
coords {
    my($pl_id, @pdata) = @_;
    my $c = undef;
#    print STDERR "Pleiades::coords requested for $pl_id ... ";
    foreach my $pdata (@pdata) {
        last if $c = $$pdata{$pl_id,'coords'};
    }
    if ($c) {
#	print STDERR "$c\n";
    } else {
#	print STDERR "not found\n";
    }
    $c;
}

sub
load {
    my $fn = shift;
    my %p = ();
    if (open(P,$fn)) {
	warn "Pleiades::load: reading $fn\n";
	while (<P>) {
	    chomp;
	    my @fields = split(/\t/,$_);
	    @fields = map { /^\s*(.*?)\s*$/ } @fields;
	    my $oraccname = $fields[0];
	    my ($pl_id, $pl_uid, $alt, $coords) = ();
	    for my $i (1 .. $#fields+1) {
		next unless $fields[$i];
		if ($fields[$i] =~ /^[0-9a-f]{32}$/) {
		    $pl_uid = $fields[$i];
		} elsif ($fields[$i] =~ /^\d+$/) {
		    $pl_id = $fields[$i];
		} elsif ($fields[$i] =~ /^\[.*\]$/) {
		    $coords = $fields[$i];
		} else {
		    $alt = $fields[$i];
		}
	    }
	    $p{$oraccname} = $pl_id;
#	    warn("Pleiades::load: $oraccname = $pl_id\n");
	    if ($alt) {
		foreach my $a (split(/,\s*/,$alt)) {
		    if ($pl_id) {
			$p{$a} = $pl_id;
			$p{$pl_id,$a} = $oraccname;
		    } else {
			$p{$a} = $oraccname;
		    }
		}
	    }
	    if ($pl_id) {
		$p{$pl_id,'oname'} = $oraccname;
		$p{$pl_id,'coords'} = $coords || '';
		$p{$pl_id,'pl_uid'} = $pl_uid || '';
	    }
	}
    } else {
	warn "Pleiades::load: can't open $fn\n";
	return undef;
    }
    return { %p };
}

1;
