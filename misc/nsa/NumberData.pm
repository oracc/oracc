package ORACC::ATF::NumberData;

require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/get_met_poss get_num_poss is_met is_num is_ntok/;

use warnings; use strict; use utf8;

my %sys_index = ();
my %num_index = ();
my %met_index = ();

my %sds = (
   'gan₂'=>'surface-met',
   'GAN₂'=>'surface-met',
   'gur'=>'drycap-met',
);

# self-initialization

while (<DATA>) {
    tr/|//d;
    my ($sys,$steps) = (/^(.*?):\s+(.*)/);
    my $i = 1;
    foreach my $s (split(/,\s+/, $steps)) {
	if ($sys =~ /-num-/) {
	    ${$num_index{$s}}{$sys} = $i++;
	} else {
	    ${$met_index{$s}}{$sys} = $i++;
	}
	${$sys_index{$s}}{$sys} = $i++;
    }
    foreach my $sds (keys %sds) {
	my $sys = $sds{$sds};
	${$met_index{$sds}}{$sys} = 123456789;
    }
    {$num_index{'igijal'}{'sexagesimal-num-*'} = 0;
}

1;

###########################################
#
# Dump the data for debugging 
sub
dump_data {
    open(DUMP,">NumberData.dump");
    print DUMP "#%sys_index\n";
    foreach my $s (sort keys %sys_index) {
	print DUMP "$s: ";
	my %s = %{$sys_index{$s}};
	foreach my $x (sort keys %s) {
	    print DUMP "$x/$s{$x} ";
	}
	print DUMP "\n";
    }
    print DUMP "#%num_index\n";
    foreach my $s (sort keys %num_index) {
	print DUMP "$s: ";
	my %s = %{$num_index{$s}};
	foreach my $x (sort keys %s) {
	    print DUMP "$x/$s{$x} ";
	}
	print DUMP "\n";
    }
    print DUMP "#%met_index\n";
    foreach my $s (sort keys %met_index) {
	print DUMP "$s: ";
	my %s = %{$met_index{$s}};
	foreach my $x (sort keys %s) {
	    print DUMP "$x/$s{$x} ";
	}
	print DUMP "\n";
    }
    close(DUMP);
}

sub
is_ntok {
    defined $sys_index{$_[0]}
    || $_[0] =~ /NINDA₂/
    || $_[0] =~ 'igiŋal'
    || defined $sds{$_[0]};
}

sub
is_num {
    my $tok = shift;
    $tok = 'disz' if $tok =~ /NINDA₂/;
    defined $num_index{$tok}
    || $tok =~ 'igiŋal';
}

sub
is_met {
    defined $met_index{$_[0]}
    || defined $sds{$_[0]};
}

sub
get_poss {
    my $ntok = shift;
    if (defined $sys_index{$ntok}) {
	return keys %{$sys_index{$ntok}};
    }
    ();
}

sub
get_met_poss {
    my $ntok = shift;
    if (defined($ntok) && defined $met_index{$ntok}) {
	return formatted_num_data(\%{$met_index{$ntok}});
    }
    ();
}

sub
get_num_poss {
    my $ntok = shift;
    if (defined($ntok) && defined($num_index{$ntok})) {
	return formatted_num_data(\%{$num_index{$ntok}});
    }
    ();
}

sub
formatted_num_data {
    my $aref = shift;
    my @ret = ();
    foreach my $a (keys %$aref) {
	push @ret, "$a/$$aref{$a}";
    }
    @ret;
}
sexagesimal-num-*-*: še, gin₂, aš, diš, u, geš₂, gešu, šar₂, šarʾu, šargal{gal}
length-met-*-*: šu-si, |ŠU.BAD|, kuš₃, gi, ninda, eš₂, UŠ, danna
surface-met-*-*: še, gin₂ tur, gin₂, ma-na tur, sar, iku, eše₃, bur₃, burʾu, šar₂, šarʾu, šargal{gal}
volume-met-*-*: še, gin₂, sar, iku, eše₃, bur₃
drycap-met-*-*: še, gin₂, sila₃, ban₂, barig, gur, šar₂, guru₇, šarʾu, šargal{gal}
liqcap-met-*-*: gin₂ tur, gin₂, sila₃, dug
weight-met-*-*: še, gin₂, ma-na, gu₂
brick-met-*-*: gin₂, sar
