#!/bin/perl
# setIC_inner_ring.pl

# This script will set the thresholds for the inner ring of the inner 
# calorimeter.
# 
# usage: setIC_inner_ring.pl [ threshold (mV) ]
#
# inputs:
#    treshold: the value to be used to set the thresholds for the inner ring
#
# outputs: none
# 
# written by: Puneet Khetarpal
# date: November 19, 2008
###############################################################################

use strict;
use warnings;
use diagnostics;

# check input #################################################################

my $numInputs = scalar(@ARGV);

if ($numInputs != 1) {
    print "\n  usage: setIC_inner_ring.pl [ threshold (mV) ]\n",
    "\t threshold: threshold for the channels between 1-255 mV\n";
    exit 1;
}


# read the inner ring table file and extract x y and threshold values

my $irfile = "/usr/local/clas/parms/ic/IC_inner_ring_thresh.txt";

open(INPUT, "< $irfile") or die ("Could not open $irfile: $!\n");
my @irRecords = <INPUT>;
close(INPUT);

my @x;
my @y;
my @thresh;

foreach my $rec (@irRecords) {
    if ($rec =~ /(\d+)\s+(\d+)\s+(\d+)/) {
	push @x, $1;
	push @y, $2;
	push @thresh, $3;
    }
}

print "Setting IC inner ring thresholds\n";
foreach my $index (0..$#x) {
    system("setdvcsthresh ic $x[$index] $y[$index] $thresh[$index]");
}
