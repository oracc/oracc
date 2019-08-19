package ORACC::CBD::Entries;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/entries_align entries_init entries_term entries_merge entries_collect/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Bases;
use ORACC::CBD::Forms;
use ORACC::CBD::Senses;
use ORACC::CBD::Guess;

use Data::Dumper;

my %entry_indexes = ();
my $map_fh = undef;
my %entry_parts = ();

sub entries_align {
    my($args,$base_cbd,$cbd,$xmap_fh) = @_;

    my @base_cbd = @$base_cbd;
    my @cbd = @$cbd;
    $map_fh = $xmap_fh if $xmap_fh;
    my %entries = entries_collect($base_cbd);
    my %parts = ();
    my %added_entries = ();
    my %incoming_entries = ();
    my $entry = '';
    my $bix = guess_init(@base_cbd);
    my $cix = guess_init(@cbd);

    open(BIX,'>bix.dump'); print BIX Dumper $bix; close(BIX);
    open(BIX,'>cix.dump'); print BIX Dumper $cix; close(BIX);

    for (my $i = 0; $i <= $#cbd; ++$i) {
	pp_line($i+1);
	if ($cbd[$i] =~ /^\@entry\S*\s+(.*?)\s*$/) {
	    $entry = $1;
	    $incoming_entries{$1} = $i;
	    unless (exists $entries{$entry}) {
		# collect entry and list parts needed by it
		if ($xmap_fh) {
		    my $e = grab_entry($i,@cbd);
		    map_entry($args, $entry, $e, $xmap_fh);
		    ++$added_entries{$entry};
		} else {
		    my ($type,@guesses) = guess_entry($entry, $bix, $cix);
		    if ($#guesses >= 0) {
			my $g = join('; ', @guesses);
			pp_warn("entry $entry unknown: [$type]: maybe $g ?");
		    } else {
			pp_warn("entry $entry not in base glossary");
		    }
		}
	    }
	    while ($cbd[$i] !~ /^\@end\s+entry/) {
		if ($cbd[$i] =~ /^\@parts\s+(.*?)\s*$/) {
		    my $p = $1;
		    $p =~ s/\s+/ /g;
		    map_parts($args,$entry,$p) unless ${$entry_parts{$entry}}{$p};
		    foreach my $p (split_parts($cbd[$i])) {
			push @{$parts{$p}} , [pp_line(), $entry];
		    }
		}
		++$i;
	    }
	}
    }
    foreach my $p (sort keys %parts) {
	foreach my $pref (@{$parts{$p}}) {
	    my($l,$e) = @$pref;
	    $p =~ s/\s*\[(.*?)\]\s*/ \[$1\] /;
	    if ((my $i = $incoming_entries{$p})) {
		if ($cbd[$i] =~ /^\@entry\S*\s+(.*?)\s*$/) {
		    my $entry = $1;
		    unless (exists $entries{$entry}
			    || $added_entries{$entry}) {
			my $e = grab_entry($i,@cbd);
			map_entry($args, $entry, $e);
		    }
		} else {
		    warn "$0: internal error: cbd[$i] is not an \@entry line\n";
		}
	    } else {
		pp_line($l);
		pp_warn("parts entry $p is not in glossary");
	    }
	}
    }
}

sub split_parts {
    my $p = shift;
    $p =~ s/^\@parts\S*\s+//;
    $p =~ s/(\]\S+)\s+/$1\cA/g;
    split(/\cA/, $p);
}

sub grab_entry {
    my($i,@cbd) = @_;
    my @e = ();
    do {
	push @e, $cbd[$i];
    } while ($cbd[$i++] !~ /^\@end\s+entry/);
    join("\cA", @e);
}

sub map_entry {
    my($args,$entry,$xentry) = @_;
    $entry =~ s/\s+\[(.*?)\]\s+/[$1]/;
    #    print MAP_FH '@'.project($$args{'cbd'}).'%'.lang().":$entry => #new\n";
    $xentry = '' if $$args{'log'};
    print $map_fh pp_file().':'.pp_line().": add entry $entry => $xentry\n";
}

sub map_parts {
    my($args,$entry,$parts) = @_;
    $entry =~ s/\s+\[(.*?)\]\s+/[$1]/;
    print $map_fh "add parts $entry => \@parts $parts\n";
}

sub entries_collect {
    my @cbd = @{$_[0]};
    my %e = ();
    my $curr_entry = '';
    for (my $i = 0; $i <= $#cbd; ++$i) {
	if ($cbd[$i] =~ /^[-+>=]*\@entry\S*\s+(.*?)\s*$/) {
	    $curr_entry = $1;
	    ++$e{$1};
	    $entry_indexes{$1} = $i;
	} elsif ($cbd[$i] =~ /^\@parts\s+(.*?)\s*$/) {
	    my $p = $1;
	    $p =~ s/\s+/ /g;
	    ++${$entry_parts{$curr_entry}}{$p};
	}
    }
    %e;
}

sub find_bases {
    foreach (@_) {
	return $_ if /^\@bases/;
    }
    undef;
}

# This routine requires refs to two arrays, each of which is the collection of lines for a single entry.
# All of the constituents of the from slice, $from, are merged into the to slice, $base.
# The merged result is returned as an array.
sub entries_merge {
    my($base,$from,$b_file,$b_line,$s_file,$s_line) = @_;
    my @b = @$base;
    my @s = @$from;

    my $base_b = find_bases(@b);
    my $base_s = find_bases(@s);
    my $new_b_hash = undef;
    my $new_b = '';
    if ($base_b && $base_s) {
	if ($base_b) {
	    $new_b_hash = bases_merge($base_b, $base_s, undef);
	    warn Dumper $new_b_hash;
	}
    } else {
	if ($base_s) {
	    pp_file($s_file);
	    pp_line($s_line);
	    pp_warn("dropping bases from source glo because none in base glo");
	}
    }
    if ($new_b_hash) {
	$new_b = bases_serialize(%$new_b_hash);
	warn "new bases => $new_b\n";
    } else {
	$new_b = $base_b;
    }

    my @new_f = forms_merge(\@b,\@s,%{$$new_b_hash{'#map'}});
    my @new_s = senses_merge_2(\@b,\@s);
    
    my @n = ();
    my $forms_done = 0;
    my $senses_done = 0;
    foreach my $b (@b) {
	if ($b =~ /^\@bases/) {
	    push @n, "\@bases $new_b";
	} elsif ($b =~ /^\@form/) {
	    unless ($forms_done) {
		++$forms_done;
		push @n, @new_f;
	    }
	} elsif ($b =~ /^\@sense/) {
	    unless ($senses_done) {
		++$senses_done;
		push @n, @new_s;
	    }
	} else {
	    push @n, $b;
	}
    }
    @n;
}

sub entries_init {
    my $args = shift;
    my $entries_outfile = $$args{'lang'}.'.map';
    if (-d '01tmp') {
	$entries_outfile = "01tmp/$entries_outfile";
    }
    open(MAP_FH, ">>$entries_outfile");
    $map_fh = \*MAP_FH;
}

sub entries_term {
    close(MAP_FH);
}

1;
