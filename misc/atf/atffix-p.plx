#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;

my $trace = 1;

while (<>) {
    if (/^(.*?):(.*?):(.*?):.*?not in main/) {
	my($file,$line,$pnum) = ($1,$2,$3);
	my $andline = `sed -n ${line}p $file`;
	my $name = '';
	($name) = ($andline =~ /=\s+(.*?)\s*$/);
#	warn "fixing $pnum=$name in $file:$line\n";
	my @res = try_searches($name); chomp @res;
	if ($#res == 0) {
	    warn "res0 = $res[0]\n";
	    $res[0] =~ s/^cdli://;
	    
	    my $newname = `grep $res[0] $ENV{'ORACC_BUILDS'}/pub/cdli/Pnames.tab`;
	    chomp $newname; $newname =~ s/^.*?\t//;
	    print "$pnum=$name => $res[0]=$newname\n";
	} elsif ($#res < 0) {
	    warn "$pnum=$name => NO RESULT\n";
	    print "$pnum=$name => XXX\n";
	} else {
	    warn "$pnum=$name => MANY RESULTS\n";
	}
    }
}

sub try_searches {
    my $orig_name = shift @_;
    my $edname = $orig_name;
    $edname =~ s/,?\s+(\S+)$/_$1/;
    my @res = run_search($edname);
    return @res if $#res == 0;
    $edname =~ s/_/ /;
    @res = run_search($edname);
    return @res if $#res == 0;
    ();
}

sub run_search {
    warn "trying $_[0]\n" if $trace;
    `se \#cdli \!cat $_[0]`;
}

1;
