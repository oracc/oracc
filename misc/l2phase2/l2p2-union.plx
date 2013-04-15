#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Util;
use Getopt::Long;

my $arg_lang = '';
my $autovars = 0;
my $coresig = '';
my $drop_zero = 1;
my %header = ();
my $in_sense = '';
my @instsigs = ();
my $mode = '';
my $oracc = "$ENV{'ORACC'}";
my $out = '';
my $project = `oraccopt`;
my %sig = ();
my %sigfiles = ();
my @sigfiles = ();
my @sigprojects = ();

GetOptions (
    'lang:s'=> \$arg_lang,
    'mode:s'=> \$mode,
    'out:s' => \$out,
    'project:s'=>\$project,
    );

$mode = `oraccopt . cbd-super` unless $mode;

my @projects = union_projects();
my @langs = union_langs(@projects);

if ($arg_lang && grep /^$arg_lang$/, @langs) {
    union_one_lang($arg_lang);
} else {
    foreach my $l (sort keys %sigfiles) {
	union_one_lang($l);
    }
}

######################################################################################

sub
lang_from {
    $_[0] =~ m,^.*?/([^/]+)/union.sig$,;
    return $1;
}

sub
union_langs {
    foreach my $p (@_) {
	foreach my $s (unions_from($p)) {
	    push @{$sigfiles{lang_from($s)}}, $s;
	}
    }
#    use Data::Dumper;
#    warn Dumper \%sigfiles;
#    exit 1;
}

sub
union_projects {
    my @p = ();
    if ($mode eq 'list') {
	if (open(P,'00lib/super-glossary.lst')) {
	    @p = (<P>); close(P);
	} else {
	    warn "l2p2-union.plx: can't open 00lib/super-glossary.lst\n";
	}
    } elsif ($mode eq 'umbrella') {
	@p = grep m#/#, `projpublic.sh $project`;
    } elsif ($mode eq 'global') {
	@p = `projpublic.sh`;
    } else {
	die "l2p2-union.plx: unknown cbd-super value '$mode'\n";
    }
    chomp @p;
    @p;
}

sub
union_one_lang {
    my($lang) = @_;

    my @sigfiles = @{$sigfiles{$lang}};
    
    return unless $#sigfiles >= 0;

    system 'mkdir', '-p', "01bld/$lang";
    $out = "01bld/$lang/union.sig" unless $out && $arg_lang;
    open(O,">$out") || die "l2p2-union.plx: can't open `$out' for output\n";
    select O;
    warn "l2p2-union.plx: creating super-glossary file $out\n";
    
    %sig = ();
    foreach my $s (@sigfiles) {
	warn "$s\n";
	load_sigfile($lang,$s);
    }

    dump_sigs($lang);
}

sub
unions_from {
    eval("<$oracc/bld/$_[0]/*/union.sig>");
}

sub
dump_sigs {
    my $lang = shift;
    print "\@project $project\n\@name $project $lang glossary\n\@lang $lang\n\n";
    foreach (sort keys %sig) {
	my @refs = uniq(@{$sig{$_}});
#	my $i = mysum(map { /;(\d+)/ } @refs);
	my $i = $#refs + 1;
	# The correct format for this is no-percent column; this is what sig-g2x expects
	print "$_\t$i\t@refs\n";
    }
}

sub
load_sigfile {
    my($lang, $file) = @_;
    if (open(S,$file)) {
	while (<S>) {
	    next if /^\s*$/;
	    if (/^\@?(proj(?:ect)?|name|lang)\s+(\S+)/) {
		next;
	    }
	    chomp;
	    
	    my($sig,$freq,$refs,$pers) = split(/\t/,$_);
	    
	    if ($refs && ($freq || !$drop_zero)) {
		if ($sig =~ /^\{/) {
		    $sig =~ s/::.*?\%/::\@$project\%/;
		} else {
		    $sig =~ s/^.*?\%/\@$project\%/;
		}
		push @{$sig{$sig}}, $refs;
	    }
	}
	close(S);
    } else {
	warn "l2p2-union.plx: can't open sigs `$file' for input\n";
    }
}

sub
sigfiles_from {
    my $lang = shift;
    my @ret = ();
    foreach my $m (map { "$oracc/bld/$_/$lang/union.sig" } @_) {
	push(@ret, $m) if -r $m;
    }
    @ret;
}

sub
uniq {
    my %x = ();
    foreach (@_) {
	@x{split(/\s+/,$_)} = ();
    }
    sort keys %x;
}

1;
