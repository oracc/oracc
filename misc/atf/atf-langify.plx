#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my $orc = "$ENV{'ORACC_BUILDS'}";

my $curr_lang = '';

my %ox4 = ();

my %eg = ();
my %es = ();
my %sb = ();

my @eg_forms = qw/a a₂ dumu ša₃ ša₃-bi ša₃-ga ša₃-zu še šu ki ki-ta a-ba ki-a diŋir kur saŋ u₂ u₃ u zi/;
my %eg_forms = (); @eg_forms{@eg_forms} = ();

my @r = `cbd-ripforms.plx $orc/eisl/00lib/sux-x-emesal.glo`;
chomp @r; @es{@r} = ();

@r = `cbd-ripforms.plx $orc/epsd2/emesal/00lib/sux-x-emesal.glo`;
chomp @r; @es{@r} = ();

@r = `cbd-ripforms.plx $orc/eisl/00lib/sux.glo`;
chomp @r; @eg{@r} = ();

@r = `cbd-ripforms.plx $orc/epsd2/00lib/sux.glo`;
chomp @r; @eg{@r} = ();

@r = `cbd-ripforms.plx $orc/epsd2/names/00lib/qpn.glo`;
chomp @r; @eg{@r} = ();

chomp @r; @sb{@r} = ();

if (-r 'langify.ox4') {
    open(OX4, 'langify.ox4');
    while (<OX4>) {
	if (/\@eisl/) {
	    my($ln,$form,$lang) = (/:(\d+):.*?:(.*?)=.*?\t(\S+)$/);
	    push @{$ox4{$ln}} , [ $form , $lang ];
	}
    }
    close(OX4);
    # print Dumper \%ox4; exit 1;
} else {
    warn "$0: proceeding without optional langify.ox4 (use ox -4 00atf/*.atf >langify.ox4 to create if needed)\n";
}

my %w = ();

while (<>) {
    if (/^\s*$/ || /^[\&\#\@\$]/) {
	print;
    } elsif (/^==/) {
	print;
	%w = ();
	s/^\S+\s+//; chomp;
	my @w = split(/\s+/,$_);
	foreach my $w (@w) {
	    $w = trclean($w);
	    ++$w{$w} if $w;
	}
    } elsif (/^\S+[:\.]\s/) {
	chomp;
	my @w = split(/\s+/,$_);
	my $done = 0;
	my @nw = ();
	$curr_lang = ' %eg ';
	push @nw, shift @w; # put line/siglum on list
	foreach my $w (@w) {
	    if ($w eq '%[os]b' || $w =~ /akk/) {
		push @nw, $w;
		$curr_lang = " $w ";
	    }
	    if ($w =~ /(?:\{\{)|(?:\}\})/) {
		push @nw, $w;
		next;
	    }
	    my $xw = trclean($w);
	    unless (length $xw) {
		push @nw, $w;
		next;
	    }
	    my $lang = find_lang($xw);
	    push @nw, $lang unless $lang eq $curr_lang;
	    push @nw, $w;
	    $curr_lang = $lang;
	}
	my $x = join(' ', @nw);
	$x =~ s,//\s+([^/]+)\s+%sb ,// %sb $1 ,;
	$x =~ s/\s+(\%\S+)\s+/ $1 /g;
	print "$x\n";
    } else {
	print;
    }
}

sub trclean {
    my $tmp = shift;
    $tmp =~ s/\^\d+\^//g;
    $tmp =~ tr/-a-wyzŋḫšṣṭA-WYZŊḪŠṢṬ₀-₉0-9{}//cd;
    $tmp =~ s/--+/-/;
    $tmp =~ s/^-//;
    $tmp =~ s/-$//;
    $tmp;
}

sub find_lang {
    my $xw = shift;
    if (exists $eg_forms{$xw}) {
	return ' %eg ';	
    } elsif (exists $es{$xw}) {
	return ' %es ';
    } elsif (exists $eg{$xw}) {
	return ' %eg ';
    } elsif (exists $sb{$xw}) {
	# warn "return \%sb for $xw\n";
	return ' %sb ';
    } else {
	if ($ox4{$.}) {
	    my @ox4 = @{$ox4{$.}};
	    # print STDERR "ox4:$.:";
	    # foreach my $ox4 (@ox4) {
	    #	print STDERR "@$ox4 :: ";
	    # }
	    # print STDERR "\n";
	    foreach my $ox4 (@ox4) {
		my @t = @$ox4;
		if ($t[0] eq $xw) {
		    warn "ox4:$.: setting lang for $xw to $t[1]\n";
		}
	    }
	}
	$curr_lang;
    }
}

1;
