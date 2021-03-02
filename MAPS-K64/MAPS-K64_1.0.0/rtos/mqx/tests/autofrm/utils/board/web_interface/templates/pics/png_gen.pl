#!/usr/bin/perl

use strict;
use warnings;
use GD;
my $pic_name = shift || die "No argument passed";

my $im      = new GD::Image(120,15);
my $white   = $im->colorAllocate(255,255,255);
my $black   = $im->colorAllocate(0,0,0);

$im->transparent($black);
$im->interlaced('true');
$im->fill(1,1,$black);

$im->string(gdLargeFont,1,-2,$pic_name,$white);
$im = $im->copyRotate90();
$im = $im->copyFlipHorizontal();
$im = $im->copyFlipVertical();

open FILE, '>', "$pic_name.png" || die "Unable to open pic file";
binmode FILE;
print FILE $im->png;
close FILE;
