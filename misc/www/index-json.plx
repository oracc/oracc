#!/usr/bin/perl
use warnings; use strict;

my @f = map { s#/home/oracc/www/json/##; $_ } </home/oracc/www/json/*>;
my @u = map { "/json/$_" } @f;

print preamble();
for (my $i = 0; $i <= $#f; ++$i) {
    next if $f[$i] =~ /index.html/;
    print "<li><a href=\"$u[$i]\">$f[$i]</a></li>\n";
}
print postamble();


sub preamble {
    my $s = <<EOS
<html>
<head>
<title>Oracc JSON Downloads Page</title>
</head>
<body>
<h1>Oracc JSON Downloads Page</h1>
<p>The following JSON archives are available here:</p>
<ul>
EOS
}

sub postamble {
    my $s = <<EOS
</ul>
</body>
</html>
EOS
}

1;
