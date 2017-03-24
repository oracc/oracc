package ORACC::FileUtil;

use File::Find;

use warnings; use strict;

$ORACC::FileUtil::ext = undef;

sub
hasOneOfExt {
    my($dir,$ext) = @_;

    return 0 unless -d $dir;

    $ORACC::FileUtil::ext = $ext;

    eval {
	find(\&wanted_has_ext, $dir);
    };
    if ($@ =~ /^HAS_EXT/) {
	return 1;
    } else {
	return 0;
    }
}

sub
wanted_has_ext {
    if (/\.$ORACC::FileUtil::ext/o) {
	die 'HAS_EXT';
    }
}

sub
hasOneFile {
    my($dir) = @_;

    return 0 unless -d $dir;

    eval {
	find(\&wanted_has_one,$dir);
    };
    if ($@ =~ /^HAS_ONE /) {
	return 1;
    } else {
	return 0;
    }
}

sub
wanted_has_one {
    if (-f $_) {
	die 'HAS_ONE';
    }
}

1;
