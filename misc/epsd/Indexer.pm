package ORACC::SE::Indexer;
use warnings; use strict; use utf8;
#
# We index records that consist of items.  Each item has a type,
# a field, a literal key and a fuzzy key.
#
# Use: 
#     caller initializes by calling index().
#     caller then calls record() for each record
#     caller may call span() to establish intermediate levels of IDs
#     caller must call item for each key that is to be indexed
#     caller calls done() when done.
# 

binmode STDERR, ':utf8';

# This controls whether some slower testing is done, e.g., 
# checking the keys for illegal characters.
my $safe_mode = 1;

my %unit_counts = ();
my @unit_bases = ();

sub
index {
    my($name,$title,$xid,$uses_graphemes,$fields,$atf,$mgroup,$mrec) = @_;
    my %index = ();
    $index{'#name'} = $name;
    $index{'#title'} = $title;
    $index{'#xid'} = $xid;
    $index{'#uses_graphemes'} = $uses_graphemes;
    if (!defined($$fields[0])) {
	$index{'#fields'} = ['#'];
	$index{'#default_field'} = '#';
	@unit_bases = (0);
    } else {
	$index{'#fields'} = $fields;
	$index{'#default_field'} = $$fields[0];
	for (my $i = 0; $i <= $#{$index{'#fields'}}; ++$i) {
	    push @unit_bases, $i * 32;
	}
    }
    $index{'#atf_fields'} = join(':','',@$atf,'');
    $index{'#max_group'} = $mgroup;
    $index{'#max_record'} = $mrec;
    $index{'#group_sprint'} = '%02x';
    $index{'#record_sprint'} = '%04x';
    $index{'#unit_sprint'} = '%02x';
    $index{'#span_sprint'} = '%02x';
    $index{'#item_sprint'} = '%04x';
    \%index;
}

sub
done {
    ;
}

# Should this really be getting fixed by putting the default fuzzy
# keys in the index with the default prefix (e.g., X:gic etc.)?
sub
fuzzy {
    my($index,$field,$default,@fuz) = @_;
    my $d = undef;
    if ($default) {
	if (!$field || $$index{'#default_field'} ne $field) {
	    $d = $$index{'#default_field'};
	}
    }
    foreach my $f (@fuz) {
	my @f = split(/\s+/,$f);
	my $key = shift @f;
	if ($field) {
	    ++${$$index{"${field}:$key;x"}}{"${field}:$key;z"};
	    ++${$$index{"${field}:$key;z"}}{"${field}:$key"};
	    ++${$$index{"#${field}:item_grep"}}{$key};
	}
	if ($d) {
	    ++${$$index{"$key;x"}}{"$key;z"};
	    ++${$$index{"$key;z"}}{$key};
	    ++${$$index{"#${d}:item_grep"}}{$key};
	}
	foreach my $fuzzy (@f) {
	    if ($field) {
		++${$$index{"${field}:$fuzzy;x"}}{"${field}:$key;z"};
		++${$$index{"${field}:$key;z"}}{"${field}:$fuzzy"};
		++${$$index{"#${field}:item_grep"}}{$fuzzy};
	    }
	    if ($d) {
		++${$$index{"$fuzzy;x"}}{"$key;z"};
		++${$$index{"$key;z"}}{$fuzzy};
		++${$$index{"#${d}:item_grep"}}{$fuzzy};
	    }
	}
    }
}

#
# Record groups can be used to implement indexing
# of more than one text in a single index.
#
sub
group {
    my ($index,$xid,$title) = @_;
    my $gid = group_id($index,$xid);
    $$index{"$gid,title"} = $title;
    $gid;
}

#
# ARG1 is the index to which it belongs
# ARG2 is the external ID of the record; 
#      if defined it must be unique within the index
# ARG3 is a URL to the display form of the entry; if ARG3 is null, ARG4 is used
# ARG4 is an XHTML summary for the entry; 
#      if ARG3 is non-null, this should be a label suitable for link text
#      if ARG3 is null, this should be a complete XHTML para
#
sub
record {
    my($index,$group_id,$xid,$url,$summary) = @_;
    my $rid = record_id($index,$group_id||'',$xid);
    @unit_counts{@{$$index{'#fields'}}} = @unit_bases;
    if ($url) {
	$$index{"$rid,url"} = $url;
    }
    if ($summary) {
	$$index{"$rid,sum"} = $summary;
    }
    $rid;
}

sub
record_grep {
    my($index,$grep,$rid) = @_;
    ++${$$index{'#record_grep'}}{$grep};
    push @{$$index{$grep,'r'}}, $rid;
}

# If a database uses fields which can contain multiple units then the
# ID passed here must be a field-unit ID.
sub
field_grep {
    my($index,$fld,$grep,$fuid) = @_;
#    ++${$$index{"#${fld}_grep"}}{$grep};
#    push @{$$index{$grep,'f#'.$fld}}, $rid;
    $fld = $$index{'#default_field'} unless $fld;
    my $hkey = "#${fld}:$fuid";
#    if ($$index{$hkey}) {
#	$$index{$hkey} .= "\n$grep";
#    } else {
	$$index{$hkey} = $grep;
#    }
}

#
# ARG1 is the index to which it belongs
# ARG2 is the external ID of the record; if defined it must be unique within the index
#
sub
span {
    my($index,$record_id,$xid) = @_;
    my $sid = span_id($index,$record_id,$xid);
    $sid;
}

#
# ARG1 is the index to which the item(s) belong.
# ARG2 is the internal ID of record or span
# ARG3 is the key; an index entry to the record is automatically generated
# ARG4 is a fuzzy version of the key; multiple items can map to the same
#    fuzzy key for aliasing, etc.
# ARG5...ARGn are tuples of the form [ type , id , field ]
#
sub
item {
    my($index,$base_id,$xid,$key,$type,$field) = @_;
    if (!$key) {
	print STDERR "ORACC::SE::Indexer: attempt to add empty key\n";
	return;
    }
    # check the keys are only alphanumeric
    if ($safe_mode) {
	check_key($key,$$index{"$base_id,xid"} || $base_id);
    }
    $field = $$index{'#default_field'} unless $field;
#    if ($field) {
	++${$$index{"#$field:item_grep"}}{$key};
#    }
#    ++${$$index{'#item_grep'}}{$key};
    my $iid = item_id($index,$base_id,$xid);
    if ($type) {
	push @{$$index{"$type\#$key"}}, $iid;
	if ($field) {
	    push @{$$index{"$field\:$type\#$field"}}, $iid;
	}
    }
    push @{$$index{$key,$field}}, $iid;
    push @{$$index{"$field\:$key"}}, $iid;
}

sub
group_id {
    my($index,$xid,$title) = @_;
    my $h = sprintf($$index{'#group_sprint'},$$index{'#group_counter'}++); 
    my $id = "g$h";
    $$index{"$id,title"} = $title;
    if ($xid) {
	xid($index,$id,$xid);
    }
    $id;
}

sub
record_id {
    my($index,$base_id,$xid) = @_;
    my $h = sprintf($$index{'#record_sprint'},$$index{'#record_counter'}++); 
    my $id = $base_id.'r'.$h;
    $$index{'#span_counter'} = $$index{'#item_counter'} = 0;
    if ($xid) {
	xid($index,$id,$xid);
    }
    $id;
}

sub
field_unit {
    my($index,$fld,$base_id,$xid) = @_;
    my $h = sprintf($$index{'#unit_sprint'},$unit_counts{$fld}++||0); 
    my $id = $base_id.'u'.$h;
    $$index{'#item_counter'} = 0;
    if ($xid) {
	xid($index,$id,$xid);
    }
    $id;
}

sub
span_id {
    my($index,$base_id,$xid) = @_;
    my $h = sprintf($$index{'#span_sprint'},$$index{'#span_counter'}++); 
    my $id = $base_id.'s'.$h;
    $$index{'#item_counter'} = 0;
    if ($xid) {
	xid($index,$id,$xid);
    }
    $id;
}

sub
item_id {
    my($index,$base_id,$xid) = @_;
    my $id = $base_id.'i'.$$index{'#item_counter'}++;
#    my $id = $base_id;
    if ($xid) {
	xid($index,$id,$xid);
    }
    $id;
}

sub
xid {
    my($index,$iid,$xid) = @_;
    $$index{"$iid,xid"} = $xid;
    $$index{$xid,'iid'} = $iid;
}

sub
check_key {
    my ($tmp,$id) = @_;
    my $orig = $tmp;
    $tmp =~ s/\p{L}|\p{N}|[ₓ×]//g;
    $tmp =~ tr/A-Za-z0-9+%\@\'^{}⟦⟧//d;
    if ($tmp) {
	print STDERR "ORACC::SE::Indexer: $id: key '$orig' has bad chars '$tmp'\n";
    }
}

1;
