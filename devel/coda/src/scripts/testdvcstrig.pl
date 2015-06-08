#!/bin/perl
# testchannels.pl

# this script will go through each of the coordinates in the pretrig3 board and
# test them
#
# usage: testchannels.pl
#
# inputs: none
# outputs: none
# 
# written by: Puneet Khetarpal
# date: October 13, 2008
###############################################################################

use strict;
use warnings;
use diagnostics;

# level  0 ####################################################################

# get input from command line
# initialize input variables
my ($numClusters, $isStatic) = (1, 0);   # num of clusters and to move cluster

# check inputs
my $numInputs = scalar(@ARGV);
if ($numInputs > 2) {   # if more than 2 input params 
    ShowUsage();
    exit(1);
} elsif ($numInputs == 1) { # if just one input param
    # check to see if it is the static keyword
    if ($ARGV[0] eq "static") {
	$isStatic = 1;
    } elsif ($ARGV[0] >= 1 && $ARGV[0] <= 3) { # check if num clusters
	$numClusters = $ARGV[0];
    } else { # else bad input param
	ShowUsage();
	exit(1);
    }
} elsif ($numInputs == 2) { # if two input params
    # first input param should be number of clusters
    if ($ARGV[0] >= 1 && $ARGV[0] <= 3) {
	$numClusters = $ARGV[0];
    } else {
	ShowUsage();
	exit(1);
    }
    # check for second param for static
    if ($ARGV[1] eq "static") {
	$isStatic = 1;
    } else {
	ShowUsage();
	exit(1);
    }
}

# inform user of what's to happen
print "number of clusters: $numClusters\nStatic Clusters: $isStatic\n";

# set all discriminators to 0x0000 mask
my $cmd = "testdvcstrig";
system("$cmd ic 0 0 0");
system("dimanv mon pretrig3");

# generate a cluster by throwing random number of hits in a 3x3 grid

# first create three grids with index 0..8
my @grid1 = GetRandomGrid();
my @grid2 = GetRandomGrid();
my @grid3 = GetRandomGrid();

# grid index position constants
my @xgrid = (1, 2, 3, 1, 2, 3, 1, 2, 3);
my @ygrid = (1, 1, 1, 2, 2, 2, 3, 3, 3);

# grid motion variables and constants
my $xmove1 = 0;
my $ymove1 = 0;
my $xmove2 = 21;
my $ymove2 = 21;
my $xmove3 = 21;
my $ymove3 = 0;
my $MOVE_SPACE_X = 5;
my $MOVE_SPACE_Y = 5;

#ProcessGrid1(12, 4, 1);

# move the grids
while ($xmove1 < 23) {
    $ymove1 = 0;
    $ymove2 = 21 if ($numClusters >= 2);
    $xmove3 = 21 if ($numClusters == 3);
    while ($ymove1 < 23) {
	# activate grid
	ProcessGrid1($xmove1, $ymove1, 1);
	ProcessGrid2($xmove2, $ymove2, 1) if ($numClusters >= 2);
	ProcessGrid3($xmove3, $ymove3, 1) if ($numClusters == 3);
	# wait for acknowledgment from user
	my $str = qx`caget TS_SCALER_TOTAL0_H`;
	my $str2 = qx`caget TS_SCALER_TOTAL1_H`;
	my $str3 = qx`caget TS_SCALER_TOTAL2_H`;
	print "$str$str2$str3\n";

	#system("/bin/sleep 5");	
	my $wait = <STDIN>;
	$str = qx`caget TS_SCALER_TOTAL0_H`;
	$str2 = qx`caget TS_SCALER_TOTAL1_H`;
	$str3 = qx`caget TS_SCALER_TOTAL2_H`;
	print "$str$str2$str3\n";

	# disable grid
	ProcessGrid1($xmove1, $ymove1, 0);
	ProcessGrid2($xmove2, $ymove2, 0) if ($numClusters >= 2);
	ProcessGrid3($xmove3, $ymove3, 0) if ($numClusters == 3);
        # increment y
	$ymove1 += $MOVE_SPACE_Y;
	$ymove2 -= $MOVE_SPACE_Y if ($numClusters >= 2);
	$xmove3 -= $MOVE_SPACE_X if ($numClusters == 3);
    }
    $xmove1 += $MOVE_SPACE_X;
    $xmove2 -= $MOVE_SPACE_X if ($numClusters >= 2);
    $ymove3 += $MOVE_SPACE_Y if ($numClusters == 3);
}

# level 1 ####################################################################

sub ShowUsage {
    print "usage: testchannels.pl [ \# clusters ] [ static ]\n";
    print "\t\# clusters: number of clusters to simulate (default: 1, max: 3)\n";
    print "\tstatic: if specified, does not move the clusters\n";
}

sub GetRandomGrid {
    # local variables
    my @mygrid = (0, 0, 0, 0, 0, 0, 0, 0, 0);

    # get a random number of hits in the grid
    my $randomHits = int(rand(6)) + 4; # returns number between 1 and 9 inc
    print "number of hits in the grid: $randomHits\n";

    # now get which element in the grid is to be activated
    for my $elem (0..$randomHits-1) {
	my $index = int(rand(9));  # returns number between 0 and 8 inclusive
	while ($mygrid[$index] > 0) {
	    $index = int(rand(9));
	}
	$mygrid[$index]++;
    }

    print "@mygrid\n";

    return @mygrid;
}

sub ProcessGrid1 {
    # local variables
    my ($x_start, $y_start, $toggle) = @_;

    # statements
    for my $index (0..8) {
	if ($grid1[$index] > 0) {
	    my $x = $x_start + $xgrid[$index];
	    my $y = $y_start + $ygrid[$index];
	    print "Position grid at: $x_start $y_start | IC coords: $x $y\n";
	    system("$cmd ic $x $y $toggle > /dev/null");
	}
    }
}

sub ProcessGrid2 {
    # local variables
    my ($x_start, $y_start, $toggle) = @_;

    # statements
    for my $index (0..8) {
	if ($grid2[$index] > 0) {
	    my $x = $x_start + $xgrid[$index];
	    my $y = $y_start + $ygrid[$index];
	    print "Position grid at: $x_start $y_start | IC coords: $x $y\n";
	    system("$cmd ic $x $y $toggle > /dev/null");
	}
    }
}

sub ProcessGrid3 {
    # local variables
    my ($x_start, $y_start, $toggle) = @_;

    # statements
    for my $index (0..8) {
	if ($grid3[$index] > 0) {
	    my $x = $x_start + $xgrid[$index];
	    my $y = $y_start + $ygrid[$index];
	    print "Position grid at: $x_start $y_start | IC coords: $x $y\n";
	    system("$cmd ic $x $y $toggle > /dev/null");
	}
    }
}
