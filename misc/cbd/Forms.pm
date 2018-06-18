package ORACC::CBD::Forms;

require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/forms_by_cfgw forms_dump forms_load forms_normify forms_print forms_reset forms_validate/;

use warnings; use strict; use open 'utf8'; use utf8;

$ORACC::CBD::Forms::external = 0;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;
use ORACC::CBD::Validate;

my %bases = ();
my %forms = ();
my $forms_inline = '';
my %seen = ();

sub forms_by_cfgw {
    if ($_[0]) {
	if (defined $forms{$_[0]}) {
	    return @{$forms{$_[0]}};
	}
    }
    return ();
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
    pp_file($pp_file_on_entry);
}

sub forms_load {
    my $args = shift;
    $forms_inline = ORACC::CBD::Util::project();
    my $formsfile = $$args{'cbd'}; $formsfile =~ s/glo$/forms/;

    if (-r $formsfile) {
	load_formsfile($formsfile, $args);
	$ORACC::CBD::Forms::external = 1;
    }

    my @moreforms = <00src/forms/*.forms>;
    my $l = ORACC::CBD::Util::lang();
    foreach my $m (@moreforms) {
	if ($m =~ /:$l\./o) {
	    load_formsfile($m, $args);
	}
    }
}

sub forms_reset {
    %forms = ();
    $ORACC::CBD::Forms::external_forms = 0;
}

sub load_formsfile {
    my($file,$args) = @_;
    my($ftag) = ($file =~ m/^(.*?):/);
    my $pp_file_on_entry = pp_file();
    my $pp_line_on_entry = pp_line();
    $ftag = ORACC::CBD::Util::project() unless $ftag;
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
