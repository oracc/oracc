package ORACC::XMD::Fields;
use warnings; use strict; use open ':utf8';
use lib '@@ORACC@@/lib';
use ORACC::Expand2;
use ORACC::XML;

my %cache = ();

sub
drop {
    my $pq = shift;
    my $xmd = $cache{$pq};
    delete $cache{$pq};
    undef $xmd;
}

sub
drop_in_project {
    my ($project,$pq) = @_;
    my $xmd = $cache{"$project:$pq"};
    if ($xmd) {
	delete $cache{"$project:$pq"};
	undef $xmd;
    }
}

sub
get_in_project {
    my ($project,$pq) = @_;
    return $cache{"$project:$pq"} if $cache{"$project:$pq"};
    my $xmd = expand_in_project(undef,"$pq.xmd", $project);
    my $doc = load_xml($xmd);
    return undef unless $doc;
    my %f = ();
    foreach my $nn ($doc->getDocumentElement()->childNodes()) {
	next unless $nn->isa('XML::LibXML::Element');
#	warn("proj field ",$nn->localName(),"\n");
	if ($nn->localName() eq 'cat') {
	    foreach my $n ($nn->childNodes()) {
		$f{$n->localName()} = $n->textContent();
	    }
	} elsif ($nn->localName() eq 'images') {
	    $f{'images'} = $nn;
	}
    }
    return $cache{"$project:$pq"} = { %f };
}

sub
get_field {
    my($pq,$field) = @_;
    my $f = get($pq);
    $$f{$field};
}

sub
get_field_in_project {
    my($project,$pq,$field) = @_;
    my $f = get_in_project($project,$pq);
    $$f{$field};
}

1;
