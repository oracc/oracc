#!/usr/bin/perl
use warnings; use strict;

my $p = `oraccopt`;
my @f = map { s#02www/downloads/##; $_ } <02www/downloads/*>;
my @u = map { "/$p/downloads/$_" } @f;

#print @f;
#print @u;

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
<title>$p Downloads Page</title>
</head>
<body>
<h1>$p Downloads Page</h1>
<p>The following files are available for download from this project:</p>
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
