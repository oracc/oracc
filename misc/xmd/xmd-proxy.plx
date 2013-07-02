#!/usr/bin/perl
use warnings; use strict;
use lib "$ENV{'ORACC'}/lib";
use ORACC::Expand2;
use ORACC::XML;

# If there is a proxy.lst create 01bld/cat/proxy-*.xml by finding
# a .xmd file for each entry in proxy.lst.  Skip entries where
# the xmd_project = project

my $project = `oraccopt`;
my $status = 0;
my @xmdfiles;

die "xmd-proxy.plx: must be run from a project directory\n"
    unless $project;

if (-r '01bld/lists/proxy-cat.lst' && !-z '01bld/lists/proxy-cat.lst') {
    open(P, '01bld/lists/proxy-cat.lst');
    @xmdfiles = (<P>); chomp @xmdfiles;
    close(P);
}

xmd_from_list(@xmdfiles)
    if $#xmdfiles >= 0;

if ($status) {
    system 'touch', '01bld/cancel';
    exit(1);
} else {
    exit(0);
}

#################################################################

sub
xmd_from_list {
    my @lst = @_;
    my $n_p = 0;
    my $n_q = 0;
    my $n_x = 0;
    my $pcat = '01bld/cat/proxy-p.xml';
    my $qcat = '01bld/cat/proxy-q.xml';
    my $xcat = '01bld/cat/proxy-x.xml';
    open(P,">$pcat") || die "xmdmanager: unable to open `$pcat'\n";
    open(Q,">$qcat") || die "xmdmanager: unable to open `$qcat'\n";
    open(X,">$xcat") || die "xmdmanager: unable to open `$xcat'\n";
    print P xmldecl();
    print Q xmldecl();
    print X xmldecl();
    print P '<catalog xmlns="http://oracc.org/ns/xmd/1.0">';
    print Q '<catalog xmlns="http://oracc.org/ns/xmd/1.0">';
    print X '<catalog xmlns="http://oracc.org/ns/xmd/1.0">';
    local($/) = undef;
    my $proj = '';
    foreach my $f (@lst) {
	$f =~ tr/\cM//d;
	next unless $f;
	my $xmd = '';
	my ($xtf_project,$proxyid,$xmd_project) = ($f =~ /^(.*?):(.*?)\@(.*?)$/);
	
	#
	# in cat proxy, we only care about the @project part of the line
	# 
	next unless $xmd_project && $xmd_project ne $project;

	$xmd = expand_in_project(undef,"$proxyid.xmd",$xmd_project);
	warn "xmd_proxy.plx: no catalogue information for $proxyid found in $xmd_project\n" 
	    and next unless -r $xmd;
	open(XMD,$xmd);
	$_ = <XMD>;
	close(XMD);
	s/^\<\?xml.*?>\s*\n//;
	if (/\?xml/) {
	    warn("$xmd: leftover XML decl\n");
	}
	s/^\<xmd.*?<cat>/<record project=\"$xmd_project\">/;
#	s,<images>.*?</images>,,;
	s,</cat></xmd>,</record>,;
	if ($proxyid =~ /^P/) {
	    tr/\n\r/  /; ### TEMPORARY UNTIL FXR/XMD IMPORT STRIPS THESE
	    print P $_;
	    ++$n_p;
	} elsif ($proxyid =~ /^Q/) {
	    print Q $_;
	    ++$n_q;
	} else {
	    print X $_;
	    ++$n_x;
	}
    }
    print P "</catalog>"; close(P);
    print Q "</catalog>"; close(Q);
    print X "</catalog>"; close(X);
    unlink $pcat unless $n_p;
    unlink $qcat unless $n_q;
    unlink $xcat unless $n_x;
}

1;
