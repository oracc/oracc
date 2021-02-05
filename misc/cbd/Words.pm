package ORACC::CBD::Words;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/words_check words_init words_term words_signs words_anagrams/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::NS;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Hash;
use String::Similarity;
use String::Similarity::Group qw/groups groups_hard/;

my @common1 = qw/a an in is of or the to/;
my %common1 = (); @common1{@common1} = ();

my @common2 = qw/and be by designation down for form make off on out part status type up/;
my %common2 = (); @common2{@common2} = ();

use Data::Dumper;

my $threshold = 0.94;

my $verbose = 0;

my %warned = ();
my @cfgws = ();
my %g = ();

sub words_init {
    ORACC::SL::BaseC::init();
}
sub words_term {
    ORACC::SL::BaseC::term();
}

sub words_signs {
    
}

sub words_anagrams {
    
}

sub words_check {
    my ($args) = @_;
    if ($$args{'words'}) {
	$threshold = $$args{'words'};
    }
    my $hash = undef;
    my $cbdname = ORACC::CBD::Util::cbdname();
    if (!$cbdname || !($hash = $ORACC::CBD::data{$cbdname})) {
	if (!$hash) {
	    if (pp_hash($args)) {
		$cbdname = ORACC::CBD::Util::cbdname();
		$hash = \%{$ORACC::CBD::data{$cbdname}};
	    }
	}
    }
    return undef unless $hash;
    warn "Checking words in $cbdname\n";
    %g = %$hash;
    if ($verbose) {
	open(G,'>g.dump');
	print G Dumper \%g;
	close(G);
	warn "Beginning check_by_groups ...\n";
    }
    check_by_groups();
    warn "Beginning check_by_base ...\n" if $verbose;
    check_by_base();
}

sub check_by_groups {
    my @ids = @{$g{'ids'}};
    my @cfgws = map { ${$g{'entries'}}{$_}  } @ids;
    my @groups = groups_hard($threshold, \@cfgws);
    foreach my $g (@groups) {
	my @m = @$g;
	if ($#m > 0) {
	    my $h = shift @m;
	    foreach my $m (@m) {
		my $l = line_of($m,'entry');
		if ($l) {
		    pp_line($l);
		    pp_warn("(words) $m looks like $h");
		}
	    }
	}
    }
}

sub line_of {
    my($what,$tag) = @_;
    my %e = %{$g{'entries'}};
    my $id = $e{$what};
    my $l = ${$e{$id,'l'}}{$tag};
    if (defined $l) {
	return $l + 1;
    } else {
	warn "strange: no line number for $tag in $what\n";
	return 0;
    }
}

sub check_by_base {
    foreach my $b (keys %{$g{'basedata'}}) {
	my @e = @{${$g{'basedata'}}{$b}};
	if ($#e > 0) {
	    my %m = index_meanings(@e);
	    foreach my $m (keys %m) {
		my @m = ();
		foreach my $ib (@{$m{$m}}) {
		    push @m, $ib unless ${$g{'ok'}}{$$ib[0]};
		}
		if ($#m > 0) {
		    my $nwarned = 0;
		    foreach my $ib (@m) {
			if ($warned{$$ib[0]}++) {
			    ++$nwarned;
			}
		    }
		    next if $nwarned > $#m;
		    # Now we have similar words by base/meaning we can filter out
		    # multiples where the transliteration of the bases are not similar
		    @m = filter_by_tlit(@m);
		    next unless $#m > 0;

		    # Issue diagnostics about any multiples that are left
		    my @cfs = map { ${$g{'entries'}}{$$_[0]} } @m;
		    my $cfstr = join('; ', @cfs);
		    my $first_ib = $m[0];
		    my $l = ${${$g{'entries'}}{$$first_ib[0],'l'}}{'entry'};
		    pp_line($l+1);
		    pp_warn("(words) '$m' is in gw/sense of $cfstr");
		}
	    }
	}
    }
}

sub filter_by_tlit {
    my @m = @_;
    my %t = ();
    foreach my $m (@m) {
	push @{$t{$$m[1]}}, $$m[0];
    }
    if (scalar keys %t == 1) {
	return @m; # there is only one distinct tlit for the bases in our set
    }
    my %nodet = ();
    foreach my $t (keys %t) {
	my $nd = $t;
	1 while $nd =~ s/\{.*?\}//;
	if ($nd) {
	    push @{$nodet{$nd}}, $t;
	} else {
	    push @{$nodet{$t}}, $t;
	}
    }
    
    #    my @g = groups_hard(0.5, [ keys %t ]);
    my @g = groups_hard(0.5, [ keys %nodet ]);
    if ($#g < 0) {
	return (); # none of the tlits in our set grouped together
    }

#    print "tlits: ", Dumper \%t;
#    print "groups: ", Dumper \@g;

    my %i = ();
    foreach my $g (@g) {
	foreach my $nd (@$g) {
	    my @t = @{$nodet{$nd}};
	    foreach my $t (@t) {
		++$i{$t};
	    }
	}
    }
#    print "index: ", Dumper \%i;
    my @new_m = ();
    foreach my $t (%t) {
	if ($i{$t}) {
	    foreach my $m (@{$t{$t}}) {
		push @new_m, [ $m, $t ];
	    }
	}
    }
#    print "new_m: ", Dumper @new_m;
    @new_m;
}

sub index_meanings {
    my %m = ();
    # first collect all the words in the gw/senses
    foreach my $eb (@_) {
	my $eid = $$eb[0];
	my $c = ${$g{'entries'}}{$eid};	
	my %e = %{${$g{'entries'}}{$eid,'e'}};
	my %t = (); # index of tokens in this entry
	my($gw) = ($c =~ /\[(.*?)\]/);
	$gw =~ tr/a-zA-Z0-9 //cd;
	foreach my $bit (split(/\s/, $gw)) {
	    ++$t{$bit};
	}
	foreach my $s (@{$e{'sense'}}) {
	    $s =~ s/^\S+\s+\S+\s+//; # remove sense-id and POS
	    $s =~ tr/a-zA-Z0-9 //cd;
	    foreach my $t (split(/\s/,$s)) {
		++$t{"$t"};
	    }
	}
	%t = de_common(%t);
	foreach my $t (keys %t) {
	    push (@{$m{$t}}, [ @$eb ]);
	}
    }

#    print M "m= ",Dumper \%m;
    
    # now group the words by similarity
    my @w = keys %m;
    my @g = groups_hard(0.8,\@w);
#    print M "g_hard= ", Dumper \@g;

    # and collect together the cfgws for any grouped words
    my %new_m = ();
    foreach my $g (@g) {
#	print M "m-in-group-loop pre g \[@$g\]= ",Dumper \%m;
	my @gw = @$g;
	my $head = join('/',@gw);
	my @ids = ();
	foreach my $w (@gw) {
	    foreach my $x (@{$m{$w}}) {
		push @ids, [ @$x ];
	    }
	    delete $m{$w};
	}
#	print M "m-in-group-loop after g= ",Dumper \%m;
#	print M "ids-in-group-loop= ",Dumper \@ids;
	my %u = ();
	foreach my $i (@ids) {
	    ++$u{"$$i[0]\:$$i[1]"};
	}
#	print M "u-in-group-loop= ",Dumper \%u;
	my %i = ();
	foreach my $u (keys %u) {
	    my($id,$tlit) = ($u =~ /^(.*?):(.*?)$/);
	    push @{$i{$id}}, $tlit;
	}
	my @newids = ();
	foreach my $i (sort keys %i) {
	    # for each of the tlits belonging to this ID
	    foreach my $t (@{$i{$i}}) {
		push @newids, [ $i, $t ];
	    }
	}
	@{$new_m{$head}} = @newids;
    }
    foreach my $n (keys %new_m) {
	$m{$n} = $new_m{$n};
    }	

 #   print M "m-after-g= ",Dumper \%m;

    %m;
}

sub de_common {
    my %t = @_;
    my %n = ();
    foreach my $t (keys %t) {
	++$n{$t} unless exists($common1{$t}) || exists($common2{$t});
    }
    if (scalar(keys(%n)) == 0) {
	foreach my $t (keys %t) {
	    ++$n{$t} unless exists($common1{$t});
	}	
    }
    if (scalar(keys(%n)) == 0) {
	%t;
    } else {
	%n;
    }
}

1;
