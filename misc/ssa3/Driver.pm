package ORACC::SSA3::Driver;
use warnings; use strict; use open 'utf8'; use utf8;
use lib '@@ORACC@@/lib';
use ORACC::SSA3::Phrasal;

sub
new {
    bless {};
}

sub
driver {
    my ($class,$sent) = @_;
    my $map;
    $sent = ORACC::SSA3::Phrasal::phrasal_merges($sent);
    $map = ORACC::SSA3::Phrasal::map_sentence($sent);
    $sent = ORACC::SSA3::Phrasal::parse_sentence($sent,$map);
    $sent = ORACC::SSA3::Phrasal::post_sentence_fixups($sent);
    $sent;
}

1;
