package ORACC::Log::Util;


require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/log_get_line log_set_line log_term/;

use warnings; use strict; use open 'utf8'; use utf8;

my $curr_file = '';
my @lines = ();

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


sub log_get_line {
    my($f,$l) = @_;
    log_open_and_load($f) unless $f eq $curr_file;
    $lines[$l-1];
}
sub log_set_line {
    my($l,$ln) = @_;
    chomp($ln);
    # warn "setting to $ln\n";
    $lines[$l-1] = "$ln\n";
}
sub log_open_and_load {
    my $f = shift;
    log_close_and_dump() if $curr_file;
    if (open(F,$f)) {
	$curr_file = $f;
	@lines = (<F>);
	# warn "$curr_file has $#lines lines\n";
    } else {
	die "can't open err forms file $f\n";
    }
}
sub log_close_and_dump {
    open(N,">$curr_file.new"); print N @lines; close(N);
    close(F);
}
sub log_term {
    log_close_and_dump();
    $curr_file = '';
}

1;
