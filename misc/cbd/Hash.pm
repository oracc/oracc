package ORACC::CBD::Hash;
require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/pp_hash/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;
use ORACC::CBD::Util;

my $field_index = 0;
my $sense_id = 0;

my $cbdid;
my $cbdlang;
my $curr_id = '';
my $curr_cf = '';
my $curr_sense_id = 0;
my $curr_sig_id = '';
my $ebang_flag = '';
my %entries = ();
my %line_of = ();
my %sense_props = ();
my %sigs = ();
my $usage_flag = 0;

sub pp_hash {
    my ($args,@cbd) = @_;
    my $defn_minus = 0;
    my %h = ();
    my %langnorms = ();
    my $last_tag = '';
    my $use_norms = 0;
    my @ee = ();

    if ($#cbd < 0) {
	@cbd = setup_cbd($args);	
	return 0 if pp_status();
    }

    my $cbdname = "$$args{'project'}\:$$args{'lang'}";
    my %entries = %{${$ORACC::CBD::data{"$cbdname"}}{'entries'}};

    $cbdlang = $$args{'lang'};
    $cbdid = $cbdlang;
    $cbdid =~ tr/-/_/;
    my $currtag = undef;
    my $currarg = undef;

    $use_norms = $langnorms{$$args{'lang'}}; ## langcore
    pp_file($$args{'file'});

    my %e = ();

    for (my $i = 0; $i <= $#cbd; ++$i) {

	next if $cbd[$i] =~ /^[#\000]/;

	pp_line($i+1);
	local($_) = $cbd[$i];

	if (/^\@(project|name|lang)\s+(.*?)\s*$/) {
	    my($key,$val) = ($1,$2);
	    $h{$key} = $val;
	    next;
	}

	next if /^\#/ || /^\@letter/;

	if (/^\@([a-z_]+[-*!]*)\s+(.*?)\s*$/) {
	    ($currtag,$currarg) = ($1,$2);
	    my $default = $currtag =~ s/!//;
	    my $starred = $currtag =~ s/\*//;
	    my $linetag = $currtag;
	    $linetag =~ s/\*$//;

	    if ($last_tag eq $currtag) {
		++$field_index;
	    } else {
		$field_index = 0;
		$last_tag = $currtag;
	    }

	    $line_of{$linetag} = pp_line()-1
		unless defined $line_of{$linetag};
	    if ($currtag =~ /^entry/) {
		$ebang_flag = $default || '';
		$usage_flag = $starred;
		$currarg =~ /^(\S+)/;
		$curr_cf = $1;
		$currarg =~ s/^\s+//; $currarg =~ s/\s*$//;
		$curr_id = $entries{$currarg};
		unless ($curr_id) {
		    pp_warn("weird; no entries_index entry for `$currarg' (this can't happen)\n");
		    $curr_id = '';
		}
		$line_of{'entry'} = pp_line()-1;
	    } elsif ($currtag =~ /^defn/) {
		$defn_minus = ($currtag =~ s/-$//);
	    }
	    if ($currtag eq 'end') {
		if ($currarg eq 'entry') {
		    %{$entries{$curr_id,'e'}} = %e;
		    %{$entries{$curr_id,'l'}} = %line_of;
		    push @ee, $curr_id;
		    %e = ();
		} else {
		    pp_warn("malformed end tag: \@end $currarg");
		}
		%line_of = ();
	    } else {
		if ($currtag eq 'sense') {
		    my($tok1) = ($currarg =~ /^(\S+)/);
		    $curr_sense_id = sprintf("\#%06d",$sense_id++);
		    my $defbang = ($default ? '!' : '');
		    $currarg = "$curr_sense_id$defbang\t$currarg";
		} elsif ($currtag eq 'form') {
		    my $barecheck = $currarg;
		    $barecheck =~ s/^(\S+)\s*//;
		    my $formform = $1;
		    my $tmp = $currarg;
		    $tmp =~ s#\s/(\S+)##; # remove BASE because it may contain '$'s.
		    $tmp =~ s/^\S+\s+//; # remove FORM because it may contain '$'s.
		    if ($currarg =~ s/^\s*<(.*?)>\s+//) {
			push @{$sigs{$curr_sense_id}}, $1;
		    }
		    if ($default || $ebang_flag) {
			$currarg = "!$currarg";
		    }
		} elsif ($currtag eq 'bff') {
		}
		if ($currtag eq 'prop' && $curr_sense_id) {
		    push @{$sense_props{$curr_sense_id}}, $currarg;
		} else {
		    push @{$e{$currtag}}, $currarg;
#			unless $currtag eq 'inote';
		}
	    }
	} elsif (/^\@([A-Z]+)\s+(.*?)\s*$/) {
	    ${$e{'rws_cfs'}}{$1} = $2;
	} else {
	    chomp;
	    pp_warn("(hash) syntax error near '$_'") if /\S/;
	}
    }
    ${${$ORACC::CBD::data{$cbdname}}{'cbdname'}} = $cbdname;
    %{${$ORACC::CBD::data{$cbdname}}{'header'}} = %h;
    @{${$ORACC::CBD::data{$cbdname}}{'ids'}} = @ee;
    %{${$ORACC::CBD::data{$cbdname}}{'entries'}} = %entries;
    1;
}

1;
