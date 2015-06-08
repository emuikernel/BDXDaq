h09866
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 stadis/scripts/update_table.pl
e
s 00036/00000/00000
d D 1.1 00/07/31 15:24:19 gurjyan 1 0
c date and time created 00/07/31 15:24:19 by gurjyan
e
u
U
f e 0
t
T
I 1
#!/apps/perl/bin/perl

use DBI;


$group = $ARGV[0];
$run   = $ARGV[1];
$event = $ARGV[2];

$table_name = $group;

$database = "clas_online";
$user = "clasrun";
$password = "";
$hostname = "claspc10.cebaf.gov";

$dbh = DBI->connect("DBI:mysql:$database:$hostname", $user, $password);

$sql_cmd = "INSERT INTO $group  VALUES (NULL, $run, $event, ";

for ($i = 3; $i <= $#ARGV; $i++) {
    $sql_cmd .= "$ARGV[$i], ";
}

$sql_cmd =~ s/,\s*$//;
$sql_cmd .= ") ";

#print "$sql_cmd \n";
$sth = $dbh -> prepare($sql_cmd)
    or die "Can't prepare $sql_cmd: $dbh-errstr\n";
    
$rv = $sth-> execute
    or die "Can't execute the query: $sth-> errstr\n";


print "Variables in the Table: $group have been UPDATED\n";
E 1
