package ORACC::BIB::CiteResolver;
use strict; use warnings;
use lib '@@ORACC@@/lib';
use ORACC::NS;
use XML::LibXML;

our $VERSION = '0.01';

my @options = qw(debug context db refs seen);
my $debug = 0;

sub
new {
    my $proto = shift;
    my $class = ref($proto) || $proto;
    my %args = @_;
    @_ = ();

    my %seen = ();
    my %db = ();
    my %refs = ();

    $args{'db'}   = \%db;
    $args{'refs'} = \%refs;
    $args{'seen'} = \%seen;

    ORACC::BIB::Config::init();

    my %hash = map(( "_$_" => $args{$_} ), @options);
    return bless \%hash, $class;
}

# This routine takes the root of a document tree and
# edits it in place.
sub
resolve_cites {
    my ($self,$root) = @_;

    my $seen = \%{$self->{_seen}};
    my $db    = \%{$self->{_db}};
    my $refs  = \%{$self->{_refs}};
    my $context = $self->{_context};
    
    foreach my $c ($root->findnodes('//cites')) {
	my $attr;
	my($prepunct,$postpunct);

	# add @prepunct if it's not set already
	$attr = $c->findnodes('@prepunct');
	if (!defined $attr) {
	    $prepunct = get_prepunct($c);
	    $c->setAttribute('prepunct', $prepunct);
	}

	# add @postpunct if it's not set already
	$attr = $c->findnodes('@postpunct');
	if (!defined $attr) {
	    $postpunct = get_postpunct($c);
	    $c->setAttribute('postpunct', $postpunct);
	}

	my $id;
	my $idnum = 0;
	foreach my $cite ($c->findnodes('cite')) {
	    my $keys_attr = $cite->getAttribute('keys');
	    if (!defined $keys_attr) {
		warn("CiteResolver: no keys attribute on cite\n");
	    } else {
		my ($ix,$res_ref) = $context->exec($keys_attr);
		my @res = @$res_ref;
		if (@res) {
		    if ($#res > 0) {
			warn "CiteResolver: '$keys_attr' matches @res\n";
		    }
		    $id = $res[0];
		    if (!defined $$refs{$id}) {
			$$refs{$id} = $ix->retrieve($id);
		    }
		} else {
		    warn "CiteResolver: no refs match keys '$keys_attr'\n";
		    $id = "BAD";
		}
 	        $cite->setAttribute('ref', $id);
	    }
	}
    }
}

sub
get_prepunct {
    my ($node) = @_;
    get_punct($node,'preceding::');
}

sub
get_postpunct {
    my ($node) = @_;
    get_punct($node,'following::');
}

sub
get_punct {
    my ($node,$axis) = @_;
    my $xpath = './' . $axis . "text()[1]";
    my $text_node = ($node->findnodes($xpath))[0];
    my $text = $text_node->toString();
#    print STDERR "text: $text\n";
    my $ret = '';
    if ($axis =~ /^preceding/) {
	$text =~ /(.)$/;
	$ret = $1;
    } else {
	$text =~ /^(.)/;
	$ret = $1;
    }
    $ret;
}

sub
get_refs {
    my $self = shift;
    %{$self->{_refs}};
}

sub
get_db {
    my $self = shift;
    %{$self->{_db}};
}

1;
