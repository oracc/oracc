package ORACC::SMA::MorphData;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/mdata_messages mdata_parse mdata_validate/;

use warnings; use strict; use open 'utf8'; use utf8;
use Data::Dumper;

my $morphdata = "/Users/stinney/oracc/misc/sma/morph.data";
my %vpr = ();
my %vsf = ();
my %isf = ();
my %nsf = ();

my @messages = ();
my $status_global = 0;
my $status_local = 0;
my $verbose = 0;

## SOMEWHERE WE NEED TO ENFORCE UNIQUE m1<=>tlit<=>signs RELATIONSHIP

# Hash the tokens that are allowed in an ePSD2 morph string
my %t = ();
my @t = qw/~ x
    a ak al am anene ani
    e eE eL en enden ene enzen eš eše eV
    i inga
    u 
    V Vmma
    b ba bara bi
    da de ga gin ŋišen ŋu ha 
    m ma me men menden meš mu 
    n na ne ni NI nu nuš 
    ra ri 
    ša ši
    ta 
    zenden zu zunene/;
@t{@t} = ();

# NSF tokens
my %nsf_t = ();
my @nsf_t = qw/a ak am anene ani e ene eše bi ŋu me men meš ri ta zu zunene/;
@nsf_t{@nsf_t} = ();

sub init {
    open(M,$morphdata) || die;
    while (<M>) {
	chomp;
	my($type,$m1,$tlit,$slsig) = split(/\t/,$_);
	if ($type eq ':') {
	    ix(\%vpr,$m1,$tlit,$slsig);
	} elsif ($type eq ';') {
	    ix(\%vsf,$m1,$tlit,$slsig);
	} elsif ($type eq '!') {
	    ix(\%isf,$m1,$tlit,$slsig);
	} elsif ($type eq ',') {
	    ix(\%nsf,$m1,$tlit,$slsig);
	} else {
	    die "$0: bad type char in morphdata '$type'\n";
	}
    }
    close(M);
}

sub ix {
    my($h,$m1,$t,$s) = @_;
    $$h{$m1,$t} = $s;
    $$h{$m1,$s} = $t;
    push @{$$h{$t}}, "$m1==$s";
    push @{$$h{$s}}, "$m1==$t";
}

sub term {
}

# Args are: m1-type, m1-text, m1-sig, m1-tlit
#
# Test if m1-text is known for m1-type
#      && m1-tlit is known for m1-text in m1-type
#      || m1-sig is known for m1-text in m1-type and if so what is m1-text for this m1-sig?
sub mdata_lookup {
    warn "mdata args: @_\n" if $verbose;
    my($t,$m1,$m1s,$m1t) = @_;    
    my %h = ();
    if ($t eq 'vpr') {
	%h = %vpr;
    } elsif ($t eq 'vsf') {
	%h = %vsf;
    } elsif ($t eq 'isf') {
	%h = %isf;
    } elsif ($t eq 'nsf') {
	%h = %nsf;
    } else {
	die "$0: bad type '$t' passed as arg1 of ORACC::SMA::MorphData::mdata\n";
    }

    # warn("mdata h hash = ", Dumper \%h) if $verbose;
    
    if ($h{$m1,$m1t}) { # the m1 interpretation of the value sequence is known
	push (@messages, "(mdata) $m1 ~~ $m1t OK") if $verbose;
	return [ 1 ];
    } elsif ($h{$m1,$m1s}) { # the m1 interpretation of the sign sequence is known
	if ($h{$m1t}) { # the value sequence is known for type/sig but with different m1
	    # There are two possibilities here:
	    # 1) the passed m1 should use a different transliteration of m1s than the passed m1t
	    # 2) the passed m1 is wrong, m1t should be read as a different m1
	    push @messages, "(mdata) m1 $m1 written $m1t should be different m1 or should be written @{$h{$m1t}}";
	    return [ 2, $h{$m1,$m1s} , $h{$m1t} ]; ## $h{$m1t} => known M1 not passed one	    
	} else {
	    # The known transliteration for m1 using signs m1s is different than passed one
	    push @messages, "(mdata) expected transliteration for m1 $m1 is $h{$m1,$m1s}, not $m1t";
	    return [ 2 , $h{$m1,$m1s} ]; ## $h{$m1,$m1s} => known value not passed one
	}
    } elsif ($h{$m1t}) {
	# The transliteration is known but only with one or more different m1
	my @e = map { s/==.*$// && $_ } @{$h{$m1t}};
	push @messages, "(mdata) m1 $m1 not known for tlit $m1t; known m1s are: @e";
	return [ 3 , "$m1 != @{$h{$m1t}}" ]; # return format is $m1==$m1s
    } elsif ($h{$m1s}) {
	# The sign sequence is known but only with one or more different m1; return format is $m1==$m1t
	push @messages, "(mdata) m1 $m1 not known for tlit $m1t; known sigs are: @{$h{$m1s}}";
	return [ 4 , "$m1 != @{$h{$m1s}}" ];
    }
    
    return [ 0 ];
}

sub mdata_parse {
    my $m = shift;
    my %m = ();

    if (mdata_validate($m)) {
	$m{'error'} = 1;
	return %m;
    }
    
    if ($m =~ s/^(.*?)://) {
	$m{'vpr'} = $1;
    }
    if ($m =~ s/,(.*)$//) {
	$m{'nsf'} = $1;
    }
    if ($m =~ s/;(.*?)$//) {
	$m{'vsf'} = $1;
    }
    if ($m =~ s/\!(.*?)$//) {
	$m{'isf'} = $1;
    }

    %m;
}

# This is a validator for Sumerian morphology as used by ePSD2 and the SMA.
#
sub mdata_validate {
    @messages = ();
    $status_local = 0;
    my $m = shift;

    # a bare base is always ok as is the unparseable morph #X
    return 0 if $m eq '~' || $m eq 'X';

    # and there must be a base, but only one
    mdata_warn("no ~ in $m") and goto done unless $m =~ /~/;
    mdata_warn("multiple ~ in $m") and goto done if $m =~ tr/~/~/ > 1;

    # and there must be a morph divider
    mdata_warn("missing : ; ! or , in $m") and goto done unless $m =~ /[:;!,]/;

    # but no more than one of any divider
    mdata_warn("multiple : in $m") and goto done if $m =~ tr/:/:/ > 1;
    mdata_warn("multiple ; in $m") and goto done if $m =~ tr/;/;/ > 1;
    mdata_warn("multiple ! in $m") and goto done if $m =~ tr/!/!/ > 1;
    mdata_warn("multiple , in $m") and goto done if $m =~ tr/,/,/ > 1;

    # NSF must follow any VSF or ISF, and if no VSF or ISF , must come after ~
    mdata_warn(", must follow any ; or ! in $m") and goto done if $m =~ /,.*?[;!]/;

    # dividers must be in the right place
    mdata_warn(": after ~ in $m") and goto done if $m =~ /~.*?:/;
    mdata_warn("junk between : and ~ in $m") and goto done if $m =~ /:.+?~/;
    mdata_warn("; before ~ in $m") and goto done if $m =~ /;.*?~/;
    mdata_warn("junk between ~ and ; in $m") and goto done if $m =~ /~.+?;/;
    mdata_warn("! before ~ in $m") and goto done if $m =~ /!.*?~/;
    mdata_warn("junk between ~ and ! in $m") and goto done if $m =~ /~.+?\!/;
    mdata_warn(", before ~ in $m") and goto done if $m =~ /,.*?~/;
    mdata_warn("junk between ~ and , in $m") and goto done if $m =~ /~.+?,/ && ($m =~ tr/!;/!;/ == 0);

    # * is allowed for non-functional graphemes; cbdpp's validation routine maps
    # * to U+273B TEARDROP-SPOKED ASTERISK so remove them now.
    $m =~ tr/*✻//d;

    # remove dividers by mapping them to '.'
    my $m2 = $m;
    $m2 =~ tr/:;!,/..../;

    # now it's an error to have two dots in a row
    mdata_warn("spurious . in $m") and goto done if $m2 =~ /\.\./;

    # now we can split on dot and check that each token is allowed--we
    # aren't validating this for legitimate Sumerian here, just
    # sanity-checking the tokens
    my @t = split(/\./, $m2);
    foreach my $t (@t) {
	mdata_warn("bad token $t in $m") and last unless exists $t{$t};
    }

  done:
    
    ++$status_global if $status_local;
    return $status_local;
}

sub mdata_messages {
    my @tmp = @messages;
    @messages = ();
    @tmp;
}

sub mdata_warn {
    ++$status_local;
    push @messages, "(mdata) @_";
}

1;
