package ORACC::CBD::Edit;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/edit/;

use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

$ORACC::CBD::Edit::force = 0;

use ORACC::CBD::PPWarn;

sub edit {
    my($proj,$lang,$data_ref,@cbd) = @_;
    my @edits = @{$$data_ref{'edit'}};
    my @clean = remove_edits(\@cbd, \@edits);
    if (cache_check($lang,@clean)) {
	foreach my $i (@edits) {
	    warn "Edit.pm: $i\n";
	}
	cache_stash($lang,@cbd);
    } else {
	pp_warn("glossary fields have been edited directly. Stop.");
    }
    @cbd;
}

sub cache_check {
    my ($lang,@cache) = @_; 
    my $glofile = ".cbdpp/$lang.glo";
    my $ok = 0;
    my $len = -s $glofile;
    if (defined $len && $len > 0) {
	pp_trace("Edit/ok -- $glofile len = $len");
	my $cache = join("\n",@cache);
	if ($len == length($cache)) {
	    my $g = '';
	    undef $/; open(G,$glofile); $g = <G>; close(G);
		if ($g cmp $cache) {
		    pp_trace("Edit/ok -- $glofile differs from new input");
		} else {
		    pp_trace("Edit/ok -- $glofile has not been edited");
		    $ok = 1;
		}
	} else {
	    pp_trace("Edit/ok -- $glofile different in size than new input");
	}
    } else {
	pp_trace("Edit/ok -- $glofile nonexistent or empty");
	$ok = 1;
    }
    $ok;
}

sub cache_create {
    my @cbd = @_;
    my @c;
    my @cache_tags = qw/entry parts sense end/;
    my %cache_tags = (); @cache_tags{@cache_tags} = ();
    for (my $i = 0; $i <= $#cbd; ++$i) {
	my ($tag) = ($cbd[$i] =~ /\@([a-z]+)/);
	next unless $tag;
	push @c, $cbd[$i] if exists $cache_tags{$tag};
    }
    @c;
}

sub cache_stash {
    my $lang = shift @_;
    my @c = cache_create(@_);
    my $glofile = ".cbdpp/$lang.glo";
    system 'mkdir', '-p', '.cbdpp';
    open(C,">$glofile") || die "cbdpp/Edit: can't write cache $glofile\n";
    print C join("\n", @c), "\n";
    close(C);
}

sub remove_edits {
    my($cbd,$eds) = @_;
    my @c = @$cbd;
    foreach my $e (@$eds) {
	my($etok,$tag) = ($e =~ /^(.)?\@([a-z]+)/);
	if ($c[$e] =~ /^\+\@([a-z]+)/) {
	    my $t = $1
	
	} elsif ($c[$e] =~ /^-\@([a-z]+)/) {

	} elsif ($c[$e] =~ /^>\@([a-z]+)/) {
	    
	} elsif ($c[$e] =~ /^=/) {
	    
	} else {
	    
	}
    }
    grep !/^\000$/, @c;
}

1;
