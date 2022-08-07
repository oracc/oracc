#!/usr/bin/perl
use warnings; use strict;

my %files = ();
my @lines = ();
my %namespaces = ();

load_file("cbd.rnc");
print "#include <string.h>\n#include <stdlib.h>\n";
print "const char * const cbd_rnc[] = {\n";
print "  \"#Concatenated cbd.rnc schema built by mk_cbdrnc_c.plx\\n\",\n";
print "  \"default namespace = \\\"$namespaces{'default'}\\\"\\n\",\n";
foreach my $ns (keys %namespaces) {
    next if $ns eq 'default';
    print "  \"namespace $ns = \\\"$namespaces{$ns}\\\"\\n\",\n";
}
print_file(@{$files{"cbd.rnc"}});
print "  NULL };\n";
print <<EOF;
int cbdrnc_len = 0;
static char *cbdbuf;
char *
cbdrnc()
{
  const char *const*p = cbd_rnc;
  if (!cbdrnc_len)
    {
      while (*p)
        cbdrnc_len += strlen(*p++);
      cbdbuf = malloc(cbdrnc_len+1);
      *cbdbuf = '\\0';
      p = cbd_rnc;
      while (*p)
        strcat(cbdbuf,*p++);
    }
  return cbdbuf;
}
void
cbdrnc_term(void)
{
    free(cbdbuf);
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
