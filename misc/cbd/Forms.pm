package ORACC::CBD::Forms;

require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/forms_align forms_init forms_term forms_by_cfgw forms_det_clean
    forms_dump forms_load forms_normify forms_print forms_reset forms_validate/;

use warnings; use strict; use open 'utf8'; use utf8;

$ORACC::CBD::Forms::external = 0;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Validate;
use Data::Dumper;

my %bases = ();
my %forms = ();
my $forms_inline = '';
my $map_fh = undef;
my %seen = ();

sub dump_forms {
    my($f,$h) = @_;
    open(F,">$f.dump"); print F Dumper $h; close(F);
}

sub forms_align {
    my($args,$base_cbd,$cbd,$xmap_fh) = @_;
    my @base_cbd = @$base_cbd;
    my @cbd = @$cbd;
    $map_fh = $xmap_fh if $xmap_fh;
    my %forms = forms_collect(@base_cbd);
    dump_forms('base',\%forms);
    my %f_index = ();
    my %incoming_forms = forms_collect(@cbd);
    dump_forms('incoming',\%incoming_forms);
    foreach my $if (keys %incoming_forms) {
	if ($forms{$if}) {
	    my %fi = ();
	    if ($f_index{$if}) {
		%fi = %{$f_index{$if}};
	    } else {
#		warn "indexing forms for $if\n";
		foreach my $f (keys %{$forms{$if}}) {
		    $f =~ s/^\S+\s+(\S+).*$/$1/ if $$args{'lang'} =~ /^sux/;
		    ++$fi{$f};
		}
		$f_index{$if} = %fi;
	    }
#	    print "fi=", Dumper \%fi;
	    foreach my $f (keys %{$incoming_forms{$if}}) {
		next if $f eq '#';
		my $fullform = $f;
		$f =~ s/^\S+\s+(\S+).*$/$1/ if $$args{'lang'} =~ /^sux/;
		#		if ($fi{$f}) {
		if (${$forms{$if}}{$f}) {
		    # warn "$if: incoming form $f already in glossary\n";
		} else {
		    map_form($args,$if,$fullform)
		}
	    }
	}
	# silently ignore missing entries because entries_align gets those
    }
}

sub map_form {
    my($args,$entry,$form) = @_;
    $entry =~ s/\s+\[(.*?)\]\s+/[$1]/;
    $form =~ s/form\!/form/;
    print $map_fh "add form $entry => $form\n";
}

sub forms_collect {
    my @cbd = @_;
    my $curr_entry = '';
    my %f = ();
    for (my $i = 0; $i <= $#cbd; ++$i) {
	if ($cbd[$i] =~ /^\@entry\S*\s+(.*?)\s*$/) {
	    $curr_entry = $1;
	    ++${$f{$curr_entry}}{'#'};
	} elsif ($cbd[$i] =~ /^\@form/) {
	    my $tmp = $cbd[$i];
	    $tmp =~ s/\s+/ /g;
	    ++${$f{$curr_entry}}{$tmp};
	}
    }
    %f;
}

sub forms_init {
    my $args = shift;
    my $forms_outfile = $$args{'lang'}.'.map';
    if (-d '01tmp') {
	$forms_outfile = "01tmp/$forms_outfile";
    }
    open(MAP_FH, ">>$forms_outfile");
    $map_fh = \*MAP_FH;
}

sub forms_term {
    close(MAP_FH);
}

sub forms_by_cfgw {
    if ($_[0]) {
	if (defined $forms{$_[0]}) {
	    return @{$forms{$_[0]}};
	}
    }
    return ();
}

sub forms_det_clean {
    foreach my $cfgw (keys %forms) {
	my @f = @{$forms{$cfgw}};
	my @nf = ();
	foreach my $f (@f) {
	    my($fi,$li,$pr,$fo) = @$f;
	    $fo =~ s/\{-/{/g; # && warn "$fi:$li: killing {- in $fo\n";
	    push @nf, [ $fi, $li, $pr, $fo ];
	}
	@{$forms{$cfgw}} = @nf;
    }
}

sub forms_dump {
    my $f = shift;
    $f = 'forms.dump' unless $f;
    use Data::Dumper;
    open(F,">$f");
    print F Dumper \%forms;
    close(F);
}

sub forms_normify {
    my $pp_file_on_entry = pp_file();
    foreach my $cfgw (keys %forms) {
	if (v_is_entry($cfgw)) {
	    my @f = @{$forms{$cfgw}};
	    my @nf = ();
	    foreach my $f (@f) {
		my($fi,$li,$pr,$fo) = @$f;
		unless ($fo =~ /\$\(/) {
		    pp_file($fi);
		    pp_line($li);
		    my($m) = ($fo =~ m/\s\#(\S+)/);
		    my($b) = ($fo =~ m#\s/(\S+)#);
		    if ($m && $b) {
			my($c) = ($cfgw =~ /^(\S+)/);
			$m =~ s/~/$c/;
			$fo .= " \$$m";
		     	#	    warn "new fo = $fo\n";
		    }
		}
		# we already warned about this error if relevant
		# so just preserve the (bad) form
		push @nf, [ $fi, $li, $pr, $fo ];
	    }
	    @{$forms{$cfgw}} = @nf;
	}
	# we already warned about this error condition in forms_validate
    }    
    pp_file($pp_file_on_entry);
}

sub forms_print {
    my ($cfgw,$fh) = @_;
    if (defined $forms{$cfgw}) {
	my @f = @{$forms{$cfgw}};
	if ($#f >= 0) {
	    foreach my $f (@f) {
		print $fh "\@form\t$$f[3]\n";
	    }
	}
    }
}

sub forms_register_inline {
    my($file,$line,$cfgw,$form) = @_;
    pp_trace("register_inline: @_");
    pp_line($line);
    my($orthform) = ($form =~ m/^(\S+)/);
    if ($form =~ m#\s/(\S+)#) {
	++${$bases{$cfgw}}{$1};
	++${$ORACC::CBD::bases{$cfgw}}{$1}
	  unless ${$ORACC::CBD::bases{$cfgw}}{$1};
    }
    unless ($seen{"$cfgw\:\:$orthform"}++) {
	push @{$forms{$cfgw}}, [ $file, $line, $forms_inline, $form ];
    } else {
	my $tmp = $cfgw;
	$tmp =~ s/\s+(\[.*?\])\s+/$1/;
	pp_warn("entry $tmp has duplicate form $orthform");
    }    
}

sub forms_validate {
    my $pp_file_on_entry = pp_file();
    foreach my $cfgw (keys %forms) {
	if (v_is_entry($cfgw)) {
	    my @f = @{$forms{$cfgw}};
	    foreach my $f (@f) {
		my($fi,$li,$pr,$fo,$af) = @$f;
		pp_file($fi);
		pp_line($li);
		v_set_cfgw($cfgw);
		v_form('form', $fo,$af); # af = autofix
	    }
	} else {
	    my @f = @{$forms{$cfgw}};
	    foreach my $f (@f) {
		my($fi,$li,$pr,$fo) = @$f;
		pp_file($fi);
		pp_line($li);
		pp_warn("orphan form: no entry `$cfgw'");
	    }
	}
    }
    pp_file($pp_file_on_entry);
}

sub forms_load {
    my $args = shift;
    $forms_inline = ORACC::CBD::Util::project();
    my $formsfile = $$args{'cbd'}; $formsfile =~ s/glo$/forms/;

    if (-r $formsfile) {
	load_formsfile($formsfile, $args, 0);
	$ORACC::CBD::Forms::external = 1;
    }

    my @moreforms = <00src/forms/*.forms>;
    my $l = ORACC::CBD::Util::lang();
    foreach my $m (@moreforms) {
	if ($m =~ /:$l\./o) {
	    load_formsfile($m, $args, 1);
	}
    }
}

sub forms_reset {
    %forms = ();
    $ORACC::CBD::Forms::external_forms = 0;
}

sub load_formsfile {
    my($file,$args,$autofix) = @_;
    my($ftag) = ($file =~ m/^(.*?):/);
    my $pp_file_on_entry = pp_file();
    my $pp_line_on_entry = pp_line();
    $ftag = ORACC::CBD::Util::project() unless $ftag;
    warn "loading $file with project tag $ftag\n";
    open(F, $file) || die "merge-forms.plx: unable to open $file\n";
    pp_file($file);
    while (<F>) {
	next if /^#/;
	chomp;
	pp_line($.);
	my($cfgw,$form) = (/^(.*?)\t(\@form.*)$/);
	$form =~ s/^\@form\s+//;
	my($orthform) = ($form =~ m/^(\S+)/);	
	if ($form =~ m#\s/(\S+)#) {
	    ++${$bases{$cfgw}}{$1};
	}
	unless ($seen{"$cfgw\:\:$orthform"}++) {
	    push @{$forms{$cfgw}}, [ $file, $., $ftag , $form , $autofix ];
	} else {
	    my $tmp = $cfgw;
	    $tmp =~ s/\s+(\[.*?\])\s+/$1/;
	    pp_warn("entry $tmp has duplicate form $orthform");
	}
    }
    close(F);
    pp_file($pp_file_on_entry);
    pp_line($pp_line_on_entry);
}

sub uniq {
    my %u = ();
    @u{@_} = ();
    sort keys %u;
}

1;
