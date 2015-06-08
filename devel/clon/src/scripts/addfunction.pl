#!/bin/perl
# addfunction.pl

# This script will read the source LUTBuilder.cc file and extract the
# function definition and place it in the header of the lut file
#
# usage: addfunction.pl [ function name ] [ file name ]
#
# inputs:
#   function name: name of the function to be searched for
#   file name: name of the file to be inserted
# outputs:
#   the output file with updated header with function definition
#
# written by: Puneet Khetarpal
# date: October 17, 2008
###############################################################################

use strict;
use warnings;
use diagnostics;

# lut builder code path
my $LUTCode = "/usr/local/clas/devel/clon/src/level1/main/LUTBuilder.cc";

# initialize input parameters
my ($funcname, $filename) = ("", "");

# check input parameters
my $numInputs = scalar(@ARGV);
if ($numInputs != 2) {  # check if correct number of inputs
    print "Usage: addfunction.pl [ function name ] [ file name ]\n";
    print "\tfunction name: name of the function to be searched\n";
    print "\tfile name: name of look up table file to insert definition\n";
    exit(1);
} else { # check if look up table file exists
    ($funcname, $filename) = (shift, shift);
    if (!(-e $filename)) {
	print "Look up table file does not exist: $filename\n";
	exit(1);
    }
}

# open the LUTBuilder.cpp file and extract the function

open(INPUT, "< $LUTCode") or die("Could not open $LUTCode: $!\n");
my @code = <INPUT>;
close(INPUT);

my $count = 0;
my $totalLines = scalar(@code);
my @stack;
my $found = 0;

while ((!$found) && ($count < $totalLines)) {
    if ($code[$count] =~ /^int $funcname\(/) {
	while (! ($code[$count] =~ /^\}/)) {
	    push @stack, "# $code[$count]";
	    $count++;
        }
	push @stack, "# $code[$count]";
	$found++;
    }

    $count++;
}

# open the lut file and read its contents
open(INPUT, "< $filename") or die("Could not open $filename: $!\n");
my @lut = <INPUT>;
close(INPUT);

# insert the function definition lines into the lut array
@lut = ($lut[0], "#\n# Function definition: \n", @stack, "#\n", 
	@lut[1..$#lut]);

# open lut file and output updated content
open(OUTPUT, "> $filename") or die("Could not open $filename: $!\n");
print OUTPUT @lut;
close(OUTPUT);
