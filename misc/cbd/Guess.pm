package ORACC::CBD::Guess;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/guess_entry guess_init/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Bases;
use ORACC::CBD::Forms;
use ORACC::CBD::History;
use ORACC::CBD::Senses;
use ORACC::SL::BaseC;
use Data::Dumper;

my $guess_trace = 1;

sub guess_entry {
    my($entry,$bix,$cix) = @_;

    print G "===\nentering guess_entry for '$entry'\n" if $guess_trace;

    #let's see if we've done this one before in the history of cbd edit
    my $g = history_guess($entry);
    if ($g ne $entry) {
	print G "returning via history\n===\n" if $guess_trace;
	return '#history', $g;
    }
    
    my $e_no_pos = $entry; $e_no_pos =~ s/\].*$/]/;

    my $e_words = $entry; $e_words =~ s/^.*?\[(.*?)\].*$/$1/;
    my @e_words =  de_common_list(split(/[\s,]/, $e_words));

    my %bix = %$bix;
    my %cix = %$cix;

    if (${$bix{'no_pos'}}{$e_no_pos}) {
	print G "returning via no_pos\n===\n" if $guess_trace;
	return '#nopos', @{$bix{'no_pos'}{$e_no_pos}};
    }

    # get all the entries that have words in common
    my %ew = ();

    if (${$cix{'words'}}{$entry}) {
	foreach my $w (@{$cix{'words'}{$entry}}) {
	    if (${$bix{'words'}}{$w}) {
		my @ww = keys %{$bix{'words'}{$w}};
		print G "bix-words{$w} => @ww\n" if $guess_trace;
		@ew{@ww} = ();
	    }
	}
    }

    # get all the entries that have bases in common
    my %bw = ();
    if (${$cix{'bases'}}{$entry}) {
#	warn "cix{bases}{$entry} = @{$cix{'bases'}{$entry}}\n";
	foreach my $s (@{$cix{'bases'}{$entry}}) {	    
	    if (${bix{'bases'}}{$s}) {
		my @bb = keys %{$bix{'bases'}{$s}};
		print G "bix-bases{$s} => @bb\n" if $guess_trace;
		@bw{@bb} = ();
	    }
	}
    }

    # now get the base and word union
    my %u = ();
    foreach my $bw (keys %bw) {
	++$u{$bw} if exists $ew{$bw};
    }
    
    my @u = sort keys %u;

    print G "u-union = @u\n" if $guess_trace;

    return '#union', @u if $#u >= 0;

    @u = sort keys %bw;

    print G "u-bases = @u\n" if $guess_trace;

    return '#bases', @u if $#u >= 0;
    
    @u = sort keys %ew;

    print G "u-words = @u\n" if $guess_trace;

    return '#words', @u if $#u >= 0 && $#u <= 4; # don't print pathological cases like 'fish'
    
    ();
}

sub guess_init {
    my @cbd = @_;
    my %cix = ();
    my %ix = ();

    history_all_init();
    
    if ($guess_trace) {
	open(G, '>guess.log');
    }

    # index entries without their POS
    my %e = ORACC::CBD::Entries::entries_collect(\@cbd);
    foreach my $e (keys %e) {
	my $e2 = $e; $e2 =~ s/\].*$/]/;
	push @{$ix{$e2}}, $e;
    }
    $cix{'no_pos'} = { %ix }; %ix = ();
    
    # index entries by GW//SENSE
    my %s = senses_collect(@cbd);
    foreach my $e (keys %e) {
	my $e2 = $e; $e2 =~ s/^.*?\[(.*?)\].*$/$1/; $e2 =~ s#//# #;
	my @e_words = grep(length,de_common_list(split(/[\s,]/, $e2)));
	my %ew = ();
	foreach my $ew (@e_words) {
	    ++${$ix{$ew}}{$e};
	    ++$ew{$ew};
	}
	if ($s{$e}) {
#	    warn "senses of $e\n";
	    foreach my $s (@{$s{$e}}) {
#		warn "\t$s\n";
		$s =~ s/^.*?\@sense\s+\S+\s+//;
		@e_words = grep(length,de_common_list(split(/[\s,]/, $s)));
		foreach my $ew (@e_words) {
		    # push @{$ix{$ew}}, $e;
		    ++${$ix{$ew}}{$e};
		    ++$ew{$ew};
		}
 	    }
	}
	my @x = keys %ew;
	@{$ix{$e}} = @x;
#	if ($e =~ /^alan /) {
#	    warn "alan words = @x\n";
#	}
    }
    $cix{'words'} = { %ix }; %ix = ();

    # index entries by base signatures
    bases_init();
    my %b = bases_collect(@cbd);
    open(B,'>B');
    foreach my $b (keys %b) {
	next if $b =~ /\s\S+\s+\[/;
	my %bh = bases_hash($cbd[$b{$b}],0,$b{$b});
#	print B Dumper(\%bh);
	my @sigs = keys %{$bh{'#sigs'}};
#	warn "sigs for $b = @sigs\n";
	@{$ix{$b}} = @sigs;
	foreach my $s (@sigs) {
	    ++${$ix{$s}}{$b};
	}
    }
    close(B);
    $cix{'bases'} = { %ix }; %ix = ();
    
    return { %cix };
}

sub uniq {
    my %u = ();
    @u{@_} = ();
    return keys %u;
}

1;
