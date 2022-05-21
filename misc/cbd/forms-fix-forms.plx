#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::CBD::Bases;
use Getopt::Long;

GetOptions(
    );

#
# The log file gives the file names for CORE and PERI so just load
# those when the FILE statements are read.
#
# Then for each CORE/PERI pair replace the one prefixed by '-' with
# the unmarked one.
#
# \@bases lines in both CORE and PERI are rewritten to be the same;
# when CORE is being fixed from PERI, PERI is used as the basis for
# the new line and CORE bases are made alts. In the opposite case CORE
# is used as the basis and PERI bases become alts.
#
# Edited outputs are placed in CORE.glo and PERI.glo
#

my $logfile = shift; die "$0: must give log file on command line\n" unless $logfile;
open(L,$logfile) || die "$0: can't open log file $logfile\n";
my @log = (<L>); chomp @log;
close(L);

my %FILE = ();
bases_init();
for (my $i = 0; $i <= $#log; ++$i) {
    next if $log[$i] =~ /: missing form: /;
    if ($log[$i] =~ /^FILE:(CORE|PERI)=(.*?)$/) {
	my($type,$file) = ($1,$2);
	open(F,$file) || die "$0: can't open $type file $file\n";
	@{$FILE{$type}} = (<F>); chomp @{$FILE{$type}};
	close(F);
    } elsif ($log[$i] =~ /discrepant forms/) {
	my($CORE,$PERI) = ($log[$i+1],$log[$i+2]);
	my($CORELN) = ($CORE =~ /:(\d+):/); --$CORELN;
	my($PERILN) = ($PERI =~ /:(\d+):/); --$PERILN;
	if ($CORE =~ /^-/) {
	    my $core = ${$FILE{'CORE'}}[$CORELN];
	    my $orig = $core;
	    if ($core =~ s/\t\@form.*$//) {
		$core .= "\t$PERI";
	    } else {
		$PERI =~ s/^.*?\@form/\@form/;
		$core = $PERI;
	    }
	    warn "reset CORE $CORELN from PERI $PERILN\n\t$orig\n =>\t$core\n";
	    ${$FILE{'CORE'}}[$CORELN] = $core;
	    reset_bases('PERI',$PERILN,'CORE',$CORELN);
	} elsif ($PERI =~ /^-/) {
	    my $peri = ${$FILE{'PERI'}}[$PERILN];
	    my $orig = $peri;
	    if ($peri =~ s/\t\@form.*$//) {
		$peri .= "\t$PERI";
	    } else {
		$CORE =~ s/^.*?\@form/\@form/;
		$peri = $CORE;
	    }
	    warn "reset PERI $PERILN from CORE $CORELN\n\t$orig\n =>\t$peri\n";
	    ${$FILE{'PERI'}}[$PERILN] = $peri;
	    reset_bases('CORE',$CORELN,'PERI',$PERILN);
	} else {
	    my $ln = $i+1;
	    warn "$logfile:$ln: neither CORE nor PERI has '-' prefix\n";
	}
    }
}
bases_term();
open(C,'>CORE.glo') || die "$0: can't write CORE.glo\n";
print C join("\n",@{$FILE{'CORE'}}), "\n";
close(C);
open(P,'>PERI.glo') || die "$0: can't write PERI.glo\n";
print P join("\n",@{$FILE{'PERI'}}), "\n";
close(P);

##################################################################################

sub reset_bases {
    my($base,$base_ln,$inco,$inco_ln) = @_;
    my $base_bases_ln = find_bases($base,$base_ln);
    my $inco_bases_ln = find_bases($inco,$inco_ln);
    my $base_entry_ln = find_entry($base,$base_ln);
    my $entry = ${$FILE{$base}}[$base_entry_ln];
    $entry =~ s/^\S*\@entry\S*\s+(.*?)\s*$/$1/;
    my $newb = bases_merge(${$FILE{$base}}[$base_bases_ln], ${$FILE{$inco}}[$inco_bases_ln],
			   0, $base_bases_ln, $entry, 'sux');
    my $newb_str = bases_string($newb);
    warn "newb => $newb_str\n";
    ${$FILE{$base}}[$base_bases_ln] = ${$FILE{$inco}}[$inco_bases_ln] = "\@bases $newb_str";
}

sub find_bases {
    my($f,$ln) = @_;
    my @f = @{$FILE{$f}};
    for (my $i = $ln; $ln; --$ln) {
	return $ln if $f[$ln] =~ /\@bases/;
    }
    return -1;
}

sub find_entry {
    my($f,$ln) = @_;
    my @f = @{$FILE{$f}};
    for (my $i = $ln; $ln; --$ln) {
	return $ln if $f[$ln] =~ /\@entry/;
    }
    return -1;
}

1;
