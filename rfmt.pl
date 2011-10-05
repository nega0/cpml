#!/usr/bin/perl -w

use strict;

while (<>) {
  my @a = split "\t";
  print "$a[0]\t$a[1] vs $a[3]\n";
  print "$a[0]\t";

  if ($a[2] < $a[4]) {
    my $d = $a[4] - $a[2];
    print"$a[1] is faster($d)\n";
  } else {
    my $d = $a[2] - $a[4];
    print "$a[3] is faster($d)\n";
  }
  print "\n";
}
