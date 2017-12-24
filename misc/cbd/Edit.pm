package ORACC::CBD::Edit;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/edit/;

use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

$ORACC::CBD::Edit::force = 0;

use ORACC::CBD::C11e;
use ORACC::CBD::PPWarn;

sub edit {
    my($args,@cbd) = @_;
    my @clean = c11e($args, @cbd);
    if (cache_check($args,@clean)) {
	my @script = edit_make_script($args,@cbd);
	edit_save_script($args,@script);
	@cbd = edit_apply_script($args,\@cbd,\@script);
	cache_stash($args,@cbd);
    } else {
	pp_warn("glossary fields have been edited directly. Stop.");
	cache_diff($args);
    }
    @cbd;
}

sub cache_check {
    my ($args,@cache) = @_; 
    my $glofile = ".cbdpp/$$args{'lang'}.glo";
    my $ok = 0;
    my $len = -s $glofile;
    if (defined $len && $len > 0) {
	pp_trace("Edit/cache_check -- $glofile len = $len");
	$$args{'cached_cbd'} = $glofile;
	my $cache = join("\n",@cache);
	if ($len == length($cache)) {
	    my $g = '';
	    undef $/; open(G,$glofile); $g = <G>; close(G);
		if ($g cmp $cache) {
		    pp_trace("Edit/cache_check -- $glofile differs from $$args{'cbd'}");
		} else {
		    pp_trace("Edit/cache_check -- $glofile has not been edited");
		    $ok = 1;
		}
	} else {
	    pp_trace("Edit/cache_check -- $glofile different in size than $$args{'cbd'}");
	}
    } else {
	pp_trace("Edit/cache_check -- $glofile nonexistent or empty");
	$ok = 1;
    }
    $ok;
}

sub cache_diff {
    my $args = shift;
    system 'diff', $$args{'cbd'}, $$args{'cached_cbd'};
}

sub cache_stash {
    my $args = shift @_;
    my @c = c11e(@_);
    my $glofile = ".cbdpp/$$args{'lang'}.glo";
    system 'mkdir', '-p', '.cbdpp';
    open(C,">$glofile") || die "cbdpp/Edit: can't write cache $glofile\n";
    print C join("\n", @c), "\n";
    close(C);
}

sub edit_apply_script {
}

sub edit_make_script {
    my($args, @c) = @_;
    my @eds = $ORACC::CBD::Util::data{'edits'};
    my @s = ();
    for (my $i; $i <= $#eds; ++$i) {
	if ($c[$i] =~ /^\+\@entry/) {
	    my $entry = $i;
	    my @e = ();
	    until ($c[$i] =~ /^\@end/ || $i > $#c) {
		push @e, ":add entry";
		push @e, $c[$i];
	    }
	    if ($i > $#c) {
		pp_warn("never found \@end for \@entry starting at line $entry");
	    } else {
		push @s, @e;
	    }
	} elsif ($c[$i] =~ /^\+\@sense/) {
	    my $pos = ($c[$i-1] =~ /^\@sense/ ? 'mid' : '1st');
	    if ($pos eq 'mid') {
		$pos = ($c[$i+1] =~ /^\@sense/ ? 'mid' : 'end');
	    }
	    my $e = pp_entry_of($i,@c);
	    push @s, ":ent $c[$e]";
	    push @s, ":pos $pos";
	    push @s, ":add $c[$i]";
	} elsif ($c[$i] =~ /^>/) {
	    my($tag) = ($c[$i] =~ /(\@[a-z]+)/);
	    my $e = pp_entry_of($i,@c);
	    if ($c[$i-1] =~ /^$tag/) {
		push @s, ":ent $c[$e]";
		push @s, ":del $c[$i-1]";
		push @s, ":add $c[$i]";
	    } else {
		pp_warn("expected $tag before '>$tag'");
	    }
	} elsif ($c[$i] =~ /^-/) {
	    my $e = pp_entry_of($i,@c);
	    push @s, ":ent $c[$e]";
	    push @s, ":del $c[$i]";
	} elsif ($c[$i] =~ /^\@bases/) {
	    my @b = ($c[$i] =~ m/\s([-+]\S+)/g);
	    push @b, ($c[$i] =~ m/(\S+=[0-9])/g);
	    push @b, ($c[$i] =~ m/(\S+>\S+)/g);
	    my $e = pp_entry_of($i,@c);
	    push @s, ":ent $c[$e]";
	    push @s, map { ":bas $_" } @b;
	} elsif (/^=/) {
	    push @s, ":map $c[$i]";
	} else {
	    pp_warn("untrapped edit line $c[$i]");
	}
    }
}

sub edit_save_script {
}

1;
