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

$ORACC::SL::report_all = 0;
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
    my($context,$test,$deep) = @_;
#    warn join(':',caller()), "\n";
    my $sig = _signature($context,tlitsplit($test,$deep));
#    warn "$test=>$sig\n";
    $sig;
}

sub messages {
    my @tmp = @messages;
    @messages = ();
    return @tmp;
}

sub pedantic {
    if (defined $_[0]) {
	$pedantic = $_[0];
    } else {
	$pedantic = 1;
    }
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
    my($context,$test,@against) = @_;
    _signature($context,tlitsplit($test,1));
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
	$tmp = "\L$tmp";
	$xid = xid($tmp);
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
protect {
    my $tmp = shift;
    $tmp =~ tr/().+/\000\001\002\003/;
    $tmp;
}

sub
tlitsplit {
    my ($tlit,$csplit) = @_;
    return '' unless $tlit;
    my $orig = $tlit;
    $tlit =~ s/\!\(.*?\)//;
    $tlit =~ tr/?[]#*<>//d;

#    if ($csplit) {
#	$tlit =~ tr/|+/  /;
#    } else {
	# protect parens and contained periods in compounds
    $tlit =~ s/(\|[^\|]+\|)/protect($1)/eg;
#    warn "protected = $tlit\n";
#    }

    $tlit =~ s/\%sux://g;
    $tlit =~ tr/-.{}:+/     /;
    $tlit =~ tr/·°//d;

    # protect numbers like 3(geszu)
    $tlit =~ s/(^|[- \.])([\d\/]+)\((.*?)\)/$1$2\000$3\001/g;

#    $tlit =~ tr/|//d;

    $tlit =~ s/\S+\((.*?)\)/$1/g;
    $tlit =~ tr/()//d;

    # undo protection measures only after we will make no further changes
    $tlit =~ tr/\000\001\002\003/().+/;

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
	foreach my $s (split(/\./,$sig)) {
	    my $u = ($mode==UCODE ? ucode($s) : uchar($s));
	    if ($u) {
		$uni .= ($mode==UCODE ? "\&#$u;" : $u);
	    } else {
		my $sn = sign_of($s);
		if ($sn) {
		    if ($sn =~ tr/\|//d) {
			foreach my $s2 (split(/\./,$sn)) {
			    my $s2sig = _signature($context,$s2);
			    my $u = $mode==UCODE ? ucode($s2sig) : uchar($s2sig);
			    if ($u) {
				$uni .= $mode==UCODE? "\&#$u;" : $u;
			    } else {
				$uni .= 'X';
				warn("$context: $s2: no ucode for compound element\n");
			    }
			}
		    } else {
			$uni .= 'X';
			warn("$context: $sn: no ucode (id=$s)\n");
		    }
		}
	    }
	}
#	$uni .= ' ';
    }
    $uni =~ s/\s*$//;
    $uni;
}

sub c10e_compound {
    return unless $_[0] =~ /^\|(.*?)\|$/;
    my $c = $1;
    my $save_pedantic = $pedantic;
    $pedantic = 0;
    my $c_sig = _signature(undef, tlitsplit($c));
#    warn "c10e: sig of $c => $c_sig\n";
    my @c = ();
    foreach my $s (split(/\./,$c_sig)) {	
	push @c, sign_of($s);
    }
    my $c2 = join('.',@c);
    #    warn "c10e: $c_sig => $c2\n";
    $c2 =~ s/^\|(.*?)\|$/$1/;
    if ($c2 && $c ne $c2) {
#	warn "c10e: $c is $c2\n";
	msg(undef, "c10e: $c should be $c2");
	return '|'.$c2.'|';
    }
    $pedantic = $save_pedantic;
    return '';
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
	$g =~ s/\%sux://g;
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
#	    warn "g=$g=$xid\n";
	    push @sig, $xid;
	} elsif ($g =~ /[A-ZŠŊṢṬ]/) {
	    my $sn;
	    if ($g =~ /^([^(0-9\/]+?)\((.*?)\)$/) {
		$g = $1;
		$sn = $2;
		$sn =~ tr/|//d;
	    } else {
		$sn = $g;
	    }
	    my $sn_id = is_form($sn) || is_sign($sn);
	    if ($sn =~ /ŠE.A.AN/) {
		warn "$sn => $sn_id\n";
	    }
#	    warn "g=$g=$sn_id\n";
	    unless ($sn_id) {
		if ($sn !~ /[\|.]/) {
		    my $tmp = lc($sn);
		    if (($sn_id = is_value($tmp))) {
			my $nsn = sign_of($sn_id);
			#			push @ORACC::SL::fixes_needed, [ $sn, $nsn ];
			if ($pedantic && (!$reported{$g}++ || $ORACC::SL::report_all)) {
			    # warn join(':',caller()), "\n";
			    msg($ctxt, "sign name '$sn' should be '$nsn'");
			}
			    
			$sn = $nsn;
			$sn =~ tr/|//d;
		    }
		}
	    }
	    if ($sn_id) {
		push @sig, $sn_id;
	    } else {
		msg($ctxt,"sign name '$sn' not in sign list")
		    unless $silent || $reported{$g}++;
		push @sig, 'q00';
	    }
	} else {
	    msg($ctxt, "grapheme '$g' not in sign list")
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
		my $ok = 1;
		foreach my $c (split(/[\.\+]/, $sn)) {
		    my $cs = is_sign($c);
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

sub
ucode {
    $db{$_[0],'ucode'}
}
sub
uchar {
    my $n = $db{$_[0],'uchar'};
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

1;
