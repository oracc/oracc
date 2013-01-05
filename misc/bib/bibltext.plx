#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDOUT, ':utf8'; binmode STDIN, ':utf8'; binmode STDERR, ':utf8';

#
# TITLI :
#    <i> ... </i>
#    *...* (with space before and after)
#    
# TITLE :
#    " ... "
#    ' ... '
#    ̀ ... '
#    

use constant {
    TITLE=>0,
    TITLI=>1,
    LCSTR=>2,
    MCSTR=>3,
    UCSTR=>4,
    INTOK=>5,
    PPTOK=>6,
    PUNCT=>7,
    FENCE=>8,
    NUTOK=>9,
};

my $errloc = 1;
my @para = ();

while (<>) {
    processPara(@para) if /^\s*$/;
    chomp; s/^\s*(.*?)\s*$/$1/;
    push @para, $_;
    close if feof();
}

processPara(@para);

########################################################################

sub
processPara {
    my $entry = join(' ', @_);
    my %biblStruct = ();
    my @toks = tokenize($entry);
    $entry = parseAuthors($entry,\%biblStruct);
    print Dumper(\%biblStruct);
    $errloc = $.;
}

########################################################################

sub
parseAuthors {
    my($entry,$sref) = @_;
    return parseNames($entry,'author',$sref);
}

sub
parseEditors {
    my($entry,$sref) = @_;
    return parseNames($entry,'editor',$sref);
}

sub
parseNames {
    my($str,$key,$sref) = @_;
    my $tok;
    while (1) {
	($tok,$str) =  gettok($str);
    }
}

########################################################################

sub
bad {
    my($what,$how) = @_;
    warn("$ARGV:$errloc: $what error: $how\n");
}

sub
get_title {
    my($s,$o,$c) = @_;
    $s =~ s/^$o(.*?)$c\s+//;
    ($1,$s);
}

sub
tokenize {
    my($str) = shift;
    my @toks = ();
    while (length $str) {
	if ($str =~ m#^<i>#) {
	    if ($str =~ m#</i>#) {
		$str =~ s#^<i>(.*?)</i>##;
		push @toks, [ TITLI , $1 ];
	    } else {
		bad('token',"<i> has no </i>");
	    }
	} elsif ($str =~ m#^\*#) {
	    if ($str =~ m/\*\s/) {
		$str =~ s#^\*(.*?)\*\s+##;
		push @toks, [ TITLI , $1 ];
	    } else {
		bad('token',"* has no closing '* '");
	    }
	} elsif ($str =~ /^"/) {
	    my($s,$t) = get_title($str,'"','"');
	    push @toks, [ TITLE , $t ];
	    $str = $s;
	} elsif ($str =~ /^'/) {
	    my($s,$t) = get_title($str,'\'','\'');
	    push @toks, [ TITLE , $t ];
	    $str = $s;
	} elsif ($str =~ /^`/) {
	    my($s,$t) = get_title($str,'`','\'');
	    push @toks, [ TITLE , $t ];
	    $str = $s;
	} elsif ($str =~ s/^in\s+//) {
	    push @toks, [ INTOK , 'in' ];
	} elsif ($str =~ s/^pp\.?\s+//i) {
	    push @toks, [ PPTOK , 'in' ];
	} elsif ($str =~ s/^(\p{Lu}+)(?![^\p{L}])//) {
	    push @toks, [ UCSTR , $1 ];
	} elsif ($str =~ s/^(\p{Lc}+)(?![^\p{L}])//) {
	    push @toks, [ LCSTR , $1 ];
	} elsif ($str =~ s/^(\p{L}+)//) {
	    push @toks, [ MCSTR , $1 ];
	} elsif ($str =~ s/^([0-9]+(?:[-—][0-9]+))//) {
	    push @toks, [ NUTOK , $1 ];
	} elsif ($str =~ s/^([,.;:])//) {
	    push @toks, [ PUNCT , $1 ];
	} elsif ($str =~ s/^([()])//) {
	    push @toks, [ PUNCT , $1 ];
	} else {
	    bad('token',"I don't know what to do next--input is `$str'");
	}
    }
    @toks;
}

1;
