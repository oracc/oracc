#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Util;
my %feat = ();
my @urn = ();
while (<>) {
    next if m/^\s*$/
	|| m/^\@(project|name|lang)\s(.*?)$/
	|| m/^\{/
	|| m/\&\&/;
    chomp;
    my %sig = ORACC::L2GLO::Util::parse_sig($_);
    warn "freq = $sig{'freq'}\n" if $sig{'freq'};
    push @urn, urn_word(\%sig);
    push @urn, urn_inst(\%sig);
    foreach my $f (qw/base cont stem pref root/) {
	if ($sig{$f}) {
	    ++${$feat{"$sig{'lang'}:$f"}}{$sig{$f}};
	}
    }
}

foreach my $f (keys %feat) {
    foreach my $v (keys %{$feat{$f}}) {
	push @urn, urn_feat($f, $v);
    }
}

print sort { $a cmp $b } uniq(@urn);

############################################################

sub
uniq {
    my %u = ();
    @u{@_} = ();
    keys %u;
}

sub
urn_feat {
    my($k,$v) = @_;
    $v =~ s/^\%.*?://;
    "urn:oracc:$k:$v\n";
}

sub
urn_inst {
    my $s = shift;
    my $u = urn_word($s);
    chomp($u);
    my $f = $$s{'form'};
    my $epos = $$s{'epos'};
    if ($epos =~ /^[A-Z]N$/) {
	$u =~ s/oracc:.*?:/oracc:$$s{'epos'}:/;
	$f =~ s/^\%.*?:/\%$$s{'lang'}:/;
    } else {
	$f =~ s/^\%.*?://;
    }
    "$u:$$s{'sense'}:$$s{'epos'}:$f:$$s{'norm'}\n";
}

sub
urn_word {
    my $s = shift;
    my $l = ($$s{'pos'} =~ /^[A-Z]N$/ ? $$s{'pos'} : $$s{'lang'});
    "urn:oracc:$l:$$s{'cf'}:$$s{'gw'}:$$s{'pos'}\n";
}

1;
