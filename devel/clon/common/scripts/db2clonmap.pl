#!/usr/bin/env perl 
#
#   db2map.pl: Create the .map files from "calib" database
#   Created  : 25-MAY-2000 by Riad Suleiman
#   $Id: db2map.pl,v 1.21 2007/05/08 17:28:24 marki Exp $
# Oct 2007: Sergey: copy to our area, rename to db2clonmap.pl, fix a little
#

##use lib ("$ENV{CLAS_TOOLS}/caldb");
use lib ("/usr/local/clas/release/current/clon/common/perl");
use CaldbUtil;

%typecode = ('int', 0, 'intblob', 0, 'float', 1, 'floatblo', 1, 'char', 2);
@function_suffix =  ('int', 'float', 'char');

# deal with input arguments

eval "\$$1=\$2" while $ARGV[0] =~ /^(\w+)=(.*)/ && shift; # see camel book 

$database = "calib";
$user = "clasuser";
$password = "";
if (!$hostname) {
    $hostname = "clasdb.jlab.org";
}

if ($help) {
    &PrintUsage();
    exit 0;
}

if (!$runMin) {
    print "ERROR: minimum run runMin not defined\n";
    &PrintUsage();
    exit 1;
}

if (!$runMax) {
    print "ERROR: maximum run runMax not defined\n";
    &PrintUsage();
    exit 1;
}                            

if (!$runIndexTable) {
    $runIndexTable = "RunIndex";
}

if (!$system) {
    if (!$quiet) {print "INFO: Making maps for all systems.\n";}
} else {
    if (!$quiet) {print "INFO: Making map for system $system.\n";}
}

if (!$time) {
    $time = '2037-1-1';
}
#
# check for authentic Map binaries
#
$put_map_float_exists = `ls $ENV{CLON_BIN}/put_map_float | wc -l`;
chomp $put_map_float_exists;
#print "put_map_float_exists = /$put_map_float_exists/\n";
if ($put_map_float_exists == 0) {
    die 'ERROR: cannot find put_map_float in $CLON_BIN';
}
$mysql_in_binary = `nm $ENV{CLON_BIN}/put_map_float | grep mysql | wc -l`;
chomp $mysql_in_binary;
#print "mysql_in_binary = /$mysql_in_binary/\n";
if ($mysql_in_binary > 0) {
    $message = "ERROR: \$CLON_BIN/put_map_float uses the CalDB."
	. " Need version of \$CLON_BIN\n"
	. "        that uses native Map routines";
    die $message;
}
#
# make a local directory
#
$dirname = "Maps_$runMin-$runMax";
if (! -e $dirname) {system("mkdir $dirname");}
#
# connect to server
#
if ($quiet) {
    $verbose = 0;
} else {
    $verbose = 1;
}
ConnectToServer($hostname, $user, $password, $database, $verbose, \$dbh);

# loop through systems
$subsystem = '';
$item = '';
ShowSystems($dbh, $subsystem, $item, \@system_array);
foreach $is (0 .. $#system_array) {
    $system_this = $system_array[$is];

    if ((!$skip_run_control || $system_this ne 'RUN_CONTROL')
	&& (!$system || $system_this eq $system)) {

	if (!$quiet) {print "$system_this\n";}

# create the map file

	$mapname = "$dirname/$system_this.map";
	if (-e $mapname) {
	    print "File $mapname exists, will not make a new map, skipping this system.\n";
	} else {
	    system("$ENV{CLON_BIN}/create_map $mapname");

# loop over subsystems

	    $item = '';
	    ShowSubsystems($dbh, $system_this, $item, \@subsystem_array);
	    foreach $iss (0 .. $#subsystem_array) {
		$subsystem = $subsystem_array[$iss];
		if (!$quiet) {print "  $subsystem\n";}
		
# loop over items
		
		ShowItems($dbh, $system_this, $subsystem, \@item_array);
		foreach $ii (0 .. $#item_array) {
		    $item = $item_array[$ii];
		    if (!$quiet) {print "    $item\n";}
		    
# get item info
		    
		    ShowItemInfo($dbh, $system_this, $subsystem, $item, \%itemInfo);
		    $length=$itemInfo{length};
		    $type=$itemInfo{type};
		    if (!$quiet) {print "      length=$length, type=$type\n";}
		    
# add the item
		    
		    $command = "$ENV{CLON_BIN}/add_map_item -m$mapname -s$subsystem -i$item -l$length -a$typecode{$type}";
		    #print "      $command\n";
		    system($command);
		    
# get the run ranges
		    ShowRunRanges($dbh, $system_this, $subsystem, $item, $runIndexTable, $time, $runMin,$runMax, \@runRangeInfo);
		    #print "      number of ranges - 1 = $#runRangeInfo\n";
		    foreach $ir (0 .. $#runRangeInfo) {
			$runStart = $runRangeInfo[$ir][0];
			$runEnd =  $runRangeInfo[$ir][1];
			$itemValueId = $runRangeInfo[$ir][2];
			$time_stamp =  $runRangeInfo[$ir][3];
			$officer =  $runRangeInfo[$ir][4];
			$comment =  $runRangeInfo[$ir][5];
			#print "        $runStart, $runEnd, $itemValueId, $time_stamp,"
			#    . " $officer, $comment\n";
			if (!($runMin > $runEnd || $runMax < $runStart)) {
			    &PutConstantsIntoMap;
			}
		    }
		}
	    }
	}
    }
}

exit 0;

sub PrintUsage {
    print <<EOM;
Purpose:
    Creates Map files from the CalDB. If system is specified only one
    Map is made. If not, Map files are made for all systems.

Warning:
    Make sure CLON_BIN points to a directory with "real" Map utilities
    (not Map utilities linked with CalDB's Map emulation).  These are
    there up to and including release-2-5 and from release-4-4 on.

usage: 
    db2map.pl runMin=<minimum run> runMax=<maximum run> \\
      [system=<system name>] \\
      [skip_run_control=<non-zero to skip RUN_CONTROL system>] \\
      [runIndexTable=<run index table name>] [time=<date/time of validity>] \\
      [hostname=<db server hostname>] [quiet=<non-zero to supress printout>] \\
      [help=<non-zero for usage message]

Example:
    db2map.pl system=SC_CALIBRATIONS runMin=1 runMax=1000000
EOM
}

sub PutConstantsIntoMap {

# get constants from the database

    ReadConstants($dbh, $system_this, $subsystem, $item, $runStart, $runIndexTable,
		  $time, \@itemvalue);
    $itemValueId = $itemvalue[0];
    $author = $itemvalue[1];
    $time_stamp = $itemvalue[2];
    $minRunSource = $itemvalue[3];
    $maxRunSource = $itemvalue[4];
    $comment = $itemvalue[5];
    #print "          $itemValueId, $author, $time_stamp, $minRunSource, $maxRunSource, $comment\n";
    $command = "$ENV{CLON_BIN}/";
    $command .= "put_map_$function_suffix[$typecode{$type}]";
    $command .= " -m$mapname -s$subsystem -i$item -t$runStart";
    #print "          command=$command\n";
    open (PUTMAP, '| ' . $command);
    foreach $iv (6 .. $#itemvalue) {
	$itemvalue_this = $itemvalue[$iv];
#       avoid error message from put_map_char if a char array starts with LF
#       by dropping the LF
	if ($type eq 'char' && ord(substr($itemvalue_this,0,1)) == 10) {
	    $itemvalue_this = substr($itemvalue_this, 1);
	}
	if ($type eq 'char' && ord(substr($itemvalue_this,0,1)) == 0) {
	    $itemvalue_this = ' ' . $itemvalue_this;
	}
	print PUTMAP "$itemvalue_this\n";
    }
    close (PUTMAP);

}
# end of perl script
