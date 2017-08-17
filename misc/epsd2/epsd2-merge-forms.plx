#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
my $lang = shift @ARGV;

my %bases = ();
my $curr_cfgw = '';
my %f = ();
my @forms = ();
my $printed_forms = 0;
my @projforms = ();
my %seen = ();

load_forms("00src/$lang.forms", 'epsd');

foreach my $pf (<00src/forms/*\:$lang.forms>) {
    my $pr = `basename $pf`; chomp $pr;
    $pr =~ s/:.*$//;
    load_forms($pf, $pr);
}

#use Data::Dumper; print Dumper \%f; exit 0;

open(G, "00src/$lang.glo") || die;
open(G2, ">00lib/$lang.glo") || die;
select G2;
while (<G>) {
    if (/^\@entry\*?\S*\s+(.*?)\s*$/) {
	$curr_cfgw = $1;
	if ($f{$curr_cfgw}) {
	    @forms = map { $$_[1] } @{$f{$curr_cfgw}};
	    delete $f{$curr_cfgw};
	} else {
	    @forms = ();
	}
	$printed_forms = 0;
    } elsif (/^\@bases/) {
	my %known_bases = ();
	my $tmp = $_;
	chomp $tmp;
	$tmp =~ s/\@bases\s+//;
	$tmp =~ s/\s+\(.*?\)(;|$)/$1/g;
	$tmp =~ tr/°·//d;
	$tmp =~ s/\*\S+(?:\s+|$)//g;
	$tmp =~ tr/;/\cA/;
	$tmp =~ s/\s*\cA\s*/\cA/g;
	@known_bases{split(/\cA/,$tmp)} = ();
#	warn "known_bases=", Dumper(\%known_bases) , "\n";
	if ($bases{$curr_cfgw}) {
	    my %new_bases = ();
	    foreach my $b (keys %{$bases{$curr_cfgw}}) {
		unless (exists $known_bases{$b}) {
		    $b =~ s/\s*$//;
		    ++$new_bases{$b};
		}
	    }
	    if (scalar(keys %new_bases)) {
		chomp;
		my $newb = join('; ', keys %new_bases);
		warn "merge_forms: adding new bases $newb to existing $_\n";
		s/$/; $newb\n/;
	    }
	}
    } elsif (/^\@sense/ && !$printed_forms) {
	if ($#forms >= 0) {
	    print join("\n", @forms), "\n";
	    ++$printed_forms;
	}
    }
    print;
}
close(G);

foreach my $cfgw (sort keys %f) {
    my @proj = map { $$_[0] } @{$f{$cfgw}};
    my %proj = (); @proj{@proj} = ();
    @proj = keys %proj;
    warn "forms for $cfgw were not merged (occurs in @proj)\n";
}

########################################################################

sub
load_forms {
    my($file,$proj) = @_;
    warn "loading $file with project tag $proj\n";
    open(F, $file) || die "merge-forms.plx: unable to open $file\n";
    while (<F>) {
	chomp;
	my($cfgw,$form) = (/^(.*?)\t(\@form.*)$/);
	$form =~ m/\@form\s+(\S+)/;
	my $orthform = $1;
	if ($form =~ m#\s/(\S+)#) {
	    ++${$bases{$cfgw}}{$1};
	}
	unless ($seen{"$cfgw\:\:$orthform"}++) {
	    push @{$f{$cfgw}}, [ $proj , $form ];
	}
	
    }
    close(F);
}

1;
