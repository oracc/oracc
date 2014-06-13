#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::L2P0::L2Super;
use File::Copy "mv";

my %data = ORACC::L2P0::L2Super::init();

my $mapdate = (stat($data{'mapfile'}))[9];
my $newdate = (stat($data{'newfile'}))[9];

if ($newdate < $mapdate) {
    die <<EOD;
super merge: can\'t merge when $data{'newfile'} is older than $data{'mapfile'}.
If you are sure you want to merge, say:

    touch $data{'newfile'}

and then run the merge again.
EOD
}

ORACC::L2GLO::Builtins::merge_exec($data{'baselang'}, 
				   $data{'basefile'}, 
				   [ $data{'srcfile'} ], 
				   $data{'outglo'});

chatty("installing $data{'outglo'} as new version of $data{'basefile'}");
if ($data{'dryrun'}) {
    super_warn("mv $data{'outglo'} $data{'basefile'}");
} else {
    mv($data{'outglo'},$data{'basefile'});
}

chatty("installing $data{'outmap'} as new version of $data{'mapfile'}");
if ($data{'dryrun'}) {
    super_warn("mv $data{'outmap'} $data{'mapfile'}");
} else {
    mv($data{'outmap'},$data{'mapfile'});
}

1;
