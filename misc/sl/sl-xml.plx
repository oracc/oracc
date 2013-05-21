#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
binmode STDIN, ':utf8'; binmode STDERR, ':utf8';  binmode STDOUT, ':utf8';

my $asl = shift @ARGV;
if ($asl) {
    unless (-e $asl) {
	$asl = "00lib/$asl.asl";
    }
} else {
    $asl = "00lib/sl.asl" unless $asl;
}
open(SL,$asl) || die "sl-xml.plx: can't read signlist `$asl'\n";
my $xl = $asl;
$xl =~ s#00lib#02xml#;
$xl =~ s/\.a?sl$/-sl.xml/;
open(XL,">$xl") || die "sl-xml.plx: can't write to $xl";
select XL;

my %v_closers = ();
my @v_closers = qw/v form end/;
@v_closers{@v_closers} = ();

my %form_closers = ();
my @form_closers = qw/form end/;
@form_closers{@form_closers} = ();

my $in_sign = 0;
my $in_form = 0;
my $in_value = 0;
my $utf8 = undef;
my $xid = 'x0000';

print '<?xml version="1.0" encoding="utf-8"?>',"\n";
print "<signlist xmlns:sl=\"http://oracc.org/ns/sl/1.0\" xmlns=\"http://oracc.org/ns/sl/1.0\" xmlns:g=\"http://oracc.org/ns/gdl/1.0\"\n>";
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
	print "<sign$deprecated n=\"$n\" xml:id=\"$xid\"><name g:me=\"1\">$n</name>";
	++$xid;
    } else {
	unless ($in_sign) {
	    warn "$asl:$.: missing \@sign\n";
	}
	
	my $curr_field = '';
	/^\@([a-z]+)/;
	$curr_field = $1;
	if (exists $v_closers{$curr_field} && $in_value) {
	    print "</v>";
	    $in_value = 0;
	}
	if (exists $form_closers{$curr_field} && $in_form) {
	    if ($curr_field ne 'end' || /sign/) {
		print "</form>";
		$in_form = 0;
	    }
	}

	if (/^\@list\s+(\S+)(?:\s+(\S+)?)?\s*$/) {
	    my ($n,$name) = ($1,$2);
	    $n = xmlify($n);
	    if ($name) {
		$name = xmlify($name);
		$name = " name=\"$name\"";
	    } else {
		$name = '';
	    }	    
	    print "<list n=\"$n\"$name/>";
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
		print "<form n=\"$n\" var=\"$v\" xml:id=\"$xid\"$uattr><name g:me=\"1\">$n</name>";
		++$xid;
		$in_form = 1;
	    }
	} elsif (s/^\@v(\??)(-?)[\t\s]+//) {
	    my $query = $1;
	    my $dropped = $2;
	    my $proof = '';
	    print "<v\n";
	    print " uncertain=\"yes\"" if $query;
	    print " deprecated=\"yes\"" if $dropped;

# what is this for?
	    if (s/^\#(\S+)\s+//) {
		print " comment=\"$1\"";
	    }

	    if (s/^\%(\S+)\s+//) {
		print " xml:lang=\"$1\"";
	    }
	    if (s/\s+\[(.*?)\]\s*$//) {
		$proof = xmlify($1);
	    }
	    if (/^(\S+)$/) {
		my $n = $1;
		$n =~ tr/_/ /;
		print " n=\"$n\">";
		print "<proof>$proof</proof>" if $proof;
		$in_value = 1;
	    } else {
		warn "$asl:$.: syntax error in \@v\n";
	    }
	} elsif (/^\@ucode/) {
	    if (/^\S+\s+(\S+)\s+(\S+)/) {
		print "<utf8 hex=\"$1\">$2</utf8>";
	    }
	} elsif (/^\@(?:note|inote|pname|uname|unote|uphase|lit)/) {
	    $curr_field = 'note' if $curr_field eq 'lit';
	    s/^\S+\s+//;
	    chomp;
	    print "<$curr_field>";
	    print xmlify($_);
	    print "</$curr_field>";
	} elsif (/^\@end\s+form\s*$/) {
	    if ($in_form) {
		$in_form = 0;
		print "</form\n>";
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
		print "</sign\n>";
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
print '</signlist>';

close(SL);

1;
