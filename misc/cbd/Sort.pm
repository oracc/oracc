package ORACC::CBD::Sort;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/sort_cbd/;

use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use Data::Dumper;

$ORACC::CBD::Edit::force = 0;

use ORACC::CBD::Util;
use ORACC::CBD::PPWarn;

use Data::Dumper;

my %cgc = ();
my $cgctmp = '';

sub sort_cbd {
    my($args,@cbd) = @_;
    my @e = ();
    my $i;
    my $hdr_end;

    for ($i = 0; $i < $#cbd; ++$i) {
	if ($cbd[$i] =~ /^\@entry\S*\s+(.*?)\s*$/) {
	    if ($#e >= 0) {
		push @{$e[$#e]}, $i-1;
	    } else {
		$hdr_end = $i-1;
	    }
	    my $cfgwpos = $1;
	    $cfgwpos =~ /^(.*?)\s+\[(.*?)\].*$/;
	    my $cf = $1;
	    my $dt = $2;
	    push @e, [ $cf, $dt, $i ];
	} elsif ($cbd[$i] =~ /^\@letter/) {
	    $cbd[$i] = "\000";
	}
    }
    push @{$e[$#e]}, $i-1;
    if ($#e > 0) {
	set_cgc(@e);
	my @ncbd = ();
	push @ncbd, @cbd[0 .. $hdr_end];
	foreach my $s (sort { $cgc{$$a[0]} <=> $cgc{$$b[0]} 
			      || $$a[1] cmp $$b[1] } 
		       @e) {
	    push @ncbd, @cbd[ $$s[2] .. $$s[3] ];
	}
	@ncbd;
    } else {
        @cbd;
    }
}

sub
set_cgc {
    %cgc = ();
    my $tmpname = '';
    if (open(TMP,">01tmp/$$.cgc")) {
	$tmpname = "01tmp/$$.cgc";
    } elsif (open(TMP,">/tmp/$$.cgc")) {
	$tmpname = "/tmp/$$.cgc";
    } else {
	die "set_cgc: can't write /tmp/$$.cgc or tmp/$$.cgc\n";
    }
    $cgctmp = $tmpname;
    my %t = (); @t{@_} = (); # uniq the sort keys
    foreach my $t (@_) {
	my $tx = ${$t}[0];
	$tx =~ tr/_/—/; ### HACK !!!
	$tx =~ tr/ /_/;
	print TMP "${tx}_\n";
    }
    close TMP;
    system 'msort', '-j', '--out', $tmpname, '-ql', '-n1', '-s', '@@ORACC@@/lib/config/msort.order', '-x', '@@ORACC@@/lib/config/msort.exclude', $tmpname;
    open(TMP,$tmpname);
    my @cgc = (<TMP>);
    close(TMP);
    chomp @cgc;
    @cgc = map { s/_$//; tr/_—/ _/; $_ } @cgc;
    @cgc{@cgc} = (0..$#cgc);
    foreach my $e (@_) {
	warn "$$e[0]: not in cgc\n" unless exists $cgc{$$e[0]};
    }
}

1;
