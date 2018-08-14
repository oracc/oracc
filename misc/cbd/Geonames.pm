package ORACC::CBD::Geonames;

require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/geonames/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::XMD::Pleiades;
use ORACC::CBD::PPWarn;

my @pleiades_data = ();
my %geonames = ();
my %geonames_loaded = ();
#my @geo_keys = qw/cfgwpos cf alias id coord uid loctype/;
my @geo_keys = qw/cfgwpos cf alias id coord loctype/;
my %geo_keys = (); @geo_keys{@geo_keys} = ();

sub geonames {
    my($cbdgeo,$geos,@cbd) = @_;
    if ($#$geos >= 0) {
	pleiades_init();
	# geonames_load('geonames');
	# geonames_load('.');
	geonames_init();
	foreach my $g (@$geos) {
	    my $cfgwpos = $cbd[$g];
	    $cfgwpos =~ s/^.*?\s+(\S+)\s+\[(.*?)\]\s+(\S+)\s*$/$1\[$2\]$3/;
	    my %geo_tags = get_tags($g,@cbd);
	    $geo_tags{'o'} = $cfgwpos;
	    geonames_augment(\%geo_tags);
#	    my $pl_id = ORACC::XMD::Pleiades::pleiades_id('',
#							  $cfgwpos, 
#							  @pleiades_data);
#	    if ($pl_id =~ /^0$/) {
#		if ($cbdgeo eq 'yes') {
#		    pp_line($g);
#		    pp_warn("no geonames data for $cfgwpos");
#		}
#	    } else {
#		my %pl_data = geonames_data($pl_id);
#		foreach my $p (keys %pl_data) {
#		    $geo_tags{$p} = $pl_data{$p}
#		    unless defined $pl_data{$p};
#		}
	    $cbd[$g] .= "\n".geonames_cbd_data(%geo_tags);
	    geonames_save_data(%geo_tags);
	}
	geonames_term();
    }

    @cbd;
}

sub get_tags {
    my($i,@cbd) = @_;
    my %tags = ();
    while ($cbd[$i] !~ /^\@end\s+entry/) {
	if ($cbd[$i] =~ /^\@([a-z0-9_]+)\s/) {
	    my $t = $1;
	    if (exists $geo_keys{$t}) {
		my($v) = ($cbd[$i] =~ /\@(\S+)\s+(.*)\s*$/);
		$t =~ s/^\@//;
		$tags{$t} = $v;
		$cbd[$i] = "\000";
	    } elsif ($t eq 'prop') {
		my($k) = ($cbd[$i] =~ /\s(\S+)\s/);
		if ($k && $k eq 'loctype') {
		    $tags{"prop/$k"} = $cbd[$i];
		    $cbd[$i] = "\000";
		}
	    }
	}
	++$i;
    }
    (%tags);
}

sub geonames_init {
    open(GEO,'>01bld/geonames.tab');
}
sub geonames_save {
    my %data = @_;
    my @v = ();
    foreach my $k (@geo_keys) {
	push @v, $data{$k} || '';
    }
    print GEO join("\t",@v), "\n";
}
sub geonames_term {
    close(GEO);
}
sub geonames_load {
    my $argproj = shift;
    my %data = ();
    if ($argproj eq '.') {
	geonames_load_project('00lib/geonames',\%data);
    } else {
	geonames_load_project("$ENV{'ORACC_BUILDS'}/$argproj/00lib/geonames",
			      \%data);
    }
}
sub geonames_load_project {
    my($geobase,$data) = @_;
    return if $geonames_loaded{$geobase}++;
    if (-r "$geobase.lst") {
	open(L,"$geobase.lst");
	while (<L>) {
	    chomp;
	    my @p = split(/\s+/,$_);
	    foreach my $p (@p) {
		geonames_load_project($p,$data);
	    }
	}
	close(L);
    }
    if (-r "$geobase.tab") {
	open(T,"$geobase.tab");
	while (<T>) {
	    chomp;
	    my($o) = (/^(\S+)\t/);
	    $$data{$o} = $_; # later .tabs overwrite earlier ones
	}
	close(T);
    }
}
sub geonames_augment {
    my $data = shift;
    if ($$data{'cfgwpos'}) {
	if ($geonames{$$data{'cfgwpos'}}) {
	    my %a = ();
	    my @a = split(/\t/,$geonames{$$data{'cfgwpos'}});
	    @a{@geo_keys} = @a;
	    foreach my $k (@geo_keys) {
		$$data{$k} = $a{$k} unless defined $$data{$k};
	    }
	}
    } else {
	warn "Geonames.pm: internal error: no cfgwpos in data\n";
    }
}
sub geonames_cbd_data {
    my $data = shift;
    my @c = ();
    foreach my $k ($geo_keys[1..$#geo_keys]) {
	if ($k eq 'loctype') {
	    push @c, "\@prop loctype $$data{$k}";
	} else {
	    push @c, "\@$k $$data{$k}";
	}
    }
    join("\n", @c);
}

# sub find {
#     $pleiades_id = ORACC::XMD::Pleiades::pleiades_id($$r[$item_index],
# 						     $f, @pleiades_data);
#     if ($pleiades_id) {
# 	$coord = ORACC::XMD::Pleiades::coords($pleiades_id, @pleiades_data);
# 	push @{$xmd_outputs{$curr_xmd}}, 
# 	    "<pleiades_id>$pleiades_id</pleiades_id>";
# 	if ($coord) {
# 	    push @{$xmd_outputs{$curr_xmd}}, 
# 		"<pleiades_coord>$coord</pleiades_coord>";
# 	}
# 	xmd_geojson($fields, $r, $pleiades_id, $coord) 
# 	    if $pleiades_id && $coord;
#     }
# }

sub pleiades_init {
    my $proj = `oraccopt`;
    my $p = ORACC::XMD::Pleiades::load('00lib/pleiades.tab');
    push(@pleiades_data, $p) if $p;
    unless ($proj eq 'geonames') {
        my $p = ORACC::XMD::Pleiades::load("$ENV{'ORACC'}/pub/geonames/pleiades.tab");
	if ($p) {
	    push(@pleiades_data, $p);
	    warn "geonames/pleiades loaded as $p\n";
	    use Data::Dumper; 
	    open(P,'>/tmp/pleiades.dump');
	    print P Dumper($p); 
	    close(P);
	}
    }
    foreach $p (@pleiades_data) {
	foreach my $pk (keys %$p) {
	    my $pkn = $$p{$pk};
	    if ($pkn) {
		$pkn =~ s/\t.*//;
		$geonames{$pkn} = $$p{$pk};
	    } else {
		warn "Geonames: no value for key $pk\n";
	    }
	}
    }
}

1;
