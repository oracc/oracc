use strict;
use warnings;
use Estraier;
$Estraier::DEBUG = 1;

# create the database object
my $db = new Database();

# open the database
unless($db->open("casket", Database::DBREADER)){
    printf("error: %s\n", $db->err_msg($db->error()));
    exit;
}

# create a search condition object
my $cond = new Condition();

# set the search phrase to the search condition object
$cond->set_phrase("rainbow AND lullaby");

# get the result of search
my $result = $db->search($cond);

# for each document in the result
my $dnum = $result->doc_num();
foreach my $i (0..$dnum-1){
    # retrieve the document object
    my $doc = $db->get_doc($result->get_doc_id($i), 0);
    next unless(defined($doc));
    # display attributes
    my $uri = $doc->attr('@uri');
    printf("URI: %s\n", $uri) if defined($uri);
    my $title = $doc->attr('@title');
    printf("Title: %s\n", $title) if defined($title);
    # display the body text
    my $texts = $doc->texts();
    foreach my $text (@$texts){
        printf("%s\n", $text);
    }
}

# close the database
unless($db.close()){
    printf("error: %s\n", $db->err_msg($db->error()));
}
