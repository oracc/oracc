package ORACC::XTF::RTF;
use warnings; use strict; use utf8;
use RTF::Writer;

my $GDL ='http://oracc.org/ns/gdl/1.0';
my $NORM='http://oracc.org/ns/norm/1.0';
my $XML='http://www.w3.org/XML/1998/namespace';
my $XTF='http://oracc.org/ns/xtf/1.0';
my $rtf = undef;

# These parameters should be exposed through the intended
# #rtf: protocol.

# Baselines:
#
# Zero point is the normal baseline.
#   subscripts from normal baseline are at $sub_base;
#   superscripts from normal baseline are at $sup_base;
#   subscripts from $sup_base are at $supsub_base
#   superscripts from $sup_base are at $supsup_base
#   subscripts from $supsup_base are at $supsupsub_base
#   superscripts from $supsup_base are at $subsubsub_base
#
# This may sound like overkill, but it ensures that we can
# handle determinatives in glosses, i.e., in ATF:
#
#   {{{d}en-lil2}} or {{{lu2}kas4}}
#
# Negative values are RTF \dn's.
# Positive values are RTF \up's.
my $norm_base = 0;
my $sub_base = -6;
my $sup_base = 6;
my $supsub_base = 4;
my $supsup_base = 10;
my $supsupsub_base = 8;

my $sub_o;
my $sub_c;
my $supsub_o;
my $supsub_c;
my $supsupsub_o;
my $supsupsub_c;

my $sup_o;
my $sup_c;
my $supsup_o;
my $supsup_c;
my $supsupsup_o;
my $supsupsup_c;

sub
set_baselines {
    $norm_base = updn($norm_base);
    $sub_o = updn($sub_base);
#    $sub_c = $norm_base;
    $sub_c = '\nosupersub';
    $supsub_o = updn($supsub_base);
    $supsub_c = updn($sup_base);
    $supsupsub_o = updn($supsupsub_base);
    $supsupsub_c = updn($supsup_base);
    $sup_o = updn($sup_base);
    $sup_c = $norm_base;
    $supsup_o = updn($supsup_base);
    $supsup_c = updn($sup_base);
}
sub
updn {
    my $param = shift;
    if ($param < 0) {
	'\dn'.$param*-1;
    } else {
	'\up'.$param;
    }
}

# Font sizes:
# fs_normal is the normal point-size in half-points
# fs_super1 is the super/subscript point-size in half-points
# fs_super2 is the double-super/subscript point-size in half-points
my $fs_normal = 24;
my $fs_super1 = 14;
my $fs_super2 = 10;

my $font_norm;
my $font_sup1;
my $font_sup2;

sub
set_fonts {
    $font_norm = '\fs'.$fs_normal;
    $font_sup1 = '\fs'.$fs_super1;
    $font_sup2 = '\fs'.$fs_super2;
}

########################################################

my $super = 0;
my $h1_o = '{\pard\b\f0\fs40 ';
my $h1_c = '\par}';
my $h2_o = '{\pard\b\f0\fs30 ';
my $h2_c = '\par}';
my $l_o = '{\pard\f0 ';
my $l_c = '\par}';
my $hyph = '-';
my $lc = '{';
my $rc = '}';
my $nl = "\n";
my $ws = " ";
my $lp = '(';
my $rp = ')';
my $italic_o = '{\i ';
my $u_times = '×';
my $pipe = '|';
my $nonx_o = '{\pard\li300 ';
my $nonx_c = '\par}';

my $ho = '{\f1 ';
my $hc = '{\f1 ';
my $hl = "\x{02f9}";
my $hr = "\x{02fa}";

my $iterate = 1;

my %nstab = (
    $GDL  => 'g',
    $NORM => 'n',
    $XTF => 'x',
    );

my %ops = (
    'x:xtf'=>'SKIP',
    'x:transliteration'=>\&x_transliteration,
    'x:protocols'=>'STOP',
    'x:protocol'=>'STOP',
    'x:object'=>\&x_object,
    'x:surface'=>\&x_surface,
    'x:column'=>\&x_column,
    'x:nonx'=>\&x_nonx,
    'g:nonw'=>'SKIP',
    'x:cmt'=>'SKIP',
    'x:l'=>\&x_l_o,
    'g:b'=>\&g_b,
    'g:c'=>\&g_c,
    'g:d'=>\&g_super_o,
    'g:g'=>\&g_g,
    'g:gg'=>\&g_gg,
    'g:gloss'=>\&g_super_o,
    'g:n'=>\&g_n,
    'g:o'=>\&g_o,
    'g:p'=>\&g_p,
    'g:q'=>\&g_q,
    'g:s'=>\&g_s,
    'g:v'=>\&g_v,
    'g:w'=>\&g_w,
    'g:x'=>\&g_x,
    );

my %ops_c = (
    'x:l'=>\&x_l_c,
    'g:d'=>\&g_super_c,
    'g:gloss'=>\&g_super_c,
    );

sub
generate {
    my($xtf,$fname) = @_;
    if (!$xtf->isa('XML::LibXML::Document')
	|| $xtf->getDocumentElement()->namespaceURI() ne $XTF) {
	warn("ORACC::XTF::RTF: argument is not an XTF document\n");
    }
    set_params($xtf);
    my $result = '';
    # new_to_file doesn't close the file properly
    $rtf = RTF::Writer->new_to_string(\$result);
    $rtf->prolog(fonts=>["Times New Roman","Half Square"]);
    iterate($xtf->getDocumentElement());
    $rtf->close();
    open(R,">$fname"); print R $result; close(R);
}

sub
iterate {
    my $n = shift;
    node_start($n);
    if ($iterate) {
	foreach my $c ($n->childNodes()) {
	    my $isa = ref($c);
	    if ($isa eq 'XML::LibXML::Element') {
		iterate($c);
	    } elsif ($isa eq 'XML::LibXML::Text') {
		escape($c->data());
	    } else {
		# skip PIs
	    }
	}
    } else {
	$iterate = 1;
    }
    node_end($n);
}

sub
set_params {
    # TODO: set params from #rtf: protocols
    set_baselines();
    set_fonts();
}

sub
escape {
    $rtf->print(@_);
}

sub
node_start {
    my $n = shift;
    my $qname = $nstab{$n->namespaceURI()}.':'.$n->localName();
    my $op = $ops{$qname};
    if ($op) {
	if ($op eq 'SKIP') {
	} elsif ($op eq 'STOP') {
	    $iterate = 0;
	} elsif (ref($op) eq 'SCALAR') {
	    $rtf->print($op);
	} elsif (ref($op) eq 'CODE') {
	    &{$op}($n);
	} else {
	    warn("ORACC::XTF::RTF: unhandled op $op\n");
	}
    } else {
	warn("ORACC::XTF::RTF: unhandled tag $qname\n");
    }
}

sub
node_end {
    my $n = shift;
    my $qname = $nstab{$n->namespaceURI()}.':'.$n->localName();
    my $op = $ops_c{$qname};
    if ($op) {
	if ($op eq 'SKIP') {
	} elsif (ref($op) eq 'SCALAR') {
	    $rtf->print($op);
	} elsif (ref($op) eq 'CODE') {
	    &{$op}($n);
	} else {
	    warn("ORACC::XTF::RTF: unhandled op_c $op\n");
	}
    }
}

sub
x_transliteration {
    my $n = shift;
    my $name = $n->getAttribute('n');
    $rtf->print(\$h1_o,$name,\$h1_c,\$nl);
}

sub
x_object {
    my $n = shift;
    my $imp = $n->getAttribute('implicit');
    if (!$imp || $imp eq '0') {
	my $s = $n->getAttribute('type');
	$s = "\u$s";
	$rtf->print(\$h2_o,$s,\$h2_c,\$nl);
    }
}

sub
x_surface {
    my $n = shift;
    my $imp = $n->getAttribute('implicit');
    if (!$imp || $imp eq '0') {
	my $s = $n->getAttribute('type');
	$s = "\u$s";
	$rtf->print(\$h2_o,$s,\$h2_c,\$nl);
    }
}

sub
x_column {
    my $n = shift;
    my $imp = $n->getAttribute('implicit');
    if (!$imp || $imp eq '0') {
	my $s = $n->getAttribute('n');
	$rtf->print(\$h2_o,"Column $s",\$h2_c,\$nl);
    }
}

sub
x_l_o {
    my $n = shift;
    my $lnum = $n->getAttribute('n');
    $lnum =~ tr/′/'/;
    $rtf->print(\$l_o,"$lnum.\t");
    foreach my $c ($n->childNodes) {
	iterate($c);
	# FIXME: when atf2xtf sets the pos attr on
	# g:gloss properly we need to handle pre/post
	# glosses correctly
	$rtf->print($ws) if $c->nextSibling()
	    && $c->localName() ne 'gloss';
    }
    $iterate = 0; # suppress auto-iteration
}

sub
x_l_c {
    $rtf->print(\$l_c,\$nl);
}

sub
x_nonx {
    my $n = shift;
    $rtf->print(\$nonx_o,'(',$n->textContent(),')',\$nonx_c);
    $iterate = 0;
}

sub
g_super_o {
    my $n = shift;
    if ($super == 1) {
	$rtf->print(\$lc,\$font_sup2,\$supsup_o);
    } elsif ($super > 1) {
	$rtf->print(\$lc,\$font_sup2,\$supsup_o); # shouldn't happen
    } else {
	$rtf->print(\$lc,\$font_sup1,\$sup_o);
    }
    ++$super;
}

sub
g_super_c {
    my $n = shift;
    if ($super == 1) {
	$rtf->print(\$rc,\$norm_base,\$font_norm);
    } elsif ($super > 1) {
	$rtf->print(\$rc,\$font_sup1,\$sup_o); # shouldn't happen
    } else {
	$rtf->print(\$rc,\$font_sup1,\$sup_o);
    }
    --$super;
}

sub
g_sub {
    my $index = shift;
    if ($super == 1) {
	$rtf->print(\$lc,\$font_sup2,\$supsub_o);
    } elsif ($super > 2) {
	$rtf->print(\$lc,\$font_sup2,\$supsub_o);  # shouldn't happen
    } else {
	$rtf->print(\$lc,\$font_sup1,\$sub_o);
    }
    $rtf->print($index);
    if ($super == 1) {
	$rtf->print(\$rc,\$font_sup1,\$supsub_c);
    } elsif ($super > 2) {
	$rtf->print(\$rc,\$font_sup1,\$supsub_c); #shouldn't happen
    } else {
	$rtf->print(\$rc,\$sub_c,\$font_norm);
    }
}

sub
g_a {
    my $n = shift;
    $rtf->print(\$sub_o,$n->textContent(),$rc);
    $iterate = 0;
}

sub
g_b {
    my $n = shift;
    g_body($n);
    $iterate = 0;
}

sub
g_c {
    my $n = shift;
    a_g_o($n);
    $rtf->print($pipe);
    foreach my $c ($n->childNodes()) {
	iterate($c);
    }
    $rtf->print($pipe);
    a_g_c($n);
    $iterate = 0;
}

sub
g_g {
    my $n = shift;
    a_g_o($n);
    $rtf->print($lp);
    foreach my $c ($n->childNodes()) {
	iterate($c);
    }
    $rtf->print($rp);
    a_g_c($n);
    $iterate = 0;
}

sub
g_gg {
    my $n = shift;
    my $type = $n->getAttributeNS($GDL,'type');
    my @c = $n->childNodes();
    iterate(shift @c);
    if ($type eq 'correction') {
	$rtf->print('!');
	if ($#c >= 0) {
	    $rtf->print($lp);
	    foreach my $c (@c) {
		iterate($c);
	    }
	    $rtf->print($rp);
	}
    } elsif ($type eq 'alternation') {
	foreach my $c (@c) {
	    $rtf->print('/');
	    iterate($c);
	}
    } elsif ($type eq 'reordering') {
	foreach my $c (@c) {
	    $rtf->print(':');
	    iterate($c);
	}
    } elsif ($type eq 'group') {
	foreach my $c (@c) {
	    $rtf->print('.');
	    iterate($c);
	}
    } else {
	warn("ORACC::XTF::RTF: unknown g:gg type $type\n");
    }
    $iterate = 0;
}

sub
g_m {
    my $n = shift;
    $rtf->print('@',$n->textContent());
    $iterate = 0;
}

sub
g_n {
    my $n = shift;
    a_g_o($n);
    $rtf->print($n->firstChild()->textContent(),$lp);
    iterate($n->lastChild());
    $rtf->print($rp);
    a_g_c($n);
    $iterate = 0;
}

sub
g_o {
    my $n = shift;
    my %o_types = (
	beside=>'.',
	joining=>'+',
	reordered=>':',
	repeated=>$u_times,
	containing=>$u_times,
	above=>'&',
	crossing=>'%',
	opposing=>'@',
	);
    my $o = $n->getAttributeNS($GDL,'type');
    $rtf->print($o_types{$o});
    if ($o eq 'repeated') {
	$rtf->print($n->textContent());
    }
    $iterate = 0;
}

sub
g_p {
    my $n = shift;
    a_g_o($n);
    my $type = $n->getAttributeNS($GDL,'type');
    $rtf->print($type);
    if ($n->hasChildNodes()) {
	$rtf->print($lp);
	foreach my $c ($n->childNodes()) {
	    iterate($c);
	}    
	$rtf->print($rp);
    }
    a_g_c($n);
    $iterate = 0;
}

sub
g_q {
    my $n = shift;
    a_g_o($n);
    iterate($n->firstChild());
    $rtf->print($lp);
    iterate($n->lastChild());
    $rtf->print($rp);
    a_g_c($n);
    $iterate = 0;
}

sub
g_s {
    my $n = shift;
    grapheme($n,0);
    $iterate = 0;
}

sub
g_v {
    my $n = shift;
    grapheme($n,1);
    $iterate = 0;
}

sub
g_w {
    my $n = shift;
    foreach my $c ($n->childNodes()) {
	iterate($c);
	$rtf->print(\$hyph) 
	    if $c->nextSibling() 
	    && ($c->localName() ne 'd' || $c->getAttributeNS($GDL,'pos') ne 'pre')
	    && ($c->nextSibling()->localName() ne 'd' 
		|| $c->nextSibling()->getAttributeNS($GDL,'pos') ne 'post')
	    && ($c->nextSibling()->localName() ne 'x'
		|| $c->nextSibling()->getAttributeNS($GDL,'type') ne 'newline');
     }
    $iterate = 0;
}

sub
g_x {
    my $n = shift;
    a_g_o($n);
    my $t = $n->getAttributeNS($GDL,'type');
    if ($t) {
	if ($t eq 'ellipsis') {
	    $rtf->print('...');
	} elsif ($t eq 'newline') {
	    $rtf->print(';');
	} elsif ($t eq 'user') {
	    $rtf->print($t->textContent());
	} else {
	    warn("ORACC::XTF::RTF: unhandled g_x type $t\n");
	}
    } else {
	# can't happen in a valid document
    }
    a_g_c($n);
    $iterate = 0;
}

sub
grapheme {
    my $n = shift;
    a_g_o($n);
    if ($n->firstChild()->isa('XML::LibXML::Element')) {
	iterate($n->firstChild());
    } else {
	g_body($n);
    }
    a_g_c($n);
}

sub
g_body {
    my $n = shift;
    my $g = $n->textContent();
    my($body,$index) = ($g =~ /^(.*?)([₀-₉₊]*)$/);
    my $v = $n->localName() eq 'v' 
	|| ($n->localName() eq 'b' && $n->parentNode()->localName() eq 'v');
    my $i = 0;
    $body =~ tr/ʾ/'/;
    if ($v) {
	my $r = $n->getAttributeNS($GDL,'role');
	if ($r && $r eq 'logo') {
	    # FIXME: for now this just lets grapheme be lowercase roman
	    # but there should be an option for smallcaps or fullcaps
	} else {
	    my $p = ($n->localName() eq 'v' 
		     ? $n->parentNode() : $n->parentNode->parentNode());
	    unless ($p->localName() eq 'd') {
		my $l = lang($n);
		if ($l && $l eq 'akk') {
		    $i = 1;
		    $rtf->print(\$italic_o);
		}
	    }
	}
    }
    $rtf->print($body);
    $rtf->print(\$rc) if $i;
    if ($index) {
	$index =~ tr/₀-₉₊/0-9x/;
	g_sub($index);
    }
}

sub
a_g_o {
    my $n = shift;
    my $a = $n->getAttributeNS($GDL,'o');
    $rtf->print($a) if $a;
    my $h = $n->getAttributeNS($GDL,'ho');
    $rtf->print(\$ho,$hl,\$rc) if $h;
}

sub
a_g_c {
    my $n = shift;
    my $h = $n->getAttributeNS($GDL,'hc');
    $rtf->print(\$hc,$hr,\$rc) if $h;
    my $a = $n->getAttributeNS($GDL,'c');
    $rtf->print($a) if $a;
}

sub
lang {
    my $n = shift;
    while ($n && !$n->hasAttributeNS($XML,'lang')) {
	$n = $n->parentNode();
    }
    if ($n) {
	$n->getAttributeNS($XML,'lang');
    } else {
	undef;
    }
}

1;
