#!/usr/bin/perl
use warnings; use strict; use open 'utf8';

my $glo = shift @ARGV;
my %header = ();

open(G, $glo) || die "super induct: unable to read glossary file $glo. Stop\n";
while (<G>) {
    if (/^\@(project|lang|name)\s+(\S+)/) {
	$header{$1} = $2;
    } elsif (/^\@entry/) {
	last;
    }
}

die "super induct: need \@project in glossary header. Stop\n" unless $header{'project'};
die "super induct: need \@lang in glossary header. Stop\n" unless $header{'lang'};

my $out = "$header{'project'}~$header{'lang'}.glo";
$out =~ tr#/#-#;
$out = "00src/$out";
open(O, ">$out") || die "super induct; unable to write output file $out. Stop.\n";
select O;
print "\@project $header{'project'}\n\@lang $header{'lang'}\n\@name $header{'name'}\n\n";
do {
    print if /^\@(?:entry|parts|sense|end)/;
    print "\n" if /^\@end/
} while (<G>);
close(G);

warn "super induct: inducted glossary $glo as $out\n";

1;
