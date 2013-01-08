#!/usr/bin/perl

my @subdirs = grep(-d $_ && -e "$_/Makefile.am", <*>);

my @EXTRA = @ARGV;
push @EXTRA, <etc/*>;
my $last_extra = pop @EXTRA;

open(M,">Makefile.am"); select M;

print "include \@top_srcdir\@/oracc-doc-rules.txt\n\n";

print <<EOM;
SUBDIRS = @subdirs

all-local:
	perl \$(top_srcdir)/misc/xdf/xdfmanager.plx

clean-local:
	rm -fr html

install-data-local:
	\$(top_srcdir)/oracc-doc-install.sh \$(DESTDIR)\$(prefix) \$(top_srcdir)

EOM

print "EXTRA_DIST = \\\n";
foreach my $e (@EXTRA) {
    print "\t$e \\\n";
}
print "\t$last_extra\n";

close(M);


1;
