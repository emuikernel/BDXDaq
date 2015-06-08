h44764
s 00017/00005/00122
d D 1.2 08/11/26 12:42:41 puneetk 2 1
c added input check, and default template source file location
c 
e
s 00127/00000/00000
d D 1.1 08/11/26 12:30:55 puneetk 1 0
c date and time created 08/11/26 12:30:55 by puneetk
e
u
U
f e 0
t
T
I 1
#!/bin/perl
# create_conf.pl

# This script will create a config file from a template provided 
#
D 2
# usage: create_conf.pl 
E 2
I 2
# usage: create_pretrig3conf.pl [ output conf file name ] 
E 2
#
D 2
# input: none
E 2
I 2
# input: output conf file name
E 2
# output: the pretrig3 config file
#
# written by: Puneet Khetarpal
# date: November 19, 2008
###############################################################################

use strict;
use warnings;
use diagnostics;

I 2
# check input #################################################################

if (scalar(@ARGV) != 1) {
    print "Usage: create_pretrig3conf.pl [ conf file]\n",
    "   conf file: pretrig3 configuration file to be created\n";
    exit 1;
}

my $outfileName = shift;

E 2
# read template file #########################################################
D 2
my $template = "pretrig3.conf";
E 2
I 2
my $template = "/usr/local/clas/parms/ic/pretrig3_template.conf";
E 2

D 2
open(INPUT, "< pretrig3.conf") or die("Could not open pretrig3.conf: $!\n");
E 2
I 2
open(INPUT, "< $template") or die("Could not open $template: $!\n");
E 2
my @tempRecords = <INPUT>;
close(INPUT);

# set all channels to 15 mV except the ones listed below #####################

my $discstart = "0xfa010000";
my $discfactor = 65536;

# go through the records
my $index = 0; 
while ($index < $#tempRecords) {
    # if a discriminator is found
    if ($tempRecords[$index] =~ /(0xfa\S+)\s/) {
	# store the disc number
	my $discNumber = GetDiscNumberFromAddress($1);
	$index += 3;
	# loop through the next 16 lines and replace the last number with
	# the threshold that we want
	foreach my $loop (1..16) {
	    # set the channel threshold value
	    if ($discNumber < 29) {
		$tempRecords[$index] =~ s/\d+(\s+)$/15$1/;
	    } else {
		$tempRecords[$index] =~ s/\d+(\s+)$/20$1/;
	    }
	    $index++;
	}
    }
    $index++;
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

# open the IC map and hodoscope files and extract the discriminator and
# channel numbers

my $icout = "/usr/local/clas/parms/ic/IC_out.txt";
open(INPUT, "< $icout") or die("Could not open $icout: $!\n");
my @icrecs = <INPUT>;
close(INPUT);

my ($disc, $chan) = (1, 0);

foreach my $count (0..$#x) {
    # extract discriminator number and channel
    my @test = grep (/^$x[$count] $y[$count]/, @icrecs);
    if ($test[0] =~ /(\d+)\s(\d+)$/) {
	($disc, $chan) = ($1, $2);
	# go through temprecords and find the discriminator and channel
	$index = 0;
	my $found = 0;
	while ($found == 0 && $index < $#tempRecords) {
	   if ($tempRecords[$index] =~ /(0xfa\S+)\s/) {
	       # store the disc number
	       my $discNumber = GetDiscNumberFromAddress($1);
	       if ($discNumber == $disc) {
		   # discriminator is found
		   # find the channel record
		   while ($found == 0) {
		       $index++;
		       if ($tempRecords[$index] =~ /^CHANNEL\s+$chan\s+/) {
			   $tempRecords[$index] =~ s/\d+(\s+)$/$thresh[$count]$1/;
			   $found = 1;
		       }
		   }
	       }
	   }
	    $index++;
	}
    }
}

D 2
open (OUTPUT, "> pretrig3_edit.conf") or die("Could not open file: $!\n");
E 2
I 2
# write to output file ################################

open (OUTPUT, "> $outfileName") or die("Could not open $outfileName: $!\n");
E 2
foreach (@tempRecords) {
    print OUTPUT "$_";
}
close(OUTPUT);


# level 1 #####################################################################

sub GetDiscNumberFromAddress {
    my $address = shift;
    my $disc = (hex($address) - hex($discstart))/ $discfactor;
    $disc++;
    return $disc;
}
E 1
