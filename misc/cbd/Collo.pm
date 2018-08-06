package ORACC::CBD::Collo;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/pp_collo/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use Data::Dumper;

use constant C_BAD   =>-1;
use constant C_HYPH  => 1;
use constant C_FORM  => 2;
use constant C_POS   => 3;
use constant C_SENSE => 4;
use constant C_SIG   => 5;

sub pp_collo {
    my ($args,$f,@cbd) = @_;
    my $ndir = projdir()."/02pub";
    system 'mkdir', '-p', $ndir;
    open(COLLO, ">$ndir/coll-$$args{'lang'}.ngm");
    foreach my $i (@{$ORACC::CBD::data{'collo'}}) {
	my $p = c_parent($i,@cbd);
	my $e = c_expand($i,@cbd);
	print COLLO $e, "\n";
	$cbd[$i] = "\000";
    }
    close(COLLO);
}

sub c_parent {
    my($i,@cbd) = @_;
    my $e = pp_entry_of($i,@cbd);
    my $c = $cbd[$e];
    $c =~ s/^\S+\s+//;
    $c =~ s/\].*$/\]/;
    $c =~ s/\s+\[/\[/;
    $c;
}

sub c_expand {
    my($i,@cbd) = @_;
    my @t = c_tokenize($cbd[$i]);
#    print Dumper \@t;
    my @l = ();
    my @r = ();
    foreach my $t (@t) {
	if ($$t[0] == C_HYPH) {
	    my $p = c_parent($i,@cbd);
	    push @l, $p;
	} elsif ($$t[0] == C_FORM) {
	    my $p = c_parent($i,@cbd);
	    push @l, "$$t[1]=$p";
	} elsif ($$t[0] == C_POS) {
	    push @l, $$t[1];
	} elsif ($$t[0] == C_SENSE) {
	    my $p = c_parent($i,@cbd);
	    $p =~ s#\]#//$$t[1]\]#;
	    push @l, $p;
#	    my $r = $p;
#	    $r =~ s#\]#//$$t[1]\]#;
#	    $r[$$t[2]] = $r;
	} elsif ($$t[0] == C_SIG) {
	    push @l, $$t[1];
	} elsif ($$t[0] == C_BAD) {
	} else {
	    warn pp_file().':'.pp_line().
		": internal error: unhandled token type $$t[0]\n";
	}
    }
#    print Dumper \@l;
    my $e = join(' ', @l);
#    if ($#r >= 0) {
#	for (my $i = 0; $i <= $#l; ++$i) {
#	    $r[$i] = '*' unless $r[$i];
#	}
#	$e .= ' => ';
#	$e .= join(' ',@r);
#    }
    $e;
}

sub c_tokenize {
    my $c = shift;
    $c =~ s/\@collo\s+//;
    my @t = ();
    my $in = $c;
    $c .= ' '; # make regex's easier
    my $i = 0;
    while ($c) {
	if ($c =~ s/^-\s+//) {
	    push @t, [ C_HYPH, '-', $i ];
	} elsif ($c =~ s/^\[(.*?)\]\s+//) {
	    push @t, [ C_SENSE, $1, $i ];
	} elsif ($c =~ s/([A-Z][A-Z]*)\s+//) {
	    push @t, [ C_POS, $1, $i ];
	} elsif ($c =~ s/^([^\s]+?\].*?)\s+//) {
	    push @t, [ C_SIG, $1, $i ];
	} elsif ($c =~ s/,(\S+)\s+//) {
	    push @t, [ C_FORM, $1, $i ];
	} else {
	    @t = ();
	    push @t, [ C_BAD, $c, $i ];
	    last;
	}
	++$i;
    }
    @t;
}

1;
