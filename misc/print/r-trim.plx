#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8'; binmode STDOUT, ':utf8'; binmode STDIN, ':utf8';
use Encode;

# this actually works on the HTML <span class="instances"> ... </span>

binmode STDOUT, 'utf8';

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::XML;

my $f = shift @ARGV;
my $x = load_xml($f);

my @spans = tags($x,'http://www.w3.org/1999/xhtml','span');
foreach my $s (@spans) {
    if ($s->getAttribute('class') eq 'instances') {
	trim_i($s);
    }
}

sub trim_i {
    my $inst = shift;
    my $t = $inst->textContent();
    my $end_punct = '';
    if ($t =~ s/([.;]\s*)$//) {
	$end_punct = $1;
    }
    my @t = split(/;\s*/, $t);
    if ($#t > 0) {
	my @t2 = do_trim(@t);
#	warn "@t => @t2\n";
	$t = join('; ', @t2);
	$t .= $end_punct;
	Encode::_utf8_on($t);
	$inst->firstChild()->setData($t);
    }
}

sub do_trim {
    my($last_v,$last_t,$last_l) = ('','','');
    my @new_t = ();
    foreach my $i (@_) {
	my($this_v,$this_t,$this_l) = ($i =~ /^(.*?)_(.*?):(.*?)$/);
	if ($i =~ /^(M[0-9R]):(.*)$/) {
	    $this_v = $1;
	    $this_l = $2;
	    $this_t = '';
	}
	my $add = '';
	if (defined($this_v) && defined($this_t) && defined($this_l)) {
	    if ($this_v ne $last_v) {
		$last_v = $this_v;
		$last_t = $this_t;
		$last_l = $this_l;
		push @new_t, $i;
	    } else {
		if ($this_t ne $last_t) {
		    $last_t = $this_t;
		    push @new_t, $this_t.':'.$this_l;
		    $last_t = $this_t;
		} else {
		    $new_t[$#new_t] .= ", $this_l";
		}
	    }
	}
    }
    @new_t;
}

sub remove_nodes {
    my($p,@c) = @_;
    foreach my $c (@c) {
	$p->removeChild($c);
    }
}

$f =~ s/\./+./;
open(X,">$f");
my $s = $x->toString();
Encode::_utf8_on($s);
print X $s;
close(X);

1;
