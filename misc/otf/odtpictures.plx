#!/usr/bin/perl
use File::Basename;
use warnings; use strict;
my %mimes = (jpg=>'jpeg',jpeg=>'jpeg','png'=>'png');
open('P','odtpictures.lst');
while (<P>) {
    chomp;
    my($href,$path) = split(/\t/,$_);
    my($name,$xpath,$suffix) = fileparse($href,'jpeg','jpg','png');
    my $mimetype = $mimes{$suffix};
    print <<EOM;
<manifest:file-entry manifest:media-type="image/$mimetype" 
  manifest:full-path="pictures/$name$suffix"/>
EOM
system "cp -f $path pictures";
}
close(P);
1;
