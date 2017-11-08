#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; 
binmode STDOUT, ':utf8'; 
binmode STDERR, ':utf8';

use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::NS;

my $lang = shift @ARGV;
#die "epadify.plx: I only work with Sumerian\n"
#    if $lang && $lang ne 'sux';
exit 1 unless $lang;
exit 0 if $lang ne 'sux';
die "epadify.plx: I work on 01bld/sux/sux.cbd and it's not there\n"
    unless -r "01bld/sux/sux.cbd";
open(IN,'00src/epad-glo.txt') 
    || die "epadify.plx: I can't work without 00src/epad.glo and it's not there\n";

my %aux = ();
my $id = 'epad00000';
my %ix = ();
my($cf,$gw,$pos) = ();
my @senses = ();
my @equivs = ();

open(X, '>02xml/epad.xml') || die; select X;
print '<?xml version="1.0" encoding="utf-8"?>', "\n";
print '<epad>';
while (<IN>) {
    chomp;
    if (/^\@entry/) {
	if (/\s+(.*?)\s+\[(.*?)\]\s+(\S+)\s*$/) {
	    ($cf,$gw,$pos) = ($1,$2,$3);
	    @senses = ();
	    @equivs = ();
	} else {
	    warn "00src/epad.glo:$.: bad syntax in \@entry line\n";
	}
    } elsif (s/^\@sense\s+//) {
	push @senses, [ $id , $_ ];
    } elsif (/^\@equiv/) {
	if (/\%sux\s+(.*?)\s*$/) {
	    push @equivs, $_;
	}
    } elsif (/^\@end entry/) {
	push @{$ix{$cf,'m'}}, @senses;
	my $str = "<term cfgw=\"$cf\[$gw\]$pos\">$cf</term><mngs>";
	foreach my $s (@senses) {
	    my $xs = xmlify($s);
	    $str .= "<mng>$$s[1]</mng>";
	}
	$str .= '</mngs>';
	$ix{$id} = $str;
	push @{$ix{$cf}}, $id;
	foreach my $e (@equivs) {
	    push @{$ix{$cf,$e}}, $id;
	}
	print "<entry xml:id=\"$id\">";
	print "<cf>$cf</cf><gw>$gw</gw><pos>$pos</pos>";
	print $str;
	print '</entry>';
	++$id;
    }
}
close(IN);
print X '</epad>';

#use Data::Dumper; print STDERR Dumper \%ix;

### THIS IS A WAY OF SUPPLEMENTING EPAD.GLO BUT IT SHOULD BE BUILTIN TO EPAD.GLO

#open(AUX,'00any/akk.aux') || die;
#while (<AUX>) {
#    my($wd,$mn) = (/^(.*?)\t(.*?)$/);
#    $wd =~ s/\s+[IVX]+$//;
#    $wd =~ s/\(.\)//;
#    $wd =~ s/m$//;
#    $wd =~ tr/ḫḪ/hH/;
#    push @{$aux{$wd}}, $mn;
#}
#close(AUX);

#open(AUX, '>01tmp/aux.log') || die;

my $cbd = load_xml('01bld/sux/sux.cbd') || die;
foreach my $e (tags($cbd,$CBD,'equiv')) {
    my $term = $e->textContent();
    my ($cf,$gw) = ();
    if ($term =~ /^(.*?)\s*\[(.*?)\]/) {
	($cf,$gw) = ($1,$2);
    } else {
	$cf = $term;
	$gw = '';
    }
    my $ok = cfgw_ok($cf,$gw);
    unless ($ok) {
	my $gw_node = (tags($e->parentNode()->parentNode(),$CBD,'gw'))[0];
	my $gw_text = $gw_node->textContent();
	# warn "gw_text=$gw_text\n";
	$ok = cfgw_ok($cf,$gw_text);
    }
    if ($ok) {
	$e->removeChildNodes();
	my $chunk = load_xml_chunk("<chunk>$ix{$ok}</chunk>");
	foreach my $c ($chunk->firstChild()->childNodes()) {
	    $e->appendChild($c);
	}
#	warn "found $term in epad\n";
    } else {
	if ($aux{$cf}) {
	    my @m = @{$aux{$cf}};
	    my $m = join(' :: ', @m);
	    my $entry_node = $e->parentNode()->parentNode();
	    my $e_cfgwpos = (tags($entry_node,$CBD,'cf'))[0]->textContent()
		.'['.(tags($entry_node,$CBD,'gw'))[0]->textContent()
		.']'.(tags($entry_node,$CBD,'pos'))[0]->textContent();
	    print AUX "$cf\t$m\t$e_cfgwpos\n";
#	    if ($#m == 0) {
#		warn "aux has $term\n";
#	    } else {
#		warn "aux has ambiguous $term\n";		
#	    }
	} else {
	    warn "no entry for $term in epad\n"
		unless $term eq '?';
	}
    }
}

close(AUX);

system 'mv', '01bld/sux/sux.cbd', '01bld/sux/sux.cbd.orig';
open(N,'>01bld/sux/sux.cbd') || die;
binmode N, ':raw';
print N $cbd->toString(0);
close(N);

#################################################################

sub
cfgw_ok {
    my($cf,$gw) = @_;
    if ($ix{$cf}) {
	if ($gw) {
	    my @m = @{$ix{$cf,'m'}};
	    foreach my $pair (@m) {
		my($id,$m) = @$pair;
#		warn "trying $m for $cf\n";
		if ($m =~ /\Q$gw/) {
		    return $id;
		}
	    }
	    warn "epad has $cf but not with meaning $gw\n";
	} else {
	    my @ids = @{$ix{$cf}};
	    if ($#ids > 0) {
		warn "multiple entries match $cf\n";
	    } else {
		return $ids[0];
	    }
	}
    }
    undef;
}

1;
