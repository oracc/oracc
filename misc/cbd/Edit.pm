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
    my @cache = cache(@cbd);
    if (pristine($lang,@cache)) {
	my @edits = @{$$data_ref{'edit'}};
	foreach my $i (@edits) {
	    warn "Edit.pm: $i\n";
	}
    } else {
	pp_warn("glossary fields have been edited directly. Stop.");
    }
    @cbd;
}

sub cache {
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

sub pristine {
    my ($lang,@cache) = @_;
    my $glofile = ".cbdpp/$lang.glo";
    my $pristine = 0;
    my $len = -s $glofile;
    if (defined $len && $len > 0) {
	pp_trace("Edit/pristine -- $glofile len = $len");
	my $cache = join("\n",@cache);
	if ($len == length($cache)) {
	    my $g = '';
	    undef $/; open(G,$glofile); $g = <G>; close(G);
		if ($g cmp $cache) {
		    pp_trace("Edit/pristine -- $glofile differs from new input");
		} else {
		    pp_trace("Edit/pristine -- $glofile has not been edited");
		    $pristine = 1;
		}
	} else {
	    pp_trace("Edit/pristine -- $glofile different in size than new input");
	}
    } else {
	pp_trace("Edit/pristine -- $glofile nonexistent or empty");
	$pristine = 1;
    }
    $pristine;
}

1;
