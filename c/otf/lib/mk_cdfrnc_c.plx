#!/usr/bin/perl
use warnings; use strict;

my %files = ();
my @lines = ();
my %namespaces = ();

load_file("cdf.rnc");
print "#include <string.h>\n#include <stdlib.h>\n";
print "const char * const cdf_rnc[] = {\n";
print "  \"#Concatenated cdf.rnc schema built by mk_cdfrnc_c.plx\\n\",\n";
print "  \"#See http://enlil.museum.upenn.edu/cdl/doc/CDF/ for more info\\n\",\n";
print "  \"default namespace = \\\"$namespaces{'default'}\\\"\\n\",\n";
foreach my $ns (keys %namespaces) {
    next if $ns eq 'default';
    print "  \"namespace $ns = \\\"$namespaces{$ns}\\\"\\n\",\n";
}
print_file(@{$files{"cdf.rnc"}});
print "  NULL };\n";
print <<EOF;
int cdfrnc_len = 0;
static char *cdfbuf;
char *
cdfrnc()
{
  const char *const*p = cdf_rnc;
  if (!cdfrnc_len)
    {
      while (*p)
        cdfrnc_len += strlen(*p++);
      cdfbuf = malloc(cdfrnc_len+1);
      *cdfbuf = '\\0';
      p = cdf_rnc;
      while (*p)
        strcat(cdfbuf,*p++);
    }
  return cdfbuf;
}
void
cdfrnc_term(void)
{
    free(cdfbuf);
}
EOF

sub
load_file {
    my $fname = shift;
    open(X,"$fname") || die "can't open $fname\n";
    @{$files{$fname}} = (<X>);
    close(X);
    chomp(@{$files{$fname}});
    foreach my $l (@{$files{$fname}}) {
	if ($l =~ /^include\s+\"(.*?)"/) {
	    my $fn = $1;
	    load_file($fn);
	} elsif ($l =~ /^\s*default\s*namespace\s*=\s*\"(.*?)\"/) {
	    $namespaces{'default'} = $1;
	} elsif ($l =~ /^\s*namespace\s+(\S+)\s*=\s*\"(.*?)\"/) {
	    $namespaces{$1} = $2;
	}
    }
}

sub
print_file {
    foreach my $l (@_) {
	if ($l =~ /^include\s+\"(.*?)\"/) {
	    my $fn = $1;
	    print_file(@{$files{$fn}});
	} elsif ($l =~ /^\s*(?:default)?\s*namespace/) {
	    ; # skip namespace lines
	} else {
	    print "  \"",escape($l),"\\n\",\n";
	}
    }
}

sub
escape {
    my $tmp = shift;
    $tmp =~ s/\\/\\\\/g;
    $tmp =~ s/"/\\"/g;
    $tmp;
}

1;
