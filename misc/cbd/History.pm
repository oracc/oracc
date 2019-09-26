package ORACC::CBD::History;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/history history_init history_term history_map history_trim history_all 
    history_all_init history_all_term history_guess/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use Data::Dumper;

my %history = ();

sub history {
    my($l,$e,$s,$to) = @_;
    $s = '' unless $s;
    my $date = `date +\%Y-\%m-\%d`; chomp $date;
    print H "$date\t$l\t$e\t$s\t$to\n";
}

sub history_init {
    open(H,'>>00etc/history.edit');
}

sub history_term {
    close(H);
}

sub history_all_init {
    %history = history_map("$ENV{'ORACC_BUILDS'}/epsd2/00etc/history.all");
#    print Dumper \%history;
}
sub history_all_term {
    %history = ();
}

sub history_guess {
    my $g = shift;
    $g =~ s/\s*\[(.*?)\]\s*/[$1]/;
    my %seen = ();
    while ($history{$g}) {
	$g = $history{$g};
	if ($seen{$g}++) {
	    warn "$0: detected map loop with $g\n";
	    last;
	}
    }
    $g =~ s/\s*\[(.*?)\]\s*/ [$1] /;
    $g;
}

sub history_trim {
    my @h = history_load();
    @h = reverse sort @h;
    my @n = ();
    my %seen = ();
    for (my $i = 0; $i <= $#h; ++$i) {
	my(@f) = split(/\t/,$h[$i]);
	my $k = '';
	if ($#f == 4) { # it's a sense map
	    $k = "$f[2]\t$f[3]";
	} else {
	    $k = $f[2];
	}
	push @n, $h[$i] unless $seen{$k}++;
    }
    history_dump(reverse @n);
}

# History file format is tabbed list:
#
#   DATE LANG CFGWPOS SENSE NEW
#
# SENSE may be empty if NEW is an \@entry
# NEW may be \@entry or \@sense
#
sub history_map {
    my %h = ();
    my @h = history_load(@_);
    my %new_ent = ();
    foreach (@h) {
	my @f = split(/\t/,$_);
	if ($f[4] =~ /\@entry/) {
	    $new_ent{$f[4]} = $f[2];
	    $f[2] =~ s/\s+(\[.*?\])\s+/$1/;
	    $f[4] =~ s/\s+(\[.*?\])\s+/$1/;
	    $f[2] =~ s/\@entry\s+//;
	    $f[4] =~ s/\@entry\s+//;
	    $h{$f[2]} = $f[4];
	}
    }
    # print Dumper \%h;
    foreach (@h) {
	my @f = split(/\t/,$_);
	if ($f[3] =~ /\@sense/) {
	    my $o_ent = $new_ent{$f[2]} || $f[2]; $o_ent =~ s/\@entry\s+//;
	    my $o_sns = $f[3]; $o_sns =~ s/\@sense\s+//;
	    my $n_sns = $f[4]; $n_sns =~ s/\@sense\s+//;
	    my $n_ent = $f[2]; $n_ent =~ s/\@entry\s+//;

	    my $o_sig = $o_ent;
	    $o_sig =~ s/\s+(\[.*?\])\s+/$1/;

	    my $n_sig = $n_ent;
	    $n_sig =~ s/\s+(\[.*?\])\s+/$1/;

##	    $h{$o_sig} = $n_sig unless exists $h{$o_sig};

	    my ($oepos,$osns) = ($o_sns =~ /^(\S+)\s+(.*?)\s*$/);
	    $o_sig =~ s#\](\S+)#//$osns]$1'$oepos#;
	    my ($nepos,$nsns) = ($n_sns =~ /^(\S+)\s+(.*?)\s*$/);	    
	    $n_sig =~ s#\](\S+)#//$nsns]$1'$nepos#;
	    $h{$o_sig} = $n_sig;
	}
    }
    %h;
}

sub history_load {
    my $histfile = shift || '00etc/history.edit';
    open(H, $histfile) || die "$0: no history in $histfile. Stop\n";
    my @h = (<H>); chomp @h;
    close(H);
    @h;
}

sub history_dump {
    open(H, '>00etc/history.edit') || die "$0: can't write to 00etc/history.edit.\n";
    print H join("\n", @_), "\n";
    close(H);    
}

1;
