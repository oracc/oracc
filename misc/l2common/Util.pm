package ORACC::L2GLO::Util;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/parse_sig/;

use warnings; use strict; use open 'utf8'; use utf8;
use lib '@@ORACC@@/lib';

use constant {
    PC=>0,
    EDa=>1,
    EDb=>2,
    Eb=>3,
    OAk=>4,
    LA=>5,
    UR=>6,
    OB=>7,
    LS=>8,
    XX=>9,
    OA=>10,
    MA=>11,
    MB=>12,
    NA=>13,
    NB=>14,
    PER=>15,
    SEL=>16,
    PAR=>17,
    XXX=>18,   
};

my @overview_periods = (
    'Archaic', 'ED IIIa', 'ED IIIb', 'Ebla', 'Old Akk.', 'Gudea', 'Ur III', 'OB', 'Post-OB', 'unplaced'
    );

my %periods_sux = (
    'ED I-II'=>PC,
    'ED IIIa'=>EDa,
    'Early Dynastic IIIa'=>EDa,
    'Early Dynastic'=>EDb,
    'ED IIIb'=>EDb,
    'Early Dynastic IIIb'=>EDb,
    'Ebla'=>Eb,
    'Old Akkadian'=>OAk,
    'Lagash II'=>LA,
    'Lagaš II'=>LA,
    'Ur III'=>UR,
    'Early Old Babylonian'=>OB,
    'Old Assyrian'=>OB,
    'Old Babylonian'=>OB,
    'Middle Assyrian'=>LS,
    'Middle Babylonian'=>LS,
    'First Millennium'=>LS,
    'Neo-Babylonian'=>LS,
    'Neo-Assyrian'=>LS,
    'Hellenistic'=>LS,
    'Uncertain'=>XX,
    'uncertain'=>XX,
    'Unknown'=>XX,
    'unknown'=>XX
    );

my %periods_qqq = (
    'ED I-II'=>PC,
    'ED IIIa'=>EDa,
    'Early Dynastic IIIa'=>EDa,
    'Early Dynastic'=>EDb,
    'ED IIIb'=>EDb,
    'Early Dynastic IIIb'=>EDb,
    'Ebla'=>Eb,
    'Old Akkadian'=>OAk,
    'Lagash II'=>LA,
    'Lagaš II'=>LA,
    'Ur III'=>UR,
    'Early Old Babylonian'=>OB,
    'Old Assyrian'=>OA,
    'Old Babylonian'=>OB,
    'Middle Babylonian'=>MB,
    'Middle Assyrian'=>MA,
    'First Millennium'=>NA,
    'Neo-Babylonian'=>NB,
    'Neo-Assyrian'=>NA,
    'Achaemenid'=>PER,
    'Persian'=>PER,
    'Hellenistic'=>SEL,
    'Seleucid'=>SEL,
    'Parthian'=>PAR,
    'unknown'=>XXX,
    'Unknown'=>XXX,
    'uncertain'=>XXX,
    'Uncertain'=>XXX
    );

my $tmp_debug = 0;
my $tmp_file = '';
my $tmp_suffix = 0;
my %warned_periods = ();
my %xis_periods = ();

$ORACC::L2GLO::Util::project = '';

@ORACC::L2GLO::Util::instfields = qw/norm base cont morph morph2/;

%ORACC::L2GLO::Util::fieldchars = (
    norm => '$',
    base => '/',
    cont => '+',
    morph => '#',
    morph2 => '##',
    stem => '*',
    flags => '!',
    );

%ORACC::L2GLO::Util::rws_map = (
    EG => '',
    MB => 'akk-x-midbab',
    OA => 'akk-x-oldass',
    OB => 'akk-x-oldbab',
    SB => 'akk-x-stdbab',
    NA => 'akk-x-neoass',
    NB => 'akk-x-neoass',
    );

%ORACC::L2GLO::Util::qpnlangs = (
    qpn=>'qpn',
    AN=>'qpn-x-places', 
    CN=>'qpn-x-celest',
    DN=>'qpn-x-divine',
    EN=>'qpn-x-ethnic',
    FN=>'qpn-x-places', 
    GN=>'qpn-x-places', 
    LN=>'qpn-x-lineage',
    MN=>'qpn-x-months',
    ON=>'qpn-x-object',
    PN=>'qpn-x-people', 
    QN=>'qpn-x-places', 
    RN=>'qpn-x-people',
    SN=>'qpn-x-places', 
    TN=>'qpn-x-temple',
    WN=>'qpn-x-waters',
    YN=>'qpn-x-ynames',
    ZN=>'qpn-x-snames',
    );

%ORACC::L2GLO::Util::long_lang = (
    akk=>'Akkadian',
    arc=>'Aramaic',
    egy=>'Egyptian',
    qca=>'Canaanite',
    qeb=>'Eblaite',
    qpc=>'Proto-Cuneiform',
    qpn=>'Proper Noun',
    sux=>'Sumerian',
    'sux-es'=>'Emesal',
    uga=>'Ugaritic',
    xhu=>'Hurrian',
    xur=>'Urartian',
    en=>'English',
    hu=>'Hungarian'
    );

%ORACC::L2GLO::Util::short_lang = (
    akk=>'Akk',
    arc=>'Aram',
    egy=>'Egy',
    qeb=>'Ebla',
    qca=>'Can',
    qpn=>'NN',
    qpn=>'NN',
    sux=>'Sum',
    'sux-es'=>'ES',
    uga=>'Uga',
    xhu=>'Hur',
    xur=>'Ura',
    );

%ORACC::L2GLO::Util::qpnchoices = (
    'qpn-x-celest'=>'p:CN',
    'qpn-x-divine'=>'p:DN',
    'qpn-x-ethnic'=>'p:EN',
    'qpn-x-lineage'=>'p:LN',
    'qpn-x-months'=>'p:MN',
    'qpn-x-object'=>'p:ON',
    'qpn-x-places'=>'p:AN,p:FN,p:GN,p:QN,p:SN',
    'qpn-x-people'=>'p:PN,p:RN',
    'qpn-x-temple'=>'p:TN',
    'qpn-x-waters'=>'p:WN',
    'qpn-x-ynames'=>'p:YN',
    'qpn-x-snames'=>'p:ZN',
    );

sub
parse_psu {
    my $psu = shift;
    my $psu_orig = $psu;
    my $first = 1;
    my %psu_sig = ();
    
    if ($psu =~ s/\{(.*?)\s+\+=\s+/{/) {
	my $psutmp = $1;
	if ($psutmp =~ /^(.*?) = (.*?)$/) {
	    $psu_sig{'form'} = $1;
	    $psu_sig{'ngram'} = $2;
	}
    } else {
	s/\{(.*?)\s+\+=\s+/{/;
	$psu_sig{'ngram'} = $1;
    }
    
    $psu =~ s#^\{(.*?)\[(.*?)//(.*?)\](.*?)'(.*?)\}::##;
    @psu_sig{qw/cf gw sense pos epos/} = ($1,$2,$3,$4,$5);
    my %parts = ();
    foreach my $sig (split(/\+\+/, $psu)) {
	if ($sig =~ /^\{/) {
	    warn "$.: parse_psu: nested psu: found `$sig' in `$psu_orig'\n";
	    return ();
	}
	push @{$psu_sig{'part_sigs'}}, $sig;
	my %sig = (parse_sig($sig));
	push @{$psu_sig{'parts'}}, "$sig{'cf'}\[$sig{'gw'}\]$sig{'pos'}";
	push @{$psu_sig{'sense-parts'}}, "$sig{'cf'}\[$sig{'gw'}//$sig{'sense'}\]$sig{'pos'}'$sig{'epos'}";

	foreach my $part (qw/norm base cont morph morph2/) {
	    if ($sig{$part}) {
		push @{$parts{$part}}, $sig{$part};
	    }
	}

	if ($first) {
	    $psu_sig{'proj'} = $sig{'proj'};
	    $psu_sig{'lang'} = $sig{'lang'};
	    $first = 0;
	}
	%sig = ();
    }
    foreach my $part (qw/form norm base cont stem morph morph2/) {
	$psu_sig{$part} = join(' ', @{$parts{$part}}) if $parts{$part};
    }
    ( %psu_sig );
}

sub
parse_sig {
    my $sig = shift;

    return parse_psu($sig) if $sig =~ /^\{/;

#    warn "$sig\n";

    local($_) = $sig;
    my %x = ();
    if (/^\@/) {
	@x{'proj','lang','form'} = /\@(.*?)\%(.*?)\:(.*?)=/;
	if ($x{'lang'} =~ /^sux/) {
	    s#V/([ti])#V\cA$1#g;
	}
	@x{'cf','gw','sense','pos','epos'} = /=(.*?)\[(.*?)\/\/(.*?)\](.*?)\'(.*?)(?:[\$\t\/]|$)/;
#	warn "bad sig $_\n" unless $x{'cf'};
	$x{'pos'} =~ tr,\cA,/,;
	$x{'epos'} =~ tr,\cA,/,;
    } else {
    }

    if (s/\!(0x.[0-9a-f]+)//i) {
	$x{'flags'} = $1;
    }

    if (s/\t^(\d+)//) {
	$x{'freq'} = $1;
	if (s/\t(.*)$//) {
	    $x{'instances'} = $1
		if $1;
	}
    }

#    warn "lang=$x{'lang'}\n";
    my $s949 = ($x{'lang'} =~ /-949/ ? "-949" : '');
    my $baselang = $x{'lang'};
    $baselang =~ s/-.*$/$s949/;
#    $x{'form'} = "\%$baselang\:$x{'form'}" if $x{'form'};

    # must keep script tags in parsed signature

    $x{'form'} = "\%$x{'lang'}\:$x{'form'}" if $x{'form'};

#    warn "x{'form'} set to $x{'form'}\n";

#    warn "x{form} = $x{'form'}\n";
    if ($x{'proj'}) {
	s/^.*?\]//; # delete everything up to end of GW//SENSE
	s/^.*?([\$\/])/$1/; # delete anything else up to NORM or BASE
	if (s/^\$(.+?)([\/#\t]|$)/$2/) {
	    $x{'norm'} = $1;
	}
	# Map + in compounds (|...+...|) to \cA
	1 while s#(/.*?\|[^\|]+?)\+(.*?\|)#$1\cA$2#;
	1 while s#\{\+#\{\cA#g;

	# Remove empty continuations
	s/\+0\s*//;

	if (s/^\/(.+?)([\+#\t]|$)/$2/) {
	    my $b = $1;
	    $b =~ tr/\cA/+/;
	    $x{'base'} = "\%$baselang\:$b";
	}
	if (s/^\+(-.+?)([#\t]|$)/$2/) {
	    $x{'cont'} = $1;
	}
	if (s/^\*.+?([#\t]|$)/$2/) {
	    $x{'stem'} = $1;
	}
	if (s/^\#(.+?)([#\t]|$)/$2/) {
	    $x{'morph'} = $1;
	}
	if (s/^\#\#(.+?)([#\t]|$)/$2/) {
	    $x{'morph2'} = $1;
	}
	( %x );
    } else {
	s/\t.*$//;
	warn "sig-g2x.plx: $.: bad parse in $_\n"
    }
}

sub
serialize_sig {
    my %s = @_;
    my $sig = '@'.$s{'proj'}.'%'.$s{'lang'}.':'."$s{'form'}=$s{'cf'}\[$s{'gw'}//$s{'sense'}\]$s{'pos'}'$s{'epos'}";
    foreach my $f (@ORACC::L2GLO::Util::instfields) {
	$sig .= "$ORACC::L2GLO::Util::fieldchars{$f}$s{$f}" if $s{$f};
    }
    $sig;
}

sub
xis_overview {
    my @p = @_;
    my @pd;
    for (my $i = 0; $i <= $#p; ++$i) {
	if ($p[$i]) {
	    push @pd, "$overview_periods[$i] \[$p[$i]\]";
	}
    }
    join('; ', @pd);
}

sub
xis_periods {
    my $tmp_file = "/tmp/$$\_$tmp_suffix.x";
    my $ret;
    open(X,">$tmp_file"); print X join("\n",@_),"\n"; close(X);
    $ret = `pd -c -p $ORACC::L2GLO::Util::project -l $tmp_file 2>/dev/null`;
    unlink $tmp_file unless $tmp_debug;
    ++$tmp_suffix;
    $ret;
}

sub
xis_stats {
    my ($lang,@refs) = @_;
    my @s = (0) x (XXX+1);

    my @rrefs = grep /^\@\@/, @refs;
    @refs = grep !/^\@\@/, @refs;

    # First look up the figures for actual references
    my $pd = '';
    $pd = xis_periods(@refs) if $#refs >= 0;
    foreach my $p (split(/;/, $pd)) {
	$p =~ /^\s*(.*?)\s+\[(\d+)/;
	my $ptmp = $1;
	if ($lang =~ /^sux/) {
	    if (defined $periods_sux{$ptmp}) {
		$s[$periods_sux{$ptmp}] += $2 if $2;
	    } else {
		my $per = $1 || '';
		warn "ORACC::L2GLO::Util: unknown SUMERIAN period `$per'\n"
		    unless $warned_periods{$per}++;
	    }
	} else {
	    if (defined $periods_qqq{$ptmp}) {
		$s[$periods_qqq{$ptmp}] += $2;
	    } else {
		warn "ORACC::L2GLO::Util: unknown GENERIC period `$1'\n"
		    unless $warned_periods{$1}++;
	    }
	}
    }

    # Now merge in the figures from meta-references
    xis_sum_pd(\@s, @rrefs) if $#rrefs >= 0;
    $pd = xis_overview(@s) unless $pd;
    ($pd, @s);
}

sub
xis_sum_pd {
    my($sref,@rrefs) = @_;
    my @rarrays = ();
    for (my $i = 0; $i <= $#rrefs; ++$i) {
	my $r = $rrefs[$i];
	$r =~ s/^.*?;\%\d+;//;
	$rarrays[$i] = [ split( /:/, $r) ];
    }
    for (my $i = 0; $i <= $#$sref; ++$i) {
	for (my $j = 0; $j <= $#rrefs; ++$j) {
	    $$sref[$i] += ${$rarrays[$j]}[$i];
	}
    }
}

1;
