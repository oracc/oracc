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
use constant C_GOESTO=> 6;
use constant C_STAR  => 7;

sub pp_collo {
    my ($args,$f,@cbd) = @_;
    my $ndir = projdir()."/02pub";
    system 'mkdir', '-p', $ndir;
    open(COLLO, ">$ndir/coll-$$args{'lang'}.ngm");
    open(CLOG,">01tmp/collo.log");
    my %collo = ();
    foreach my $i (@{$ORACC::CBD::data{'collo'}}) {
	my $e = c_expand($i,@cbd);
	next unless $e;
	++$collo{$e};
	my $er = $i+1;
	print CLOG pp_file().":$er: $cbd[$i] >> $e\n";
	$cbd[$i] = "\000";
    }
    foreach my $c (sort keys %collo) {
	print COLLO $c, "\n";
    }
    close(COLLO);
    close(CLOG);
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
    if ($#t == 0 && ${$t[0]}[0] eq '-') {
	pp_line($i+1);
	pp_warn("\@collo with '-' must have more than one token");
	return "";
    }
#    print Dumper \@t;
    my @l = ();
    my @r = ();
    my $r_mode = 0;
    foreach my $t (@t) {
	if ($r_mode) {
	    if ($$t[1] eq '-') {
		# warn "Collo: replacing '-' with $l[$r_mode-1]\n";
		push @r, $l[$r_mode-1];
	    } else {
		push @r, $$t[1];
	    }
	    ++$r_mode;
	} else {
	    if ($$t[0] == C_HYPH) {
		my $p = c_parent($i,@cbd);
		push @l, $p;
		push @r, '*';
	    } elsif ($$t[0] == C_FORM) {
		my $p = c_parent($i,@cbd);
		push @l, ":$$t[1]=$p";
		push @r, '*';
	    } elsif ($$t[0] == C_POS) {
		push @l, $$t[1];
		push @r, '*';
	    } elsif ($$t[0] == C_SENSE) {
		my $p = c_parent($i,@cbd);
		push @l, $p;
		$p =~ s#\]#//$$t[1]\]#;
		push @r, $p;
	    } elsif ($$t[0] == C_SIG) {
		if ($$t[1] =~ /^-(.+?)([=\[].*)$/) {
		    my($f,$s) = ($1,$2);
		    if ($s =~ /^\[/) {
			# get the CF from parent
			my $p = c_parent($i,@cbd);
			$p =~ s/\s*\[.*//;
			$s = "$p$s";
		    } else {
			$s =~ s/^=//;
			if ($s =~ /^\[/) {
			    pp_line($i+1);
			    pp_warn("\@collo has bad format in $$t[0] (expected -FORM=CF\[GW\])\n");
			}
		    }
		    push @l, ":$f=$s";
		    push @r, '*';
		} else {
		    push @l, $$t[1];
		    push @r, '*';
		}
	    } elsif ($$t[0] == C_GOESTO) {
		# we know we've processed all the lhs; 
		# just replace the entire rhs with the remaining tokens
		$r_mode = 1;
		@r = ();
	    } elsif ($$t[0] == C_STAR) {
		push @l, '*';
		push @r, '*';
	    } elsif ($$t[0] == C_BAD) {
		pp_line($i+1);
		pp_warn("\@collo has unparseable token '$$t[1]'");
	    } else {
		warn pp_file().':'.pp_line().
		    ": internal error: unhandled token type $$t[0]\n";
	    }
	}
    }
    my $r = join('',@r);
    if ($r =~ /^\*+$/) {
	"@l";
    } else {
	if ($#l != $#r) {
	    pp_line($i+1);
	    my $ll = join('|',@l);
	    my $rr = join('|',@r);
	    pp_warn("\@collo has different length left and right sides /$ll/=>/$rr/");
	    "";
	} else {
	    my $l = "@l";
	    my $r = "@r";
	    if ($l && $r) {
		"@l => @r";
	    } else {
		pp_line($i+1);
		pp_warn("\@collo has empty side(s) '$l => $r'");		
	    }
	}
    }
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
	} elsif ($c =~ s/^([;!]*(?:n|[A-Z][A-Z]*))\s+//) {
	    push @t, [ C_POS, $1, $i ];
	} elsif ($c =~ s/^([;!]*\S+\[.*?\].*?)\s+//) {
	    push @t, [ C_SIG, $1, $i ];
	} elsif ($c =~ s/^-([;!]*\S+)\s+//) {
	    push @t, [ C_FORM, $1, $i ];
	} elsif ($c =~ s/^=>\s+//) {
	    push @t, [ C_GOESTO, $1, $i ];
	} elsif ($c =~ s/^\*\s+//) {
	    push @t, [ C_STAR, '*', $i ];
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
