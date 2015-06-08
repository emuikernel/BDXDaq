h18137
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 stadis/scripts/make_table.pl
e
s 00147/00000/00000
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


$name = $ARGV[0];
$table_name = $name;

# construct the variables names from command line argument
# add_columns.pl table_name -v var_1_name var_1_atributes -v var_2_name var_2_attributes -v var_3_name....

$nv = 0;
$i = 0;
$dvname = '';
while  ($i < $#ARGV) {
    $i++;
    if ($ARGV[$i] =~ /^-v$/) {
	$i++;
	$vname[$nv] = $ARGV[$i];
	$i++;
	$def = 0;
	while ($def == 0 && $i <= $#ARGV ) {
	    if ($ARGV[$i] =~ /^-v$/) {
		$def = 1;
		$i--;
	    } else {
		$dvname[$nv] .= "$ARGV[$i] ";
		$i++;
	    }
	}
	$nv++;
    }
}

$database = "clas_online";
$user = "clasrun";
$password = "";
$hostname = "claspc10.cebaf.gov";

$dbh = DBI->connect("DBI:mysql:$database:$hostname", $user, $password);

$sql_cmd = "SHOW TABLES FROM $database LIKE \'$table_name\'";

$sth = $dbh -> prepare($sql_cmd)
    or die "Can't prepare $sql_cmd: $dbh-errstr\n";

$rv = $sth-> execute
    or die "Can't execute the query: $sth-> errstr\n";

$nrows = $sth->rows;

if ($nrows == 0) { # table doesn't exist so create it 

    $sql_cmd = "CREATE TABLE $table_name (tou TIMESTAMP PRIMARY KEY, run INT UNSIGNED NOT NULL, event INT UNSIGNED NOT NULL, ";

    $i = 0;
    for $cn (@vname) {
	$col_name = $cn;
	$col_type = $dvname[$i];
	$col_ok = &CHECK_COLUMN($col_type, $col_name);
	$sql_cmd .= " $col_name $col_ok,";
	$i++;
    }

    $sql_cmd .= " KEY run (run))";
    print "$sql_cmd \n";
    $sth = $dbh -> prepare($sql_cmd)
	or die "Can't prepare $sql_cmd: $dbh-errstr\n";

    $rv = $sth-> execute
	or die "Can't execute the query: $sth-> errstr\n";
}

for $n (@vname) {
    $sql_cmd = "SELECT var_name FROM latest_value WHERE var_name=\'$n\' and var_group=\'$table_name\'";
    $sth = $dbh -> prepare($sql_cmd)
	or die "Can't prepare $sql_cmd: $dbh-errstr\n";
    
    $rv = $sth-> execute
	or die "Can't execute the query: $sth-> errstr\n";
    
    $nrows = $sth->rows;
    
    if ($nrows == 0) {
	$sql_cmd = "INSERT INTO latest_value (var_name, var_group) VALUES (\'$n\',\'$table_name\')";
	
	$sth = $dbh -> prepare($sql_cmd)
	    or die "Can't prepare $sql_cmd: $dbh-errstr\n";
	
	$rv = $sth-> execute
	    or die "Can't execute the query: $sth-> errstr\n";

    }
}
print "Variable name: $name added under group: $group \n";

sub CHECK_COLUMN {
    local($type,$name) = @_;
    @mysql_col = qw(tinyint smallint mediumint int integer
		    bigint real double float decimal numeric);
    @mysql_qualifiers = qw(NOT  NULL UNSIGNED AUTOINCREMENT);

    @t_split = split(/ /,$type);
    if (!($t_split[0] )) {
	$t_split[0] = $type;
    }
    local $l_ok = 0;
    for $t (@mysql_col) {
	if ($t_split[0] =~ /^$t$/i) {
	    $l_ok = 1;
	    $type = $t;
	}
    }
    if (!($l_ok)) {
	print "make_tables E: column type $t_split[0] for column $name ";
	print "does not match known MYSQL column types \n";
	print "make_tables E: setting column type to default FLOAT \n"; 
	$type = 'FLOAT';
    } 

    if ($l_ok) {
	$l_ok_q = 0;
	$l_all = 1;
	$bad_q = '';

	for ($ii=1; $ii <= $#t_split; $ii++) {
	    $ts = $t_split[$ii];
	    for $q (@mysql_qualifiers) {
		if ($ts =~ /^$q$/i) {
		    $l_ok_q = 1;
		    $type .= " $q";
		}
	    }
	    if (!($l_ok_q)) {
		$bad_q .= "$ts ";
		$l_all = 0;
	    }
	}
	if (!($l_all)) {
	    print "make_tables E: column qualifier $bad_q for column $name ";
	    print "does not match known MYSQL column qualifiers \n";
	    print "make_tables E: setting column qualifier to :$type:  \n"; 
	}
    }

    return($type)
}
E 1
