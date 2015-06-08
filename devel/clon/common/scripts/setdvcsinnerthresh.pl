#!/bin/perl
# setdvcsinnerthresh.pl

# This script will set the thresholds for the dvcstrig discriminator for 
# 1st and 2nd inner rings of IC crystals
#
# usage: setdvcsinnerthresh.pl [-f file name] | [-r [1 | 2] -t threshold]
#
# inputs:
#   -f file name: use this switch to specify a file containing the coordinates
#                 and thresholds to be set for IC in the format:
#                 ICx1 ICy1 threshold(in mV)
#                 ICx2 ICy2 threshold(in mV)
#   -r 1|2 : use this switch to specify (1) inner most ring or (2) second
#            inner most ring of crystals
#   -t threshold: this is required with the -r switch to specify the threshold
#            for the ring of choice (1-255 mV)
#
# outputs:
#   the status of the changed discriminator channels
# 
# written by: Puneet Khetarpal
# date: October 7, 2009
###############################################################################

# modules used ################################################################

use strict;
use warnings;
use diagnostics;
use Getopt::Std;  # for command line options

# level 0 #####################################################################

# check inputs

my $fileName = "";
my $ring = 0;
my $threshold = 1;
my %options;
getopts("f:r:t:", \%options);

# look for file name
if ($options{f}) {
    $fileName = $options{f};
    # check if the file name exists
    if (length($fileName) > 0 && (-f $fileName)) {
        print "$fileName will be used for threshold values\n";
        ProcessFile();
    } else {
        print "Invalid file name specified: $fileName\n";
        PrintUsage();
        exit;
    }
} elsif (defined $options{r} && defined $options{t}) { # require both ring number and threshold
    $ring = int($options{r});
    $threshold = int($options{t});

    if (($ring != 1) && ($ring != 2) || ($threshold < 1) || ($threshold > 255)){
      print "Invalid input: -r $ring -t $threshold\n";
      PrintUsage();
      exit; 
    } 
    ProcessRing();
} else {
    PrintUsage();
    exit;      
}


# level 1 #####################################################################

# preconditions: the file name is viable
# postconditions: the file is read and its contents extracted and the
#    thresholds based on the coordinates are set
sub ProcessFile {
    # local variables
    my $x = 0;
    my $y = 0;
    my $val = 0;  

    # statements
    # open file and read its contents
    open(INPUT, "< $fileName") or die("Could not open $fileName: $!\n");
    my @records = <INPUT>;
    close(INPUT);

    # loop through the records
    foreach my $rec (@records) {        
	if ($rec =~ /\s*(\S+)\s+(\S+)\s+(\S+)\s*$/) {
          ($x, $y, $val) = ($1, $2, $3); 
	}
        print "$x, $y, $val\n";
        # set the thresholds
        system("setdvcsthresh ic $x $y $val");
    }
}

# preconditions: ring is either 1 or 2, and the threshold values are 
#     within permissible range
# postconditions: the thresholds for inner and second inner ring will be set
#     depending on the ring number specified
sub ProcessRing {
    # statements
    if ($ring == 1) {
	print "Setting inner most ring (1) discriminators to $threshold mV...\n";
        ProcessInnerRing();
    } else {
        print "Setting second inner most ring (2) discriminators to $threshold mv...\n";
	ProcessSecondInnerRing();
    }

}



# preconditions: none
# postconditions: the usage of the script is displayed on the screen
sub PrintUsage {
    print <<EOF;

 usage: setdvcsinnerthresh.pl [-f file name] | [-r [1 | 2] -t threshold]

 inputs:
   -f file name: use this switch to specify a file containing the coordinates
                 and thresholds to be set for IC in the format:
                 ICx1 ICy1 threshold(in mV)
                 ICx2 ICy2 threshold(in mV)
   -r 1|2 : use this switch to specify (1) inner most ring or (2) second
            inner most ring of crystals
   -t threshold: this is required with the -r switch to specify the threshold
            for the ring of choice (1-255 mV)
EOF
    return;
}

# level 2 #####################################################################

# precondition: the threshold value is viable
# postcondition: the thresholds for inner most ring discriminators are set
sub ProcessInnerRing {
    # local variables
    my @xCoord = ( 9, 10, 11, 12, 13, 14, 15,
                   9, 10,             14, 15,
                   9,                     15,
                   9,                     15,
                   9,                     15,
                   9, 10,             14, 15,
                   9, 10, 11, 12, 13, 14, 15);
    my @yCoord = (15, 15, 15, 15, 15, 15, 15,
                  14, 14,             14, 14,
                  13,                     13,
                  12,                     12, 
                  11,                     11, 
                  10, 10,             10, 10,
                   9,  9,  9,  9,  9,  9,  9);

    my $num = $#xCoord;

    # statements
    foreach my $i (0..$num) {
	print "$xCoord[$i], $yCoord[$i], $threshold\n";
        system("setdvcsthresh ic $xCoord[$i] $yCoord[$i] $threshold");
    }    

    print "Done.\n";
}

# precondition: the threshold value is viable
# postcondition: the thresholds for 2nd inner most ring discriminators are set
sub ProcessSecondInnerRing {
    # local variables
    my @xCoord = ( 8,  9, 10, 11, 12, 13, 14, 15, 16,
                   8,                             16,
                   8,                             16,
                   8,                             16, 
                   8,                             16,
                   8,                             16,
                   8,                             16,
                   8,                             16,
                   8,  9, 10, 11, 12, 13, 14, 15, 16);
    my @yCoord = (16, 16, 16, 16, 16, 16, 16, 16, 16,
                  15,                             15,
                  14,                             14,
                  13,                             13, 
                  12,                             12,
                  11,                             11, 
                  10,                             10, 
                   9,                              9,
 		   8,  8,  8,  8,  8,  8,  8,  8,  8);

    my $num = $#xCoord;

    # statements
    foreach my $i (0..$num) {
	print "$xCoord[$i], $yCoord[$i], $threshold\n";
        system("setdvcsthresh ic $xCoord[$i] $yCoord[$i] $threshold");
    }    

    print "Done.\n";
}

