package ORACC::CHI::Padder;
use warnings; use strict;
use lib '@@ORACC@@/lib';

#AOC
#B
#C
#DT
#K
#Loan
#ND
#Rm
#SM
#SP
#Sp
#UET


my %padspecs = (
    A       => [ 'digits', 5 ],
    AOC     => [ 'digits', 3 ],
    AIUP    => [ 'digits', 2 ],
    Ashm    => [ 'pattern', [
		     [ '^(\d+)(-)(\d+)(.*?)$', [ 4 , 3 ] ]
		 ]
    ],
    B       => [ 'digits', 3 ],
    BM      => [ 'digits', 6 ],
    BMAcc   => [ 'nopad' ], # already handled in nm_bmacc
    BT      => [ 'digits', 2 ],
    C       => [ 'digits', 3 ],
    CBS     => [ 'digits', 5 ],
    CG      => [ 'pattern',  [
		     [ '^(\d+)(-)(\d+)(-)(\d+)(.*?)$', [ 2, 2, 3 ] ] 
		 ]
    ],
    DT      => [ 'digits', 3 ],
    ES      => [ 'digits', 5 ],
    F       => [ 'digits', 4 ],
    G       => [ 'nopad' ], 	# Gawra excavation numbers
    FS      => [ 'digits', 4 ],
    Golenishev => [ 'nopad' ],
    HS      => [ 'digits', 5 ],
    IM      => [ 'digits', 6 ],
    'Ist-Ni'=> [ 'digits', 6 ],
    'Ist-O' => [ 'digits', 4 ],
    K       => [ 'digits', 5 ],
    L       => [ 'pattern', [
		     [ '^(\d+)(-)(\d+)(.*?)$', [ 2, 3 ] ] 
		 ]
    ],
    Loan    => [ 'digits', 4 ],
    M       => [ 'digits', 4 ],
    N       => [ 'digits', 4 ],
    N02T    => [ 'digits', 4 ],
    N03T    => [ 'digits', 4 ],
    N05T    => [ 'digits', 4 ],
    N06T    => [ 'digits', 4 ],
    N11     => [ 'digits', 3 ],
    ND      => [ 'digits', 4 ],
    'PRAK-B'=> [ 'digits', 4 ],
    Rm      => [ 'pattern', [ 
		     [ '^(\d+)(-a)?$', [ 4 , 0 ] ],
		     [ '^([IV]+(?:-F)?)(-)(\d+)(-[ab])?$', [ 0, 4, 0 ] ],
		     [ '^(\d+)(-)(\d+)(-)(\d+)(.*?)$', [ 2, 2, 3 ] ],
		     [ '^(L)(-)(\d+)(-)(\d+)(.*?)$', [ 2, 3 ] ],
		 ]
    ],
    SM      => [ 'digits', 4 ],
    SP      => [ 'digits', 3 ],
    Sp      => [ 'pattern', [ 
		     [ '^(\d+)(-a)?$', [ 4 , 0 ] ],
		     [ '^([IV]+(?:-F)?)(-)(\d+)(-[ab])?$', [ 0, 4, 0 ] ],
		     [ '^(\d+)(-)(\d+)(-)(\d+)(.*?)$', [ 2, 2, 3 ] ],
		     [ '^(L)(-)(\d+)(-)(\d+)(.*?)$', [ 2, 3 ] ],
		 ]
    ],
    U       => [ 'digits', 5 ],
    UET     => [ 'pattern', [
		     [ '^(\d+)(-)(\d+)$', [ 2, 4 ] ] ,
		 ]
    ],
    UM      => [ 'pattern', [ 
		     [ '^(\d+)(-)(\d+)(-)(\d+)(.*?)$', [ 2, 2, 3 ] ],
		     [ '^(L)(-)(\d+)(-)(\d+)(.*?)$', [ 2, 3 ] ],
		 ] 
    ],
    'UM-f'  => [ 'pattern', [
		     [ '^(\d+)(-)(\d+)(-)(\d+)(.*?)$', [ 2, 2, 3 ] ] 
		 ]
    ],
    VAT     => [ 'digits', 5 ],
    );

sub
pad {
    my($id,$chi) = @_;
    my($p,$n) = ($chi =~ /^(.*?)_(.*?)$/);
    if ($padspecs{$p}) {
	my ($type,$arg1) = @{$padspecs{$p}};
	if ($type eq 'digits') {
	    if ($n =~ /^0*(\d+)(.*)$/) {
		my($digits,$postfix) = ($1,$2);
		$postfix = '' unless $postfix;
		my $z = '0'x($arg1 - length($digits));
		my $ret = "${p}_$z$digits$postfix";

		# last-minute post-diddling
		if ($ret =~ /^N\d\dT/ && $ret =~ /-\d\d$/) {
		    $ret =~ s/-(\d\d)$/-0$1/;
		}
		return $ret;
	    }
	} elsif ($type eq 'pattern') {
	    my @chunks = ();
	    my $matched = 0;
	    foreach my $patspec (@$arg1) {
		my($pat,$pads) = (@$patspec);
		if (@chunks = ($n =~ /$pat/)) {
#		    @chunks = map { $_ || '' } @chunks;
		    my @pads = @$pads;
		    my @newchi = ();
		    my $nth_paddable = 0;
		    for (my $i = 0; $i <= $#chunks; ++$i) {
			if ($chunks[$i]) {
			    if ($chunks[$i] =~ /^\d/) {
				if ($pads[$nth_paddable]) {
				    my $z = '0'x($pads[$nth_paddable] - length($chunks[$i]));
				    push @newchi, "$z$chunks[$i]";
				} else {
				    push @newchi, $chunks[$i];
				}
				++$nth_paddable;
			    } else {
				push @newchi, $chunks[$i];
			    }
			} else {
#			    warn "chunk $i (counting from zero) is undefined when $n matches $pat\n";
			}
		    }
		    my $ret = $p.'_'.join('',@newchi);
		    # do last-minute post-diddling here
		    return $ret;
		}
	    }
	    # we've returned if there was a match
	    unless ($n =~ /^x[a-f0-9]+$/) {
		warn "ORACC::CHI::Padder: $id: no patterns matched $p $n\n";
	    }
	} elsif ($type eq 'nopad') {
	    # no-op
	} else {
	    warn "ORACC::CHI::Padder: unhandled padding type '$type'\n";
	}
    } else {
	warn "ORACC::CHI::Padder: $id: no padspec for prefix $p\n";
    }
    $chi;
}

1;
