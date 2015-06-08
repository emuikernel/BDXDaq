h46638
s 00000/00003/00251
d D 1.13 06/12/04 10:08:36 wolin 14 13
c Typo
c 
e
s 00014/00005/00240
d D 1.12 06/12/04 10:06:02 wolin 13 12
c New database
c 
e
s 00001/00000/00244
d D 1.11 06/11/20 11:24:28 boiarino 12 11
c package clonjava;
c 
e
s 00001/00001/00243
d D 1.10 06/07/09 13:30:22 boiarino 11 10
c add clonjava.
e
s 00001/00001/00243
d D 1.9 02/01/30 16:26:45 wolin 10 9
c New cariboulake
e
s 00005/00005/00239
d D 1.8 01/11/09 10:13:41 wolin 9 8
c Minor
e
s 00005/00006/00239
d D 1.7 99/05/13 10:31:43 wolin 8 7
c Removed cc_lo
c 
e
s 00010/00006/00235
d D 1.6 99/05/13 10:25:45 wolin 7 6
c Added start_date
c 
e
s 00009/00007/00232
d D 1.5 99/04/22 13:55:25 wolin 6 5
c Added run time
c 
e
s 00008/00005/00231
d D 1.4 99/04/22 13:44:52 wolin 5 4
c Added target
c 
e
s 00038/00025/00198
d D 1.3 99/03/19 15:32:32 wolin 4 3
c Added livetimes, fcup_active, other minor mods
c 
e
s 00043/00019/00180
d D 1.2 99/03/19 14:40:58 wolin 3 1
c Added begin and comment info
c 
e
s 00000/00000/00000
d R 1.2 99/03/19 13:08:56 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 dbutil/s/run_log_check.java
e
s 00199/00000/00000
d D 1.1 99/03/19 13:08:55 wolin 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
//  run_log_check.java

// dumps run log info for operators to check on shift

I 12
package clonjava;
E 12

import java.lang.*;
I 4
import java.text.DecimalFormat;
E 4
import java.sql.*;
D 11
import clon.util.*;
E 11
I 11
import clonjava.clon.util.*;
E 11


//-----------------------------------------------------------------------------
//  begin class definition
//-----------------------------------------------------------------------------


public class run_log_check {

I 13
    private static String database       = "clasprod";
E 13
  
D 10
  private static String driver         = "COM.cariboulake.sql.ingres.JSQLDriver";
E 10
I 10
D 13
  private static String driver         = "com.cariboulake.jsql.JSQLDriver";
E 10
  private static String url            = "jdbc:caribou:jsqlingres://db5:6024/";
  private static String database       = "clasprod";
  private static String account        = "foo";
  private static String password       = null;
E 13
I 13
    private static String driver         = "com.mysql.jdbc.Driver";
    private static String url            = "jdbc:mysql://clondb1:3306/";
    private static String password       = "";
    //    private static String password       = "e1tocome";
    private static String account        = "clasrun";

    //   private static String driver         = "com.cariboulake.jsql.JSQLDriver";
    //   private static String url            = "jdbc:caribou:jsqlingres://db5:6024/";
    //   private static String account        = "foo";
    //   private static String password       = null;
E 13
  private static int run_start;
D 3
  private static int run_end;
E 3
I 3
  private static int run_end           = 0;
E 3


//-----------------------------------------------------------------------------


  static public void main (String[] args) {
    
    java.sql.Connection con = null;
D 3
    int run,nevent,nfile,nlong;
E 3
I 3
D 4
    int run,nevent,nfile,nlong,ec_hi,ec_lo,cc_hi,cc_lo;
E 4
I 4
D 6
    int run,nevent,nfile,ec_hi,ec_lo,cc_hi,cc_lo;
E 6
I 6
D 8
    int deltat,run,nevent,nfile,ec_hi,ec_lo,cc_hi,cc_lo;
E 8
I 8
    int deltat,run,nevent,nfile,ec_hi,ec_lo,cc_hi;
E 8
E 6
I 5
D 7
    String target;
E 7
I 7
    String start_date,target;
E 7
E 5
E 4
    int beam,torus,mini;
    float current;
D 4
    String l1;
E 4
I 4
    int fcup_active,fcup_live,clock_active,clock_live;
E 4
E 3
    int last_run;
I 4
    float livef,livec;
D 7
    DecimalFormat fmt = new DecimalFormat("#0.00");
E 7
I 7
    DecimalFormat fmt  = new DecimalFormat("#0.00");
E 7
E 4


    // decode command line
    decode_command_line(args);


    // load jdbc driver
    try {
      Class.forName(driver);
    }
    catch(Exception e) {
      System.err.println("Unable to load JDBC driver " + driver);
      System.err.println(e.toString());
      System.exit(-1);
    }
    

    // connect to database
    try  {
      con = DriverManager.getConnection(url+database,account,password);
    }
    catch (Exception e) {
      System.err.println("Unable connect to database " + url + database);
      System.err.println(e.toString());
      System.exit(-1);
    }
    

    // create and execute query
    try {

      // create statement object
      Statement s = con.createStatement();


      // create sql string
D 3
      String sql = "select run,nfile,nevent,nlong" + 
	" from run_log_end" + 
	" where session_name='clasprod' " + 
	//	" and nevent>0 and nlong>0" +
	" and run>=" + run_start;
      if(run_end>0)sql+=" and run<=" + run_end;
      
E 3
I 3
D 4
      String sql = "select b.run,e.nfile,e.nevent,e.nlong,b.beam_energy,c.beam_current_request," +
	"b.torus_current," +
	"b.mini_current,b.ec_total_hi,b.ec_total_lo,b.cc_hi,b.cc_lo,b.l1_program" +
E 4
I 4
D 5
      String sql = "select b.run,nfile,nevent,beam_energy,beam_current_request," +
E 5
I 5
D 6
      String sql = "select b.run,nfile,nevent,target,beam_energy,beam_current_request," +
E 5
	"torus_current," +
	"mini_current,ec_total_hi,ec_total_lo,cc_hi,cc_lo," +
	"fcup_active,fcup_live,clock_active,clock_live" +
E 6
I 6
D 7
      String sql = "select interval('mins',end_date-start_date)," +
E 7
I 7
      String sql = "select interval('mins',end_date-start_date)," + 
	" start_date," +
E 7
	" b.run,nfile,nevent,target,beam_energy,beam_current_request," +
D 8
	" torus_current,mini_current,ec_total_hi,ec_total_lo,cc_hi,cc_lo," +
E 8
I 8
	" torus_current,mini_current,ec_total_hi,ec_total_lo,cc_hi," +
E 8
	" fcup_active,fcup_live,clock_active,clock_live" +
E 6
E 4
	" from run_log_begin b, run_log_comment c, run_log_end e" +
	" where" +
	" b.session_name='clasprod' and c.session_name='clasprod' and e.session_name='clasprod'" + 
	" and b.run=e.run and b.run=c.run" + 
	" and b.run>=" + run_start;
      if(run_end>0)sql +=" and b.run<=" + run_end;
      sql += " order by b.run";

E 3

I 13
D 14
      System.out.println(sql);


E 14
E 13
      // query and get result
      ResultSet result = s.executeQuery(sql);


      // print run info
      last_run=run_start-1;
D 3
      System.out.println("Run     " + "Nfile" + "      Nevent" + "       Nlong");
      System.out.println("---     " + "-----" + "      ------" + "       -----\n");
E 3
I 3
D 4
      System.out.println("    Run   " + "Nfile" + "      Nevent" + "       Nlong  " +
			 "Beam     I  Torus   Mini  ec_hi  ec_lo  cc_hi  cc_lo  L1");
      System.out.println("    ---   " + "-----" + "      ------" + "       -----  " +
			 "----     _  -----   ----  -----  -----  -----  -----  --\n");
E 4
I 4
D 5
      System.out.println("    Run   " + "File" + "    Event  LiveF  LiveC  " + 
E 5
I 5
D 6
      System.out.println("   Run   " + "File" + "    Event    Target  LiveF  LiveC  " + 
E 6
I 6
D 7
      System.out.println("   Run   " + "File" + "    Event  Time    Target  LiveF  LiveC  " + 
E 7
I 7
D 9
      System.out.println("   Run        Start      " + "File" + "    Event  Time    Target  LiveF  LiveC  " + 
E 7
E 6
E 5
D 8
			 "Beam     I  Torus   Mini  ec_hi  ec_lo  cc_hi  cc_lo   FcupA");
E 8
I 8
			 "Beam     I  Torus   Mini  ec_hi  ec_lo  cc_hi   FcupA");
E 8
D 5
      System.out.println("    ---   " + "----" + "    -----  -----  -----  " + 
E 5
I 5
D 6
      System.out.println("   ---   " + "----" + "    -----    ------  -----  -----  " + 
E 6
I 6
D 7
      System.out.println("   ---   " + "----" + "    -----  ----    ------  -----  -----  " + 
E 7
I 7
      System.out.println("   ---        -----      " + "----" + "    -----  ----    ------  -----  -----  " + 
E 7
E 6
E 5
D 8
			 "----     _  -----   ----  -----  -----  -----  -----   -----\n");
E 8
I 8
			 "----     _  -----   ----  -----  -----  -----   -----\n");
E 9
I 9
      System.out.println("    Run       Start      " + "File" + "    Event  Time    Target  LiveF  LiveC  " + 
			 "Beam     I  Torus   Mini  ec_hi  ec_lo  cc_hi      FcupA");
      System.out.println("    ---       -----      " + "----" + "    -----  ----    ------  -----  -----  " + 
			 "----     _  -----   ----  -----  -----  -----      -----\n");
E 9
E 8
E 4
E 3
      while (result.next()) {

	// unpack result
I 6
	deltat=result.getInt(1);
E 6
D 4
	run=result.getInt(1);
	nfile=result.getInt(2);
	nevent=result.getInt(3);
	nlong=result.getInt(4);
I 3
	beam=result.getInt(5);
	current=result.getFloat(6);
	torus=result.getInt(7);
	mini=result.getInt(8);
	ec_hi=result.getInt(9);
	ec_lo=result.getInt(10);
	cc_hi=result.getInt(11);
	cc_lo=result.getInt(12);
	l1=result.getString(13);
E 4
I 4
	run=result.getInt("run");
I 7
	start_date=result.getString("start_date");
E 7
	nfile=result.getInt("nfile");
	nevent=result.getInt("nevent");
I 5
	target=result.getString("target");
E 5
	fcup_active=result.getInt("fcup_active");
	fcup_live=result.getInt("fcup_live");
	clock_active=result.getInt("clock_active");
	clock_live=result.getInt("clock_live");
	beam=result.getInt("beam_energy");
	current=result.getFloat("beam_current_request");
	torus=result.getInt("torus_current");
	mini=result.getInt("mini_current");
	ec_hi=result.getInt("ec_total_hi");
	ec_lo=result.getInt("ec_total_lo");
	cc_hi=result.getInt("cc_hi");
D 8
	cc_lo=result.getInt("cc_lo");
E 8


	// get livetimes
	livef=(float)((fcup_active>0)?((double)fcup_live)/fcup_active:0.);
	livec=(float)((clock_active>0)?((double)clock_live)/clock_active:0.);
E 4
E 3


	// check for gaps
	if(run>(last_run+1)) System.out.println("*** missing run " + (last_run+1) 
						+ " to run " + (run-1) + " ***");
	

	// print run info
D 3
	System.out.print(Format.justify(run,8) + Format.justify(nfile,-5)
			   + Format.justify(nevent,-12) + Format.justify(nlong,-12));

E 3
I 3
	if (run==last_run) {
D 5
	  System.out.print("*** ");
E 5
I 5
	  System.out.print("***");
E 5
	} else {
D 5
	  System.out.print("    ");
E 5
I 5
	  System.out.print("   ");
E 5
	}
	last_run=run;
E 3

D 3
	// check for duplicates
	if (run==last_run) System.out.print("     (duplicate entry)");
E 3
I 3
D 4
	System.out.print(Format.justify(run,6) + Format.justify(nfile,-5));
        System.out.print(Format.justify(nevent,-12) + Format.justify(nlong,-12));
E 4
I 4
D 7
	System.out.print(Format.justify(run,6) + Format.justify(nfile,-4));
E 7
I 7
	System.out.print(Format.justify(run,6));
	System.out.print(Format.justify(start_date.substring(2,16),-15));
	System.out.print(Format.justify(nfile,-5));
E 7
        System.out.print(Format.justify(nevent,-9));
I 6
        System.out.print(Format.justify(deltat,-6));
E 6
I 5
        System.out.print(Format.justify(target,-10));
E 5
        System.out.print(Format.justify(fmt.format(livef),-7));
        System.out.print(Format.justify(fmt.format(livec),-7));
E 4
	System.out.print(Format.justify(beam,-6) + Format.justify(current,-6));
	System.out.print(Format.justify(torus,-7) + Format.justify(mini,-7));
	System.out.print(Format.justify(ec_hi,-7) + Format.justify(ec_lo,-7));
D 8
	System.out.print(Format.justify(cc_hi,-7) + Format.justify(cc_lo,-7));
E 8
I 8
	System.out.print(Format.justify(cc_hi,-7));
E 8
D 4
	// System.out.print("  " +l1);
E 4
I 4
D 9
	System.out.print(Format.justify(fcup_active,-8));
E 9
I 9
	System.out.print(Format.justify(fcup_active,-11));
E 9
E 4
E 3
	System.out.println();

D 3

	last_run=run;
E 3
      }
      

      // done...no more rows
      result.close();
      s.close();
D 3
      System.out.println("\n\n");
E 3
I 3
      System.out.println();
E 3


    }
    catch (SQLException e) {
      System.out.println(e.getMessage());
    }
    catch (Exception e) {
      e.printStackTrace();
      System.exit(-1);
    }


    System.exit(-1);

  }  // main done


//-----------------------------------------------------------------------------
  

  static public void decode_command_line(String[] args) {
    
    int i;
    String help = "\nUsage:\n\n" + 
      "   java run_log_check [-driver driver] [-url url] [-db database]\n" + 
      "                      [-account account] [-pwd password] run_start [run_end]\n\n";
    
    
    // must have at least 1 arg
    if(args.length<1) {
      System.out.println(help);
      System.exit(-1);
    }


    // loop over all args
    for(i=0; i<args.length; i++) {
      
      if(!args[i].startsWith("-")) {
	break;

      } else if(args[i].equalsIgnoreCase("-h")) {
	System.out.println(help);
	System.exit(-1);
	
      } else if(args[i].equalsIgnoreCase("-driver")) {
	driver=args[i+1];
	i++;

      } else if(args[i].equalsIgnoreCase("-url")) {
	url=args[i+1];
	i++;

      } else if(args[i].equalsIgnoreCase("-db")) {
	database=args[i+1];
	i++;

      } else if(args[i].equalsIgnoreCase("-account")) {
	account=args[i+1];
	i++;

      } else if(args[i].equalsIgnoreCase("-pwd")) {
	password=args[i+1];
	i++;
      }
      
    }


    // get start,end run numbers
    run_start=Integer.parseInt(args[i]);
    if((i+1)<args.length)run_end=Integer.parseInt(args[args.length-1]);
    
    return;
  }
  
}
//-----------------------------------------------------------------------------
//  end class definition
//-----------------------------------------------------------------------------
E 1
