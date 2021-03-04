package ORACC::CBD::Map;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/map_apply_glo map_apply_sig map_load map_set_map/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Bases;
use ORACC::L2GLO::Util;
use Data::Dumper;

my $acd_rx = $ORACC::CBD::acd_rx;

my $marshalling = '';

my @cmdcombos = qw/addentry addparts addsense mapentry mapsense mapbase newbases/;
my %combofuncs = (
    addentry => \&addentry,
    addbase => \&addbase,
    addparts => \&addparts,
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

sub map_apply_glo {
    my @cbd = @_;
    my @n = ();
#    print STDERR Dumper \%currmap; exit 0;
    for (my $i = 0; $i <= $#cbd; ++$i) {
	if ($cbd[$i] =~ /^$acd_rx?\@entry\S*\s+(.*?)\s*$/) {
	    my $key = $1;
	    $key =~ s/\s*(\[.*?\])\s*/$1/;
	    if ($currmap{$key}) {
		my %emap = %{$currmap{$key}};
		# warn "$key\n";
		# print STDERR Dumper \%emap if $key =~ /^ašte/;

		# emap may have @parts @bases @form @sense:

		if ($emap{'parts'}) {
		    while ($i < $#cbd && $cbd[$i] !~ /^\@parts/) {
			push @n, $cbd[$i++];
		    }
		    while ($cbd[$i] =~ /^\@parts/) {
			push @n, $cbd[$i++];
		    }
		    push @n, @{${$emap{'parts'}}};
		}
		
		if ($emap{'bases'}) {
		    while ($cbd[$i] !~ /^\@bases/) {
			push @n, $cbd[$i++];
		    }
		    push @n, $${$emap{'bases'}}[0];
		    ++$i;
		} elsif ($emap{'base'}) {
#		    print STDERR Dumper \%emap;
		    my $addb = join('; ', @{$emap{'base'}});
		    while ($cbd[$i] !~ /^\@bases/) {
			push @n, $cbd[$i++];
		    }
		    my $bm = bases_merge($cbd[$i++],$addb,0);
		    my $nb = bases_serialize(%{$bm});
		    warn "bases = $cbd[$i-1]\n";
		    warn "addb = $addb\n";
		    warn "new bases = $nb\n";
#		    warn Dumper $bm;
		    warn "====\n";
		    push @n, '@bases '.$nb;
		}
		
		while ($i < $#cbd && $cbd[$i] !~ /^\@(?:form|sense)/) {
		    push @n, $cbd[$i++];
		}
		while ($cbd[$i] =~ /^\@form/) {
		    push @n, $cbd[$i++];
		}
		if ($emap{'form'}) {
		    if ($emap{'#basemap'}) {
			my %bmap = %{${$emap{'#basemap'}}};
			foreach my $f (@{${$emap{'form'}}}) {
			    my($b) = ($f =~ m#\s/(\S+)#);
			    if ($b && $bmap{$b}) {
				$f =~ s#\s/(\S+)# /$bmap{$b}#;
			    }
			    push @n, $f;
			}
		    } else {
			push @n, @{${$emap{'form'}}};
		    }
		}
		while ($i < $#cbd &&$cbd[$i] !~ /^\@sense/) {
		    push @n, $cbd[$i++];
		}
		while ($cbd[$i] =~ /^\@sense/) {
		    push @n, $cbd[$i++];
		}
		if ($emap{'sense'}) {
		    push @n, @{${$emap{'sense'}}};
		}
	    }
	    while ($i < $#cbd && $cbd[$i] !~ /^\@end\s+entry/) {
		push @n, $cbd[$i++];
	    }
	    push @n, $cbd[$i];
	} else {
	    push @n, $cbd[$i];
	}
    }
    if ($currmap{'#entries'}) {
	my @newentries = @{$currmap{'#entries'}};
	for (@newentries) {
	    my @e = split(/\cA/,$_);
	    push @n, @e, '';
	}
    }
    @n;
}

sub map_apply_sig {
    my ($args,$s) = @_;
    chomp($s);
    $s =~ s/\t(.*$)//;
    my $t = $1;
    if ($s =~ /^\{/) {
	my($psu,$sigs) = ($s =~ /^(.*?)::(.*?)$/);
	my @s = ();
	foreach my $s (split(/\+\+/,$sigs)) {
#	    warn "map_apply_sig_sub in psu\n";
	    push @s, map_apply_sig_sub($args,$s);
	}
	$s = $psu.'::'.join('++',@s);
    } elsif ($s =~ /\&\&/) {
	my @s = ();
	foreach my $s2 (split(/\&\&/,$s)) {
#	    warn "map_apply_sig_sub in cof\n";
	    push @s, map_apply_sig_sub($args,$s2);
	}
	$s = join('&&',@s);
    } else {
#	warn "map_apply_sig_sub non-psu\n";
	$s = map_apply_sig_sub($args,$s);
    }
    "$s\t$t\n";
}

sub map_apply_sig_sub {
    my ($args, $s) = @_;
    my $orig = $s;
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
	    $f{'epos'} = $f2{'epos'};
	}
	if ($$args{'#lang'}) { # could add && $$args{'coerce-lang'} here
	    $f{'lang'} = $$args{'#lang'};
	}
	$f{'form'} =~ s/^%.*?://;
	$f{'base'} =~ s/^%.*?:// if $f{'base'};
	if ($currmap{$key,'base'}) {
	    if ((my $mbase = ${$currmap{$key,'base'}}{$f{'base'}})) {
		$f{'base'} = $$mbase;
	    }
	}
	$s = serialize_sig(%f);
	my $op = (($orig eq $s) ? '==' : '->');
#	warn "map_apply_sig_sub $orig $op $s\n";
    } elsif ($$args{'#lang'}) {
	$s =~ s/\%.*?:/\%$$args{'#lang'}:/
    }
    $s;
}

sub map_load {
    my ($map,$for) = @_;
    $marshalling = $for;
    my %map = ();
    open(M, $map) || die "$0: failed to open map file for read\n";
    while (<M>) {
	next if /^\s*$/ || /^\#/;
	chomp;
	s/^.*?:\d+:\s+//; # remove file:line: info
	s/:add base /add base/;
#	warn "$_\n" if /add base/;
	if (/^(add|cut|fix|map|new)\s+(.*?)$/) {
	    my ($cmd,$arg) = ($1,$2);
	    my($what,$from,$to) = ();
	    if (/=>/) {
		unless (($what,$from,$to) 
			= ($arg =~ /^(entry|parts|sense|bases|base|form)\s+(.*?)\s*=>\s*(.*?)\s*$/)) {
		    warn "$map:$.: syntax error: bad field\n";
		    next;
		}
	    } else {
		unless (($what,$from) = ($arg =~ /^(entry|parts|sense|bases|base|form)\s+(.*?)\s*$/)) {
		    warn "$map:$.: syntax error: bad field\n";
		    next;
		}
	    }
	    $from =~ s/\s+(\[.*?\])\s+/$1/;
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
		    } elsif ($marshalling eq 'glos') {
			my($what,$from,$to) = @$mapdata;		       
			if ($to =~ /^\@(\S+)/) {
			    my $tag = $1;
			    if ($to =~ /^$acd_rx?\@entry/) {
				push @{$map{'#entries'}}, $to;
			    } else {
				push @{${$map{$mapkey}{$tag}}}, $to;
			    }
			} elsif ($what eq 'add' && $from eq 'base') {
			    push @{${$map{$mapkey}}{$from}}, $to;
			} else {
			    ${${$map{$mapkey}{'#basemap'}}}{$from} = $to;
			}
		    } else {
			die "$0: no support for marshalling type '$marshalling'\n";
		    }
		}
	    } else {
		warn "$map:$.: cmd/field combination '$combo' not handled\n"
		    unless exists $combofuncs{$combo};
	    }
	} elsif (/: add base (.*)\s=>\s+(\S+)/) {
	    # now handled above
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

sub addparts {
    return undef if $marshalling eq 'sigs';
    my($k,$f,$to) = @_;
    ($k,['add','parts',$to]);
}

sub addbase {
    return undef if $marshalling eq 'sigs';
# This is not needed after all    
#    my($k,$f,$to) = @_;
#    warn "addbase $k $f $to\n";
#    ($k,['add','base',$to]);
}

sub addsense {
    return undef if $marshalling eq 'sigs';
    my($k,$f,$to) = @_;
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
    $to =~ /^(.*?)\s+~\s+(.*?)$/;
    ($k,['base',$1,$2]);
}

1;

