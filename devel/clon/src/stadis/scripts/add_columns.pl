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
$n_added = 0;
if ($nrows) {

#  table exists now make sure column does not already exist
    $sql_cmd = "SELECT * FROM $table_name";
    $sth = $dbh -> prepare($sql_cmd)
	or die "Can't prepare $sql_cmd: $dbh-errstr\n";

    $rv = $sth-> execute
	or die "Can't execute the query: $sth-> errstr\n";
    
    $existing_cols = $sth->{NAME};

    $sql_cmd = "ALTER  TABLE $table_name ";

    $i = 0;
    for $c (@vname) {
	$l_new = 1;
	for $n (@$existing_cols) {
	    if ($c =~ /^$n$/i) {
		print "add_columns  E:  Column name $c ALREADY exists bonehead!!\n";
		$l_new =0;
	    }
	}
	if ($l_new) {
	    $col_name = $c;
	    $col_type = $dvname[$i];
	    $col_ok = &CHECK_COLUMN($col_type, $col_name);
	    $sql_cmd .= "ADD $col_name $col_ok,";
	    $n_added++;
	}
	$i++;
    } 
    $sql_cmd =~ s/,$//;
    print "$sql_cmd \n";
    if ($n_added) {
	$sth = $dbh -> prepare($sql_cmd)
	    or die "Can't prepare $sql_cmd: $dbh-errstr\n";
	
	$rv = $sth-> execute
	    or die "Can't execute the query: $sth-> errstr\n";
    }

} else {
    print "add_columns E: table $name does not exist use make_tables.pl to make the table \n";
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
    print "type name :$type: :$name: \n";
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
