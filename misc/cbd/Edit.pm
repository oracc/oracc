package ORACC::CBD::Edit;
require Exporter;
@ISA=qw/Exporter/;
@EXPORT = qw/edit/;

use warnings; use strict; use open 'utf8'; use utf8;
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';

use Data::Dumper;

$ORACC::CBD::Edit::force = 0;

use ORACC::CBD::Util;
use ORACC::CBD::C11e;
use ORACC::CBD::PPWarn;
use ORACC::CBD::Entries;
use ORACC::CBD::Bases;
use ORACC::CBD::Senses;

sub edit {
    my($args,@cbd) = @_;
    $ORACC::CBD::PPWarn::trace = 1 if $ORACC::CBD::PPWarn::edit_trace;
    my @clean = c11e($args, @cbd);
    if (cache_check($args,@clean)) {
#	print Dumper \@cbd;
	edit_make_script($args,@cbd); # saves in global %data
	edit_save_script($args);
	@cbd = edit_apply_script($args,@cbd);
#	cache_stash($args,@cbd);
    } else {
	pp_warn("$$args{'cbd'}: glossary fields have been edited directly. Stop. (See 'edit.diff')");
	cache_diff($args);
    }
    @cbd;
}

sub cache_check {
    my ($args,@cache) = @_; 
    my $glofile = ".cbdpp/".ORACC::CBD::Util::lang().".glo";
    my $ok = 0;
    if ($$args{'reset'}) {
	pp_trace("Edit/cache_check -- reset requested");
	unlink $glofile;
	$ok = 1;
    } else {
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
    }
    $ok;
}

sub cache_diff {
    my $args = shift;
    system "diff $$args{'cbd'} $$args{'cached_cbd'} >edit.diff";
    exit 1;
}

sub cache_stash {
    my $args = shift @_;
    my @c = c11e($args, @_);
    my $glofile = ".cbdpp/".ORACC::CBD::Util::lang()."glo";
    system 'mkdir', '-p', '.cbdpp';
    open(C,">$glofile") || die "cbdpp/Edit: can't write cache $glofile\n";
    print C join("\n", @c), "\n";
    close(C);
}

sub edit_apply_script {
    my($args, @c) = @_;
    my @s = @{$ORACC::CBD::data{'script'}};
    my $from_line = 0;
    my %cbddata = %{$ORACC::CBD::data{ORACC::CBD::Util::cbdname()}};
    my %edit_cache = ();
    my %deletia = ();
    for (my $i = 0; $i <= $#s; ++$i) {
	if ($s[$i] =~ /^:cbd (\S+)$/) {
	    pp_file($1);
	} elsif ($s[$i] =~ /^\@(\d+)$/) {
	    $from_line = pp_line($1);
	} elsif ($s[$i] =~ s/^:map =//) {
	    --$from_line; # map = line is the one after \@entry
	    $deletia{$from_line} = 1;
	    my $eid = ${$cbddata{'entries'}}{$s[$i]};
	    if ($eid) {
		my $to_line = -1;
		my @t_slice = ();
		if ($edit_cache{$eid}) {
		    ($to_line,@t_slice) = @{$edit_cache{$eid}};
		} else {
		    $to_line = ${$cbddata{'entries'}}{$eid,'line'};
		    my $to_end_line = $to_line;
		    until ($c[$to_end_line] =~ /^\@end/ || $i > $#c) {
			++$to_end_line;
		    }
		    @t_slice = @c[$to_line .. $to_end_line];
		}
		my $from_end_line = $from_line;
		until ($c[$from_end_line] =~ /^\@end/ || $i > $#c) {
		    ++$from_end_line;
		}
		my @f_slice = @c[$from_line .. $from_end_line];
		my $f = join("\n", @f_slice);
		my $t = join("\n", @t_slice);
		warn "map-from block:\n$f\n";
		warn "-----------------\n";
		warn "map-to block:\n$t\n";
		warn "-----------------\n";
		my @res = entries_merge(\@t_slice, \@f_slice, pp_file(), $to_line, pp_file(), $from_line);
		warn "result block:\n";
		warn join("\n",@res), "\n";
		warn "=================\n";
		@{$edit_cache{$eid}} = ($to_line,@res);
	    } else {
		pp_warn("non-existent map target '$s[$i]'");
	    }
	}
    }
    my @newc = ();
    for (my $i = 0; $i <= $#c; ++$i) {
	if ($c[$i] =~ /^$acd_rx?\@entry\S*\s+(.*?)\s*$/) {
	    my $e = $1;
	    if ($deletia{$i}) {
		until ($c[$i] =~ /^\@end\s+entry/) {
		    ++$i;
		    last if $i > $#c;
		}
		++$i;
	    } else {
		my $eid = ${$cbddata{'entries'}}{$e};
		if ($edit_cache{$eid}) {		    
		    my($ln,@e) = @{$edit_cache{$eid}};
		    push @newc, @e;
		    until ($c[$i] =~ /^\@end\s+entry/) {
			++$i;
			last if $i > $#c;
		    }
		} else {
		    until ($c[$i] =~ /^\@end\s+entry/) {
			push @newc, "$c[$i++]";
			last if $i > $#c;
		    }
		    push @newc, $c[$i];
		}
	    }
	} else {
	    push @newc, $c[$i];
	}
    }
    @newc;
}

sub edit_make_script {
    return unless defined $ORACC::CBD::data{'edit'};
    my($args, @c) = @_;
    my @eds = @{$ORACC::CBD::data{'edit'}};
    my @s = ();
    push @s, ":cbd $$args{'cbd'}";
    warn "edit_make_script: status on entry = ", pp_status(), "\n";
    for (my $ed = 0; $ed <= $#eds; ++$ed) {
	my $i = $eds[$ed];
	push @s, "\@$i";
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
	} elsif ($c[$i] =~ /^=/) {
	    push @s, ":map $c[$i]";
	} else {
	    pp_warn("untrapped edit line $c[$i]");
	}
    }
    unless (pp_status()) {
	@{$ORACC::CBD::data{'script'}} = @s;	
    } else {
	warn "edit_make_script: non-zero status; no script\n";
    }
}

sub edit_save_script {
    if (defined $ORACC::CBD::data{'script'}) {
	open(S, ">edit.edit");
	print S join("\n", @{$ORACC::CBD::data{'script'}}), "\n";
	close(S);
    }
}

1;
