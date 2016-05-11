#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}";

my @codeset = ();
my %fields = ();
my @periods = ();
my %periods = ();
my @places = ();
my %places = ();
my @rulers = ();
my %rulers = ();
my %scodes = ();

my $verbose = 0;
my $excode = 1;

my %period_codes = (
    'Early Dynastic'=>1000000000000,
    'rim:E1'=>1000000000000,
    'Old Akkadian'  =>2000000000000,
    'rim:E2'  =>2000000000000,
    'Lagaš II'      =>3000000000000,
    'rim:E3/1'      =>3000000000000,
    'Ur III'        =>4000000000000,
    'rim:E3/2'      =>4000000000000,
    'Old Babylonian'=>5000000000000,
    'rim:E4'=>5000000000000,
    'rim:A'=>6000000000000,
    'rim:AP'=>7000000000000,
    'rim:SE'=>8000000000000,
    );

my %a_codes = (
    'AA'=>97,
    'Akkadian Version'=>98,
    );

while (<>) {
    chomp;
    my %r = ();
    %fields = ();
    @fields{qw/Q rim period place designation/} = split(/\t/, $_);
    $fields{'designation'} =~ s/\s+\[.*$//;
    if ($fields{'rim'} && $fields{'rim'} =~ /^ri/) {
	$fields{'rim'} =~ s#E\.1#E1# if $fields{'rim'} =~ m#^rim:E.1.9.5.31add#;
	$fields{'rim'} =~ s#rim:3#rim:E3#;
	$fields{'rim'} =~ s#E3/2/1#E3/2.1# if $fields{'rim'} =~ m#^rim:E3/2/1#;
	$fields{'rim'} =~ s#32#33# if $fields{'Q'} eq 'Q001830';
	$fields{'rim'} .= 'a' if $fields{'Q'} eq 'Q001404';
	$fields{'rim'} =~ s/E4.12.(11|5)/E4.2.14.$1/;
	$fields{'rim'} =~ s/\.0\.(\d+)$/.99.$1/;
	$fields{'rim'} =~ s/\.add(\d+)$/.$1add/;
	$fields{'rim'} =~ s/\.add(\d+)\./.$1./;
    } else {
	$fields{'designation'} .= ' 01' if $fields{'designation'} =~ /^Anadarum|Dabrum/;
	$fields{'rim'} = rim_from_designation($fields{'designation'});
    }
 
    @r{qw/period place ruler text/} = split(/\./, $fields{'rim'});
    $r{'place'} = 0 unless $r{'place'};
    if ($r{'place'} =~ s/a$//) {
	$r{'place'} *= 2;
	$r{'place'} += 1;
    } else {
	$r{'place'} *= 2;
    }
    $r{'ruler'} = 0 unless $r{'ruler'};
    if ($r{'ruler'} =~ s/a$//) {
	$r{'ruler'} *= 2;
	$r{'ruler'} += 1;
    } else {
	$r{'ruler'} *= 2;
    }
    warn "place is not numeric :$r{'place'}:\n" unless $r{'place'} =~ /^\d+$/;
    if (defined $r{'period'}) {
        my $period = $period_codes{$r{'period'}};
	warn "no period_code for `$r{'period'}'\n" unless $period;
	my $place = $r{'place'} * 10000000000;
	my $ruler = $r{'ruler'} * 10000000;
	my $text = $r{'text'};
	warn "$fields{'Q'} == $fields{'rim'} == $fields{'designation'}\n"
	    if $verbose;
	unless (defined $text) {
	    if ($fields{'rim'} eq 'rim:E3/1.1.7') {
		if ($fields{'designation'} =~ /Statue/) {
		    $fields{'designation'} =~ /\s(\S+)$/;
		    $text = "150$1";
		} elsif ($fields{'designation'} =~ /Cylinder/) {
		    $fields{'designation'} =~ /\s(\S+)$/;
		    my $n = $1;
		    $n =~ s/^(\d+).*$/$1/;
		    $text = ''.(201 + $n);
		} else {
		    warn "bad 3/1.1.7\n";
		}
	    } else {
		warn "no text component in `$fields{'rim'}'\n";
		$text = '999';
	    }
	}
	$text =~ s/add$//;
	my ($n,$a) = ($text,'');
	unless ($text =~ /\d$/) {
	    ($n,$a) = ($text =~ /^(\d+?)\s*([^\d]+)$/);
	}
	unless ($n) {
	    warn "unusual text name `$text'\n" unless $n eq '0';
	    $n = 0;
	}
	$n *= 100;
	my $acode = 0;
	if ($a) {
	    if ($a =~ /^[a-zA-Z]$/) {
		$acode = ord($a) - ord('A');
	    } elsif ($a = ', ex') {
		$acode = $excode++;
	    } else {
		warn "no a_code for `$a'\n" unless defined $a_codes{$a};
		$acode = $a_codes{$a} ? $a_codes{$a} : 99;
	    }
	} else {
	    $acode = 0;
	}
	my $s = $period + $place + $ruler + $n + $acode;
	if ($scodes{$s}) {
	    my $x = 0;
	    for ($x = 1; $x < 99; ++$x) {
		last unless $scodes{$s+$x};
	    }
	    if ($x < 99) {
		$s += $x;
	    } else {
		my @eQ = @{$scodes{$s}};
		warn "duplicate sort code $s ($fields{'Q'} and @eQ)\n";
	    }
	}
	push @{$scodes{$s}}, $fields{'Q'};
	# print "$fields{'Q'}\t$s\n";
	push @codeset, [ $s , $fields{'Q'} ];
    } else {
	if ($fields{'rim'}) {
	    warn "royal-sort.plx: unparseable entry '$fields{'rim'}\n";
	} else {
	    # warn "royal-sort.plx: empty 'rim' field\n";
	}
    }
}

@codeset = sort { $$a[0] <=> $$b[0] } @codeset;
my $x = 1000000;
foreach my $c (@codeset) {
    print "$$c[1]\t$x\n";
    ++$x;
}

############################################################################################################################
    
sub
rim_from_designation {
    my $des = shift;
    my $ruler = '';
    my $text = '';
    my $rim = '';
    my %tab = (
	'Ur-Namma'=>         [ 'rim:E3/2.1.1' , 'Ur III' ],
	'Šulgi'=>            [ 'rim:E3/2.1.2' , 'Ur III' ],
	'Amar-Suen'=>        [ 'rim:E3/2.1.3' , 'Ur III' ],
	'Amar-Suena'=>       [ 'rim:E3/2.1.3' , 'Ur III' ],
	'Šu-Suen'=>          [ 'rim:E3/2.1.4' , 'Ur III' ],
	'Sin-abušu' =>       [ 'rim:E3/2.1.9' , 'Ur III' ],
	'Anonymous Ešnuna'=> [ 'rim:E3/2.3.99' ,  'Ur III' ],
	'Idattu' =>          [ 'rim:E3/2.7.1' , 'Ur III' ],
	'Lugal-ayaŋu' =>     [ 'rim:E3/2.7.2' , 'Ur III' ],
	'Unattributed Ur III' => [ 'rim:E3/2.99.0' , 'Ur III' ],
	'Anonymous'=>        [ 'rim:E1.99.0' ,  'Early Dynastic' ],
	'Anonymous unclear' => [ 'rim:E1.99.0', 'Early Dynastic' ],
	'Unattributed'=>     [ 'rim:E1.99.0' ,  'Early Dynastic' ],
	'Anonymous Adab'=>   [ 'rim:E1.1.99' ,  'Early Dynastic' ],
	'Anonymous Agrab'=>  [ 'rim:E1.1a.99' ,  'Early Dynastic' ],
	'Anonymous Fara'=>   [ 'rim:E1.6.99' ,  'Early Dynastic' ],
	'Anonymous Khafaje'=>[ 'rim:E1.7.99' ,  'Early Dynastic' ],
	'Anonymous Kiš'=>    [ 'rim:E1.8.99' ,  'Early Dynastic' ],
	'Anonymous Kiš?'=>    [ 'rim:E1.8.99' ,  'Early Dynastic' ],
	'Anonymous Lagaš'=>  [ 'rim:E1.9.99' ,  'Early Dynastic' ],
	'Anonymous Nippur'=> [ 'rim:E1.11.99' ,  'Early Dynastic' ],
	'Anonymous Sippar'=> [ 'rim:E1.11a.99' ,  'Early Dynastic' ],
	'Anonymous Umma'=>   [ 'rim:E1.12.99' ,  'Early Dynastic' ],
	'Anonymous Ur'=>     [ 'rim:E1.13.99' ,  'Early Dynastic' ],
	'Anonymous Uruk'=>   [ 'rim:E1.14.99' ,  'Early Dynastic' ],
	'An-Uruk'=>          [ 'rim:E1.14.99' ,  'Early Dynastic' ],
	'URU-KA-gina'=>      [ 'rim:E1.9.9'   ,  'Early Dynastic' ],
	'Arad-Dumuzida'=>    [ 'rim:E1.99.1'  ,  'Early Dynastic' ],
	'Bara-sagnudi' =>    [ 'rim:E1.12.8'  , 'Early Dynastic' ],
	'Geme-Bau' =>        [ 'rim:E1.99.2'  , 'Early Dynastic' ],
	'Gudea' =>           [ 'rim:E3/1.1.7' , 'Lagaš II' ],
	'Gungunum' => [ 'rim:E4.2.5' , 'Old Babylonian' ],
	'Lagaš II Unidentified' => [ 'rim:E3/1.99.0' , 'Lagaš II' ],
	'Unattributed Sargonic' => [ 'rim:E2.99.0' , 'Old Akkadian' ],
	'Unattributed Girsu' => [ 'rim:E3/1.99.0' , 'Lagaš II' ],
	'Lipit-Eštar' => [ 'rim:E4.1.5' , 'Old Babylonian' ],
	'Lugal-kigine-dudu' => [ 'rim:E1.14.14' , 'Early Dynastic' ],
	'Mes-kigala' => [ 'rim:E1.1.9' , 'Early Dynastic' ],
	'Šaratigubisin' => [ 'rim:E2.8.2' , 'Old Akkadian' ],
	'Tulili' => [ 'rim:E1.99.3' , 'Early Dynastic' ],
	'Ur-Bau' => [ 'rim:E3/1.1.6' , 'Lagaš II' ],
	'Ur-Numušda' => [ 'rim:E3/2.7.4' , 'Ur III' ],
	'Tiglath-pileser III' => [ 'rim:AP.1.1' , 'Neo-Assyrian' ],
	'Shalmaneser V' => [ 'rim:AP.1.2' , 'Neo-Assyrian' ],
	'Sargon' => [ 'rim:AP.1.3' , 'Neo-Assyrian' ],
	'Sargon II' => [ 'rim:AP.1.3' , 'Neo-Assyrian' ],
	'Sennacherib' => [ 'rim:AP.1.4' , 'Neo-Assyrian' ],
	'Esarhaddon' => [ 'rim:AP.1.5' , 'Neo-Assyrian' ],
	'Ashurbanipal' => [ 'rim:AP.1.6' , 'Neo-Assyrian' ],
	'Sin-šarru-iškun' => [ 'rim:AP.1.7' , 'Neo-Assyrian' ],
	'Aššur-etel-ilani' => [ 'rim:AP.1.8' , 'Neo-Assyrian' ],
	'Antiochus I' => [ 'rim:SE.1.1' , 'Seleucid' ],
	'Kudur-mabuk' => [ 'rim:E4.2.13a' , 'Old Babylonian' ],
	'E-pa-e' => [ 'rim:E1.99.3', 'Early Dynastic' ],
	'Nigir-eša-tum' => [ 'rim:E1.99.4', 'Early Dynastic' ],
	'Ur-ešlila' =>  [ 'rim:E1.99.5', 'Early Dynastic' ],
	'Lugal-ayamu' => [ 'rim:E2.9.4', 'Old Akkadian' ],
	'En-metena' => [ 'rim:E1.9.5', 'Early Dynastic' ],
	'Anadarum' => [ 'rim:E1.10.01', 'Early Dynastic' ],
	'Dabrum' => [ 'rim:E1.10.03', 'Early Dynastic' ],
	'Ebih-il' => [ 'rim:E1.10.04', 'Early Dynastic' ],
	'Iddin-narim' => [ 'rim:E1.10.05', 'Early Dynastic' ],
	'Ipum-šar' => [ 'rim:E1.10.06', 'Early Dynastic' ],
	'Kinuri' => [ 'rim:E1.10.07', 'Early Dynastic' ],
	'Maširum' => [ 'rim:E1.10.08', 'Early Dynastic' ],
	'Nani' => [ 'rim:E1.10.09', 'Early Dynastic' ],
	'Šalim' => [ 'rim:E1.10.10', 'Early Dynastic' ],
	'Arši-aha' => [ 'rim:E1.10.11', 'Early Dynastic' ],
	'Udumes' => [ 'rim:E1.99.00', 'Early Dynastic' ],
	'Anonymous Mari' => [ 'rim:E1.10.99', 'Early Dynastic' ],
	);
    $des =~ s/,.*$//;
    ($ruler,$text) = ($des =~ /^(.*?)\s(\S+)$/);
    if ($des =~ /Unattri/) {
	($ruler,$text) = ($des =~ /^(.*?)\s+(\d+|YN)$/);
    } elsif ($text eq 'seal') {
	$text = '1001' if $text eq 'seal';
    }
    if ($tab{$ruler}) {
	$rim = "${$tab{$ruler}}[0].$text";
#	$fields{'period'} = ${$tab{$ruler}}[1];
#	$fields{'place'} = 'Girsu';
    } elsif ($des =~ /building of Nin/) {
	$rim = "rim:E3/1.1.7.200";
#	$fields{'place'} = 'Girsu';
    } elsif ($des =~ /^Laws of Lip/) {
	$rim = "rim:E4.1.5.9";
#	$fields{'period'} = 'Old Babylonian';
#	$fields{'place'} = 'Isin';
    } else {
	warn "no table entry for `$ruler'\n";
    }
    $rim;
}

1;
