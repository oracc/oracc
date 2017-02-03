#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Util;
use Getopt::Long;
use Data::Dumper;

my %entries = ();
my %printed_forms = ();
my %sigs = ();
my $sig_id = 0;
my @sigtab = ();
my %header = ();

my $arglang = '';
my $curr_cof_form = '';
my $curr_cof_sig  = '';
my $filter = 0;
my $input = '';
my $lang = '';
my $make_sigtab = 0;
my $name = '';
my $output = '';
my $project = '';
my $verbose = 0;

GetOptions (
    'filter'  => \$filter,
    'input:s' => \$input,
    'sig:s' => \$input,
    'lang:s'=> \$arglang,
    'output:s' => \$output,
    'project:s'=>\$project,
    'verbose+'=>\$verbose
    );
$name = "$project/$arglang" if $project && $arglang;
$lang = $header{'lang'} = $arglang if $arglang;
$header{'project'} = $project if $project;
$header{'name'} = $name;

if ($filter) {
    $input = '-';
    $output = '-';
}

my $dirname = '';
$input = shift @ARGV unless $input;
unless ($input eq '-') {
    $dirname = `dirname $input`;
    chomp $dirname;
    if ($dirname)  {
	$dirname .= '/' unless $dirname =~ m#/$#;
    }
}

if ($input eq '-') {
    while (<>) {
	input_line();
    }
} else {
    open(IN,$input) || die "sig-g2a.plx: can't open input $input\n";
    while (<IN>) {
	input_line();
    }
    close(IN);
}

sub
input_line {
    return if /^\s*$/;
    if (/^\@(project|name|lang)\s+(\S+)$/) {
	$header{$1} = $2;
	return;
    }
    chomp;
    my %sig = ();
    my $sig = $_;
    if ($sig =~ /\&\&/) {
	my @cof = ();
	my $index = 0;
	foreach my $c (split(/\&\&/, $sig)) {
	    add_sig($c,$sig,$index++);
	}
    } else {
	add_sig($sig);
    }
}

sub
add_sig {
    my($sig, $full_cof_sig, $index) = @_;
    my $orig_sig = $_;
    my %sig = ();

    if ($sig =~ /^\{/) {
	%sig = (ORACC::L2GLO::Util::parse_psu($sig));
    } else {
	%sig = ORACC::L2GLO::Util::parse_sig($sig);
    }

    $sig{'base'} =~ s/^\+//
	if $sig{'base'};
    $sig{'sig'} = $orig_sig;
    if ($full_cof_sig) {
	if (!$index) {
	    $curr_cof_sig = $sig{'form'};
	} else {
	    $sig{'form'} = $curr_cof_sig;
	}
	if ($header{'lang'} =~ /sux/) {
	    $sig{'norm'} = make_cof_sux_form($full_cof_sig,$index);
	} else {
	    $sig{'norm'} = make_cof_norm($full_cof_sig,$index);
	}
    }
#    warn Dumper \%sig;
    my $entry = make_entry(%sig);
    if ($sig{'parts'}) {
	${$entries{$entry}}{'parts'} = $sig{'parts'};
    }
    ++${${$entries{$entry}}{'bases'}}{$sig{'base'}}
        if $sig{'base'};
    push @{${$entries{$entry}}{"$sig{'epos'}\t$sig{'sense'}"}}, { %sig };
}

exit 0 unless scalar keys %entries;

use Data::Dumper; open L, ">data.log"; print L Dumper(\%entries); close L;

unless ($output eq '-') {
    $output = "$dirname$header{'lang'}.g2a" unless $output;
    open(G, ">$output") || die "sig-g2a.plx: can't open $output for output\n";
    select G;
}

print <<EOH;
\@project	$header{'project'}
\@name		$header{'name'}
\@lang		$header{'lang'}

EOH

if ($make_sigtab) {
    foreach my $e (sort keys %entries) {
	print "\@entry $e\n";
	foreach my $s (sort keys %{$entries{$e}}) {
	    print "\@sense $s\n";
	    foreach my $info (@{${entries{$e}{$s}}}) {
		my $sid = sprintf("s%06x",$sig_id++);
		push @sigtab, [ $sid , $$info{'sig'} ]; # , $$info{'freq'} , $$info{'instances'}
		print "\@form <$sid> $$info{'form'}";
		print " \%$$info{'lang'}" if $header{'lang'} =~ /^qpn/ || $$info{'lang'} =~ /-[0-9]/; # print info{lang} if it has a writing system
		print " \$$$info{'norm'}" if $$info{'norm'};
		my $b = $$info{'base'};
		if ($b) {
		    $b =~ s/^\+// and print " /$b";
		}
		print " +$$info{'cont'}" if $$info{'cont'};
		print " #$$info{'morph'}" if $$info{'morph'};
		print " ##$$info{'morph2'}" if $$info{'morph2'};
		print "\n";
	    }
	}
	print "\@end entry\n\n";
    }
    close G;    
    open(S, ">$dirname$header{'name'}.g2s") 
	|| die "sig-g2a.plx: can't open $dirname$header{'name'}.g2s for output\n";
    foreach my $t (@sigtab) {
	print S join("\t",@$t), "\n";
    }
    close S;
} else {
    foreach my $e (sort keys %entries) {
	my @senses = ();
	print "\@entry $e\n";
	print "\@parts @{${$entries{$e}}{'parts'}}\n" if ${$entries{$e}}{'parts'};
	print "\@bases ", join('; ', map { s/^%.*?:\+?//; $_ } sort keys %{${$entries{$e}}{'bases'}}), "\n"
	    if ${$entries{$e}}{'bases'};
	foreach my $s (sort keys %{$entries{$e}}) {
	    next if $s eq 'parts' || $s eq 'bases';
	    push(@senses, "\@sense $s\n");
	    %printed_forms = ();
	    foreach my $info (@{${entries{$e}{$s}}}) {
		my $sid = sprintf("s%06x",$sig_id++);
		my $pform = $$info{'form'};
		my $formlang = ($$info{'lang'} || $pform);
		$formlang =~ s/:.*$//;
#		warn "g2a: formlang = $formlang; info{lang} = $$info{'lang'}; pform=$pform\n";
		$pform =~ s/^\%.*?://;
		$$info{'base'} =~ s/^\%.*?:// if $$info{'base'};
		push @sigtab, [ $sid , $$info{'sig'} ]; # , $$info{'freq'} , $$info{'instances'}

		my @fbits = ('@form', $pform);
		if ($$info{'lang'} =~ /-949/) {
		    my $nlang = $$info{'lang'};
		    $nlang =~ s#-949#/n#;
		    push @fbits, "\%$nlang";
		} else {
		    push @fbits, "\%$formlang" 
			if ($header{'lang'} =~ /^qpn/ || $formlang =~ /-\d\d\d$/);
		}
# Always emit norm if this is a COF; for Sumerian this actually uses the form but adding
# parens in the right places
##		warn "norm = $$info{'norm'}\n";
		if ($$info{'norm'} && ($$info{'norm'} =~ / /
				       || $$info{'lang'} !~ /^sux/)) {
		    push @fbits, "\$$$info{'norm'}";
		}
		  
		my $b = $$info{'base'};
		if ($b && $$info{'lang'} =~ /^sux/) {
		    $b =~ s/\%.*?://;
		    $b =~ s/^\+//;
		    push @fbits, "/$b";
		}
		push(@fbits, "+$$info{'cont'}") if $$info{'cont'};
		push(@fbits, "#$$info{'morph'}") if $$info{'morph'};
		push(@fbits, "##$$info{'morph2'}") if $$info{'morph2'};
		my $formline = join(' ', @fbits);
		if (${$entries{$e}}{'parts'}) {
		    $formline =~ s/%[-A-Za-z0-9]+://g;
		    $formline =~ s/(\$.*?)$/fixnorm($1)/e;
		    $formline =~ s/\s+(?!\$)/_/g;
		    $formline =~ s/_/ /;
		}
		$formline =~ s/\s\(/ \$(/g;
# 		warn "formline: $formline\n";
		print "$formline\n"
		    unless $printed_forms{$formline}++;
	    }
	}
	print @senses;
	print "\@end entry\n\n";
    }
}

############################################################################

sub
fixnorm {
    my $tmp = shift;
    $tmp =~ s/\s+/ \$/g;
    $tmp;
}

sub
make_entry {
    my %x = @_;
    "$x{'cf'} \[$x{'gw'}\] $x{'pos'}";
}

sub
make_cof_norm {
    my ($f,$i) = @_;
    my @norm = ($f =~ m/\$(\p{L}+)/g);
    for (my $j = 0; $j <= $#norm; ++$j) {
	if ($i != $j) {
	    # this had been commented out, but that breaks output of COFs to *.new;
	    # watch this
	    $norm[$j] =~ s/^(.*?)$/($1)/;
	}
    }
    join(' ', @norm);
}


sub
make_cof_sux_form {
    my ($f,$i) = @_;
    my($form) = ($f =~ m/:(.*?)=/);
    my @norm = ($f =~ m/\$(\p{L}+)/g);
    my @n = ();
    foreach (@norm) {
	push @n, $form;
    }
    for (my $j = 0; $j <= $#n; ++$j) {
	if ($i != $j) {
	    # this had been commented out, but that breaks output of COFs to *.new;
	    # watch this
	    $n[$j] =~ s/^(.*?)$/($1)/;
	}
    }
    my $ret = join(' ', @n);
    $ret =~ s/\s([^(])/ \$$1/g;
    $ret;
}

1;
