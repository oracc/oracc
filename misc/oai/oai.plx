#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
my %args = @ARGV;

validate();

my $response_date = `date -u +\%FT\%TZ`; chomp $args{'date'};
my $request = 'http://oracc.org/oai';

my %funcs = (
    GetRecord=>\&GetRecord,
    Identify=>\&Identify,
    ListIdentifiers=>\&ListIdentifiers,
    ListMetadataFormats=>\&ListMetadataFormats,
    ListRecords=>\&ListRecords,
    ListSets=>\&ListSets,
    );

foreach my $a (sort keys %args) {
    print "request passed validation:\n";
    print "$a => $args{$a}\n";
}

#if ($args{'verb'}) {
#    if ($funcs{$args{'verb'}}) {
#	&$funcs{$args{'verb'}};
#    } else {
#	oai_error();
#    }
#} else {
#    oai_error();
#}

############################################################################################

my @verbs = qw/GetRecord Identify ListIdentifiers ListMetadataFormats ListRecords ListSets/;
my %verbs = (); @verbs{@verbs} = ();
my @oracc_args = qw/date duplicate project request/;
my %oracc_args = (); @oracc_args{@oracc_args} = ();

my @verbargs = qw/
  GetRecord:identifier GetRecord:metadataPrefix
  ListIdentifiers:from ListIdentifiers:until ListIdentifiers:metadataPrefix ListIdentifiers:set ListIdentifiers:resumptionToken
  ListMetadataFormats:identifier
  ListRecords:from ListRecords:until ListRecords:metadataPrefix ListRecords:set ListRecords:resumptionToken
  ListSets:resumptionToken
    /;

my %allowed_args = ();
my %verbargs = ();
foreach my $va (@oracc_args, @verbargs) {
    if ($va =~ /:/) {
	++$verbargs{$va};
	$va =~ s/^.*?://;
    }
    ++$allowed_args{$va};
}

sub validate {
    # is there a verb and is it legal
    badVerb() unless $args{'verb'} && exists($verbs{$args{'verb'}});
    # are the arguments legal for this verb?
    badArgument() if check_args();
}

sub check_args {
    # first see if the server told us about duplicate arguments
    return 1 if $args{'duplicates'};
    # now see if we have any illegal args
    foreach my $a (keys %args) {
	return 1 unless exists $allowed_args{$a};
    }
    # now check if all the args are allowed for the given verb
    foreach my $a (keys %args) {
	next if exists $oracc_args{$a};
	my $va = "$args{'verb'}\:$a";
	return 1 unless exists $verbargs{$va};
    }
    return 0;
}

############################################################################################

sub badArgument {
    er('badArgument','Missing, repeated or illegal argument',$response_date,$request);
}

sub badVerb {
    er('badVerb','Illegal OIA verb',$date,$base_url);
}

sub er {
    my($err,$str,$dat,$url) = @_;
    print <<ER;
<?xml version="1.0" encoding="UTF-8"?>
<OAI-PMH xmlns="http://www.openarchives.org/OAI/2.0/" 
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://www.openarchives.org/OAI/2.0/
         http://www.openarchives.org/OAI/2.0/OAI-PMH.xsd">
  <responseDate>$dat</responseDate>
  <request>$url</request>
  <error code="$err">$str</error>
</OAI-PMH>
ER
    exit 0;
}
1;
