#!/usr/bin/perl -w

#  oplog_checker.pl - run from clasmail account (the only one from that account)

#  checks oplog for action_needed but not acknowledged/resolved entries and sends out email

#  still to do
#        store open thread?

#  E.Wolin, 7-apr-03


use DBI;
use Env;
use XML::Parser;
use Time::Local;


#  Perl code:
#  ----------


#  init
$mail_file           = "$CLON_PARMS/logbook/oplog_mail_list.cfg";
$database            = "clasprod";
$max_day             = 180;
$unack_only          = 0;
$debug               = 0;
$verbose             = 0;
$summary_addr        = "";
$rc_addr             = "";
$pdl_addr            = "";
$summary             = "";
$now                 = time;
$banner              = "\n".
" To acknowledge: reply to email with \"acknowledge\" on first non-blank line.\n".
" To resolve:     reply with \"resolve\" on first non-blank line; text of\n".
"                 message will get entered as a new entry in the logbook.\n\n".
"                                                E.Wolin, 9-apr-2001\n\n\n\n";


# decode command line args
$line=join(" ",@ARGV);
if($line =~ s/-h//i) {
    die "\n usage:\n   oplog_checker [-unack_only] [-day max_day] [-mail mail_file] [-verbose] [-debug]\n\n";}
($mail_file    	      =  $1) if ($line =~ s/-mail\s+(\S+)//i);
($max_day    	      =  $1) if ($line =~ s/-day\s+(\S+)//i);
($unack_only   	      =   1) if ($line =~ s/-unack_only\s*//i);
($verbose             =   1) if ($line =~ s/-verbose\s*//i);
($debug        	      =   1) if ($line =~ s/-debug\s*//i);


#  read mail list and store email addresses by system and for rc,pdl
$parser = new XML::Parser(ErrorContext => 2);
$parser->setHandlers(
		     Start   => \&start_handler,
		     );
$parser->parsefile($mail_file);


#  connect to database
($dbh=DBI->connect("DBI:mysql:$database:clondb1","clasrun","")) 
    || die "Failed to connect to MySQL database\n";


#  search for unacknowledged/unresolved entries
$sql="select unique_key,entry_date,ifnull(entry_type,'none'),".
    "ifnull(system_type,'undefined'),ifnull(subject,'none'),".
    "ifnull(operators,'none'),ifnull(comment,'(no comment)'),".
    "ifnull(forward_key,0),ifnull(attr_acknowledged,'N')".
    " from operator_log where entry_date>date_sub(now(),interval $max_day day)".
    " and ifnull(attr_ignore,'N')!='Y' and ifnull(attr_action_needed,'N')='Y'".
    " and ifnull(attr_resolved,'N')!='Y'";
($sth=$dbh->prepare($sql)) || die "Can't prepare $sql: $dbh->errstr\n";
($sth->execute) || die "Can't execute the query: $sth->errstr\n";


while(@row=$sth->fetchrow_array) {
    $unique_key   = $row[0];
    $entry_date   = $row[1];
    $entry_type   = $row[2];
    $system_type  = $row[3];
    $subject      = $row[4];
    $operators    = $row[5];
    $comment      = $row[6];
    $forward_key  = $row[7];
    $acknowledged = $row[8];
    if($verbose>0) {print "checking action_needed ukey: $unique_key, ack: $acknowledged\n";}


#  search thread 
    $ack=($acknowledged eq 'Y')?1:0;
    $res=0;
    if($forward_key>0) {
	$sql="select ifnull(attr_acknowledged,'N'),ifnull(attr_resolved,'N'),".
	    "ifnull(forward_key,0),ifnull(attr_ignore,'N')".
	    " from operator_log where unique_key=?";
	($sth2=$dbh->prepare($sql)) || die "Can't prepare $sql: $dbh->errstr\n";
	$fkey=$forward_key;
	while($fkey>0) {
	    ($sth2->execute($fkey)) || die "Can't execute the query: $sth2->errstr\n";
	    @row2=$sth2->fetchrow_array;
	    if($verbose>0) {print "...thread ukey: $fkey, ack: $row2[0], res: $row2[1]\n";}
	    $fkey = $row2[2];
	    next if($row2[3] eq 'Y');
	    $ack+=($row2[0] eq "Y")?1:0;
	    if($row2[1] eq "Y") {$res=1; last;}
	}
	$sth2->finish;
    }


#  result of search along thread
    next if($res>0);                       # thread is resolved
    next if(($ack>0)&&($unack_only>0));    # ack but not resolved and unack_only set


#  send email
    open(TMP,">/tmp/oplog_checker.tmp");
    print TMP $banner.
	      "Date:           $entry_date\n".
	      "Key:            $unique_key\n".
	      "Operators:      $operators\n".
	      "System:         $system_type\n".
	      "Entry type:     $entry_type\n".
	      "Subject:        $subject\n".
	      "Comment:\n\n$comment";
    close(TMP);
    if($ack>0) {
	$sub="Unresolved $system_type problem (key $unique_key) in logbook";
    } else {
	$sub="Unacknowledged $system_type problem (key $unique_key) in logbook";
    }
    $cmd="cat /tmp/oplog_checker.tmp | mailx -r clasmail\@clon10.jlab.org".
	" -s \"$sub\" $addr{$system_type}";
    $stat=($ack>0)?"Unresolved":"Unacknowledged";
    $sum="date:      $entry_date\n".  
	 "system:    $system_type\n".
	 "status:    $stat\n".
	 "subject:   $subject\n".
	 "key:       $unique_key\n".
	 "notified:  $addr{$system_type}\n".
         "ref:       <http://clasweb.jlab.org/clasonline/servlet/newloginfo?action=logentry&entryId=$unique_key>\n\n";

    $summary.="$sum\n";
    print "$sum\n";
    if($debug==0) {
	system($cmd);
    }
}
$sth->finish;


#  send summary email
if(length($summary)>0) {
    open(TMP,">$CLON_PARMS/logbook/oplog_err.txt");
    print TMP "$summary";
    close(TMP);
    $cmd="cat $CLON_PARMS/logbook/oplog_err.txt | mailx -r clasmail\@clon10.jlab.org ".
	"-s \"Operator log action needed summary\" $summary_addr $pdl_addr $rc_addr";
    if($debug==0) {
	system($cmd);
    } else {
	print "$cmd\n";
    }
}


#  done
$dbh->disconnect;
exit;


#-------------------------------------------------------------------------


#  start tag handler
sub start_handler {
    my ($parser,$tag) = @_;
    %atts = @_;


    if($tag eq "system") {
	$addr{$atts{"name"}}=$atts{"addr"};

    } elsif($tag eq "summary") {
	$summary_addr=$atts{"addr"};

    } elsif($tag eq "rc") {
	($d,$m,$y)=split("-",$atts{"beg"}); $beg=timelocal( 0, 0, 0, $d,$m-1,$y);
	($d,$m,$y)=split("-",$atts{"end"}); $end=timelocal(59,59,23, $d,$m-1,$y);
	if(($now>=$beg)&&($now<=$end)) {$rc_addr=$atts{"email"};}

    } elsif($tag eq "pdl") {
	($d,$m,$y)=split("-",$atts{"beg"}); $beg=timelocal( 0, 0, 0, $d,$m-1,$y);
	($d,$m,$y)=split("-",$atts{"end"}); $end=timelocal(59,59,23, $d,$m-1,$y);
	if(($now>=$beg)&&($now<=$end)) {$pdl_addr=$atts{"email"};}
    }
}


#-------------------------------------------------------------------------

