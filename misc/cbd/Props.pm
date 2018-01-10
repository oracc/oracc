package ORACC::CBD::Props;

require Exporter;
@ISA=qw/Exporter/;

#@EXPORT = qw/v_project v_lang v_name v_entry v_acd_ok v_bases v_form
#    v_parts v_sense v_bff v_bib v_isslp v_equiv v_inote v_note v_root
#    v_norms v_conts v_prefs v_collo v_geos v_usage v_end v_deprecated/;

@EXPORT = qw/proplist prop/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;

my %props = ();

sub proplist {
    my ($f) = @_;
    my $orig_f = $f;
    my $propproj = '';
    if ($f =~ s/^(.*?)://) {
	$propproj = $1;
    }
    my $ok = -1;
    unless (($ok = props_load($propproj,$f))) {
	unless ($propproj) {
	    unless (props_load('#system',$f)) {
		$ok = 0;
	    }
	}
    }
    pp_warn("unable to load properties file given in '\@proplist $orig_f'")
	unless $ok;
}

sub props_load {
    my($p,$f) = @_;
    my $fn = '';
    if ($p) {
	if ($p eq '#system') {
	    $fn = "$ENV{'ORACC_BUILDS'}/lib/data/props/$f.txt";
	} else {
	    $fn = "$ENV{'ORACC_BUILDS'}/$p/00lib/$f.txt";
	}
    } else {
	$fn = "00lib/$f.txt";
    }
    warn "props_load: trying to load $fn\n";
    if (open(P,$fn)) {
	my $key = '';
	while (<P>) {
	    next if /^\#/ || /^\s*$/;
	    chomp;
	    if (s/^(\S+)\s*//) {
		$key = $1;
	    } else {
		s/^\s*//;
	    }
	    my @v = split(/\s+/, $_);
	    if ($key) {
		@{$props{$key}}{@v} = ();
	    } else {
		pp_warn("proplist $f may not begin with blank space; no key defined yet");
	    }
	}
	return 1;
    } else {
	return 0;
    }
}

sub prop {
    my ($p) = @_;
    $p =~ s/\@prop\s*//;
    my($k,$v) = ($p =~ /^\s*(\S+)\s+(.*)\s*$/);
    if ($k) {
	if ($props{$k}) {
	    my @v = split(/\s+/, $v);
	    foreach my $v (@v) {
		unless (exists ${$props{$k}}{$v}) {
		    pp_warn("\@prop KEY `$k' has undefined value `$v'");
		}
	    }
	} else {
	    pp_warn("\@prop has undefined KEY `$k'");
	}
    } else {
	pp_warn("\@prop must give KEY and VALUE(S)");
    }
}

1;
