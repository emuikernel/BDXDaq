h13902
s 00068/00000/00000
d D 1.1 10/06/08 12:16:36 boiarino 1 0
c date and time created 10/06/08 12:16:36 by boiarino
e
u
U
f e 0
t
T
I 1
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
print "which layer ? ";
$layer = <STDIN>;
print "which wire ? ";
$wire = <STDIN>;
#
# define the string, get_columns, to be those things you wish to
# print out
#
$get_columns = "sec2adbcrate.adbcratename,tdcmuxmap.adbcrateloc,tdcmuxmap.adbslot,adb2mux.adbconnector,adbpin2muxpin.adbpin,adb2mux.muxconnector,muxpin2tdcpin.muxpin,adb2mux.siglength,tdcmuxmap.tdccrateloc,tdcmuxmap.tdcslot,tdcmuxmap.tdcconnector,muxpin2tdcpin.tdcpin,sigcablename,adbcrate_location.spaceframe_location,tdccrate_location.spaceframe_location";
#
# define the string, included_tables, to be the necessary list of
# tables to support the query
#
$included_tables = "tdcmuxmap,stbadbmap,locwire2stb,layer2loclayer,wire2locwire,sec2adbcrate,stbpin2adbpin,adb2mux,muxpin2tdcpin,adbpin2connhalf,adbpin2muxpin,adbcrate_location,tdccrate_location";
#
# define the string, table_connect, to be the necessary join of the tables
# required to define the entire signal wire to tdc mapping
#
$table_connect ="sec2adbcrate.adbcrateloc=tdcmuxmap.adbcrateloc and layer2loclayer.layer= wire2locwire.layer and wire2locwire.locwire=locwire2stb.locwire and layer2loclayer.loclayer=locwire2stb.loclayer and wire2locwire.stbboardloc=stbadbmap.stbboardloc and locwire2stb.stbconnector=stbadbmap.stbconnector and stbadbmap.suplayer=locwire2stb.suplayer and locwire2stb.suplayer=layer2loclayer.suplayer and layer2loclayer.suplayer=stbpin2adbpin.suplayer and stbpin2adbpin.suplayer=sec2adbcrate.suplayer and stbpin2adbpin.stbpin=locwire2stb.stbpin and stbpin2adbpin.adbpin=adbpin2connhalf.adbpin and stbpin2adbpin.adbpin=adbpin2muxpin.adbpin and adbpin2muxpin.muxpin=muxpin2tdcpin.muxpin and adbpin2connhalf.adbconnhalf=adb2mux.adbconnhalf and adb2mux.adbconnector=stbadbmap.adbconnector and adb2mux.pinmatch=adbpin2muxpin.pinmatch and adb2mux.muxconnector=tdcmuxmap.muxconnector and tdcmuxmap.adbslot=stbadbmap.adbslot and adbcrate_location.adbcrateloc=tdcmuxmap.adbcrateloc and tdccrate_location.tdccrateloc=tdcmuxmap.tdccrateloc";
#
# define the string, specific_request, to be the specific specification of
# sector, layer and wire
#
$specific_request = "sector=$sector and layer2loclayer.layer=$layer and wire=$wire;";
#
#
# write the query
#
$sql_cmd = "select $get_columns from $included_tables";
$sql_cmd .= " where $table_connect and $specific_request";
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
print "--------------------------------------------------------------------\n";
print "signal cable name = $$row{sigcablename} \nadb crate name = $$row{adbcratename}, located on $$row{spaceframe_location} \nadb crate number = $$row{adbcrateloc} \nadb slot = $$row{adbslot} \nadb connector = $$row{adbconnector} \nadb pin = $$row{adbpin} \nmux connector = $$row{muxconnector} \nmux pin = $$row{muxpin} \nsignal length = $$row{siglength} \ntdc crate number = $$row{tdccrateloc}, located on $$row{spaceframe_location} \ntdc slot = $$row{tdcslot} \ntdc connector = $$row{tdcconnector} \ntdc pin = $$row{tdcpin} \n";
print "--------------------------------------------------------------------\n";
}
#
exit;

E 1
