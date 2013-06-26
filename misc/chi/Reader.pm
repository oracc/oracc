package ORACC::CHI::Reader;
use warnings; use strict;

my @chi_relators = qw/has-id has-image has-ref has-copy has-dupl has-join 
			has-name id was-remapped-from/;
my %chi_relators = (); @chi_relators{@chi_relators} = ();
my @pending = ();

sub
read {
    my($chifile,$expand) = @_;
    unless (-r $chifile) {
	warn "ORACC::CHI::Loader: can't load CHI file $chifile\n";
	return;
    } else {
	open(CHI, $chifile) || die;
    }

    my %chis = ();
    my %prefixes = ();
    while (<CHI>) {
	next if /^\s*$/;
	chomp;
	my($t1,$t2,$t3) = (/^(\S+)\t(\S+)\t(\S.*)\s*$/);
	if ($t1) {
	    if ($t1 =~ /^\@/) {
		${$prefixes{$t1}}{$t2} = $t3;
	    } else {
		add($chifile, $., \%chis, $t1, $t2, $t3, $expand);
	    }
	} else {
	    warn "$chifile:$.: syntax error: $_\n";
	    next;
	}
    }
    close(CHI);

    $chis{'@prefixes'} = { %prefixes };

    if ($#pending >= 0) {
	foreach my $p (@pending) {
	    my ($file, $line, $xp) = @$p;
	    my @s = split(/\.+-?/, $xp);
	    my $t1 = shift @s;
	    foreach my $s (@s) {
		add($file, $line, \%chis, $t1, 'has-join', $s);
	    }	    
	}
    }

#    open(C, ">CHI-Loader.dump"); use Data::Dumper; print C Dumper \%chis; close(C);

    return { %chis };
}

sub
add {
    my($file, $line, $chis, $t1, $t2, $t3, $expand) = @_;
    if ($t2 eq '_terminal') {
	if ($$chis{$t1}) {
	    warn "ORACC::CHI::Loader: skipping duplicate _terminal CHI $t1\n";
	} else {
	    $$chis{$t1} = { '_terminal'=>$t3 };
	    if ($expand && $t3 =~ /\./) {
		push @pending, [ $file, $line, $t3 ];
	    }
	}
    } elsif (exists $chi_relators{$t2} || $t2 =~ /^x-/) {
	push @{$$chis{$t1}{$t2}}, $t3;
    } else {
	warn "$file:$line: unknown relator `$t2'\n";
    }
}

1;
