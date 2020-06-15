package ORACC::CBD::Senses;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/senses_align senses_collect senses_init senses_term
    senses_merge senses_merge_2 senses_string senses_uniq de_common_list/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::History;

my $acd_rx = $ORACC::CBD::acd_rx;

my $use_map_fh;

use Data::Dumper;

my @common1 = qw/a an in is of or the to/;
my %common1 = (); @common1{@common1} = ();

my @common2 = qw/and be by designation down for form make off on out part status type up/;
my %common2 = (); @common2{@common2} = ();

my %sense_lnum = ();

my %sense_map = ();

my $map_fh = undef;

sub senses_align {
    my($args,$base_cbd,$cbd,$xmap_fh) = @_;

    my @base_cbd = @$base_cbd;
    my @cbd = @$cbd;
    
    if ($xmap_fh) {
	$map_fh = $xmap_fh;
	$use_map_fh = 1;
    } else {
	$use_map_fh = 0;
    }

    history_all_init();
    
    my $cbd_cbdname = cbdname_from_fn($$args{'cbd'});
    my %in_cbddata = %{$ORACC::CBD::data{$cbd_cbdname}};
    my %entry_map = %{$in_cbddata{'entry_map'}};
    %sense_map = %{$in_cbddata{'sense_map'}};
    
    my @senses = ();
    my @lines = ();
    my %base_senses = senses_collect(@base_cbd);
    my $curr_entry = '';
    my $last_sense = 0;

    for (my $i = 0; $i <= $#cbd; ++$i) {
	if ($cbd[$i] =~ /^$acd_rx?\@entry\S*\s+(.*?)\s*$/) {
	    $curr_entry = $1;
	    # warn "$curr_entry\n";
	    @lines = ();
	} elsif ($cbd[$i] =~ /^\@sense/) {
	    $cbd[$i] =~ s/\s+/ /g;
	    $cbd[$i] =~ s/\s*$//;
	    if ($sense_map{$curr_entry}) {
		if (${$sense_map{$curr_entry}}{$cbd[$i]}) {
		    push @senses, ${$sense_map{$curr_entry}}{$cbd[$i]};
		}
	    } else {
		push @senses, $cbd[$i];
	    }
	    push @lines, $i+1;
	    my $ss = $cbd[$i]; $ss =~ s/^\@sense\s+//;
	    $sense_lnum{$ss} = $i+1;
	} elsif ($cbd[$i] =~ /^\@end\s+entry/) {
	    if ($#senses >= 0) {

		my $sense_entry = $curr_entry;
		if ($entry_map{$sense_entry}) {
		    $sense_entry = $entry_map{$sense_entry};
		}

		my $senses_b = $base_senses{$sense_entry};
		if ($senses_b) {
		    my $senses_b_str = senses_string($senses_b);
		    my $senses_i_str = senses_string([ @senses ]);
#		    warn "aligning:\n\t$senses_i_str\ninto\t$senses_b_str\n" ;
#			if $ORACC::CBD::PPWarn::trace;
#		    my $nsenses = 
		    senses_merge($args, 
				 # $curr_entry
				 $sense_entry, 
				 [ @senses ], $senses_b,
				 @lines,
			);
#		    if ($#$nsenses >= 0) {
#			my $nsenses_str = senses_string($nsenses);
#			$nsenses_str =~ tr/\t//d;
#			$nsenses_str =~ s/^/\n/;
#			my $ins = $base_senses{$curr_entry,'insert'};
#			$base_cbd[$ins] .= $nsenses_str;
#			warn "adding $senses_b_str after sense at pos $ins\n"
#			    if $ORACC::CBD::PPWarn::trace;
#		    }
		}
		@senses = ();
	    } else {
		warn "$0: no \@senses in $curr_entry\n" if $ORACC::CBD::PPWarn::trace;
	    }
	}    
    }

    history_all_term();
    
    %sense_map;
}

sub senses_collect {
    my @cbd = @_;
    my %s = ();
    my $curr_entry = '';
    my $last_sense = 0;
    my @s = ();
    for (my $i = 0; $i <= $#cbd; ++$i) {
	if ($cbd[$i] =~ /^$acd_rx?\@entry\S*\s+(.*?)\s*$/) {
	    $curr_entry = $1;
	} elsif ($cbd[$i] =~ /^\@sense/) {
	    $cbd[$i] =~ s/\s+/ /g;
	    $cbd[$i] =~ s/\s*$//;
	    push @s, $cbd[$i];
	    $last_sense = $i;
	} elsif ($cbd[$i] =~ /^\@end\s+entry/) {
	    $s{$curr_entry} = [ @s ];
	    $s{$curr_entry,'insert'} = $last_sense;
	    @s = ();
	}
    }
    open(S,'>senses.dump'); print S Dumper \%s; close(S);
    %s;
}

sub make_entry_sense {
    my($e,$s) = @_;
    $e =~ s/\s+(\[.*?\])\s+/$1/;
    my($pos,$sns) = ($s =~ /^\@sense\!?\s+(\S+)\s+(.*)$/);
    if ($pos && $sns) {
	$e =~ s#(\]\S+)$#//$sns$1'$pos#;
    } else {
	warn "make_entry_sense passed bad args: $e , $s\n";
    }
    $e;
}

sub senses_merge {
    my($args,$entry,$i,$b,@l) = @_;
    my %map = ();
    my @newb = ();
    my %bb = ();
    @bb{@$b} = ();
#    print Dumper \%bb;
    my %b = index_senses(@$b);
#    print Dumper \%b;
    foreach my $s (@$i) {
	pp_line(shift @l);
	my @matches = ();
	if (exists $bb{$s}) {
	    # warn "$s exists verbatim in base\n";
	    next;
	}
	my $es = make_entry_sense($entry,$s);
	my $guess = history_guess_sense($es);
	if ($guess ne $es) {
	    my $m = $guess; $m =~ s#^.*?//##; $m =~ s/\].*?'/]/; $m =~ s/^(.*?)](\S+)$/\@sense $2 $1/;
#	    warn "history_guess found $guess from $es; mapping to $m\n";
	    map_sense($args, '0', $entry, $s, $m);
	    next;
	}
	# does s occur in @b?
	foreach my $b (@$b) {
	    if ($b =~ /\Q$s/ || $s =~ /\Q$b/) {
		push @matches, $b;
	    }
	}
	if ($#matches == 0) {	    
	    map_sense($args, '1', $entry, $s, $matches[0]); # ideally would check for leftover tokens not in base
	    next;
	} else {
	    my %i = index_senses($s);
#	    print Dumper \%i;
	    my %bs = ();
	    # create a hash where the keys are the index into @$b
	    # and the values are the number of tokens that occur in
	    # each of the keys--this means that the highest token
	    # count is the best match
	    foreach my $i (keys %i) {
		if ($b{$i}) {
		    foreach my $bs (@{$b{$i}}) {
			++$bs{"#$bs"};
		    }
		}
	    }
	    foreach my $k (keys %bs) {
		unless (defined $bs{$k}) {
		    warn "\$bs{ $k } has undefined value\n" ;
		    $bs{$k} = 0;
		}
	    }
#	    print Dumper \%bs;
	    if (scalar keys %bs) {
		my @m = sort { &bscmp(\%bs) } keys %bs;
		my $index = $m[0]; $index =~ s/^#//; # this may be too rough and ready; perhaps need diagnostic when multiple candidates
		map_sense($args, '2', $entry, $s, ${$b}[$index]);
	    } else {
		if ($use_map_fh) {
		    $s =~ s/sense/sense+/;
		    warn "Senses[3]: adding $s\n" if $ORACC::CBD::PPWarn::trace;
		    add_sense($args, $entry, $s);
		    push @newb, $s;
		} else {
		    my $ee = $entry;
		    $ee =~ s/\s+\[/[/; $ee =~ s/\]\s+$/]/;
		    if ($#$b == 0) {
			if ($$args{'apply'}) {
			    map_sense($args, '2', $entry, $s, $$b[0]);			    
			} else {
			    pp_warn("SENSE[3] $ee: $s >> $$b[0]");
			}
		    } else {
			pp_warn("SENSE[4] $ee: $s !! new or bad sense");
		    }
		}
	    }
	}	
    }
#    [ @newb ];
}

sub bscmp {
    my %bs = %{$_[0]};
    my $bsa = $bs{$a} || 0;
    my $bsb = $bs{$b} || 0;
    $bsb <=> $bsa || $a cmp $b
}
sub add_sense {
    my($args,$entry,$in) = @_;
    $entry =~ s/\s+\[(.*?)\]\s+/[$1]/;
#    my $add_sig = $entry;
#    $entry = $add_sig;
    my($epos,$sense) = ($in =~ /^\@sense\S*\s+(\S+)\s+(.*?)\s*$/);
    #    $add_sig =~ s#](\S+)#//$sense]$1'$epos#;
    pp_line($sense_lnum{"$epos $sense"});
    print $map_fh pp_file().':'.pp_line().": add sense $entry => \@sense $epos $sense\n";
}

sub map_sense {
    my($args,$code,$entry,$in,$base) = @_;
    my $from_sig = $entry;
    $from_sig =~ s/\s+\[(.*?)\]\s+/[$1]/;
    my $to_sig = $from_sig;
    my($epos,$sense) = ($in =~ /^\@sense\S*\s+(\S+)\s+(.*?)\s*$/);
    my ($epos_b,$sense_b) = ($base =~ /^\@sense\S*\s+(\S+)\s+(.*?)\s*$/);
    
    if ($sense ne $sense_b) {
	if ($use_map_fh) {
	    $from_sig =~ s#](\S+)#//$sense]$1'$epos#;
	    $to_sig =~ s#](\S+)#//$sense_b]$1'$epos_b#;
	    warn "Senses[$code]: mapping $from_sig => base $to_sig\n" if $ORACC::CBD::PPWarn::trace;
	    # print MAP_FH '@'.project($$args{'cbd'}).'%'.lang().":$from_sig => $to_sig\n";
	    print $map_fh "map sense $from_sig => $to_sig\n";
	} else {
	    $entry =~ s/\s+\[/[/; $entry =~ s/\]\s+/]/;
	    if ($code == 0) { # this SENSE was mapped based on history.all
		pp_warn("SENSE[0] $entry: $in ~ $base") unless $$args{'apply'};
		${$sense_map{$entry}}{$in} = $base;
	    } elsif ($code == 1) { # this SENSE is a subset of base SENSE or vice versa
		pp_warn("SENSE[1] $entry: $in ~ $base") unless $$args{'apply'};
		${$sense_map{$entry}}{$in} = $base;
	    } elsif ($code == 2) { # this SENSE has token matches with base SENSE or has been vetted
		pp_warn("SENSE[2] $entry: $in > $base") unless $$args{'apply'};
		${$sense_map{$entry}}{$in} = $base;
	    } elsif ($code == 3) { # this SENSE doesn't match anything in base cbd
		die "(This can't happen)\n";
	    } else {
		pp_warn("unknown SENSE map code == $code");
	    }
	}
    }
}

sub senses_uniq {
    my @c = @_;
    my %seen = ();
    for (my $i = 0; $i <= $#c; ++$i) {
	if ($c[$i] =~ /^[-+>]?\@entry/) {
	    %seen = ();
	} elsif ($c[$i] =~ /^\@sense/) {
	    my $s = $c[$i];
	    $s =~ s/\s+/ /g;
	    if ($seen{$s}++) {
		$c[$i] = "\000";
	    }
	}
    }
    @c;
}

sub senses_init {
    my $args = shift;
    my $senses_outfile = $$args{'lang'}.'.map';
    if (-d '01tmp') {
	$senses_outfile = "01tmp/$senses_outfile";
    }
    open(MAP_FH, ">>$senses_outfile");
    $map_fh = \*MAP_FH;
}

sub senses_string {
    join ("\n\t", @{$_[0]});
}

sub senses_term {
    close(MAP_FH);
}

sub index_senses {
    my %t = ();
    for (my $i = 0; $i <= $#_; ++$i) {
	my $s = $_[$i];
	next if $s =~ /^-\@sense/;
	$s =~ s/^\+?\@sense\S*\s+//;
	$s =~ s/\#\S+\s+//; # remove sense-id
	$s =~ s/^\S+\s+//; # and POS
	$s =~ tr/a-zA-Z0-9 //cd; # reduce to alphanumerics
	$s =~ "\L$s"; # reduce to lowercase
	foreach my $t (split(/\s/,$s)) {
	    push @{$t{$t}}, $i; # register token $t as occurring in sense $i
	}
    }
    %t = de_common(%t);
#    print Dumper \%t;
    %t;
}

sub de_common_list {
    my %c = (); @c{@_} = ();
    %c = de_common(%c);
    return keys %c;
}

sub de_common {
    my %t = @_;
    my %n = ();
    foreach my $t (keys %t) {
	$n{$t} = $t{$t} unless exists($common1{$t}) || exists($common2{$t});
    }
    if (scalar(keys(%n)) == 0) {
	foreach my $t (keys %t) {
	    $n{$t} = $t{$t} unless exists($common1{$t});
	}	
    }
    if (scalar(keys(%n)) == 0) {
	%t;
    } else {
	%n;
    }
}

sub senses_find {
    my @f = ();
    foreach (@_) {
	push @f, $_ if /^\@sense/;
    }
    @f;
}

sub senses_merge_2 {
    my ($b_e,$i_e,$no_sense_plus) = @_;
    my @bb = senses_find(@$b_e);
    my @ii = senses_find(@$i_e);
    my $b = [ @bb ];
    my $i = [ @ii ];
    my @newb = ();
    if ($b) {
	@newb = @$b;

	my $senses_b_str = senses_string($b);
	my $senses_i_str = senses_string($i);
	warn "aligning:\n\t$senses_i_str\ninto\t$senses_b_str\n" 
	    if $ORACC::CBD::PPWarn::trace;

	my %b = index_senses(@$b);
	#    print Dumper \%b;
	foreach my $s (@$i) {
	    next if $s =~ /^-/;
	    my @matches = ();
	    my $s_no_pos = $s; $s_no_pos =~ s/^\@sense\S*\s+\S+//;		
	    # does s occur in @b?
	    foreach my $b (@$b) {
		my $b_no_pos = $b; $b_no_pos =~ s/^\@sense\S*\s+\S+//;
		# warn "testing b_no_pos $b_no_pos vs s_no_pos $s_no_pos\n";
		if ($b_no_pos =~ /\Q$s_no_pos/ || $s_no_pos =~ /\Q$b_no_pos/) {
		    push @matches, $b;
		}
	    }
	    if ($#matches >= 0) {
		warn "Senses[1] $s == $matches[0]\n" if $ORACC::CBD::PPWarn::trace;
	    } else {
		$s =~ s/sense/sense+/ unless $no_sense_plus;
		warn "Senses[2] ADD $s\n" if $ORACC::CBD::PPWarn::trace;
		push @newb, $s;
	    }	
	}
    } else {
	warn "$0: internal error in senses_merge_2: received undefined bases senses array\n";
    }
    @newb;
}

1;
