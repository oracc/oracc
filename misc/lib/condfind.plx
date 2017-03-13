#!/usr/bin/perl
use warnings; use strict;
use lib "$ENV{'ORACC'}/lib";
use ORACC::FileUtil;

my($dir,$ext) = @ARGV;
my $ret = 0;

if ($ext) {
    $ret = ORACC::FileUtil::hasOneOfExt($dir,$ext);
} else {
    $ret = ORACC::FileUtil::hasOneFile($dir);
}

print 'ok' if $ret;
exit !$ret;

#if ($ret) {
#    print "YEP\n";
#} else {
#    print "NOPE\n";
#}

1;
