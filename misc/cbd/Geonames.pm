package ORACC::CBD::Geonames;

require Exporter;
@ISA=qw/Exporter/;

@EXPORT = qw/geonames/;

use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::XMD::Pleiades;
use ORACC::CBD::PPWarn;

my @pleiades_data = ();

sub geonames {
    my($cbdgeo,$geos,@cbd) = @_;
    if ($#$geos >= 0) {
	pleiades_init();
	foreach my $g (@$geos) {
	    my $cfgwpos = $cbd[$g];
	    $cfgwpos =~ s/^.*?\s+(\S+)\s+\[(.*?)\]\s+(\S+)\s*$/$1\[$2\]$3/;
	    my $pl_id = ORACC::XMD::Pleiades::pleiades_id('',
							  $cfgwpos, 
							  @pleiades_data);
	    if ($pl_id =~ /^0$/) {
		if ($cbdgeo eq 'yes') {
		    pp_line($g);
		    pp_warn("no geonames data for $cfgwpos");
		}
	    } else {
		$cbd[$g] .= "\n\@pl_id $pl_id";
	    }
	}
    }

    @cbd;
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
}

1;
