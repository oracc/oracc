package ORACC::Expand2;

require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/expand expandb expand_in_project expand_no_project corpus_html/;

use warnings; use strict;
use ORACC::Base;

my %known_type = ();
@known_type{ qw/xsl atf xtf xtl xlf xce ail xil png psd jpg gif 
		 xmd html xim xpf
		 htm sxh txh xht xrf xsb xst xph 
		 apt rxr xcl xff lbl xsf txt/ } = ();

my %pqdir_types = ();
@pqdir_types{ qw/atf xtf xtl xlf xce xmd sxh txh xrf xsb xpf xph apt rxr 
		  xst xim xcl xff lbl xsf txt psd/ } = ();

my $treatment = '';

sub
expand_in_project {
    $treatment = shift;
    my($id,$project) = @_;
    unless ($project) {
	$project = $id;
	$project =~ s/:.*$//;
	$id =~ s/^.*?://;
    }
    expand($id,$project);
}

sub
expand_no_project {
    $treatment = '';
    expand(@_);
}

sub
expandb {
    my $base = shift;
    $base .= '/' unless $base =~ m,/$,;
    my $savedB = $ORACC::Base::base;
    $ORACC::Base::base = $base;
    my $ret = expand(@_);
    $ORACC::Base::base = $savedB;
    $ret;
}

sub
expand {
    my $file = shift;
    my $project = shift || $treatment;
    my $ext = $file;
    $ext = ''
	unless ($ext =~ s/^.*?\.([^\.]+)$/$1/);

    if (length $ext) {
	if ($file =~ /^www\//) {
	    oracc_file($file);
	} elsif (exists $known_type{$ext}) {
	    if (exists $pqdir_types{$ext}) {
		pq_file($project,$file);
	    } elsif ($ext eq 'html' || $ext eq 'htm') {
		pq_html($project,$file);
	    } elsif ($ext eq 'xil' || $file =~ /^l[a-zA-Z0-9]{6,}$/) {
		list_file($file);
	    } elsif ($ext eq 'ail' || $ext eq 'xil') {
		if ($file =~ m#[PQX]\d{6,}#) {
		    pq_file($project,$file);
		} elsif ($project) {
		    dir_file("bld/$project",$file);
		} else {
		    dir_file('corpora',$file);
		}
	    } elsif ($ext eq 'xsl') {
		dir_file('tools',$file);
	    } elsif ($ext eq 'png' || $ext eq 'jpg' || $ext eq 'gif') {
		bad("$file type '$ext' not yet handled");
		'';
	    } else {
		bad("$file type '$ext' not handled");
		'';
	    }
	} else {
	    bad("$file has unknown type '$ext'");
	    '';
	}
     } else {
	 if ($file =~ m#(^|/)[PQX]\d{6,}#) {
	     pq_dir($project,$file);
	 } elsif ($file =~ /^l[A-Za-z0-9]{6}$/) {
	     list_file('lists', "$file/$file.xil");
	 } else {
	     dir_file('corpora',$file);
	 }
     }
}
sub
pq_file {
    my ($proj,$file) = @_;
    my $basefile = $file;
    $basefile =~ s,^.*?[\\/]([^\\/]+)$,$1,;
    my $basedir = $basefile;
    $basedir =~ s/\.[^.]+$//;
    $basedir =~ /^(.)/;
    my $dir = $1;
    my $dest = $basedir;
    $dest =~ s/^(....).*$/$1/;
    $dest = "${ORACC::Base::base}bld/$proj/$dest/$basedir/";
#    my $projdir = set_projdir($treatment,$proj);
#    if ($projdir) {
#	$basefile = "$projdir/$basefile";
#    }
#    if ($treatment) {
#	$basefile = "$treatment/$basefile";
#    } elsif (length $proj) {
#	$basefile = "$proj/$basefile";
#    }
    "$dest$basefile";
}
sub
pq_html {
    my $file = pq_file(@_);
    $file =~ s,/([^/]+)$,/html/$1,;
    $file;
}
sub
pq_dir {
    my ($proj,$file) = @_;
    my $basefile = $file;
    $basefile =~ s,^.*?[\\/]([^\\/]+)$,$1,;
    my $basedir = $basefile;
    $basedir =~ s/\.[^\.]+$//;
    $basedir =~ /^(.)/;
    my $dir = $1;
    my $dest = $basedir;
    $dest =~ s/^(....).*$/$1/;
    $dest = "${ORACC::Base::base}bld/$proj/$dest/$basedir/";
    $dest =~ s,/$,,;
#    if ($proj) {
#	$dest .= "/$proj";
#    } elsif ($treatment) {
#	$dest .= "/$treatment";
#    }
    $dest;
}
sub
oracc_file {
    my ($file) = @_;
    my $dest = "$ORACC::Base::base$file";
    $dest;
}
sub
dir_file {
    my ($dir,$file) = @_;
    my $basefile = $file;
    $basefile =~ s,^.*?[\\/]([^\\/]+)$,$1,;
    my $dest = $ORACC::Base::base.$dir.'/';
    "$dest$basefile";
}
sub
list_file {
    my ($list,$file) = @_;
    $file = '' unless $file;
    $list .= '/' if length $file;
    $list =~ s/${ORACC::Base::base}lists//;
    if ($ORACC::Apply::project) {
	$list = "$ORACC::Apply::project/00lib/$list";
    } elsif ($list !~ m,^lists/,) {
	$list = "00lib/lists/$list";
    }
    "${ORACC::Base::base}$list$file";
}
sub
corpus_html {
    my $x = expand("$_[0].xil");
    $x =~ s/([^\/]+)\.xil$/html\/${1}.html/;
    $x;
}

sub
set_projdir {
    my @tries = @_;
    my $ret = undef;
    unless ($tries[0]) {
	shift @tries;
	shift @tries unless $tries[0];
    }
    if ($tries[0]) {
	$ret = $tries[0];
    }
    $ret;
}

sub
bad {
    print STDERR 'ORACC::Expand: ', @_, "\n";
}
1;
