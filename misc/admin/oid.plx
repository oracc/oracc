#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8'; binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use Getopt::Long;
use Data::Dumper;
use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::XML;

my %args = ();
my $checking = 0;
my $status = 0;
my $oid_dir = "$ENV{'ORACC_BUILDS'}/oid";
my $oid_file = "$oid_dir/oid.tab";
my %oid_ids = ();
my %oid_key = ();
my %oid_ext = ();
my @oid_add = ();
my $oid_top = 0;

# OID FILE FORMAT
# OID KEY TYPE EXT
#
# OID = integer; OID is unique
# KEY = signature (for now; might be more formats later); KEY is unique
# TYPE= word or sense
# EXT = extended info (for now, OID of word sense belongs to)

oid_args(); $args{'errfile'} = '<stdin>' unless $args{'errfile'};
oid_load();
oid_check();
if ($status) {
    die "$0: errors in processing. Stop.\n";
}
if ($args{'add'}) {
    oid_add();
# print Dumper \%oid_ids;
# print Dumper \%oid_key;
# print Dumper \%oid_ext;
# exit 1;
    oid_dump() unless $status;
}

1;

######################################################################################

sub bad {
    warn "$args{'errfile'}:$.: ", @_, "\n";
    ++$status;
}

sub oid_add {
    foreach my $a (@oid_add) {
	my($key,@f) = @$a;
	++$oid_top;
	$oid_ids{$oid_top} = $key;
	$oid_key{$key} = $oid_top;
	$oid_ext{$key} = [ @f ];
    }
}

sub oid_args {
    GetOptions(
	\%args,
	qw/add check errfile verbose/,
	) || die "unknown arg";
}

sub oid_check {
    $checking = 1;
    while (<>) {
	my($oid,$key,@f) = oid_parse($_);
	oid_validate($oid,$key) && next;
	if ($oid eq '0') {
	    if ($oid_key{$key}) {
		$oid_ext{$key} = [ @f ];
	    } else {
		$oid_key{$key} = -1;
		push @oid_add, [ $key, @f ];
	    }
	}
    }
    $checking = 0;
}

sub oid_dump {
    open(T,">$oid_file");
    $oid_file =~ s/tab$/xml/;
    open(X,">$oid_file");
    print X '<oids>';
    foreach my $oid (sort {$a <=> $b} keys %oid_ids) {
	print T "$oid\t$oid_ids{$oid}\t@{$oid_ext{$oid_ids{$oid}}}\n";
	my $xk = xmlify($oid_ids{$oid});
	print X "<oid id=\"$oid\" key=\"$xk\"/>";
    }
    close(T);
    print X '</oids>';
    close(X);
}

sub oid_load {
    if (-r $oid_file) {
	my $errfile = $args{'errfile'};
	$args{'errfile'} = $oid_file;
	open(O,$oid_file) || die "$0: unable to open $oid_file for read\n";
	while (<O>) {
	    my($oid,$key,@f) = oid_parse($_);
	    oid_validate($oid,$key) && next;
	    $oid_top = $oid if $oid > $oid_top;
	    # load only validations--these don't apply when reading check data
	    if ($oid_ids{$oid}) {
		bad("duplicate OID $oid; already defined for $oid_ids{$oid}");
		next;
	    }
	    if ($oid_key{$key}) {
		bad("duplicate KEY $key; already defined for $oid_key{$key}");
		next;
	    }
	    if ($oid =~ /^\d+$/) {
		$oid_ids{$oid} = $key;
	    }
	    $oid_key{$key} = $oid;
	    $oid_ext{$key} = [ @f ];
	}
	close(O);
	$args{'errfile'} = $errfile;
    }
}

sub oid_parse {
    chomp($_[0]);
    split(/\t/,$_[0]);
}

sub oid_validate {
    my($oid,$key) = @_;
    return bad("no OID in line") unless defined $oid;
    return bad("no KEY in line") unless $key;
    if ($oid !~ /^\d+$/) {
	return bad("malformed OID; should be all digits");
	return 1;
    }
    if ($oid) { # can supply 0 when adding so this is deliberately not "defined $oid"
	if ($oid_ids{$oid}) {
	    if ($oid_ids{$oid} ne $key) {
		return bad("OID $oid should have KEY $oid_ids{$key} not $key");
	    }
	} else {
	    if ($oid_key{$key}) {
		return bad("KEY `$key' should have OID $oid_key{$key} not $oid");
	    } else {
		return bad("OID $oid not defined") if $checking;
	    }
	}
    } else {
	if ($oid_key{$key} && $oid ne '0') {
	    return bad("won't add KEY `$key' because it already has OID $oid_key{$key}");
	}
    }
}

1;
