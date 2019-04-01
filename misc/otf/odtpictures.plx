#!/usr/bin/perl
use File::Basename;
use warnings; use strict;
my %mimes = (jpg=>'jpeg',jpeg=>'jpeg','png'=>'png');
open('P','odtpictures.lst');
while (<P>) {
    chomp;
    my($href,$path) = split(/\t/,$_);
    if ($path) {
	if (-r $path) {
	    my($name,$xpath,$suffix) = fileparse($href,'jpeg','jpg','png');
	    my $mimetype = $mimes{$suffix};
	    print <<EOM;
<manifest:file-entry manifest:media-type="image/$mimetype" 
  manifest:full-path="pictures/$name$suffix"/>
EOM
	    system "cp -f $path pictures";
	} else {
	    warn "odtpictures.plx: missing picture '$path' dropped from manifest\n";
	}
    } else {
	warn "odtpictures.plx: picture '$href' has no PATH\n";
    }
}
close(P);
1;
