#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use Getopt::Long;
use ORACC::XML;
use ORACC::NS;
use ORACC::L2GLO::Util;

# If a simple lang argument is given the slice and
# name are assigned to the lang and all dialects of
# the lang are output.
#
# qpn gets special handling, and NN are automatically
# suppressed when a lang other than qpn is specified
# but no POS tests are given.
#
# A lang slice includes all dialects more specific than
# the request.  So, requesting akk also includes 
# akk-x-midbab etc.

my $all = 0;
my @choice_tests = ();
my @constraints = ();
my $corpus = '';
my %corpus = ();
my $dir_output = '';
my $exact_lang_matches = 0;
my $qpn_pattern = '[A-Z]N';
my %matches = ();
my $sigs = '';
my $slice = '';
my $slice_proj = '';
my $slice_lang = '';
my $slice_name = '';
my @slice_tests = ();
my $stdout = 0;
my %type_of = (
    l=>'lang',
    p=>'pos',
    );
GetOptions(
    'all'=>\$all,
    'choice:s'=>\@choice_tests,
    'corpus:s'=>\$corpus,
    'dir:s'=>\$dir_output,
    'exact'=>\$exact_lang_matches,
    'file:s'=>\$slice,
    'language:s'=>\$slice_lang,
    'name:s'=>\$slice_name,
    'project:s'=>\$slice_proj,
    'sigs:s'=>\$sigs,
    'stdout'=>\$stdout,
    'test:s'=>\@slice_tests,
    );

$sigs = '01bld/project.sig' unless $sigs;
$slice = shift @ARGV unless $slice;
$dir_output = "01bld/$slice_lang" unless $dir_output;
my @printsigs = ();

if ($slice_lang =~ /^qpn-/ && $#choice_tests < 0) {
    if ($ORACC::L2GLO::Util::qpnchoices{$slice_lang}) {
	@choice_tests = ($ORACC::L2GLO::Util::qpnchoices{$slice_lang});
    } else {
	die "l2p2-sig-slicer: no choices to select QPNs for $slice_lang\n";
    }
}

if ($#slice_tests >= 0) {
    @slice_tests = map { split(/,/,$_) } @slice_tests;
    load_tests();
} elsif ($#choice_tests >= 0) {
    @choice_tests = map { split(/,/,$_) } @choice_tests;
    load_tests('choice');
} elsif ($slice) {
    load_slice($slice);
} elsif ($slice_lang) {
    push @slice_tests, "l:$slice_lang";
    load_tests();
}

$slice_name = $slice_lang unless $slice_name;
$slice_proj = `oraccopt` unless $slice_proj;

warn "l2p2-sig-slicer.plx: no constraints in $slice\n" 
    unless $#constraints >= 0;

lang_suppress_NN();

#use Data::Dumper; warn Dumper(\@constraints);

load_corpus($corpus) if $corpus;

open(SIGS,$sigs) || die "l2p2-sig-slicer.plx: can't open '$sigs'\n";
while (<SIGS>) {
    next if /^\@(?:proj|name|lang)/;
    next if /^\s*$/;
    my $sig = $_;
    if (/^\{(.*?)\}::/) {
	$sig = $1;
	# import project and lang from first component if
	# they aren't set in PSU.
	# FIXME: what happens with PSUs like aban DN?
	# These should be listed in the glossary under
	# the head, and xref-ed from the tails--this may
	# already be handled, but it needs confirming.
	unless ($sig =~ /\%(.*?):/) {
	    m/(\@.*?\%.*?:)/;
	    $sig .= $1;
	}
    } elsif (/\&\&/) {
	# For COFs we only import the SIG once into the 
	# lang of the head; tails which belong in NN
	# need to be imported there by some other mechanism.
	$sig =~ s/\&\&.*$//;
    }
    my $matched = 1;

    $sig =~ s/-[0-9][0-9][0-9]:/:/
	unless $slice_lang && $slice_lang =~ /-\d\d\d$/; # remove script codes unless we've asked for a script
    foreach my $c (@constraints) {
	if ($sig =~ /$$c[1]/) {
	    $matched = ($$c[0] eq 'yes');
	} else {
	    $matched = ($$c[0] eq 'not');
	}
	if ($matched) {
	    last if $$c[2] eq 'or';
	} else {
	    last unless $$c[2] eq 'or';
	}
    }

#    if ($corpus && $matched) {
#	$matched = trim_refs($_);
#    }
#    print if $matched;

    if ($matched) {	
	chomp;

#	No, we must preserve script codes in signatures to FORMs can keep them

#	my $s949 = (/-949:/ ? "-949" : "");
#	s/\%(.*?):/\%$slice_lang$s949:/ unless $slice_lang =~ /^qpn/;
	my($msig,$refs) = (/^(.*?)\t(.*?)$/);
	unless ($msig) {
	    $msig = $_;
	    $refs = '';
	}
	if ($matches{$msig}) {
	    $refs =~ s/^\d+\s+//;
	    $matches{$msig} .= " $refs";
	} else {
	    $matches{$msig} = $refs;
	}
    }
}
close SIGS;

merge_matches();
if ($#printsigs >= 0) {
    print_sigs();
}

#####################################################################

sub
lang_suppress_NN {
    my $have_pos = 0;
    foreach my $t (@constraints) {
	++$have_pos and last if ($$t[1] =~ /^\\\]/);
    }
    unless ($have_pos) {
	push @constraints, make_constraint('pos', '!'.$qpn_pattern);
    }
}

sub
load_corpus {
    open(C,$_[0]) || die ("sig-slicer.plx: can't open corpus `$_[0]'\n");
    my @corpus = (<C>); chomp @corpus;
    @corpus{@corpus} = ();
    close(C);
}

sub
load_slice {
    my $x = load_xml($_[0]);
    if ($x) {
	$slice_name = childText($x,$G2SLICE,'name') unless $slice_name;
	$slice_lang = childText($x,$G2SLICE,'lang') unless $slice_lang;
	$slice_proj = childText($x,$G2SLICE,'proj') unless $slice_proj;
	foreach my $s (tags($x,$G2SLICE,'keep')) {
	    my $type = $s->getAttribute('type');
	    my $test = $s->textContent();
	    push @constraints, make_constraint($type,$test);
	}
	foreach my $s (tags($x,$G2SLICE,'corpus')) {
	    $corpus = $s->textContent();
	}
    } else {
	exit 1;
    }
}

sub
load_tests {
    my $binop = shift;
    foreach my $t (@slice_tests,@choice_tests) {
	my($prefix,$test) = ($t =~ /^(.):(.*)$/);
	if ($prefix) {
	    my $type = $type_of{$prefix};
	    if ($type) {
		push @constraints, make_constraint($type,$test,$binop);
	    } else {
		warn "sig-slicer.plx: unknown prefix `$prefix' in -t arg `$t'\n";
	    }
	} else {
	    warn "sig-slicer.plx: malformed -t arg `$t'\n";
	}
    }
}

sub
make_constraint {
    my ($type,$test,$binop) = @_;
    my $not = 'yes';
    if ($test =~ s/^\!//) {
	$not = 'not';
    }
    if ($type eq 'lang') {
	if ($test =~ /^qpn/) {
	    make_constraint('pos',(($not eq 'not') ? '!' : '').$qpn_pattern,$binop);
	} elsif ($test eq 'sux') {
	    # Do not merge Emesal into SUX because SUX is Emegir
	    [$not , "\%sux:", $binop ? 'or' : 'and'];
	} elsif ($exact_lang_matches) {
	    [$not , "\%(?:$test):", $binop ? 'or' : 'and'];
	} else {
	    [$not , "\%(?:$test)(?:-.+?)?:", $binop ? 'or' : 'and'];
	}
    } elsif ($type eq 'pos') {
	[$not , "\\](?:$test)'", $binop ? 'or' : 'and'];
    } else {
	die "sig-slicer.plx: type `$type' not handled\n";
    }
}

sub
merge_matches {
    foreach my $s (sort keys %matches) {
	my $sigref = "$s\t$matches{$s}";
	if ($corpus) {
	    if (trim_refs($sigref)) {
		if (uniq_refs($sigref)) {
		    push @printsigs, "$sigref\n";
		}
	    }
	} elsif (uniq_refs($sigref)) {
	    push @printsigs, "$sigref\n";
	}
    }
}

sub
print_sigs {
    unless ($stdout) {
	my $dirpart = '';
	$dirpart = "$dir_output/" if $dir_output;
	open(SLICE,">$dirpart$slice_name.sig"); select SLICE;
    }
    print "\@project $slice_proj\n\@name $slice_name\n\@lang $slice_lang\n\n";
    print @printsigs;
}

sub
trim_refs {
    $_[0] =~ /\@(.*?)\%/;
    my $refs_project = $1;
    $_[0] =~ s/\t([^\t]+$)//;
    my $refs = $1;
    my @keepers = ();
    foreach my $r (split(/\s+/, $refs)) {
	my($prj,$id) = ($r =~ /^(.*?:)?(.*?)\./);
	if (exists $corpus{$id}) {
	    push @keepers, $r;
	} else {
	    if ($prj) {
		$prj =~ s/:$//;
		if ($prj ne $refs_project
		    && exists $corpus{"$prj:$id"}) {
		    push @keepers, "$r";
		}
	    } elsif (exists $corpus{"$refs_project:$id"}) {
		if ($refs_project ne $slice_proj) {
		    push @keepers, "$refs_project:$r";
		} else {
		    push @keepers, "$r";
		}
	    }
	}
    }
    if ($#keepers >= 0) {
	$_[0] .= "\t@keepers\n";
	return 1;
    } else {
	return 0;
    }
}

sub
uniq_refs {
    my($sig,$count,$refs) = ($_[0] =~ /^(.*?)\t(.*?)\t(.*?)$/);
    return $_[0] unless $sig;
    my %refs = ();
    @refs{split(/\s+/, $refs)} = ();
    if (scalar keys %refs) {
	$_[0] = join("\t", $sig, scalar keys %refs, join(' ', sort keys %refs));
    } else {
	if ($all) {
	    $_[0] = "$sig\t0";
	} else {
	    '';
	}
    }
}

1;
