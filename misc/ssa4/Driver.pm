package ORACC::SSA4::Driver;
use warnings; use strict; use open 'utf8'; use utf8;
use lib '@@ORACC@@/lib';
use ORACC::SSA4::Phrasal;

sub
new {
    bless {};
}

sub
driver {
    my ($class,$sent) = @_;
    my $map;
    $sent = ORACC::SSA4::Phrasal::phrasal_merges($sent);
    $map = ORACC::SSA4::Phrasal::map_sentence($sent);
    $sent = ORACC::SSA4::Phrasal::parse_sentence($sent,$map);
    $sent = ORACC::SSA4::Phrasal::post_sentence_fixups($sent);
    $sent;
}

1;
