#!/usr/bin/perl
use warnings; use strict;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XPD::Util;
use ORACC::XML;
use Cwd;
use File::Spec::Functions qw/catfile splitdir/;

my $project = `oraccopt`;
my $verbose = 0;

if ($project) {
    warn "Collecting project data for $project ...\n";
} else {
    my $cwd = `pwd`;
    chomp $cwd;
    warn "No project data found in $cwd ... skipping\n";
    exit 1;
}

my %longlang = (
    akk=>'Akkadian',
    'akk-x-conakk'=>'Conventional Akkadian',
    'akk-x-stdbab'=>'Standard Babylonian',
    'akk-x-earakk'=>'Early Akkadian',
    'akk-x-oldakk'=>'Old Akkadian',
    'akk-x-oldass'=>'Old Assyrian',
    'akk-x-oldbab'=>'Old Babylonian',
    'akk-x-midass'=>'Middle Assyrian',
    'akk-x-midbab'=>'Middle Babylonian',
    'akk-x-obperi'=>'OB Peripheral',
    'akk-x-mbperi'=>'MB Peripheral',
    'akk-x-neoass'=>'Neo-Assyrian',
    'akk-x-neobab'=>'Neo-Babylonian',
    'akk-x-ltebab'=>'Late Babylonian',
    arc=>'Aramaic',
    elx=>'Elamite',
    en=>'English',
    hun=>'Hungarian',
    peo=>'Old Persian',
    qpn=>'Proper Nouns',
    sux=>'Sumerian',
    'sux-x-emesal'=>'Emesal Sumerian',
    uga=>'Ugaritic',
    xhu=>'Hurrian',
    'qpn-x-celest'=>'Celestial Names',
    'qpn-x-divine'=>'Divine Names',
    'qpn-x-ethnic'=>'Ethnic Names',
    'qpn-x-lineage'=>'Lineage Names',
    'qpn-x-months'=>'Month Names',
    'qpn-x-object'=>'Object Names',
    'qpn-x-places'=>'Places',
    'qpn-x-people'=>'People',
    'qpn-x-temple'=>'Temples',
    'qpn-x-waters'=>'Watercourses',
    'qpn-x-ynames'=>'Year Names',
    none=>'none',
    );

my $stats_xml_frag = `project-stats.plx`;

### Compute glossary languages
my @langs = `glo-lang.sh`;
chomp @langs;
@langs = grep(length,@langs);

my %top_l = ();
@top_l{map { /^(...)/ && $1 } @langs} = ();
my @top_l = sort keys %top_l;

### Compute languages used in translations
my @translangs = ();
if (-d '00atf') {
    @translangs = `find 00atf -type f -print0 | xargs -0 grep -h '\@translation' | cut -d' ' -f3 |sort -u`;
    chomp @translangs;
}

#@translangs = ('en') unless $#translangs >= 0;

push @translangs, 'none';

##
## Also:
##
##  outline-default-heading-format : e.g., vol_no: chap_no
##  outline-listall-heading-format
##
## and
##
##  outline-use-default-select -- false = don't use the select combo in default mode
##  outline-use-listall-select -- false = don't use the select combo in listall mode
##

my @system_select1 = compute_select1('default');
my @custom_select1 = compute_select1('special');

my @pll_projects = compute_project_hierarchy();
my @pll_links = ();
my @pll_lists = ();
my $xpll = undef;
my $xpll_link_ids = load_xml("$ENV{'ORACC'}/lib/data/pll_linkids.xml");
warn "p3-project-data.plx: failed to load $ENV{'ORACC'}/lib/data/pll_linkids.xml"
    unless $xpll_link_ids;
if (($xpll = load_xml("00lib/pll.xml"))) {
    @pll_links = tags($xpll,undef,'link');
    @pll_lists = tags($xpll,undef,'list');
}

my $pgfile = "02xml/project-data.xml";

open(O,">$pgfile"); select O;

#print "<project-data xmlns:ev=\"http://www.w3.org/2001/xml-events\">";

print "<project-data project=\"$project\">";

print $stats_xml_frag;

print '<translangs>';
foreach my $tl (@translangs) {
    if ($longlang{$tl}) {
	print "<lang abbrev=\"$tl\">$longlang{$tl}</lang>";
    } else {
	warn "p3-project-data.plx: no longlang for $tl\n";
    }
}
print '</translangs>';

@top_l = sort @top_l;
print '<glossaries>';
for (my $i = 0; $i <= $#top_l; ++$i) {
    my $top_l = $top_l[$i];
    my @top_l = grep(/^$top_l$/, @langs);
    my @sub_l = grep(/^$top_l-/, @langs);
    unless ($top_l eq 'qcu') {
	print '<glogroup>';
	if ($#top_l == 0) {
	    if ($longlang{$top_l}) {
		print "<glossary abbrev=\"$top_l\">$longlang{$top_l}</glossary>";
	    } else {
		print "<glossary abbrev=\"$top_l\">$top_l</glossary>";
	    }
	}
	foreach my $sub (sort @sub_l) {
	    if ($longlang{$sub}) {
		print "<glossary abbrev=\"$sub\">$longlang{$sub}</glossary>";
	    } else {
		warn "p3-project-data.plx: no longlang for $sub\n";
		print "<glossary abbrev=\"$sub\">$sub</glossary>";
	    }
	}
	print '</glogroup>';
    }
}
print '</glossaries>';

print '<outline-sorters>', @system_select1, @custom_select1, '</outline-sorters>';

print '<projects>', map ({ pll_project($_) }  @pll_projects), '</projects>';
print '<links>',    map ({ pll_link_ids($_) } @pll_links),    '</links>';
print '<lists>',    map ({ $_->toString(0) }  @pll_lists),    '</lists>';

print '</project-data>';

close(O);

system 'chmod', 'o+r', $pgfile;

######################################################################################

sub
compute_project_hierarchy {
    my $project_dir = getcwd();
    my @project_hier = splitdir($project_dir);
    my @superproj = ();
    my @sibproj = ();
    my @subproj = ();
    my @configs = ();

    my @p_tmp = @project_hier;
    my @super_parts = @p_tmp;
    pop @super_parts;
    my $super_path = catfile(@super_parts);
    my @project_parts = splitdir($project);
    my $project_base = $project_parts[$#project_parts];
    do {
	pop @p_tmp;
	my $c = catfile(@p_tmp, '02xml', 'config.xml');
	push @superproj, $c if -r $c;
    } while ($#p_tmp > 0);

    my @subpaths = grep(-d, <[a-z]*>);
    if ($#subpaths >= 0) {
	@subproj = grep /02xml/, `find @subpaths -follow -name config.xml`; chomp(@subproj);
    }

#    warn "super_path/project_base = $super_path/$project_base\n";

    if (-d "$super_path/00lib") {
	@sibproj = grep !m#^$super_path/$project_base#, `find $super_path/* -name 'config.xml'`; 
	chomp(@sibproj);
    }

#    warn "superproj: ", Dumper(\@superproj);
#    warn "sibproj: ", Dumper(\@sibproj);
#    warn "subproj: ", Dumper(\@subproj);

    foreach my $p (@superproj) {
	next unless $p && $p !~ /^\d/;
#	warn "p=$p\n";
	ORACC::XPD::Util::load_xpd($p);
	my $public = ORACC::XPD::Util::option('public') || 'no';
	if ($public eq 'yes') {
	    my($proj,$abbr) = (
		ORACC::XPD::Util::option('project') , 
		ORACC::XPD::Util::option('abbrev'));
	    warn "$p: no project\n" unless $proj;
	    push @configs, [ 'super' , $p , $proj , $abbr ];
	}
	ORACC::XPD::Util::unload();
    }

    foreach my $p (@subproj) {
	next unless $p && $p !~ /^\d/;
	warn "loading project=$p\n"
	    if $verbose;
	ORACC::XPD::Util::load_xpd($p);
	my $public = ORACC::XPD::Util::option('public') || '';
	if ($public eq 'yes') {
	    my($proj,$abbr) = (
		ORACC::XPD::Util::option('project') , 
		ORACC::XPD::Util::option('abbrev'));
	    warn "$p: no project\n" unless $proj;
	    push @configs, [ 'sub' , $p , $proj , $abbr ];
	} else {
	    warn "$p is not public\n"
		if $verbose;
	}
	ORACC::XPD::Util::unload();
    }
    @configs;
}

sub
compute_select1 {
    my $mode = shift;
    my $use_mode = `oraccopt . outline-$mode-select`; # ORACC::XPD::Util::option("outline-$mode-select");
    my $fields = `oraccopt . outline-$mode-sort-fields` # ORACC::XPD::Util::option("outline-$mode-sort-fields")
	|| 'period,genre,provenience';
    unless ($use_mode && $use_mode eq 'false') {
	my $labels = `oraccopt . outline-$mode-sort-labels` # ORACC::XPD::Util::option("outline-$mode-sort-labels")
	    || 'Time,Genre,Place';
	my @select1 = ();
	push @select1, "<select id=\"p3OS$mode\" name=\"p3OS$mode\" onchange=\"p3${mode}Sortstate()\" value=\"$fields\">\n";
	push @select1, make_select1($fields,$labels);
	push @select1, '</select>', "\n";
	@select1;
    } else {
	("<input type=\"hidden\" xmlns=\"http://www.w3.org/1999/xhtml\" id=\"p3OS$mode\" name=\"p3OS$mode\" value=\"$fields\"/>\n");
    }
}

# Base algorithm implementation from 
# http://www.perlmonks.org/index.pl?node_id=29374
sub
nextPermute(\@)
{
    my( $vals )= @_;
    my $last= $#{$vals};
    return ""   if  $last < 1;
    # Find last item not in reverse-sorted order:
    my $i= $last-1;
    $i--   until  $i < 0  ||  $vals->[$i] lt $vals->[$i+1];
    # If complete reverse sort, we are done!
    return ""   if  -1 == $i;
    # Re-sort the reversely-sorted tail of the list:
    @{$vals}[$i+1..$last]= reverse @{$vals}[$i+1..$last]
      if  $vals->[$i+1] gt $vals->[$last];
    # Find next item that will make us "greater":
    my $j= $i+1;
    $j++  until  $vals->[$i] lt $vals->[$j];
    # Swap:
    @{$vals}[$i,$j]= @{$vals}[$j,$i];
    return 1;
}

sub
make_select1 {
    my($values_option, $labels_option) = @_;
    my @values = split(/,/,$values_option);
    my @labels = split(/,/,$labels_option);
    my %vhmap = (); @vhmap{@values} = @labels;
    my @select1 = ();
    @values = sort @values;
    do {
	my $value = join(',',@values);
	my $vdes = $value;
	my $label = join('/',@vhmap{@values});
	push @select1, 
	"  <option value=\"$value\">$label</option>\n";
	if ($value eq $values_option) {
	    $select1[$#select1] =~ s/">/" selected="selected">/;
	}
    } while (nextPermute(@values));
    @select1;
}

sub
pll_link_ids {
    my $link = shift;
    my $ref = $link->getAttribute('id');
    if ($ref) {
	my $x = $xpll_link_ids->getElementById($ref);
	if ($x) {
	    $link->setAttribute('menu', $x->getAttribute('menu'));
	    $link->setAttribute('url', $x->getAttribute('url'));
	} else {
	    warn "p3-project-data.plx: unknown link ID `$ref'\n";
	}
    }
    $link->toString(1);
}

sub
pll_project {
#    warn Dumper $_[0];
    my @p = @{$_[0]};
#    warn "\@p = @p\n";
    return "<project type=\"$p[0]\" menu=\"$p[3]\" proj=\"$p[2]\"/>";
}

1;
