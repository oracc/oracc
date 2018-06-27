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

sub entries_align {
    my($args,$base_cbd,$cbd,$xmap_fh) = @_;

    my @base_cbd = @$base_cbd;
    my @cbd = @$cbd;
    $map_fh = $xmap_fh if $xmap_fh;
    my %entries = entries_collect($base_cbd);
    my %parts = ();
    my %added_entries = ();
    my %incoming_entries = ();

    for (my $i = 0; $i <= $#cbd; ++$i) {
	if ($cbd[$i] =~ /^\@entry\S*\s+(.*?)\s*$/) {
	    my $entry = $1;
	    $incoming_entries{$1} = $i;
	    unless (exists $entries{$entry}) {
		# collect entry and list parts needed by it
		my $e = grab_entry($i,@cbd);
		map_entry($args, $entry, $e, $xmap_fh);
		++$added_entries{$entry};
	    }
	    while ($cbd[$i] !~ /^\@end\s+entry/) {
		if ($cbd[$i] =~ /^\@parts/) {
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
    print $map_fh "add entry $entry => $xentry\n";
}

sub entries_collect {
    my @cbd = @{$_[0]};
    my %e = ();
    for (my $i = 0; $i <= $#cbd; ++$i) {
	if ($cbd[$i] =~ /^\@entry\S*\s+(.*?)\s*$/) {
	    ++$e{$1};
	    $entry_indexes{$1} = $i;
	}
    }
    %e;
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
