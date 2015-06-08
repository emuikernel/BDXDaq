h25169
s 00274/00000/00000
d D 1.1 08/10/21 16:16:42 puneetk 1 0
c date and time created 08/10/21 16:16:42 by puneetk
e
u
U
f e 0
t
T
I 1
#!/bin/perl
# ic_clusters.pl

# this script will go through each of the coordinates in the pretrig3 board and
# test them
#
# usage: ic_clusters.pl [ # of clusters ] [ static | nostatic delay x-space 
#        y-space ]
#
# inputs: 
#    # of clusters: number of clusters to generate limit: 1-3
#    static | nostatic: specify flag if want to move the clusters
#        if nostatic is specified, then require delay (in seconds) to wait
#        before moving each cluster, x-space and y-space is the spacing 
#        between the motion of the clusters.
# outputs: 
#    the cluster information and TS_SCALAR value after each cluster
#    generation
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
my $DELAY = -1;
my $MOVE_SPACE_X = 3;
my $MOVE_SPACE_Y = 3;

# check inputs
my $numInputs = scalar(@ARGV);
if ($numInputs > 5 || $numInputs < 2) {   # if incorrect input params 
    ShowUsage();
    exit(1);
} elsif ($numInputs >= 2) { # if two or more input params
    # first input param should be number of clusters
    if ($ARGV[0] >= 1 && $ARGV[0] <= 3) {
	$numClusters = $ARGV[0];
    } else {
	ShowUsage();
	exit(1);
    }
    # check for second param for static or nostatic
    if ($ARGV[1] eq "static") {
	$isStatic = 1;
	if ($numInputs > 2) {
	    ShowUsage();
	    exit(1);
	}
    } elsif ($ARGV[1] eq "nostatic") {
	$isStatic = 0;
    } else {
	ShowUsage();
	exit(1);
    }
}

# if no static and > 2 input parms, then extract delay and spacing
if ((not $isStatic) && $numInputs > 2) {
    # check if delay is specified
    if ($numInputs == 3) {
	if ($ARGV[2] < -1 || $ARGV[2] > 60) {
	    ShowUsage();
	    exit(1);
	}
	$DELAY = $ARGV[2];
    } elsif ($numInputs == 4) {
	if ($ARGV[3] < 1 || $ARGV[3] > 10) {
	    ShowUsage();
	    exit(1);
	} 
	$MOVE_SPACE_X = $ARGV[3];
	$MOVE_SPACE_Y = $ARGV[3];
    } elsif ($numInputs == 5) {
	if ($ARGV[3] < 1 || $ARGV[4] < 1 || $ARGV[3] > 10 || $ARGV[4] > 10) {
	    ShowUsage();
	    exit(1);
	}
	$MOVE_SPACE_X = $ARGV[3];
	$MOVE_SPACE_Y = $ARGV[4];
    }
}


# inform user of what's to happen
print "Number of clusters: $numClusters\nStatic Clusters: $isStatic\n";
print "Delay: $DELAY\nX Spacing: $MOVE_SPACE_X\nY Spacing: $MOVE_SPACE_Y\n";

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

# if static, then set up clusters in predefined location
if ($isStatic) {
    SetupStaticClusters();
} else {
    SetupNoStaticClusters();
}

# level 1 ####################################################################

sub ShowUsage {
    print "usage: ic_clusters.pl [ \# clusters ] [ static | nostatic ]\n",
          " [ delay ] [ spacing | x spacing  y spacing ]\n";
    print "\t\# clusters: number of clusters to simulate (1-3)\n";
    print "\tstatic or nostatic: not to move or to move the clusters\n";
    print "\t   if nostatic is specified, then the following options may\n",
    "\t   also be specified\n";
    print "\tdelay: in seconds specify the amount of time to wait before\n",
    "\t  moving the cluster. If -1 is specified, then clusters will be moved by\n",
    "\t  pushing the ENTER key, otherwise limit is 60 seconds. Default: -1 \n";
    print "\tspacing: number of cells to move the cluster (1-10). If just\n",
    "\t  one spacing is specified then it is assumed to be the same for both\n",
    "\t  X and Y. Default: 3\n";
    print "\tX spacing: number of cells to move cluster in X dir (1-10)\n";
    print "\tY spacing: number of cells to move cluster in Y dir (1-10)\n\n";
}

sub GetRandomGrid {
    # local variables
    my @mygrid = (0, 0, 0, 0, 0, 0, 0, 0, 0);

    # get a random number of hits in the grid
    #my $randomHits = int(rand(6)) + 4; # returns number between 4 and 9 inc
    my $randomHits = int(rand(9)) + 1;
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

sub SetupStaticClusters {
    # set the grids accordingly
    print "Setting up static clusters\n";
    ProcessGrid1(0, 6, 1);
    ProcessGrid2(20, 10, 1) if ($numClusters >= 2);
    ProcessGrid3(15, 1, 1) if ($numClusters == 3);
    # get the ic scaler values
    my $str = qx`caget TS_SCALER_TOTAL0_H`;
    my $str2 = qx`caget TS_SCALER_TOTAL1_H`;
    my $str3 = qx`caget TS_SCALER_TOTAL2_H`;
    # print the output
    print "$str$str2$str3\n";
}

sub SetupNoStaticClusters {
    print "Setting up non static clusters\n";
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

	    if ($DELAY == -1) {
		print "Please press ENTER to continue: ";
		my $wait = <STDIN>;
	    } else {
		system("/bin/sleep $DELAY");
	    }

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
}

# level 2 #####################################################################


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
E 1
