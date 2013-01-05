package ORACC::XVL::AVL;
use warnings; use strict; use open ':utf8';

my $logging = 0;

open(AVLLOG,">avl.log") if $logging;

my %info = ();
my %lists = ();
my %values = ();
my @path = ('.','./lib','@@ORACC@@/lib/data/vl');

sub
badval {
    if ($lists{$_[0]}) {
	$#{$lists{$_[0]}} + 1;
    } else {
	0;
    }
}

sub
list {
    if ($lists{$_[0]}) {
	@{$lists{$_[0]}};
    } else {
	();
    }
}

sub
load_class {
    my $class = shift;
    my $loaded = 0;
    unless ($values{$class}) {
	foreach my $p (@path) {
	    if (-e "$p/$class.avl") {
		open(C,"$p/$class.avl");
		my $val = 0;
		while (<C>) {
		    next if /^[\&\#\@]/ || /^\s*$/;
		    if (/^(.*?)(?:\t|$)/) {
			my $v = $1;
			$v =~ s/^\s*(.*?)\s*$/$1/;
			${$values{$class}}{$1} = ++$val
			    unless ${$values{$class}}{$1}; # skip duplicates
			logval($class, $1, $val);
		    } else {
			die;
		    }
		}
		close(C);
		$loaded = 1;
		@{$lists{$class}} = 
		    sort { ${$values{$class}}{$a} <=> 
			       ${$values{$class}}{$b} } keys %{$values{$class}};
	    }
	}
	die "ORACC::AVL::AVL: couldn't find $class.avl; stop.\n" unless $loaded;
    }
}

sub
logval {
    if ($logging) {
	print AVLLOG "$_[0] $_[1] $_[2]\n";
    }
}

sub
path {
    @path = (@_, @path);
}

sub
sortcode {
    my ($class,$value) = @_;
    if ($values{$class} && $value) {
	# load_class cannot generate sortcode=0 so this is
	# a safe test for undefined value
	${$values{$class}}{$value} || 0;
    } else {
	0;
    }
}

sub
values {
    my($class) = @_;
    return $values{$class};
}
1;
