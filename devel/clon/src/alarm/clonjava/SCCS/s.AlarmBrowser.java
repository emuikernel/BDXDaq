h18756
s 00000/00000/00084
d D 1.6 07/11/23 16:24:11 boiarino 7 6
c *** empty log message ***
e
s 00001/00001/00083
d D 1.5 06/10/12 23:20:36 boiarino 6 5
c set "" password
c 
e
s 00009/00006/00075
d D 1.4 06/10/12 14:46:00 boiarino 5 4
c package clonjava;
c 
e
s 00001/00001/00080
d D 1.3 06/08/22 22:32:24 boiarino 4 3
c *** empty log message ***
e
s 00009/00001/00072
d D 1.2 03/10/05 17:18:57 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 03/10/03 15:30:48 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 alarm_browser/s/AlarmBrowser.java
e
s 00073/00000/00000
d D 1.1 03/10/03 15:30:47 wolin 1 0
c date and time created 03/10/03 15:30:47 by wolin
e
u
U
f e 0
t
T
I 3
//
// java AlarmBrowser
//          -a clasprod
//          -u alarm_browser
//          -driver com.cariboulake.jsql.JSQLDriver
//

I 5
package clonjava;

E 5
E 3
I 1
import java.lang.*;


class AlarmBrowser
{


  static public void main(String args[]) {

    boolean beep = true;
D 3
    String driver = "COM.cariboulake.sql.ingres.JSQLDriver";
E 3
I 3
D 5
    //String driver = "COM.cariboulake.sql.ingres.JSQLDriver";
    String driver = "com.cariboulake.jsql.JSQLDriver";
E 3
D 4
    String url       = "jdbc:caribou:jsqlingres://db5:6024/";
E 4
I 4
    String url       = "jdbc:caribou:jsqlingres://clondb1:6024/";
E 4
    String database  = "clasprod";
    String account   = "foo";
    String password  = null;
E 5
I 5

    String driver         = "com.mysql.jdbc.Driver";
    String url            = "jdbc:mysql://clondb1:3306/";
    String database       = "clasprod";
    String account        = "clasrun";
D 6
    String password       = null/*"e1tocome"*/;
E 6
I 6
    String password       = "";
E 6

E 5
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







E 1
