package ORACC::CBD::Map;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/map_apply_sig map_load map_set_map/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::L2GLO::Util;
use Data::Dumper;

my $marshalling = '';

my @cmdcombos = qw/addentry addsense mapentry mapsense mapbase newbases/;
my %combofuncs = (
    addentry => \&addentry,
    addsense => \&addsense,
    addform => \&addform,
    newbases => \&newbases,
    mapentry => \&mapentry,
    mapsense => \&mapsense,
    mapbase => \&mapbase,
);

my %currmap = ();

sub map_set_map {
    my $ref = shift;
    %currmap = %$ref;
}

sub map_apply_sig {
    my $s = shift;
    chomp($s);
    my($t) = ($s =~ s/\t(.*$)//);
    my %f = parse_sig($s);
    my $key = "$f{'cf'}\[$f{'gw'}]$f{'pos'}";
#    print Dumper \%currmap;
#    warn "key=$key\n";
    if ($currmap{$key}) {
	my $keysense = "$f{'cf'}\[$f{'gw'}//$f{'sense'}]$f{'pos'}'$f{'epos'}";
#	warn "keysense=$keysense\n";
	if ((my $to = ${$currmap{$key,'sense'}}{$keysense})) {
#	    warn "mapping $keysense => $$to\n";
	    my %f2 = parse_sig($$to); # why is this a ref to a scalar??
#	    print Dumper \%f2;
	    $f{'sense'} = $f2{'sense'};
	}
	$f{'form'} =~ s/^%.*?://;
	$f{'base'} =~ s/^%.*?://;
	if ($currmap{$key,'base'}) {
	    if ((my $mbase = ${$currmap{$key,'base'}}{$f{'base'}})) {
		$f{'base'} = $$mbase;
	    }
	}
	$s = serialize_sig(%f);	
    }
    
    "$s\t$t\n";
}

sub map_load {
    my ($map,$for) = @_;
    $marshalling = $for;
    my %map = ();
    open(M, $map);
    while (<M>) {
	next if /^\s*$/ || /^\#/;
	if (/^(add|cut|fix|map|new)\s+(.*?)$/) {
	    my ($cmd,$arg) = ($1,$2);
	    my($what,$from,$to) = ();
	    if (/=>/) {
		unless (($what,$from,$to) = ($arg =~ /^(entry|sense|bases|base|form)\s+(.*?)\s*=>\s*(.*?)\s*$/)) {
		    warn "$map:$.: syntax error: bad field\n";
		    next;
		}
	    } else {
		unless (($what,$from) = ($arg =~ /^(entry|sense|bases|base|form)\s+(.*?)\s*$/)) {
		    warn "$map:$.: syntax error: bad field\n";
		    next;
		}
	    }
	    my %f = parse_sig($from);
	    my $key = "$f{'cf'}\[$f{'gw'}]$f{'pos'}";
	    my $combo = "$cmd$what";
	    my $combofunc = $combofuncs{$combo};
	    if ($combofunc) {
		my ($mapkey,$mapdata) = &$combofunc($key,\%f,$to);
		if ($mapkey) {
		    if ($marshalling eq 'sigs') {
			my($what,$from,$to) = @$mapdata;
			$map{$mapkey} = 1;
			${${$map{$mapkey,$what}}{$from}} = $to;
		    }
		}
	    } else {
		warn "$map:$.: cmd/field combination '$combo' not handled\n";
	    }
	} else {
	    warn "$map:$.: syntax error: bad command\n";
	}
    }
    close(M);
    map_set_map(\%map);
    %map;
}


sub addentry {
    return undef if $marshalling eq 'sigs';
    my($k,$f,$to) = @_;
    ($k,['add','entry',$to]);
}

sub addsense {
    return undef if $marshalling eq 'sigs';
    my($k,$f,$to) = @_;
#    $to =~ s#\]#//$$f{'sense'}]#;
#   $to .= "'$$f{'epos'}";
    ($k,['add','sense',$to]);
}

sub addform {
    return undef if $marshalling eq 'sigs';
    my($k,$f,$to) = @_;
    ($k,['add','form',$to]);
}

sub newbases {
    return undef if $marshalling eq 'sigs';
    my($k,$f,$to) = @_;
    ($k,['add','bases',$to]);
}

sub mapentry {
    return undef if $marshalling eq 'sigs';
    my($k,$f,$to) = @_;
    ($k,['map','entry',$to]);
}

sub mapsense {
    if ($marshalling eq 'sigs') {
	my($k,$f,$to) = @_;
	my $from = $k;
	$from =~ s#\]#//$$f{'sense'}]#;
	$from .= "'$$f{'epos'}";
	($k,['sense',$from,$to]);
    } else {
	return undef;
    }
}

sub mapbase {
    my($k,$f,$to) = @_;
    $to =~ /^(\S+)\s+~\s+(\S+)$/;
    ($k,['base',$1,$2]);
}

1;

