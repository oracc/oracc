#!/usr/bin/perl
use warnings; use strict;
use lib '/Users/stinney/orc/lib';
use ORACC::XMD::Fields;
use CGI;

my $q = CGI->new();

my $cgimode = $q->param('cgimode');
my $input;
my $lang = '';
my $project = '';
my $useproto = '';
my $langline = '';
my $useline = '';

if (!$cgimode) {
#    undef $/;
    $input = join('',(<>));
} else {
    $input = $q->param('input');
}

$lang = $q->param('lang');
$project = $q->param('project');
$useproto = $q->param('use');

if ($lang) {
    $langline = "#atf: lang $lang\n";
}

if ($project) {
    $project =~ s/(\S+)/#project: \L$1\n/;
}

if ($useproto && $useproto ne 'none') {
    $useline = "#atf: use $useproto\n";
}

my %dollar = (
    ']'=>'broken', '#'=>'traces', '_'=>'blank',
    );
my %surface = (
    'o'=>'obverse', 'r'=>'reverse',
    'le'=>'left','re'=>'right',
    'te'=>'top','be'=>'bottom',
    );

print $q->header(-type=>"text") if $cgimode;

$input =~ tr/\r/\n/;
my @lines = grep defined&&length, split(/\n/,$input);

foreach my $l (@lines) {

    if ($l =~ /^\#/) {
	print "$l\n";
	next;
    }

    if ($l =~ /^\&/) {
	if ($l =~ /^&[PQX]\d+/) {
	    my($pq,$n) = ($l =~ /^\&(.*?)(?:\s*=\s*(.*))?$/);
	    my $f = ORACC::XMD::Fields::get_in_project('cdli',$pq);
	    my $des = $$f{'designation'};
	    ORACC::XMD::Fields::drop_in_project('cdli',$pq);
	    print "\&$pq = $des\n";
	    if ($n && $n ne $des) {
		print "#$des\n";
	    }
	} else {
	    my $orig = $l;
	    $l =~ s/^\&\s*//;
	    $l =~ s/^X\d+\s*//;
	    $l =~ s/^=\s*//;
	    $l =~ tr/ /_/;
	    $l =~ s/_+/_/g;
	    my @res = `/usr/local/oracc/bin/se \#cdli \!cat $l`;
	    if ($#res >= 0) {
		foreach my $pq (@res) {
		    $pq =~ s/\s*$//;
		    my $f = ORACC::XMD::Fields::get_in_project('cdli',$pq);
		    my $des = $$f{'designation'};
		    ORACC::XMD::Fields::drop_in_project('cdli',$pq);
		    print "\&$pq = $des\n";
		}
		print $project;
		print $useline;
		print $langline;
		$orig =~ s/^\&/\#/;
	    } else {
		print $project;
		print $useline;
		print $langline;
	    }
	    print "$orig\n\n";
	}
	next;
    }

    my @tokens = grep defined&&length, split(/\s+/,$l);
    for (my $i = 0; $i <= $#tokens; ++$i) {
	$_ = $tokens[$i];
	my $dollar = '';
	my $surface = '';
	my $surface_post = '';
	my $column = 0;
	my $column_post = '';
	my $from_line = 0;
	my $from_post = '';
	my $to_line = 0;
	my $to_post = '';
	my $orig = '';
	
	# $-lines: ] = broken, # = traces , _ = blank
	if (s/([\]\#_])//) {
	    $dollar = $dollar{$1};
	}
	
	if (s/^([ores]|[lrtb]e)//) {
	    $surface = $surface{$1};
	    $surface_post = $_;
	} elsif (s/^c(\d*)// || s/^(\d+)(?=c)//) {
	    $column = $1;
	    $column_post = $_
	} elsif (m/^(.*?)-(.*?)$/) {
	    $orig = $_;
	    my($fr,$to) = ($1,$2);
	    $fr =~ s/^(\d+)//;
	    $from_line = $1; $from_post = $fr;
	    $to =~ s/^(\d+)//;
	    $to_line = $1;
        } elsif (m/^(\d+)/) {
	    $orig = $_;
	    s/^(\d+)//;
	    $from_line = $1;
	    $from_post = $_;
	} else {
	    warn("$_: unknown token\n");
	}
	
	if ($dollar) {
	    print "\$ ";
	    if ($surface) {
		if ($surface_post) {
		    if ($surface_post eq '+') {
			print "rest of $surface $dollar\n";
		    } else {
			print "start of $surface $dollar\n";
		    }
		} else {
		    print "$surface $dollar\n";
		}
	    } elsif ($column || $column_post) {
		if ($column_post) {
		    if ($column_post eq '+') {
			print "rest of column $dollar\n";
		    } elsif ($column_post eq '-') {
			print "start of column $dollar\n";
		    } else {
			print "$column columns $dollar\n";
		    }
		} else {
		    print "column $column $dollar\n";
		}
	    } else {
		print "$from_line lines $dollar\n";
	    }
	} else {
	    if ($surface) {
		print "\@$surface$_\n";
	    } elsif ($column) {
		print "\@column $column$column_post\n";
	    } else {
		if ($to_line) {
		    foreach my $l ($from_line .. $to_line) {
			print "$l$from_post.\t\n";
		    }
		} else {
		    print "$from_line$from_post.\t\n";
		}
	    }
	}
    }
}

1;
