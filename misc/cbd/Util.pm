package ORACC::CBD::Util; require Exporter; @ISA=qw/Exporter/;

@EXPORT = qw/pp_args pp_cbd pp_load pp_entry_of pp_sense_of
    header_vals setup_args setup_cbd cbdname cbdname_from_fn project
    lang name projdir file_index errfile pp_tags set_acd_rx
    set_default_base pp_reinit
    /;

use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';


# warn "hello from Util.pm\n";

use Data::Dumper;

my @data = qw/allow usage collo sense/;


%ORACC::CBD::data = (); @ORACC::CBD::data{@data} = ();

$ORACC::CBD::acd_rx = '[-+>=]?';

use ORACC::L2GLO::Langcore;
use ORACC::CBD::ATF;
use ORACC::CBD::Forms;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Sigs;
use ORACC::CBD::SuxNorm;
use ORACC::CBD::Validate;

# FIXME: this should use Unicode Norm D
my %vowel_of = (
    'Ā'=>'A',
    'Ē'=>'E',
    'Ī'=>'I',
    'Ū'=>'U',
    'Â'=>'A',
    'Ê'=>'E',
    'Î'=>'I',
    'Û'=>'U',
    );

my %file_indexes = ();

use Getopt::Long;

%ORACC::CBD::bases = ();
%ORACC::CBD::forms = ();
$ORACC::CBD::bases = 0;
$ORACC::CBD::det_minus = 0;
$ORACC::CBD::noforms = 0;
$ORACC::CBD::noletters = 0;
$ORACC::CBD::nonormify = 0;
$ORACC::CBD::nosetupargs = 0;
$ORACC::CBD::nosigs = 0;
$ORACC::CBD::novalidate = 0;
$ORACC::CBD::qpn_base_lang = 'sux';
$ORACC::CBD::nodiagnostics = 0;

my $file_index = 1;

sub pp_reinit {
    ORACC::CBD::Validate::reinit();
    ORACC::CBD::Entries::reinit();
}

sub set_acd_rx {
    return $ORACC::CBD::acd_rx;
}

sub dump_file_indexes {
    print Dumper \%file_indexes;
}

sub pp_args {
    my $cbd = shift;
    my %args = ();

    GetOptions(
	\%args,
	qw/announce apply auto bare base:s check kompounds dets dry dynamic edit entries=s file 
	filter fix:s force glo:s homograph increment:s inplace invert letters lines list:s lang:s log:s mode:s noforms
	nonormify nopsus nosigs novalid output:s project:s quiet reset sigs stdout trace+ vfields:s words=f xml/,
	) || die "unknown arg";

    $cbd = $args{'glo'} if $args{'glo'};
    if ($args{'trace'}) {
	if ($args{'trace'} == 1) {
	    $ORACC::CBD::PPWarn::trace = 1
	} elsif ($args{'trace'} == 2) {
	    $ORACC::CBD::PPWarn::edit_trace = 1;
	}
    }
    $ORACC::CBD::check_compounds = $args{'kompounds'};
    $ORACC::CBD::nodiagnostics = $args{'quiet'};
    $ORACC::CBD::noforms = $args{'noforms'};
    $ORACC::CBD::nonormify = $args{'nonormify'};
    $ORACC::CBD::novalidate = $args{'novalid'};

    # We reset args{'lang'} to header lang for output files, so we
    # stash the original in #lang for modules that special purpose the
    # -lang arg
    $args{'#lang'} = $args{'lang'};

    if ($ORACC::CBD::nosetupargs) {
	@{$args{'argv'}} = @ARGV;
    } else {
        unless ($args{'filter'}) {
	    die "cbdpp.plx: must give glossary on command line\n"
		unless setup_args(\%args, shift @ARGV || $cbd) || $ORACC::CBD::no_cbd_ok;
	} else {
	    $args{'cbd'} = '<stdin>';
	}
    }
    %args;
}

my $last_char = '';
sub cbd_dump {
    my($args, $outh, @c) = @_;
    foreach (@c) {
	next if /^\000$/;
	my $acd_rx = $ORACC::CBD::acd_rx;
	if (/^$acd_rx?\@entry\S*\s+(.*?)\s*$/ && $$args{'letters'}) {
	    my $cfgw = $1;
	    my $init_char = first_letter($cfgw);
	    if (!$last_char || $last_char ne $init_char) {
		$last_char = $init_char;
		print $outh "\@letter $last_char\n\n";
	    }
	}
	print $outh "$_\n";
    }
}

sub pp_cbd {
    my ($args,@c) = @_;
    return if pp_status() && !$$args{'force'};
    if ($$args{'filter'} || $$args{'stdout'}) {
	$last_char = '';
	cbd_dump($args, \*STDOUT, @c);
    } elsif ($$args{'output'}) {
	if ($ORACC::CBD::Forms::external) {
	    die "$0: can't use -output with external forms\n";
	} else {
	    my $outf = $$args{'output'};
	    open(CBD, ">$outf") || die "$0: unable to open $outf for output\n";
	    $last_char = '';
	    cbd_dump($args, \*CBD, @c);
	}
    } else {
	my $ldir = '01tmp';
	system 'mkdir', '-p', $ldir;
	my $outf = $ldir.'/'.lang().'.glo';
	open(CBD, ">$outf") 
	    || die "cbdpp.plx: can't write to $outf";
	if ($ORACC::CBD::Forms::external) {
	    my $cfgw = '';
	    my $forms_printed = 0;
	    foreach (@c) {
		next if /^\000$/ || /^\s*$/;
		if (/\@entry.*?\s+(.*)$/) {
		    $cfgw = $1;
		    my $init_char = first_letter($cfgw);
		    if (!$last_char || $last_char ne $init_char) {
			$last_char = $init_char;
			print CBD "\@letter $last_char\n\n";
		    }
		    $forms_printed = 0;
		} elsif (/\@sense/ && !$forms_printed) {
		    forms_print($cfgw, \*CBD);
		    ++$forms_printed;
		} elsif ($ORACC::CBD::det_minus && /^\@(?:bases|form)/) {
		    s/\{-/{/g;
		} elsif (/^\@end\s+entry/) {
		    chomp;
		    $_ .= "\n"; # double blank line after entry
		}
		print CBD "$_\n";
	    }
	} else {
	    $last_char = '';
	    cbd_dump($args, \*CBD, @c);
	}
	close(CBD);
	warn "cbdpp.plx: wrote $outf\n"
	    if $$args{'announce'};
    }
}

sub first_letter {
    my($first) = ($_[0] =~ /^(.)/);
    $first = "\U$first";
    $first = $vowel_of{$first} if $vowel_of{$first};
    $first;
}

sub pp_load {
    my ($args,$file) = @_;
    my @c = ();

    if ($file) {
	pp_file($file);
    } else {
	$file = pp_file();
    }
    
    if ($$args{'filter'}) {
	@c = (<>); chomp @c;
    } else {
	%{$ORACC::CBD::data{'files'}{$file}} = header_vals($file);
	if ($file =~ m#^00src#) {
	    # spoof args{cbd} so forms_load works
	    my $actual_args_cbd = $$args{'cbd'};
	    $$args{'cbd'} = $file;
	    ORACC::CBD::Forms::forms_load($args) unless $ORACC::CBD::noforms;
	    $$args{'cbd'} = $actual_args_cbd;
	}
	open(C,$file) || die "cbdpp.plx: unable to open $file. Stop.\n";
	@c = (<C>); chomp @c;
	close(C);
#	print Dumper $ORACC::CBD::data{'files'};
    }
#    print Dumper \%ORACC::CBD::data; exit 0;

    $c[0] =~ s/^\x{ef}\x{bb}\x{bf}//; # remove BOM
    $c[0] =~ s/^\x{feff}//; # remove BOMs
    
    my $insert = -1;
    for (my $i = 0; $i <= $#c; ++$i) {
	$c[$i] =~ s/\s+/ /g; # normalize white space
	my $acd_rx = $ORACC::CBD::acd_rx;
	pp_line($i+1);
	if ($c[$i] =~ /^$ORACC::CBD::Edit::acd_rx?\@([a-z]+)/) {
	    my $tag = $1;
	    if ($tag ne 'end') {
		if ($tag eq 'project') {
		    pp_line($i+1);
		} elsif ($tag eq 'lang') {
		    pp_line($i+1);
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
		push(@{$ORACC::CBD::data{$tag}}, $i) if exists $ORACC::CBD::data{$tag};
		$insert = $i;
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

sub header_vals {
    my ($c) = @_;
    my %h = ();
    if ($c) {
	my @p = `head -4 $c`;
	$p[0] =~ s/^\x{ef}\x{bb}\x{bf}//; # remove BOMs
	$p[0] =~ s/^\x{feff}//; # remove BOMs
#	print Dumper \@p;
	foreach my $p (@p) {
	    if ($p =~ /^\@(project|lang|name)\s+(.*?)\s*$/) {
		my($t,$v) = ($1,$2);
		$h{$t} = $v;
	    } else {
		$p = undef;
	    }
	}
    
	$h{'projdir'} = "$ENV{'ORACC_BUILDS'}/$h{'project'}";
	$file_indexes{$c} = $file_index++ unless $file_indexes{$c};
	$h{'file_index'} = $file_indexes{$c};
	
	if ($c =~ /^01tmp/) {
	    my $e = $c;
	    $e =~ s/01tmp/00lib/;
	    if (-r $e) {
		$h{'errfile'} = $e;
	    } else {
		$h{'errfile'} = $c;
	    }
	} else {
	    $h{'errfile'} = $c;
	}
    } else {
	die "$0: can't get header info from empty file name\n";
    }
#    print "header_vals $c = ", Dumper  \%h;
    %h;
}

sub header_info {
    my($file,$tag) = @_;
    return '' unless $file && $tag;
    if ($ORACC::CBD::data{'files'}{$file}) {
	my %f = %{$ORACC::CBD::data{'files'}};
	my %h = %{$f{$file}};
	if ($h{$tag}) {
	    return $h{$tag};
	} else {
	    warn "header_info no '$tag' entry in file '$file'\n";
	}
    } else {
#	warn "header_info: no 'files' entry for file '$file'\n";
    }
    undef;
}

sub cbdname {
    my $p = project(@_);
    my $l = lang(@_);
    if ($p && $l) {
	return $p.':'."${l}_$file_indexes{pp_file()}";
    } else {
	return '';
    }
}

sub cbdname_from_fn {
    my $fn = shift;
    ${$ORACC::CBD::data{'cbdmap'}}{$fn};
}

sub h_file {
    if ($_[0]) {
#	warn "h_file: _[0] = $_[0]\n";
	$_[0]
    } else {
	pp_file()
    }
}
sub errfile {    
    header_info(h_file(@_),'errfile');
}
sub file_index {
    header_info(h_file(@_),'file_index');
}
sub lang {
    header_info(h_file(@_),'lang');
}
sub name {
    header_info(h_file(@_),'name');
}
sub projdir {
    header_info(h_file(@_),'projdir');
}
sub project {
    header_info(h_file(@_),'project');
}

sub setup_args {
    my ($args,$file) = @_;
    my $tried_file = '';
    if (!$file) {
	if ($$args{'project'}) {
	    my $p = $$args{'project'};
	    my $l = '';
	    my $qpnlang = '';
	    if ($p =~ s/:(.*?)$//) {
		$l = $1;		
		if ($l =~ s#/(\S+)$##) {
		    $qpnlang = $1;
		}
	    } else {
		if ($$args{'base'}) {
		    unless ($$args{'#did_base'}) {
			if (-r $$args{'base'}) {
			    my %h = header_vals($$args{'base'});
			    if ($h{'lang'}) {
				$l = $h{'lang'};
			    } else {
				warn "$0: no \@lang in base glossary $$args{'base'}\n";
			    }
			} else {
			    warn "$0: base glossary $$args{'base'} not readable\n";
			}
			$$args{'#did_base'} = 1;
		    } else {
			$l = lang();
		    }
		} else {
		    warn "$0: no base glossary\n";
		}
	    }
	    if ($p && $l) {
		$file = "$ENV{'ORACC_BUILDS'}/$p/00lib/$l.glo";
		unless (-r $file) {
		    warn "$0: looked for $file but couldn't find it\n";
		    $tried_file = $file;
		    $file = undef;
		} else {
		    warn "$0: found $file via -project arg\n";
		}
	    } else {
		$p = '' unless $p;
		$l = '' unless $l;
		warn "$0: need project and lang but have project=$p and lang=$l\n";
	    }
	}
    }
    $$args{'#cbd_ok'} = ($file ? 1 : 0);
    $$args{'cbd'} = $file || $tried_file;
    return undef unless $file;
    my $lng = '';
    $ORACC::CBD::qpn_base_lang = 'sux'; # reset with @qpnbaselang in glossary header
    $file;
}

sub pp_clean_det_minus {
    my @n = ();
    foreach (@_) {
	if (/^\@(?:bases|form)/) {
	    s/\{-/{/g;
	}
	push @n, $_;
    }
    @n;
}

sub setup_cbd {
    my($args,$glossary) = @_;
    my $file = $glossary ? $glossary : $$args{'cbd'};    
    die "cbdpp.plx: can't read glossary $file\n" unless -r $file;
    pp_file($file);
    atf_reset();
    
    my %h = header_vals($file);
    die "cbdpp.plx: $file: can't continue without project and language\n"
	    unless $h{'project'} && $h{'lang'};
    $ORACC::CBD::bases = lang_uses_base($h{'lang'});
    $ORACC::CBD::norms = lang_uses_norm($h{'lang'});
    #    warn "uses_base = $ORACC::CBD::bases\n";
    $$args{'lang'} = $h{'lang'};
    system 'mkdir', '-p', "01bld/$h{'lang'}";
    my @cbd = pp_load($args, $glossary);
    @cbd = ORACC::CBD::Validate::pp_validate($args, @cbd);
    if ($ORACC::CBD::Forms::external) {
	unless ($ORACC::CBD::noforms) {
	    $ORACC::CBD::Forms::external = 0; # so v_form will validate
	    ORACC::CBD::Forms::forms_validate();
	    if ($ORACC::CBD::bases || lang() =~ /qpn/) { # fix-me should check use base
		ORACC::CBD::Forms::forms_normify() unless $ORACC::CBD::nonormify;
	    }
	    $ORACC::CBD::Forms::external = 1;
	}
#	ORACC::CBD::Forms::forms_dump();
    } else {
	if ($ORACC::CBD::bases || lang() =~ /qpn/) {
	    @cbd = ORACC::CBD::SuxNorm::normify($$args{'cbd'}, @cbd)
		unless $ORACC::CBD::nonormify;
	}
    }
    #    sigs_from_glo($args,@cbd) unless $$args{'check'} || pp_status();
    if ($ORACC::CBD::det_minus) {
	@cbd = pp_clean_det_minus(@cbd);
	forms_det_clean() if $ORACC::CBD::Forms::external && $ORACC::CBD::det_minus;
    }
    @cbd;
}

sub pp_tags {
    my $i = 0;
    my $t = shift;
    my @t = ();
    for ($i = 0; $i <= $#_; ++$i) {
	if ($_[$i] =~ /^@([a-z]+)/) {
	    push(@t,$i) if $t eq $1;
	}
    }
    @t;
}

sub default_base_glo {
    my $glo = shift;
    if ($glo =~ /sux-x-emesal/) {
	return "$ENV{'ORACC_BUILDS'}/epsd2/emesal/00lib/sux-x-emesal.glo";
    } elsif ($glo =~ /sux/) {
	return "$ENV{'ORACC_BUILDS'}/epsd2/00src/sux.glo";
    } elsif ($glo =~ /qpn/) {
	return "$ENV{'ORACC_BUILDS'}/epsd2/names/00lib/qpn.glo";
    } else {
	return undef;
    }
}

sub set_default_base {
    my $args = shift;
    my $default_base = default_base_glo($$args{'cbd'});
    if ($default_base) {
	$$args{'base'} = $default_base;
    } else {
	warn "$0: no default base defined for glossary $$args{'cbd'}\n";
    }
}

1;
