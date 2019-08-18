package ORACC::CBD::Entries;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/entries_align entries_init entries_term/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
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
		    pp_warn("entry $entry not in base glossary");
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
	if ($cbd[$i] =~ /^\@entry\S*\s+(.*?)\s*$/) {
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

# This routine requires refs to two arrays, each of which is the collection of lines for a single entry.
# All of the constituents of the from slice, $from, are merged into the to slice, $base.
# The merged result is returned as an array.
sub entries_merge {
    my($base,$from,$b_file,$b_line,$s_file,$s_line) = @_;
    my @b = @$base;
    my @s = @$from;
    my $base_b = find_bases(@b);
    my $base_s = find_bases(@s);
    if ($base_b && $base_s) {
	if ($base_b) {
	    my %new_bases = bases_merge($base_b, $base_s, undef);
	    warn Dumper \%new_bases;
	    exit 1;
	}
    } else {
	if ($base_s) {
	    pp_file($s_file);
	    pp_line($s_line);
	    pp_warn("dropping bases from source glo because none in base glo");
	}
    }
    @b;
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
