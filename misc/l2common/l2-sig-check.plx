#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use Getopt::Long;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

my @sigs = (<01bld/*.sig>);
push @sigs, (<01bld/[a-z][a-z][a-z]*/*.sig>);

foreach my $s (@sigs) {
  check($s);
}

my %f = ();

sub check {
    my $f = $_[0];
    my $saw_fields = 0;
    %f = ();
    warn "l2-sig-check.plx: can't read signatures file $f\n" and return
	unless (open(F, $f));
    warn "l2-sig-check.plx: checking $f ...\n";
    while (<F>) {
	next if /^\@(?:project|name|lang)/ || /^\s*$/;
	chomp;
	if (/^\@fields/) {
	    if ($saw_fields++) {
		warn "$f:$.: misplaced \@fields line\n";
	    } else {
		set_f($_);
	    }
	    next;
	} elsif (0 == scalar keys %f) {
	    warn "$f:.: no \@fields found before sigs\n";
	}
#	my @fields = split(/\t/,$_);
#	my $sig = $fields[0]."\t$fields[$rank]\t$fields[$freq]\n";
#	my ($lng) = ($sig =~ /\%(.*?):/);
#	push @{$lemmdata{$lng}}, $sig;
    }
    close(F);
}

sub set_f {
    my ($fields,@required) = @_;
    %f = ();
    my @f = split(/\s/, $fields);
    shift @f; # drop '@field';
    for (my $i = 0; $i <= $#f; ++$i) {
	$f{$f[$i]} = $i;
    }

    if ($#required >= 0) {
	foreach my $rq (@required) {
	    die "l2-sig-check.plx: missing fields: @required. Stop\n"
		unless exists $f{$rq};
	}
    }
}

1;
