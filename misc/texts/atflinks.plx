#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDOUT, ':utf8';

my $project = `proj-from-conf.sh`;
my $currpqx = '';

foreach my $atf ('00atf' , `find [a-z][a-z][a-z][a-z]* -type d -name 00atf`) {
    chomp $atf if $atf =~ /\n$/;
    my $currproj = $atf;
    $currproj =~ s#/?00atf##;
    $currproj ? ($currproj = "$project/$currproj") : ($currproj = $project);

#    warn "atflinks.plx: testing $atf for .atf files\n";
    
    if (`condfind.plx $atf atf`) {
#	warn "$atf has atf files\n";
	foreach (`grep -h '^\\(&\\|#link:\\|<<\\|>>\\|||\\)' $atf/*.atf`) {
	    if (/^\&/) {
		if (/^\&([PQX]\d+)/) {
		    $currpqx = $1;
		} else {
		    # silently ignore malformed &-lines
		}
	    } else {
		s/^#link:\s*//;
		s/(<<|>>|\|\|)\s+/$1/;
		s/^def\s+(\S+)\s*=\s*/def_$1 /;
		s/[\t ]+/\t/;
		s/_/ /;
		print "$currproj\:$currpqx\t$_";
	    }
	}
    }
}

1;
