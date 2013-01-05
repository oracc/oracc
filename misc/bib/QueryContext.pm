package ORACC::BIB::QueryContext;
use strict; use warnings;
use lib '@@ORACC@@/lib';
use ORACC::BIB::Config;
use ORACC::BIB::Index;
use GDBM_File;

our $VERSION = '0.01';

# Preloaded methods go here.

my @options = qw(debug ixlist);

my $tracing = 0;

sub
new {
    my $proto = shift;
    my $class = ref($proto) || $proto;
    my %args = @_;

    my @ixlist = ();
    my $dblist_ref = $ORACC::BIB::env->get('dblist');
    foreach my $db (@$dblist_ref) {
	trace("adding index $db...");
	push (@ixlist, ORACC::BIB::Index->new(dbname => $db));
    }
    $args{'ixlist'} = \@ixlist;

    my %hash = map(( "_$_" => $args{$_} ), @options);
    return bless \%hash, $class;
}

my %warned_multiple = ();
my %warned_none = ();
my %seen = ();
my $idnum = 0;

sub
exec {
    my $self = shift;
    my @ixlist = @{$self->{_ixlist}};
    my $keys = join ' ', @_;
    my @res = ();
    my $ix;
    my $id;
    my %refs = ();
    
    foreach my $x (@ixlist) {
	trace("looking up '$keys' in db '", $x->name, "' ...");
	@res = $x->lookup($keys);
	if (@res) {
	    $ix = $x;
	    last;
	}
    }
    return ($ix, \@res);
}

sub
trace {
    print STDERR "QueryContext: ", @_, "\n" if $tracing;
}

1;
