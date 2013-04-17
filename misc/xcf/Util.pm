package ORACC::XCF::Util;
use warnings; use strict;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::NS;

my $xcf_rnc = "$ENV{'ORACC'}/lib/schemas/xcf.rnc";

my %builtins = (
    'oracc'=>$ENV{'ORACC'},
    'varoracc'=>$ENV{'ORACC_VAR'},
    'datestamp'=>`date +%Y-%m-%d`,
    'timestamp'=>`date`,
    'dateversion'=>`date +%Y-%m-%d:%HH:%MM:%ss`,
    );

#### dateversion needs ADD HH:MM:ss
sub
load {
    my ($fn,$set_rnc) = (shift,shift);
    my %predefs = @_;

    $builtins{'project'} = `oraccopt`;
    return undef unless $builtins{'project'};
    $builtins{'projecthome'} = "$builtins{'oracc'}/$builtins{'project'}";
    chomp(@builtins{qw/datestamp dateversion timestamp/});

    rnv($xcf_rnc,undef,0);
    rnv($xcf_rnc,$fn,0);
    rnv($set_rnc,undef,0) if $set_rnc;
    my $x = load_xml($fn);
# print STDERR $x->toString(1); exit 0;
    my %set = (%builtins,%predefs);
    if ($x) {
	foreach my $set (tags($x,$XCF,'set')) {
	    my $var = undef;
	    if ($set->hasAttribute('to')) {
		$var = ivar($set->getAttribute('to'),\%set);
	    } else {
		my @to = tags($set,$XCF,'to');
		if ($#to >= 0) {
		    my @var = ();
		    foreach my $to (@to) {
			push @var, ivar($to->getAttribute('to'),\%set);
		    }
		    $var = [ @var ];
		} else {
		    my @kv = tags($set,$XCF,'key');
		    my %var = ();
		    foreach my $kv (@kv) {
			$var{$kv->getAttribute('k')}
			    = ivar($kv->getAttribute('to'),\%set);
		    }
		    $var = { %var };
		}
	    }
	    my $name = $set->getAttribute('var');
	    unless ($name) {
		warn("no name in ", $set->toString(1), "\n");
	    } elsif ($set{$name}) {
#		print STDERR ref($set{$name}), "\n";
		if (ref($set{$name}) eq 'ARRAY') {
		    push @{$set{$name}}, $var;
		} else {
		    $set{$name} = [$set{$name},$var];
		}
	    } else {
		$set{$name} = $var;
	    }
	}
    }
#    use Data::Dumper; print STDERR Dumper(\%set); exit 0;
    validate($set_rnc,%set)
	if $set_rnc;
    return listify_values(%set);
}

# interpolate variables in string
sub
ivar {
    my($str,$setref) = @_;
    $str =~ s/\$\$/\000/g;
    $str =~ s/\$\{?([a-zA-Z0-9]+)\}?/getref($1,$setref)/eg;
    $str =~ tr/\000/$/;
    $str;
}

sub
getref {
    my($r,$set) = @_;
    if ($$set{$r}) {
	return $$set{$r};
    } else {
	warn("XCF: undefined variable '$r'\n");
	return '';
    }
}

# force all values to be lists
sub
listify_values {
    my %s = @_;
    my %ret = ();
    foreach my $k (keys %s) {
	if (ref($s{$k}) eq 'ARRAY') {
	    $ret{$k} = $s{$k};
	} else {
	    $ret{$k} = [ $s{$k} ];
	}
    }
    return { %ret };
}

sub
validate {
    my ($rnc,$set) = @_;
    my $xml = xcfXML($set);
    if ($rnc) {
	open(OUT,"/tmp/$$.xcf");
	print OUT $xml;
	close(OUT);
	rnv($rnc,"/tmp/$$.xcf");
	unlink("/tmp/$$.xcf");
    }
}

sub
dumpconfig {
    my $set = shift;
    my $xml = xcfXML($set);
    my $doc = load_xml_string($xml);
    print STDERR $doc->toString(1);
}

sub
xcfXML {
    my $set = shift;
    my %set = %$set;
    my @xml = (xmldecl(),
	       '<xr:xcf-result xmlns:xr="http://oracc.org/ns/xcf-result/1.0">');
    foreach my $v (sort keys %set) {
	if (ref($set{$v}) eq 'ARRAY') {
	    push (@xml, 
		  map ({ "<$v>".liXML($_)."</$v>" } @{$set{$v}}));
	} elsif (ref($set{$v}) eq 'HASH') {
	    push (@xml, 
		  "<$v>",
		  map ( { "<$_>${$set{$v}}{$_}</$_>" } sort keys %{$set{$v}}),
		  "</$v>");
	} else {
	    push @xml, "<$v>$set{$v}</$v>";
	}
    }
    push @xml, '</xr:xcf-result>';
    join('',@xml);
}

sub
liXML {
    my $v = shift;
    if (ref($v) eq 'HASH') {
	join('', map ( { "<$_>$$v{$_}</$_>" } sort keys %$v));
    } else {
	"$v";
    }
}

1;
