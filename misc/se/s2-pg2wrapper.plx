#!/usr/bin/perl
use warnings; use strict;
use File::Temp qw/tempfile/;
use Getopt::Long qw(:config no_ignore_case);
use lib '/usr/local/oracc/lib';

my $ce_type = 'line';
my $input = '';
my $kwic = 0;
my $line = 0;
my $output = '';
my $pgargs = '';
my $project = '';
my $state = '';
my $text = 0;
my $unit = 0;
my $verbose = 0;

GetOptions(
    'i:s'=>\$input,
    'k'=>\$kwic,
    'l'=>\$line,
    'o:s'=>\$output,
    'p:s'=>\$pgargs,
    'P:s'=>\$project,
    's:s'=>\$state,
    't'=>\$text,
    'u'=>\$unit,
    );

my $dir = $input;
$dir =~ s#/[^/]+$##;

if ($kwic) {
    $ce_type = 'kwic';
} elsif ($unit) {
    $ce_type = 'unit';
}

$pgargs =~ s/^/-/;
my @pgargs = split(/_/, $pgargs);
for (my $i = 0; $i <= $#pgargs; ++$i) {
    if ($pgargs[$i] =~ /^-s/) {
	$pgargs[$i] =~ tr/^/_/;
	last;
    }
}

my %input = ();
my @order = ();
if ($input) {
    open(I,$input);
    while (<I>) {
	input_line($_);
    }
    close(I);
} else {
    while (<>) {
	input_line($_);
    }
}

#my($fh,$file) = tempfile(DIR=>$dir);
#print $fh join("\n", @order), "\n";
#close($fh);

my $file = "$dir/pgwrap.order";
open(F,">$file");
print F join("\n", @order), "\n";
close(F);

warn "pg2 @pgargs -l $file\n"
    if $verbose;
my @pg = `/usr/local/oracc/bin/pg2 @pgargs -l $file`;

if ($output) {
    open(O, ">$output");
} else {
    open(O, ">&STDOUT");
}

# generate the pg outline (should control this with an
# arg as there may be times when we call the wrapper but
# will not need the outline)
system "/usr/local/oracc/bin/pg2 -x @pgargs -l $file >/dev/null";

my @outline = ();
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
    $heading_template = ORACC::XPD::Util::option("outline-$state-heading-template");
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

#############################################################

sub
input_line {
    my $line = shift;
    chomp $line;
    my ($start,$end,$word) = ();
    if ($line =~ /\s+(\S+)\s+/) {
	($start,$end,$word) = split(/\s+/, $line);
    } elsif ($line =~ /\.\d+\.\d+$/ && $line !~ /_/) {
	$start = $word = $line;
	$start =~ s/\.\d+$//;
	$end = $start;
    } else {
	$start = $line;
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

1;
