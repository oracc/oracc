#!/usr/bin/perl
use warnings; use strict;

my $date = `TZ=GMT date '+%F %R'`;
my $text_good = `wc -l status/repo-good.txt`; $text_good =~ s/\s.*$//;
my $text_bad  = `wc -l status/repo-bad.txt`;  $text_bad =~ s/\s.*$//;
my $text_errors = `wc -l status/repo-status.log`; $text_errors =~ s/\s.*$//;
chomp($text_good,$text_bad,$text_errors);

my $text_total = $text_good+$text_bad;
my $utf8_status = `grep -c 'bad UTF-8 character' status/repo-status.log`;
chomp($utf8_status);
if ($utf8_status eq '0') {
    $utf8_status = 'CLEAN';
} else {
    $utf8_status .= ' ERRORS';
}
my @text_pids = (`grep 'P-ID not in main catalog' status/repo-status.log`);
@text_pids = map { /:(P.*?):/ && $1 } @text_pids;
my $text_pids = $#text_pids + 1;
open(P,">status/not-in-cat.txt");
print P join("\n",@text_pids),"\n";
close(P);

###################################################

open(H,">status/index.html");
preamble();
overview();
downloads();
postamble();

###################################################

sub
preamble {
    print H <<EOP;
<html>
<head>
<title>ORACCI Repository Status $date GMT</title>
<body>
<h1>ORACCI Repository Status $date GMT</h1>
EOP
}

sub
overview {
    print H <<EOP;
<h2>Overview</h2>
<table>
<tr><td>UTF-8 Status</td><td>$utf8_status</td></tr>
<tr><td>Total texts:</td><td>$text_total</td><td><a href="repo-list.txt">Download list</a></td></tr>
<tr><td>Good texts:</td><td>$text_good</td><td><a href="repo-good.txt">Download list</a></td></tr>
<tr><td>Bad texts:</td><td>$text_bad</td><td><a href="repo-bad.txt">Download list</a></td></tr>
<tr><td>Total errors:</td><td>$text_errors</td><td><a href="repo-status.log">Download log file</a></td></tr>
<tr><td>P-IDs not in catalog</td><td>$text_pids</td><td><a href="not-in-cat.txt">Download list</a></td></tr>
</table>
EOP
}

sub
downloads {
    print H <<EOP;
<h2>Additional Downloads</h2>
<ul>
<li><a href="repo-bad.atf">ATF file containing all bad texts</a></li>
</ul>
EOP
}

sub
postamble {
    print H <<EOP;
</body></html>
EOP
}

1;
