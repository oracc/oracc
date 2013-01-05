package ORACC::Log::Util;

sub
create_locator {
    my $l = { @_ };
    $l;
}

sub
entry {
    my($mess,$type,$class,$locator) = @_;
    my @l = "<$type class=\"$class\">";
    push @l, xml_locator($locator);
    push @l, "<message>$mess</message>";
    push @l, "</$type>";
    join('',@l);
}

sub
xml_locator {
    my $l = shift;
    my @loc = '<loc';
    if ($l) {
	foreach my $f (keys %$l) {
	    push @loc, " $f=\"$$l{$f}\"";
	}
    }
    push @loc, '/>';
    @loc;
}

1;
