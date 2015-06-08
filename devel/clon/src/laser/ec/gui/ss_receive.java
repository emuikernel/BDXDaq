/*----------------------------------------------------------------------
**          Program:  ss_receive.java
**          Created by Arnie Larson. 7/15/99
**   
**     This program receives messages over the RTServer using smartsockets.
**    The messages are of project 'clasprod' and use subject callbacks to    
**    process messages of subject 'Status' and 'Progress'.  These messages 
**    are printed onto a gui to read the messages.  You could use the Status 
**    callback process to implement a way of observing status messages using 
**    some sort of boolean indicator but that has not been finished here.
**
**------------------------------------------------------------------------
*/
import java.awt.*;
import java.awt.event.*;
import java.applet.*;
import java.io.*;
import com.smartsockets.*;

/*
 ******** This class holds all of the other classes and main routine ********
*/


public class ss_receive  {


  String keyword[] = new String[200];  
  int no_index =0;
  int value[] = new int[200];  
  int go_flag=1;
  boolean done_flag=false;
  boolean found_keyword=false;
  boolean do_once=true;

  private Button done_button;
  private  TextArea done_text;
  
  // no_index = 1;
  // keyword[0] = ("larson");
  // value[0] = 100;
    
 public void printStatus()
 {   
  System.out.println("no of vals=  " + no_index);  
  for(int j = 0; j < no_index; j++) 
     {
       System.out.println( "key=" + keyword[j] + "              value = " + value[j]);           
      }
  }
      
  public void addStatus(String s, int val) 
  {  
    for(int j = 0; j < no_index; j++) 
    {
       if(s.equals(keyword[j]))
       { 
       value[j] = val;
       //System.out.println("test string = " + s + " k = " + keyword[j]);
       return;
       }
     } 
    //System.out.println(" no match " );   
    keyword[no_index] = s;
    value[no_index] = val;
    no_index++;        
   }
    
 public int checkStatus(String s)  
 {  
   for(int j = 0; j < no_index; j++) 
   {  
        if(s.equals(keyword[j]))
       { 
       found_keyword=true;
       //System.out.println("test string = " + s + " k = " + keyword[j]);
       return value[j];
       }
       else
       {
       found_keyword=false;
       //System.out.println("test string = not found"+s+"not here"+keyword[j]);
       }
    } 
   return -9999;
 }
 
 /*


    
 */   
 
  Label A1 = new Label("Info messages from 'Progress' and 'Status' subjects.");
/*  
 Label[] 
   L1 = new Label("config_file_read"),
   L2 = new Label("clon10_connection"),
   L3 = new Label("coda_ready" ),
   L4 = new Label("camac1_connection" ),
   L5 = new Label("trigger_bit_11_set" ),
   L6 = new Label("laser_power_on" ),
   L7 = new Label("laser_ready" ),
   L8 = new Label("laser_enabled"),
   L9 = new Label("coda_config_ok"),
   L10 = new Label("laser_filter_ready"),
   L11 = new Label("pulser_ready"),
   L12 = new Label("jorway_pulser"),
   L14 = new Label("pulser_loaded"),
   L15 = new Label("number_of_pulses"),
   L16 = new Label("pulser_frequency"),
   L17 = new Label("CCWlimit"),
   L18 = new Label("CWlimit"),
   L19 = new Label("filter_moving"),
   L20 = new Label("filter_setting"),
   L21 = new Label("wait_setting"),
   L22 = new Label("asleep");
 */
 
 TextArea t1 = new TextArea("Progress Messages",10,60,
       TextArea.SCROLLBARS_VERTICAL_ONLY);
 TextArea t2 = new TextArea("Status Messages",10,60,
       TextArea.SCROLLBARS_VERTICAL_ONLY);
             
 // TextField t2 = new TextField("Where am I?");
   
/*
 *  This method, prnt(String s), is used routinely in the code to put  
 *     text onto the TextArea and debug the program.
 */ 
            
 public void prnt(String s) {
   t1.append("\n" + s);
   // System.out.println("Just did a prnt!!!");
 } 
     
 public void prnt2(String s) {
   t2.append("\n" + s);
 }

/*
 *******class TextThingy creates GUI components ************
 */
 
public class TextThingy extends Frame { 
 
 public TextThingy() {
   
   Panel a = new Panel();
   Panel b = new Panel();
   Panel c = new Panel();
   add("South", a);
   a.add(t1);
   t1.setEditable(false);
   t1.setBackground(Color.white);
   a.setBackground(Color.cyan); 
   // a.setForeground(Color.cyan);  
      
   // b.add(t2);
   add("North", b);
   b.setBackground(Color.cyan);
   b.add(A1);
   
   add("Center", c);
   c.setBackground(Color.cyan);
   c.add(t2);
   t2.setEditable(false);
   t2.setBackground(Color.white);

   }
} 
public class Done_message extends Frame { 
 
 public Done_message() {
   
   Panel a = new Panel();
  
   add("Center", a);
   Font buttonFont = new Font("Serif", Font.BOLD, 90);

   done_button = new Button("DONE");
   done_button.setFont(buttonFont);
   done_button.setForeground(Color.black);
   done_button.setBackground(Color.red); 
   a.add(done_button);
   
  
   done_text = new TextArea("End Message ",10,60, TextArea.SCROLLBARS_NONE);
   a.add(done_text);
   done_text.setEditable(false);
   done_text.setBackground(Color.white);
   done_text.append("     The EC calibration has ended normally. \n");
   done_text.append("You should end the run in the usual fashion. \n");
   done_text.append(" (Click the end button on the CODA gui.) \n");

   a.setBackground(Color.gray); 
   }
} 
/*
 ************  class Progress is used to process messages **********
 *                      with the subject 'Progress'.
 */

public class Progress implements TipcProcessCb {
   
    public void process(TipcMsg msg, Object arg) {     
      
      // System.out.println("*** Received Progress message of type <"      
      //        		+ msg.getType().getName() +"> ***");
      // prnt("*** Received Progress message of type <"      
      //        		+ msg.getType().getName() +"> ***");
   
      int mt = msg.getType().getNum();
      switch (mt) {
        case TipcMt.INFO:        		
          try {
            msg.setCurrent(0);
            prnt(" " + msg.nextStr());
            // System.out.println("Text from message = " + msg.nextStr());                  
          } // catch
           catch (TipcException e) { }
           break;
           
         default:
           // handle message of unknown type
          // System.out.println("Can not process <" + msg.getType().getName() + "> message."); 
           prnt("Can not process <" + msg.getType().getName() + "> message."); 
           break;
       }  // switch  
        
    } // process
   
  } // Progress
/*
 ************** class Status is used to process messages *************
 *                   of subject 'Status'.
 */   
public class Status implements TipcProcessCb {
    
   public void process(TipcMsg msg, Object arg) {
     String temp;
     int i;
     // System.out.println("*** Received Status message of type <" + 
     //         		msg.getType().getName() +"> ***");
     // prnt("*** Received Status message of type <" + 
     //         		msg.getType().getName() +"> ***");
      int mt = msg.getType().getNum();
      switch (mt) {
        case TipcMt.INFO:        		
          try {
      
            msg.setCurrent(0);
            do {
              temp = msg.nextStr();
              if(temp == null)
                break;
               i = msg.nextInt4();
              prnt2("ID = " + temp + ", Value = " + i);
              
              addStatus(temp,i);
              }
            while(true);
          } // catch
           catch (TipcException e) { }
           break;
           
         default:
           // handle message of unknown type
           System.out.println("Can not process <" + msg.getType().getName() + "> message."); 
           prnt("Can not process <" + msg.getType().getName() + "> message."); 
           break;
       }  // switch 
    } // process
  } // Status
   
/*
 **** This method combines all of the classes and is called in main() ****
 */
    
  public ss_receive() {
    
    TextThingy thig = new TextThingy(); 
      // makes an instance of TextThingy which initializes all graphical parts  
    thig.setTitle(" Status Receive Interface");
    thig.pack();
      //  thig.setSize(450,400); // can be used to manually set frame size
    thig.setVisible(true);
      // thig.setBackground(Color.yellow); // colors entire frame
    thig.addWindowListener(
        new WindowAdapter() {
          public void windowClosing(WindowEvent e){
            System.out.println(e);
            System.out.println("Window Closing");
            System.exit(0);
          }
        });   
     // LaserStatus ls = new LaserStatus();
     // prnt("testing, inside ss_receive");

    TipcMsg msg = null;
     // set the ss.project
    try {
      Tut.setOption("ss.project", "clasprod");
      TipcSrv srv=TipcSvc.getSrv();

      // create a new info mt/subject callback and register it
      Progress Prog = new Progress();
      TipcCb rcbh1 = srv.addProcessCb(Prog,"/EClaser/progress", null);
      // check the 'handle' returned for validity
      if (null == rcbh1) {
        Tut.exitFailure("Couldn't register subject callback!\n");
      } // if

      // create a new info mt/subject callback and register it
      Status Stat = new Status();
      TipcCb rcbh2 = srv.addProcessCb(Stat,"/EClaser/status", null);
      // check the 'handle' returned for validity
      if (null == rcbh2) {
        Tut.exitFailure("Couldn't register subject callback!\n");
      } // if

      // connect to RTserver
      if (!srv.create()) {
        Tut.exitFailure("Couldn't connect to RTserver!\n");
      } // if
      // subscribe to the appropriate subject
      srv.setSubjectSubscribe("/EClaser/progress", true);
      srv.setSubjectSubscribe("/EClaser/status", true);

      // read and process all incoming messages
      System.out.println(" about to check messages ");
     // while (srv.mainLoop(TipcDefs.TIMEOUT_FOREVER)) {} 
	while( go_flag == 1 )
	{
       System.out.println("  status report ");
	srv.mainLoop(1.0);
	printStatus();
        found_keyword=false;
        done_flag=false;
	int val = checkStatus("done");
	if( found_keyword )
	 {
          // System.out.println(" done found " +val);
          if(val == 1){done_flag=true;}
 	 }
 	 else
 	 {
 	 // System.out.println("done NOT found "+val);
 	 }
 	 
	if(done_flag && do_once)
	{
	 do_once=false;
	Done_message win= new Done_message();
	  win.setTitle(" Status Receive Interface");
          win.pack();
          win.setSize(450,400); // can be used to manually set frame size
          win.setVisible(true);
          win.addWindowListener(
          new WindowAdapter() 
          {
            public void windowClosing(WindowEvent e)
            {
              System.out.println(e);
              System.out.println("Window Closing");
              System.exit(0);
            }
          }     );  
	 }
	//sleep(100);
	}
	
	
      // unregister the callbacks
      srv.removeProcessCb(rcbh1);
      srv.removeProcessCb(rcbh2);

      // disconnect from RTserver
      srv.destroy();
    } catch (TipcException e) {
      Tut.fatal(e);
    } // catch
  } // subjcbs2 (constructor)

  public static void main(String[] argv) {
    // only one method is called, ss_receive
    // System.out.println("Inside Main");
    
     new ss_receive();
    // this mesthod combines all of the other classes   
  } // main
} // ss_receive class
