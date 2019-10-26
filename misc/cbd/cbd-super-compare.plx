#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";

use ORACC::CBD::XML;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Entries;
use ORACC::CBD::Senses;
use ORACC::CBD::Bases;
use ORACC::CBD::Forms;
use Data::Dumper;

system 'rm', '-f', '.super-compare-failed';

warn "$0 invoked as: @ARGV\n";

$ORACC::CBD::no_cbd_ok = 1;

my %args = pp_args();

## No: we are working on 01tmp/xxx.glo where the normification
## has already been done, so we want to do it correspondingly to
## incoming glossaries
##
## 2019-10-26: this program is now used differently, comparison
## is to a nonorm superglo so we need nonorm here, too

$ORACC::CBD::nonormify = 1;

my @base_cbd = ();
my %base_h = ();

if ($args{'base'}) {
    if ($args{'file'} || $args{'#cbd_ok'} || $args{'list'}) {
	if ($args{'base'} =~ /00src/) {
	    die "$0: -base should be a cbd with internal forms\n";
	} else {
	    @base_cbd = setup_cbd(\%args,$args{'base'});
	    %base_h = header_vals($args{'base'});
#	    print Dumper %base_h;
	    if (pp_status()) {
		pp_diagnostics();
		system 'touch', '.supercancel';
		die "$0: can't align bases unless base glossary is clean. Stop.\n";
	    }
	    for (my $i = 0; $i <= $#base_cbd; ++$i) {
		if ($base_cbd[$i] =~ /^\@letter/) {
		    $base_cbd[$i] = "\000";
		}
	    }
	}
    } else {
	%base_h = header_vals($args{'base'});
    }
} else {
    die "$0: must give base glossary with -base GLOSSARY\n";
}

if ($args{'list'}) {
    my @l = `cat $args{'list'}`; chomp @l;
    foreach my $l (@l) {
	pp_status(0);
	pp_reinit();
	$args{'file'} = undef;
	$args{'cbd'} = undef;
	$args{'project'} = $l;
	setup_args(\%args);
	do_one();
	if (pp_status()) {
	    pp_diagnostics();
	    pp_status(0);
	}	
    }
} else {
    do_one();
}

######################################################################################################

sub do_one {
    my @cbd = setup_cbd(\%args) if $args{'#cbd_ok'};

    if (pp_status()) {
	pp_diagnostics();
	system 'touch', '.supercancel';
	die "$0: can't compare unless incoming glossary $args{'cbd'} is clean. Stop.\n";
    }

    for (my $i = 0; $i <= $#cbd; ++$i) {
	if ($cbd[$i] =~ /^\@letter/) {
	    $cbd[$i] = "\000";
	}
    }

    my $mapfile = project($args{'cbd'}) || $args{'project'};
    $mapfile =~ tr#/#-#;
    if ($args{'dynamic'}) {
	system 'mkdir', '-p', '01map';
	$mapfile = '01map/'.$mapfile;
    } else {
	$mapfile = '00map/'.$mapfile;
    }
    $mapfile .= '~'.$base_h{'lang'}.'.map';

    if ($args{'stdout'}) {
	*MAP_FH = *STDOUT;
    } else {
	open(MAP_FH,">$mapfile") || die "$0: unable to open $mapfile for output. Stop.\n";
    }

    unless ($args{'#cbd_ok'}) {
	# warn "$0: Skipping non-existent glossary $args{'cbd'}\n";
	close(MAP_FH);
	return;
    }

    # Now that the *_align routines do multiple duty we need to be sure apply is switched
    # off when this program calls them.
    my $saved_args_apply = $args{'apply'};
    $args{'apply'} = 0;

    entries_align(\%args, \@base_cbd, \@cbd, \*MAP_FH);
    senses_align(\%args, \@base_cbd, \@cbd, \*MAP_FH);
    bases_align(\%args, \@base_cbd, \@cbd, \*MAP_FH);
    forms_align(\%args, \@base_cbd, \@cbd, \*MAP_FH);
    close(MAP_FH);

    if ($saved_args_apply) {
	my @exec_args = ();
	push @exec_args, '-inplace' if $args{'inplace'};
	push @exec_args, '-increment', $args{'increment'} if defined $args{'increment'};
	push @exec_args, '-base', $args{'base'}, $mapfile;
	warn "cbd-super-compare.plx: applying @exec_args\n";
	exec "$ENV{'ORACC_BUILDS'}/bin/cbd-super-map-glos.plx", @exec_args;
    }
}

1;
