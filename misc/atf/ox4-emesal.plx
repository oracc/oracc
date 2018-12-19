#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::ATF::OX4;
use ORACC::CBD::Bases;
use Data::Dumper;
use Getopt::Long;

my $byforms = 0;
GetOptions(
    f=>\$byforms,
    );

my %ambo = ();
my %eg = ();
my %es = ();

my $curr_cfgw = '';

bases_init();
open(F, "$ENV{'ORACC_BUILDS'}/epsd2/00src/sux.forms");
while (<F>) {
    my($word,$form) = (/^(.*?)\t\@form\s+(\S+)/);
    if ($byforms) {
	++$eg{"$form"};
    } else {
	++$eg{"$form=$word"};
    }
}
close(F);
my $is_compound = 0;
open(F,  "$ENV{'ORACC_BUILDS'}/epsd2/emesal/00lib/sux-x-emesal.glo");
while (<F>) {
    if (/^\@entry\S*\s+(.*?)\s*$/) {
	$curr_cfgw = $1;
	my $cf = $curr_cfgw; $cf =~ s/\s*\[.*$//;
	$is_compound = 1 if $cf =~ /\s/;
    } elsif (/^\@bases/) {
	my %h = bases_hash($_,$is_compound);
	foreach my $b (keys %h) {
	    next if $b =~ /#/;
	    if ($byforms) {
		++$es{$b};
	    } else {
		++$es{"$b=$curr_cfgw"};
	    }
	}
    } elsif (/^\@form\s+(\S+)/) {
	if ($byforms) {
	    ++$es{$1};
	} else {
	    ++$es{"$1=$curr_cfgw"};
	}
    }
}
close(F);

foreach my $e (keys %es) {
    ++$ambo{$e} if exists $eg{$e};
}

#print "eg = ", Dumper \%eg;
#print "es = ", Dumper \%es;
#print "ambo = ", Dumper \%ambo;

#warn scalar keys %eg, " forms in EG\n";
#warn scalar keys %es, " forms in ES\n";
#warn scalar keys %ambo, " forms in both\n";
#open(A,'>ambig.lst');
#print A join("\n", sort keys %ambo), "\n";
#close(A);

open(OX4, "ox -4 @ARGV |");

if ($byforms) {
    ox4_init(\*OX4,0,0);
} else {
    ox4_init(\*OX4,1,1);
}

while (1) {
    my($ox4,$tok,$sigref) = ox4_next();
    last unless $ox4;
    #    print "$ox4: tok=$tok; sig=", Dumper $sigref;
    my $ox4_cfgw = '';
    if ($sigref && $$sigref{'cf'}) {
	$ox4_cfgw = "$$sigref{'cf'} \[$$sigref{'gw'}\] $$sigref{'pos'}";
    }
    my($lang,$form) = ($ox4 =~ /\%(.*?):(.*?)=/);
#    warn "found mu-tin\n" if $form eq 'mu-tin';
    if ($lang eq 'sux-x-emesal') {
	if (exists($eg{$form}) && !exists($ambo{$form})) {
	    warn "EG `$form' tagged as ES\n";
	} elsif (!exists($es{$form})) {
	    warn "ES unknown form `$form'\n";
	}
    } elsif ($lang eq 'sux') {
	if (exists($es{$form}) && (!exists($ambo{$form}))) { # bias towards emesal when forms only # $byforms || 
	    warn "ES `$form' tagged as EG\n";
	} elsif (!exists($eg{$form})) {
	    warn "EG unknown form `$form'\n";
	}
    }
}

close(OX4);

1;
