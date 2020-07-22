package ORACC::OID;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/oid_args oid_check oid_check_mode oid_dump_mode oid_edit
    oid_edit_mode oid_wants_mode oid_wants oid_fail oid_finish oid_init oid_load oid_keys
    oid_dump oid_load_domain oid_lookup oid_merge oid_rename
    oid_status/;

$ORACC::OID::verbose = 0;

use warnings; use strict; use open 'utf8'; use utf8;
use lib "$ENV{'ORACC_BUILDS'}/lib/";
use ORACC::SL::BaseC;
use ORACC::XML;

use Data::Dumper;

my %oid_ids = ();
my %oid_doms = ();
my %oid_keys = ();
my %oid_typs = ();
my %oid_key = ();
my %oid_ext = ();

my %oid_deletions = ();
my %oid_redirects = ();

my @oid_add = ();
my $oid_top = 'o0000000';

my $arg_oid_file = '';
my $assign = 0;
my $check = 0;
my $checking = '';
my $domain = '';
my $dumpout = 0; # does an identity dump to stdout for testing load/dump
my $edit = '';
my @edits = '';
my $errfile = '';
my $ids = 'oid';
my $keyfile = '';
my @keys = ();
my $nowrite = 0;
my $oid_dir = "$ENV{'ORACC_BUILDS'}/oid";
my $oid_file = "$oid_dir/$ids.tab";
my $oid_lock = "$oid_dir/.oidlock";
my $outfile = '';
my $project = '';
my @res = ();
my $status = 0;
my $stdout = 0;
my $wants = 0; # read cbdoid input and output the ones that are wanted
my $xid_template = 'x0000000';
my $xids = 0;

my @domains = qw/sl sux akk arc egy grc hit qca qpc qpe elx peo plq qam qcu qeb xht xhu xur qur xlu hlu uga/;
my %domains = (); @domains{@domains} = ();

my %domain_authorities = (
    sux => 'epsd2',
    sl  => 'ogsl',
    );

my %typ_has_ext = (); @typ_has_ext{qw/form sense/} = (1,1);

my %oid = ();
my $inited = 0;

sub oid_args {
    use Getopt::Long;
    GetOptions(
	assign  => \$assign,
	check => \$check,
	'domain:s'  => \$domain,
	dump => \$dumpout,
	edit  => \$edit,
	'keyfile:s' => \$keyfile,
	nowrite => \$nowrite,
	'oidfile:s' => \$arg_oid_file,
	'outfile:s' => \$outfile,
	'project:s' => \$project,
	x => \$xids,
	w => \$wants, 
	);

    oid_fail("must give project with -project") unless $project;
    oid_fail("must give domain with -domain") unless $domain;
    oid_fail("unknown domain $domain") unless exists $domains{$domain};

    if ($xids) {
	$oid_file = 'xid.tab' unless $arg_oid_file;
    }

    if ($arg_oid_file) {
	$oid_lock = '';
	$oid_file = $arg_oid_file;
    }

    if ($check || $dumpout) {
    } elsif ($edit) {
	@edits = (<>);
	chomp @edits;
    } else {
	if ($keyfile) {
	    open(K,$keyfile) || fail("can't open key file $keyfile");
	    @keys = (<K>);
	    chomp(@keys);
	} else {
	    @keys = (<>);
	}
	chomp @keys;
	if ($xids) {
	    $ids = 'xid';
	    $oid_top = $xid_template;
	    $oid_file = $arg_oid_file = 'xid.tab' unless $arg_oid_file;
	}
    }
}

sub oid_check_mode {
    $check;
}

sub oid_dump_mode {
    $dumpout;
}

sub oid_edit_mode {
    $edit;
}

sub oid_wants_mode {
    $wants;
}

sub oid_load_domain {
    oid_init(@_);
}

sub oid_init {
    my $d = shift @_;
    if (open(O, "$ENV{'ORACC_BUILDS'}/oid/$ids.tab")) {
	if ($d) {
	    while (<O>) {
		my($oid,$dom,$key) = split(/\t/, $_);
		$oid{$dom,$key} = $oid if ($d eq $dom);
	    }
	    close(O);
	} else {
	    while (<O>) {
		my($oid,$dom,$key) = split(/\t/, $_);
		$oid{$dom,$key} = $oid;
	    }
	    close(O);
	}
    } else {
	warn "$0: can't read OID file\n";
    }
    ++$inited;
    if (!$d || $d eq 'sl') {
	ORACC::SL::BaseC::init();
    }

    %oid;
}

sub oid_lookup {
    my($type,$key) = @_;
    oid_init() unless $inited;
    if ($type eq 'sl') {
	if ($key =~ /^\|/) {
	    if (!$oid{$type,$key}) {
		my $res = ORACC::SL::BaseC::c10e_compound($key);
		if ($res ne $key) {
		    warn "OID.pm: mapping $key to $res via c10e\n" if $ORACC::OID::verbose;
		    $key = $res;
		}
	    }
	} else {
	    my $res = ORACC::SL::BaseC::sign_of($key);
	    if ($res && $res ne $key) {
		warn "OID.pm: mapping $key to $res via OGSL\n" if $ORACC::OID::verbose;
		$key = $res;
	    }
	}
    } else {
	$key =~ s/\s*(\[.*?\])\s*/$1/;
    }
    my $ret = $oid{$type,$key};
#    unless ($ret) {
#	my $key2 = $key; $key2 =~ tr/|//d;
#	my $s = ORACC::SL::BaseC::tlit_sig('oid',$key2);
#	if ($s) {
#	    my $ss = '';
#	    warn "OID.pm: $key => sig $s\n" if $ORACC::OID::verbose;	    
#	}
#    }
    $ret;
}

sub oid_finish {
    if ($status) {
	oid_fail("errors in processing. Stop.");
    }

    if ($dumpout || $edit) {
	oid_dump();
    } elsif ($assign || $xids) {
	oid_add();
	oid_dump();
    }
    
    print @res unless $edit;

    unlink $oid_lock;
}

sub oid_term {
    ORACC::SL::BaseC::term();
}

######################################################################################

sub oid_bad {
    warn "$errfile:$.: ", @_, "\n";
    ++$status;
}

sub oid_fail {
    unlink $oid_lock;
    my $f = $errfile || $0;
    $. = '' unless defined $.;
    die "$f: ", @_, "\n";
}

sub oid_add {
    oid_fail("project $project lacks authority to assign IDs in domain $domain")
	unless $xids || $domain_authorities{$domain} eq $project;

    foreach my $a (@oid_add) {
	my($dom,$key,$typ,$ext) = @$a;
	my $this_oid = oid_next_available();
	$ext = '' unless $ext;
	warn "add $dom $key $typ $ext => $this_oid\n";
	$oid_keys{$this_oid} = $key;
	$oid_doms{$this_oid} = $dom;
	$oid_ids{$dom,$key} = $this_oid;
	$oid_ext{$this_oid} = [ $typ, $ext ];
	push @res, "$key\t$this_oid\n";
    }
}

sub oid_keys {
    foreach my $key (@keys) {
	my($d,$k,$t,$x) = split(/\t/, $key);
	$d = $domain if ($d eq 'qpn' || $d eq 'sux-x-emesal');
	if ($oid_ids{$d,$k}) {
	    push @res, "$k\t$oid_key{$d,$k}\n";
	} else {
	    push @oid_add, [ $d, $k, $t, $x ];
	}
    }
}

sub oid_edit {
    foreach (@edits) {
	my @f = split(/\t/,$_);
	next if $f[2] =~  m#//#; # ignore sense changes for now
	my($oid,$cgp,$type) = ($f[1],$f[2],$f[3]);
	if ($type eq '+') {
	    # nothing to do for +
	} elsif ($type eq '-') {
	    # need to store message/redirect
	    oid_delete($f[1],$f[4]);
	} elsif ($type eq '=') {
	    # merge this OID this OID with the now-oid
	    oid_merge($domain,$f[1],$f[4]);
	} elsif ($type eq '>') {
	    # rename this OID to the now-cgp
	    oid_rename($domain,$f[1],$f[4]);
	} else {
	    warn "$0: unhandled edit type $type\n";
	}
    }
}

sub oid_check {
    $checking = 1;
    open(O,$oid_file);
    while (<O>) {
	my($oid,$dom,$key,@f) = oid_parse($_);
	oid_validate($oid,$dom,$key,@f);
    }
    close(O);
    $checking = 0;
}

sub oid_dump {
    return if $status || $nowrite;
    $oid_file = $outfile if $outfile;
    unless ($dumpout) {
	open(T,">$oid_file"); select T;
	$oid_file =~ s/tab$/xml/ || ($oid_file .= '.xml');
	open(X,">$oid_file");
	print X '<oids>';
    }
    foreach my $oid (sort {$a cmp $b} keys %oid_keys) {
	my @e = ();
	my $e1 = '';
	if ($oid_ext{$oid}) {
	    @e = @{$oid_ext{$oid}};
	    if ($e[0] eq 'sense') {
		if ($e[1] !~ /^[ox]\d+/) {
		    $e1 = $oid_ids{$oid_doms{$oid},$e[1]};
		    warn "$0: no OID for sense's owner $e[1]\n" unless $e1;
		    $e1 = '' unless $e1;
		} else {
		    $e1 = $e[1];
		}
	    } else {
		$e1 = '';
	    }
	} else {
	    $e[0] = $oid_typs{$oid};
	    $e1 = '';
	}
	print "$oid\t$oid_doms{$oid}\t$oid_keys{$oid}\t$e[0]\t$e1\n";
	unless ($dumpout) {
	    my $xk = xmlify($oid_keys{$oid});
	    print X "<oid id=\"$oid\" dom=\"$oid_doms{$oid}\" key=\"$xk\"/>";
	}
    }
    unless ($dumpout) {
	close(T); select STDOUT;
	print X '</oids>';
	close(X);
    }
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
	my @later = ();
	open(O,$oid_file) || die "$0: unable to open $oid_file for read\n";
	$errfile = $oid_file;
	while (<O>) {
	    my($oid,$dom,$key,$typ,$ext) = oid_parse($_);
	    if ($typ eq 'sense') {
		push @later, [ $.,$oid,$dom,$key,$typ,$ext ];
	    } else {
		oid_validate($oid,$dom,$key,$typ,$ext) && next;
		do_entry($oid,$dom,$key,$typ,$ext);
	    }
	}
	close(O);
	$errfile = $keyfile || '<keys>';
	foreach my $l (@later) {
	    my($ln,$oid,$dom,$key,$typ,$ext) = @$l;
	    $. = $ln;
	    oid_validate($oid,$dom,$key,$typ,$ext) && next;
	    do_entry($oid,$dom,$key,$typ,$ext);
	}
    }
    open(D,'>D'); print D Dumper \%oid_ext; close(D);
    # print Dumper \%oid_deletions;
    # print Dumper \%oid_redirects;
}

sub do_entry {
    my($oid,$dom,$key,$typ,$ext) = @_;

    if ($key eq 'deleted') {
	++$oid_deletions{$oid};
    } elsif ($key =~ /^o\d+$/) {
	$oid_redirects{$oid} = $key;
    }

    if ($oid_keys{$oid}) {
	oid_bad("duplicate OID $oid; already defined for $oid_keys{$oid} in domain $oid_doms{$oid}");
	return;
    } else {
	$oid_keys{$oid} = $key;
    }

    if ($oid_key{$dom,$key}) {
	oid_bad("duplicate KEY $key; already defined in DOMAIN $dom for $oid_key{$dom,$key}");
	return;
    }

    if ($oid =~ /^o\d+$/) {
	$oid_ids{$dom,$key} = $oid;
    }
    $oid_doms{$oid} = $dom;
    $oid_typs{$oid} = $typ;
    $oid_key{$dom,$key} = $oid unless $oid_deletions{$oid} || $oid_redirects{$oid};
    $oid_ext{$oid} = [ $typ, $ext ] unless $oid_deletions{$oid} || $oid_redirects{$oid};
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

sub oid_next_available {
    1 while $oid_keys{++$oid_top};
#    warn "$0: oid_next_available = $oid_top\n";
    $oid_top;
}

sub oid_parse {
    chomp($_[0]);
    split(/\t/,$_[0]);
}

sub oid_validate {
    my($oid,$dom,$key,$typ,$ext) = @_;
    return oid_bad("no OID in line") unless $oid;
    return oid_bad("no KEY in line") unless $key;
    return oid_bad("no DOMAIN in line") unless $dom;
    if ($oid !~ /^[ox]\d+$/) {
	return oid_bad("malformed OID; should be [ox]+digits");
	return 1;
    }
    if ($oid) {
	if ($oid_deletions{$oid}) {
	    
	} elsif ($oid_redirects{$oid}) {
	    
	} else {
	    if ($oid_keys{$oid}) {
		if ($oid_keys{$oid} ne $key || $oid_doms{$oid} ne $dom) {
		    return oid_bad("OID $oid should have KEY $oid_keys{$dom,$key} not $key");
		}
	    } else {
		if ($oid_key{$dom,$key}) {
		    return oid_bad("KEY `$key' should have OID $oid_key{$dom,$key} not $oid");
		} else {
		    return oid_bad("OID $oid not defined (needed by $key)") if $checking;
		}
	    }
	    return oid_bad("OID $oid has no type") unless $typ;
	    if ($typ_has_ext{$typ}) {
		my $xmsg = '';
		if ($typ eq 'sense') {
		    my $cgp = $key;
		    $cgp =~ s#//.*?](\S+)'.*$#]$1#;
		    my $cgp_id = $oid_key{$dom,$cgp};
		    if ($cgp_id) {
			$xmsg = "--expected\t$oid\t$dom\t$key\t$typ\t$cgp_id";
		    } else {
			$xmsg = "--expected $cgp = $cgp_id but no WORD OID";
		    }
		}
		return oid_bad("type $typ has no extended data$xmsg") unless $ext;
	    }
	}
    } else {
	if ($oid_key{$key} && $oid ne '0') {
	    return oid_bad("won't add KEY `$key' because it already has OID $oid_key{$dom,$key}");
	}
    }
    0;
}

sub oid_wants {
    my @wants = ();
    foreach my $key (@keys) {
	my($d,$k,$t,$x) = split(/\t/, $key);
	$d = $domain if ($d eq 'qpn' || $d eq 'sux-x-emesal');
	if (!$oid_ids{$d,$k}) {
	    if ($x) {
		my $parent = $oid_ids{$d,$x};
		$key =~ s/\t[^\t]+$/\t$parent/ if $parent;
	    }
	    push @wants, $key, "\n";
	}
    }
    print @wants;
    exit 0;
}

sub oid_delete {
    my($oid,$why) = @_;
    $oid_keys{$oid} = 'deleted';
}

sub oid_merge {
    my($dom,$oid,$now_oid) = @_;
    $oid_keys{$oid} = $now_oid;
}

sub oid_rename {
    my($dom,$oid,$key) = @_;
    if ($oid_key{$dom,$key}) {
	# old kiʾus o01234567 is now $oid, like a merge
	$oid_keys{$oid_key{$dom,$key}} = $oid;
    }
    $oid_keys{$oid} = $key;
}

1;
