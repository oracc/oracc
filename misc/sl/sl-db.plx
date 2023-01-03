#!/usr/bin/perl
use warnings; use strict; use utf8;
use Getopt::Long;
use lib "$ENV{'ORACC'}/lib";
use open 'utf8'; binmode STDERR, 'utf8';
use ORACC::XML;
use ORACC::NS;
use Encode;
use Fcntl;
use NDBM_File;
use ORACC::SE::Indexer;
use ORACC::SE::XML;
use ORACC::SE::DBM;
use ORACC::SE::TSV;

use constant { TOP=>0, SUB=>1 };

my $boot = 0;

GetOptions(
    'boot'=>\$boot
    ) || usage_and_exit();

# This database uses the value as the primary key, with extensions
# to index homophones, containers, etc.  The value may be lower or
# upper case depending on whether the key data is a value or a sign
# name.
#
# The following extensions and cases are used:
#
#  <lower>		sign value
#  <lower>,h    	sign value homophones (gi1, gi2, gi3 etc.)
#  <lower>,aka    	sign value aliases (ga2 => ja2 etc.)
#  <lower>,forms    	sign forms (KA×GU, URU×GU etc.)
#
#  <upper>		sign name
#  <upper>,form		form name; may return more than one result
#  <upper>,uchar       	sign in Unicode/utf8
#  <upper>,ucode       	sign's Unicode hex code
#  <upper>,c		sign names used in compounds
#  <upper>,cinit	sign names used in initial position in compounds
#  <upper>,clast	sign names used in final position in compounds
#  <upper>,contains	sign names used as containers
#  <upper>,contained	sign names used within containers
#  <upper>,multi	sign names repeated two-up, crossed, etc.
#  
# In addition, the special convention m,<mod> is used as a key which
# returns all the signs utilising a given mod, e.g., m,g returns all
# gunufied signs.
#
# Besides the actual data, the db also contains the XHTML link code to
# find the article for a sign by id; this is given under <id>,link.

my $project = `oraccopt`;
my $dc_uri = 'http://dublincore.org/documents/2003/06/02/dces/';
my $html_uri = 'http://www.w3.org/1999/xhtml';
my $sl_uri = 'http://oracc.org/ns/sl/1.0';
my $xml_uri = 'http://www.w3.org/XML/1998/namespace';
my $gdl_uri = 'http://oracc.org/ns/gdl/1.0';

my $dbdir = undef;
my $dbbase = undef;
my $dbname = undef;
my $sl_xml = undef;

if ($boot) {
    $sl_xml = "$ENV{'ORACC'}/xml/ogsl/ogsl-sl.xml";
    unless (-r $sl_xml) {
	warn "sl-db.plx: must install ogsl-sl.xml in $ENV{'ORACC'}/ogsl first. Stop.\n";
	exit 1;
    }
    $dbdir = "$ENV{'ORACC'}/pub/ogsl/sl"; system 'mkdir', '-p', $dbdir;
    $dbname = "ogsl-db";
} else {
    $dbdir = '02pub/sl'; system 'mkdir','-p',$dbdir;
    $dbbase = shift @ARGV;
    unless ($dbbase) {
	warn "sl-db.plx: must either give db-name or '-boot' on command line. Stop.\n";
	exit 1;
    }
    $dbname = "$dbbase-db";
    $sl_xml = "02xml/$dbbase-sl.xml";
}

my %db;
my %values = ();

my $xp = XML::LibXML->new();
my $sl = $xp->parse_file($sl_xml);
my $do_aliases = !$boot;
my $global_id = '';

# first load up %values with the sign names and values
foreach my $s ($sl->getDocumentElement()->getElementsByTagNameNS($sl_uri,
								 'sign')) {
    my $parent_id = subsign($s,TOP,undef);
    foreach my $f ($s->getElementsByTagNameNS($sl_uri,'form')) {
	subsign($f,SUB,$parent_id);
    }
}

# now register all the qualified forms and make the lists of allowable qualifications by value
foreach my $q ($sl->getDocumentElement()->getElementsByTagNameNS($sl_uri,
								 'q')) {
    my $qn = $q->getAttribute('qn');
    my $qm = $q->getAttribute('qm');
    my $type = $q->getAttribute('type');
    my $base = $q->getAttribute('base');
    # warn "setting values $qn to $type\n";
    $values{$qn} = $type;
    push(@{$values{$qn,'map'}}, $qm) if $qm;
    my $qv = $qn; $qv =~ s/\(.*$//;
    push @{$values{$qv,'qual'}}, $qn;
    my $q0 = $qv; $q0 =~ tr/₀-₉ₓ⁻⁺//d; $q0 .= '₀';
    push @{$values{$q0,'qual'}}, $qn;
    $values{$qv,'qbase'} = $base if $base;
}

#add_aliases() if $do_aliases;

add_dumb_aliases();

dump_db();

##################################################################

# add the sign components: each value is a space-delimited string of IDs
#   ,c entry for each one
#   ,cinit entry for each initial one (e.g., KI of KI.BI.GAR)
#   ,clast entry for each last one (e.g., GAR of KI.BI.GAR)
#   ,m<MOD> entry for each one followed by a modifier, e.g., ,mg for mod=gunu
#   ,xc entry for each container
#   ,xi entry for each inner sign (no attempt to preserve grouping etc.)
#   ,d<DELIM> for each sign that is repeated with DELIM, e.g., ,dover for NUN-over-NUN
sub
add_comp {
    my($id,$cg) = @_;
#    warn "add_comp $id ", $cg->toString(), "\n";
    my $atf = $cg->getAttribute('form');
    $values{$atf} = $id;
    if ($atf =~ tr/\|//d) {
	$values{$atf} = $id;
    }
#    push @{$values{$id,'values'}}, "|$atf|";
#    my @g = $cg->getElementsByTagNameNS($gdl_uri,'g');
    add_comp_children($cg,$id);
}

sub
add_comp_children {
    my($parent,$id) = @_;
    my @g = grep ($_->isa('XML::LibXML::Element'), $parent->childNodes());
    foreach (my $i = 0; $i <= $#g; ++$i) {
	if ($g[$i]->localName() eq 'g') {
	    add_comp_children($g[$i]);
	} else {
	    next if $g[$i]->localName() eq 'o';
	    my $g = $g[$i];
	    my $n = $g->nextSibling();
	    my $gt = gtext($g,$n,$id);
	    push @{$values{$gt,'c'}}, $id;
	    if ($i == 0) {
		push @{$values{$gt,'cinit'}}, $id;
	    } elsif ($i == $#g) {
		push @{$values{$gt,'clast'}}, $id;
	    }
	    if ($n && $n->localName() eq 'o') {
		my $ng = $g[$i+2];
		if ($ng) {
		    my $ngt = gtext($ng,$ng->nextSibling(),$id);
		    if ($n->getAttributeNS($gdl_uri,'type') eq 'containing') {
			push @{$values{$gt,'contains'}}, $id;
			push @{$values{$ngt,'contained'}}, $id;
		    } elsif ($gt eq $ngt) {
			push @{$values{$gt,'multi'}}, $id;
		    }
		}
		++$i; # don't gtext this node
	    }
	}
    }
}

sub
gtext {
    my ($g,$n,$id) = @_;
    my $gt = undef;
    if ($g->nodeName() eq 'g:g') {
	$gt = join('', map { gval($_) } $g->childNodes());
    } else {
	$gt = $g->getAttribute('n') || $g->getAttribute('form') || $g->textContent();
    }
    warn "empty gtext in ", $g->toString(0), "\n"
	unless $gt;
    if ($gt) {
	$gt =~ tr/a-zšŋ/A-ZŠŊ/;
	if ($n && $n->localName() eq 'mod') {
	    my $mod = $n->getAttribute('c');
	    push @{$values{$gt,'m'}}, $id;
	    $gt .= '@' . $mod;
	    if ($values{$gt}) {
		push @{$values{'m',$mod}}, $values{$gt};
	    } else {
		my $xid = ($g->findnodes("ancestor::*[\@xml:id]/\@xml:id"))[0]->getValue();
		print STDERR "$xid: Component $gt not independently listed\n";
	    }
	}
	$gt;
    } else {
	my $xid = ($g->findnodes("ancestor::*[\@xml:id]/\@xml:id"))[0]->getValue();
	print STDERR "$xid: couldn't set \$gt\n";
	'';
    }
}

sub
gval {
    my $g = shift;
    return('') unless $g->isa('XML::LibXML::Element');
    my $n = $g->nodeName();
    if ($n eq 'g:s') {
	return $g->textContent();
    } elsif ($n eq 'g:o') {
	my $o = $g->getAttributeNS($GDL,'type');
	if ($o eq 'beside') {
	    return '.';
	} elsif ($o eq 'containing') {
	    return '×';
	} elsif ($o eq 'above') {
	    return '&';
	} elsif ($o eq 'crossing') {
	    return '%';
	} else {
	    warn "gval: unhandled o attr $o\n";
	    return '';
	}
    } else {
	warn "gval: unhandled tag $n\n";
	return '';
    }
}

sub
add_aliases {
    if (open(IN,'00lib/alias-smart.txt')) {
	while (<IN>) {
	    next if /^\#/ || /^\s+/ || /=>/;
	    my @v = split(/\s+/,$_);
	    my $aka = shift @v;
	    foreach my $v (@v) {
		$values{$v,'aka'} = $aka;
	    }
	}
	close(IN);
    } else {
	warn "sl-db.plx: no aliases file 00lib/alias-smart.txt\n";
	return;
    }
}

sub
add_dumb_aliases {
    if (open(IN,'00lib/alias-dumb.txt')) {
	while (<IN>) {
	    next if /^\#/ || /^\s+/ || /=>/;
	    my @v = split(/\s+/,$_);
	    my $aka = shift @v;
	    foreach my $v (@v) {
		$values{$v,'aka'} = $aka;
	    }
	}
	close(IN);
    } else {
	warn "sl-db.plx: no aliases file 00lib/alias-dumb.txt\n";
	return;
    }
}

sub
dump_db {
    unlink "$dbdir/$dbname";
#    tie(%db, 'NDBM_File', "$dbdir/$dbname", O_RDWR|O_CREAT|O_TRUNC, 0644) 
    #	|| die "mk-pslu.plx: can't write $dbdir/$dbname\n";
    my $ixname = "\U$project". ' Index';
    my $ix = ORACC::SE::Indexer::index($dbbase,$ixname,'x1',0,
				       [ qw/h aka c cinit clast contains contained forms multi mod link/ ],
				       [], 0, 10000, 0, 1);
    my %db = %$ix;
    foreach my $k (keys %values) {
	my $dbk = $k;
	Encode::_utf8_off($dbk);
	# sort the values here if the key otherwise contains a \^
	if ($dbk =~ /(?:link|name|atf|aka|uchar|ucode|qbase)$/) {
	    my $v = $values{$k};
	    Encode::_utf8_off($v);
	    $db{$dbk} = $v;
	} elsif ($dbk =~ /h$/) {
	    my $str = hsort(@{$values{$k}});	    
	    $db{$dbk} = $str;
	    if ($k =~ /₊/) {
		$k =~ s/h$//;
#		print STDERR "mk-pslu.plx: h-str for $k = $str\n";
	    }
	} elsif ($dbk =~ /forms$/) {
	    my $str = fsort(@{$values{$k}});
	    $db{$dbk} = $str;
	    if ($k =~ /₊/) {
		$k =~ s/h$//;
#		print STDERR "mk-pslu.plx: h-str for $k = $str\n";
	    }
	} elsif ($dbk =~ //) {
	    my $v = join(' ', sort uniq(grep(defined,@{$values{$k}}))); # FIXME: shouldn't be necessary to grep out defined
	    Encode::_utf8_off($v);
	    $db{$dbk} = $v;
	} else {
	    my $v = $values{$k};
	    Encode::_utf8_off($v);
	    $db{$dbk} = $v;
	}
    }

    ORACC::SE::DBM::setdir($dbdir);
#    ORACC::SE::XML::toXML(\%db);
    ORACC::SE::TSV::toTSV(\%db);
#    ORACC::SE::DBM::create($db{'#name'});

#    ORACC::SE::XML::toXML(\%db);
#    use Data::Dumper; open(D,'>./02pub/pslu.dump'); binmode(D, ':raw'); print D Dumper(\%db); close(D);
    untie %db;
}

sub
fsort {
    my @srt = @_;
    my @ret = ();
    foreach my $s (sort { $$a[1] cmp $$b[1] } @srt) {
	push @ret, "$$s[0]/$$s[1]";
    }
    join(' ', @ret);
}

sub
hsort {
    my @srt = @_;
    my @ret = ();
    foreach my $s (sort { vkey($a) <=> vkey($b) } @srt) {
	push @ret, $$s[0].'/'.vkey($s);
    }
    join(' ', @ret);
}

sub
uniq {
    my %tmp;
    @tmp{@_} = ();
    keys %tmp;
}

sub
vkey {
    my $x = shift;
    my $v = $$x[1];
    my $num = 0;

    if ($v =~ /([0-9₀₁₂₃₄₅₆₇₈₉]+)$/) {
	my $n = $1;
	$n =~ tr/₀-₉/0-9/;
	$num = $n;
    } else {
	$num = 0;
    }

    if ($v =~ /ₓ/) {
	$num += 1000;
    }

    $num;
}

sub
subsign {
    my($node,$mode,$parent_id) = @_;

    return if $node->hasAttribute('deprecated') && $node->getAttribute('deprecated') eq '1';

    my $form_is_TOP = 0;
    
    my $type = $node->getAttribute('type');
#    next unless $type eq 'normal' || $type eq 'numeric';

    my $rf = $node->getAttribute('ref');
    my $id = '';

    # if an @form is a known sign name, use the ID of the sign name
    ### THIS DOES NOT DO WHAT IT THINKS IT DOES; there is always a @ref attr but this does not mean
    ### the @form is also an @sign
    if ($rf) {
	$id = $rf;
    } else {
	$id = $node->getAttributeNS($xml_uri,'id');
	$form_is_TOP = 1;
    }
    my $sn = $node->getAttribute('n');
    
    if ($parent_id) {
	push @{$values{$id,'signs'}}, $parent_id;
#	warn "FORM: parent-id=$parent_id; form id=$id; sn=$sn\n";
#    } else {
#	warn "SIGN: id=$id; sn=$sn\n";
    }

    my($ucode,$uchar) = ('','');
    my @v = ();

    if ($parent_id) {
	my $var = $node->getAttribute('var');
	warn "FORM $sn in SIGN $values{$parent_id,'name'} has no VAR attribute\n" unless $var;
	push @{$values{$parent_id,'forms'}}, [$id,$var];
    }

    foreach my $c ($node->childNodes()) {
	my $lname = $c->localName();

	last if $lname eq 'form' && $mode == TOP;

	if ($lname eq 'utf8') {
	    $ucode = $c->getAttribute("hex");
	    $uchar = $c->textContent();
	} elsif ($lname eq 'list') {
	    # add the num entries (e.g. REC144)
	    my $n = $c->getAttribute('n');
	    my $n_orig = $n; $n_orig =~ tr/?//d;
	    $values{$n} = $id;
	    $n =~ s/\d+[a-z]*$//;
	    push @{$values{$n,'h'}}, [$id,$n_orig];
	} elsif ($lname eq 'v') {
	    my $v = $c->getAttribute('n');
	    $v =~ tr/?//d;
	    my $orig_v = $v;
	    $v =~ s/\{.*?\}//g;
	    next unless $v;
	    if ($values{$v}) {
		# This is now fixed in sl-xml.plx
		### FIXME: THIS IS A TODO LIST ITEM IN OGSL
#		warn "duplicate value in ogsl-sl.xml: $v occurs in $values{$values{$v},'name'} and $sn\n"
#		    unless !defined($v) || !length($v) || $v  =~ /ₓ/ || $v =~ /\.\.\./;
	    } else {
		$values{$v} = $id;
	    }
	    push(@{$values{$id,'values'}}, $orig_v) if $v;
	    # homophones: each value is a space-delimited string of IDs
	    #   strip off the digits
	    #   add the id to the entry $v,h
	    my $v_orig = $v;
	    $v =~ s/[₊₀-₉]*$//;
	    push @{$values{$v,'h'}}, [$id,$v_orig];
	} elsif ($lname eq 'name') {
	    foreach my $gc (tags($c,$GDL,'c')) {
		add_comp($id,$gc);
	    }
	}
    }
    
    my $xsn = $sn;
    $xsn =~ tr/|//d;

    if ($mode == TOP) {
	$values{$sn} = $id;
	$values{$xsn} = $id;
    } else {
	if ($form_is_TOP) {
	    $values{$sn} = $id;
	    $values{$xsn} = $id;
	}
	push @{$values{$sn,'form'}}, $id;
	push @{$values{$xsn,'form'}}, $id;
    }

    $values{$id,'ucode'} = $ucode if $ucode;
    $values{$id,'uchar'} = $uchar if $uchar;
    $values{$id,'name'} = $sn;

    $id;
}

# Process @forms which do not occur as @sign
sub
xsign {
}

sub
usage_and_exit {
    warn "sl-db.plx: bad command line arguments. Stop.\n";
    exit 1;
}
