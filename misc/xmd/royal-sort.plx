#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}";

my %fields = ();
my @periods = ();
my %periods = ();
my @places = ();
my %places = ();
my @rulers = ();
my %rulers = ();
my %scodes = ();

my $verbose = 0;

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
    if ($fields{'rim'} && $fields{'rim'} =~ /^ri/) {
	$fields{'rim'} =~ s#E3/2/1#E3/2.1# if $fields{'rim'} =~ m#^rim:E3/2/1#;
	$fields{'rim'} =~ s#32#33# if $fields{'Q'} eq 'Q001830';
	$fields{'rim'} .= 'a' if $fields{'Q'} eq 'Q001404';
	$fields{'rim'} =~ s/\.0\.(\d+)$/.99.$1/;
    } else {
	$fields{'rim'} = rim_from_designation($fields{'designation'});
    }

    @r{qw/period place ruler text/} = split(/\./, $fields{'rim'});
    if ($r{'period'}) {
        my $period = $period_codes{$r{'period'}};
	warn "no period_code for `$r{'period'}'\n" unless $period;
	my $place = $r{'place'} * 10000000000;
	my $ruler = $r{'ruler'} * 10000000;
	my $text = $r{'text'};
	warn "$fields{'Q'} == $fields{'rim'} == $fields{'designation'}\n"
	    if $verbose;
	unless ($text) {
	    if ($fields{'rim'} eq 'rim:E3/1.1.7') {
		if ($fields{'designation'} =~ /Statue/) {
		    $fields{'designation'} =~ /\s(\S+)$/;
		    $text = "150$1";
		} elsif ($fields{'designation'} =~ /Cylinder/) {
		    $fields{'designation'} =~ /\s(\S+)$/;
		    $text = ''.(201 + $1);
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
	    warn "unusual text name `$text'\n";
	    $n = 0;
	}
	$n *= 100;
	my $acode = 0;
	if ($a) {
	    if ($a =~ /^[a-zA-Z]$/) {
		$acode = ord($a) - ord('A');
	    } else {
		warn "no a_code for `$a'\n" unless defined $a_codes{$a};
		$acode = $a_codes{$a} ? $a_codes{$a} : 99;
	    }
	} else {
	    $acode = 0;
	}
	my $s = $period + $place + $ruler + $n + $acode;
	if ($scodes{$s}) {
	    my @eQ = @{$scodes{$s}};
	    warn "duplicate sort code $s ($fields{'Q'} and @eQ)\n";
	}
	push @{$scodes{$s}}, $fields{'Q'};
	print "$fields{'Q'}\t$s\n";
    } else {
	if ($fields{'rim'}) {
	    warn "royal-sort.plx: unparseable entry '$fields{'rim'}\n";
	} else {
	    warn "royal-sort.plx: empty 'rim' field\n";
	}
    }
}
    
sub
rim_from_designation {
    my $des = shift;
    my $ruler = '';
    my $text = '';
    my $rim = '';
    my %tab = (
	'Amar-Suen'=>        [ 'rim:E3/2.1.3' , 'Ur III' ],
	'Amar-Suena'=>       [ 'rim:E3/2.1.3' , 'Ur III' ],
	'Anonymous Ešnuna'=>  [ 'rim:E3/2.3.99' ,  'Ur III' ],
	'Anonymous Lagaš'=>  [ 'rim:E1.9.99' ,  'Early Dynastic' ],
	'Anonymous Nippur'=> [ 'rim:E1.11.99' ,  'Early Dynastic' ],
	'Anonymous Ur'=>     [ 'rim:E1.13.99' ,  'Early Dynastic' ],
	'Anonymous Uruk'=>   [ 'rim:E1.14.99' ,  'Early Dynastic' ],
	'Šu-Suen'=>          [ 'rim:E3/2.1.4' ,  'Ur III' ],
	'URU-KA-gina'=>      [ 'rim:E1.9.9'   ,  'Early Dynastic' ],
	'Arad-Dumuzida'=>    [ 'rim:E1.99.1'  ,  'Early Dynastic' ],
	'Bara-sagnudi' =>    [ 'rim:E1.12.8'  , 'Early Dynastic' ],
	'Geme-Bau' =>        [ 'rim:E1.99.2'  , 'Early Dynastic' ],
	'Gudea' =>           [ 'rim:E3/1.1.7' , 'Lagaš II' ],
	'Gungunum' => [ 'rim:E4.2.5' , 'Old Babylonian' ],
	'Idattu' =>   [ 'rim:E3/2.7.1' , 'Ur III' ],
	'Lipit-Eštar' => [ 'rim:E4.1.5' , 'Old Babylonian' ],
	'Lugal-ayaŋu' => [ 'rim:E3/2.7.2' , 'Ur III' ],
	'Lugal-kigine-dudu' => [ 'rim:E1.14.14' , 'Early Dynastic' ],
	'Mes-kigala' => [ 'rim:E1.1.9' , 'Early Dynastic' ],
	'Šaratigubisin' => [ 'rim:E2.8.2' , 'Old Akkadian' ],
	'Sin-abušu' => [ 'rim:E3/2.7.3' , 'Ur III' ],
	'Tulili' => [ 'rim:E1.99.3' , 'Early Dynastic' ],
	'Ur-Bau' => [ 'rim:E3/1.1.6' , 'Lagaš II' ],
	'Ur-Numušda' => [ 'rim:E3/2.7.4' , 'Ur III' ],
	);
    ($ruler,$text) = ($des =~ /^(.*?)\s(\S+)$/);
    $text = '1001' if $text eq 'seal';
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
