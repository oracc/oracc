#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8'; binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

use Data::Dumper;
use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::XML;
use Getopt::Long;

my $assign = 0;
my $checking = '';
my $domain = '';
my $errfile = '';
my $keyfile = '';
my @keys = ();
my $nowrite = 0;
my $oid_dir = "$ENV{'ORACC_BUILDS'}/oid";
my $oid_file = "$oid_dir/oid.tab";
my $oid_lock = "$oid_dir/.oidlock";
my %oid_ids = ();
my %oid_keys = ();
my %oid_doms = ();
my %oid_key = ();
my %oid_ext = ();
my @oid_add = ();
my $oid_top = 'o0000000';
my $project = '';
my @res = ();
my $status = 0;
my $xid_template = 'x0000000';

my %domain_authorities = (
    sux => 'epsd2',
    sl  => 'ogsl',
    );

GetOptions(
    assign  => \$assign,
    'domain:s'  => \$domain,
    'keyfile:s' => \$keyfile,
    nowrite => \$nowrite,
    'project:s' => \$project,
    );

fail("must give project with -project") unless $project;
fail("must give domain with -domain") unless $domain;

if ($keyfile) {
    open(K,$keyfile) || fail("can't open key file $keyfile");
    @keys = (<K>);
    chomp(@keys);
} else {
    @keys = (<>);
}
chomp @keys;

oid_load();

oid_keys();

if ($status) {
    fail("errors in processing. Stop.");
}

if ($assign) {
    fail("project $project lacks authority to assign IDs in domain $domain")
	unless $domain_authorities{$domain} eq $project;
    oid_add();

# print Dumper \%oid_ids;
# print Dumper \%oid_keys;
# print Dumper \%oid_doms;
# print Dumper \%oid_ext;
# exit 1;

    oid_dump() unless $status || $nowrite;
}

print @res;

unlink $oid_lock;

1;

######################################################################################

sub bad {
    warn "$errfile:$.: ", @_, "\n";
    ++$status;
}

sub fail {
    unlink $oid_lock;
    my $f = $errfile || $0;
    $. = '' unless defined $.;
    die "$f:$.: ", @_, "\n";
}

sub oid_add {
    foreach my $a (@oid_add) {
	my($dom,$key,$typ,$ext) = @$a;
	++$oid_top;
	$oid_keys{$oid_top} = $key;
	$oid_doms{$oid_top} = $dom;
	$oid_ids{$dom,$key} = $oid_top;
	$oid_ext{$dom,$key} = [ $typ, $ext ];
	push @res, "$key\t$oid_top\n";
    }
}

sub oid_keys {
    foreach my $key (@keys) {
	my($d,$k,$t,$x) = split(/\t/, $key);
	if ($oid_ids{$d,$k}) {
	    push @res, "$k\t$oid_key{$d,$k}\n";
	} else {
	    push @oid_add, [ $d, $k, $t, $x ];
	}
    }
}

sub oid_lock {
    if (-r $oid_lock) {
	my $ntries = 0;
	while (1) {
	    if ($ntries++ == 3) {
		warn "$0: removing stale lock file\n";
		unlink $oid_lock;
	    }
	    sleep 5;
	}
    }
    open OUT, '>$oid_lock';
    print OUT $$;
    close OUT;
}

sub oid_check {
    $checking = 1;
    while (<>) {
	my($oid,$dom,$key,@f) = oid_parse($_);
	oid_validate($oid,$dom,$key) && next;
	if ($oid eq '0') {
	    if ($oid_key{$dom,$key}) {
		$oid_ext{$dom,$key} = [ @f ];
	    } else {
		$oid_key{$dom,$key} = -1;
		push @oid_add, [ $dom, $key, @f ];
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
    foreach my $oid (sort {$a cmp $b} keys %oid_keys) {
	my @e = @{$oid_ext{$domain,$oid_keys{$oid}}};
	my $e1 = '';
	if ($e[0] eq 'sense') {
	    $e1 = $oid_key{$oid_doms{$oid},$e[1]};	    
	}
	print T "$oid\t$oid_doms{$oid}\t$oid_keys{$oid}\t$e[0]\t$e1\n";
	my $xk = xmlify($oid_keys{$oid});
	print X "<oid id=\"$oid\" dom=\"$oid_doms{$oid}\" key=\"$xk\"/>";
    }
    close(T);
    print X '</oids>';
    close(X);
}

# OID FILE FORMAT
# OID DOMAIN KEY TYPE EXT
#
# OID = integer; OID is unique
# DOMAIN = project or authority responsible for ID
# KEY = signature (for now; might be more formats later); KEY is unique
# TYPE= sign or word or sense
# EXT = extended info (for now, OID of word sense belongs to)

sub oid_load {
    if (-r $oid_file) {
	open(O,$oid_file) || die "$0: unable to open $oid_file for read\n";
	while (<O>) {
	    my($oid,$dom,$key,$typ,$ext) = oid_parse($_);
	    oid_validate($oid,$dom,$key) && next;
	    $oid_top = $oid if $oid gt $oid_top;
	    # load only validations--these don't apply when reading check data
	    if ($oid_keys{$oid}) {
		bad("duplicate OID $oid; already defined for $oid_keys{$oid} in domain $oid_doms{$oid}");
		next;
	    } else {
		$oid_keys{$oid} = $key;
	    }
	    if ($oid_key{$dom,$key}) {
		bad("duplicate KEY $key; already defined in DOMAIN $dom for $oid_key{$dom,$key}");
		next;
	    }
	    if ($oid =~ /^o\d+$/) {
		$oid_ids{$dom,$key} = $oid;
	    }
	    $oid_key{$dom,$key} = $oid;
	    $oid_ext{$dom,$key} = [ $typ, $ext ];
	}
	close(O);
	$errfile = $keyfile || '<keys>';
    }
}

sub oid_parse {
    chomp($_[0]);
    split(/\t/,$_[0]);
}

sub oid_validate {
    my($oid,$dom,$key) = @_;
    return bad("no OID in line") unless $oid;
    return bad("no KEY in line") unless $key;
    return bad("no DOMAIN in line") unless $key;
    if ($oid !~ /^[ox]\d+$/) {
	return bad("malformed OID; should be [ox]+digits");
	return 1;
    }
    if ($oid) {
	if ($oid_keys{$oid}) {
	    if ($oid_keys{$oid} ne $key || $oid_doms{$oid} ne $dom) {
		return bad("OID $oid should have KEY $oid_keys{$dom,$key} not $key");
	    }
	} else {
	    if ($oid_key{$dom,$key}) {
		return bad("KEY `$key' should have OID $oid_key{$dom,$key} not $oid");
	    } else {
		return bad("OID $oid not defined") if $checking;
	    }
	}
    } else {
	if ($oid_key{$key} && $oid ne '0') {
	    return bad("won't add KEY `$key' because it already has OID $oid_key{$dom,$key}");
	}
    }
}

