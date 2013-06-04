package ORACC::SE::Compiler;
use warnings; use strict; use utf8;

my $token = '(?:\s+(?:and|or|with)\s+)|[() ]|'
    . "[-\\\"\\.~_:\\\$^\\#a-zA-Z0-9\\&\\*%\\@\\+\\{\\}\x80-\xff\x{100}-\x{fff0}]+";
my $fuzzy = 1;
my @toks = ();
my $bad_search = 0;
my $bad_search_error = '';

sub
parse {
    my $term = shift;
    $term =~ s/\s+/ /g; $term =~ s/^\s*//; $term =~ s/\s*$//;
    my %term = ();
    $term{'string'} = $term;
    # The second sub in the map makes ' ' to default to 'with'
    @toks 
	= map { s/^\s+(and|or|with)\s+$/$1/ || s/ /with/; $_ } 
          grep length, split(/($token)/o,$term);
    @{$term{'tokens'}} = join('|',@toks);
    $fuzzy = 1;
    $bad_search = 0;
    $bad_search_error = '';
    $term{'parsed'} = compile(@toks);
    if ($bad_search) {
	$term{'bad'} = $bad_search_error;
    }
    \%term;
}

sub
compile {
    my @c = ();
    while ($#toks >= 0) {
	my $t = shift @toks;
	if ($t eq '(') {
	    push @c, compile(@toks);
	    last if $bad_search;
	} elsif ($t eq ')') {
	    last;
	} elsif ($t =~ /^(?:and|or|with)$/i) {
	    push @c, [ 'op', $t ];
	} else {
	    if ($t =~ /\{.*?\}/) {
		$t =~ s/\{\{(.*?)\}\}/\{$1\}/;
		my $t2 = $t;
		$t =~ tr/{}/\cA\cB/;
		$t2 =~ s/\{(.*?)\}/~$1~/g;
		$t2 =~ s/^(\^?)~/$1/;
		$t2 =~ s/~(\$?)$/$1/;
		unshift @toks, '(', $t, 'or', $t2, ')';
	    } else {
		if ($t =~ /\cA/) {
		    $t =~ s/\cA/\cA\{/g;
		    $t =~ s/\cB/\}\cB/g;
		}
		push @c, [ 'term', $t ];
	    }
	}
    }
    my @newc = ();
    for (my $i = 0; $i <= $#c; ++$i) {
	my $type = ${$c[$i]}[0];
	if ($type eq 'op') {
	    if ($i < $#c) {
		my $next = $i+1;
		my $next_type = ${$c[$next]}[0];
		if ($next_type eq 'group' || $next_type eq 'term') {
		    if ($next == $#c) {
			push @newc, $c[$next], $c[$i];
		    } else {
			push @newc, [ 'group', pop @newc, $c[$next], $c[$i] ];
		    }
		    ++$i;
		} else {
		    bad_search("double boolean");
		    last;
		}
	    } else {
		bad_search("boolean must have two arguments");
		last;
	    }
	} elsif ($type eq 'group' || $type eq 'term') {
	    push @newc, $c[$i];
	} else {
	    bad_search("unknown argument type '$type'");
	    last;
	}
    }
    [ 'group', @newc ];
}

sub
bad_search {
    $bad_search_error = shift;
    print STDERR "ORACC::SE::Compiler: ", $bad_search_error, "\n";
    $bad_search = 1;
}

1;
