package ORACC::SE::Evaluator;
BEGIN { push @INC, 
	'/usr/local/share/epsd/lib',
	'/usr/local/share/cdl/tools',
    }
use warnings; use strict; use utf8; use open ':utf8';
use ORACC::SE::DBM;
use ORACC::ATF::Unicode;

my @indexes = ();
my @pattern = ();

my $utf8_flag = 0;

my $gtoken = '[-.^$~_\"\cA\cB]|[a-z]+[:#]';
my $fuzzy = 1;
my $detpat = '(?:\{[^}]+\})';
# \cA and \cB are null breakpoints inserted before and after optional keys
# typed in curly brackets, i.e., {gec}gu-za => \cA{gec}\cBgu-za
my %re_of = (
	     "\cA"=>'\{',
	     "\cB"=>'\}',
	     '^'=>"^$detpat*",
	     '$'=>"(?:$detpat+\$|\$)",
	     '-'=>"(?:$detpat*[-.]$detpat*)",
	     '.'=>"(?:$detpat*[-.]$detpat*)",
	     '_'=>"(?:^|$|\\s|$detpat+\\s*)",
	     '~'=>'(?:^|$|[-.\\s])',
	     );

#"(?:-|$detpat+-?)",

my $nonterms = 0;

sub
init {
    my (@indexnames) = @_;
    foreach my $i (@indexnames) {
	push @indexes, ORACC::SE::DBM::load($i);
    }
}
sub
term {
    foreach my $ix (@indexes) {
	ORACC::SE::DBM::unload($ix);
    }
}

sub
lookup {
    my ($term,$uflag) = @_;
    my @res = ();
    my $rtotal = 0;
    $nonterms = 0;
    $fuzzy = 1;
    $utf8_flag = $uflag if defined $uflag;

    foreach my $ix (@indexes) {
	$$term{'expanded'} = expand($ix,$$term{'parsed'});
#	main::tdump($term,$$ix{'#title'});
	my $res = exec_search($ix,$$term{'expanded'});
	my @su = sortuniq($$res[1]);
	$rtotal += $#su+1;
	push @res, [$ix, @su];
    }
    ($rtotal,@res);
}

sub
mlookup {
    my ($query,$uflag) = @_;
    my @res = ();
    my $rtotal = 0;
    $nonterms = 0;
    $fuzzy = 1;
    $utf8_flag = $uflag if defined $uflag;

    foreach my $ix (@indexes) {
	my $term = ORACC::SE::Compiler::parse($query);
	$$term{'expanded'} = expand($ix,$$term{'parsed'});
#	main::tdump($term,$$ix{'#title'});
	my $res = exec_search($ix,$$term{'expanded'});
	my @su = sortuniq($$res[1]);
	$rtotal += $#su+1;
	push @res, [$ix, @su];
    }
    ($rtotal,@res);
}

sub
expand {
    my ($index,@terms) = @_;
    foreach my $t (@terms) {
	my @l = @$t;
	my $name = shift @l;
	if ($name eq 'group') {
	    expand($index,@l);
	} elsif ($name eq 'term') {
	    push @$t, expand_term($index,$l[0]);
	}
    }
    $terms[0];
}

# Set the field and type from the first element; any 
# field/type after that has to be identical or it's a bad
# search
sub
expand_term {
    my ($index,$raw) = @_;
    my @ret = ();
    my @regexp = ();
    my $field = undef;
    my $type = undef;
    my $needs_re = 0;
    my $fewest_records = undef;
    my $lowest_freq = 0xffffffff;

    foreach my $tok (grep length, split(/($gtoken)/o, $raw)) {
	if ($tok eq '"') {
	    $fuzzy = !$fuzzy;
#	} elsif ($tok =~ /^[\cA\cB]$/) {
	} elsif ($tok =~ /^[-.~_^\$\cA\cB]$/) {
	    push @ret, [ 'join', $tok ];
	    if ($re_of{$tok}) {
		push @regexp, $re_of{$tok};
	    } else {
		push @regexp, $tok;
	    }
	    ++$needs_re;
	} elsif ($tok eq '?') {
	    my $xt = pop @ret;
	    push @regexp, '(?:', $xt, ')?';
	} elsif ($tok =~ /\#$/) {
	    if (defined $type) {
		bad_search("type mismatch--'$tok' is not initial type\n")
		    unless $type eq $tok;
	    } else {
		$type = $tok;
	    }
	} elsif ($tok =~ /:$/) {
	    if (defined $field) {
		bad_search("field mismatch--'$tok' is not initial field\n")
		    unless $field eq $tok;
	    } else {
		$field = $tok;
		$field =~ s/:$//;
	    }
	} else {
	    $field = $$index{'#default_field'} unless defined $field;
	    $type = '' unless defined $type;
#	    if (!$utf8_flag && $$index{'#atf_fields'} =~ /:${field}:/) {
#		my $atok = ORACC::ATF::Unicode::gconv($tok);
#		if ($field eq $$index{'#default_field'} && $atok ne $tok) {
#		    $tok = "(?:$tok|$atok)";
#		} else {
#		    $tok = $atok;
#		}
#	    }

	    my $det = $tok =~ tr/{}//d;
	    print STDERR "det\n" if $det;

	    my @w = ORACC::SE::DBM::wild($index,$field,$tok);
	    my $pat = shift @w;
	    my @trylist = ();
	    if ($fuzzy) {
		my %f = ();
		foreach my $w (@w) {
		    next unless $w;
		    @f{split(/\s+/, 
			     ORACC::SE::DBM::fuzzy($index,$field,$w)
			     || '')}
		    = ();
		}
		@trylist = sort keys %f;
	    } else {
		@trylist = @w;
	    }
	    my $freq = ORACC::SE::DBM::freq($index,$field,@trylist) || 0;
	    my @recids = ORACC::SE::DBM::records_list($index,$field,@trylist);

	    if ($freq < $lowest_freq) {
		$lowest_freq = $freq;
		$fewest_records = [ @recids ];
	    }

	    push @ret, [ 'key', $pat, $freq,
			 [ @trylist ],
			 [ @recids ] ];
	    push (@regexp, '\{?') unless $det;
	    if ($#trylist > 0) {
		push @regexp, '(?:', join('|',
					  map { s/^.*?://; $_ } 
					  grep(defined, @trylist)),')';
	    } elsif ($#trylist == 0) {
		push @regexp, @trylist;
	    } else {
		push @regexp, $pat;
	    }
	    push (@regexp, '\}?') unless $det;
	}
    }
    my $re = '';
    if ($#regexp >= 0 && $needs_re) {
	for (my $i = 0; $i <= $#regexp; ++$i) {
	    if ($regexp[$i] =~ /\\\{(.*?)\}\\/) {
		my $add = "(?>$1)";
		$regexp[$i-1] =~ s/\{/\{$add/;
	    }
	}
	unshift @regexp, '(?:^|[-\s])'."$detpat*" unless $regexp[0] =~ /\^/;
	push @regexp, "$detpat*".'(?:[-\s]|$)' unless $regexp[$#regexp] =~ /\$/;
	$re = join('',@regexp);
	unshift @ret, $re;
	unshift @ret, 'yes';
    } else {
	unshift @ret, '';
	unshift @ret, 'no';
    }
    my $recids_ref;
    if ($needs_re) {
	$recids_ref = [ term_grep($index, $field, $re, $fewest_records) ];
    } else {
	$recids_ref = $fewest_records;
    }
    unshift @ret, [ $lowest_freq , $field, $type , $recids_ref ];
    @ret;
}

sub
term_grep {
    my($ix,$fld,$re,$recref) = @_;
    my @ret = ();
    print STDERR "term_grep: undefined field (re=$re)\n" unless defined $fld;
    print STDERR "term_trep: undefined regex\n" unless defined $re;
    foreach my $r (@$recref) {
	print STDERR "term_grep: undefined record\n" unless defined $r;
	my $rr = $r;
	$r =~ s/[si].*$//;
#	print STDERR "$r = ",$$ix{"\#${fld}:$r"}, "\n";
	my $target = $$ix{"#${fld}:$r"};
	print STDERR "term_grep: key '\#${fld}:$r' yields undefined value\n"
	    unless $target;
	if ($target && $target =~ /$re/) {
	    push @ret, $rr;
	}
    }
    @ret;
}

# When this is called the hard work of resolving terms has been done; we just
# walk the groups AND/ORing the term results together.
sub
exec_search {
    my ($index,@terms) = @_;
    my @stack = ();
    foreach my $t (@terms) {
	my @l = @$t;
	my $name = shift @l;
	if ($name eq 'group') {
	    push @stack, exec_search($index,@l);
	} elsif ($name eq 'term') {
	    my $info = $l[1];
	    push @stack, [ $$info[1], $$info[3] ];
	} elsif ($name eq 'op') {
            # if the fields of the two args are different, 
	    # call 'with_func' instead of 'and_func'
	    my $op = (shift(@l).'_func');
	    if ($op eq 'and_func' && ${$stack[0]}[0] ne ${$stack[1]}[0]) {
		$op = 'with_func';
	    }
	    {
		no strict;
		my $s1 = shift @stack;
		my $s2 = shift @stack;
		push @stack, &$op($s1, $s2); 
	    }
	} else {
	    die "exec_search: unhandled term '$name'\n";
	}
    }
    if ($#stack > 0) {
	warn "exec_search: stack contains more than one result\n";
	undef;
    } elsif ($#stack < 0) {
	warn "exec_search: stack contains no results\n";
	undef;
    } else {
	$stack[0];
    }
}

# and the argument record lists, but require item IDs in list2
# to be >= the ID in list1
sub
and_func {
    my ($l1,$l2) = @_;
    my %l1;
    my %l2;
    my %ret = ();
    my @ret = ();
    decode_rec_and(\%l1,$$l1[1]);
    decode_rec_and(\%l2,$$l2[1]);
    foreach my $r (keys %l1) {
	if ($l2{$r}) {
	    my $i1 = ${$l1{$r}}[0];
	    foreach my $i2 (@{$l2{$r}}) {
		if ($i2 > $i1) {
		    my $found = $r.'i'.$i2;
		    push @ret, $found unless $ret{$found}++;
		    last;
		}
	    }
	}
    }
    [ $$l1[0], [ @ret ] ];
}

# or the argument lists
sub
or_func {
    my ($l1,$l2) = @_;
    my %ret = ();
    @ret{@{$$l1[1]}} = ();
    @ret{@{$$l2[1]}} = ();
    [ $$l1[0], [ sort keys %ret ] ];
}

# and the argument record lists disregarding item IDs
sub
with_func {
    my ($l1,$l2) = @_;
    my %l1;
    my %l2;
    my %ret = ();
    my @ret = ();
    decode_rec_with(\%l1,$$l1[1]);
    decode_rec_with(\%l2,$$l2[1]);
    foreach my $r (keys %l1) {
	if (defined $l2{$r}) {
	    my $found = $r.${$l1{$r}}[0];
	    push @ret, $found unless $ret{$found}++;
	}
    }
    [ $$l1[0], [ @ret ] ];
}

sub
decode_rec_and {
    my($href,$rref) = @_;
    foreach my $r (@$rref) {
	my($r,$i) = ($r =~ /^(.*?)i(.*)?$/);
	push @{$$href{$r}}, $i || '';
    }
}

sub
decode_rec_with {
    my($href,$rref) = @_;
    foreach my $r (@$rref) {
	my($r,$i) = ($r =~ /^(.*?)([usi].*)?$/);
	push @{$$href{$r}}, $i || '';
    }
}

sub
sortuniq {
    my %x = ();
    @x{ map { s/[usi].*$//; $_ } @{$_[0]} } = ();
    sort keys %x;
}

1;
