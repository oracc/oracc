package ORACC::SL::Base;
use warnings; use strict; use utf8; use open 'utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::Legacy::Sexify;

binmode STDERR, ':utf8';
use constant UCODE => 1;
use constant UCHAR => 0;

use Encode;
use Fcntl;
use NDBM_File;

my $base = "$ENV{'ORACC'}/pub/ogsl";
my $dbdir = "$base";
my %db = ();
my $loaded = 0;
my $pedantic = 0;
my %reported = ();
my $silent = 0;

sub
check {
    my($context,$test) = @_;
    _signature($context,tlitsplit($test));
}

sub
same_tlit {
    my($context,$test,@against) = @_;
    my $test_sig = _signature($context,tlitsplit($test));
    foreach my $a (@against) {
	my $a_sig = _signature($context,tlitsplit($a));
	return $a if $test_sig eq $a_sig;
    }
    return undef;
}

sub
init {
    return if $loaded;
    my $ogsl_db = "$dbdir/ogsl-db";
    $silent = 1 if defined $_[0];
    $loaded = 1;
    binmode STDERR, ':utf8';
    die "can't read $ogsl_db\n" unless (-r $ogsl_db || -r "$ogsl_db.db");
    tie (%db, 'NDBM_File', "$ogsl_db", O_RDONLY, 0640) or die;
}
sub
term {
    return unless $loaded;
    $loaded = 0;
    untie %db;
}
sub
xid {
    my $x = shift;
    Encode::_utf8_off($x);
    $db{$x};
}
sub
is_sign {
    my $x = shift;
    Encode::_utf8_off($x);
    $db{$x};
}
sub
is_value {
    my $x = shift;
    Encode::_utf8_off($x);
    defined $db{$x};
}
sub
sign_of {
    my $x = shift;
    Encode::_utf8_off($x);
    my $n = $db{$x,'name'};
    Encode::_utf8_on($n);
    $n;
}
sub
atf_of {
    my $x = shift;
    Encode::_utf8_off($x);
    $db{$x,'atf'}
}
sub
alias {
    my $x = shift;
    Encode::_utf8_off($x);
    my $a = $db{$x,'aka'} || $x;
    Encode::_utf8_on($a);
    $a;
}
sub
alias_words {
    return '' unless defined($_[0]) && length($_[0]);
    my $x = shift;
    Encode::_utf8_off($x);
    if (defined $db{$x,'aka'}) {
	alias($x);
    } else {
	my @tmp = split(/([-{}\s])/, $x);
	@tmp = map({ alias($_) } @tmp);
	my $res = join('', @tmp);
	$res;
    }
}
sub
homophones {
    my ($hlist,$alias) = ($db{$_[0],'h'}, $_[1]);
    my @h = ();
    if ($hlist) {
	foreach my $h (split(/\s+/, $hlist)) {
	    $h =~ s,^(.*)/,,;
	    my $hid = $1;
	    my $htmp = $_[0];
	    Encode::_utf8_on($htmp);
	    if ($h ne '0') {
		$h = 'x' if $h == 1000;
		$h =~ tr/x0-9/ₓ₀-₉/;
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
list {
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

    if ($csplit) {
	# remove parens and contained periods in compounds
	# this gets done naturally further down
    } else {
	# protect parens and contained periods in compounds
	$tlit =~ s/(\|[^\|]+\|)/protect($1)/eg;
    }

    $tlit =~ tr/-.{}:+·°/     /;

    # protect numbers like 3(geszu)
    $tlit =~ s/(^|[- \.])([\d\/]+)\((.*?)\)/$1$2\000$3\001/g;

    $tlit =~ tr/|//d;

    $tlit =~ s/\S+\((.*?)\)/$1/g;
    $tlit =~ tr/()//d;

    # undo protection measures only after we will make no further changes
    $tlit =~ tr/\000\001\002\003/().+/;

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

sub
_signature {
    my ($context,@g) = @_;
    my @sig = ();
    my $ctxt = $context ? "$context: " : "";
    my @newg = ();
    foreach my $g (@g) {
	if ($g =~ m#^[/0-9]+(?:\@v)?$#) {
	    push @newg, ORACC::Legacy::Sexify::sexify($g,0,1,0);
	} else {
	    push @newg, $g;
	}
    }
    foreach my $g (@newg) {
	next if !$g;
	if (is_value($g)) {
	    my $xid = xid($g);
	    if ($xid) {
		push @sig, $xid;
	    } else {
		print STDERR "$ctxt$g => $xid => no sign!\n"
		    unless $silent || $reported{$g}++;
	    }
	} elsif ($g =~ /[A-ZŠŊṢṬ]/) {
	    my $sn;
#	    print STDERR "$g ... " if $g =~ /iku/i;
	    if ($g =~ /^([^(0-9\/]+?)\((.*?)\)$/) {
#		print STDERR "got split\n" if $g =~ /iku/i;
		$g = $1;
		$sn = $2;
		$sn =~ tr/|//d;
	    } else {
#		print STDERR "left whole\n"  if $g =~ /iku/i;
		$sn = $g;
	    }
	    unless (is_sign($sn)) {
		if ($sn !~ /[\|.]/) {
		    my $tmp = lc($sn);
		    if (is_value($tmp)) {
			my $xid = xid($tmp);
			my $nsn = sign_of($xid);
			print STDERR "$ctxt$sn should be $nsn\n"
			    if $pedantic && !$reported{$g}++;
			$sn = $nsn;
			$sn =~ tr/|//d;
		    }
		}
	    }
	    if (is_sign($sn)) {
		push @sig, xid($sn);
	    } else {
		print STDERR "${ctxt}sign name '$sn' not in sign list\n"
		    unless $silent || $reported{$g}++;
		push @sig, 'q00';
	    }
	} else {
	    print STDERR "${ctxt}grapheme '$g' not in sign list\n"
		unless $silent || $reported{$g}++;
	    push @sig, 'q01';
	}
    }
    join('.',@sig);
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
    ($db{$id,'values'});
}

1;
