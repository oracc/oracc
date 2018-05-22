package ORACC::CBD::Words;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/words_check/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::NS;
use ORACC::CBD::Hash;
use ORACC::CBD::PPWarn;
use String::Similarity;
use String::Similarity::Group qw/groups groups_hard/;

my @common = qw/a and be is of or the to/;
my %common = (); @common{@common} = ();

use Data::Dumper;

my %warned = ();
my @cfgws = ();
my %g = ();

sub words_check {
    my ($args) = @_;
    my $cbdname = "$$args{'project'}:$$args{'lang'}";
    my $hash = $ORACC::CBD::data{$cbdname};
    if (!$hash) {
	if (pp_hash($args)) {
	    $hash = \%{$ORACC::CBD::data{$cbdname}};
	}
    }
    return undef unless $hash;

    %g = %$hash;
#    open(M, '>m.log');
    check_by_base();
#    close(M);
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
    my @g = groups_hard(0.5, [ keys %t ]);
    if ($#g < 0) {
	return (); # none of the tlits in our set grouped together
    }

#    print "tlits: ", Dumper \%t;
#    print "groups: ", Dumper \@g;

    my %i = ();
    foreach my $g (@g) {
	foreach my $t (@$g) {
	    ++$i{$t};
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
#	$gw =~ tr/a-zA-Z0-9 //cd;
	++$t{"$gw"};
	foreach my $s (@{$e{'sense'}}) {
	    $s =~ s/^\S+\s+\S+\s+//; # remove sense-id and POS
	    $s =~ tr/a-zA-Z0-9 //cd;
	    foreach my $t (split(/\s/,$s)) {
		++$t{"$t"};
	    }
	}
	foreach my $t (keys %t) {
	    push (@{$m{$t}}, [ @$eb ])
		unless exists $common{$t};
	}
    }

#    print M "m= ",Dumper \%m;
    
    # now group the words by similarity
    my @w = keys %m;
    my @g = groups_hard(0.75,\@w);
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

1;
