h03387
s 00002/00002/00040
d D 1.4 08/11/20 13:30:51 puneetk 4 3
c added additional channels as part of the inner ring
c 
e
s 00005/00003/00037
d D 1.3 08/11/19 14:34:50 puneetk 3 2
c added more channels to be set for the inner ring
c 
e
s 00002/00001/00038
d D 1.2 08/11/19 13:16:08 puneetk 2 1
c changed status information to stdin
c 
e
s 00039/00000/00000
d D 1.1 08/11/19 13:08:57 puneetk 1 0
c date and time created 08/11/19 13:08:57 by puneetk
e
u
U
f e 0
t
T
I 1
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
D 2
    print "\nusage: setIC_inner_ring.pl [ threshold (mV) ]\n",
E 2
I 2
    print "\n  usage: setIC_inner_ring.pl [ threshold (mV) ]\n",
E 2
    "\t threshold: threshold for the channels between 1-255 mV\n";
    exit 1;
}

my $threshold = shift;

D 3
my @x = (9,   9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 15);
my @y = (11, 12, 13, 10, 14,  9, 15,  9, 15,  9, 15, 10, 14, 11, 12, 13);
E 3
I 3
my @x = (9,   9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 15,
D 4
	 9,   9, 10, 14, 15, 15, 14);
E 4
I 4
	 9,   9, 10, 14, 15, 15, 14, 10, 15,  9,  9, 15, 15);
E 4
my @y = (11, 12, 13, 10, 14,  9, 15,  9, 15,  9, 15, 10, 14, 11, 12, 13,
D 4
	 10, 14, 15, 15, 15, 10, 9);
E 4
I 4
	 10, 14, 15, 15, 14, 10,  9,  9,  9,  9, 15, 15,  8);
E 4
E 3

I 2
print "Setting IC inner ring threshold to: $threshold mV\n";
E 2
D 3
foreach my $index (0..15) {
E 3
I 3
foreach my $index (0..$#x) {
E 3
    system("setdvcsthresh ic $x[$index] $y[$index] $threshold");
}
E 1
