package ORACC::CBD::Util;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/pp_cbd pp_load pp_entry_of pp_sense_of project_from_header/;

use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

my @data = qw/usage collo sense/;

%ORACC::CBD::Util::data = (); @ORACC::CBD::Util::data{@data} = ();

use ORACC::L2GLO::Langcore;
use ORACC::CBD::Forms;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Validate;

sub pp_cbd {
    my ($args,@c) = @_;
    return if pp_status();
    if ($$args{'filter'}) {
	foreach (@c) {
	    print "$_\n" unless /^\000$/;
	}
    } else {
	my $ldir = "$$args{'projdir'}/01tmp";
	system 'mkdir', '-p', $ldir;
	open(CBD, ">$ldir/$$args{'lang'}.glo") 
	    || die "cbdpp.plx: can't write to $ldir/$$args{'lang'}.glo";
	if ($ORACC::CBD::Forms::external) {
	    my $cfgw = '';
	    my $forms_printed = 0;
	    foreach (@c) {
		next if /^\000$/;
		if (/\@entry.*?\s+(.*)$/) {
		    $cfgw = $1;
		    $forms_printed = 0;
		} elsif (/\@sense/ && !$forms_printed) {
		    forms_print($cfgw, \*CBD);
		    ++$forms_printed;
		}
		print CBD "$_\n";
	    }
	} else {
	    foreach (@c) {
		print CBD "$_\n" unless /^\000$/;
	    }
	}
	close(CBD);
    }
}

sub pp_load {
    my $args = shift;
    my @c = ();
    
    if ($$args{'filter'}) {
	@c = (<>); chomp @c;
    } else {
	forms_load($args) if $$args{'cbd'} =~ m#^00src#;
	open(C,$$args{'cbd'}) || die "cbdpp.plx: unable to open $$args{'cbd'}. Stop.\n";
	@c = (<C>); chomp @c;
	close(C);
    }

    my $insert = -1;
    for (my $i = 0; $i <= $#c; ++$i) {
	if ($c[$i] =~ /^$ORACC::CBD::Edit::acd_rx?\@([a-z]+)/) {
	    my $tag = $1;
	    if ($tag ne 'end') {
		if ($tag eq 'project') {
		    pp_line($i+1);
		    $$args{'project'} = v_project($c[$i]);
		} elsif ($tag eq 'lang') {
		    pp_line($i+1);
		    $$args{'lang'} = v_lang($c[$i]);
		} elsif ($tag eq 'qpnbaselang') {
		    pp_line($i+1);
		    my $l = v_lang($c[$i]);
		    if ($l) {
			if (lang_uses_base($l)) {
			    $ORACC::CBD::qpn_base_lang = $l;
			} else {
			    pp_warn("\@qpnbaselang language `$l' does not use bases");
			}
		    }
		    $c[$i] = "\000";
		}
		push(@{$ORACC::CBD::Util::data{$tag}}, $i) if exists $ORACC::CBD::Util::data{$tag};
	    } else {
		$insert = -1;
	    }
	} elsif ($c[$i] =~ s/^\s+(\S)/ $1/) {
	    if ($insert >= 0) {
		$c[$insert] .= $c[$i];
		$c[$i] = "\000";
	    } else {
		pp_warn("indented lines only allowed within \@entry");
	    }
	}
    }

    @c;
}

sub pp_entry_of {
    my ($i,@c) = @_;
    while ($i && $c[$i] !~ /^[+-]?\@entry/) {
	--$i;
    }
    $i;
}

sub pp_sense_of {
    my ($i,@c) = @_;
    while ($i && $c[$i] !~ /\@sense/) {
	--$i;
    }
    $i;
}

sub project_from_header {
    my $c = shift;
    my $p = `head -1 $c`;
    if ($p =~ /^\@project\s+(.*?)\s*$/) {
	$p = $1;
    } else {
	$p = undef;
    }
    $p;
}

1;
