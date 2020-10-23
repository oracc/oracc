#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; 
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use Data::Dumper;
use Getopt::Long;
use lib "$ENV{'ORACC_BUILDS'}/lib";
use ORACC::CBD::FBM;
use ORACC::CBD::Forms;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Validate;

my $verbose = 1;

my $file = $ARGV[0];
$file = '-' unless $file;

fbm_init();
while (<>) {
    chomp;
    do_fbm($file,$.,$_);
}
fbm_term();

#########################################################################

sub do_fbm {
    return if $_[2] =~ /^\s*\#/;
    my %data = ();
    @data{'file','line','input'} = @_;
    pp_file($data{'file'});
    pp_line($data{'line'});
    # .forms file style:
    if ($data{'input'} =~ /^.*?\t\s*\@form\s+/) {
	forms_dot_forms(\%data);
	fbm_base_in_form(\%data);
	print Dumper \%data if $verbose;
	fbm_morph_check(\%data);
	pp_diagnostics() if pp_status();
	pp_status(0);
    } else {
	warn "$data{'file'}:$data{'line'}: no TAB in line\n";
    }
}

1;
