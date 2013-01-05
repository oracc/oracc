# Encapsulates the data and functionality of an XBib index
package ORACC::BIB::Config;
use strict; use warnings;
use GDBM_File;
use lib '@@ORACC@@/lib';

our $VERSION = '0.01';

# Preloaded methods go here.

$ORACC::BIB::env = undef;

my @options = qw(debug cfgfile);
my $debug = 0;

my %known_keys = ('dbpath'     => '/usr/share/XBib/db',
		  'dblist'     => 'bib',
		  'dbhtml.xsl' => '@@ORACC@@/lib/bib/CMS_A-HTML-each.xsl',
		  'common'     => '@@ORACC@@/lib/bib/common.xml',
		  'names'      => '@@ORACC@@/lib/bib/names.xml',
		  'jrnser'     => '@@ORACC@@/lib/bib/jrnser.xml');

sub
init {
    if (!defined $ORACC::BIB::env) {
	$ORACC::BIB::env = ORACC::BIB::Config->new();
	foreach my $k (keys %known_keys) {
	    $ORACC::BIB::env->set($k, $known_keys{$k});
	}
    }
}

sub
new {
    my $proto = shift;
    my $class = ref($proto) || $proto;
    my %args = @_;
    @_ = ();

    $args{'cfgfile'} = 'xbib.cfg' unless defined $args{'cfgfile'};
    my $cfgfile = $args{'cfgfile'};

    my %hash = map(( "_$_" => $args{$_} ), @options);
    return bless \%hash, $class;
}

sub
load {
    my($self,$cfgfile);
    die "no such config file '$cfgfile'\n" unless -e "$cfgfile";
    die "unreadable config file '$cfgfile'\n" unless -r "$cfgfile";
    open(IN,$cfgfile) || die;
    while (<IN>) {
	my($k,$v) = (/^(\S+)\s*(.*)$/);
	$ORACC::BIB::env->set($k,$v);
    }
    close(IN);
}

sub
set {
    my($self,$key,$val) = @_;
    if (defined $known_keys{$key}) {
	${$self->{_keys}}{$key} = $val;
    } else {
	warn "ORACC::BIB::Config: setting unknown key '$key'\n";
    }
}

sub
get {
    my ($self,$key) = @_;
    my $val = undef;
    if (defined $known_keys{$key}) {
	$val = ${$self->{_keys}}{$key};
    } else {
	warn "ORACC::BIB::Config: attempt to get unknown key '$key'\n";
    }
    $val;
}

1;
