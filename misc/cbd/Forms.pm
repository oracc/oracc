package ORACC::CBD::Forms;

require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/forms_load forms_normify forms_validate/;

use warnings; use strict; use open 'utf8'; use utf8;

$ORACC::CBD::Forms::external = 0;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Validate;

my %bases = ();
my %forms = ();
my $forms_inline = '';
my %seen = ();

sub forms_normify {
}

sub forms_register_inline {
    my($file,$line,$cfgw,$form) = @_;
    warn "register_inline: @_\n";
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
    foreach my $cfgw (keys %forms) {
	if (v_is_entry($cfgw)) {
	    my @f = @{$forms{$cfgw}};
	    foreach my $f (@f) {
		my($fi,$li,$pr,$fo) = @$f;
		pp_file($fi);
		pp_line($li);
		v_set_cfgw($cfgw);
		v_form('form', $fo);
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
}

sub forms_load {
    my $args = shift;
    $forms_inline = $$args{'project'};
    my $formsfile = $$args{'cbd'}; $formsfile =~ s/glo$/forms/;

    if (-r $formsfile) {
	load_formsfile($formsfile, $args);
	$ORACC::CBD::Forms::external = 1;
    }

    my @moreforms = <00src/forms/*.forms>;
    foreach my $m (@moreforms) {
	if ($m =~ /:$$args{'lang'}\./o) {
	    load_formsfile($m, $args);
	}
    }
}

sub load_formsfile {
    my($file,$args) = @_;
    my($ftag) = ($file =~ m/^(.*?):/);
    my $pp_file_on_entry = pp_file();
    my $pp_line_on_entry = pp_line();
    $ftag = $$args{'project'} unless $ftag;
    warn "loading $file with project tag $ftag\n";
    open(F, $file) || die "merge-forms.plx: unable to open $file\n";
    pp_file($file);
    while (<F>) {
	chomp;
	pp_line($.);
	my($cfgw,$form) = (/^(.*?)\t(\@form.*)$/);
	$form =~ s/^\@form\s+//;
	my($orthform) = ($form =~ m/^(\S+)/);	
	if ($form =~ m#\s/(\S+)#) {
	    ++${$bases{$cfgw}}{$1};
	}
	unless ($seen{"$cfgw\:\:$orthform"}++) {
	    push @{$forms{$cfgw}}, [ $file, $., $ftag , $form ];
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
