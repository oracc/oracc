package ORACC::CBD::Map;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/map_load/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::L2GLO::Util;
use Data::Dumper;

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

sub map_load {
    my $map = shift;
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
		push @{$map{$mapkey}}, $mapdata;
	    } else {
		warn "$map:$.: cmd/field combination '$combo' not handled\n";
	    }
	} else {
	    warn "$map:$.: syntax error: bad command\n";
	}
    }
    close(M);
    %map;
}


sub addentry {
    my($k,$f,$to) = @_;
    ($k,['addentry',$to]);
}

sub addsense {
    my($k,$f,$to) = @_;
#    $to =~ s#\]#//$$f{'sense'}]#;
#   $to .= "'$$f{'epos'}";
    ($k,['addsense',$to]);
}

sub addform {
    my($k,$f,$to) = @_;
    ($k,['addform',$to]);
}

sub newbases {
    my($k,$f,$to) = @_;
    ($k,['newbases',$to]);
}

sub mapentry {
    my($k,$f,$to) = @_;
    ($k,['mapentry',$to]);
}

sub mapsense {
    my($k,$f,$to) = @_;
    $k =~ s#\]#//$$f{'sense'}]#;
    $k .= "'$$f{'epos'}";
    ($k,['mapsense',$to]);
}

sub mapbase {
    my($k,$f,$to) = @_;
    $to =~ /^(\S+)\s+~\s+(\S+)$/;
    ($k,['mapbase',$1,$2]);
}

1;

