#! /usr/bin/perl -w
#--
# Perl binding of Hyper Estraier
#                                                        Copyright (C) 2004-2007 Mikio Hirabayashi
#  This file is part of Hyper Estraier.
#  Hyper Estraier is free software; you can redistribute it and/or modify it under the terms of
#  the GNU Lesser General Public License as published by the Free Software Foundation; either
#  version 2.1 of the License or any later version.  Hyper Estraier is distributed in the hope
#  that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
#  License for more details.
#  You should have received a copy of the GNU Lesser General Public License along with Hyper
#  Estraier; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
#  Boston, MA 02111-1307 USA.


use lib qw(./src/blib/lib ./src/blib/arch);
use strict;
use warnings;
use ExtUtils::testlib;
use Time::HiRes qw(gettimeofday);
use Estraier;
$Estraier::DEBUG = 1;


# global constants
use constant {
    TRUE => 1,
    FALSE => 0,
    PROTVER => "1.0",
    SEARCHMAX => 10,
    SEARCHAUX => 32,
    SNIPWWIDTH => 480,
    SNIPHWIDTH => 96,
    SNIPAWIDTH => 96,
    VM_ID => 0,
    VM_URI => 1,
    VM_ATTR => 2,
    VM_FULL => 3,
    VM_SNIP => 4,
    READMAX => 1024 * 1024 * 256,
};


# main routine
sub main {
    (scalar(@ARGV) >= 1) || usage();
    my $rv;
    if($ARGV[0] eq "put"){
        $rv = runput();
    } elsif($ARGV[0] eq "out"){
        $rv = runout();
    } elsif($ARGV[0] eq "edit"){
        $rv = runedit();
    } elsif($ARGV[0] eq "get"){
        $rv = runget();
    } elsif($ARGV[0] eq "uriid"){
        $rv = runuriid();
    } elsif($ARGV[0] eq "inform"){
        $rv = runinform();
    } elsif($ARGV[0] eq "optimize"){
        $rv = runoptimize();
    } elsif($ARGV[0] eq "merge"){
        $rv = runmerge();
    } elsif($ARGV[0] eq "search"){
        $rv = runsearch();
    } else {
        usage();
    }
    return $rv;
}


# print the usage and exit
sub usage {
    printf(STDERR "%s: command line utility for the core API of Hyper Estraier\n", $0);
    printf(STDERR "\n");
    printf(STDERR "usage:\n");
    printf(STDERR "  %s put [-cl] [-ws] db [file]\n", $0);
    printf(STDERR "  %s out [-cl] db expr\n", $0);
    printf(STDERR "  %s edit db expr name [value]\n", $0);
    printf(STDERR "  %s get db expr [attr]\n", $0);
    printf(STDERR "  %s uriid db uri\n", $0);
    printf(STDERR "  %s inform db\n", $0);
    printf(STDERR "  %s optimize [-onp] [-ond] db\n", $0);
    printf(STDERR "  %s merge [-cl] db target\n", $0);
    printf(STDERR "  %s search [-vu|-va|-vf|-vs] [-gs|-gf|-ga] [-cd] [-ni] [-sf|-sfr|-sfu|-sfi]" .
           " [-attr expr] [-ord expr] [-max num] [-sk num] [-aux num] [-dis name]" .
           " db [phrase]\n", $0);
    printf(STDERR "\n");
    exit(1);
}


# print error string and flush the buffer */
sub printerror {
    my $msg = shift;
    printf(STDERR "%s: ERROR: %s\n", $0, $msg);
}


# parse arguments of the put command
sub runput {
    my $dbname = undef;
    my $file = undef;
    my $opts = 0;
    my $i;
    for($i = 1; $i < scalar(@ARGV); $i++){
        if(!defined($dbname) && $ARGV[$i] =~ /^-/){
            if($ARGV[$i] eq "-cl"){
                $opts |= Database::PDCLEAN;
            } elsif($ARGV[$i] eq "-ws"){
                $opts |= Database::PDWEIGHT;
            } else {
                usage();
            }
        } elsif(!defined($dbname)){
            $dbname = $ARGV[$i];
        } elsif(!defined($file)){
            $file = $ARGV[$i];
        } else {
            usage();
        }
    }
    usage() if(!defined($dbname));
    procput($dbname, $file, $opts);
}


# parse arguments of the out command
sub runout {
    my $dbname = undef;
    my $expr = undef;
    my $opts = 0;
    my $i;
    for($i = 1; $i < scalar(@ARGV); $i++){
        if(!defined($dbname) && $ARGV[$i] =~ /^-/){
            if($ARGV[$i] eq "-cl"){
                $opts |= Database::ODCLEAN;
            } else {
                usage();
            }
        } elsif(!defined($dbname)){
            $dbname = $ARGV[$i];
        } elsif(!defined($expr)){
            $expr = $ARGV[$i];
        } else {
            usage();
        }
    }
    usage() if(!defined($dbname) || !defined($expr));
    procout($dbname, $expr, $opts);
}


# parse arguments of the edit command
sub runedit {
    my $dbname = undef;
    my $expr = undef;
    my $name = undef;
    my $value = undef;
    my $i;
    for($i = 1; $i < scalar(@ARGV); $i++){
        if(!defined($dbname) && $ARGV[$i] =~ /^-/){
            usage();
        } elsif(!defined($dbname)){
            $dbname = $ARGV[$i];
        } elsif(!defined($expr)){
            $expr = $ARGV[$i];
        } elsif(!defined($name)){
            $name = $ARGV[$i];
        } elsif(!defined($value)){
            $value = $ARGV[$i];
        } else {
            usage();
        }
    }
    usage() if(!defined($dbname) || !defined($expr) || !defined($name));
    procedit($dbname, $expr, $name, $value);
}


# parse arguments of the get command
sub runget {
    my $dbname = undef;
    my $expr = undef;
    my $attr = undef;
    my $i;
    for($i = 1; $i < scalar(@ARGV); $i++){
        if(!defined($dbname) && $ARGV[$i] =~ /^-/){
            usage();
        } elsif(!defined($dbname)){
            $dbname = $ARGV[$i];
        } elsif(!defined($expr)){
            $expr = $ARGV[$i];
        } elsif(!defined($attr)){
            $attr = $ARGV[$i];
        } else {
            usage();
        }
    }
    usage() if(!defined($dbname) || !defined($expr));
    procget($dbname, $expr, $attr);
}


# parse arguments of the uriid command
sub runuriid {
    my $dbname = undef;
    my $uri = undef;
    my $i;
    for($i = 1; $i < scalar(@ARGV); $i++){
        if(!defined($dbname) && $ARGV[$i] =~ /^-/){
            usage();
        } elsif(!defined($dbname)){
            $dbname = $ARGV[$i];
        } elsif(!defined($uri)){
            $uri = $ARGV[$i];
        } else {
            usage();
        }
    }
    usage() if(!defined($dbname) || !defined($uri));
    procuriid($dbname, $uri);
}


# parse arguments of the inform command
sub runinform {
    my $dbname = undef;
    my $i;
    for($i = 1; $i < scalar(@ARGV); $i++){
        if(!defined($dbname) && $ARGV[$i] =~ /^-/){
            usage();
        } elsif(!defined($dbname)){
            $dbname = $ARGV[$i];
        } else {
            usage();
        }
    }
    usage() if(!defined($dbname));
    procinform($dbname);
}


# parse arguments of the optimize command
sub runoptimize {
    my $dbname = undef;
    my $opts = 0;
    my $i;
    for($i = 1; $i < scalar(@ARGV); $i++){
        if(!defined($dbname) && $ARGV[$i] =~ /^-/){
            if($ARGV[$i] eq "-onp"){
                $opts |= Database::OPTNOPURGE;
            } elsif($ARGV[$i] eq "-ond"){
                $opts |= Database::OPTNODBOPT;
            } else {
                usage();
            }
        } elsif(!defined($dbname)){
            $dbname = $ARGV[$i];
        } else {
            usage();
        }
    }
    usage() if(!defined($dbname));
    procoptimize($dbname, $opts);

}


# parse arguments of the merge command
sub runmerge {
    my $dbname = undef;
    my $tgname = undef;
    my $opts = 0;
    my $i;
    for($i = 1; $i < scalar(@ARGV); $i++){
        if(!defined($dbname) && $ARGV[$i] =~ /^-/){
            if($ARGV[$i] eq "-cl"){
                $opts |= Database::MGCLEAN;
            } else {
                usage();
            }
        } elsif(!defined($dbname)){
            $dbname = $ARGV[$i];
        } elsif(!defined($tgname)){
            $tgname = $ARGV[$i];
        } else {
            usage();
        }
    }
    usage() if(!defined($dbname) || !defined($tgname));
    procmerge($dbname, $tgname, $opts);
}


# parse arguments of the search command
sub runsearch {
    my $dbname = undef;
    my $phrase = undef;
    my @attrs = ();
    my $ord = undef;
    my $max = SEARCHMAX;
    my $skip = 0;
    my $opts = 0;
    my $aux = SEARCHAUX;
    my $dis = undef;
    my $cd = FALSE;
    my $view = VM_ID;
    my $i;
    for($i = 1; $i < scalar(@ARGV); $i++){
        if(!defined($dbname) && $ARGV[$i] =~ /^-/){
            if($ARGV[$i] eq "-vu"){
                $view = VM_URI;
            } elsif($ARGV[$i] eq "-va"){
                $view = VM_ATTR;
            } elsif($ARGV[$i] eq "-vf"){
                $view = VM_FULL;
            } elsif($ARGV[$i] eq "-vs"){
                $view = VM_SNIP;
            } elsif($ARGV[$i] eq "-gs"){
                $opts |= Condition::SURE;
            } elsif($ARGV[$i] eq "-gf"){
                $opts |= Condition::FAST;
            } elsif($ARGV[$i] eq "-ga"){
                $opts |= Condition::AGITO;
            } elsif($ARGV[$i] eq "-cd"){
                $cd = TRUE;
            } elsif($ARGV[$i] eq "-ni"){
                $opts |= Condition::NOIDF;
            } elsif($ARGV[$i] eq "-sf"){
                $opts |= Condition::SIMPLE;
            } elsif($ARGV[$i] eq "-sfr"){
                $opts |= Condition::ROUGH;
            } elsif($ARGV[$i] eq "-sfu"){
                $opts |= Condition::UNION;
            } elsif($ARGV[$i] eq "-sfi"){
                $opts |= Condition::ISECT;
            } elsif($ARGV[$i] eq "-attr"){
                usage() if(++$i >= scalar(@ARGV));
                push(@attrs, $ARGV[$i]);
            } elsif($ARGV[$i] eq "-ord"){
                usage() if(++$i >= scalar(@ARGV));
                $ord = $ARGV[$i];
            } elsif($ARGV[$i] eq "-max"){
                usage() if(++$i >= scalar(@ARGV));
                $max = Estraier::atoi($ARGV[$i]);
            } elsif($ARGV[$i] eq "-sk"){
                usage() if(++$i >= scalar(@ARGV));
                $skip = Estraier::atoi($ARGV[$i]);
            } elsif($ARGV[$i] eq "-aux"){
                usage() if(++$i >= scalar(@ARGV));
                $aux = Estraier::atoi($ARGV[$i]);
            } elsif($ARGV[$i] eq "-dis"){
                usage() if(++$i >= scalar(@ARGV));
                $dis = $ARGV[$i];
            } else {
                usage();
            }
        } elsif(!defined($dbname)){
            $dbname = $ARGV[$i];
        } elsif(!defined($phrase)){
            $phrase = $ARGV[$i];
        } else {
            $phrase = $phrase . " " . $ARGV[$i];
        }
    }
    usage() if(!defined($dbname));
    procsearch($dbname, $phrase, \@attrs, $ord, $max, $skip, $opts, $aux, $dis, $cd, $view);
}


# perform the put command
sub procput {
    my $dbname = shift;
    my $file = shift;
    my $opts = shift;
    my $draft = "";
    if(defined($file)){
        unless(open(IN, "<$file")){
            printerror($dbname . ": could not open");
            return 1;
        }
        binmode(IN);
        sysread(IN, $draft, READMAX);
        close(IN);
    } else {
        binmode(STDIN);
        sysread(STDIN, $draft, READMAX);
    }
    my $doc = new Document($draft);
    my $db = new Database();
    unless($db->open($dbname, Database::DBWRITER | Database::DBCREAT)){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        return 1;
    }
    $db->set_informer("main::informer");
    unless($db->put_doc($doc, $opts)){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        $db->close();
        return 1;
    }
    unless($db->close()){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        return 1;
    }
    return 0;
}


# perform the out command
sub procout {
    my $dbname = shift;
    my $expr = shift;
    my $opts = shift;
    my $db = new Database();
    unless($db->open($dbname, Database::DBWRITER)){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        return 1;
    }
    $db->set_informer("main::informer");
    my $id = Estraier::atoi($expr);
    if($id < 1 && ($id = $db->uri_to_id($expr)) < 1){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        $db->close();
        return 1;
    }
    unless($db->out_doc($id, $opts)){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        $db->close();
        return 1;
    }
    unless($db->close()){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        return 1;
    }
    return 0;
}


# perform the edit command
sub procedit {
    my $dbname = shift;
    my $expr = shift;
    my $name = shift;
    my $value = shift;
    my $db = new Database();
    unless($db->open($dbname, Database::DBWRITER)){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        return 1;
    }
    $db->set_informer("main::informer");
    my $id = Estraier::atoi($expr);
    if($id < 1 && ($id = $db->uri_to_id($expr)) < 1){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        $db->close();
        return 1;
    }
    my $doc = $db->get_doc($id, Database::GDNOTEXT);
    unless(defined($doc)){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        $db->close();
        return 1;
    }
    $doc->add_attr($name, $value);
    unless($db->edit_doc($doc)){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        $db->close();
        return 1;
    }
    unless($db->close()){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        return 1;
    }
    return 0;
}


# perform the get command
sub procget {
    my $dbname = shift;
    my $expr = shift;
    my $attr = shift;
    my $db = new Database();
    unless($db->open($dbname, Database::DBREADER)){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        return 1;
    }
    my $id = Estraier::atoi($expr);
    if($id < 1 && ($id = $db->uri_to_id($expr)) < 1){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        $db->close();
        return 1;
    }
    if(defined($attr)){
        my $value = $db->get_doc_attr($id, $attr);
        unless(defined($value)){
            printerror($dbname . ": " . $db->err_msg($db->error()));
            $db->close();
            return 1;
        }
        printf("%s\n", $value);
    } else {
        my $doc = $db->get_doc($id, 0);
        unless(defined($doc)){
            printerror($dbname . ": " . $db->err_msg($db->error()));
            $db->close();
            return 1;
        }
        printf("%s", $doc->dump_draft());

    }
    unless($db->close()){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        return 1;
    }
    return 0;
}


# perform the uriid command
sub procuriid {
    my $dbname = shift;
    my $uri = shift;
    my $db = new Database();
    unless($db->open($dbname, Database::DBREADER)){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        return 1;
    }
    my $id = $db->uri_to_id($uri);
    unless($id > 0){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        $db->close();
        return 1;
    }
    printf("%d\n", $id);
    unless($db->close()){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        return 1;
    }
    return 0;
}


# perform the inform command
sub procinform {
    my $dbname = shift;
    my $db = new Database();
    unless($db->open($dbname, Database::DBREADER)){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        return 1;
    }
    printf("number of documents: %d\n", $db->doc_num());
    printf("number of words: %d\n", $db->word_num());
    printf("file size: %d\n", $db->size());
    unless($db->close()){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        return 1;
    }
    return 0;
}


# perform the optimize command
sub procoptimize {
    my $dbname = shift;
    my $opts = shift;
    my $db = new Database();
    unless($db->open($dbname, Database::DBWRITER)){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        return 1;
    }
    $db->set_informer("main::informer");
    unless($db->optimize($opts)){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        $db->close();
        return 1;
    }
    unless($db->close()){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        return 1;
    }
    return 0;
}


# perform the merge command
sub procmerge {
    my $dbname = shift;
    my $tgname = shift;
    my $opts = shift;
    my $db = new Database();
    unless($db->open($dbname, Database::DBWRITER)){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        return 1;
    }
    $db->set_informer("main::informer");
    unless($db->merge($tgname, $opts)){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        $db->close();
        return 1;
    }
    unless($db->close()){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        return 1;
    }
    return 0;
}


# perform the search command
sub procsearch {
    my $dbname = shift;
    my $phrase = shift;
    my $attrs = shift;
    my $ord = shift;
    my $max = shift;
    my $skip = shift;
    my $opts = shift;
    my $aux = shift;
    my $dis = shift;
    my $cd = shift;
    my $view = shift;
    my $db = new Database();
    unless($db->open($dbname, Database::DBREADER)){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        return 1;
    }
    my $cond = new Condition();
    $cond->set_phrase($phrase) if(defined($phrase));
    foreach my $expr (@$attrs) {
        $cond->add_attr($expr);
    }
    $cond->set_order($ord) if(defined($ord));
    $cond->set_max($max) if($max >= 0);
    $cond->set_skip($skip) if($skip >= 0);
    $cond->set_options($opts);
    $cond->set_auxiliary($aux);
    $cond->set_distinct($dis) if(defined($dis));
    my ($sec, $usec) = gettimeofday();
    my $stime = $sec + $usec / 1000000.0;
    my $res = $db->search($cond);
    ($sec, $usec) = gettimeofday();
    my $etime = $sec + $usec / 1000000.0;
    my $border = sprintf("--------[%.0f]--------", $stime * 100);
    printf("%s\n", $border);
    printf("VERSION\t%s\n", PROTVER);
    printf("NODE\tlocal\n");
    printf("HIT\t%d\n", $res->hint(""));
    my @snwords = ();
    my $words = $res->hint_words();
    my $i;
    foreach my $word (@$words){
        $i++;
        my $hits = $res->hint($word);
        push(@snwords, $word) if $hits > 0;
        printf("HINT#%d\t%s\t%d\n", $i, $word, $hits)
    }
    printf("TIME\t%0.3f\n", $etime - $stime);
    printf("DOCNUM\t%d\n", $db->doc_num());
    printf("WORDNUM\t%d\n", $db->word_num());
    if($view == VM_URI){
        printf("VIEW\tURI\n");
    } elsif($view == VM_ATTR){
        printf("VIEW\tATTRIBUTE\n");
    } elsif($view == VM_FULL){
        printf("VIEW\tFULL\n");
    } elsif($view == VM_SNIP){
        printf("VIEW\tSNIPPET\n");
    } else {
        printf("VIEW\tID\n");
    }
    printf("\n");
    printf("%s\n", $border) if($view != VM_ATTR && $view != VM_FULL && $view != VM_SNIP);
    my $dnum = $res->doc_num();
    foreach my $i (0..$dnum-1){
        my $id = $res->get_doc_id($i);
        if($view == VM_URI){
            my $doc = $db->get_doc($id, $cd ? 0 : Database::GDNOTEXT);
            next unless defined($doc);
            next if $cd && !$db->scan_doc($doc, $cond);
            printf("%d\t%s\n", $id, $doc->attr('@uri'));
        } elsif($view == VM_ATTR){
            my $doc = $db->get_doc($id, $cd ? 0 : Database::GDNOTEXT);
            next unless defined($doc);
            printf("%s\n", $border);
            my $names = $doc->attr_names();
            foreach my $name (@$names){
                printf("%s=%s\n", $name, $doc->attr($name));
            }
            printf("\n");
        } elsif($view == VM_FULL){
            my $doc = $db->get_doc($id, 0);
            next unless defined($doc);
            printf("%s\n", $border);
            printf("%s", $doc->dump_draft());
        } elsif($view == VM_SNIP){
            my $doc = $db->get_doc($id, 0);
            next unless defined($doc);
            printf("%s\n", $border);
            my $names = $doc->attr_names();
            foreach my $name (@$names){
                printf("%s=%s\n", $name, $doc->attr($name));
            }
            printf("\n");
            printf("%s", $doc->make_snippet(\@snwords, SNIPWWIDTH, SNIPHWIDTH, SNIPAWIDTH));
        } else {
            printf("%d\n", $id);
        }
    }
    printf("%s:END\n", $border);
    unless($db->close()){
        printerror($dbname . ": " . $db->err_msg($db->error()));
        return 1;
    }
    return 0;
}


# callback function for database events
sub informer {
    printf("%s: INFO: %s\n", $0, shift);
}


# perform the main routine
$0 =~ s/.*\///;
exit(main());



# END OF FILE
