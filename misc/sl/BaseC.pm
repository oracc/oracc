package ORACC::SL::BaseC;
use warnings; use strict; use utf8; use open 'utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::Legacy::Sexify;
use IPC::Open2;

#### Implementation of ORACC::SL::Base using the C version of the sl db

binmode STDERR, ':utf8';
use constant UCODE => 1;
use constant UCHAR => 0;

use Encode;
use Fcntl;
use NDBM_File;

# Sign names that frustrate the parser
my %exceptions = ('|(4×ZA)×KUR|'=>'ŋeštae');

$ORACC::SL::report_all = 0;
@ORACC::SL::BaseC::last_tlit = '';
#@ORACC::SL::fixes_needed = ();

my $db_file = "@@ORACC@@/pub/ogsl/sl";
my $db_name = 'ogsl';

my %db = ();
my $loaded = 0;
my @messages = ();
my $pedantic = 0;
my $sl_pid = 0;
my %reported = ();
my $silent = 0;

sub
check {
    tlit_sig(@_);
#    my($context,$test,$deep) = @_;
#    warn join(':',caller()), "\n";
#    my $sig = _signature($context,tlitsplit($test,$deep));
#    warn "$test=>$sig\n";
#    $sig;
}

sub messages {
    my @tmp = @messages;
    @messages = ();
    return @tmp;
}

# return the previous value
sub pedantic {
    my $ret = $pedantic;
    if (defined $_[0]) {
	$pedantic = $_[0];
    } else {
	$pedantic = 1;
    }
    $ret;
}

sub same_tlit {
    my($context,$test,@against) = @_;
    my $test_sig = _signature($context,tlitsplit($test,1));
    foreach my $a (@against) {
	my $a_sig = _signature($context,tlitsplit($a,1));
	return $a if $test_sig eq $a_sig;
    }
    return undef;
}

sub tlit_sig {
    my($context,$test) = @_;
    my $s = _signature($context,tlitsplit($test,1));
    if ($s =~ /q/ && $test =~ /\|/) {
	$test =~ tr/|//d;
	$s = _signature($context,tlitsplit($test,1));
    }
    return $s;
}

# Use this when you know the input $test string is clean and
# conforms to OGSL sign name rules
sub tlit_sig_clean {
    my($context,$test) = @_;
    my $s = _signature($context,tlitsplit($test,0));
    if ($s =~ /q/ && $test =~ /\|/) {
	$test =~ tr/|//d;
	$s = _signature($context,tlitsplit($test,0));
    }
    return $s;
}

sub
init {
    return if $loaded;

    # check if the sldb exists
    my $db_file = "@@ORACC@@/pub/ogsl/sl";
    my $db_name = 'ogsl';
    if (-r "$db_file/$db_name.dbh") {
	$loaded = 1;
    } else {
	die "ORACC::SL::BaseC: no signlist database $db_file/$db_name\n";
    }

    # open the signlist engine for write and read
    $sl_pid = open2(\*SL_OUT, \*SL_IN, '@@ORACC@@/bin/sl');
    binmode SL_OUT, ':utf8';
    binmode SL_IN, ':utf8';

    $loaded = 1;

#    my $ogsl_db = "$dbdir/ogsl-db";
#    $silent = 1 if defined $_[0];
#    $loaded = 1;
#    binmode STDERR, ':utf8';
#    die "can't read $ogsl_db\n" unless (-r $ogsl_db || -r "$ogsl_db.db");
#    tie (%db, 'NDBM_File', "$ogsl_db", O_RDONLY, 0640) or die;

}

sub
term {
    return unless $loaded;
    print SL_IN "\cD\n";
    $loaded = 0;
#    untie %db;
}

sub
xid {
#    my $x = shift;
#    Encode::_utf8_off($x);
#    $db{$x};
    slse($_[0]) || slse("$_[0];form");
}

# N.B.: this must be passed an xid, not a sign name or value
sub
xid_form {
#    my $x = shift;
#    Encode::_utf8_off($x);
#    $db{$x,'form'};
    slse($_[0].';forms');
}

sub
is_form {
#    my $x = shift;
#    Encode::_utf8_off($x);
#    $db{$x};
    slse("$_[0];form");
}

sub
is_sign {
#    my $x = shift;
#    Encode::_utf8_off($x);
#    $db{$x};
    slse($_[0]);
}

sub
is_value {
#    my $x = shift;
#    Encode::_utf8_off($x);
#    defined $db{$x};
    slse($_[0]);
}

sub
sign_of {
#    my $x = shift;
#    Encode::_utf8_off($x);
#    my $n = $db{$x,'name'};
#    Encode::_utf8_on($n);
    #    $n;
    my $tmp = $_[0];
    my $xid = '';
    unless ($tmp =~ /^o/) {
	unless (($xid = xid($tmp))) {
	    $tmp = "\L$tmp";
	    $xid = xid($tmp);
	}
#	warn "sign_of: $_[0] => $tmp => $xid\n";
    } else {
	$xid = $tmp;
    }
#    my $xid = ($_[0] =~ /^x/ ? $_[0] : xid("\L$_[0]"));
    slse($xid.';name');
}

sub
xatf_of {
    my $x = shift;
    Encode::_utf8_off($x);
    $db{$x,'atf'}
}

# this requires a sign value and does only dumb aliasing
sub
alias {
    my $x = shift;
#    Encode::_utf8_off($x);
#    my $a = $db{$x,'aka'} || $x;
#    Encode::_utf8_on($a);
#    $a;
    my $a = slse($x.';aka');
    $a || $x
}

sub
alias_words {
    return '' unless defined($_[0]) && length($_[0]);
    my $x = shift;
    my @tmp = split(/([-{}\s])/, $x);
    @tmp = map({ alias($_) } @tmp);
    my $res = join('', @tmp);
    $res;
}

sub
xcinit {
    my $id = sign_of(xid($_[0]));
    my $forms = $db{$id,'cinit'};
    Encode::_utf8_on($forms);
    ($forms);
}

sub
xcontains {
    my $id = sign_of(xid($_[0]));
    my $forms = $db{$id,'contains'};
    Encode::_utf8_on($forms);
    ($forms);
}

sub
xcontained {
    my $id = sign_of(xid($_[0]));
    my $forms = $db{$id,'contained'};
    Encode::_utf8_on($forms);
    ($forms);
}

sub
xforms {
    my $id = xid($_[0]);
    my $forms = $db{$id,'forms'};
    Encode::_utf8_on($forms);
    my @f = ();
    my $sign = $db{$id,'name'};
    Encode::_utf8_on($sign);
    push @f, "var\t$sign";
    foreach my $form (split(/\s+/,$forms)) {
	my($f,$v) = ($form =~ m#(.*?)/(.*)$#);
	my $n = $db{$f,'name'};
	Encode::_utf8_on($n);
	push @f, "$v\t$n";
    }
    @f;
}

#sub
#xhomophones {
#    my ($hlist,$alias) = ($db{$_[0],'h'}, $_[1]);
#    my @h = ();
#    if ($hlist) {
#	foreach my $h (split(/\s+/, $hlist)) {
#	    $h =~ s,^(.*)/,,;
#	    my $hid = $1;
#	    my $htmp = $_[0];
#	    Encode::_utf8_on($htmp);
#	    if ($h ne '0') {
#		$h = 'x' if $h == 1000;
#		$h =~ tr/x0-9/ₓ₀-₉/;
#		$htmp .= $h;
#	    }
#	    push @h, $htmp.'('.sign_of($hid).')';
#	}
#    } else {
#	push @h, $_[0];
#    }
#
#    if ($alias) {
#	map { alias($_) } @h;
#    } else {
#	@h;
#    }
#}

sub
xlist {
    my ($hlist,$alias) = ($db{$_[0],'l'}, $_[1]);
    my @h = ();
    if ($hlist) {
	foreach my $h (split(/\s+/, $hlist)) {
	    $h =~ s,^(.*)/,,;
	    my $hid = $1;
	    my $htmp = $_[0];
	    Encode::_utf8_on($htmp);
	    if ($h ne '0') {
		$htmp .= $h;
	    }
	    push @h, $htmp.'('.sign_of($hid).')';
	}
    } else {
	push @h, $_[0];
    }

    if ($alias) {
	map { alias($_) } @h;
    } else {
	@h;
    }
}

sub
protect1 {
    my $tmp = shift;
    $tmp =~ tr/./\003/;
    "×(".$tmp.")";
}

sub
protect2 {
    my $tmp = shift;
    $tmp =~ tr/()+/\000\001\002/;
    $tmp;
}

sub deepsplit {
    my $deep = shift;
    #    $deep =~ tr/{}/  /;
    $deep =~ s/-\{/ :\{/g;
    $deep =~ s/(?<!:)\{/ \{/g;
    $deep =~ s/\}-/\}: /g;
    $deep =~ s/\}(?!\:)/} /g;
    $deep =~ s/\s+/ /g; $deep =~ s/^\s+//; $deep =~ s/\s+$//;
#    warn "deep = $deep\n";
    my @d = split(/[-\s]+/,$deep);
#    warn "at-d = @d\n";
    my @n = ();
    @ORACC::SL::BaseC::last_tlit = ();
    foreach my $d (@d) {
	my $orig_d = $d;
	$d =~ s/:\{//g;
	$d =~ s/\}://g;
	$d =~ tr/{}//;
	# remove value being qualified if any
	if ($d =~ /\)$/) {
	    if ($pedantic) {
		my $q = $d;
		$q =~ tr/\000\001\002\003/()+./;
#		warn "BaseC q-checking $q\n";
		qualcheck($q);
	    }
	    $d =~ s/^[a-zšŋṣṭḫ].*?\((.*?)\)$/$1/;
	}
	my $sn = ($d =~ /^\|/ ? $d : sign_of("\L$d"));
	if ($exceptions{$sn}) {
	    push @ORACC::SL::BaseC::last_tlit, $orig_d;
	    push @n, $exceptions{$sn};
	} else {
	    if ($sn && $sn =~ /^\|/) {
		$sn =~ tr/|//d;
		# split on inner periods--safe as long as caller has
		# hidden inner periods as tlitsplit does
		my @b = split(/\./,$sn);
		my $s = _signature('',@b);
		if ($s && $s !~ /q/) {
		    my $nbits = ($s =~ tr/././) + 1;
		    push @ORACC::SL::BaseC::last_tlit, "^$nbits^$orig_d";
		    push @n, join('-',@b);
		} else {
		    push @ORACC::SL::BaseC::last_tlit, $orig_d;
		    @messages = ();
		    push @n, $d;
		}
	    } else {
		push @ORACC::SL::BaseC::last_tlit, $orig_d;
		push @n, $sn || $d;
	    }
	}
    }
    join('-',@n);
}

sub
tlitsplit {
    my ($tlit,$csplit) = @_;
    return '' unless $tlit;

    $tlit = remove_lang($tlit);
    
    my $orig = $tlit;

#    my $c10ed = c10e_tlit($tlit);
#
#    $tlit = $c10ed if $c10ed;
    
    $tlit =~ s/\!\(.*?\)//;
    $tlit =~ tr/?![]#*<>//d;

    # protect periods in parenthesized signs in compounds
    1 while $tlit =~ s/×\(([^)]+\..*?)\)/protect1($1)/eg;
    
    # protect parens and + in compounds
    $tlit =~ s/(\|[^\|]+\|)/protect2($1)/eg;

    # remove vertical bars if we are splitting compounds
    if ($csplit) {
	my $d = deepsplit($tlit);
	$tlit = $d if $d;
    }

    # now do a basic split into graphemes
    $tlit =~ tr/-{}:+/     /;

    # if we did a deep split we've already segmented compounds if they
    # can be successfully segmented
    $tlit =~ tr/./ / unless $csplit;

    # remove base-morphology characters
    $tlit =~ tr/·°//d;

    # protect numbers like 3(geszu)
    $tlit =~ s/(^|[- \.])([\d\/]+)\((.*?)\)/$1$2\000$3\001/g;

    $tlit =~ s/\S+\((.*?)\)/$1/g;
    $tlit =~ tr/()//d;

    # undo protection measures only after we will make no further changes
    $tlit =~ tr/\000\001\002\003/()+./;

#    warn "presplit = $tlit\n";
    
    my @bits = grep(defined && length, split(/\s+/,$tlit));

#    warn("$orig => ", join(' :: ', @bits), "\n");

    @bits;
}

sub
tlit2ids {
    my ($context,@g) = @_;
    _signature($context, @g);
}

sub
tlit2uni {
    _tlit2uni(UCODE,@_);
}
sub
tlit2cunei {
    _tlit2uni(UCHAR,@_);
}

sub msg {
    my($ctxt,$m) = @_;
    $m =~ tr/\cA/./;
    if ($ctxt) {
	print STDERR "${ctxt}(BaseC): $m\n";
    } else {
#	warn "adding $m to sl messages\n";
	push @messages, $m;
    }
}

sub
_tlit2uni {
    my ($mode,$context,@g) = @_;
    my $uni = '';
    foreach my $g (@g) {
	my $sig = _signature($context, $g);
#	warn "_signature: $g => $sig\n";
	foreach my $s (split(/\./,$sig)) {
	    my $u = ($mode==UCODE ? ucode($s) : uchar($s));
	    if ($u) {
		$uni .= ($mode==UCODE ? "\&#$u;" : $u);
	    } else {
		my $sn = sign_of($s);
		if ($sn) {
		    if ($sn =~ tr/\|//d) {
			$sn =~ s/\(([^\)]+)\)/&hidedots($1)/eg;
			foreach my $s2 (split(/\./,$sn)) {
			    my $s2sig = _signature($context,$s2);
			    my $u = $mode==UCODE ? ucode($s2sig) : uchar($s2sig);
			    if ($u) {
				$uni .= $mode==UCODE? "\&#$u;" : $u;
			    } else {
				$uni .= 'X';
				$s2 =~ tr/\cA/./;
				warn("$context: $s2: no ucode for compound element\n");
			    }
			}
		    } else {
			$uni .= 'X';
			warn("$context: $sn: no ucode (id=$s)\n");
		    }
		} else {
		    warn("$context: $s: undefined return from sign_of\n");
		}
	    }
	}
#	$uni .= ' ';
    }
    $uni =~ s/\s*$//;
    $uni;
}

sub c10e_tlit {
    my $tlit = shift;
    my @t = split(/-/,$tlit);
    my @n = ();
    foreach my $t (@t) {
	if ($t =~ /^\|/) {
	    push @n, c10e_compound($t);
	} else {
	    my $sn = sign_of($t);
	    if ($sn) {
		push @n, $sn;
	    } else {
		push @n, $t;
	    }
	}
    }
    join('-',@n);
}

sub c10e_compound {
    return $_[0] unless $_[0] =~ /^\|(.*?)\|$/;
    my $c = $1;
    my $save_pedantic = $pedantic;
    $pedantic = 0;
    my $c_sig = _signature(undef, tlitsplit($c));
#    warn "c10e: sig of $c => $c_sig\n";
    my @c = ();
    foreach my $s (split(/\./,$c_sig)) {
	my $sn = sign_of($s);
	if ($sn) {
	    push @c, $sn;
	} else {
	    return $c; # abort c10e if a compound element doesn't exist as a separate sign
	}
    }
    my $c2 = join('.',@c);
    #    warn "c10e: $c_sig => $c2\n";
    $c2 =~ s/^\|(.*?)\|$/$1/;
    if ($c2 && $c ne $c2) {
	warn "c10e: $c is $c2\n";
	msg(undef, "c10e: $c should be $c2") if $save_pedantic;
	return '|'.$c2.'|';
    }
    $pedantic = $save_pedantic;
    return $c;
}

sub
_signature {
    my ($context,@g) = @_;
#    @ORACC::SL::fixes_needed = ();
    my @sig = ();
    my $ctxt = $context ? "$context: " : "";
    my @newg = ();
    foreach my $g (@g) {
	$g =~ s/^\$//;
	$g = remove_lang($g); warn "$0: leftover lang in $g\n" if $g =~ /\%.*?:/;
	$g =~ tr/·°//d;
	if ($g =~ m#^[/0-9]+(?:\@v)?$#) {
	    push @newg, ORACC::Legacy::Sexify::sexify($g,0,1,0);
	} else {
	    push @newg, $g;
	}
    }
    foreach my $g (@newg) {
	next if !$g;
	my $xid = is_value($g);
	if ($xid) {
	    if ($pedantic && $g =~ /[A-ZŠŊṢṬ]/) {
		my $sn_nm = sign_of($xid);
		my $sn_g = $g; $sn_g =~ tr/|//d;
		$sn_nm =~ tr/|//d;
		# warn "sn $g has id $xid and sign_of $sn_nm\n";
		if ($sn_nm ne $sn_g) {
		    msg($ctxt, "name of pseudo-sign $sn_g should be $sn_nm");
		}
	    }
#	    warn "g=$g=$xid\n";
	    push @sig, $xid;
	} elsif ($g =~ /[A-ZŠŊṢṬ]/) {
	    my $sn;
	    if ($g =~ /^([^(0-9\/]+?)\((.*?)\)$/) {
		$g = $1;
		$sn = $2;
		$sn =~ tr/|//d unless $sn =~ /^\d\(\|/;
	    } else {
		$sn = $g;
	    }
	    $sn =~ tr/\cA/./;
	    my $sn_id = is_form($sn) || is_sign($sn);
	    if ($sn_id) {
		my $sn_nm = sign_of($sn_id);
		# warn "sn $sn has id $sn_id and sign_of $sn_nm\n";
		if ($sn_nm ne $sn) {
		    msg($ctxt, "name of pseudo-sign $sn should be $sn_nm");
		}
	    }
#	    warn "g=$g; sn=$sn; sn_id=$sn_id\n";
#	    if ($sn =~ /ŠE.A.AN/) {
#		warn "$sn => $sn_id\n";
#	    }
	    unless ($sn_id) {
		if ($sn !~ /[\|.]/) {
		    my $tmp = lc($sn);
		    if (($sn_id = is_value($tmp))) {
			my $nsn = sign_of($sn_id);
			#			push @ORACC::SL::fixes_needed, [ $sn, $nsn ];
			if ($pedantic && (!$reported{$g}++ || $ORACC::SL::report_all)) {
			    # warn join(':',caller()), "\n";
			    msg($ctxt, "sign name $sn should be $nsn");
			}
			
			$sn = $nsn;
			$sn =~ tr/|//d;
		    # } else {
		    # 	my $n = $g;
		    # 	$n =~ tr/|//d;
		    # 	my @n = tlitsplit($n);
		    # 	my $nsig = _signature('',@n);
		    # 	if ($nsig) {
		    # 	    msg($ctxt,"bad sign name $sn");
		    # 	    $sn_id = $nsig;
		    # 	}
		    }
		}
	    }
	    if ($sn_id) {
		push @sig, $sn_id;
	    } else {
		# warn "BaseC checking sign $sn\n";
		my $snn = $sn;
		my $nsn = '';
		if ($snn =~ tr/|//d == 1) {
		    $nsn = is_sign($snn);
		} else {
		    $nsn = is_sign($sn);
		}
		if ($nsn) {
		    push @sig, $sn_id;
		} elsif ($g eq '...') {
		    push @sig, 'q99';
		} else {
		    msg($ctxt,"sign name $sn not in sign list")
			unless $silent || $reported{$g}++;
		    push @sig, 'q00';
		}
	    }
	} elsif ($g eq '...') {
	    push @sig, 'q99';
	} else {
	    msg($ctxt, "grapheme $g not in sign list")
		unless $silent || $reported{$g}++;
	    push @sig, 'q01';
	}
    }
    my @nsig = ();
    foreach my $s (@sig) {
#	warn "sign_of $s ...\n";
	if ($s =~ /^q[0-9][0-9]/) {
	    push @nsig, $s;
	} else {
	    my $sn = sign_of($s);
	    if ($sn =~ tr/\|//d) {
		my @c = ();
		$sn =~ s/\(([^\)]+)\)/&hidedots($1)/eg;
		my $ok = 1;
		foreach my $c (split(/[\.\+]/, $sn)) {
		    my $cs = is_sign(sign_of($c));
		    if ($cs) {
			push @c, $cs;
		    } else {
			$ok = 0;
			last;
		    }
		}
		if ($ok) {
		    #		warn "pushing ".join('.',@c)." onto nsig\n";
		    push @nsig, @c;
		} else {
		    push @nsig, $s;
		}
	    } else {
		push @nsig, $s;
	    }
	}
    }
    join('.',@nsig);
}

sub hidedots {
    my $d = shift;
    $d =~ tr/./\cA/;
    "($d)";
}

sub
ucode {
    slse("$_[0];ucode");
}

sub
uchar {
#    my $n = $db{$_[0],'uchar'};
    my $n = slse("$_[0];uchar");
    Encode::_utf8_on($n);
    $n;
}

sub
values {
    my $id = xid($_[0]);
    my $v = $db{$id,'values'};
    Encode::_utf8_on($v);
    $v;
}

sub slse { slseA(@_) };

sub
slseA {
    print SL_IN "$_[0]\n";
    my $res = <SL_OUT>;
    chomp($res);
#    warn "slseA -k $_[0] => $res\n";
    $res;
}

sub slseB {
    my $res = `@@ORACC@@/bin/sl -k '$_[0]'`;
    chomp($res);
#    warn "slseB -k $_[0] => $res\n";
    $res;
}

### Qualifier checking routines

sub qmsg { msg '', @_ };

sub qualcheck {
    my $qn = shift;
#    1 while $qn =~ s/\{[^\}]+\}//;
#    $qn =~ s/(\|[^\|]+\|)/protect2($1)/eg;
#    $qn =~ s/^.*?(\S+?\(.*?\)).*$/$1/;
#    $qn =~ tr/\000\001\002/()+/;
#    $qn =~ s/^.*?_//;
#    $qn =~ s/^.*?-//;
#    $qn =~ s/\s*$//;
    # is this qualified sign known?
    my $q = ORACC::SL::BaseC::is_value($qn);
    if ($q) {
	# is it type 'may'--those must be unqualified in @bases
	if ($q eq 'may') {
	    my $qv = $qn; $qv =~ s/\(.*$//;
	    qmsg("[Q1a] vq=$qn: redundant qualifier in $qn--use plain $qv");
	} elsif ($q eq 'map') {
	    # or is it 'map', in which case we can look up what it should be
	    my $qm = ORACC::SL::BaseC::is_value("$qn;map");
	    qmsg("[Q1c] vq=$qn: non-canonical qualifier--use $qm");
	}
    } else {
	# are then any known qns for this value?
	my ($qv,$qq) = ($qn =~ /^(.*?)\((.*)\)$/);
	unless ($qv && $qq) {
	    qmsg("vq $qn doesn't parse into v and q");
	    return;
	}
	my $void = ORACC::SL::BaseC::is_value($qv);
	my $qoid = ORACC::SL::BaseC::is_value($qq);
	if (!$qoid) {
	    # perform sametlit check, warn && return if it should be something else
	    my $should = tlit_sig($qq);
	    if ($should) {
		qmsg("[Q5] vq=$qn: qualifier $qq should be $should");
		qualcheck("$qv($should)");
	    } else {
		qmsg("[Q7] vq=$qn: unknown qualifier $qq");
	    }
	} else {
	    if ($void eq $qoid) {
		qmsg("[Q1b] vq=$qn: redundant qualifier in $qn--use plain $qv") unless $qv =~ /ₓ/;
	    } else {
		my $qqv = ORACC::SL::BaseC::is_value("$qoid;values") || '';
		my $supp = " known values are: $qqv";
		my $found_base = 0;
		my $qb = $qv; $qb =~ tr/₀-₉ₓ⁻⁺//d;
		if ($qqv) {
		    # Try to find a base that matches this q, like meₓ(AK) for me₆(AK)
		    #		warn("looking[1] for base $qb in $qqv\n");
		    foreach my $vb (split(/\s+/,$qqv)) {
			if ($vb =~ /^${qb}[₀-₉ₓ⁻⁺]*$/) {
			    my $qqname = sign_of($qq);
			    if ($vb =~ /ₓ$/) {
				$supp = " suggest $vb($qqname)";
			    } elsif (is_value("$vb($qqname)")) {
				my $t = is_value("$vb($qqname)");
				if ($t eq 'map') {
				    my $m = ORACC::SL::BaseC::is_value("$vb($qqname);map");
				    $supp = " suggest $m";
				} elsif ($t eq 'may') {
				    if ($vb =~ tr/⁻⁺//d) {
					$supp = " suggest $vb($qqname)";
				    } else {
					$supp = " suggest $vb";
				    }
				} else {
				    $supp = " suggest $vb($qqname)";
				}
			    } else {
				# $supp = " suggest $vb";
			    }
			    ++$found_base;
			    last;
			}
		    }
		}
		
		# Also, if the qq is a FORM, get the SIGN and see if this shares a base with that
		# like adda(|LU₂×BAD|) for addaₓ(|LU₂×BAD|)
		if (!$found_base) {
		    my $signs = is_value("$qoid;signs");
		    if ($signs) {
			foreach my $s (split(/\s+/,$signs)) {
			    my $svv = is_value("$s;values");
			    foreach my $vb (split(/\s+/,$svv)) {
				if ($vb =~ /^${qb}[₀-₉ₓ⁻⁺]*$/) {
				    my $qqname = sign_of($qq);
				    if (is_value("$vb($qqname)") || $vb =~ /ₓ$/) {
					$supp = " suggest $vb($qqname)";
				    } elsif (is_value("$vb($qqname)")) {
					my $t = is_value("$vb($qqname)");
					if ($t eq 'map') {
					    my $m = ORACC::SL::BaseC::is_value("$vb($qqname);map");
					    $supp = " suggest $m";
					} elsif ($t eq 'may') {
					    if ($vb =~ tr/⁻⁺//d) {
						$supp = " suggest $vb($qqname)";
					    } else {
						$supp = " suggest $vb";
					    }
					} else {
					    $supp = " suggest $vb($qqname)";
					}
				    } else {
					# $supp = " suggest $vb";
				    }
				    ++$found_base;
				    last;
				}
			    }
			}
		    }
		}
		
		if ($found_base) {
		    qmsg("[Q4] vq=$qn: $qv unknown for $qq:$supp");
		} else {
		    my $vsn = ORACC::SL::BaseC::sign_of($void);
		    my $qvv = ORACC::SL::BaseC::is_value("$qoid;values") || '';
		    my $supp = '';
		    if ($qqv) {
			$supp = "; $qq can qualify $qqv";
		    }
		    if ($vsn) {
			qmsg("[Q6] vq=$qn: value $qv belongs to $vsn$supp");
		    } else {
			qmsg("[Q8] vq=$qn: sign $qq needs value $qv");
		    }
		}
	    }
	    #	} else {
#	    my $qqq = ORACC::SL::BaseC::is_value("$qv;qual");
#	    if ($qqq) {
#		qmsg("[Q2] vq=$qn: unknown: known for $qv = $qqq");
#	    } else {
#		my $qc = qualcorr($qn);
#		if ($qc) {
#		    qmsg("[Q3] vq=$qn: unknown value-qualifier; did you mean $qc?");
#		} else {		
#		}
#	    }
	}
    }
}

sub qualcorr {
    my $qn = shift;
    my ($qv,$qq) = ($qn =~ /^(.*?)\((.*?)\)$/);
    if ($qv) {
	my $qb = $qv; $qb =~ tr/₀-₉ₓ⁻⁺//d;
	my $q0 = ORACC::SL::BaseC::is_value("$qb₀;qual");
	if ($q0) {
	    foreach my $qc (split(/\s+/,$q0)) {
		my $qcq = $qc; $qcq =~ s/^.*?\((.*?)\)$/$1/;
		if ($qcq eq $qq) {
		    return $qc;
		}
	    }
	}
    }
    undef;
}

sub remove_lang {
    my $t = shift;
    1 while $t =~ s/\%[-a-z]?://;
    $t;
}

1;
