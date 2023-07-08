
#if 0
    if ($caller eq 'esp') {
	my $pr1 = $pr;
	$pr1 =~ s/\s.*$//;
	my $letter = letter_of($pr1);
	# my $html = "https://@@ORACC_HOST@@/$ixproject/signlist/$letter/$pr1/index.html";
	my $html = "/$ixproject/signlist/$letter/$pr1/index.html";
	warn "slse-slave: caller=esp so returning $html\n";
	if ($ext) {
	    esp_ext($pr);
	    exit 0;
	} else {
	    print "Status: 302 Found\nLocation: $html\n\n";
	}
    }
#endif 

void
sll_esp_ext(const char *key, const char *ext, List *lp)
{
  char *r;
  int i = 0;
  sll_esp_header(key,ext);
  for (r = list_first(lp); r; r = list_next(lp))
    {
      char *s = r;
      const char *id, *v, *sn, *p;
      if (i++)
	p = ";";
      else
	p = "";
      while (*s)
	{
	  if (*s == 'o' && isdigit(s[1]))
	    id = s;
	  else if (islower(*s))
	    v = s;
	  else
	    sn = s;
	  while (*s && '\t' != *s)
	    ++s;
	  if (*s)
	    *s++ = '\t';
	}
      sll_esp_p(id, sn, v, p);
    }
  sll_esp_trailer();
}

#if 0
sub
esp_ext {
    my $pr = shift;
    esp_html_header();
    my %known = ();
    my $first_id = undef;
    my @ss = split(/\s+/,$pr);
    for (my $i = 0; $i <= $#ss; ++$i) {
	my $s = $ss[$i];
	my $punct = ($i < $#ss ? ';' : '');
	next if $known{$s}++;
	if ($s =~ m,/,) {
	    my ($id,$n) = ($s =~ m,^(.*?)/(.*?)$,);
	    $first_id = $id unless $first_id;
	    if ($n =~ /^~[a-z]+/) {
		# do nothing
	    } else {
		if ($n >= 1000) {
		    $n = 'x';
		} elsif ($n == 0) {
		    $n = '';
		}
	    }
	    my $sn = slse("$id\;name");
	    my $vh = $v;
	    if (is_signlist($vh)) {
		esp_printhtml($id,$sn,"$vh$n",$punct);
	    } else {
		esp_printhtml($id,$sn,"$vh<sub>$n</sub>",$punct);
	    }
	} else {
	    my $sn = slse("$s;name");
	    esp_printhtml($s,$sn);
	}
    }
    esp_html_trailer();
}
#endif

void
sll_esp_header(const char *v, const char *ext)
{
  char *vcat = NULL;
  struct sllext *ep = sllext(ext, strlen(ext));
  vcat = malloc(strlen(v) + strlen(ep->pre) + strlen(ep->pst) + 1);
  (void)sprintf(vcat, "%s%s%s", ep->pre, v, ep->pst);
  printf("%s\n\n", "Content-type: text/html; charset=utf-8");
  printf("%s\n", "<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"sux\" xml:lang=\"sux\">");
  printf("%s\n", "<head><meta http-equiv="Content-Type" content=\"text/html; charset=utf-8\" />");
  printf("<title>OGSL Results for %s</title>\n", vcat);
  printf("%s\n", "<link rel=\"shortcut icon\" type=\"image/ico\" href=\"/favicon.ico\" />");
  printf("%s\n", "<link rel=\"stylesheet\" type="text/css" href="/css/p3.css" />");
  printf("%s\n", "<script src=\"/js/p3.js\" type="text/javascript"><![CDATA[ ]]></script>");
  printf("%s\n", "</head><body class=\"ogslres\">");
  printf("<h1 class=\"ogslres\">%s</h1>\n", vcat);
  free(vcat);
}

#if 0
sub
esp_html_header {
    my $vcat = $v;
    $vcat = "$ext_pre{$ext}$vcat$ext_post{$ext}";
    print <<EOH;
Content-type: text/html; charset=utf-8

<html xmlns="http://www.w3.org/1999/xhtml" lang="sux" xml:lang="sux">
<head><meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>OGSL Results for $vcat</title>
<link rel="shortcut icon" type="image/ico" href="/favicon.ico" />
<link rel="stylesheet" type="text/css" href="/css/p3.css" />
<script src="/js/p3.js" type="text/javascript"><![CDATA[ ]]></script>
</head><body class="ogslres">
EOH
print "<h1 class=\"ogslres\">$vcat</h1>\n";
}
#endif

void
sll_esp_trailer(void)
{
  printf("%s\n", "</body></html>");
}
#if 0
sub
esp_html_trailer {
    print '</body></html>',"\n";
}
#endif

void
sll_esp_p(const char *oid, const char *sn, const char *v, const char *p)
{
  const char *vx = (v ? "&#xa0;=&#xa0;" : "");
  const char *pspan = (p ? "<span class=\"ogsl-punct\">;</span>" : "");
  const char html[1024];
  const char *letter = sll_lookup(sll_tmp_key(oid,"d"));
  (void)sprintf(html, "/%s/signlist/%s/%s/index.html", project, letter, oid);
  printf("<p><a target=\"slmain\" href=\"%s\">%s<span class=\"sign\">%s</span></a>%s</p>\n",
	 html, v, sn, pspan);
}

#if 0
sub
esp_printhtml {

  my $letter = letter_of($id);
    #    my $html = "https://@@ORACC_HOST@@/$ixproject/signlist/$letter/$id/index.html";
    my $html = "/$ixproject/signlist/$letter/$id/index.html";
    print "<p><a target=\"slmain\" href=\"$html\">$v<span class=\"sign\">$sn</span></a>$pspan</p>\n";
}
#endif
