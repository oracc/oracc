#-------------------------------------------------------------------------------------------------
# Perl binding of Hyper Estraier
#                                                       Copyright (C) 2004-2007 Mikio Hirabayashi
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
#-------------------------------------------------------------------------------------------------


package Estraier;

use strict;
use warnings;
use Carp;

require Exporter;
require XSLoader;
our @ISA = qw(Exporter);
our $VERSION = '1.0';
our $DEBUG = 0;
XSLoader::load('Estraier', $VERSION);

use constant {
    TRUE => 1,
    FALSE => 0,
};


sub atoi {
    my $str = shift;
    $str =~ s/^ *//;
    my $sign = 1;
    if($str =~ /^-/){
        $sign = -1;
        $str =~ s/^-*//
    }
    return 0 unless $str =~ /^[0-9]+/;
    my $t = 0;
    foreach my $d (split(//, $str)){
        last unless($d =~ /^[0-9]$/);
        $t = $t * 10 + $d;
    }
    return $t * $sign;
}



package Document;
use Carp;


sub new {
    my $class = shift;
    my $draft = shift;
    if(scalar(@_) != 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    my $self = [0];
    $$self[0] = defined($draft) ? Estraier::doc_new_from_draft($draft) : Estraier::doc_new();
    bless($self, $class);
    return $self;
}


sub new_with_ptr {
    my $class = shift;
    my $ptr = shift;
    if(scalar(@_) != 0 || !defined($ptr)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    my $self = [0];
    $$self[0] = $ptr;
    bless($self, $class);
    return $self;
}


sub DESTROY {
    my $self = shift;
    return undef unless($$self[0]);
    Estraier::doc_delete($$self[0]);
    return undef;
}


sub add_attr {
    my $self = shift;
    my $name = shift;
    my $value = shift;
    if(scalar(@_) != 0 || !defined($name)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    $value = "\t(NULL)\t" if(!defined($value));
    Estraier::doc_add_attr($$self[0], $name, $value);
    return undef;
}


sub add_text {
    my $self = shift;
    my $text = shift;
    if(scalar(@_) != 0 || !defined($text)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    Estraier::doc_add_text($$self[0], $text);
    return undef;
}


sub add_hidden_text {
    my $self = shift;
    my $text = shift;
    if(scalar(@_) != 0 || !defined($text)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    Estraier::doc_add_hidden_text($$self[0], $text);
    return undef;
}


sub set_keywords {
    my $self = shift;
    my $kwords = shift;
    if(scalar(@_) != 0 || !defined($kwords) || ref($kwords) ne "HASH"){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    Estraier::doc_set_keywords($$self[0], $kwords);
    return undef;
}


sub set_score {
    my $self = shift;
    my $score = shift;
    if(scalar(@_) != 0 || !defined($score) || $score < 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    Estraier::doc_set_score($$self[0], $score);
    return undef;
}


sub id {
    my $self = shift;
    if(scalar(@_) != 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    return Estraier::doc_id($$self[0]);
}


sub attr_names {
    my $self = shift;
    if(scalar(@_) != 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    return Estraier::doc_attr_names($$self[0]);
}


sub attr {
    my $self = shift;
    my $name = shift;
    if(scalar(@_) != 0 || !defined($name)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    return Estraier::doc_attr($$self[0], $name);
}


sub texts {
    my $self = shift;
    if(scalar(@_) != 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    return Estraier::doc_texts($$self[0]);
}


sub cat_texts {
    my $self = shift;
    if(scalar(@_) != 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    return Estraier::doc_cat_texts($$self[0]);
}


sub keywords {
    my $self = shift;
    if(scalar(@_) != 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    return Estraier::doc_keywords($$self[0]);
}


sub score {
    my $self = shift;
    if(scalar(@_) != 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    return Estraier::doc_score($$self[0]);
}


sub dump_draft {
    my $self = shift;
    if(scalar(@_) != 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    return Estraier::doc_dump_draft($$self[0]);
}


sub make_snippet {
    my $self = shift;
    my $words = shift;
    my $wwidth = shift;
    my $hwidth = shift;
    my $awidth = shift;
    if(scalar(@_) != 0 || !defined($words) || ref($words) ne "ARRAY" ||
       !defined($wwidth) || $wwidth < 0 || !defined($hwidth) || $hwidth < 0 ||
       !defined($awidth) || $awidth < 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    return Estraier::doc_make_snippet($$self[0], $words, $wwidth, $hwidth, $awidth);
}



package Condition;
use Carp;


use constant {
    SURE => 1 << 0,
    USUAL => 1 << 1,
    FAST => 1 << 2,
    AGITO => 1 << 3,
    NOIDF => 1 << 4,
    SIMPLE => 1 << 10,
    ROUGH => 1 << 11,
    UNION => 1 << 15,
    ISECT => 1 << 16,
    ECLSIMURL => 10.0,
    ECLSERV => 100.0,
    ECLDIR => 101.0,
    ECLFILE => 102.0,
};


sub new {
    my $class = shift;
    if(scalar(@_) != 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    my $self = [0];
    $$self[0] = Estraier::cond_new();
    bless($self, $class);
    return $self;
}


sub DESTROY {
    my $self = shift;
    return undef unless($$self[0]);
    Estraier::cond_delete($$self[0]);
    return undef;
}


sub set_phrase {
    my $self = shift;
    my $phrase = shift;
    if(scalar(@_) != 0 || !defined($phrase)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    Estraier::cond_set_phrase($$self[0], $phrase);
    return undef;
}


sub add_attr {
    my $self = shift;
    my $expr = shift;
    if(scalar(@_) != 0 || !defined($expr)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    Estraier::cond_add_attr($$self[0], $expr);
    return undef;
}


sub set_order {
    my $self = shift;
    my $expr = shift;
    if(scalar(@_) != 0 || !defined($expr)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    Estraier::cond_set_order($$self[0], $expr);
    return undef;
}


sub set_max {
    my $self = shift;
    my $max = shift;
    if(scalar(@_) != 0 || !defined($max) || $max < 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    Estraier::cond_set_max($$self[0], $max);
    return undef;
}


sub set_skip {
    my $self = shift;
    my $skip = shift;
    if(scalar(@_) != 0 || !defined($skip) || $skip < 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    Estraier::cond_set_skip($$self[0], $skip);
    return undef;
}


sub set_options {
    my $self = shift;
    my $options = shift;
    if(scalar(@_) != 0 || !defined($options)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    Estraier::cond_set_options($$self[0], $options);
    return undef;
}


sub set_auxiliary {
    my $self = shift;
    my $min = shift;
    if(scalar(@_) != 0 || !defined($min)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    Estraier::cond_set_auxiliary($$self[0], $min);
    return undef;
}


sub set_eclipse {
    my $self = shift;
    my $limit = shift;
    if(scalar(@_) != 0 || !defined($limit)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    Estraier::cond_set_eclipse($$self[0], $limit);
    return undef;
}


sub set_distinct {
    my $self = shift;
    my $name = shift;
    if(scalar(@_) != 0 || !defined($name)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    Estraier::cond_set_distinct($$self[0], $name);
    return undef;
}



package Result;
use Carp;


sub new {
    my $class = shift;
    my $resptr = shift;
    my $idxsptr = shift;
    my $resnum = shift;
    my $hints = shift;
    my $cond = shift;
    if(scalar(@_) != 0 || !defined($resptr) || !defined($resnum) ||
       !defined($hints) || !defined($cond)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    my $self = [$resptr, $idxsptr, $resnum, $hints, $cond];
    bless($self, $class);
    return $self;
}


sub DESTROY {
    my $self = shift;
    return undef unless($$self[0]);
    Estraier::res_delete($$self[0], defined($$self[1]) ? $$self[1] : 0, $$self[3], $$self[4]);
    return undef;
}


sub doc_num {
    my $self = shift;
    if(scalar(@_) != 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return -1;
    }
    return $$self[2];
}


sub get_doc_id {
    my $self = shift;
    my $index = shift;
    if(scalar(@_) != 0 || !defined($index)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return -1;
    }
    return -1 if($index < 0 || $index >= $$self[2]);
    return Estraier::res_get_doc_id($$self[0], $index);
}


sub get_dbidx {
    my $self = shift;
    my $index = shift;
    if(scalar(@_) != 0 || !defined($index)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return -1;
    }
    return -1 if(!defined($$self[1]) || $index < 0 || $index >= $$self[2]);
    return Estraier::res_get_dbidx($$self[1], $index);
}


sub hint_words {
    my $self = shift;
    if(scalar(@_) != 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    return Estraier::res_hint_words($$self[3]);
}


sub hint {
    my $self = shift;
    my $word = shift;
    if(scalar(@_) != 0 || !defined($word)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return 0;
    }
    return Estraier::res_hint($$self[3], $word);
}


sub get_score {
    my $self = shift;
    my $index = shift;
    if(scalar(@_) != 0 || !defined($index)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return -1;
    }
    return -1 if($index < 0 || $index >= $$self[2]);
    return Estraier::res_get_score($$self[4], $index);
}


sub get_shadows {
    my $self = shift;
    my $id = shift;
    if(scalar(@_) != 0 || !defined($id) || $id < 1){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    return Estraier::res_get_shadows($$self[4], $id);
}



package Database;
use Carp;


sub VERSION {
    return Estraier::db_version();
}


use constant {
    ERRNOERR => 0,
    ERRINVAL => 1,
    ERRACCES => 2,
    ERRLOCK => 3,
    ERRDB => 4,
    ERRIO => 5,
    ERRNOITEM => 6,
    ERRMISC => 9999,
    DBREADER => 1 << 0,
    DBWRITER => 1 << 1,
    DBCREAT => 1 << 2,
    DBTRUNC => 1 << 3,
    DBNOLCK => 1 << 4,
    DBLCKNB => 1 << 5,
    DBPERFNG => 1 << 10,
    DBCHRCAT => 1 << 11,
    DBSMALL => 1 << 20,
    DBLARGE => 1 << 21,
    DBHUGE => 1 << 22,
    DBHUGE2 => 1 << 23,
    DBHUGE3 => 1 << 24,
    DBSCVOID => 1 << 25,
    DBSCINT => 1 << 26,
    DBSCASIS => 1 << 27,
    IDXATTRSEQ => 0,
    IDXATTRSTR => 1,
    IDXATTRNUM => 2,
    OPTNOPURGE => 1 << 0,
    OPTNODBOPT => 1 << 1,
    MGCLEAN => 1 << 0,
    PDCLEAN => 1 << 0,
    PDWEIGHT => 1 << 1,
    ODCLEAN => 1 << 0,
    GDNOATTR => 1 << 0,
    GDNOTEXT => 1 << 1,
    GDNOKWD => 1 << 2,
};


sub new {
    my $class = shift;
    if(scalar(@_) != 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    my $self = [0, ERRNOERR, undef];
    bless($self, $class);
    return $self;
}


sub DESTROY {
    my $self = shift;
    $self->close();
}


sub search_meta {
    my $dbs = shift;
    my $cond = shift;
    if(scalar(@_) != 0 || !defined($dbs) || ref($dbs) ne "ARRAY" ||
       !defined($cond) || ref($cond) ne "Condition"){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    my @dbptrs;
    foreach my $elem (@$dbs){
        return undef if(ref($elem) ne "Database" || !$$elem[0]);
        push(@dbptrs, $$elem[0]);
    }
    my ($resptr, $idxsptr, $resnum, $hints, $tcond) =
        Estraier::db_search_meta(\@dbptrs, $$cond[0]);
    return new Result($resptr, $idxsptr, $resnum, $hints, $tcond);
}


sub err_msg {
    my $self = shift;
    my $ecode = shift;
    if(scalar(@_) != 0 || !defined($ecode)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    return Estraier::db_err_msg($ecode);
}


sub open {
    my $self = shift;
    my $name = shift;
    my $omode = shift;
    if(scalar(@_) != 0 || !defined($name) || !defined($omode)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return Estraier::FALSE;
    }
    if($$self[0]){
        $$self[1] = ERRMISC;
        return Estraier::FALSE;
    }
    ($$self[0], $$self[1]) = Estraier::db_open($name, $omode);
    return $$self[0] ? Estraier::TRUE : Estraier::FALSE;
}


sub close {
    my $self = shift;
    if(scalar(@_) != 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return Estraier::FALSE;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return Estraier::FALSE;
    }
    my $ok;
    ($ok, $$self[1]) = Estraier::db_close($$self[0]);
    $$self[0] = 0;
    return $ok ? Estraier::TRUE : Estraier::FALSE;
}


sub error {
    my $self = shift;
    return $$self[1];
}


sub fatal {
    my $self = shift;
    if(scalar(@_) != 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return Estraier::FALSE;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return Estraier::FALSE;
    }
    return Estraier::db_fatal($$self[0]);
}


sub add_attr_index {
    my $self = shift;
    my $name = shift;
    my $type = shift;
    if(scalar(@_) != 0 || !defined($name) || !defined($type)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return Estraier::FALSE;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return Estraier::FALSE;
    }
    if(!Estraier::db_add_attr_index($$self[0], $name, $type)){
        $$self[1] = Estraier::db_error($$self[0]);
        return Estraier::FALSE;
    }
    return Estraier::TRUE;
}


sub flush {
    my $self = shift;
    my $max = shift;
    if(scalar(@_) != 0 || !defined($max)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return Estraier::FALSE;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return Estraier::FALSE;
    }
    if(!Estraier::db_flush($$self[0], $max)){
        $$self[1] = Estraier::db_error($$self[0]);
        return Estraier::FALSE;
    }
    return Estraier::TRUE;
}


sub sync {
    my $self = shift;
    if(scalar(@_) != 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return Estraier::FALSE;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return Estraier::FALSE;
    }
    if(!Estraier::db_sync($$self[0])){
        $$self[1] = Estraier::db_error($$self[0]);
        return Estraier::FALSE;
    }
    return Estraier::TRUE;
}


sub optimize {
    my $self = shift;
    my $options = shift;
    if(scalar(@_) != 0 || !defined($options)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return Estraier::FALSE;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return Estraier::FALSE;
    }
    if(!Estraier::db_optimize($$self[0], $options)){
        $$self[1] = Estraier::db_error($$self[0]);
        return Estraier::FALSE;
    }
    return Estraier::TRUE;
}


sub merge {
    my $self = shift;
    my $name = shift;
    my $options = shift;
    if(scalar(@_) != 0 || !defined($name) || !defined($options)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return Estraier::FALSE;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return Estraier::FALSE;
    }
    if(!Estraier::db_merge($$self[0], $name, $options)){
        $$self[1] = Estraier::db_error($$self[0]);
        return Estraier::FALSE;
    }
    return Estraier::TRUE;
}


sub put_doc {
    my $self = shift;
    my $doc = shift;
    my $options = shift;
    if(scalar(@_) != 0 || !defined($doc) || ref($doc) ne "Document" || !defined($options)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return Estraier::FALSE;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return Estraier::FALSE;
    }
    if(!Estraier::db_put_doc($$self[0], $$doc[0], $options)){
        $$self[1] = Estraier::db_error($$self[0]);
        return Estraier::FALSE;
    }
    return Estraier::TRUE;
}


sub out_doc {
    my $self = shift;
    my $id = shift;
    my $options = shift;
    if(scalar(@_) != 0 || !defined($id) || $id < 1 || !defined($options)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return Estraier::FALSE;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return Estraier::FALSE;
    }
    if(!Estraier::db_out_doc($$self[0], $id, $options)){
        $$self[1] = Estraier::db_error($$self[0]);
        return Estraier::FALSE;
    }
    return Estraier::TRUE;
}


sub edit_doc {
    my $self = shift;
    my $doc = shift;
    if(scalar(@_) != 0 || !defined($doc) || ref($doc) ne "Document"){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return Estraier::FALSE;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return Estraier::FALSE;
    }
    if(!Estraier::db_edit_doc($$self[0], $$doc[0])){
        $$self[1] = Estraier::db_error($$self[0]);
        return Estraier::FALSE;
    }
    return Estraier::TRUE;
}


sub get_doc {
    my $self = shift;
    my $id = shift;
    my $options = shift;
    if(scalar(@_) != 0 || !defined($id) || $id < 1 || !defined($options)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return undef;
    }
    my $docptr = Estraier::db_get_doc($$self[0], $id, $options);
    if(!$docptr){
        $$self[1] = Estraier::db_error($$self[0]);
        return undef;
    }
    return new_with_ptr Document($docptr);
}


sub get_doc_attr {
    my $self = shift;
    my $id = shift;
    my $name = shift;
    if(scalar(@_) != 0 || !defined($id) || $id < 1 || !defined($name)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return undef;
    }
    my $value = Estraier::db_get_doc_attr($$self[0], $id, $name);
    if(!$value){
        $$self[1] = Estraier::db_error($$self[0]);
        return undef;
    }
    return $value;
}


sub uri_to_id {
    my $self = shift;
    my $uri = shift;
    if(scalar(@_) != 0 || !defined($uri)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return -1;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return -1;
    }
    my $id = Estraier::db_uri_to_id($$self[0], $uri);
    if($id < 1){
        $$self[1] = Estraier::db_error($$self[0]);
        return -1;
    }
    return $id;
}


sub name {
    my $self = shift;
    if(scalar(@_) != 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return undef;
    }
    return Estraier::db_name($$self[0]);
}


sub doc_num {
    my $self = shift;
    if(scalar(@_) != 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return -1;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return -1;
    }
    return Estraier::db_doc_num($$self[0]);
}


sub word_num {
    my $self = shift;
    if(scalar(@_) != 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return -1;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return -1;
    }
    return Estraier::db_word_num($$self[0]);
}


sub size {
    my $self = shift;
    if(scalar(@_) != 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return -1;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return -1;
    }
    return Estraier::db_size($$self[0]);
}


sub search {
    my $self = shift;
    my $cond = shift;
    if(scalar(@_) != 0 || !defined($cond) || ref($cond) ne "Condition"){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return undef;
    }
    my ($resptr, $resnum, $hints, $tcond) = Estraier::db_search($$self[0], $$cond[0]);
    return new Result($resptr, undef, $resnum, $hints, $tcond);
}


sub scan_doc {
    my $self = shift;
    my $doc = shift;
    my $cond = shift;
    if(scalar(@_) != 0 || !defined($doc) || ref($doc) ne "Document" ||
       !defined($cond) || ref($cond) ne "Condition"){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return undef;
    }
    return Estraier::db_scan_doc($$self[0], $$doc[0], $$cond[0]);
}


sub set_cache_size {
    my $self = shift;
    my $size = shift;
    my $anum = shift;
    my $tnum = shift;
    my $rnum = shift;
    if(scalar(@_) != 0 || !defined($size) ||
       !defined($anum) || !defined($tnum) || !defined($rnum)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return undef;
    }
    Estraier::db_set_cache_size($$self[0], $size, $anum, $tnum, $rnum);
    return undef;
}


sub add_pseudo_index {
    my $self = shift;
    my $path = shift;
    if(scalar(@_) != 0 || !defined($path)){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return undef;
    }
    return Estraier::db_add_pseudo_index($$self[0], $path);
}


sub set_wildmax {
    my $self = shift;
    my $num = shift;
    if(scalar(@_) != 0 || !defined($num) || $num < 0){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return undef;
    }
    Estraier::db_set_wildmax($$self[0], $num);
    return undef;
}


sub set_informer {
    my $self = shift;
    my $informer = shift;
    if(scalar(@_) != 0 || !defined($informer) || ref(\$informer) ne "SCALAR"){
        croak(__FILE__ . ": invalid parameter") if($Estraier::DEBUG);
        return undef;
    }
    unless($$self[0]){
        $$self[1] = ERRMISC;
        return undef;
    }
    $$self[2] = $informer;
    Estraier::db_set_informer($$self[0], $informer);
    return undef;
}



Estraier::TRUE;


# END OF FILE
