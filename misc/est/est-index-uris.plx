#!/usr/bin/perl
use strict; use warnings;
use Estraier;
$Estraier::DEBUG = 1;

my $dbname = shift @ARGV;

die "est-index-uris.plx: must give EST index name on commandline\n"
    unless $dbname;

if (-d $dbname) {
    exec 'rm', '-fr', $dbname;
}

my $doc = undef;

# create the database object
my $db = new Database();

# open the database
unless($db->open($dbname, Database::DBWRITER | Database::DBCREAT)){
    printf("error: %s\n", $db->err_msg($db->error()));
    exit;
}

while (<>) {
    chomp;
    next if /^\s*$/;
    if (/^\@title/) {
	if ($doc) {
	    # register the document object to the database
	    unless($db->put_doc($doc, Database::PDCLEAN)){
		printf("error: %s\n", $db->err_msg($db->error()));
	    }
	    undef $doc;
	}
	# create a document object
	$doc = new Document();
	s/^.*?=//;
	$doc->add_attr('@title', $_);
    } elsif (/^\@uri/) {
	if ($doc) {
	    s/^.*?=//;
	    $doc->add_attr('@uri', $_);
	} else {
	    die "est-index-uris.plx: attempt to set index data before '\@title' was found\n";
	}
    } else {
	if ($doc) {
	    $doc->add_text($_);
	} else {
	    die "est-index-uris.plx: attempt to set index data before '\@title' was found\n";
	}
    }
}

if ($doc) {
    unless($db->put_doc($doc, Database::PDCLEAN)){
	printf("error: %s\n", $db->err_msg($db->error()));
    }
}

# close the database
unless($db->close()){
    printf("error: %s\n", $db->err_msg($db->error()));
}

1;
