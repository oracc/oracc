#!/usr/bin/perl
my %cfs = (
   'gi'=>'gi',
   'gin2'=>'gij',
   'gin2-tur'=>'gijtur',
   'ma-na'=>'mana',
   'ma-na-tur'=>'manatur',
   'sze'=>'sze',
   'gun2'=>'gun',
   'gu(n)2'=>'gun',
   'gu2'=>'gun',
   'ban2'=>'ban',
   'bariga'=>'bariga',
   'gur'=>'gur',
   'guru7'=>'guru',
   'sila3'=>'sila',
   'asz'=>'asz',
   'disz'=>'disz',
   'szar2'=>'szar',
   'szar'=>'sar',
   'szaru'=>'szaru',
   'szargal{gal}'=>'szargal',
   'dug'=>'dug',
   'iku'=>'iku',
   'esze3'=>'esze',
   'bur3'=>'bur',
   'burux'=>'buru',
);

my %glyphs = (
   '1/3(disz)'=>'1/3~a',
   '1/2(disz)'=>'1/2~a',
   '2/3(disz)'=>'2/3~a',
   '5/6(disz)'=>'5/6~a',
);

while (<>) {
    s/gu\(n\)/gun/g;
    if (/^\d/) {
	my $out = '@inst ';
	my($inst,$equiv) = (/^(.*?)\s+(.*)$/);
	my($q,$s,$u) = ($inst =~ /^(.*?)\((.*?)\/(.*?)\)/);
	my($eq,$eu) = ($equiv =~ /^\[=(.*?)\s+(.*?)\]/);
	my $gbase = "$q($s)";
	my $g = $glyphs{$gbase};
	if (defined $g) {
	    $gbase = "$g=$gbase";
	}
	$out .= "$q $cfs{$u}\t1\t$eq\*$cfs{$eu}\t\t$gbase $u";
	$_ = $out . "\n";
    }
    print;
}
