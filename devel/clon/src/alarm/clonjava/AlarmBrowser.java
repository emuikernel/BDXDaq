//
// java AlarmBrowser
//          -a clasprod
//          -u alarm_browser
//          -driver com.cariboulake.jsql.JSQLDriver
//

package clonjava;

import java.lang.*;


class AlarmBrowser
{


  static public void main(String args[]) {

    boolean beep = true;

    String driver         = "com.mysql.jdbc.Driver";
    String url            = "jdbc:mysql://clondb1:3306/";
    String database       = "clasprod";
    String account        = "clasrun";
    String password       = "";

    String project   = "clastest";
    String uniq_name = null;

    String help = "\nUsage:\n\n" +
      "   java AlarmBrowser [-driver driver] [-url url] [-db database] [-account account]\n" +
      "                     [-pwd password] [-a project] [-u uniq_name] [-nobeep]\n\n";


    for(int i=0; i<args.length; i++) {
      
      if(args[i].equalsIgnoreCase("-h")) {
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
	
      } else if(args[i].equalsIgnoreCase("-a")) {
	project=args[i+1];
	i++;
	
      } else if(args[i].equalsIgnoreCase("-u")) {
	uniq_name=args[i+1];
	i++;
	
      } else if(args[i].equalsIgnoreCase("-noBeep")) {
	beep = false;
      }
    }
    
    (new Alarm(driver,url,database,account,password,beep,project,uniq_name)).setVisible(true);

	}
}







