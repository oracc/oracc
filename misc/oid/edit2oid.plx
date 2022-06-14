#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::OID;

# edit2oid.plx : turn a CBD edit script into an OID edit script

my $D = `isogmt`."\t"; # date field configured from :dat; default to today

my $e = '';
my $edit_oid = '';

my $edit_entry = '';
my $edit_sense = '';

my %new_ent = ();
my %new_oid = ();

my %seen = ();

my @edit = (<>); chomp @edit;
for (my $i = 0; $i <= $#edit; ++$i) {
    $_ = $edit[$i];
    if (s/^:rnm \>\s*//) {
	if (s/^.?\@entry\*?\s+//) {
	    if ($edit_oid) {
		s/\s+(\[.*?\])\s+/$1/;
		print "$D$edit_oid\t$edit_entry\t>\t$_\n";
		$new_oid{$_} = $edit_oid;
		$new_ent{$edit_entry} = $_;
	    }
	} else {
	    s/^.?\@sense\s+//;
	    my $from_e = $edit_entry;
	    my($epos,$mean) = ($edit_sense =~ /\s(\S+)\s+(.*)$/);
	    if ($epos && $mean) {
		$from_e =~ s#](\S+)$#//$mean]$1'$epos#;
		my $from_o = oid_lookup('sux',$from_e);
		my $to_e = $new_ent{$edit_entry} || $edit_entry;
		($epos,$mean) = (m/^(\S+)\s+(.*)$/);
		$to_e =~ s#](\S+)$#//$mean]$1'$epos#;
		if ($from_o) {
		    print "$D$from_o\t$from_e\t>\t$to_e\n" if $from_o;
		} else {
		    ### don't warn here: we are renaming something that never got an OID
		    ### warn "no OID for $from_e\n" unless $from_o; ### 
		}
	    } else {
		warn "edit.edit:$i: syntax error: $_\n";
	    }
	}
    } elsif (/:ent\s+(.*?)$/) {
	$edit_entry = $1; $edit_entry =~ s/^-?\@entry\*?\s+//; $edit_entry =~ s/\s+(\[.*?\])\s+/$1/;
	$edit_oid = oid_lookup('sux', $edit_entry) || '';
	warn "no OID for $edit_entry\n" unless $edit_oid;
	$edit_sense = '';
    } elsif (/:sns\s+(.*?)$/) {
	$edit_sense = $1;
    } elsif (s/:mrg\s+//) {
	if (/entry/) {
	    my $mrg_entry = $_; $mrg_entry =~ s/.?\@entry\*?\s+//; $mrg_entry =~ s/\s+(\[.*?\])\s+/$1/;
	    my $mrg_oid = $new_oid{$mrg_entry} || oid_lookup('sux',$mrg_entry);
	    if ($mrg_oid) {
		print "$D$edit_oid\t$edit_entry\t=\t$mrg_oid\t$mrg_entry\n";
		$new_oid{$edit_entry} = $mrg_oid;
		$new_ent{$edit_entry} = $mrg_entry;
	    } else {
		warn "no OID for merge target $mrg_entry\n";
	    }
	} else {
	    warn "merge sense not yet implemented\n";
	}
    } elsif (/:del/) {
	my $why = $edit[$i+1]; $why =~ s/:why\s+// || ($why = '');
	if (/entry/) {
	    print "$D$edit_oid\t$edit_entry\t-\t$why\n";
	} else {
	    my $s = $edit_entry; 
	    my($epos,$mean) = (/^:del\s+-\@sense\s+(\S+)\s+(.*$)/);
	    $s =~ s#\](\S+)#//$mean]$1'$epos#;
	    my $s_oid = oid_lookup('sux', $s);
	    my $r_oid = $new_oid{$edit_entry} || $edit_oid;
	    my $r_ent = $new_ent{$edit_entry} || $edit_entry;
	    if ($s_oid) {
		print "$D$s_oid\t$s\t-\t$r_oid\t$r_ent\n";
	    } else {
		### don't warn here: we are deleting something that never got an OID
		### warn "no OID for sense $s\n"; ###
	    }
	}
    } elsif (s/^:add\s+//) {
	if (s/^.?\@entry\*?\s+//) {
	    s/\s+(\[.*?\])\s+/$1/;
	    my $ent = $_;
	    my $o = oid_lookup('sux',$ent);
	    if ($o) {
		if ($seen{$ent}++) {
		    # silently skip duplicates
		} else {
		    print "$D$o\t$ent\t+\n";
		}
	    } else {
		warn "no OID for added entry $ent\n";
	    }
	} else {
	    my $s = $new_ent{$edit_entry} || $edit_entry; 
	    my($epos,$mean) = (/^.?\@sense\s+(\S+)\s+(.*$)/);
	    $s =~ s#\](\S+)#//$mean]$1'$epos#;
	    my $o = oid_lookup('sux',$s);
	    if ($o) {
		if ($seen{$s}++) {
		    # silently skip duplicates
		} else {
		    print "$D$o\t$s\t+\n";
		}
	    } else {
		warn "$s\n";
	    }
	}
    } elsif (/^:why/) {
	# already handled
    } elsif (/^\@/ || /^:cbd/) {
	# ignore line numbers
    } elsif (/^:dat\s+(\S+)/) {
	$D = "$1\t";
    } else {
	warn "edit.edit:$i: unhandled edit script tag $_\n";
    }
}

1;
