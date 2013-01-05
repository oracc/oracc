#!/usr/bin/perl
use warnings; use strict;
BEGIN { push @INC, '/usr/local/share/cdl/signlists/psl/tools' };
use open 'utf8';
use GDBM_File;
use PSL; PSL::init();
use XML::LibXML;

my %keys;
my %db;
my $db_file = '/usr/local/share/epsd/db/epsd-keys';
my $summary_file = '/usr/local/share/epsd/db/epsd-summaries';
my $xhtml_uri = 'http://www.w3.org/1999/xhtml';
my $grep_file = '/usr/local/share/epsd/db/epsd-grep';

open(GREP,">$grep_file");
open(IN,"tools/getkeys.sh|");
while (<IN>) {
    if (/^(..:e\d+):(.*)$/) {
	my($id,$key) = ($1,$2);
	if (/^tr:/) {
	    my $akey = PSL::alias_words($key);
# WATCHME: we only write the aliased key to GREP because the only searches
# that should be getting made are aliased ones--is this OK?
	    if ($akey ne $key) {
#		print STDERR "index-db.plx: aliasing $key to $akey\n";
		push @{$keys{$akey}}, $id;
		print GREP $akey,"\n";
	    } else {
		print GREP $key,"\n";
	    }
	}
	push @{$keys{$key}}, $id;
    } else {
	die "syntax error: $_";
    }
}
close(IN);
close(GREP);

unlink $db_file;
tie %db, 'GDBM_File', $db_file, &GDBM_WRCREAT, 0644;
foreach my $k (keys %keys) {
    die unless defined $k;
    $db{$k} = join("\cA", @{$keys{$k}});
}
untie %db;

unlink $summary_file;
my $xp = XML::LibXML->new();
my $sdoc = $xp->parse_file('results/summaries.html');
tie %db, 'GDBM_File', $summary_file, &GDBM_WRCREAT, 0644;
foreach my $s ($sdoc->getDocumentElement()->getElementsByTagNameNS($xhtml_uri,'p')) {
    my $key = $s->getAttribute('id');
    if ($key) {
	my $summary = ($s->childNodes())[0]->toString();
	$db{$key} = $summary;
    }
}
untie %db;

0;
