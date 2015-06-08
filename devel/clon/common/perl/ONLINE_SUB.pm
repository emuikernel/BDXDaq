#!/usr/bin/env perl

package ONLINE_SUB;

use Time::CTime;
use CGI;

require Exporter;
@ISA = qw(Exporter);
@EXPORT=  qw(GET_HANDLE MAX_RUN SELECTION_FORM LATEST_VALUE_TABLE 
	     GET_LATEST MAKE_TABLE_AND_PLOT 
	     LATEST_VALUE_GROUP GET_LATEST_GROUP MAKE_TABLE_AND_PLOT_GROUP);

$ONLINE_SUB::Version="0.01";

sub GET_HANDLE {
    $database = "clas_online";
    $user = "clasrun";
    $password = "";
    $hostname = "clondb1";

    $dbh = DBI->connect("DBI:mysql:$database:$hostname", $user, $password);
    return($dbh);
}

# Find the maximum run number stored in the latest value table

sub MAX_RUN {
    local($dbh) = @_;
    $sql_cmd  = "SELECT max(run) FROM latest_value ";
	
    $sth = $dbh -> prepare($sql_cmd)
	or die "Can't prepare $sql_cmd: $dbh-errstr\n";
    $rv = $sth-> execute
	or die "Can't execute the query: $sth-> errstr\n";
    
    ($max_run) = $sth-> fetchrow_array;
    $sth->finish;

    return($max_run);
    
}

# this prints out the generic selection form

sub SELECTION_FORM {
    local($cgi) = @_;
    $submit_string[0] = 'Select Data based on Time/Date';
    $submit_string[1] = 'Select Data based on Run Number';
    $submit_string[2] = 'Return Latest Value Table only';
    $submit_string[3] = 'Replot';

    $now = time();
    $two_hrs_ago = $now - 86400;

    ($this_min, $this_hour, $this_day, $this_month, $this_year) = 
	split(/:/,strftime("%M:%H:%d:%b:%Y",localtime($now)));
    
    ($hrs_ago_min, $hrs_ago_hour, $hrs_ago_day, $hrs_ago_month, 
     $hrs_ago_year) = 
	 split(/:/,strftime("%M:%H:%d:%b:%Y",localtime($two_hrs_ago)));

    $max_run = &MAX_RUN($dbh);
    $max_run_10 = $max_run-10;

    print $cgi->p("<TABLE NOSAVE> <TR> <TH align=left> &nbsp </TH>");
    print $cgi->p("<th align=center> Start FROM (>)</th> 
                   <th align=center> End ON (<=) </th>
                   <th align=center> Click ONE button </th>");
    print $cgi->p("</TR> <TR>");
    print $cgi->p("<th align=left> Time based selection</Th><TD>",
		  textfield(-name    => 'start_time',
			    -default => "$hrs_ago_hour:$hrs_ago_min",
			    -size    => 5,
			    -maxlength =>5),
		  textfield(-name    => 'start_date',
			    -default => "$hrs_ago_day-$hrs_ago_month-$hrs_ago_year",
			    -size    => 11,
			    -maxlength =>11),
		  "</td><td>");
    print $cgi->p(textfield(-name    => 'end_time',
			    -default => "",
			    -size    => 5,
			    -maxlength =>5),
		  textfield(-name    => 'end_date',
			    -default => "",
			    -size    => 11,
			    -maxlength =>11),
		  "</td><TD>",
		  submit(-name  =>'data_select',
			 -value => $submit_string[0]),
		  "</td></tr><tr>");
    print $cgi->p("<th align=left>Select based on Run Number</TH><td align=center>",
		  textfield(-name    => 'run_low',
			    -default => "$max_run_10",
			    -size    => 15,
			    -maxlength =>20),
		  "</td><td align=center>",
		  textfield(-name    => 'run_high',
			    -default => "",
			    -size    => 15,
			    -maxlength =>20),
		  "</td><TD ROWSPAN=1>",
		  submit(-name  =>'data_select',
			 -value => $submit_string[1]),
		  "</td></tr>");
    print $cgi->p("<tr><th colspan=3 align=left> 
                   Return the latest value table </TH><td>",
		  submit(-name  =>'data_select',
			 -value => $submit_string[2]),
		  "</td></tr>");
    print $cgi->p("<tr><th> Manually set the y scale </th><td> Y lower", 
		  textfield(-name     => 'y_low',
			    -default  => '',
			    -size     => 5,
			    -maxlength=> 20),
		  "</td><td> Y upper limit",
		  textfield(-name     => 'y_high',
			    -default  => '',
			    -size     => 5,
			    -maxlength=> 20),
		  "</td> <td>",
		  submit(-name  =>'data_select',
			 -value => $submit_string[3]),
		  "</td>");
    print $cgi->p("</tr></TABLE>");

    print $cgi->p(reset("RESET FORM"));

}

# dumps out the latest value table

sub LATEST_VALUE_TABLE {
    ($cgi, $dbh, $n_selected, *selected_var, *selected_grp) =@_;

    print $cgi->p("<table border COLS=7>");
    print $cgi->p("<TR><TH> Name</th> <TH> Group</th>");
    print $cgi->p("<TH> Run Number</th> <TH> Event Number</th>");
    print $cgi->p("<TH> Time of last update</th> <TH> value</th>");
    print $cgi->p("<TH> Error</th> ");
    print $cgi->p("</tr>");

    for ($i=0; $i<$n_selected; $i++) {

	my @row = GET_LATEST($dbh, $selected_var[$i], $selected_grp[$i]);

	print $cgi->p("<TR>");
	print $cgi->p("<td>$row[0] </td><td> $row[1]</td><td> $row[2]</td><td> $row[3]</td><td> $row[4]</td><td> $row[5]</td><td> $row[6] </td>");
	print $cgi->p("</TR>");
    }
    print $cgi->p("</table>");
}


sub MAKE_TABLE_AND_PLOT {
    local($dbh,$cgi, $n_selected, *selected_var, *selected_grp, $cgi_string) = @_;
    $xsize = 600;
    $ysize = 250;
    $cgi_string .= "xsize=$xsize&ysize=$ysize";

    for ($i=0; $i<$n_selected; $i++) {
	$start_cgi_string  = 
	    "var_names=$selected_var[$i]&grp_names=$selected_grp[$i]&";

#	print $cgi->p("$start_cgi_string$cgi_string<BR>");

	my @row = GET_LATEST($dbh, $selected_var[$i], $selected_grp[$i]);

	print $cgi->p("<table cols=5 width=80\%>");
	print $cgi->p("<TR> <td> Name </td> <th align=left> $selected_var[$i] </th>");
	print $cgi->p("<td rowspan=6 colspan=3>");
	print $cgi->p("<img border=0 
src=\"../ONLINE_TIMELINE/online_gif.pl?$start_cgi_string$cgi_string \" alt=\"$selected_var[$i]\">");
	print $cgi->p("</td> </tr>");
	print $cgi->p("<TR> <td> Group </td> <th align=left> $selected_grp[$i] </th> </tr>");
	print $cgi->p("<TR> <td> Latest Value </td> <th align=left> $row[5] +/- $row[6] </th> </tr>");
	print $cgi->p("<TR> <td> Time of last update </td> <th align=left> $row[4] </th> </tr>");
	print $cgi->p("<TR> <td> Run <BR> Event</td> <th align=left> $row[2] <br> $row[3] </th> </tr>");
	print $cgi->p("<TR> <td> Alarm Status</td> <th align=left> no alarm</th> </tr>");
	print $cgi->p("</table>");
	print $cgi->hr;
    }
}

sub GET_LATEST {
    local($dbh, $var_name, $grp_name) = @_;

    $sql_cmd = "SELECT var_name, var_group, run, event, 
                  DATE_FORMAT(tou,\'%T %d-%b-%Y %W\'), 
                  value, error FROM latest_value 
                  WHERE var_name=\'$var_name\' and
                       var_group=\'$grp_name\'";
    
    $sth = $dbh -> prepare($sql_cmd)
	or die "Can't prepare $sql_cmd: $dbh-errstr\n";
    
    $rv = $sth-> execute
	or die "Can't execute the query: $sth-> errstr\n";
    
    $nrows = $sth->rows;
    

    @row = $sth-> fetchrow_array;

    $sth->finish;
    return(@row);
}

sub LATEST_VALUE_GROUP {
    ($cgi, $dbh, $n_selected, $selected_var, $selected_grp) =@_;

    print $cgi->p("<table border COLS=7>");
    print $cgi->p("<TR><TH> Name</th> <TH> Group</th>");
    print $cgi->p("<TH> Run Number</th>");
    print $cgi->p("<TH> Time of last update</th> <TH> value</th>");
    print $cgi->p("</tr>");

    for ($i=0; $i<$n_selected; $i++) {

	my @row = GET_LATEST_GROUP($dbh, $$selected_var[$i], $$selected_grp[$i]);

	print $cgi->p("<TR>");
	print $cgi->p("<td>$$selected_var[$i] </td><td> $$selected_grp[$i] </td><td> $row[0]</td><td> $row[1]</td><td> $row[2]</td>");
	print $cgi->p("</TR>");
    }
    print $cgi->p("</table>");
}


sub MAKE_TABLE_AND_PLOT_GROUP {
    local($dbh,$cgi, $n_selected, $selected_var, $selected_grp, $cgi_string) = @_;
    $xsize = 600;
    $ysize = 250;
    $cgi_string .= "xsize=$xsize&ysize=$ysize";

    for ($i=0; $i<$n_selected; $i++) {
	$start_cgi_string  = 
	    "var_names=$$selected_var[$i]&grp_names=$$selected_grp[$i]&";

#	print $cgi->p("$start_cgi_string$cgi_string<BR>");

	my @row = GET_LATEST_GROUP($dbh, $$selected_var[$i], $$selected_grp[$i]);

	print $cgi->p("<table cols=5 width=80\%>");
	print $cgi->p("<TR> <td> Name </td> <th align=left> $$selected_var[$i] </th>");
	print $cgi->p("<td rowspan=6 colspan=3>");
	print $cgi->p("<img border=0 
src=\"../ONLINE_TIMELINE/ntuple_gif.pl?$start_cgi_string$cgi_string \" alt=\"$$selected_var[$i]\">");
	print $cgi->p("</td> </tr>");
	print $cgi->p("<TR> <td> Group </td> <th align=left> $$selected_grp[$i] </th> </tr>");
	print $cgi->p("<TR> <td> Latest Value </td> <th align=left> $row[2]  </th> </tr>");
	print $cgi->p("<TR> <td> Time of last update </td> <th align=left> $row[1] </th> </tr>");
	print $cgi->p("<TR> <td> Run </td> <th align=left> $row[0] </th> </tr>");
	print $cgi->p("<TR> <td> Alarm Status</td> <th align=left> no alarm</th> </tr>");
	print $cgi->p("</table>");
	print $cgi->hr;
    }
}

sub GET_LATEST_GROUP {
    local($dbh, $var_name, $grp_name) = @_;

    if ($grp_name eq 'run_log_begin') {
	$time_col = 'start_date'; 
	$event = '';
    } elsif ($grp_name eq 'run_log_end') {
	$time_col = 'end_date';
	$event = '';
    } else {
	$time_col = 'tou';
	$event = 'event,';
    }
    $sql_cmd = "SELECT run,
                  DATE_FORMAT($time_col,\'%T %d-%b-%Y %W\'), $var_name
                  FROM $grp_name ORDER BY $time_col DESC LIMIT 1 ";
    
    $sth = $dbh -> prepare($sql_cmd)
	or die "Can't prepare $sql_cmd: $dbh-errstr\n";
    
    $rv = $sth-> execute
	or die "Can't execute the query: $sth-> errstr\n";
    
    $nrows = $sth->rows;
    

    @row = $sth-> fetchrow_array;

    $sth->finish;
    return(@row);
}
1;
