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
