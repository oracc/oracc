package ORACC::P3::Slicer;
use warnings; use strict;
use integer;

#
# This is the P3 replacement for P2's se-pg2wrapper.plx
#

use lib '@@ORACC@@/lib';

my $verbose = 1;
sub xsystem;
sub
glos_info {
    my %p = @_;
    my $npages = 0;
    my $nitems = 0;
    my $uzpage = 0;
    my $zprev = 0;
    my $znext = 0;

    $nitems = $p{'#listitems'};
    $npages = ($nitems / $p{'pagesize'}) + (($p{'#listitems'} % $p{'pagesize'}) != 0);

    if ($p{'page'} == 1) {
	xsystem "head -$p{'pagesize'} $p{'#list'} >$p{'tmpdir'}/pgwrap.out";
    } else {
	my $from = (($p{'page'}-1) * $p{'pagesize'}) + 1;
	my $to = ($p{'page'} * $p{'pagesize'});
	if ($to > $nitems) {
	    $to = $nitems;
	}
	xsystem "sed -n '${from},${to}p' $p{'#list'} >$p{'tmpdir'}/pgwrap.out";
    }

    $uzpage = ($p{'uzpage'} || 0);
    if ($p{'item'}) {
	if ($p{'item'} > 1) {
	    $zprev = $p{'item'} - 1;
	} else {
	    $zprev = 0;
	}
	if ($p{'item'} < $nitems) {
	    $znext = $p{'item'} + 1;
	} else {
	    $znext = 0;
	}
    }

    open(P,">$p{'tmpdir'}/pg.info");
    print P <<P;
pages $npages
items $nitems
uzpage $uzpage
zprev $zprev
znext $znext
P
    close(P);
}

sub
page_setup {
    my $tmpdir = shift;
    my($input,$order) = read_input(@_);
    my $fh = File::Temp->new( DIR => $tmpdir , UNLINK => 0);
    print $fh join("\n", @$order), "\n";
    close($fh);
    ($fh->filename, $input);
}

# generate the pg outline as HTML directly onto STDOUT so it goes to client
sub
page_outline {
    my($fname,@args) = @_;
    xsystem '@@ORACC@@/bin/pg2', '-x', @args, '-l', $fname;
}

sub
page_info {
    my($tmpdir,$f,$input,$kwic,$project,$state,@pgargs) = @_;

    my %input = %$input;

    warn "pg2 @pgargs -l $f\n" if $verbose;
    my @pg = `@@ORACC@@/bin/pg2 @pgargs -l $f`;
#    unlink $f;

    my @outline = ();

    open(O, ">$tmpdir/pgwrap.out");
    # CBD results have no headings, so we have to take
    # care not to prepend '#' to the first result ID.
    my $has_headings = ($pg[1] =~ /\#/);

    foreach my $b (split(/\#/, $pg[1])) {
	chomp $b;
	next unless defined($b) && length($b);
	my($heading, @starts) = split(/ /, $b);
	unless ($has_headings) {
	    unshift(@starts, $heading);
	    $heading = '';
	}
	my @entries = ();
	foreach (@starts) {
	    foreach my $e (keys %{$input{$_}}) {
		if ($kwic) {
		    my @words = @{${$input{$_}}{$e}};
		    foreach my $w (@words) {
			push @entries, [ $_ , $e , $w ];
		    }
		} elsif ($e =~ /\.x/ || $e !~ /\./) {
		    push @entries, [ $_ , undef , undef ];
		} else {
		    push @entries, [ $_ , $e , @{${$input{$_}}{$e}} ];
		}
	    }
	}
	push @outline, [ $heading , @entries ]
    }

    my $heading_template = '';
    if ($has_headings) {
	use ORACC::XPD::Util;
	ORACC::XPD::Util::set_project($project);
	# look up the proper state in the pgargs
	my $hstate = grep(defined&&length, map { /^-S(.*)$/ && $1 } @pgargs);
	$heading_template = ORACC::XPD::Util::option("outline-$hstate-heading-template");
    }

    foreach my $o (@outline) {
	my $h = $$o[0];

	if ($heading_template) {
	    my @t = split(/\t/,$h);
	    $h = $heading_template;
	    # We have to hack $i here because in the template
	    # we count from 1 and have unadorned numbers, e.g.,
	    # 1,2,3
	    for (my $i = 0; $i <= $#t; ++$i) {
		my $x = $i + 1;
		$h =~ s/$x/$t[$i]/eg;
		$h = "\u$h";
	    }
	} else {
	    $h =~ s#\s+#/#g;
	}

	$h =~ tr/_/ /;
	print O "#$h\n" if $h;
	foreach my $e (@$o[1..$#$o]) {
	    my @o = grep(defined && length, @$e);
	    print O "@o\n" if $#o >= 0;
	}
    }
    close(O);
}

sub
read_input {
    my($infile,$kwic) = @_;
    my %input = ();
    my @order = ();
    open(IN, $infile) || die;
    while (<IN>) {
	chomp;
	my ($start,$end,$word) = ();
	if (/\s+(\S+)\s+/) {
	    ($start,$end,$word) = split(/\s+/, $_);
	} elsif (/\.\d+\.\d+$/ && !/_/) {
	    $start = $word = $_;
	    $start =~ s/\.\d+$//;
	    $end = $start;
	} else {
	    $start = $_;
	}
	push @order, $start unless (defined $input{$start} && !$kwic);
	if ($end) {
	    push @{${$input{$start}}{$end}}, $word;
	} else {
	    my @w = ();
	    @w = split(/\s+/, $word) if $word;
	    push @{${$input{$start}}{$start}}, @w;
	}
    }
    ({%input}, \@order);
}

sub
xsystem {
    warn "system @_\n"
	if $verbose;
    system @_;
}

1;
