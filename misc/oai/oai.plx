#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
use Data::Dumper;

my %args = @ARGV;
my $response_date = `date -u +\%FT\%TZ`; chomp $response_date;
my $request = $args{'request'};

### Handle server-detected error first ###
warn "$ARGV[4] ; $ARGV[5]\n";
if ($ARGV[4] =~ /^bad/) {
     er($ARGV[4],$ARGV[5],$response_date,$request);   
}

### Validation configuration ###

my @verbs = qw/GetRecord Identify ListIdentifiers ListMetadataFormats ListRecords ListSets/;
my %verbs = (); @verbs{@verbs} = ();
my @oracc_args = qw/project request verb/; # cheat to say 'verb' is an oracc_arg but we don't want to arg check it
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

#######

my $earliest_date = `date -u +\%FT\%TZ`; chomp $earliest_date;

#foreach my $a (keys %args) {
#    warn "oai.plx: $a = $args{$a}\n";
#}

validate();

my %funcs = (
    GetRecord=>\&GetRecord,
    Identify=>\&Identify,
    ListIdentifiers=>\&ListIdentifiers,
    ListMetadataFormats=>\&ListMetadataFormats,
    ListRecords=>\&ListRecords,
    ListSets=>\&ListSets,
    );

#warn "request passed validation\n";

&{$funcs{$args{'verb'}}};

############################################################################################

sub validate {
#    warn Dumper \%verbs;
#    warn Dumper \%allowed_args;
#    warn Dumper \%oracc_args;
#    warn Dumper \%verbargs;
    # is there a verb and is it legal
    badVerb() unless $args{'verb'} && exists($verbs{$args{'verb'}});
    # are the arguments legal for this verb?
    badArgument() if check_args();
}

sub check_args {
    # check if all the args are allowed for the given verb
    foreach my $a (keys %args) {
	next if exists $oracc_args{$a};
	my $va = "$args{'verb'}\:$a";
#	warn "check_args trying $va\n";
	return 1 unless exists $verbargs{$va};
    }
    return 0;
}

sub verb_args {
    my %a = (); @a{@_} = ();
    my $thisargs = '';
    foreach my $arg (keys %args) {
	next if exists $oracc_args{$arg};
	# if this arg is a required or optional arg keep checking, otherwise error
	badArgument() unless exists $a{$arg};
	$thisargs .= " $arg=\"$args{$arg}\"";
    }
    $thisargs;
}

############################################################################################

sub badArgument {
    er('badArgument','Missing, repeated or illegal argument',$response_date,$request);
}

sub badVerb {
    er('badVerb','Illegal OIA verb',$response_date,$request);
}

sub er {
    my($err,$str,$dat,$url) = @_;
    print <<EOF;
<?xml version="1.0" encoding="UTF-8"?>
<OAI-PMH xmlns="http://www.openarchives.org/OAI/2.0/" 
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://www.openarchives.org/OAI/2.0/
         http://www.openarchives.org/OAI/2.0/OAI-PMH.xsd">
  <responseDate>$dat</responseDate>
  <request>$url</request>
  <error code="$err">$str</error>
</OAI-PMH>
EOF
    exit 0;
}

############################################################################################

sub Identify {
    verb_args();

    print <<EOF;
<?xml version="1.0" encoding="UTF-8"?>
<OAI-PMH xmlns="http://www.openarchives.org/OAI/2.0/" 
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://www.openarchives.org/OAI/2.0/
         http://www.openarchives.org/OAI/2.0/OAI-PMH.xsd">
  <responseDate>$response_date</responseDate>
  <request verb="Identify">$request</request>
  <Identify>
    <repositoryName>Oracc OIA Repository</repositoryName>
    <baseURL>$request</baseURL>
    <protocolVersion>2.0</protocolVersion>
    <adminEmail>stinney\@upenn.edu</adminEmail>
    <earliestDatestamp>$earliest_date</earliestDatestamp>
    <deletedRecord>no</deletedRecord>
    <granularity>YYYY-MM-DDThh:mm:ssZ</granularity>
    <description>
      <oai_dc:dc 
        xmlns:oai_dc="http://www.openarchives.org/OAI/2.0/oai_dc/" 
        xmlns:dc="http://purl.org/dc/elements/1.1/" 
        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
        xsi:schemaLocation="http://www.openarchives.org/OAI/2.0/oai_dc/ 
        http://www.openarchives.org/OAI/2.0/oai_dc.xsd">
        <dc:description>Oracc is a collaborative effort to develop
a complete corpus of cuneiform whose rich annotation and open
licensing support the next generation of scholarly research. Created
by Steve Tinney, Oracc is steered by Jamie Novotny, Eleanor Robson,
Tinney, and Niek Veldhuis.</dc:description>
        <dc:publisher>Oracc</dc:publisher>
        <dc:rights>Metadata may be used without restrictions.</dc:rights>
      </oai_dc:dc>
    </description>
 </Identify>
</OAI-PMH>
EOF
    exit 0;
}

sub ListMetadataFormats {
    my $thisargs = verb_args('identifier');
    print <<EOF;
<?xml version="1.0" encoding="UTF-8"?>
<OAI-PMH xmlns="http://www.openarchives.org/OAI/2.0/" 
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://www.openarchives.org/OAI/2.0/
         http://www.openarchives.org/OAI/2.0/OAI-PMH.xsd">
  <responseDate>2002-06-08T15:19:13Z</responseDate>
  <request verb="ListMetadataFormats"$thisargs>$request</request>
  <ListMetadataFormats>
   <metadataFormat>
    <metadataPrefix>oai_dc</metadataPrefix>
    <schema>http://www.openarchives.org/OAI/2.0/oai_dc.xsd</schema>
    <metadataNamespace>http://www.openarchives.org/OAI/2.0/oai_dc/
      </metadataNamespace>
   </metadataFormat>
</OAI-PMH>
EOF
    exit 0;
}

sub ListSets {
    print <<EOH;
<?xml version="1.0" encoding="UTF-8"?>
<OAI-PMH xmlns="http://www.openarchives.org/OAI/2.0/" 
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://www.openarchives.org/OAI/2.0/
         http://www.openarchives.org/OAI/2.0/OAI-PMH.xsd">
  <responseDate>$response_date</responseDate> 
  <request verb="ListSets">$request</request>
  <error code="noSetHierarchy">This repository does not support sets</error>
</OAI-PMH>
EOH
    exit 0;
}

1;
