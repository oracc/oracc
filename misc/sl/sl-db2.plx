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
my $sldb2 = 0;

my %cmembseen = ();

GetOptions(
    'boot'=>\$boot
    ) || usage_and_exit();


### OGSL BUG: @sign BAR.AN @v kunga₂ @form |ŠU₂.AN| ... @sign |ŠU₂.AN| @v kunga₂ shouldn't be allowed

### BUGS:
### ;lists gives sign name without number e.g. LAK ELLES
### ;cmemb needs to be uniqed to prevent, e.g., 'U&U' generating two entries
###

##########################################################################################
#
#                         SLDB2 (2023-01-26)
#
##########################################################################################

# In SLDB2 type keys are single-letter. Key-base is a transliteration
# component and the value is an OID.

#
# ;f form
# ;l list
# ;n number		(not yet implemented)
# ;p punctuation	(not yet implemented)
# ;s sign
# ;v value
# 

#
# There are two types of compound: a compound-sign, e.g., |A×HA|, and
# a qualified-value like a(ZA).  The key of a cs or qv is a
# transliteration component.  The value of a compound-sign or
# qualified-value is its signature, one or more OIDs.
#

#
# ;cs compound-sign
# ;qv qualified-value
#

#
# SLDB2 does not use ;map entries--instead, canonicalize the qualifier
# and then test for the resultant qv
#
# SLDB2 does not use the must/may mechanism.  To test must/may in a qv
# simply check a(A);qv--if it is present the qv must be qualified; if
# it is not, the qv may be qualified in transliteration (but not in
# ePSD bases).  This assumes that the qualifier has the value in its
# value-set.
#

#
# Data associated with a sign or form is stored in the following
# fields where the key is the OID of the @sign or @form:
#
# <empty> 	an OID with no field has name of the sign or form as value
# ;signs
# ;uchar	the unicode value as UTF-8
# ;ucode	the unicode value as hex
#

#
# A sign can may have an OID;forms field which gives the IDs and variant
# codes in the format OID1/~a OID2/~b:
#
# ;forms o0000801;forms  o0000864/~a o0000863/~b o0000866/~c
#

#
# A sign or form usually has an OID;values field which gives a
# space-separated list of its known values:
#
# ;values guₓ gudiddi si₁₈ sigₓ še₂₆ šed₁₅ šeg₁₁ šiₓ šidₓ šigₓ
#
# If this field is absent for an OID it means that there are no known
# values for the sign/form.
#

#
# A sign or form usually has an OID;lists field which gives a
# space-separated list of its known sign-list references
#
# ;lists ELLES349 LAK181 RSP064
#

#
# A form also has an OID:par field which gives a space-separated list
# of the signs under which it appears as a form.
#
# ;parents
#
# This is a change in SLDB2; formerly ;p was used with a qv to get the
# parent sign and ensured that the result was a singleton OID not a
# list.
#
# The following procedure can now be used to obtain this information:
#
# * test that the qv is given in SLDB--this indicates that the qualifier is a form
# * get the parent from the qualifier of the qv
#

#
# A value-base (with no index) has a VBASE;h field which gives a
# space-separated list of the OIDs and the index the value-base has
# for that OID:
#
# ni;h	o0000479/1 o0000486/2 o0000202/3 o0002323/4 o0000477/5
# 
# If there are ₓ-indexed values for the base value, these are given at
# the end of the VBASE;h field with a /0 index:
#
# saŋ;h o0000518/1 o0000434/4 o0000485/5 o0002837/5 o0002477/0 o0002519/0
#
# The VBASE can also be a list abbreviation such as LAK in which case
# the list gives the list entries that are entered in the signlist.
#

#
# There are several fields specific to cs compounds.
#
# OID;cmemb	space-separated list of OIDs of cs entries that contain this OID
# OID;cinit	space-separated list of OIDs of cs entries that start with this OID
# OID;clast	space-separated list of OIDs of cs entries that end with this OID
# OID;contains	OID is a container sign; list of space-separated OIDs that are part of contained sequences
# OID;contained	OID is a contained sign; list of space-separated OIDs that are containers of OID
#

#
# For qv compounds, the ;qual field is no longer used.  This formerly gave a
# list of the possibly qualifiers of a value including all of the
# lists by which it could be referenced.  This information is now
# computed at run-time if needed (which it rarely is).
#

#
# For qv compounds, the ;qbase field is no longer used.  This formerly
# gave the value in a form's parent sign that corresponded to the
# value in the form, e.g., saŋa₅ -> saŋa₂.  This can be computed if
# necessary as follows:
#
#  * get the form OID via the value
#  * get the sign OID via the form OID's ;p field
#  * reduce the value to its value-base
#  * get the VBASE;h field for the value
#  * search the VBASE;h field for the sign OID
#  * retrieve the index from the number after the / in the OID/INDEX pair
#

#
# The ;aka field is not yet implemented in SLDB2
#

#
# The ;multi field is not yet implemented in SLDB2
#
############################################################################################################

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
    $dbname = "$dbbase-db2";
    $sl_xml = "02xml/$dbbase-sl.xml";
}

my %db;
my %topvalues = (); # these are values which occur as children of @sign
my %values = ();
my %vq = (); # these are values which must be deleted from %values at end of run and registered as $v,'q'

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
    my $o = $q->getAttribute('o');
    my $p = $q->getAttribute('p');
    # warn "setting values $qn to $type\n";
    # $values{$qn} = $type unless $type eq 'map';

    if ($type eq 'must') {
	my $qv = $qn;
	my $qs = $qn;
	if ($qn =~ /\)\(/) {
	    $qv =~ s/\)\(.*$/\)/;
	    $qs =~ s/^.*?\)\((.*?)\)$/$1/;
	} else {
	    $qv =~ s/\(.*$//;
	    $qs =~ s/^.*?\((.*?)\)$/$1/;
	}
	push @{$vq{$qv}}, $qs;
	$values{$qn,'qv'} = $o;
    }

    # $values{"$qn;p"} = $p;
    # push(@{$values{$qn,'map'}}, $qm) if $qm;
    my $qv = $qn; $qv =~ s/\(.*$//;
    # push @{$values{$qv,'qual'}}, $qn;
    my $q0 = $qv; $q0 =~ tr/₀-₉ₓ⁻⁺//d; $q0 .= '₀';
    # push @{$values{$q0,'qual'}}, $qn;
    # $values{$qv,'qbase'} = $base if $base;
}

## add_aliases() if $do_aliases; # this was already commented out in sldb1

# add_dumb_aliases(); # this was still in use in sldb1

# Values in %vq must always be qualified; remove the version that has
# no ';' field and enter it as ;q

foreach my $vq (keys %vq) {
    unless ($topvalues{$vq}) {
	$values{$vq,'q'} = $vq{$vq};
	delete $values{$vq};
    }
}

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

    # $values{$atf} = $id; # 'cs' # NO: @sign/@form compounds are now added before this

## TSV no longer contains pipeless versions of compounds
#    if ($atf =~ tr/\|//d) {
#	$values{$atf,'cs'} = $id;
#   }

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
	    push @{$values{$gt,'cmemb'}}, $id unless $cmembseen{"$gt;cmemb:$id"}++;
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
			# push @{$values{$gt,'multi'}}, $id;
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
    my $ixname = "\U$project". ' Index';
    my $ix = ORACC::SE::Indexer::index($dbbase,$ixname,'x1',0,
				       [ qw/h aka c cinit clast contains contained forms multi mod link/ ],
				       [], 0, 10000, 0, 1);
    my %db = %$ix;
    $db{'#tsv'} = "02pub/sl/$dbname.tsv";
    $db{'#nohash'} = 1;
    foreach my $k (keys %values) {
	my $dbk = $k;
	Encode::_utf8_off($dbk);
	# sort the values here if the key otherwise contains a \^
	if ($dbk =~ /(?:link|name|atf|aka|uchar|ucode|s|f|l|v|p|cs|qv)$/) {
	    my $v = $values{$k};
	    Encode::_utf8_off($v);
	    $db{$dbk} = $v;
	} elsif ($dbk =~ /h$/) {
	    my $kk = $dbk;
	    $kk =~ tr//;/;
            warn "db_dump $kk\n";
	    my $str = hsort(@{$values{$k}});
	    $db{$dbk} = $str;
	    if ($k =~ /₊/) {
		$k =~ s/h$//;
	    }
	} elsif ($dbk =~ /forms$/) {
	    my $str = fsort(@{$values{$k}});
	    $db{$k} = $str;
	} elsif ($dbk =~ //) {
	    # FIXME: shouldn't be necessary to grep out defined
	    # warn "dbk = $dbk\n";
	    my $v = join(' ', sort uniq(grep(defined,@{$values{$k}})));
	    Encode::_utf8_off($v);
	    $db{$dbk} = $v;
	} else {
	    my $v = $values{$k};
	    Encode::_utf8_off($v);
	    $db{$dbk} = $v;
	}
    }

    ORACC::SE::DBM::setdir($dbdir);
    ORACC::SE::TSV::toTSV(\%db);
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
    # print STDERR "hsort in:";
    foreach my $s (sort { vkey($a) <=> vkey($b) } @srt) {
	my $vk = vkey($s);
	# print STDERR " $$s[0]";
	if ($vk == 1) {
	    push @ret, $$s[0];
	} else {
	    $vk = 0 if $vk == 1000;
	    push @ret, $$s[0].'/'.$vk;
	}
    }
    # print STDERR "\n";
    # warn "hsort ret: @ret\n";
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

    if ($v =~ /([₀₁₂₃₄₅₆₇₈₉]+)$/) {
	my $n = $1;
	$n =~ tr/₀-₉/0-9/;
	$num = $n;
    } else {
	$num = 1;
    }

    if ($v =~ /ₓ/) {
	$num = 1000;
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
    if ($rf) {
	$id = $rf;
    } else {
	$id = $node->getAttributeNS($xml_uri,'id');
	$form_is_TOP = 1;
    }
    my $sn = $node->getAttribute('n');

#    if ($sn eq '|AŠ×DIŠ@t|') {
#	warn "found |AŠ×DIŠ\@t| id=$id\n";
#    }
    
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
	push @{$values{$id,'parents'}}, $parent_id;
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
	    # $values{$n} = $id;
	    $values{$n,'l'} = $id; # only list names given with @list get ';l'
	    $n =~ s/\d+[a-z]*$//;
	    push @{$values{$n,'h'}}, [$id,$n_orig];
	    push @{$values{$id,'lists'}}, $n; # or n_orig?
	} elsif ($lname eq 'v') {
	    my $dropped = $c->getAttribute('deprecated');
	    next if $dropped and $dropped eq 'yes';

	    my $type = $c->getAttribute('type');
	    next if $type && ($type eq 'tlit' || $type eq 'word');

	    my $xlang = $c->getAttribute('xml:lang');
	    if ($xlang) {
		# warn "found <v xml:lang> = $xlang\n";
		# next;
		# need to allow %akk and %elx as valid graphemes
	    }

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
		# $values{$v} = $id;
		$values{$v} = $id; # 'v'
		++$topvalues{$v} if $mode == TOP && $v !~ /ₓ$/; # force ₓ-values to have ;q
	    }
	    push(@{$values{$id,'values'}}, $orig_v) if $v;
	    # homophones: each value is a space-delimited string of IDs
	    #   strip off the digits
	    #   add the id to the entry $v,h
	    my $v_orig = $v;
	    $v =~ s/[ₓ₀-₉]*$//;
	    push @{$values{$v,'h'}}, [$id,$v_orig];
	} elsif ($lname eq 'name') {
	    foreach my $gc (tags($c,$GDL,'c')) {
		add_comp($id,$gc);
	    }
	} elsif ($lname eq 'inherited') {
	    foreach my $iv (tags($c,'','iv')) {
		my $n = $iv->getAttribute('n');
		my $ivb = $n;
		$ivb =~ s/[₀-₉ₓ]+$//;
		push @{$values{$ivb,'h'}}, [$id,$n];
		push @{$values{$id,'values'}}, $n;
		# warn "adding inherited $n to $ivb;h $id\n";
	    }
	}
    }

    my $xsn = $sn;
    $xsn =~ tr/|//d;

    if ($mode == TOP) {
	# $values{$sn} = $id;
	# $values{$xsn} = $id;
	$values{$sn} = $id; # ,'s'
	# $values{$xsn,'sign'} = $id;
    } else {
	if ($form_is_TOP) {
	    # $values{$sn} = $id;
	    # $values{$xsn} = $id;
	}
	$values{$sn} = $id; # ,'f'
	# $values{$xsn,'form'} = $id;
    }

#    my $psn = $sn;
#    $values{$psn} = $id if $psn =~ tr/+/./;
    
    $values{$id,'ucode'} = $ucode if $ucode;
    $values{$id,'uchar'} = $uchar if $uchar;
    # $values{$id} = $sn;
    # $values{$id,'name'} = $sn;

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
