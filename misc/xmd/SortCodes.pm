package ORACC::XMD::SortCodes;
use warnings; use strict;

# Return an even-sized list giving the local names of fields which 
# have a sort code and their corresponding codes.
#
# The non-XML implementation is a little cheezy, but it's fast
# so I'm just going to live with the shame.
sub
get {
    local $/ = undef;
    open(X,$_[0]) || return ();
    my $X = <X>;
    close(X);
    return ($X =~ m/([A-Za-z0-9_.]+)[\n\s]+c=\"(\d+)\"/g );
}

1;
