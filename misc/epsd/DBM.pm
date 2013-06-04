package ORACC::SE::DBM;
use warnings; use strict; use open 'utf8'; use utf8;
use Fcntl;
use NDBM_File;
use Encode;
use Data::Dumper;

$ORACC::SE::DBM::project = 'epsd2';
my $dbdir = "@@ORACC@@/www/$ORACC::SE::DBM::project/db";

# Convert the archival XML to a DBM for runtime use.
sub
create {
    my $name = shift;
    my $xmldb = ORACC::SE::XML::fromXML($name);
    my %db = ();
    my $dbref;
    unlink "$dbdir/$name.db";
    print STDERR "creating $dbdir/$name.db\n";
    my $cdbref 
	= tie(%db,'NDBM_File',"$dbdir/$name",
	      O_CREAT|O_TRUNC|O_RDWR,0644) || die;

    foreach my $k (keys %$xmldb) {
	Encode::_utf8_off($k);
	my $data_type = ref($$xmldb{$k});
	if (!$data_type || !defined $$xmldb{$k}) {
	    Encode::_utf8_off($$xmldb{$k});
	    $db{$k} = $$xmldb{$k};
	} elsif ($data_type eq 'SCALAR') {
	    $db{$k} = $$xmldb{$k};
	} elsif ($data_type eq 'ARRAY') {
	    $db{$k} = join(' ', @{$$xmldb{$k}});
	} else {
	    die "DBM.pm: key '$k' has $data_type data: don't grok\n";
	}
    }
    undef $cdbref;
    untie %db;
}

sub
Dumper_out {
    my($dbref,$ixname) = @_;
    my $outfile = "$dbdir/$ixname.dump";
    $Data::Dumper::Useqq = 1;
    open(O, ">$outfile") || die "DBM.pm: can't write to output file $outfile\n";
    print O Dumper $dbref;
    close(O);
    system 'mv', '-f', $outfile, "$outfile.x";
    open(O,"$outfile.x");
    open(P,">$outfile");
    while (<O>) {
	s/\\x\{(.*?)\}/utfify($1)/eg;
	print P;
    }
    close(O);
    close(P);
}

sub
utfify {
    chr(hex("0x$_[0]"));
}

sub
Dumper_in {
}

sub
setdir {
    $dbdir = $_[0];
}

sub
ufix {
    Encode::_utf8_on($_[0]);
}

my $dbref;
sub
load {
    my $name = shift;
    my %db = ();
    $dbref = tie(%db,'NDBM_File',"$dbdir/$name",
		 O_RDONLY,0644) || die "can't open '$dbdir/$name.db'";
    $dbref->filter_fetch_key(sub { ufix($_) });
    $dbref->filter_fetch_value(sub { ufix($_) });
    \%db;
}

sub
unload {
    my $hashref = shift;
    undef $dbref;
    untie %$hashref;
}

sub
freq {
    my ($index,$field,@keys) = @_;
    my $f = 0;
    if ($#keys > 0) {
	$f = uniq($index,$field,@keys);
    } elsif ($#keys == 0) {
	$f = $$index{$field.':'.$keys[0],'f'} || 0;
    }
    $f;
}

sub
fuzzy {
    my($index,$field,$key) = @_;
    Encode::_utf8_off($key);
    if ($field eq $$index{'#default_field'}) {
	$field = '';
    } else {
	$field .= ':';
    }
    my $x = $$index{$field.$key.';x'};
    if ($x) {
	Encode::_utf8_off($x);
	$$index{$x};
    } else {
	$key;
    }
}

sub
greppable_records {
    my($index,$field,$recids_ref) = @_;
    if ($field) {
	
    } else {
	
    }
}

sub
html {
    my($index,$recid) = @_;
    my $h = $$index{"$recid,sum"};
    if ($h !~ /^<p/) {
	$h = "<p>$h</p>";
    }
    $h;
}

sub
records {
    my($index,$key) = @_;
    Encode::_utf8_off($key);
    $$index{$key};
}

sub
records_list {
    my($index,$field,@keys) = @_;
    my %r = ();
    foreach my $k (@keys) {
	# FIXME: this 'unless' is a hack--get to the bottom of this
	# someday
	$k = $field.':'.$k unless $k =~ /:/;
	my $r = records($index,$k);
	if ($r) {
	    @r{split(/\s+/,$r)} = ();
	}
    }
    sort keys %r;
}

sub
uniq {
    my($index,$field,@k)  = @_;
    my %u = ();
    foreach my $k (@k) {
	Encode::_utf8_off($k);
	next unless $$index{"${field}:$k"};
	@u{ split(/ /,$$index{"${field}:$k"}) } = ();
    }
    keys %u;
}

sub
wild {
    my($index,$field,$raw) = @_;
    $field = $$index{'#default_field'} unless $field;
    if ($raw =~ /[*%|]/) {
	my $pat = $raw;
	$pat =~ s/([*%])/$1 eq '%' ? '[0-9₀-₉₊x]*' : '.*?'/eg;
	$pat = "^$pat\$";
	my @match = ($$index{"#${field}:item_grep"} =~ /$pat/mg);
#	my $w = $$index{"#${field}:item_grep"};
#	utf8::upgrade($w); utf8::upgrade($pat);
#	Encode::_utf8_off($w); Encode::_utf8_off($pat);
#	my @match = ($w =~ /$pat/mg);
	($pat, @match);
    } else {
	($raw, $raw);
    }
}

sub
xids {
    my($index,@rec) = @_;
    my @ret = ();
    foreach my $r (@rec) {
	push @ret, $$index{"$r,xid"};
    }
    @ret;
}

1;
