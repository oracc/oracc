package ORACC::SSA3::Parser;
use warnings; use strict; use open 'utf8'; use utf8;
use lib '@@ORACC@@/lib';
use ORACC::SSA3::Phrasal;

sub
new {
    bless {};
}

my %known_promotable_tags = ();
my @known_promotable_tags = ('NP-ABS', 'NP-ERG', 'VC-N', 'VC-F');
@known_promotable_tags{ @known_promotable_tags } = ();
my %known_unpromotable_tags = ();

sub
parse {
    my ($class,$xcl,$sent) = @_;
    my ($nodes,$map) = ();
    ORACC::SSA3::Phrasal::phrasal_reinitialize();
    $sent = ORACC::SSA3::Phrasal::phrasal_merges($sent);
    ($nodes,$map) = ORACC::SSA3::Phrasal::map_sentence($sent);
    $$sent{'children'} = $nodes;
    $sent = ORACC::SSA3::Phrasal::parse_sentence($sent,$map);
    $sent = ORACC::SSA3::Phrasal::post_sentence_fixups($sent);
    if ($$sent{'label'} =~ /^\#/) {
	if ($#{$$sent{'children'}} == 0) {
	    fixup($sent);
	}
    } elsif (!$$sent{'label'}) {
	$$sent{'label'} = 'S-MAT';
    }
    $$sent{'done'} = 1;
    $sent;
}

sub
fixup {
    my $s = shift;
    my $c = ${$$s{'children'}}[0];
    my $s_label = $$s{'label'};
    my $c_label = $$c{'label'};
    if (exists($known_promotable_tags{$c_label})) {
	$s_label =~ s/^(.*?)(?:-.*)?$/$2/;
	$c_label .= $s_label if $s_label;
	$$s{'label'} = $c_label;
	$$s{'children'} = $$c{'children'};
    } else {
	unless (exists($known_unpromotable_tags{$c_label})) {
	    warn "SSA3::Parser: need promotability info on unknown tag `$c_label'\n";
	}
    }
}

1;
