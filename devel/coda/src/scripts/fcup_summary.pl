#!/usr/bin/perl -w

#  fcup_summary.pl

#  dumps run_log_end fcup data
#
# do not started automatically by any scripts, can be run manually
# to get fcap info
#
#  E.Wolin, 12-jun-03



use DBI;
use Time::Local;
use Date::Manip qw(ParseDate UnixDate);


#  Perl code:
#  ----------

# init
$database = "clasprod";
$start="1-jan-2003";
$target="H2_full";


# decode command line args
$line=join(" ",@ARGV);
if($line =~ s/-h//i) {
    die "\n usage:\n\n   fcup_summary [-s start_date] [-e end_date] [-t target]\n\n".
	"Some legal targets are H2_full, 4He_full, 3He_full, D2_full, NH3, C12\n".
	    "See the run sheet for a complete list\nUse * to match any target\n\n";}
($start    	      =  $1) if ($line =~ s/-s\s+(\S+)//i);
($target              =  $1) if ($line =~ s/-t\s+(\S+)//i);
($end    	      =  $1) if ($line =~ s/-e\s+(\S+)//i);


$sql_cmd="select e.run,end_date,fcup_active,fcup_live,target from run_log_end e, run_log_comment c".
    " where e.run=c.run and e.session_name=c.session_name and end_date>=\'$start\'";
if(($target ne "\%")&&($target ne "*")) {$sql_cmd.=" and target=\'$target\'";}
if(defined $end) {$sql_cmd.= " and end_date<=\'$end\'";}
$sql_cmd.=" order by end_date";


($dbh=DBI->connect("DBI:Ingres:db5\::$database","clasrun",undef)) 
    || die "Failed to connect to Ingres database\n";
($sth=$dbh->prepare($sql_cmd)) 
    || die "Can't prepare $sql_cmd: $dbh->errstr\n";
($sth->execute) 
    || die "Can't execute the query: $sth->errstr\n";


$fcup_active_sum=0;
$fcup_live_sum=0;
while(@row=$sth->fetchrow_array) {

    ($run,$end_date,$fcup_active,$fcup_live,$target)=@row;    
    $pdate=ParseDate($end_date);
    ($y,$mon,$d,$h,$m)=UnixDate($pdate,"%Y","%m","%d","%H","%M");
    $utime=timelocal(0,$m,$h,$d,$mon-1,$y);

    $fcup_active/=10000;
    $fcup_live/=10000;
    $fcup_active_sum+=$fcup_active;
    $fcup_live_sum+=$fcup_live;

    printf("%8d    %s %s  %8d %8d     %8d %8d       %s\n",
	   $run,$end_date,$utime,$fcup_active,$fcup_active_sum,$fcup_live,$fcup_live_sum,$target);

}


$sth->finish;
$dbh->disconnect;


#  done
exit;

