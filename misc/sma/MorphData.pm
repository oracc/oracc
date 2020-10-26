package ORACC::SMA::MorphData;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/mdata_messages mdata_validate/;

use warnings; use strict; use open 'utf8'; use utf8;

my $morphdata = "/Users/stinney/oracc/misc/sma/morph.data";
my %vpr = ();
my %vsf = ();
my %isf = ();
my %nsf = ();

my @messages = ();
my $status_global = 0;
my $status_local = 0;

## SOMEWHERE WE NEED TO ENFORCE UNIQUE m1<=>tlit<=>signs RELATIONSHIP

# Hash the tokens that are allowed in an ePSD2 morph string
my %t = ();
my @t = qw/~ x 
    a ak al am anene ani
    e en ene enzen eš eše
    i inga
    u 
    V Vmma
    b ba bara bi
    da de ga gin ŋišen ŋu ha 
    m me men menden meš mu 
    n na ne ni NI nu nuš 
    ra ri 
    ša ši 
    ta 
    x zenden zu zunene/;
@t{@t} = ();

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
sub mdata {
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

    if ($h{$m1,$m1t}) { # the m1 interpretation of the value sequence is known
	return [ 1 ];
    } elsif ($h{$m1,$m1s}) { # the m1 interpretation of the sign sequence is known
	if ($h{$m1t}) { # the value sequence is known for type/sig but with different m1
	    # There are two possibilities here:
	    # 1) the passed m1 should use a different transliteration of m1s than the passed m1t
	    # 2) the passed m1 is wrong, m1t should be read as a different m1
	    return [ 2, $h{$m1,$m1s} , $h{$m1t} ]; ## $h{$m1t} => known M1 not passed one
	} else {
	    # The known transliteration for m1 using signs m1s is different than passed one
	    return [ 2 , $h{$m1,$m1s} ]; ## $h{$m1,$m1s} => known value not passed one
	}
    } elsif ($h{$m1t}) {
	# The transliteration is known but has one or more different m1
	return [ 3 , @{$h{$m1t}} ]; # return format is $m1==$m1s
    } elsif ($h{$m1s}) {
	# The sign sequence is known but has one or more different m1; return format is $m1==$m1t
	return [ 4 , @{$h{$m1s}} ];
    }
    
}

sub mdata_parse {
    my $m = shift;

    mdata_validate($m);
    
    my %m = ();
    
    if ($m =~ s/^(.*?)://) {
	$m{'vpr'} = $1;
    }
    # ; ! , 
}

# This is a validator for Sumerian morphology as used by ePSD2 and the SMA.
#
sub mdata_validate {
    @messages = ();
    $status_local = 0;
    my $m = shift;

    # a bare base is always ok
    return 0 if $m eq '~';

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
    $m =~ tr/:;!,/..../;

    # now it's an error to have two dots in a row
    mdata_warn("spurious . in $m") and goto done if $m =~ /\.\./;

    # now we can split on dot and check that each token is allowed--we
    # aren't validating this for legitimate Sumerian here, just
    # sanity-checking the tokens
    my @t = split(/\./, $m);
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
