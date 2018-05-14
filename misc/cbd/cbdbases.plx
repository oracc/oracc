#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::Util;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Bases;
use ORACC::CBD::Forms;

use Getopt::Long;

%ORACC::CBD::bases = ();
%ORACC::CBD::forms = ();

my $project = '';
my $lang = '';

my %args = ();
GetOptions(
    \%args,
    qw/cbd:s log:s out:s/,
    ) || die "unknown arg";

foreach my $f (qw/cbd log out/) {
    if ($args{$f}) {
	if ($f eq 'cbd' || $f eq 'log') {
	    die "cbdbases.plx: can't read $f file $args{$f}\n"
		unless -r $args{$f};
	} else {
	    die "cbdbases.plx: can't write $f file $args{$f}\n"
		unless open(O,">$args{'out'}");
	}
    } else {
	die "cbdbases.plx must give -$f <filename> on command line\n";
    }
}

$args{'project'} = project_from_header($args{'cbd'})
    unless $args{'project'};

bases_log(\%args);
pp_file($args{'cbd'});
my @cbd = pp_load(\%args);
@cbd = do_bases(\%args, @cbd);
pp_diagnostics(\%args);
dump_cbd();

sub dump_cbd {
    for (my $i = 0; $i <= $#cbd; ++$i) {
	next if $cbd[$i] =~ /^\000$/;
	print O $cbd[$i], "\n";
    }
    close(O);
}

sub do_bases {
    my($args,@cbd) = @_;
    ($project,$lang) = @$args{qw/project lang/}	;
    my $is_compound = 0;
    my %base_data = ();
    my $cfgw = '';
    my $do_cfgw = 'a [water] N';
    for (my $i = 0; $i <= $#cbd; ++$i) {
	next if $cbd[$i] =~ /^\000$/ || $cbd[$i] =~ /^\#/;
	pp_line($i+1);
	if ($cbd[$i] =~ /^\@entry\s+(.*?)\s*$/) {
	    $cfgw = $1;
	    $cfgw =~ /^(.*?)\s+\[/;
	    my $cf = $1;
	    $is_compound = 1 if $cf =~ /\s/;
	    my @f = forms_by_cfgw($cfgw);
	    foreach my $f (@f) {
		my $f3 = $$f[3];
		next if $f3 =~ /_/;
		$f3 =~ m#/(\S+)#;
		my $b = $1;
		warn "no base in $f3\n" unless $b;
		bases_stats($cfgw,$b);
	    }
	} elsif ($cbd[$i] =~ /^\@end\s+entry/) {
#	    if ($cfgw eq $do_cfgw) {
		my $new_bases = bases_process(%base_data);
		$cbd[$base_data{'line'} - 1] = "\@bases $new_bases";
#	    }
	} elsif ($cbd[$i] =~ /^\@form/) {
	    next if $cbd[$i] =~ /_/;
	    $cbd[$i] =~ m#/(\S+)#;
	    my $b = $1;
	    bases_stats($cfgw,$b);
	} elsif ($cbd[$i] =~ s/^\@bases\S*\s+//) {
	    $base_data{'line'} = pp_line();
	    $base_data{'data'} = $cbd[$i];
	    $base_data{'cfgw'} = $cfgw;
	    $base_data{'compound'} = $is_compound;
	}
    }
    @cbd;
}

1;
