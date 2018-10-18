#!/usr/bin/perl
use warnings; use strict; use open ':utf8';
use Encode;

binmode STDERR, ':utf8'; binmode STDOUT, ':utf8';

my %curr_chars = ();
my $curr_lang = '';
my $curr_mode = '';
my %curr_trans = ();
my %trans_recip = ();
my %curr_script = ();
my $did_u2u = 0;
my $in_charset = 0;
my $status = 0;
my $order = '';

my @charsets = ();

my %mode_of = (
    n => 'm_normalized',
    a => 'm_alphabetic',
    g => 'm_graphemic',
    );

print <<EOH;
#include <ctype.h>
#include "warning.h"
#include "atf.h"
#include "charsets.h"
#define append(s) strcpy(buf+buf_used,s); buf_used+=strlen(s)
#define wcharcat(s) buf[buf_used++] = s
int chartrie_suppress_errors = 0;
EOH

open(C, "charsets.txt");
while (<C>) {
    next if /^\s*$/ || /^\#/;
    if (/^\@chars/) {
	if ($in_charset) {
	    bad("\@chars missing \@end chars");
	} else {
	    $in_charset = 1;
	}
	if (s/\@chars\s\%(...)//) {
	    $curr_lang = $1;
	}
	if (s#^/(.)##) {
	    $curr_mode = $1;
	} else {
	    bad("\@chars $curr_lang missing mode specifier");
	}
	if (!/^\s*$/) {
	    bad("\@chars contains junk");
	}
    } elsif (s/^\@order\s+//) {
	check_charset('@order');
	chomp;
	$order = $_;
	if ($order ne 'reverse') {
	    bad("\@order $order: unknown 'order' value");
	}
    } elsif (s/^\@allow\s+//) {
	check_charset('@allow');
	s/\s*$//;
	%curr_chars = ();
	@curr_chars{grep(defined,split(/(.)/,$_))} = ();
    } elsif (s/^\@nokey\s+//) {
	check_charset('@nokey');
	s/\s*$//;
	foreach my $c (grep(defined,split(/(.)/,$_))) {
	    if (exists $curr_chars{$c}) {
		$curr_chars{$c} = '';
		$trans_recip{$c} = '';
	    } else {
		bad("char '$c' in \@nokey but not in \@allow");
	    }
	}
    } elsif (s/^\@trans\s*$//) {
	check_charset('@trans');
	while (<C>) {
	    last if m/^\@end\s+chars\s*$/;
	    # @trans lines are the input transliteration, the ouptput transliteration
	    # and optionally the unicode letter in the target script that the unicode
	    # transliteration represents, expressed as a C-style 0xHEX constant.
	    my($in,$out,$uni) = (m/^(.*?)\s+(.*?)(?:\s+(\S+))?\s*$/);
	    if (defined $in) {
		if (exists $curr_chars{$out}) {
		    $curr_trans{$in} = $out;
		    $trans_recip{$out} = $in;
		    if ($uni) {
			$curr_script{$out} = $uni;
		    }
		} else {
		    bad("result of translation '$in' not in \@allow");
		}
	    } else {
		chomp;
		bad("$_: syntax error in \@trans");
	    }
	}
	finish_charset();
    } elsif (/^\@end\s+chars\s*$/) {
	finish_charset();
    } else {
	chomp;
	bad("$_: syntax error in \@chars");
    }
}
close(C);

print <<EOP;

struct charset *cset_index[c_count][modes_top];

static void
set_charset(struct charset *cp)
{
  cset_index[cp->lcode][cp->mcode] = cp;
}

struct charset *
get_charset(enum langcode lcode, enum t_modes mcode)
{
  return cset_index[lcode][mcode];
}

static void
free_charset(struct charset *cp)
{
  if (cp->asc)
    cp->asc(NULL,0);
  if (cp->u2u)
    cp->u2u(NULL,0);
  if (cp->to_uni)
    inctrie_free(cp->to_uni);
}

void
charsets_init(void)
{
EOP
    foreach my $c (@charsets) {
	print "  set_charset(\&$c);\n";
    }
print <<EOP;
    subdig_init();
}

void
charsets_term(void)
{
EOP

    foreach my $c (@charsets) {
	print "  free_charset(\&$c);\n";
    }
print <<EOP;
    subdig_term();
}

void
chartrie_init(struct charset*cp)
{
  int i;
  cp->to_uni = inctrie_create();
  for (i = 0; cp->keys[i].key; ++i)
    inctrie_insert(cp->to_uni,cp->keys[i].key,cp->keys[i].val,0);
}

void
chartrie_not(const char *str, const char *sofar,struct charset *cp,
	     const char *f, size_t l)
{
    if (!isspace(*sofar) && !chartrie_suppress_errors)
	vwarning2(f,l,"%s: character '%c' (hex %x) not allowed in %s/%s",sofar,*sofar,*sofar,cp->lang,cp->mode);
}

void
charset_init_lang(struct lang_context *lp)
{
  enum t_modes i;
  for (i = 0; i < modes_top; ++i)
    {
      struct charset *cp = get_charset(lp->core->code,i);
      if (cp)
	{
	  if (cp->keys)
	    chartrie_init(cp);
	  lp->cset = cp;
	}
      else
        {
	    /* FIXME: we can't issue a diagnostic here, but we should
                when a lang/mode is actually used */
	  /*cp = get_default_charset();*/
	}
    }
}

EOP

####################################################

sub
bad {
    warn("charsets.txt:$.: ", @_, "\n");
    ++$status;
}

sub
check_charset {
    bad("$_[0] found outside of \@chars...\@end chars") unless $in_charset;
}

sub
finish_charset {
    return if $status;

    dump_val();
    dump_asc();
    dump_u2u();
    dump_keys();
    dump_struct();

    %curr_chars = ();
    $curr_lang = '';
    $curr_mode = '';
    %curr_trans = ();
    %curr_script = ();
    $did_u2u = $in_charset = $status = 0;
}

sub
case1 {
    my $x = sprintf("0x%05x",ord($_[0]));
    print <<EOC;
        case $x:
EOC
}

sub
case2 {
    my $x = sprintf("0x%05x",ord($_[0]));
    my $y = $_[1];
    if ($y eq '\\') {
	$y = sprintf("\\x%2x",ord($_[0]));
    }
    print <<EOC;
        case $x:
          append("$y");
          break;
EOC
}

sub
case3 {
    my $x = sprintf("0x%05x",ord($_[0]));
    print <<EOC;
        case $x:
          wcharcat($_[1]);
          break;
EOC
}

sub
hexify {
    my @u = (unpack('C*',encode("utf8",$_[0])));
    my $ustr = '';
    foreach my $u (@u) {
	$ustr .= sprintf("\\x%02x",$u);
    }
    $ustr;
}

sub
dump_val {
    print <<EOP;
static int
${curr_lang}_${curr_mode}_val(wchar_t *w, ssize_t len)
{
  int errs = 0;
  ssize_t i;
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
EOP
    foreach my $k (keys %curr_chars) {
	case1($k);
    }
    print <<EOP;
          break;
        default:
          vwarning("Character %s (hex %x) not allowed in %%$curr_lang/$curr_mode",wcs2utf(&w[i],1),(int)w[i]);
          ++errs;
          break;
	}
    }
  return errs;
}

EOP
}

sub
dump_asc {
    print <<EOP;
static char *
${curr_lang}_${curr_mode}_asc(wchar_t *w, ssize_t len)
{
  static char *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
	  free(buf);
	  buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced < (2 * len))
    {
      while  (buf_alloced <= (2 * len))
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced);
    }
  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
EOP
    foreach my $k (keys %curr_chars) {
	next if ord($k) == 0;
	if (defined $trans_recip{$k}) {
	    case2($k,$trans_recip{$k} || $k);
	} else {
	    if (ord($k) > 0x80) {
		bad("Unicode char $k has no ASCII form in $curr_lang/$curr_mode");
	    } else {
		case2($k,$k);
	    }
	}
    }
# shouldn't be a need for this if we run _val properly
#vwarning("Unicode %s not allowed in %$curr_lang/$curr_mode",*w);
    print <<EOP;
        default:
          break;
	}
    }
  return buf;
}

EOP
}

sub
dump_u2u {
    return unless scalar keys %curr_script && !$did_u2u;

    $did_u2u = 1;

    print <<EOP;
static wchar_t *
${curr_lang}_u2u(wchar_t *w, ssize_t len)
{
  static wchar_t *buf;
  static int buf_alloced, buf_used;
  ssize_t i;

  if (!w)
    {
      if (buf)
        {
          free(buf);
          buf_alloced = buf_used = 0;
	}
      return buf = NULL;
    }

  if (buf_alloced <= len)
    {
      while  (buf_alloced <= len)
	buf_alloced += 128;
      buf = realloc(buf,buf_alloced * sizeof(wchar_t));
    }

  for (i = 0; i < len; ++i)
    {
      switch (w[i])
        {
EOP
    foreach my $k (keys %curr_script) {
	next if ord($k) == 0;
	case3($k,$curr_script{$k});
    }
# shouldn't be a need for this if we run _val properly
#vwarning("Unicode %s not allowed in %$curr_lang/$curr_mode",*w);
    print <<EOP;
        default:
          break;
	}
    }
  return buf;
}

EOP
}

sub
dump_keys {
    print <<EOP;
struct charset_keys ${curr_lang}_${curr_mode}_keys[] =
  {
EOP
    foreach my $k (keys %curr_trans) {
	my $str = hexify($curr_trans{$k});
	print "    { \"$k\"\t, \"$str\" },\n"; # unless $k eq $curr_trans{$k};
    }
    foreach my $k (keys %curr_chars) {
	next if ord($k) > 0x80;
	next if $curr_trans{$k};
	next if defined $curr_chars{$k}; # curr_chars='' means char was in nokey
	if ($k eq '\\') {
	    $k = sprintf("\\x%2x",ord($k));
	}
	print "    { \"$k\"\t, \"$k\" },\n";
    }
    print <<EOP;
    { NULL, NULL },
  };
EOP
}

sub
dump_struct {
    my $u2u = 0;
    $u2u = "${curr_lang}_u2u" if scalar keys %curr_script;
    push @charsets, "${curr_lang}_${curr_mode}";
    print <<EOP;
struct charset ${curr_lang}_${curr_mode} =
  {
    \"$curr_lang\", c_$curr_lang , \"$curr_mode\", $mode_of{$curr_mode},
    ${curr_lang}_${curr_mode}_keys,
    0,
    ${curr_lang}_${curr_mode}_val,
    ${curr_lang}_${curr_mode}_asc,
    $u2u,
  };

EOP
}

1;
