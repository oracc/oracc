#!/usr/bin/perl
use warnings; use strict; use utf8;
use lib "$ENV{'ORACC'}/lib";
use open 'utf8'; binmode STDERR, 'utf8';
use XML::LibXML;
use Encode;
use Fcntl;
use GDBM_File;

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
#
#  <upper>		sign name
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

my $dbdir = '02pub/'; system 'mkdir','-p',$dbdir;
my $dbbase = shift @ARGV;
my $dbname = "$dbbase-db";
my %db;

my %values = ();

my $xp = XML::LibXML->new();
my $sl = $xp->parse_file("02xml/$dbbase.xml");

# first load up %values with the sign names and values
foreach my $s ($sl->getDocumentElement()->getElementsByTagNameNS($sl_uri,
								 'sign')) {
    my $type = $s->getAttribute('type');
#    next unless $type eq 'normal' || $type eq 'numeric';
    my $id = $s->getAttributeNS($xml_uri,'id');
    my $sn = $s->getAttribute('n');
    my $unode = ($s->getElementsByTagNameNS($sl_uri,'utf8'))[0];
    my $ucode = $unode ? $unode->getAttribute("hex") : '';
    my $uchar = $unode ? $unode->textContent() : '';
    my $xsn = $sn;
    $xsn =~ tr/|//d;
    $values{$xsn} = $id;
    $values{$id,'values'} = join(', ', map { $n->getAttribute('n') } tags($s,$sl_uri,'v');
    $values{$id,'ucode'} = $ucode if $ucode;
    $values{$id,'uchar'} = $uchar if $uchar;
    $values{$id,'name'} = $sn;
    foreach my $l ($s->getElementsByTagNameNS($sl_uri,'list')) {
	my $n = $l->getAttribute('n');
	$values{$n} = $id;
    }

#    my $name = $s->firstChild();
#    my $nameg = $name->firstChild();
#    if (!defined $nameg) {
#	print STDERR "mk-pslu.plx: bad name format in $sn\n";
#	next;
#    }
#    if ($nameg->localName() eq 'g' && $nameg->nextSibling()) {
#	gtext($nameg,$nameg->nextSibling(),$id);
#    }
#
    foreach my $vnode ($s->getElementsByTagNameNS($sl_uri,'v')) {
	my $v = $vnode->getAttribute('n');
	
	$v =~ tr/?//d;
	$v =~ s/\{.*?\}//g;
	$values{$v} = $id;
	# homophones: each value is a space-delimited string of IDs
	#   strip off the digits
	#   add the id to the entry $v,h
	my $v_orig = $v;
	$v =~ s/[₊₀-₉]*$//;
	push @{$values{$v,'h'}}, [$id,$v_orig];
    }
    # add the num entries (e.g. REC144)
    foreach my $numnode ($s->getElementsByTagNameNS($sl_uri,'list')) {
	my $d = $numnode->getAttribute('n');
	my $d_orig = $d;
	$values{$d} = $id;
	$d =~ s/\d+[a-z]*$//;
	push @{$values{$d,'h'}}, [$id,$d_orig];
    }
}

# now process the elements of composite signs
#foreach my $s ($sl->getDocumentElement()->getElementsByTagNameNS($sl_uri,
#								 'sign')) {
#    my $id = $s->getAttributeNS($xml_uri,'id');
#    foreach my $cg ($s->getElementsByTagNameNS($gdl_uri,'cg')) {
#	add_comp($id,$cg);
#    }
#}

#add_aliases();

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
    my $atf = $cg->getAttribute('atf');
    $atf =~ tr/\|//d;
    $values{$atf} = $id;
    $values{$id,'atf'} = "|$atf|";
    my @g = $cg->getElementsByTagNameNS($gdl_uri,'g');
    foreach (my $i = 0; $i <= $#g; ++$i) {
	my $g = $g[$i];
	my $n = $g->nextSibling();
	my $gt = gtext($g,$n,$id);
	push @{$values{$gt,'c'}}, $id;
	if ($i == 0) {
	    push @{$values{$gt,'cinit'}}, $id;
	} elsif ($i == $#g) {
	    push @{$values{$gt,'clast'}}, $id;
	}
	if ($n && $n->localName() eq 'rel') {
	    my $ng = $g[$i+1];
	    my $ngt = gtext($ng,$ng->nextSibling(),$id);

	    if ($n->getAttribute('c') eq 'times') {
		push @{$values{$gt,'contains'}}, $id;
		push @{$values{$ngt,'contained'}}, $id;
	    } elsif ($gt eq $ngt) {
		push @{$values{$gt,'multi'}}, $id;
	    }
	}
    }
}

sub
gtext {
    my ($g,$n,$id) = @_;
    my $gt = $g->textContent();
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
}

sub
add_aliases {
    open(IN,'sources/aliases');
    while (<IN>) {
	next if /^\#/ || /^\s*$/;
	my @v = split(/\s+/,$_);
	my $aka = shift @v;
	foreach my $v (@v) {
	    $values{$v,'aka'} = $aka;
	}
    }
    close(IN);
}

sub
dump_db {
    unlink "$dbdir/$dbname";
    tie(%db, 'GDBM_File', "$dbdir/$dbname", O_RDWR|O_CREAT|O_TRUNC, 0644) 
	|| die "mk-pslu.plx: can't write $dbdir/$dbname\n";
    foreach my $k (keys %values) {
	my $dbk = $k;
	Encode::_utf8_off($dbk);
	# sort the values here if the key otherwise contains a \^
	if ($dbk =~ /(?:link|name|atf|aka|uchar|ucode)$/) {
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
	} elsif ($dbk =~ //) {
	    my $v = join(' ', sort uniq(@{$values{$k}}));
	    Encode::_utf8_off($v);
	    $db{$dbk} = $v;
	} else {
	    my $v = $values{$k};
	    Encode::_utf8_off($v);
	    $db{$dbk} = $v;
	}
    }
    untie %db;
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

    if ($v =~ /₊/) {
	$num += 1000;
    }

    $num;
}
