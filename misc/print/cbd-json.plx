#!/usr/bin/perl
use warnings; use strict;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;

my $cbd = shift @ARGV;
my @stack = ();
my $depth = 1;
my $iterate = 0;
my $xcbd = load_xml($cbd);

my %howto = ();

###########################################################

print "{\n";
iterate($xcbd->getDocumentElement());
print "\n}\n";

###########################################################

sub
iterate {
    my $n = shift;
    my $ecount = 0;
    node_start($n);
    foreach my $c ($n->childNodes()) {
	my $isa = ref($c);
	# the input will be Oracc data, no whitespace, no mixed content
	if ($isa eq 'XML::LibXML::Element') {
	    if ($ecount++) {
		tabs();
		print ",\n";
	    }
	    ++$depth;
	    iterate($c);
	    --$depth;
	    print "\n";
	}
    }
    node_end($n);
}

sub
node_start {
    my $n = shift;
    my $nm = $n->nodeName();
    my $howto = $howto{$nm};
    if ($howto) {
	my $nam = $nm;
	if ($$howto{'nam'} =~ /^\@(\S+)/) {
	    my $aname = $1;
	    my $attr = undef;
	    if ($attr = $n->getAttribute($aname)) {
		$nam = $attr->value();
	    }
	} elsif ($$howto{'nam'} eq /text()/) {
	    $nam = $n->textContent();
	} elsif (length $$howto{'nam'}) {
	    $nam = $$howto{'nam'};
	} else {
	    $nam = $nm;
	}

	tabs();
	jprint($nam);
	print ": ";

	# Value must always push closer information 
	my $val = undef;
	if ($$howto{'val'} =~ /^\@(\S+)/) {
	    my $aname = $1;
	    my $attr = undef;
	    if ($attr = $n->getAttribute($aname)) {
		jprint($attr->value());
	    } else {
		warn "element $nm wants value from absent attribute $aname\n";
	    }
	    push @stack, '';
	} elsif ($$howto{'val'} =~ /^[\[\{]$/) {
	    $val = $$howto{'val'};
	    if ($val eq '{') {
		push @stack, '}';
	    } else {
		push @stack, ']';
	    }
	} elsif ($$howto{'val'} =~ /text()/) {
	    jprint($n->textContent());
	    push @stack, '';
	} elsif (length $$howto{'val'}) {
	    $val = $$howto{'val'};
	    jprint($$howto{'val'});
	    push @stack, '';
	} else {
	    $val = '{';
	    push @stack, '}';
	}
    } else {
	tabs();
	jprint($nm);
	print ": {\n";
	push @stack, '}';
    }
}

sub
node_end {
    my $closer = pop @stack;
    if ($closer) {
	tabs();
	print $closer;
    }
}

sub
jprint {
    print '"',jsonify($_[0]),'"';
}

sub
tabs {
    print "\t"x$depth;
}

1;
