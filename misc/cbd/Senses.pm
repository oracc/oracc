package ORACC::CBD::Senses;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/senses_align senses_collect senses_init senses_term senses_merge senses_string/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use Data::Dumper;

my @common1 = qw/a an in is of or the to/;
my %common1 = (); @common1{@common1} = ();

my @common2 = qw/and be by designation down for form make off on out part status type up/;
my %common2 = (); @common2{@common2} = ();

my %sense_lnum = ();

my $map_fh = undef;

sub senses_align {
    my($args,$base_cbd,$cbd,$xmap_fh) = @_;

    my @base_cbd = @$base_cbd;
    my @cbd = @$cbd;
    $map_fh = $xmap_fh if $xmap_fh;

    my @senses = ();
    my %base_senses = senses_collect(@base_cbd);
    my $curr_entry = '';
    my $last_sense = 0;

    for (my $i = 0; $i <= $#cbd; ++$i) {
	if ($cbd[$i] =~ /^\@entry\S*\s+(.*?)\s*$/) {
	    $curr_entry = $1;
	} elsif ($cbd[$i] =~ /^\@sense/) {
	    $cbd[$i] =~ s/\s+/ /g;
	    $cbd[$i] =~ s/\s*$//;
	    push @senses, $cbd[$i];
	    my $ss = $cbd[$i]; $ss =~ s/^\@sense\s+//;
	    $sense_lnum{$ss} = $i+1;
	} elsif ($cbd[$i] =~ /^\@end\s+entry/) {
	    if ($#senses >= 0) {
		my $senses_b = $base_senses{$curr_entry};
		if ($senses_b) {
		    my $senses_b_str = senses_string($senses_b);
		    my $senses_i_str = senses_string([ @senses ]);
		    warn "aligning:\n\t$senses_i_str\ninto\t$senses_b_str\n" 
			if $ORACC::CBD::PPWarn::trace;
#		    my $nsenses = 
		    senses_merge($args, 
				 $curr_entry, 
				 [ @senses ], $senses_b);
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
}

sub senses_collect {
    my @cbd = @_;
    my %s = ();
    my $curr_entry = '';
    my $last_sense = 0;
    my @s = ();
    for (my $i = 0; $i <= $#cbd; ++$i) {
	if ($cbd[$i] =~ /^\@entry\S*\s+(.*?)\s*$/) {
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
    %s;
}

sub senses_merge {
    my($args,$entry,$i,$b) = @_;
    my %map = ();
    my @newb = ();
    my %b = index_senses(@$b);
#    print Dumper \%b;
    foreach my $s (@$i) {
	my @matches = ();
	# does s occur in @b?
	foreach my $b (@$b) {
	    if ($b =~ /\Q$s/ || $s =~ /\Q$b/) {
		push @matches, $b;
	    }
	}
	if ($#matches == 0) {
	    map_sense($args, '1', $entry, $s, $matches[0]);
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
		my $index = $m[0]; $index =~ s/^#//;
		map_sense($args, '2', $entry, $s, ${$b}[$index]);
	    } else {
		$s =~ s/sense/sense+/;
		warn "Senses[3]: adding $s\n" if $ORACC::CBD::PPWarn::trace;
		add_sense($args, $entry, $s);
		push @newb, $s;
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
	$from_sig =~ s#](\S+)#//$sense]$1'$epos#;
	$to_sig =~ s#](\S+)#//$sense_b]$1'$epos_b#;
	warn "Senses[$code]: mapping $from_sig => base $to_sig\n" if $ORACC::CBD::PPWarn::trace;
	# print MAP_FH '@'.project($$args{'cbd'}).'%'.lang().":$from_sig => $to_sig\n";
	print $map_fh "map sense $from_sig => $to_sig\n";
    }
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
	$s =~ s/^\@sense\S*\s+//;
	$s =~ s/\#\S+\s+//; # remove sense-id
	$s =~ s/^\S+\s+//; # and POS
	$s =~ tr/a-zA-Z0-9 //cd; # reduce to alphanumerics
	foreach my $t (split(/\s/,$s)) {
	    push @{$t{$t}}, $i; # register token $t as occurring in sense $i
	}
    }
    %t = de_common(%t);
#    print Dumper \%t;
    %t;
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

1;
