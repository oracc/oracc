package ORACC::L2GLO::Util;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/parse_sig serialize_sig/;

use warnings; use strict; use open 'utf8'; use utf8;
use lib '@@ORACC@@/lib';

no strict;
use constant {
    PC=>0,
    ED12=>1,
    ED3a=>2,
    ED3b=>3,
    Eb=>4,
    OAk=>5,
    LA2=>6,
    UR3=>7,
    OB=>8,
    LS=>9,
    XX=>10,
    OA=>11,
    MA=>12,
    MB=>13,
    NA=>14,
    NB=>15,
    PER=>16,
    SEL=>17,
    PAR=>18,
    XXX=>19,
};

my @overview_periods = (
    'Archaic', 'ED IIIa', 'ED IIIb', 'Ebla', 'Old Akk.', 'Gudea', 'Ur III', 'OB', 'Post-OB', 'unplaced'
    );

my %periods_sux = (
    'Archaic'=>PC,
    'ED I-II'=>PC,
    'ED IIIa'=>ED3a,
    'Early Dynastic IIIa'=>ED3a,
    'Early Dynastic'=>ED3b,
    'ED IIIb'=>ED3b,
    'Early Dynastic IIIb'=>ED3b,
    'Ebla'=>Eb,
    'Old Akkadian'=>OAk,
    'Lagash II'=>LA2,
    'Lagaš II'=>LA2,
    'Ur III'=>UR3,
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

use strict;

my $tmp_debug = 0;
my $tmp_file = '';
my $tmp_suffix = 0;
my %warned_periods = ();
my %xis_periods = ();

$ORACC::L2GLO::Util::project = '';

@ORACC::L2GLO::Util::instfields = qw/norm base cont morph morph2 stem rws/;

%ORACC::L2GLO::Util::fieldchars = (
    norm => '$',
    base => '/',
    cont => '+',
    morph => '#',
    morph2 => '##',
    stem => '*',
    rws => '@',
    flags => '!',
    );

## Watchme: rws_map had EG=>''

%ORACC::L2GLO::Util::rws_map = (
    EG => 'sux',
    ES => 'sux-x-emesal',
    UGN=> 'sux-x-udganu',
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
    LN=>'qpn-x-lineag',
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
    qka=>'Kassite',
    'sux-es'=>'Emesal',
    uga=>'Ugaritic',
    xco=>'Chorasmian',
    xhu=>'Hurrian',
    xur=>'Urartian',
    ar=>'Arabic',
    en=>'English',
    fa=>'Farsi',
    he=>'Hebrew',
    hu=>'Hungarian'
    );

%ORACC::L2GLO::Util::short_lang = (
    akk=>'Akk',
    arc=>'Aram',
    egy=>'Egy',
    qeb=>'Ebla',
    qca=>'Can',
    qka=>'Kass',
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
    'qpn-x-lineag'=>'p:LN',
    'qpn-x-months'=>'p:MN',
    'qpn-x-object'=>'p:ON',
    'qpn-x-places'=>'p:AN,p:FN,p:GN,p:QN,p:SN',
    'qpn-x-people'=>'p:PN,p:RN',
    'qpn-x-temple'=>'p:TN',
    'qpn-x-waters'=>'p:WN',
    'qpn-x-ynames'=>'p:YN',
    'qpn-x-snames'=>'p:ZN',
    );

@ORACC::L2GLO::Util::instfields = qw/norm base cont morph morph2 stem rws/;

%ORACC::L2GLO::Util::fieldchars = (
    norm => '$',
    base => '/',
    cont => '+',
    morph => '#',
    morph2 => '##',
    stem => '*',
    rws => '@',
    flags => '!',
    );

## Watchme: rws_map had EG=>''

%ORACC::L2GLO::Util::rws_map = (
    EG => 'sux',
    ES => 'sux-x-emesal',
    UGN=> 'sux-x-udganu',
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
    LN=>'qpn-x-lineag',
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
    qka=>'Kassite',
    'sux-es'=>'Emesal',
    uga=>'Ugaritic',
    xco=>'Chorasmian',
    xhu=>'Hurrian',
    xur=>'Urartian',
    ar=>'Arabic',
    en=>'English',
    fa=>'Farsi',
    he=>'Hebrew',
    hu=>'Hungarian'
    );

%ORACC::L2GLO::Util::short_lang = (
    akk=>'Akk',
    arc=>'Aram',
    egy=>'Egy',
    qeb=>'Ebla',
    qca=>'Can',
    qka=>'Kass',
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
    'qpn-x-lineag'=>'p:LN',
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
#    } else {
#	s/\{(.*?)\s+\+=\s+/{/;
#	$psu_sig{'ngram'} = $1;
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
    my $noproj = 0;
    my $rwslang = '';

    return parse_psu($sig) if $sig =~ /^\{/;

    if ($sig =~ /\&\&/) {
	$sig =~ s/\&\&.*$//; # this is rough and ready but may be enough for the use of this routine
    }
    
#    warn "$sig\n";

    my($sig2,@rest) = split(/\t/,$sig);
    
    local($_) = $sig2;
    
    my %x = ();
    if (/^\@/) {
	@x{'proj','lang','form'} = (/\@(.*?)\%(.*?)\:(.*?)=/);
	s/^.*?=//;
    } elsif (s/^\%(.*?)://) {
	$x{'lang'} = $1;
    } else {
	$noproj = 1;
    }

    s#V/([ti])#V\cA$1#g; # protect V/i and V/t from // parse
    1 while s/\[([^\]]*?)'(.*?)\]/[$1\cB$2]/; # protect ' in GW/SENSE from EPOS parse
    
    if (/\'/) {
	@x{'cf','gw','sense','pos','epos'} = /^(.*?)\[(.*?)\/\/(.*?)\](.*?)\'(.*?)(?:[\@\$\#\t\/]|$)/;
	s/^.*?\'.*?(?=[\@\$\t\/]|$)//;
    } elsif (m,//,) {
	@x{'cf','gw','sense','pos'} = /^(.*?)\[(.*?)\/\/(.*?)\](.*?)(?:[\@\$\#\t\/]|$)/;
	s/^.*?\].*?(?=[\@\$\t\/]|$)//;
    } elsif (m,\[,) {
	@x{'cf','gw','pos'} = /^(.*?)\[(.*?)\](.*?)(?:[\@\$\t\/\#]|$)/;
	s/^.*?\].*?(?=[\@\$\t\/]|$)//;
    } elsif (/^[A-Za-z\/]+/) {
	$x{'pos'} = $_; # Assume it's PN/DN etc
	$_ = '';
    } else {
	# leave it to fail later;
    }

    $x{'gw'} =~ tr/\cB/'/ if $x{'gw'};
    $x{'sense'} =~ tr/\cB/'/ if $x{'sense'};
    $x{'pos'} =~ tr,\cA,/, if $x{'pos'};
    $x{'epos'} =~ tr,\cA,/, if $x{'epos'};

    # new 2021-12-16: after POS there can be an RWS like @EG, @ES, @UGN
    if (s/^\@([A-Z]+)//) {
	$x{'rws'} = $1;
       	$rwslang = $ORACC::L2GLO::Util::rws_map{$x{'rws'}};
    } else {
	$rwslang = '';
    }
    
    # unprotect remainder from V/[ti] protection because tum₂-u₃=tum[bring//to bring]V/t'V/t$tum!V/tum₂#~!V
    tr,\cA,/,;
    
    if (s/\!(0x.[0-9a-f]+)//i) {
	$x{'flags'} = $1;
    }

    if ($#rest >= 0) {
	if ($rest[0] =~ /^\d+/) {
	    $x{'freq'} = $rest[0];
	    $x{'instances'} = $rest[1];
	}
    }

    #    warn "lang=$x{'lang'}\n";
    my $baselang = '';
    if ($x{'lang'}) {
	my $s949 = ($x{'lang'} =~ /-949/ ? "-949" : '');
	$baselang = $x{'lang'};
	$baselang =~ s/-.*$/$s949/;
    }

    # must keep script tags in parsed signature
    my $flang = $rwslang ? $rwslang : $x{'lang'};
    $x{'form'} = "\%$flang\:$x{'form'}" if $x{'form'};
    
    #   if ($x{'proj'}) {
    #	s/^.*?\]//; # delete everything up to end of GW//SENSE
    #	s/^.*?([\$\/])/$1/; # delete anything else up to NORM or BASE

    # protect * in morphology; this is not robust enough yet
    ## no: s/\*([a-zšŋVA-Z]+(?:[:;!.,]))/\cB$1/g;
    ## This does morph-delimiter then * which must be a morph star and not a stem one
    s/([:;!.,])\*/$1\cB/g;
    
    if (s/^\$(.*?)([\*\/#\t]|$)/$2/) {
	$x{'norm'} = $1;
	$x{'norm'} =~ tr/\cB/*/;
    }
    tr/\cB/*/;
    
    # Map + in compounds (|...+...|) to \cA
    1 while s#(/.*?\|[^\|]+?)\+(.*?\|)#$1\cA$2#;
    1 while s#\{\+#\{\cA#g;

    # Remove empty continuations
    s/\+0\s*//;

    if (s/^\/(.+?)([#\t]|\+-|$)/$2/) {
	my $b = $1;
	$b =~ tr/\cA/+/;
	if ($b =~ /^\%\S+?:/) {
	    $x{'base'} = $b;
	} else {
	    $x{'base'} = "\%$baselang\:$b";
	}
    }
    if (/^\+/) {
	if (s/^\+(-.+?)(?=[#\t]|$)//) {
	    $x{'cont'} = $1;
	} else {
	    warn "sig-g2x.plx: $.: bad cont: `$_'\n";
	}
	s/^\+.*?(?=[#\t])|$//;
    }
    if (s/^\#(.+?)([#\t]|$)/$2/) {
	$x{'morph'} = $1;
    }
    if (s/^\#\#(.+?)([#\t]|$)/$2/) {
	$x{'morph2'} = $1;
    }
    if (s/^\*(.+?)(?:[#\t]|$)//) {
	$x{'stem'} = $1;
    }
    if (length $_) {
	my $ln = $. || '0';
	warn "$0: parse_sig: $ln: bad parse: sig=`$sig'; leftovers=`$_'\n";
    }

    # warn "$0: parse_sig: form = $x{'form'}\n";

    ( %x );
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
