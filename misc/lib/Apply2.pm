package ORACC::Apply2;
use lib '@@ORACC@@/lib';
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/apply_make apply_to_text apply_to_textlist set_base/;

use warnings; use strict;
use Cwd;
use XML::LibXML;
use ORACC::Expand2;
use ORACC::Base;

$ORACC::Apply::base = $ORACC::Base::base;
$ORACC::Apply::chdir = 1;
$ORACC::Apply::check = 1;
$ORACC::Apply::dryrun = 0;
@ORACC::Apply::extensions = ();
$ORACC::Apply::project = '';
$ORACC::Apply::verbose = 1;

my $pwd = getcwd();

sub
apply_to_textlist {
    my ($textlist,@cmds) = @_;
    $textlist .= '.xil' unless $textlist =~ /\.xil$/;
    my $textlist_file = expand($textlist, $ORACC::Apply::project);
    if (check_file($textlist_file)) {
	foreach my $pq (xtl_files($textlist_file)) {
	    if ($#ORACC::Apply::extensions >= 0) {
		foreach my $e (@ORACC::Apply::extensions) {
		    $e =~ s/^\.//;
		    my $pq_dir = expand("$pq.$e", $ORACC::Apply::project);
		    unless (-e $pq_dir) {
			$pq_dir = expand("$pq.$e");
		    }
		    do_to($pq_dir, @cmds);
		}
	    } else {
		my $pq_dir = expand($pq, $ORACC::Apply::project);
		do_to($pq_dir, @cmds);
	    }
	}
    }
}
sub
apply_to_text {
    my($text,@cmds) = @_;
    if ($#ORACC::Apply::extensions >= 0) {
	foreach my $e (@ORACC::Apply::extensions) {
	    $e =~ s/^\.?/./;
	    my $etext = expand("$text$e", $ORACC::Apply::project);
	    do_to($etext,@cmds);
	}
    } else {
	my $etext = expand("$text", $ORACC::Apply::project);
	do_to($etext,@cmds);
    }
}
sub
apply_make {
    my ($pq,@m) = @_;
    $pq =~ /^([PQ]\d+[a-z]*)$/; $pq = $1;
#    message(@m)
#	if $ORACC::Apply::verbose || $ORACC::Apply::dryrun;
    $ORACC::Apply::check = 0;
    $pq =~ s/^.*?([PQ]\d{6,}).*$/$1/;
    my ($mk_dir) = ($pq =~ /([PQ]\d{6,})/);
#    $mk_dir =~ s/^PQ=//;
    $mk_dir = expand($mk_dir);

    if ($#ORACC::Apply::extensions >= 0) {
	foreach my $e (@ORACC::Apply::extensions) {
	    push @m, "$pq.$e";
	}
    }
    my $m = join(' ', @m);
    print STDERR "cd $mk_dir; $m\n";
    do_to($mk_dir, @m);
#    system (@_)
#	unless $ORACC::Apply::dryrun;
}
sub
do_to {
    my ($file,@cmds) = @_;
#    print STDERR "file=$file; cmds[0] = $cmds[0]\n";
    if (check_file($file)) {
	$file =~ /^([a-z0-9_\/.-]+)$/i; $file = $1;
	my %path = split_file($file);
	if ($#cmds < 0) {
#	    if (@ORACC::Apply::extensions >= 0) {
#		foreach my $e (@ORACC::Apply::extensions) {
#		    print "$file.$e\n";
#		    my $epath = $file;
#		    $epath = 0;
#		}
#	    } else {
		print $file, "\n";
#	    }
	} else {
	    if ($ORACC::Apply::chdir || $cmds[0] eq 'make') {
		message("cd $path{'dir'}")
		    if $ORACC::Apply::verbose || $ORACC::Apply::dryrun;
		chdir($path{'dir'}) || die("couldn't change directory to $path{'dir'}\n")
		    unless $ORACC::Apply::dryrun;
		if ($cmds[0] eq 'make') {
		    dispatch(undef,\%path,@cmds);
#		    if (defined $cmds[4]) { # make has a target
#			message(@cmds)
#			    if $ORACC::Apply::verbose || $ORACC::Apply::dryrun;
#			system (@cmds)
#			    unless $ORACC::Apply::dryrun;
#		    } else {
#			# this assumes PQ is set in @cmds
#			message(@cmds)
#			    if $ORACC::Apply::verbose || $ORACC::Apply::dryrun;
#			system (@cmds)
#			    unless $ORACC::Apply::dryrun;
#		    }
#		}
		} else {
		    dispatch($file,\%path,@cmds);
		}
		$pwd =~ /^([a-z0-9_\/.-]+)$/i; $pwd = $1;
		chdir($pwd);
	    } else {
		dispatch($file,\%path,@cmds);
	    }
	}
    }
}

sub
dispatch {
    my($file,$path,@cmds) = @_;
#    print STDERR "file=$file; cmds[0] = $cmds[0]\n";
    if (ref($cmds[0]) eq 'HASH') {
	my %call = %{$cmds[0]};
	$call{'ApplyPath'} = $path;
	if ($call{'func'}) {
	    &{$call{'func'}}(\%call);
	} else {
	    die "ORACC::Apply::dispatch: no func in call hash\n";
	}
    } else {
	if ($file) {
	    message(@cmds, $file)
		if $ORACC::Apply::verbose || $ORACC::Apply::dryrun;
	    system (@cmds, $file)
		unless $ORACC::Apply::dryrun;
	} else {
	    message(@cmds)
		if $ORACC::Apply::verbose || $ORACC::Apply::dryrun;
	    system (@cmds)
		unless $ORACC::Apply::dryrun;
	}
    }
}

sub
check_file {
    return 1 unless $ORACC::Apply::check;
    my $f = shift;
    if (!-e $f) {
	bad("file '$f' not found");
	0;
    } elsif (!-r _) {
	bad("file '$f' not readable");
	0;
    } else {
	1;
    }
}
sub
split_file {
    my $file = shift;
    my %tmp = ();

    if (-d $file) {
	$tmp{'dir'} = $file;
	if ($file =~ m,/,) {
	    $file =~ /([PQ]\d{6,})/;
	    $tmp{'basename'} = $tmp{'text'} = $1;
	} else {	    
	    $tmp{'dir'} = '';
	    $tmp{'basename'} = $file;
	    $tmp{'text'} = $file;
	}
    } else {
	if ($file =~ m,/,) {
	    $file =~ m#^(.*?)/([^/]+)$#;
	    @tmp{'dir','basename'} = ($1,$2);
	    $tmp{'text'} = $2;
	} else {
	    $tmp{'dir'} = '';
	    $tmp{'basename'} = $file;
	    $tmp{'text'} = $file;
	}
	if ($tmp{'basename'} =~ /\.(.*)$/) {
	    $tmp{'ext'} = $1;
	}    
    }

    %tmp;
}
sub
create_file_name {
    my ($pref,$ext) = ();
    $$pref{'dir'}.'/'.$$pref{'basename'}.'/'.$$pref{'basename'}.'.'.$ext;
}

sub
xtl_files {
    my $xtl = shift;
    my $xtl2list = expand('xil2list.xsl');
    my @ret = ();
    my $xsl_call = "xsltproc -xinclude $xtl2list $xtl";
    if ($PSD::Apply::verbose) {
	message($xsl_call);
    }
    open(IN,"$xsl_call |");
    while (<IN>) {
	chomp;
	push @ret, $_;
    }
    close(IN);
    @ret;
}
sub
bad {
    print STDERR "ORACC::Apply: ", @_, "\n";
}
sub
message {
    print STDERR join(' ', @_), "\n";
}
1;
