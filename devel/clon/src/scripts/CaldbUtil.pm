package CaldbUtil;
require Exporter;
use DBI;

@ISA = qw(Exporter);
@EXPORT = qw(ConnectToServer ReadConstants ReadConstantsByIndex
	     ShowSetsRun ShowRunRanges
	     ShowHistory WriteConstantSet LinkConstantSet
	     WriteAndLinkConstantSet FormatTimestamp ShowSystems
	     ShowSubsystems ShowItems ShowSSI ShowItemInfo 
             ShowConstantSetId ShowSetsItem ShowChangesIndex
	     ShowChangesConstants DeleteRunIndexRow DayTime 
	     GetRunPeriod checkRunPeriod DisconnectFromServer);
$VERSION = '$Revision: 2.38 $';

########

sub ConnectToServer {

    my ($hostname, $user, $password, $database, $verbose, $dbhref) =  @_;

# connect to MySQL database on localhost

    if ($verbose) {print "hostname=$hostname, user=$user\n";}

    $dbh = DBI->connect("DBI:mysql:$database:$hostname", $user, $password);

    if (defined $dbh) {
	if ($verbose) {print "Connection successful: handle: $dbh\n";}
    } else {
	die "Could not connect to the database server, exiting.\n";
    }

# return the connection handle

    $$dbhref = $dbh;

}

########

sub ReadConstants {

    my($dbh, $system, $subsystem, $item, $run, $runIndexTable, $date,
       $itemvalueref) = @_;
    @$itemvalueref = ();

# set date to infinite future if not already defined

    if (! $date) {$date = "2037-1-1";}

# construct the SQL query

    $itemValueTable = $system.'_'.$subsystem.'_'.$item;
    $sql  = "SELECT type, length, $itemValueTable.*";
    $sql .= " FROM System";
    $sql .= " LEFT JOIN Subsystem USING (systemId)";
    $sql .= " LEFT JOIN Item USING (subsystemId)";
    $sql .= " LEFT JOIN $runIndexTable USING (itemId)";
    $sql .= " LEFT JOIN $itemValueTable USING (itemValueId)";
    $sql .= " WHERE systemName = '$system'";
    $sql .= " AND subsystemName = '$subsystem'";
    $sql .= " AND itemName = '$item'";
    $sql .= " AND minRun <= $run AND maxRun >= $run";
    $sql .= " AND $runIndexTable.time <= '$date'";
    $sql .= " ORDER BY $runIndexTable.time DESC,";
    $sql .= " $runIndexTable.itemValueId DESC LIMIT 1";

# prepare and execute the query 

    &DO_IT($dbh);

# get the result and pass it to the caller, excluding type

    @result = $sth->fetchrow_array;
    $type = $result[0];
    $length = $result[1];
    if ($type !~ /blo/) {
	foreach $ic (2 .. $#result) {
	    $$itemvalueref[$ic - 2] = $result[$ic];
	}
    } else { # it's a blob!
	foreach $ic (2 .. 7) {
	    $$itemvalueref[$ic - 2] = $result[$ic];
	}
	$blob = $result[8];
	$blob =~ s/^\s+//; # strip off leading white space
	@value = split(/\s+/, $blob);
	foreach $ic (0 .. $length - 1) {
	    $$itemvalueref[$ic + 6] = $value[$ic];
	}
    }

    return 0;

}

########

sub ReadConstantsByIndex {

    my($dbh, $system, $subsystem, $item, $itemValueId,
       $itemvalueref) = @_;
    @$itemvalueref = ();

# construct the SQL query

    $itemValueTable = $system.'_'.$subsystem.'_'.$item;
    $sql  = "SELECT type, length, $itemValueTable.*";
    $sql .= " FROM System";
    $sql .= " LEFT JOIN Subsystem USING (systemId)";
    $sql .= " LEFT JOIN Item USING (subsystemId),";
    $sql .= " $itemValueTable";
    $sql .= " WHERE systemName = '$system'";
    $sql .= " AND subsystemName = '$subsystem'";
    $sql .= " AND itemName = '$item'";
    $sql .= " AND itemValueId = $itemValueId";

# prepare and execute the query 

    &DO_IT($dbh);

# get the result and pass it to the caller, excluding type

    @result = $sth->fetchrow_array;
    $type = $result[0];
    $length = $result[1];
    if ($type !~ /blo/) {
	foreach $ic (2 .. $#result) {
	    $$itemvalueref[$ic - 2] = $result[$ic];
	}
    } else { # it's a blob!
	foreach $ic (2 .. 7) {
	    $$itemvalueref[$ic - 2] = $result[$ic];
	}
	$blob = $result[8];
	$blob =~ s/^\s+//; # strip off leading white space
	@value = split(/\s+/, $blob);
	foreach $ic (0 .. $length - 1) {
	    $$itemvalueref[$ic + 6] = $value[$ic];
	}
    }

    return 0;

}

########

sub ShowSetsRun {

    my($dbh, $run, $runIndexTable, $date, $itemInfoRef) = @_;
    @$itemInfoRef = ();

# set date to infinite future if not already defined

    if (! $date) {$date = "2037-1-1";}

# construct the SQL query to get items

    $sql  = "SELECT systemName, subsystemName, itemName, itemId";
    $sql .= " FROM System LEFT JOIN Subsystem USING (systemId)";
    $sql .= " LEFT JOIN Item USING (subsystemId)";
    $sql .= " ORDER BY systemName, subsystemName, itemName";

# prepare and execute the query 

    &DO_IT($dbh);

    $nitem = 0;
    while ($this_itemref = $sth->fetchrow_hashref) {
	if ($$this_itemref{itemId}) { # check for non-null itemId
	    $itemref[$nitem++] = $this_itemref;
	}
    }

# loop over items, get info about the relevant constants

    for ($iitem = 0; $iitem < $nitem; $iitem++) {
	$this_itemref = $itemref[$iitem];
	$sql = "SELECT minRun, maxRun, itemValueId, officer,";
	$sql .= " DATE_FORMAT(time,'%Y-%m-%d %T'), comment";
	$sql .= " FROM $runIndexTable";
	$sql .= " WHERE $runIndexTable.itemId = $$this_itemref{itemId}";
	$sql .= " AND minRun <= $run";
	$sql .= " AND maxRun >= $run";
	$sql .= " AND time <= '$date'";
	$sql .= " ORDER BY time DESC LIMIT 1";
	&DO_IT($dbh);
	@columns = $sth->fetchrow_array;
	if (@columns) {
	    push @$itemInfoRef,
	    [ $$this_itemref{systemName}, $$this_itemref{subsystemName},
	      $$this_itemref{itemName}, @columns ];
	}
    }

    return 0;

}

########

sub ShowRunRanges {

    my($dbh, $system, $subsystem, $item, $runIndexTable, $date,
       $runMin, $runMax, $runRangeInfoRef) = @_;
    @$runRangeInfoRef = ();

# set date to infinite future if not already defined

    if (! $date) {$date = "2037-1-1";}

# construct the SQL query to get relevant run index entries

    $ncol = 6;
    $sql  = "SELECT minRun, maxRun, itemValueId, officer,";
    $sql .= " DATE_FORMAT(time,'%Y-%m-%d %T'), comment";
    $sql .= " FROM System LEFT JOIN Subsystem USING (systemId)";
    $sql .= " LEFT JOIN Item USING (subsystemId)";
    $sql .= " LEFT JOIN $runIndexTable USING (itemId)";
    $sql .= " WHERE systemName = '$system' AND subsystemName = '$subsystem'";
    $sql .= " AND itemName = '$item' AND time <= '$date'";
    $sql .= " AND !(minRun > $runMax or maxRun < $runMin)";

# prepare and execute the query 

    &DO_IT($dbh);

# Loop through fetched run index entries, make two arrays
#   1. entries themselves
#   2. array of possible run range beginnings
# Array 2 consists of all minRun's from the run index and all (maxRun + 1)'s
# since the end of a run range can also trigger the beginning of a new one.
# Store the run index entry or -1 for minRun and maxRun+1 respectively in
# array 2.
# Store the date of the run index entry in array 2.

    @runindex = ();
    @begin = ();
    $nindex = 0;
    while (@columns = $sth->fetchrow_array) {
	#print "$nindex @columns\n";
	# put the results in the runindex array
	push(@runindex, [ @columns ] );
	# add begin of range to possible begins, along with element number of
	# run index entry
	push(@begin,
	     { run => $columns[0], index => $nindex, date => $columns[4] });
	# add end of range + 1 to possible begins, with element number of run
	# index array set to -1
	push(@begin,
	     { run => $columns[1] + 1, index => -1, date => $columns[4] });
	$nindex++;
    }

# sort the begin run array, primarily by run number, secondarily by date

    @begin_sorted = sort byrun @begin;

# squeeze out duplicate begin runs, taking the first one in the array

    @begin_squeezed = ();
    $ibsq = 0;
    foreach $ib (0 .. $#begin_sorted) {
	#print "$ib $begin_sorted[$ib]{run} $begin_sorted[$ib]{index} $begin_sorted[$ib]{date}\n";
	if ($ib == 0 # copy the first one
	    || $begin_sorted[$ib]{run} != $begin_sorted[$ib-1]{run} # or it's a new run
	    ) {
	    $begin_squeezed[$ibsq] = $begin_sorted[$ib];
	    $ibsq++;
	    #print "copy it\n";
	}
    }

# Check each potential begin run for a later (in time) run range which
# overlaps it. If there is an overlap, discard it.

    $ibun = 0;
    @begin_uncovered = ();
    foreach $ibsq (0 .. $#begin_squeezed) {
	#print "$ibsq $begin_squeezed[$ibsq]{run} $begin_squeezed[$ibsq]{index} $begin_squeezed[$ibsq]{date}\n";
	$cover = Cover($begin_squeezed[$ibsq]);
	#print "cover = $cover\n";
	if (!$cover) {
	    $begin_uncovered[$ibun++] = $begin_squeezed[$ibsq];
	}
    }

# Generate final list of run ranges. For begin runs that came from
# maxRun+1's, look for the run range that applies. Take the run index
# entries and replace minRun and maxRun with begin run and end run and
# put that in the returned array. Make sure that the range is within the
# bounds specified by the caller.

    foreach $ibun (0 .. $#begin_uncovered - 1) { # don't generate a range for the last run
	$runMinEff = $begin_uncovered[$ibun]{run}; # effective run min.
	$runMaxEff = $begin_uncovered[$ibun + 1]{run} - 1; # effective run max.
	if (!($runMinEff > $runMax || $runMaxEff < $runMin)) {
	    if ($begin_uncovered[$ibun]{index} == -1) {
		$indexThis = IndexBelow($begin_uncovered[$ibun]);
	    } else {
		$indexThis = $begin_uncovered[$ibun]{index};
	    }
	    #print "$ibun $indexThis\n";
	    if ($indexThis != -1) {
		foreach $i (0 .. $ncol - 1) {
		    $info[$i] = $runindex[$indexThis][$i];
		}
		splice (@info, 0,  2, $runMinEff, $runMaxEff);
		push(@$runRangeInfoRef, [ @info ]);
	    }
	}
    }

    return 0;

}

########

sub byrun {
    # primary: ascending run order
    if ($$a{run} < $$b{run}) {
	return -1;
    } elsif ($$a{run} > $$b{run}) {
	return 1;
    } else {
	# secondary: order by descending date
	return $$b{date} cmp $$a{date};
    }
}

########

sub Cover {
    
    my($beginRef) = @_;

    $run = $$beginRef{run};
    $index = $$beginRef{index};
    $date = $$beginRef{date};

    $cover = 0;

    foreach $iri (0 .. $#runindex) {
	if ($iri != $index) {
	    $runMin = $runindex[$iri][0];
	    $runMax = $runindex[$iri][1];
	    if ($runMin <= $run && $runMax >= $run) {
		$dateIndex = $runindex[$iri][4];
		if ($dateIndex gt $date) {
		    #print "covered by iri = $iri\n";
		    $cover = 1;
		}
	    }
	}
    }
    return $cover;

}

########

sub IndexBelow {
    
    my($beginRef) = @_;

    $run = $$beginRef{run};
    $date = $$beginRef{date};

    #print "IndexBelow: looking for range under $run $date\n";

    $indexBelow = -1;
    $dateLatest = '';

    foreach $iri (0 .. $#runindex) {
	$runMin = $runindex[$iri][0];
	$runMax = $runindex[$iri][1];
	if ($runMin <= $run && $runMax >= $run) {
	    $dateIndex = $runindex[$iri][4];
	    if ($dateIndex gt $dateLatest) {
		#print "highest overlap so far: $runMin $runMax $dateIndex\n";
		$dateLatest = $dateIndex;
		$indexBelow = $iri;
	    }
	}
    }
    return $indexBelow;

}

########

sub ShowHistory {

    my($dbh, $system, $subsystem, $item, $run, $runIndexTable,
       $historyInfoRef) = @_;
    @$historyInfoRef = ();

# construct the SQL query to get items

    $sql  = "SELECT DATE_FORMAT(time,'%Y-%m-%d %T'), minRun, maxRun,";
    $sql .= " itemValueId, officer, comment, RunIndexId";
    $sql .= " FROM System LEFT JOIN Subsystem USING (systemId)";
    $sql .= " LEFT JOIN Item USING (subsystemId)";
    $sql .= " LEFT JOIN $runIndexTable USING (itemId)";
    $sql .= " WHERE systemName = '$system' AND subsystemName = '$subsystem'";
    $sql .= " AND itemName = '$item' AND minRun <= $run AND maxRun >= $run";
    $sql .= " ORDER BY time DESC";

# prepare and execute the query 

    &DO_IT($dbh);

    while (@columns = $sth->fetchrow_array) {
	push(@$historyInfoRef, [ @columns ]);
    }

}

########

sub WriteConstantSet {

    my($dbh, $system, $subsystem, $item, $sourceRunMin,
       $sourceRunMax, $comment, $itemValueIdRef, @constants) = @_;

# check the type
    $status = ShowItemInfo($dbh, $system, $subsystem, $item, \%itemInfo);

    if ($status != 0) {
	return 1;
    }

# construct the SQL query to insert constant set

    $itemValueTable = $system.'_'.$subsystem.'_'.$item;
    $columns = "minRunSource, maxRunSource, author, comment";
    $values = "$sourceRunMin, $sourceRunMax, '$ENV{USER}', '$comment'";
    $iv = 0;
    if ($itemInfo{type} =~ /blo/ ) { 
	print " blob: $itemInfo{type}\n";
        $columns .= ", v_0001";
        $values .= ",\'";
	$values .= "$constants[0]"; # add the first constant to the string
        for ($ic = 1; $ic <= $#constants; $ic++) {
	    $values .=" $constants[$ic]"; # add the other constants with a
                                          # leading blank
	}
        $values .= "\'";
    } elsif ($itemInfo{type} eq 'char') {
        $columns .= ", v_0001";
        $values .= ",\'";
        foreach $value (@constants) {$values .= "$value\\r";}
        $values .= "\'";
    } else {
	foreach $value (@constants) {
	    $iv++;
	    $iv_zpad = sprintf("%04d", $iv);
	    $columns .= ", v_$iv_zpad";
	    $values .= ", $value";
	}
    }
    $sql  = "INSERT $itemValueTable ( $columns ) VALUES ( $values )";

# prepare and execute the query 

    &DO_IT($dbh);

    $$itemValueIdRef = $sth->{mysql_insertid};

    return 0;
}

########

sub LinkConstantSet {

    my($dbh, $system, $subsystem, $item, $runIndexTable, $runMin, $runMax,
       $itemValueId, $comment, $runIndexIdRef) = @_;
#
# get the item id
#
    $status = ShowItemInfo($dbh, $system, $subsystem, $item, \%itemInfo);

    if ($status != 0) {
	return 1;
    }

    $itemId = $itemInfo{itemId};

# construct the SQL query to make the link

    $sql  = "INSERT $runIndexTable";
    $sql .= " ( minRun, maxRun, itemId, itemValueId, officer, comment )";
    $sql .= " VALUES ( $runMin, $runMax, $itemId, $itemValueId, '$ENV{USER}',";
    $sql .= " '$comment' )";

# prepare and execute the query

    &DO_IT($dbh);

    $$runIndexIdRef = $sth->{mysql_insertid};

    return 0;
}

########

sub WriteAndLinkConstantSet {
    my($dbh, $system, $subsystem, $item, $runIndexTable, $minRun, $maxRun,
       $commentRunIndex, $sourceRunMin, $sourceRunMax, $commentItemValue,
       $constantsRef, $runIndexIdRef) = @_;
    WriteConstantSet($dbh, $system, $subsystem, $item,
		     $sourceRunMin, $sourceRunMax,
		     $commentItemValue, \$itemValueId, @$constantsRef);
    LinkConstantSet($dbh, $system, $subsystem, $item, $runIndexTable,
		    $minRun, $maxRun, $itemValueId,
		    $commentRunIndex, \$runIndexId);
    $$runIndexIdRef = $runIndexId;
}

########

sub FormatTimestamp {
    my ($dbh, $timestamp) = @_;
    $sql = "SELECT DATE_FORMAT($timestamp, '%Y-%m-%d %T')";
    &DO_IT($dbh);
    $time_formatted = $sth->fetchrow;
    return $time_formatted;
}

########

sub ShowSystems {
    my($dbh, $subsystem, $item, $systemArrayRef) = @_;
    @$systemArrayRef = ();
    $tables = "System";
    $where_clause = "";
    if ($subsystem || $item) {
	$tables .= ", Subsystem";
	if ($subsystem) {$where_clause .= "subsystemName = '$subsystem' AND ";}
	$where_clause .= "Subsystem.systemId = System.systemId";
    }
    if ($item) {
	$tables .= ", Item";
	$where_clause .= " AND itemName = '$item'";
	$where_clause .= " AND Item.subsystemId = Subsystem.subsystemId";
    }
    $sql = "SELECT DISTINCT systemName FROM $tables";
    if ($where_clause) { $sql .= " WHERE $where_clause";}
    $sql .= " ORDER BY systemName";
    #print "$sql\n";
    &DO_IT($dbh);
    while ($this_system = $sth->fetchrow) {
	push(@$systemArrayRef, $this_system);
    }

}

########

sub ShowSubsystems {
    my($dbh, $system, $item, $subsystemArrayRef) = @_;
    @$subsystemArrayRef = ();
    $tables = "Subsystem";
    $where_clause = "";
    if ($system) {
	$tables .= ", System";
	$where_clause .= "systemName = '$system' AND ";
	$where_clause .= "Subsystem.systemId = System.systemId";
    }
    if ($item) {
	$tables .= ", Item";
	if ($where_clause) {$where_clause .= " AND ";}
	$where_clause .= " itemName = '$item'";
	$where_clause .= " AND Item.subsystemId = Subsystem.subsystemId";
    }
    $sql = "SELECT DISTINCT subsystemName FROM $tables";
    if ($where_clause) { $sql .= " WHERE $where_clause";}
    $sql .= " ORDER BY subsystemName";
    #print "$sql\n";
    &DO_IT($dbh);
    while ($this_subsystem = $sth->fetchrow) {
	push(@$subsystemArrayRef, $this_subsystem);
    }

}

########

sub ShowItems {
    my($dbh, $system, $subsystem, $itemArrayRef) = @_;
    @$itemArrayRef = ();
    $tables = "Item";
    $where_clause = "";
    if ($subsystem || $system) {
	$tables .= ", Subsystem";
	if ($subsystem) {$where_clause .= "subsystemName = '$subsystem' AND ";}
	$where_clause .= "Subsystem.subsystemId = Item.subsystemId";
    }
    if ($system) {
	$tables .= ", System";
	$where_clause .= " AND systemName = '$system'";
	$where_clause .= " AND System.systemId = Subsystem.systemId";
    }
    $sql = "SELECT DISTINCT itemName FROM $tables";
    if ($where_clause) { $sql .= " WHERE $where_clause";}
    $sql .= " ORDER BY itemName";
    #print "$sql\n";
    &DO_IT($dbh);
    while ($this_item = $sth->fetchrow) {
	push(@$itemArrayRef, $this_item);
    }

}

########

sub ShowSSI {
    my($dbh, $system, $subsystem, $item, $itemArrayRef) = @_;
    @$itemArrayRef = ();
    $tables = "Item,Subsystem,System";
    if ($system ) {
	$where_clause = "systemName = '$system' ";
    }
    if ($subsystem ) {
	if ($where_clause) {
	    $where_clause .= " AND subsystemName = '$subsystem' ";
	} else {
	    $where_clause = "subsystemName = '$subsystem' ";
	}
    }
    if ($item ) {
	if ($where_clause) {
	    $where_clause .= " AND itemName = '$item' ";
	} else {
	    $where_clause  = " itemName = '$item' ";
	}
    }
    if($where_clause){
	$where_clause .= " AND System.systemId = Subsystem.systemId ";
    } else {
        $where_clause  = " System.systemId = Subsystem.systemId ";
    }
    $where_clause .= " AND Subsystem.subsystemId = Item.subsystemId";
    $sql  = "SELECT systemName,subsystemName,itemName,itemId FROM $tables";
    $sql .= " WHERE $where_clause";
    $sql .= " ORDER BY systemName, subsystemName, itemName ";
    &DO_IT($dbh);
    while (@column = $sth->fetchrow_array) {
	push @$itemArrayRef, [@column];
    }
    
}

########

sub ShowItemInfo {
    my($dbh, $system, $subsystem, $item, $itemInfoRef) = @_;
    $column_names = "System.systemId, Subsystem.subsystemId, itemId, length,"
	. " type, Item.description";
    $tables = "Item, Subsystem, System";
    $where_clause = "itemName = '$item'"
	. " AND subsystemName = '$subsystem'"
	    . " AND Subsystem.subsystemId = Item.subsystemId"
		. " AND systemName = '$system'"
		    . " AND System.systemId = Subsystem.systemId";
    $sql = "SELECT $column_names FROM $tables WHERE $where_clause";
    #print "$sql\n";
    &DO_IT($dbh);
    @column = $sth->fetchrow_array;
    #print "number of columns = $#column\n";
    if ($#column == -1) {
	return 1;
    }
    $$itemInfoRef{systemId} = $column[0];
    $$itemInfoRef{subsystemId} = $column[1];
    $$itemInfoRef{itemId} = $column[2];
    $$itemInfoRef{length} = $column[3];
    $$itemInfoRef{type} = $column[4];
    $$itemInfoRef{description} = $column[5];

    return 0;

}

########

sub ShowConstantSetId {
    my($dbh, $itemId, $run, $runIndexTable, $date, $constantSetIdRef) = @_;

# set date to infinite future if not already defined

    if (! $date) {$date = "2037-1-1";}

# construct the SQL query

    $sql  = "SELECT itemValueId";
    $sql .= " FROM $runIndexTable";
    $sql .= " WHERE itemId = $itemId";
    $sql .= " AND minRun <= $run AND maxRun >= $run";
    $sql .= " AND time <= '$date'";
    $sql .= " ORDER BY time DESC LIMIT 1";

    #print "debug: query is \"$sql\"\n";

# prepare and execute the query 

    &DO_IT($dbh);

# get the result and pass it to the caller

    $$constantSetIdRef = $sth->fetchrow;

    return 0;

}

########

sub ShowSetsItem {
    my($dbh, $system, $subsystem, $item, $itemValueRef) = @_;
    $column_names = "itemValueId, author, DATE_FORMAT(time,'%Y-%m-%d %T'), minRunSource, maxRunSource, comment";
    $table = $system . '_' . $subsystem . '_' . $item;
    $sql = "SELECT $column_names FROM $table ORDER BY itemValueId";
    #print "$sql\n";
    &DO_IT($dbh);
    while (@column = $sth->fetchrow_array) {
	$$itemValueRef[$iiv]{itemValueId} = $column[0];
	$$itemValueRef[$iiv]{author} = $column[1];
	$$itemValueRef[$iiv]{time} = $column[2];
	$$itemValueRef[$iiv]{minRunSource} = $column[3];
	$$itemValueRef[$iiv]{maxRunSource} = $column[4];
	$$itemValueRef[$iiv]{comment} = $column[5];
	$iiv++;
    }
}

########

sub ShowChangesIndex {

    my($dbh, $system, $subsystem, $item, $runIndex, $officer, $time, $changeArrayRef) =  @_;
    $sql = "SELECT $runIndex.*", 
    $sql .= " FROM System";
    $sql .= " LEFT JOIN Subsystem USING (systemId)";
    $sql .= " LEFT JOIN Item USING (subsystemId)";
    $sql .= " LEFT JOIN $runIndex USING (itemId)";
    $sql .= " WHERE systemName = \'$system\' AND subsystemName = \'$subsystem\' AND itemName = \'$item\' AND time >= \'$time\' AND officer LIKE \'$officer\'";
    DO_IT($dbh);
    while ($hashref = $sth->fetchrow_hashref) {
	push (@$changeArrayRef, { %$hashref } );
    }

}

########

sub ShowChangesConstants {

    my($dbh, $system, $subsystem, $item, $author, $time, $changecArrayRef) =  @_;
    $constants_table = $system . '_' . $subsystem . '_' . $item;
    $sql = "SELECT *", 
    $sql .= " FROM $constants_table";
    $sql .= " WHERE time >= \'$time\' AND author = \'$author\'";
    DO_IT($dbh);
    while ($hashref = $sth->fetchrow_hashref) {
	push (@$changecArrayRef, { %$hashref } );
    }

}

########

sub DeleteRunIndexRow {
    my ($dbh, $runIndex, $id) = @_;
    $sql = "DELETE from $runIndex WHERE RunIndexId = $id";
    DO_IT($dbh);
    return;
}

########

sub DayTime {
    my ($timestamp, $dateref, $timeref) = @_;
    $year = substr($timestamp, 0, 4);
    $month = substr($timestamp, 4, 2);
    $day = substr($timestamp, 6, 2);
    $hour = substr($timestamp, 8, 2);
    $minute = substr($timestamp, 10, 2);
    $second = substr($timestamp, 12, 2);
    $$dateref = $year . '-' . $month . '-' . $day;
    $$timeref = $hour . ':' . $minute . ':' . $second;
}

########

sub DO_IT {    

    my($dbh) = @_;
    $sth = $dbh->prepare($sql)
	or die "Can't prepare $sql: $dbh->errstr\n";
    
    $rv = $sth->execute
	or die "Can't execute the query $sql\n error: $sth->errstr\n";
    
    return 0;

}

sub checkRunPeriod {
    my($dbhr,$minRun,$maxRun,$runP) = @_;
    $sql_cmd ="SELECT period from runinfo.run_period ";
    $sql_cmd.="where ((($minRun >= minRun) && ($maxRun <= maxRun)) ";
    $sql_cmd.=" && period != 'all') ";
    $sthref = $dbhr -> prepare($sql_cmd)
	or die "Can't prepare $sql_cmd: $dbh-errstr\n";
    $rv = $sthref -> execute
	or die "Can't execute the query: $sthref-> errstr\n";
    $nr=$sthref->rows;
    if($nr) { $$runP= $sthref->fetchrow_array;}
    else {$$runP= 0;}
}


sub GetRunPeriod {
    my($dbhr,$runP,$minRunP,$maxRunP) = @_;
    $sql_cmd ="SELECT minRun,maxRun from runinfo.run_period ";
    $sql_cmd.="where period=\'$runP\'  ORDER BY time DESC LIMIT 1";
    $sthref = $dbhr -> prepare($sql_cmd)
	or die "Can't prepare $sql_cmd: $dbh-errstr\n";
    $rv = $sthref -> execute
	or die "Can't execute the query: $sth-> errstr\n";
        @result = $sthref->fetchrow_array;
    if($#result gt 0) {
     $$minRunP = $result[0];
     $$maxRunP = $result[1];
    }
    if ($verbose) {print "GetRunPeriod $#result minRunP=$$minRunP maxRunP=$$maxRunP \n";} 
}
 
########
 
sub DisconnectFromServer {
    my($dbh) = @_;
    $rc = $dbh->disconnect;
}

########
