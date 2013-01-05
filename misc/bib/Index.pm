# Encapsulates the data and functionality of an XBib index
package ORACC::BIB::Index;
use strict; use warnings;
use lib '@@ORACC@@/lib';
use Encode;
use GDBM_File;
use ORACC::BIB::Config;
use ORACC::BIB::KeyMangler;

our $VERSION = '0.01';

# Preloaded methods go here.

my @options = qw(debug dbname dbpath khash teihash htmhash);
my $debug = 0;

sub
new {
    my $proto = shift;
    my $class = ref($proto) || $proto;
    my %args = @_;
    @_ = ();
    my %keys = ();
    my %tei = ();
    my %htm = ();

    ORACC::BIB::Config::init();

    # This is not quite right yet; we need to make dbpath
    # a real path and search along it for the index
    my $dbdir = '';
    if (defined $args{'dbpath'}) {
	$dbdir = $args{'dbpath'};
    } else {
	$dbdir = $ORACC::BIB::env->get('dbpath');
    }
    $dbdir .= '/' unless $dbdir =~ m,/$,;

    $debug = $args{'debug'} if defined($args{'debug'});

    if (defined $args{'dbname'}) {
	my $dbname = $args{'dbname'};
	die "no such index '$dbdir$dbname.db'\n" unless -e "$dbdir$dbname.db";
	die "unreadable index '$dbdir$dbname.db'\n" unless -r "$dbdir$dbname.db";
	die "index lacks '$dbdir$dbname-tei.db'\n" unless -r "$dbdir$dbname-tei.db";
	die "index lacks '$dbdir$dbname-htm.db'\n" unless -r "$dbdir$dbname-htm.db";
	tie(%keys, 'GDBM_File', "$dbdir$dbname.db",
	    &GDBM_File::GDBM_READER, 0640) or die;
	tie(%tei, 'GDBM_File', "$dbdir$dbname-tei.db", 
	    &GDBM_File::GDBM_READER, 0640) or die;
	tie(%htm, 'GDBM_File', "$dbdir$dbname-htm.db", 
	    &GDBM_File::GDBM_READER, 0640) or die;
	$args{'khash'} = \%keys;
	$args{'teihash'} = \%tei;
	$args{'htmhash'} = \%htm;
    } else {
	die "arguments to ORACC::BIB::Index constructor must include 'dbname'";
    }

    my %hash = map(( "_$_" => $args{$_} ), @options);
    return bless \%hash, $class;
}

sub
name {
    my $self = shift;
    $self->{_dbname};
}

sub
destroy {
    my $self = shift;
    untie(%{$self->{_khash}});
    untie(%{$self->{_rhash}});
}

sub
lookup {
    my $self = shift;
    my %keys = %{$self->{_khash}};
    my %res;
    my %qkeys;
    my @nkeys = mk_keys(@_);
    foreach my $q (@nkeys) {
	Encode::_utf8_off($q);
	if (defined($keys{$q})) {
	    @{$qkeys{$q}} = split(',',$keys{$q});
	} else {
	    # if any of the keys is undefined in a given index, there 
	    # can be no matches for that index
	    return ();
	}
    }
    my @qkeys = sort { $#{$qkeys{$b}} cmp $#{$qkeys{$a}} } keys %qkeys;
    if ($#qkeys >= 0) {
	foreach my $k (@{$qkeys{$qkeys[0]}}) {
	    $res{$k} = 1;
	}
	for (my $i = 1; $i <= $#qkeys; ++$i) {
	    my %tmp = ();
	    foreach my $t (@{$qkeys{$qkeys[$i]}}) {
		$tmp{$t} = 1;
	    }
	    foreach my $k (keys %res) {
		delete $res{$k} unless defined $tmp{$k};
	    }
	}
	sort {$a cmp $b} keys %res;
    } else {
	();
    }
}

sub
mk_keys {
    my @ret = ();
    foreach (@_) {
	foreach my $k (split) {
	    ORACC::BIB::KeyMangler::canonicalize($k);
	    push @ret, $k;
	}
    }
    @ret;
}

sub
intersection {
    my(%tmp);
    foreach my $t (@{$_[1]}) {
	$tmp{$t} = 1;
    }
    foreach my $k (keys %{$_[0]}) {
	delete ${$_[0]}{$k} unless defined $tmp{$k};
    }
}

sub
retrieve {
    my ($self,@refs) = @_;
    my $ret = '';
    my %refs = %{$self->{_teihash}};
    foreach my $r (@refs) {
	$r =~ s/^.*?\///; # strip prefix for now
	$ret .= $refs{$r} if defined $refs{$r};
    }
    $ret;
}

sub
retrieve_htm {
    my ($self,@refs) = @_;
    my $ret = '';
    my %refs = %{$self->{_htmhash}};
    foreach my $r (@refs) {
	$r =~ s/^.*?\///; # strip prefix for now
	$ret .= "$refs{$r}\n" if defined $refs{$r};
    }
    $ret;
}

1;
