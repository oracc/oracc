#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use Getopt::Long;

binmode STDIN, ':utf8'; binmode STDERR, ':utf8';  binmode STDOUT, ':utf8';

my $check = 0;
my $signlist = 'sl';
my $unicodify = 0;
my $xml = 0;
my $verbose = 0;

GetOptions(
    'check'=>\$check,
    'signlist:s'=>\$signlist,
    'unicode'=>\$unicodify,
    'xml'=>\$xml,
    'verbose'=>\$verbose,
    );

my $asl = $signlist;
if ($asl) {
    unless (-e $asl) {
	$asl = "00lib/$asl.asl";
    }
} else {
    $asl = "00lib/sl.asl" unless $asl;
}
open(SL,$asl) || die "slmanager.plx: can't read signlist `$asl'\n";
my $xl = $asl;

if ($xml) {

    die "slmanager.plx: XML OUTPUT NO LONGER SUPPORTED: use sl-xml.plx instead\n";
    
    $xl =~ s#00lib#02xml#;
    $xl =~ s/\.a?sl$/.xml/;
    open(XL,">$xl") || die "slmanager.plx: can't write to $xl";
    select XL;
}

my %v_closers = ();
my @v_closers = qw/v form end/;
@v_closers{@v_closers} = ();

my %form_closers = ();
my @form_closers = qw/form end/;
@form_closers{@form_closers} = ();

my $in_sign = 0;
my $in_form = 0;
my $in_value = 0;
my $longform = 0;
my $utf8 = undef;
my $xid = 'x0000';

my %ucodes = ();
my $sign = '';
my $ucode = 0;

if ($unicodify) {
    load_ucodes();
    add_ucodes();
    # system 'mv', "01tmp/$$.tmp", $asl;
    exit 0;
}

if ($xml) {
    print '<?xml version="1.0" encoding="utf-8"?>',"\n";
    print "<signlist xmlns:sl=\"http://oracc.org/ns/1.0\" xmlns=\"http://oracc.org/ns/sl/1.0\"\n>";
}

while (<SL>) {
    next if /^\s*$/ || /^\#/;
    if (/^\@sign\s+(\S+)$/ || /^\@nosign\s+(\S+)$/) {
	my $deprecated = '';
	if (/^\@nosign/) {
	    $deprecated = ' deprecated="1"';
	}
	if ($in_sign) {
	    warn "$asl:$.: missing \@end sign\n";
	}
	$in_sign = 1;
	my $n = xmlify($1);
	unless ($deprecated) {
	 #   warn "undefined deprecated\n";
	    $deprecated = '';
	}
	unless ($n) {
	    # warn "undefined n\n";
	    $n = '';
	}
	print "<sign$deprecated n=\"$n\" xml:id=\"$xid\">" if $xml;
	++$xid;
    } else {
	unless ($in_sign) {
	    warn "$asl:$.: missing \@sign\n";
	}
	
	my $curr_field = '';
	/^\@([a-z]+)/;
	$curr_field = $1;
	if (exists $v_closers{$curr_field} && $in_value) {
	    print "</v>" if $xml;
	    $in_value = 0;
	}
	if (exists $form_closers{$curr_field} && $in_form) {
	    if ($longform && /sign/) {
		warn "$asl:$.: expected long version of \@form to end with \@end form\n";
	    } else {
		if ($curr_field ne 'end' || /sign/) {
		    print "</form>" if $xml;
		    $in_form = 0;
		}
	    }
	}

	if (/^\@list\s+(\S+)(?:\s+(\S+)?)?\s*$/) {
	    ++$longform if $in_form;
	    my ($n,$name) = ($1,$2);
	    $n = xmlify($n);
	    if ($name) {
		$name = xmlify($name);
		$name = " name=\"$name\"";
	    } else {
		$name = '';
	    }
	    print "<list n=\"$n\"$name/>" if $xml;
	} elsif (/^\@form\s+(\S+)\s+(\S+)(?:\s+\S+\s+(\S+))?$/) {
	    if ($in_form) {
		warn "$asl:$.: missing `\@end form'\n";
		$in_form = 0;
	    } else {
		my ($n,$v,$u) = ($2,$1,$3);
		$n = xmlify($n);
		$v = xmlify($v);
		my $uattr = "";
		$uattr = " utf8=\"$u\"" if $u;
		print "<form n=\"$n\" var=\"$v\"$uattr>" if $xml;
		$in_form = 1;
	    }
	} elsif (s/^\@v(\??)(-?)[\t\s]+//) {
	    ++$longform if $in_form;
	    my $query = $1;
	    my $dropped = $2;
	    my $proof = '';
	    if ($xml) {
		print "<v\n";
		print " uncertain=\"yes\"" if $query;
		print " deprecated=\"yes\"" if $dropped;
	    }

# what is this for?
	    if (s/^\#(\S+)\s+//) {
		print " comment=\"$1\"" if $xml;
	    }

	    if (s/^\%(\S+)\s+//) {
		print " xml:lang=\"$1\"" if $xml;
	    }
	    if (s/\s+\[(.*?)\]\s*$//) {
		$proof = xmlify($1);
	    }
	    if (/^(\S+)$/) {
		my $n = $1;
		$n =~ tr/_/ /;
		if ($xml) {
		    print " n=\"$n\">";
		    print "<proof>$proof</proof>" if $proof;
		}
		$in_value = 1;
	    } else {
		warn "$asl:$.: syntax error in \@v\n";
	    }
	} elsif (/^\@ucode/) {
	    warn "ucode\n";
	    ++$longform if $in_form;
	} elsif (/^\@(?:note|inote|pname|uname|unote|uphase|lit)/) {
	    ++$longform if $in_form;
	    $curr_field = 'note' if $curr_field eq 'lit';
	    s/^\S+\s+//;
	    chomp;
	    if ($xml) {
		print "<$curr_field>";
		print xmlify($_);
		print "</$curr_field>";
	    }
	} elsif (/^\@end\s+form\s*$/) {
	    if ($in_form) {
		$longform = $in_form = 0;
		print "</form\n>" if $xml;
	    } else {
		warn "$asl:$.: superfluous '\@end form'\n";
	    }
	} elsif (/^\@end\s+sign\s*$/) {
	    if ($in_form) {
		warn "$asl:$.: missing `\@end form'\n";
		$in_form = 0;
	    }
	    if ($in_sign) {
		$in_sign = 0;
		print "</sign\n>" if $xml;
	    } else {
		warn "$asl:$.: superfluous '\@end sign'\n";
	    }
	} elsif (/^\@fake/) {
	    # TODO
	} else {
	    warn "$asl:$.: syntax error\n";
	}
    }
}
print '</signlist>' if $xml;
close(SL);

##################################################################

sub
add_ucodes {
    open(A,"$asl");
    open(N,">01tmp/$$.tmp");
    while (<A>) {
	if (/^\@sign\s+(.*?)\s*$/) {
	    $sign = $1;
	    $ucode = 0;
	} elsif (/^\@ucode\s+(\S+)\s*$/) {
	    my $tmp = $1;
	    if ($tmp =~ m/\./) {
		my $seq = make_sequence_from_ucode($tmp);
		s/$/ $seq/;
	    } else {
		add_utf8($tmp);
	    }
	    $ucode = 1;
	} elsif (/^\@ucode\s+(.*?)\s*$/) {
	    $ucode = 1;
	} elsif (/^\@v/) {
	    if (!$ucode) {
		if ($sign =~ /\|/) {
		    my ($ucode_hex,$ucode_utf8) = make_sequence($sign);
		    print N "\@ucode $ucode_hex $ucode_utf8\n"
			if $ucode_hex;
		}
		$ucode = 1;
	    }
	}
	print N;    
    }
    close(N);
    close(A);
}

sub
add_utf8 {
    my $hex = shift;
    my $utf = chr(hex("0$hex"));
    s/$/ $utf/;
}

sub
load_ucodes {
    open(A,"$asl");
    while (<A>) {
	if (/^\@ucode\s+(x[0-9A-F]+)(\s|$)/) {
	    add_utf8($1);
	}
	if (/^\@sign\s+(.*?)\s*$/) {
	    $sign = $1;
	} elsif (/^\@ucode\s+(.*?)\s+(.*?)\s*$/) {
	    my($hex,$utf8) = ($1,$2);
	    $sign =~ tr/|//d;
	    $ucodes{$sign} = [ $1 , $2 ];
	}
    }
    close(A);
}

sub
make_sequence {
    my $sign = shift;
    my @hex = ();
    my @utf8 = ();
    $sign =~ tr/|//d;
    1 while $sign =~ s/\(([^\)]+?)\./($1\cA/;
    foreach my $s (split(/\./,$sign)) {
	$s =~ tr/\cA/./;
	my $u = $ucodes{$s};
	if ($u) {
	    push @hex, $$u[0];
	    push @utf8, $$u[1];
	} else {
	    warn "no ucode for $s\n";
	    return ();
	}
    }
    (join('.',@hex),join('',@utf8));
}

sub
make_sequence_from_ucode {
    my $hex = shift;
    my @utf8 = ();
    foreach my $s (split(/\./,$hex)) {
	push @utf8, chr(hex("0$s"));
    }
    join('',@utf8);
}

1;
