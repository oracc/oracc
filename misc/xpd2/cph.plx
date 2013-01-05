#!/usr/bin/perl
use warnings; use strict;
use Cwd;
use File::Spec::Functions qw/catfile splitdir/;
use Data::Dumper;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XPD::Util;

print Dumper compute_project_hierarchy();

sub
compute_project_hierarchy {
    my $project_dir = getcwd();
    my @project_hier = splitdir($project_dir);
    my @superproj = ();
    my @subproj = ();
    my @configs = ();

    my @p_tmp = @project_hier;
    do {
	pop @p_tmp;
	my $c = catfile(@p_tmp, '00lib', 'config.xml');
	push @superproj, $c if -r $c;
    } while ($#p_tmp > 0);

    @subproj = grep !/^00lib/, `find * -name 'config.xml'`; chomp(@subproj);

    foreach my $p (@superproj) {
	ORACC::XPD::Util::load_xpd($p);
	if (ORACC::XPD::Util::option('public') eq 'yes') {
	    push @configs, [ 'super' , $p , 
			     ORACC::XPD::Util::option('project') , 
			     ORACC::XPD::Util::option('abbrev') ];
	}
	ORACC::XPD::Util::unload();
    }

    foreach my $p (@subproj) {
	ORACC::XPD::Util::load_xpd($p);
	my $public = ORACC::XPD::Util::option('public') || '';
	if ($public eq 'yes') {
	    push @configs, [ 'sub' , $p , 
			     ORACC::XPD::Util::option('project') , 
			     ORACC::XPD::Util::option('abbrev') ,
	    ];
	}
	ORACC::XPD::Util::unload();
    }
    @configs;
}
