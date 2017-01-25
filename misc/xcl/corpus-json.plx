#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDOUT, ':utf8';

use lib "$ENV{'ORACC'}/lib";
use ORACC::L2GLO::Util;

my %sigs = ();

my $project = shift @ARGV;
my @files = @ARGV;
chomp @files;

if ($files[0] =~ /.lst$/) {
    @files = `cat $files[0]`;
    chomp @files;
}

print "{\n\t\"type\": \"corpus\",\n\t\"project\": \"$project\",\n\t\"members\": [\n";

for (my $i = 0; $i <= $#files; ++$i) {
    my $file = $files[$i];
    if ($file =~ /:/) {
	my ($fileproj,$four) = ($file =~ m/^(.*?):(....)/);
	$file =~ s/^.*?://;
	$file = "$ENV{'ORACC'}/$fileproj/01bld/$four/$file/$file.xtf";
    }
    if (-r $file) {
	my @f = `xsltproc $ENV{'ORACC'}/lib/scripts/xcl-json.xsl $file`;
	foreach (@f) {
	    print "\t$_";
	    if (/\"sig\":\s*\"(.*?)\"/) {
		++$sigs{$1};
	    }
	}
	print "\t,\n" if $i < $#files;
    } else {
	warn "corpus-json.plx: can't find $file; skipping it\n";
    }
}
print "\t],\n\t\"sigs\": {\n";
my @sigs = sort keys %sigs;
for (my $i = 0; $i <= $#sigs; ++$i) {
    print "\t\t\"$sigs[$i]\": {\n";
    my %p = parse_sig($sigs[$i]);
    $p{'form'} = '' unless $p{'form'};
    $p{'form'} =~ s/^.*?://; #### PROBABLY HAVE TO REVISIT THIS
    if ($p{'parts'}) {
	sig_json("\t\t\t",%p);
    } else {
	sig_json("\t\t\t",%p);
    }
    print "\n\t\t}";
    print " ,\n" if $i < $#sigs;
}
print "\n\t}\n";

print "}\n";

###############################################################

sub
sig_json {
    my ($prefix,%p) = @_;
    my @pr = ();
    foreach my $p (qw/form cf gw sense pos epos norm base cont morph morph2/) {
	if ($p{$p}) {
	    my $p_j = $p{$p};
	    $p_j =~ s/(["\\])/\\$1/g;
	    push @pr, "$prefix\"$p\": \"$p_j\"";
	}
    }
    print join(",\n", @pr), "\n";
}

1;
