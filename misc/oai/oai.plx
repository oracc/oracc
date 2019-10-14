#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use Data::Dumper;

my %args = @ARGV;
my $response_date = `date -u +\%FT\%TZ`; chomp $response_date;
my $request = $args{'request'};

### Handle server-detected error first ###
#warn "$ARGV[4] ; $ARGV[5]\n";
if ($ARGV[4] =~ /^bad/) {
     er($ARGV[4],$ARGV[5],$response_date,$request);   
}

### Validation configuration ###

my @verbs = qw/GetRecord Identify ListIdentifiers ListMetadataFormats ListRecords ListSets/;
my %verbs = (); @verbs{@verbs} = ();
my @oracc_args = qw/project request verb/; # it's a cheat to say 'verb' is an oracc_arg
					   # but we don't want to arg check it
my %oracc_args = (); @oracc_args{@oracc_args} = ();

my @verbargs = qw/
  GetRecord:identifier GetRecord:metadataPrefix
  ListIdentifiers:from ListIdentifiers:until ListIdentifiers:metadataPrefix ListIdentifiers:set ListIdentifiers:resumptionToken
  ListMetadataFormats:identifier
  ListRecords:from ListRecords:until ListRecords:metadataPrefix ListRecords:set ListRecords:resumptionToken
  ListSets:resumptionToken
    /;

my @verbargs_r = qw/
  GetRecord:identifier GetRecord:metadataPrefix
  ListIdentifiers:metadataPrefix
  ListRecords:metadataPrefix
    /;

my %allowed_args = ();
my %verbargs = ();
foreach my $va (@oracc_args, @verbargs) {
    my $tmp = $va;
    if ($va =~ /:/) {
	++$verbargs{$va};
	$tmp =~ s/^.*?://;
    }
    ++$allowed_args{$tmp};
}

my %args4verbs = ();
foreach my $va (@verbargs) {
    $va =~ m/^(.*?):(.*?)$/;
    push @{$args4verbs{$1}}, $2;
}

my %args4verbs_r = ();
foreach my $va (@verbargs_r) {
    $va =~ m/^(.*?):(.*?)$/;
    push @{$args4verbs_r{$1}}, $2;
}

#######

my $earliest_date = `head -1 $ENV{'ORACC_BUILDS'}/agg/projects/public-project-dates.tab|cut -f2`;
chomp $earliest_date;

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

my $valid_args = 0;

sub is_oai_iso_date {
    $_[0] =~ /^\d\d\d\d-\d\d-\d\d(?:T\d\d:\d\d:\d\dZ)?$/;
}

sub validate {
#    warn Dumper \%verbs;
#    warn Dumper \%allowed_args;
#    warn Dumper \%oracc_args;
#    warn Dumper \%verbargs;
    # is there a verb and is it legal
    badVerb() unless $args{'verb'} && exists($verbs{$args{'verb'}});
    # do we have all the arguments we need for this verb?
    badArgument() if need_args();
    # are the arguments legal for this verb?
    badArgument() if check_args();
    # if there is a resumptionToken are there, wrongly, other arguments?
    badArgument() if $args{'resumptionToken'} && $valid_args > 1;
    # if from or until are given, check their formats
    badArgument() if $args{'from'} && !is_oai_iso_date($args{'from'});
    badArgument() if $args{'until'} && !is_oai_iso_date($args{'until'});
    badArgument() if ($args{'from'} && $args{'until'}) && $args{'from'} ge $args{'until'};
}

sub check_args {
    # check if all the args are allowed for the given verb
    foreach my $a (keys %args) {
	next if exists $oracc_args{$a};
	my $va = "$args{'verb'}\:$a";
	return 1 unless exists $verbargs{$va};
	++$valid_args;
    }
    return 0;
}

sub need_args {
    foreach my $a (@{$args4verbs_r{$args{'verb'}}}) {
	return 1 unless exists $args{$a};
    }
    return 0;
}

sub verb_args {
    my $thisargs = '';
    my %a = (); @a{@{$args4verbs{$args{'verb'}}}} = ();
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

sub badResumptionToken {
    er('badResumptionToken','this repository never uses resumption tokens',$response_date,$request);
}

sub badVerb {
    er('badVerb','Illegal OAI verb',$response_date,$request);
}

sub cannotDisseminateFormat {
    er('cannotDisseminateFormat','this repository exclusively serves oai_dc metadata',$response_date,$request,
       $_[0]);
}

sub idDoesNotExist {
    er('idDoesNotExist','unknown identifier',$response_date,$request,$_[0]);
}

sub noRecordsMatch {
    er('noRecordsMatch','selection criteria do not match any records',$response_date,$request,$_[0]);
}

sub noSetHierarchy {
    er('noSetHierarchy','This repository does not support sets',$response_date,$request,$_[0]);
}

sub er {
    my($err,$str,$dat,$url,$args) = @_;
    if ($args) {
	$args = " verb=\"$args{'verb'}\"".$args;
    } else {
	$args = "";
    }
    print <<EOF;
<?xml version="1.0" encoding="UTF-8"?>
<OAI-PMH xmlns="http://www.openarchives.org/OAI/2.0/" 
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://www.openarchives.org/OAI/2.0/
         http://www.openarchives.org/OAI/2.0/OAI-PMH.xsd">
  <responseDate>$dat</responseDate>
  <request$args>$url</request>
  <error code="$err">$str</error>
</OAI-PMH>
EOF
    exit 0;
}

######################################################################################
# Helper routines for verb handlers

sub is_identifier {
    my $id = shift;
    my $ok = `grep '$id	' $ENV{'ORACC_BUILDS'}/agg/projects/public-project-dates.tab`;
    chomp $ok;
    $ok;
}

sub outputRecords {
    my %need = ();
    @need{@_} = ();
    my $x = load_xml("$ENV{'ORACC_BUILDS'}/agg/projects/public-projects.oai");
    my @or = tags($x,undef,'oai-record');
    
    my $thisargs = verb_args();
    print <<EOF;
<?xml version="1.0" encoding="UTF-8"?>
<OAI-PMH xmlns="http://www.openarchives.org/OAI/2.0/" 
         xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
         xsi:schemaLocation="http://www.openarchives.org/OAI/2.0/
         http://www.openarchives.org/OAI/2.0/OAI-PMH.xsd">
  <responseDate>$response_date</responseDate>
  <request verb="$args{'verb'}"$thisargs>$request</request>
  <$args{'verb'}>
EOF
    if ($args{'verb'} eq 'ListIdentifiers') {
	foreach my $or (@or) {
	    if (exists $need{$or->getAttribute('id')}) {
		print $or->firstChild()->firstChild();
	    }
	}
    } else {
	foreach my $or (@or) {
	    if (exists $need{$or->getAttribute('id')}) {
		print $or->firstChild();
	    }
	}
    }
    print <<EOF;
  </$args{'verb'}>
</OAI-PMH>
EOF
}

#######################################################################################

sub GetRecord {
    my $thisargs = verb_args();
    idDoesNotExist() unless is_identifier($args{'identifier'});
    cannotDisseminateFormat($thisargs)
	unless $args{'metadataPrefix'} && $args{'metadataPrefix'} eq 'oai_dc';
    outputRecords($args{'identifier'});
}

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

sub ListIdentifiers {
    my $thisargs = verb_args();
    noSetHierarchy($thisargs) if $args{'set'};
    cannotDisseminateFormat($thisargs)
	unless $args{'metadataPrefix'} && $args{'metadataPrefix'} eq 'oai_dc';
    my @r = `cat $ENV{'ORACC_BUILDS'}/agg/projects/public-project-dates.tab`;
    chomp @r;
    if ($args{'from'}) {
	my @new_r = ();
	foreach my $r (@r) {
	    my($id,$ds) = split(/\t/,$r);
	    if ($ds ge $args{'from'}) {
		push @new_r, $r;
	    }
	}
	@r = @new_r;
    }
    if ($args{'until'}) {
	my @new_r = ();
	foreach my $r (@r) {
	    my($id,$ds) = split(/\t/,$r);
	    if ($ds le $args{'until'}) {
		push @new_r, $r;
	    }
	}
	@r = @new_r;
    }
    if ($#r >= 0) {
	my @o = ();
	foreach my $r (@r) {
	    $r =~ s/\t.*$//;
	    push @o, $r;
	}
	outputRecords(@o);
    } else {
	noRecordsMatch($thisargs);
    }
}

sub ListRecords {
    ListIdentifiers(); # works because different output is based on args{'verb'}
}

sub ListMetadataFormats {
    my $thisargs = verb_args();
    if ($args{'identifier'}) {
	my $idinfo = is_identifier($args{'identifier'});
	idDoesNotExist($thisargs) unless $idinfo;
    }
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
    <metadataNamespace>http://www.openarchives.org/OAI/2.0/oai_dc/</metadataNamespace>
   </metadataFormat>
</OAI-PMH>
EOF
    exit 0;
}

sub ListSets {
    noSetHierarchy();
    exit 0;
}

1;
