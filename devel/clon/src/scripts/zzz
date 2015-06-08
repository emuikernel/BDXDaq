#!/usr/bin/perl
#
use DBI; # define the database calls
#
# connect to the database
#
$database = "dc_chan_status"; 
$user = "clasuser"; 
$password =""; 
$hostname = "clasdb.jlab.org"; 
$dbh = DBI->connect("DBI:mysql:database=$database;host=$hostname",  
                    $user, $password);
#
# ask the questions, get the answers
#
print "which sector ? ";
$sector = <STDIN>;
print "which region ? ";
$region = <STDIN>;
print "which fuse number ? ";
$fusenum = <STDIN>;
#
# define the string, get_columns, to be those things you wish to
# print out
#
$get_columns = "wire2locwire.layer,wire2locwire.wire";
#
# define the string, included_tables, to be the necessary list of
# tables to support the query
#
$included_tables = "tdcmuxmap,stbadbmap,locwire2stb,layer2loclayer,wire2locwire,sec2adbcrate,stbpin2adbpin,adb2mux,muxpin2tdcpin,adbpin2connhalf,adbpin2muxpin,adbcrate_location,tdccrate_location,fusebox";
#
# define the string, table_connect, to be the necessary join of the tables
# required to define the entire signal wire to tdc mapping
#
$table_connect ="sec2adbcrate.adbcrateloc=tdcmuxmap.adbcrateloc and layer2loclayer.layer= wire2locwire.layer and wire2locwire.locwire=locwire2stb.locwire and layer2loclayer.loclayer=locwire2stb.loclayer and wire2locwire.stbboardloc=stbadbmap.stbboardloc and locwire2stb.stbconnector=stbadbmap.stbconnector and stbadbmap.suplayer=locwire2stb.suplayer and locwire2stb.suplayer=layer2loclayer.suplayer and layer2loclayer.suplayer=stbpin2adbpin.suplayer and stbpin2adbpin.suplayer=sec2adbcrate.suplayer and stbpin2adbpin.stbpin=locwire2stb.stbpin and stbpin2adbpin.adbpin=adbpin2connhalf.adbpin and stbpin2adbpin.adbpin=adbpin2muxpin.adbpin and adbpin2muxpin.muxpin=muxpin2tdcpin.muxpin and adbpin2connhalf.adbconnhalf=adb2mux.adbconnhalf and adb2mux.adbconnector=stbadbmap.adbconnector and adb2mux.pinmatch=adbpin2muxpin.pinmatch and adb2mux.muxconnector=tdcmuxmap.muxconnector and tdcmuxmap.adbslot=stbadbmap.adbslot and adbcrate_location.adbcrateloc=tdcmuxmap.adbcrateloc and tdccrate_location.tdccrateloc=tdcmuxmap.tdccrateloc and fusebox.fusenum=wire2locwire.stbboardloc and fusebox.region=layer2loclayer.region and fusebox.sector=sec2adbcrate.sector";
#
# define the string, specific_request, to be the specific specification of
# sector, layer and wire
#
$specific_request = "fusebox.sector=$sector and fusebox.region=$region and fusebox.fusenum=$fusenum";
#
#
# write the query
#
$sql_cmd = "select $get_columns from $included_tables";
$sql_cmd .= " where $specific_request and $table_connect ;";
#
# prepare the command
#
$sth = $dbh -> prepare($sql_cmd) 
    or die "Can't prepare sql:$sql_cmd\nerror:$dbh->errstr\n"; 
#
# execute the command
#
$rv = $sth-> execute 
    or die "Can't execute the query sql:$sql_cmd\nerror: $sth-> errstr\n";
#
# fetch and display
#
while ($row = $sth->fetchrow_hashref) {
print "layer = $$row{layer} \n wire = $$row{wire} \n";
}
#
exit;

