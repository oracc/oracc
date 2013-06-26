package ORACC::CHI::Importer;
use warnings; use strict; use open 'utf8';

use ORACC::CHI::Checker;
use ORACC::CHI::Compare;
use ORACC::CHI::Maker;

my %authsets = ();
my %chis = ();
my %chi_warnings = ();
my $have_auth = 0;


# Load a file of chis which may not have terminals
# for all of the chis; autocreate terminals as necessary.
#
# Input must be sorted.
#
# Output is echoed directly as we go, so no other manipulation
# of the chis is done.
sub
import_chi {
    my ($chi1,$out) = @_;
    my %terminals = ();
    open(O, ">$out") || die "ORACC::CHI::Importer: can't write $out for export\n";
    open(C, $chi1) || die "ORACC::CHI::Importer: can't find $chi1 to import\n";
    while (<C>) {
	my($t1,$t2,$t3) = (/^(\S+)\t(\S+)\t(.*?)$/);
	if ($t2 eq '_terminal') {
	    ++$terminals{$t1};
	} else {
	    unless ($terminals{$t1}) {
		warn "bad _terminal $t1\n" if $t1 =~ /\./;
		print O "$t1\t_terminal\t-\n";
	    }
	}
	print O;
    }
    close(C);
}

# Input should be tab-delimited fields where the first three
# are:
#
#  ID COLLECTION PROVENIENCE
#
# where:
#    ID is an arbitrary ID;
#    COLLECTION must be a CDLI collection name if the 
#       collection is known to CDLI, or an
#       ad hoc collection name, or empty
#    PROVENIENCE must be a CDLI provenience if the
#       provenience is known to CDLI, or an ad hoc
#       provenience, or empty.
#
# The remaining fields are sets of numbers for registration 
# as CHIs.
sub
import_tab {
    my($input,$basename) = @_;
    
    if (open(AUTH, "$basename-auth.set")) {
	while (<AUTH>) {
	    chomp;
	    /^(.*?)\./;
	    $authsets{$1} = $_;
	}
	close(AUTH);
	$have_auth = 1;
    } else {
	$have_auth = 0;
    }

    open(T, ">$basename.tab");
    if ($input eq '-') {
	while (<>) {
	    create_sub($basename);
	}
    } else {
	open(IN, $input) || die ("ORACC::CHI::Importer: can't open '$input' for reading\n");
	while (<IN>) {
	    create_sub($basename);
	}
	close(IN);
    }
    close(T);

    open(C, ">$basename.chi");
    foreach my $c (sort keys %chis) {
	print C $chis{$c};
    }
    close(C);
    
    foreach my $c (sort keys %chi_warnings) {
	my @w = grep !/discrepant/, @{$chi_warnings{$c}};
	warn @w if $#w >= 0;
    }
    
    open(D, ">discrepant.log");
    foreach my $c (sort keys %chi_warnings) {
	foreach my $d (grep /discrepant/, @{$chi_warnings{$c}}) {
	    $d =~ s/:\s+(\S+) and (\S+)//;
	    print D "$d\n\t$1\nand\t$2\n\n";
	}
    }
    close(D);
}

sub
create_sub {
    my $basename = shift;
    chomp;
    my @fields = split(/\t/, $_);
    my @xfields = ();
    my $chi = undef;
    my $set = undef;
    
    if ($#fields == 0) {
	unshift @fields, "$basename.$.";
	$chi = ORACC::CHI::Maker::set(@fields);
    } else {
	foreach my $s (@fields[3..$#fields]) {
	    if ($s) {
		if (my $set = ORACC::CHI::Maker::set($fields[0],$s)) {
		    push @xfields, $set;
		} else {
		    warn "set creation failed for $s\n";
		}
	    } else {
		push @xfields, '';
	    }
	}
	return unless $xfields[0];
	$chi = $xfields[0];
	shift @xfields;
    }
    
    if ($chi =~ /\./) {
	$set = $chi;
	$chi =~ s/\..*$//;
    } else {
	$set = '-';
    }
    
    my @C = ();
    push @C, "$chi\t_terminal\t$set\n";
    push @C, "$chi\tid\t$fields[0]\n";
    foreach my $x (@xfields) {
	push @C, "$chi\thas-name\t$x\n";
    }
    if ($chis{$chi}) {
	if ($authsets{$chi}) {
	    my($prev) = ($chis{$chi} =~ /_terminal\t(\S+)/);
	    if ($prev ne $authsets{$chi}) {
		$chis{$chi} =~ s/_terminal\t(\S+)/_terminal\t$authsets{$chi}/;
	    }
	} else {
	    my($prev) = ($chis{$chi} =~ /_terminal\t(\S+)/);
	    my($prev_id) = ($chis{$chi} =~ /id\t(\S+)/);
	    if ($prev eq $set) {
		push @{$chi_warnings{$chi}}, "$chi: keeping $prev_id; dropping duplicate set $fields[0]=$set\n";
	    } else {
		if (!ORACC::CHI::Compare::ignoring_joins($prev, $set)) {
		    push @{$chi_warnings{$chi}}, "$chi: keeping $prev_id; dropping similar set: $fields[0]=$set (vs. $prev)\n";
		} else {
		    push @{$chi_warnings{$chi}}, "$chi: discrepant sets ($prev_id vs. $fields[0]): $prev and $set\n";
		}
	    }
	}
    } else {
	$chis{$chi} = join('',@C);
    }
    print T join("\t", @fields, @xfields), "\n";
}

1;
