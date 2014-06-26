#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8'; binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';

sub xsystem;

my $bin = '/usr/local/oracc/bin';
my $oracc_log = "/usr/local/oracc/tmp/sop/$ARGV[0]/oracc.log";
my $request_log = "/usr/local/oracc/tmp/sop/$ARGV[0]/request.log";
my $status_file = "/usr/local/oracc/tmp/sop/$ARGV[0]/status";
my $verbose = 1;
my $zip_response = "/usr/local/oracc/tmp/sop/$ARGV[0]/response.zip";
my @zippers = ($request_log, $oracc_log);

warn "oracc-exec: @ARGV\n";

set_status('run');

sleep(10);

zip_response();

set_status('done');

######################################################################################

sub
set_status {
    open(S,">$status_file");
    print S $_[0];
    close(S);
}

sub
xsystem {
    warn "xsystem @_\n" if $verbose;
    system @_;
}

sub
zip_response {
    unlink($zip_response);
    xsystem 'zip', $zip_response, @zippers;
}

1;
