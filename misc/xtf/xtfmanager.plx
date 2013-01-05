#!/usr/bin/perl
use warnings; use strict;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XPD::Util;
use ORACC::XML;
use ORACC::NS;
use ORACC::Expand2;
use Getopt::Long;
use Pod::Usage;

my @depend = qw/txh/;
my %cmd = (
    txh=>"$ENV{'ORACC'}/bin/g2-xtf2txh.plx",
    xst=>"$ENV{'ORACC'}/bin/xcl2xst.plx",
    uxh=>"$ENV{'ORACC'}/bin/xcl2uxh.plx",
    );

my $xtfrnc = "$ENV{'ORACC'}/lib/schemas/xtf.rnc";

my $bin = "$ENV{'ORACC'}/bin";

my %input_of = (
    xtf=>'atf',
    txh=>'xmd',
    xst=>'xcl',
    uxh=>'xcl',
    );

my %todo = ();
my $basename = 'xtfmanager';
my $corpus = '';
my $dryrun = 0;
my $force = 0;
my $project = '';
my $quiet = 0;
my $syntax = 0;
my $verbose = 0;
my @all_atf = ();
my @all_xtf = ();
my %invalid = ();

my $list = '';
my $valid_list = '';
my $invalid_list = '';

my %paths = ();
my @forms = ();

GetOptions(
    'basename:s'=>\$basename,
    'corpus:s'=>\$corpus,
    'dryrun'=>\$dryrun,
    'force'=>\$force,
    'forms:s'=>\@forms,
    'list:s'=>\$list,
    'invalid_list:s'=>\$invalid_list,
    'project:s'=>\$project,
    'quiet'=>\$quiet,
    'syntax'=>\$syntax,
    'valid_list:s'=>\$valid_list,
    'verbose'=>\$verbose,
    ) || pod2usage(1);


$project = ORACC::XPD::Util::project() unless $project;
print STDERR "project=$project\n" if $verbose;
push(@depend, 'xst') if $syntax;

my $tmp = "01tmp";

my @texts = (@ARGV);
if ($#texts < 0) {
    # list must be a qualified list, usually have-atf or have-xtf
    $list = '01bld/lists/have-atf.lst' unless $list;
    if ($list) {
	open(L,$list) || die "xtfmanager.plx: can't open list '$list'\n";
	while (<L>) {
	    chomp;
	    push @texts, $_;
	}
	close(L);
    }
}

foreach my $pq (@texts) {
    my $xmd = expand_in_project($project,"$pq.xmd");
    my $path = $xmd;
    $path =~ s/xmd$//;
    $paths{$pq} = $path;
    my $atf = $path . 'atf';
    my $xtf = $path . 'xtf';
    
    print STDERR "atf=$atf; xtf=$xtf\n" if $verbose;

    push @all_xtf, $xtf;

    my @needs = ();
    my $xtf_date;
    ### IF YOU WANT TO ADD .atf HANDLING BACK IN, LOOK AT
    ### AN OLD VERSION OF THIS SCRIPT ...
    if (-e $xtf) {
	$xtf_date = (stat($xtf))[9];
    } else {
	print STDERR "$pq has no xtf\n"
	    if $verbose;
	next;
    }

    if ($#needs >= 0) {
	push @needs, @depend;
    } else {
	push @needs, try_depends($pq,$path,$xtf_date);
    }

    if ($#needs >= 0) {
	if ($dryrun || $verbose) {
	    print STDERR "$pq needs ", join(' ', @needs), "\n";
	}
	foreach my $n (@needs) {
	    push @{$todo{$n}}, $pq;
	}
    }
}

foreach my $d (@depend) {
    my @still = nobad(@{$todo{$d}});
    if ($#still >= 0) {
	open(OUT, ">$tmp/$d.lst");
	print OUT join("\n", paths($input_of{$d},@still)), "\n";
	close(OUT);
	my $sys = '';
	my $formsarg = '';
	my $v = $verbose ? '-verbose' : '';
	$v = '' if $d eq 'xtf'; # FIXME: atf2xtf should take -verbose!
	if ($d eq 'xlf' && $#forms >= 0) {
	    $formsarg = '-forms='.join(' -forms=',@forms);
	}
	$sys = "$cmd{$d} $v -project $project $formsarg -list $tmp/$d.lst 2>$tmp/$basename-$d.log";
	if ($dryrun || $verbose) {
	    print STDERR $sys, "\n";
	}
	system $sys unless $dryrun;
    } else {
	unlink "$tmp/$d.lst";
	unlink "$tmp/$d.log";
    }
    # validate all XTF files in the run after doing the XTF phase
    if ($d eq 'xtf') {
	open(RNV,"|xargs $ENV{'ORACC'}/bin/rnv -q $xtfrnc 2>$tmp/$$.rnv");
	print RNV join("\n",@all_xtf);
	close(RNV);
	open(INV,"/usr/bin/atfgrabpq.plx $tmp/$$.rnv|");
	my @invalid = (<INV>);
	close(INV);
	chomp @invalid;
	@invalid = grep length, @invalid;
	@invalid{@invalid} = ();
	if ($#invalid >= 0) {
	    if ($invalid_list) {
		open(INV,">$invalid_list") 
		    || die("xtfmanager.plx: can't write $invalid_list\n");
		print INV join("\n", sort @invalid), "\n";
		close(INV);
	    }
	    unless ($quiet) {
		print STDERR "xtfmanager.plx: invalid xtfs: ", 
		join(' ', @invalid),"\n";
	    }
	}
	if ($valid_list) {
	    my %valid = ();
	    @valid{@texts} = ();
	    delete @valid{@invalid};
	    open(VAL,">$valid_list") 
		|| die("xtfmanager.plx: can't write $valid_list\n");
	    print VAL join("\n", sort keys %valid), "\n";
	    close(VAL);
	}
    }
}

sub
nobad {
    grep !exists($invalid{$_}), @_;
}

sub
paths {
    my $e = shift;
    map { $paths{$_}.$e } @_;
}

sub
try_depends {
    my ($pq,$path,$ref_date) = @_;
    warn "xtfmanager.plx:$pq: try_depends passed undefined ref_date\n" 
	and return @depend
	unless $ref_date;
    return @depend if $force;
    my @ret = ();
    my $start = 0;
    for (my $i = $start; $i <= $#depend; ++$i) {
	my $nxt = $path . $depend[$i];
	my $nxt_date = (stat($nxt))[9];
	if (!$nxt_date || ($ref_date > $nxt_date)) {
	    push @ret, @depend[$i..$#depend];
	    last;
	} else {
	    $ref_date = $nxt_date;
	}
    }
    @ret;
}

1;
__END__

=head1 NAME

xtfmanager.plx -- manage the generation of XTF and derivatives

=head1 SYNOPSIS

xtfmanager.plx [options]

=head1 OPTIONS

=over

=item B<-basename>

The basename for log files written by this xtfmanager run.

=item B<-config>

Give the name of the configuration file; see 
http://oracc.museum.upenn.edu/oracc/doc/XTF/#config
for configuration info (not yet used).

=item B<-corpus>

The corpus (XTC) file to process.

=item B<-dryrun>

Don't carry out any operations, just print a report on what would
need to be done.

=item B<-force>

Run entire processing pipeline; default is to rebuild only products
that are out of date.

=item B<-from [SOURCES]>

List the sources to use; use '-from -' to read from STDIN.

=item B<-verbose>

Make noise about what is happening.

=back

=head1 DESCRIPTION

Manage the XTF processing pipeline including ATF to XTF conversion,
XTF lemmatization, inversion into XCL and XIM production.

Reads filenames from STDIN or processes an XTC file.

=head1 AUTHOR

Steve Tinney (stinney@sas.upenn.edu)

=head1 COPYRIGHT

Copyright(c) Steve Tinney 2006.

Released under the Gnu General Public License
(http://www.gnu.org/copyleft/gpl.html).
