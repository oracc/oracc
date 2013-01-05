require "mkmf"

dir_config('estraier')

ENV["PATH"] = ENV["PATH"] + ":/usr/local/bin:.:..:../.."
estcflags = `estconfig --cflags`.chomp
estldflags = `estconfig --ldflags`.chomp
estlibs = `estconfig --libs`.chomp
$CFLAGS = "-I. -I.. -I../.. #{estcflags} -Wall #{$CFLAGS} -O3 -fomit-frame-pointer -fforce-addr"
$LDFLAGS = "#{$LDFLAGS} -L. -L.. -L../.. #{estldflags}"
$libs = "#{$libs} #{estlibs}"

if have_header('estraier.h') and have_library('estraier')
  create_makefile('estraier')
end
