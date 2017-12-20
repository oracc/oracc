package ORACC::CBD::SuxNorm;
use warnings; use strict; use open 'utf8'; use utf8;

use ORACC::CBD::PPWarn;

my %norms = ();

#
# This version of the sux-norm code expects an array of lines
# as created by cbdpp.plx
#
sub normify {
    my $glo = shift;
    my $base = '';
    my $cf = '';
    my $compound = 0;
    my @forms = ();
    my $in_entry = 0;
    my $lang = '';
    my @lines = ();
    my @parts = ();
    my $skipping = 0;

    for (my $i = 0; $i <= $#_; ++$i) {
	push(@lines,$_[$i]);
	
	next if $_[$i] =~ /^\000$/;

	local($_) = $lines[$i];

	tr/∼/~/ 
	    && pp_warn "$glo:$i: autocorrected Unicode tilde (∼)--please change to ~ in source file";
	if (/^\@lang\s+(\S+)/) {
	    $lang = $1;
	} if (/^\@parts/) {
	    $skipping = 1;
	} elsif (/^\@entry/) {
	    warn "glo:$i: (normify) missing \@end entry\n" if $in_entry;
	    @forms = ();
	    my($xcf,$gw,$pos) = (/\s(\S+)\s+\[(.*?)\]\s+(\S+)\s*$/);
	    if ($xcf && $gw && $pos) {
		$cf = $xcf;
	    $base = "$xcf\[$gw\]$pos";
	    } else {
		warn "$glo:$i: (normify) bad \@entry\n";
		$skipping = 1;
	    }
	    $in_entry = 1;
	} if (/^\@sense\s+(\S+)\s*(.*?)$/) {
	    my $sbase = "$cf\[$2\]$1";
	    foreach my $f (@forms) {
		register_norm($$f[0], $sbase, $$f[1]);
	    }
	} elsif (/^\@end/) {
	    $skipping = 0;
	    $in_entry = 0;
	} elsif (/^\@form/ && $lang =~ /qpn/ && !/\%\S+/) {
	    warn "$glo:$i: (normify) no %lang code in qpn form\n";
	} elsif (/^\@form/ && !$skipping && ($lang =~ /sux/ || /\%sux/) && !/\$\(/) {
	    
	    ## Note that this code does not get executed in sux COFs becasue
	    ## of the test in the elsif above; this means that NORM must be
	    ## maintained manually in sux COFs
	    
	    my($form) = (/^\@form!?\s+(\S+)/);
	    my($stem) = (/\s\*(\S+)/);
	    my($morph) = (/\s\#([^\#]\S*)/);
	    if ($morph) {
		my $norm = norm_from_m1($morph,$stem||$cf);
		register_norm($form, $base, $norm);
		push @forms, [ $form , $norm ];
		if (/\s\$/) {
		    s/(\s\$)\S+/$1$norm /;
		} else {
		    s/^(\@form!?\s+\S+\s+(?:\%sux\S*\s+)?)/$1\$$norm /;
		}
		$lines[$i] = $_;
	    } else {
		warn "$glo:$i: (normify) no #morph in line\n";
	    }
	}
    }

    #warn Dumper(\%norms);

    my @pparts = ();
    for (my $i = 0; $i <= $#lines; ++$i) {
	my $err_line = $i + 1;
	if ($lines[$i] =~ /^\@parts/) {
	    $compound = 1;
	    push @pparts, [ split_parts($lines[$i]) ];
	} elsif ($compound && $lines[$i] =~ /^\@form/ && ($lang =~ /sux/ || $lines[$i] =~ /\%sux/)) {
	    next if $lines[$i] =~ /\s\$/;
	    my($form) = ($lines[$i] =~ /^\@form!?\s+(\S+)/);
	    my @forms = split(/_/,$form);
	    my $parts_ok = 0;
	    my @parts_errs = ();
	    foreach my $pparts (@pparts) {
		my @parts = @{$pparts};
		my @this_parts_errs = ();
		if ($#forms == $#parts) {
		    my $last_non_zero_form = '';
		    for (my $j = 0; $j <= $#forms; ++$j) {
			if ($forms[$j] eq '0') {
			    $forms[$j] = $last_non_zero_form;
			} else {
			    $last_non_zero_form = $forms[$j];
			}
			my $key = "$forms[$j]=$parts[$j]";
			$key =~ s#//(.*?)\]#]#;
			$key =~ s#'.*$##;
			if ($norms{$key}) {
			    $lines[$i] .= " \$$norms{$key}";
			} else {
			    if ($key eq 'n=n[]n') {
				$lines[$i] .= " \$n";
			    } else {
				push @this_parts_errs, "$glo:$err_line: (normify) no NORM for $key\n";
			    }
			}
		    }
		} else {
		    warn "$glo:$err_line: (normify) compound's # of forms != # of parts (`@forms' vs. `@parts'\n";
		}
		if ($#this_parts_errs < 0) {
		    $parts_ok = 1;
		    @parts_errs = ();
		    last;
		} else {
		    @parts_errs = @this_parts_errs;
		    $lines[$i] =~ s/\s\$.*$//;
		}
	    }
	    if (!$parts_ok) {
		warn @parts_errs;
		@parts_errs = ();
	    }
	} elsif ($lines[$i] =~ /^\@end/) {
	    $compound = 0;
	    @pparts = ();
	}
    }
    @lines;
}

sub norm_from_m1 {
    my($m1,$stem) = @_;
    if ($m1 =~ /~/) {
	$m1 =~ s/~/$stem/;
    }
    $m1 =~ s/(^|[.,:;!]).*?=/$1/g;
    $m1;
}

sub register_norm {
    my($form,$base,$norm) = @_;
    $norms{"$form=$base"} = $norm;
    my $b2 = $base;
    $b2 =~ s/\[(.*?),.*\]/[$1]/;
    $norms{"$form=$b2"} = $norm unless $b2 eq $base;
    my $b3 = $b2;
    $b3 =~ s/\[(?:to\s+(?:be\s+)?)(.*?)\]/[$1]/;
    $norms{"$form=$b3"} = $norm unless $b3 eq $b2;
    my $b4 = $base;
    $b4 =~ s/\[\(.*?\)\s+(.*?)\]/[$1]/;
    $norms{"$form=$b4"} = $norm unless $b4 eq $base;
}

sub split_parts {
    my $tmp = shift;
    $tmp =~ s/^\@parts\s*//;
    $tmp =~ s/(\](\S+))\s+/$1\cA/g;
    split(/\cA/, $tmp);
}

1;
