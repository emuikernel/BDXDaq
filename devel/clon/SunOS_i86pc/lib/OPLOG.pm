#!/usr/bin/env perl
#
# used for example by logbook_mail_resolve
#

use DBI;

package OPLOG;
$OPLOG::Version="0.02";

sub new {
    my $class = shift;
    my $self = {};
    my %params = @_;
    my @names = qw(entry_date run entry_type 
		   system_type subject lost_time operators email_address
		   comment attr_shift_summary attr_data_quality 
		   attr_repair attr_action_needed attr_acknowledged
		   attr_resolved attr_ignore action_email_address unique_key
		   forward_key backward_key thread_id database prev_id
		   user);


    foreach $n (@names) {
	$self->{$n}  = $params{$n};
    }

# connect to the DB

    $database = $self->{database};
#    $user = $self->{user};
#    if (not defined $user) {
#	$user = `whoami`;
#    }
    $user = "clasrun" ;
    $password = "e1tocome";
#    $hostname = "db5";
#    $hostname = "clasweb-bck";
    $hostname = "clonweb";
	

#    $self->{dbh} = DBI->connect("DBI:Ingres:$hostname\::$database",$user, undef);
    $self->{dbh}  = DBI->connect("DBI:mysql:database=$database;host=$hostname",  
		    $user, $password);
	
    if (not defined $self->{'dbh'}) {
	die "Failed to connect to mysql database\n";
    }
	
#    $self->{dbh}->do('set lockmode session where readlock=nolock')
#	or  die "Cannot set readlock=nolock: \n $dbh->errstr\n";
    
# check for keys that are not correct...need to figure out how to
# call subroutine before class is "blessed" as this is a duplicate
# of the CHECK_LIST subroutine

    foreach $p (keys %params) {
	$n_index = 0;
	$lok = 0;
	while ($lok==0 && $n_index <= $#names) {
	    if ("$names[$n_index]" eq "$p") {
		$lok =1;
	    }
	    $n_index++;
	}
	if ($lok == 0) {
	    print "OPLOG W: class does not support \"$p\" variable which you try to set to \"$params{$p}\"\n";
	}
    }
    
    return bless $self, $class;
}

sub CHECK_LIST {
    my $self = shift;
    my $params = shift;
    my @names = qw(entry_date run entry_type 
		   system_type subject lost_time operators email_address
		   comment attr_shift_summary attr_data_quality 
		   attr_repair attr_action_needed attr_acknowledged
		   attr_resolved attr_ignore action_email_address unique_key
		   forward_key backward_key thread_id prev_id);

    foreach $p (keys %$params) {
	$n_index = 0;
	$lok = 0;
	while ($lok==0 && $n_index <= $#names) {
	    if ("$names[$n_index]" eq "$p") {
		$lok =1;
	    }
	    $n_index++;
	}
	if ($lok == 0) {
	    print "OPLOG W: class does not support \"$p\" variable which you try to set to \"$$params{$p}\"\n\n";
	}
    }
}
sub OPLOG_SET {
    my $self = shift;
    my %params = @_;

    foreach $key (keys %params) {
	$self->{$key} = $params{$key};
    }

    $self->CHECK_LIST(\%params);

    return(1);
}

sub OPLOG_SHOW {
    my $self = shift;
    my @params = @_;
    my %return_hash = {};

    foreach $key (@params) {
	$return_hash{$key} = $self->{$key};
    }

    return(\%return_hash);
}

sub GET_DATE_TIME {
    my $self = shift;
    my @month_name = qw(Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec);
    my @time_array = localtime(time);
    my $year = 1900 + $time_array[5];

    $month = $month_name[$time_array[4]];

    for ($i = 0; $i <5; $i++) {
	if ($time_array[$i] < 10) {
	    $time_array[$i] = '0'.$time_array[$i];
	}
    }
    $day = $time_array[3];
    $month = $time_array[4]+1;

#    my $date_time = "$day-$month-$year $time_array[2]:$time_array[1]:$time_array[0]";
    my $date_time = "$year-$month-$day $time_array[2]:$time_array[1]:$time_array[0]";
    
    return($date_time);
}

sub GET_RUN_NUMBER {
    my $self = shift;
    return(`$ENV{CLON_BIN}/run_number`);
}

sub GET_ENTRY {
    my $self = shift;
    my $id = shift;
    my $thread = shift;
    my @columns = qw(entry_date run entry_type 
		   system_type subject lost_time operators email_address
		   comment attr_shift_summary attr_data_quality 
		   attr_repair attr_action_needed attr_acknowledged
		   attr_resolved attr_ignore action_email_address unique_key
		   forward_key backward_key thread_id);

    $sql_names = join(',',@columns);
    $sql_cmd = "SELECT $sql_names FROM operator_log WHERE unique_key=$id";

#    die "$sql_cmd\n";

    $sth = $dbh -> prepare($sql_cmd)
	or die "Can't prepare $sql_cmd: $dbh-errstr\n";
    
    $rv = $sth-> execute
	or die "Can't execute the query: $sth-> errstr\n";
    
    @entry = $sth->fetchrow_array;
    $sth->finish;

    $i = 0;
    foreach $n (@columns) {
	$self->{$n} = $entry[$i];
	$i++;
    }
	
    return(@entry);
}

sub GET_ID_BY_RUN {
    my $self = shift;
    my $runnum = shift;
    my $dbh = $self->{dbh};

    $sql_cmd = "SELECT unique_key FROM operator_log WHERE run=$runnum";

#    die "$sql_cmd\n";

    $sth = $dbh -> prepare($sql_cmd)
	or die "Can't prepare $sql_cmd: $dbh-errstr\n";
    
    $rv = $sth-> execute
	or die "Can't execute the query: $sth-> errstr\n";
    
    undef @ids;
    while (($id_found) = $sth->fetchrow_array) {
	push(@ids, $id_found);
    }
    $sth->finish;

    return(@ids);
}

sub GET_UNIQUE_SEQ {
    my $self = shift;
    my $key = $self->{unique_key};
    return $key;
}

sub OPLOG_MAKE_ENTRY {
    my $self = shift;

    my $dbh = $self->{dbh};

    my $lpass = 0;
    $all_set = $self->CHECK_REQUIRED();

    if ($all_set) {
	$sql_cmd = "SELECT sequence_num FROM oplog_seq";
	
	$sth = $self->{dbh} -> prepare($sql_cmd)
	    or die "Can't prepare $sql_cmd: $dbh->errstr\n";
	
	$rv = $sth-> execute
	    or die "Can't execute the query: $sth-> errstr\n";
	
	($seq) = $sth->fetchrow_array;
	$sth->finish;

	$new_seq = $seq + 1;
	
	$sql_cmd = "UPDATE oplog_seq SET sequence_num=$new_seq";
	
	$sth = $dbh -> prepare($sql_cmd)
	    or die "Can't prepare $sql_cmd: $dbh-errstr\n";

	$rv = $sth-> execute
	    or die "Can't execute the query: $sth-> errstr\n";
	
	$sth -> finish;
# now that you have the unique ID of THIS entry check if the entry is to be threaded
# and if so update the "key" columns of all appropriate entries

	$prev_id = $self->{prev_id};
	if ($prev_id != 0) {
	    $thread_ok = $self->THREAD_ENTRY($new_seq);
	}

	my @names = qw(entry_date run entry_type 
		       system_type subject lost_time operators email_address
		       comment attr_shift_summary attr_data_quality 
		       attr_repair attr_action_needed attr_acknowledged
		       attr_resolved attr_ignore action_email_address unique_key
		       forward_key backward_key thread_id);

	%quotes = (entry_date           => '\'',
		   run                  => '',
		   entry_type           => '\'',
		   system_type          => '\'',
		   subject              => '\'',
		   lost_time            => '\'',
		   operators            => '\'',
		   email_address        => '\'',
		   comment              => '\'',
		   attr_shift_summar    => '\'',
		   attr_data_quality    => '\'',
		   attr_repair          => '\'',
		   attr_action_needed   => '\'',
		   attr_acknowledged    => '\'',
		   attr_resolved        => '\'',
		   attr_ignore          => '\'',
		   action_email_address => '\'',
		   unique_key           => '',
		   forward_key          => '',
		   backward_key         => '',
		   thread_id            => '');
	
	$self->{unique_key} = $new_seq;
	
	for $n (@names) {
	    if ($self->{$n}) {
		$columns .= "$n,";
		$values  .= "$quotes{$n}$self->{$n}$quotes{$n},";
	    }
	}
	
	$columns =~ s/,\s*$//;
	$values =~ s/,\s*$//;
	
#	print "Columns are $columns \n";
#	print "Values are $values \n" ;
	
	$sql_cmd = "INSERT INTO operator_log ($columns) VALUES ($values)";
	
	$sth = $dbh -> prepare($sql_cmd)
	    or die "Can't prepare $sql_cmd: $dbh-errstr\n";
	
	$rv = $sth-> execute
	    or die "Can't execute the query: $sth-> errstr\n";
	
	$sth->finish;
	$lpass = $new_seq;
#	print "MADE new entry  $new_seq $self->{unique_key} $lpass!!!\n";
    } else {
	print "OPLOG W: No entry made, all not all fields were set\n";
    }
    return($lpass);
}

sub OPLOG_DISCONNECT {
    $self = shift;
    $dbh = $self->{dbh};
    $dbh->disconnect;

    return(1);
}
sub GET_LAST_ID {
    my $self = shift;
    $dbh = $self->{dbh};
    $sql_cmd = "SELECT sequence_num FROM oplog_seq";

#    die "$sql_cmd \n";

    $sth = $dbh -> prepare($sql_cmd)
	or die "Can't prepare $sql_cmd: $dbh-errstr\n";
	
    $rv = $sth-> execute
	or die "Can't execute the query: $sth-> errstr\n";
	
    ($seq) = $sth->fetchrow_array;
    $sth->finish;
    return($seq);
}

sub CHECK_REQUIRED {
    my $self = shift;

    @required = qw(entry_date run entry_type system_type subject comment operators);

    $lok = 1;
    foreach $r (@required) {
	if (not defined $self->{$r}) {
	    print "OPLOG W: required entry \"$r\" not defined\n";
	    $lok = 0;
	}
    }
    return($lok);
}

sub THREAD_ENTRY {
    my $self = shift;
    my $this_id = shift;
    my $dbh = $self->{dbh};
    my $prev_id = $self->{prev_id};

    my $ok = 0;

# is previous entry threaded?

    undef($thread_id);

    $sql_cmd = "SELECT thread_id FROM operator_log WHERE unique_key=$prev_id";
	
    $sth = $dbh -> prepare($sql_cmd)
	or die "Can't prepare $sql_cmd: $dbh->errstr\n";
	
    $rv = $sth-> execute
	or die "Can't execute the query: $sth-> errstr\n";
	
    ($thread_id) = $sth->fetchrow_array;
    $sth->finish;

    if (not defined $thread_id) {  # entry not threaded
	$new_thread_id = $prev_id;

#	print "threaded entry starting new thread thread_id:$new_thread_id!!\n";

	$sql_cmd = "UPDATE operator_log SET thread_id=$new_thread_id, forward_key=$this_id WHERE unique_key=$prev_id";

#	print "$sql_cmd\n";

	$sth = $dbh -> prepare($sql_cmd)
	    or die "Can't prepare $sql_cmd: $dbh->errstr\n";
	$rv = $sth-> execute
	    or die "Can't execute the query: $sth-> errstr\n";

	$sth->finish;
	$self->{thread_id} = $new_thread_id;
	$self->{backward_key} = $prev_id;
	$ok = 1;
    } else {                               # entry threaded---find last entry on thread

#	print "making a entry to a preexisting thread-thread_id: $thread_id\n";
	$sql_cmd = "SELECT unique_key FROM operator_log WHERE forward_key is null AND thread_id=$thread_id";
#	print "$sql_cmd \n";

	$sth = $dbh -> prepare($sql_cmd)
	    or die "Can't prepare $sql_cmd: $dbh->errstr\n";
#	print "sth $sth  dbh $dbh \n";
	$rv = $sth-> execute
	    or die "Can't execute the query: $sth-> errstr\n";
	($last_id) = $sth->fetchrow_array;
	$sth->finish;

	if (not defined $last_id) {
	    $sql_cmd = "SELECT unique_key, forward_key FROM operator_log WHERE thread_id=$thread_id order by unique_key desc";
#	    print "$sql_cmd \n";
	    
	    $sth = $dbh -> prepare($sql_cmd)
		or die "Can't prepare $sql_cmd: $dbh->errstr\n";
#	    print "sth $sth  dbh $dbh \n";
	    $rv = $sth-> execute
		or die "Can't execute the query: $sth-> errstr\n";
	    ($last_id, $bad_forward) = $sth->fetchrow_array;
	    $sth->finish;
	    
	    print "WARNING last threaded entry as forward_key set!!!$last_id\n";
	    print "\n\n Fixing the problem by setting forward key = $this_id on $last_id forward_key was $bad_forward\n";

	}


	if (defined $last_id) {

#	    print "thread_id: $thread_id  last_id in thread:$last_id\n";

	    $sql_cmd = "UPDATE operator_log SET forward_key=$this_id WHERE unique_key=$last_id";
#	    print "$sql_cmd\n";

	    $sth = $dbh -> prepare($sql_cmd)
		or die "Can't prepare $sql_cmd: $dbh->errstr\n";
#	    print "sth $sth  dbh $dbh \n";
	    $rv = $sth-> execute
		or die "Can't execute the query: $sth->errstr\n";
	    $sth->finish;
	    
#	    print "done dealing with previous threaded entries\n";
	    $self->{thread_id} = $thread_id;
	    $self->{backward_key} = $last_id;

	    $ok = 1;
	} 
    }
    return($ok);
}
1;


