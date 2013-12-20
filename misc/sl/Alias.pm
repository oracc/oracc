package ORACC::SL::Alias;
use warnings; use strict; use utf8; use open 'utf8';

my %a = ();

sub
alias {
    $a{$_[0]} || $_[0];
}

sub
alias_words {
    return '' unless defined($_[0]) && length($_[0]);
    my $x = shift;
    my @tmp = split(/([-{}\s])/, $x);
    @tmp = map({ alias($_) } @tmp);
    my $res = join('', @tmp);
    $res;
}

sub
init {
    open(A, "$ENV{'ORACC'}/pub/ogsl/alias-dumb.txt") || die "Alias.pm: no aliases.dumb\n";
    while (<A>) {
	chomp;
	my @a = split(/\s+/, $_);
	my $head = shift @a;
	foreach (@a) {
	    $a{$_} = $head;
	}
    }
    close(A);
}

1;
