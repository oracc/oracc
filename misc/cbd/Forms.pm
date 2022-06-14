package ORACC::CBD::Forms;

require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/forms_align forms_compare forms_init forms_term forms_by_cfgw
    forms_det_clean forms_dump forms_load forms_normify forms_print
    forms_reset forms_validate forms_merge forms_dot_forms/;

use warnings; use strict; use open 'utf8'; use utf8;

$ORACC::CBD::Forms::external = 0;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Validate;
use ORACC::CBD::Util;
my $acd_rx = $ORACC::CBD::acd_rx;
use Data::Dumper;

$ORACC::CBD::Forms::forms_check_form = '';

my %formcharkeys = (
    '%'=>'lang',
    '$'=>'norm',
    '/'=>'base',
    '+'=>'cont',
    '#'=>'morph',
    );

my %bases = ();
my %efl = ();
my %forms = ();
my $forms_inline = '';
my $alang = '';
my $map_fh = undef;
my %seen = ();
my %fi_added = ();

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
#		warn "indexing forms for $if; argslang=$$args{'lang'}\n";
		foreach my $f (keys %{$forms{$if}}) {
		    my $tmp = $f;
		    $tmp =~ s/^\@form\S*\s+//;
		    $tmp =~ s/\s.*$//
			if ($$args{'lang'} =~ /^sux/
			    || ($$args{'lang'} =~ /^qpn/ && $f =~ m,\s/,)); # use presence of base as proxy for Sum lang
		    ++$fi{$tmp};
		}
		%{$f_index{$if}} = %fi;
	    }
	    foreach my $f (keys %{$incoming_forms{$if}}) {
		next if $f eq '#';
		my $fullform = $f;
		$f =~ s/^\@form\S*\s+//;
		$f =~ s/\s.*$//
		    if ($$args{'lang'} =~ /^sux/
			|| ($$args{'lang'} =~ /^qpn/ && $f =~ m,\s/,));
		if ($fi{$f} || $fi_added{$if,$f}) {
		    # if (${$forms{$if}}{$f}) { ## why did I try this?
		    # warn "$if: incoming form $f already in glossary\n";
		} else {
#		    warn "$if: form $f/fullform=$fullform not found in glossary\n";
		    map_form($args,$if,$fullform);
		    ++$fi_added{$if,$f};
		}
	    }
	}
	# silently ignore missing entries because entries_align gets those
    }
    open(FI,'>fi.dump'); print FI "fi=", Dumper \%f_index; close(FI);
}

sub form_chars_ok {
    if (/\@form\S*\s+(\S+)/) {
	$ORACC::CBD::Forms::forms_check_form = $1;
	return $ORACC::CBD::Forms::forms_check_form =~ /^[-_\\%*+:.&×‌\/|@(){}a-zA-ZšḫŋṣṭŠḪŊṢṬʿʾ₀-₉ₓ0-9⁻]+$/;
    }
    1;
}

sub forms_compare {
    my($args,$base_cbd,$cbd,$xmap_fh) = @_;
    my @cbd = @$cbd;
    my $curr_entry = '';

    $alang = $$args{'lang'} if $$args{'lang'};
    $alang = 'sux' unless $alang && $alang ne 'qpn';
    warn "$0: alang set to $alang\n";
    
    my %forms = ();
    if ($$args{'forms'}) {
	my @forms = `cat $$args{'forms'}`; chomp @forms;
	%forms = forms_collect(@forms);
    } else {
	my @base_cbd = @$base_cbd;
	%forms = forms_collect(@base_cbd);
    }

    dump_forms('base',\%forms);
    
    my %f_index = ();
    foreach my $k (keys %forms) {
	foreach my $f (keys %{$forms{$k}}) {
	    next if $f eq '#';
	    $f =~ /\s+(\S+)/;
	    my $fo = $1;
	    $f_index{$k,$fo} = $f;
	    $efl{$k,$fo} = ${$forms{$k}}{$f};
	}
    }

#    dump_forms('f_index',\%f_index);

    for (my $i = 0; $i <= $#cbd; ++$i) {
	if ($cbd[$i] =~ /\@entry\S*\s*(.*?)\s*$/) {
	    $curr_entry = $1;
	} elsif ($cbd[$i] =~ /\@form\S*\s+(\S+)/) {
	    my $curr_form = $1;
	    if ($alang && $cbd[$i] =~ /\s\%(\S+)\s/) {
		my $flang = $1;
		next unless $alang eq $flang;
	    }
	    my $core_form = $f_index{$curr_entry,$curr_form};
	    my $efl = $efl{$curr_entry,$curr_form};
	    if ($core_form) {
		my $cbd_form = $cbd[$i];
		$cbd_form =~ s/form\!/form/;
		if ($core_form ne $cbd_form) {
		    my $l = $i+1;
		    warn pp_file().":$l: discrepant forms for $curr_form=$curr_entry:\nCORE:$efl:\t$core_form\nPERI:$l:\t$cbd[$i]\n";
		}		
	    } else {
		my $l = $i+1;
		warn pp_file().":$l: missing form:\t$curr_entry\t$cbd[$i]\n";
	    }
	}
    }
}

sub forms_find {
    my @f = ();
    foreach (@_) {
	push @f, $_ if /^\@form/;
    }
    @f;
}

sub forms_merge {
    my($b,$s,%map) = @_;
    my %b = ();
    my @b = forms_find(@$b);
    foreach my $bf (@b) {
	$bf =~ /\@form\S*\s+(\S+)/;
	$b{$1} = $bf;
    }
    my %s = ();
    my @s = forms_find(@$s);
    foreach my $sf (@s) {
	$sf =~ /\@form\S*\s+(\S+)/;
	$s{$1} = $sf;
    }
    foreach my $sf (keys %s) {
	unless ($b{$sf}) {
	    my $sff = $s{$sf};
	    warn "$sff\n" if $sff =~ /\s\!/;
	    $sff =~ s/\@form\s*\!\s*/\@form /;
	    my($base) = ($sff =~ m#\s/(\S+)#);
	    if ($map{$base}) {
		$base = $map{$base};
		$sff =~ s#^(.*?\s+)/\S+(.*)$#$1/${base}$2#;
	    }
	    $b{$sf} = $sff;
	}
    }
    sort values %b;
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
	if ($cbd[$i] =~ /^$acd_rx?\@entry\S*\s+(.*?)\s*$/) {
	    $curr_entry = $1;
	    ++${$f{$curr_entry}}{'#'};
	} elsif ($cbd[$i] =~ /^\@form/) {
	    my $flang = '';
	    if ($alang && $cbd[$i] =~ /\%(\S+)\s/) {
		$flang = $1;
		next unless $flang eq $alang;
	    }
	    my $tmp = $cbd[$i];
	    $tmp =~ s/\s+/ /g;
	    ${$f{$curr_entry}}{$tmp} = $i+1;
	} elsif ($cbd[$i] =~ /^(.*?)\t(\@form.*)\s*$/) {
	    my $flang = '';
	    if ($alang && $cbd[$i] =~ /\%(\S+)\s/) {
		$flang = $1;
		next unless $flang eq $alang;
	    }
	    my $tmp = '';
	    ($curr_entry,$tmp) = ($1,$2);
	    $tmp =~ s/\s+/ /g;
	    ${$f{$curr_entry}}{$tmp} = $i+1;
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
	    $fo =~ s/\{-/{/g unless $ORACC::CBD::nominusstripping; # && warn "$fi:$li: killing {- in $fo\n";
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

# parse a .forms line and fill the arg hashref with the components
# hashref must contain 'input' field, and pp_file and pp_line must
# be set by caller
sub forms_dot_forms {
    my $data = shift;
    my $l = $$data{'input'};
    @$data{'cf','gw','pos','formsig'} = ($l =~ m/^(.*?)\s*\[(.*?)\]\s*(\S+)\t\s*(.*?)$/);
    pp_warn("(forms_dot_forms) cfgwpos parse failure in .forms style entry")
	unless $$data{'cf'} && $$data{'gw'} && $$data{'pos'};
    pp_diagnostics() && return if pp_status();
    pp_status(0);
    forms_at_form($data,$$data{'formsig'});
    pp_diagnostics() && return if pp_status();
}

# parse the part of a form that follows @form; pass a hashref to store
# the parts and the string to parse as arg2
sub forms_at_form {
    my($data,$l) = @_;
    $l =~ s/^\@form\s+//;
    $l =~ s/(\S+)//;
    $$data{'form'} = $1;
    $l =~ s/^\s*(.*?)\s*$/$1/;
    pp_warn("(forms_at_form) empty form") unless length $l;
    $l = " $l";
    while (length $l) {
	my $err_l = $l;
	$l =~ s/^\s+(.)(\S+)//;
	my($char,$value) = ($1,$2);
	if ($char) {
	    my $key = $formcharkeys{$char};
	    if ($key) {
		if ($char eq '#' && ($value =~ s/^#//)) {
		    $char = '##';
		    $key = 'morph2';
		}
		## FIXME: NEED TO HANDLE COF NORMS HERE
		$$data{$key} = $value;
	    } else {
		pp_warn("(forms_at_form) unknown form sig key char '$char' at \"$err_l\" in \"$$data{'formsig'}\"");
	    }
	} else {
	    warn "exiting with l = $l\n";
	}
    }
}

1;
