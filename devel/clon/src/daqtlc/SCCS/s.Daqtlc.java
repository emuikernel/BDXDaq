h02601
s 00001/00001/02085
d D 1.28 04/12/07 16:58:45 vvsap 29 28
c *** empty log message ***
e
s 00002/00010/02084
d D 1.27 04/03/11 10:08:35 gurjyan 28 27
c debugging
c 
e
s 00001/00001/02093
d D 1.26 04/02/03 20:48:54 boiarino 27 26
c *** empty log message ***
e
s 00000/00000/02094
d D 1.25 02/09/30 11:47:51 gurjyan 26 25
c *** empty log message ***
e
s 00001/00001/02093
d D 1.24 02/09/30 11:40:38 gurjyan 25 24
c et2et clon10-clon00-daq1 changed to clon10-clon00
c 
e
s 00009/00002/02085
d D 1.23 01/11/13 15:40:00 gurjyan 24 23
c *** empty log message ***
e
s 00002/00000/02085
d D 1.22 01/11/12 12:14:19 gurjyan 23 22
c *** empty log message ***
e
s 00032/00029/02053
d D 1.21 01/11/06 12:55:22 gurjyan 22 21
c *** empty log message ***
e
s 00007/00005/02075
d D 1.20 01/10/30 13:40:27 gurjyan 21 20
c *** empty log message ***
e
s 00002/00002/02078
d D 1.19 01/10/23 09:16:29 gurjyan 20 19
c *** empty log message ***
e
s 00100/00015/01980
d D 1.18 01/10/19 13:25:11 gurjyan 19 18
c test
e
s 00003/00000/01992
d D 1.17 01/10/12 08:53:52 gurjyan 18 17
c *** empty log message ***
e
s 00020/00012/01972
d D 1.16 01/09/28 16:04:29 gurjyan 17 16
c rewrite
c 
e
s 00003/00003/01981
d D 1.15 01/06/06 14:54:41 gurjyan 16 15
c no recover for recsis and trigmon
c 
e
s 00077/00000/01907
d D 1.14 01/06/01 15:46:23 gurjyan 15 14
c added raid7 and raid8. Modified message structure.
c 
e
s 00010/00008/01897
d D 1.13 01/05/01 14:46:11 gurjyan 14 13
c *** empty log message ***
e
s 00002/00000/01903
d D 1.12 01/03/02 21:45:22 gurjyan 13 12
c *** empty log message ***
e
s 00002/00002/01901
d D 1.11 00/11/30 16:13:26 gurjyan 12 11
c no 04 in the message 
c 
e
s 00003/00003/01900
d D 1.10 00/11/09 15:36:34 gurjyan 11 10
c typo
c 
e
s 00033/00033/01870
d D 1.9 00/11/03 09:57:49 gurjyan 10 9
c live time pulser rate is now 100000Hz
c 
e
s 00039/00010/01864
d D 1.8 00/10/25 15:52:46 gurjyan 9 8
c typo
c 
e
s 00007/00007/01867
d D 1.7 00/10/24 13:35:36 gurjyan 8 7
c clon04 ET added 
c 
e
s 00005/00012/01869
d D 1.6 00/10/18 14:48:24 gurjyan 7 6
c new et2et message in the evt_status message
c 
e
s 00006/00006/01875
d D 1.5 00/10/18 12:44:01 gurjyan 6 5
c 
e
s 00054/00028/01827
d D 1.4 00/10/05 13:21:32 gurjyan 5 4
c add new online process monitoring algorithm
c 
e
s 00000/00000/01855
d D 1.3 00/09/07 11:47:19 gurjyan 4 3
c 
e
s 00006/00006/01849
d D 1.2 00/08/04 16:48:42 gurjyan 3 1
c add env variables
c 
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 daqtlc/Daqtlc.java
e
s 01855/00000/00000
d D 1.1 00/07/31 14:48:42 gurjyan 1 0
c date and time created 00/07/31 14:48:42 by gurjyan
e
u
U
f e 0
t
T
I 1
import java.io.*;
import java.lang.*;
import java.awt.*;
import java.util.*;
import java.beans.*;
import java.text.DecimalFormat;
import jclass.chart.*; 
import com.smartsockets.*;
import clon.ipcutil.*;
import jas.hist.*;
import jas.hist.util.*;



public class Daqtlc extends DaqControlGui implements TipcProcessCb, Runnable {
  
I 19
    private int ts_flag = 1;
    private int roc_flag = 1;
    private int eb_flag = 1;
    private int er_flag = 1;
    private int etsys04_flag = 1;

E 19
D 5

E 5
  private static clt_Thread t_clt; 
  private static final HistogramUpdate hdr_clt = new HistogramUpdate(HistogramUpdate.DATA_UPDATE,true);
  private static tlt_Thread t_tlt; 
  private static final HistogramUpdate hdr_tlt = new HistogramUpdate(HistogramUpdate.DATA_UPDATE,true);
  private static flt_Thread t_flt; 
  private static final HistogramUpdate hdr_flt = new HistogramUpdate(HistogramUpdate.DATA_UPDATE,true);
  private static dr_Thread t_dr; 
  private static final HistogramUpdate hdr_dr = new HistogramUpdate(HistogramUpdate.DATA_UPDATE,true);
  private static evtr_Thread t_evtr; 
  private static final HistogramUpdate hdr_evtr = new HistogramUpdate(HistogramUpdate.DATA_UPDATE,true);
  private static l2pf_Thread t_l2pf; 
  private static final HistogramUpdate hdr_l2pf = new HistogramUpdate(HistogramUpdate.DATA_UPDATE,true);

  private static final HistogramUpdate hdr_ebbuf = new HistogramUpdate(HistogramUpdate.DATA_UPDATE,true);
  private static final HistogramUpdate hdr_drroc = new HistogramUpdate(HistogramUpdate.DATA_UPDATE,true);
  
I 5
D 29
  //for online monitoring 
E 29
I 29
  //For online monitoring   
E 29
  private int estmon, ipcb, ss, trgm, evtm, recs;
E 5

  public static JASHistData data_drroc; 
  public static JASHistData data_ebbuf; 
  public static JASHistData data_l2pf; 
  public static JASHistData data_evtr; 
  public static JASHistData data_dr; 
  public static JASHistData data_clt; 
  public static JASHistData data_tlt; 
  public static JASHistData data_flt; 

  private static final long mek = 1;

  public static double live_clock, live_trig, live_fcup;
  
  public  SysExe mySysExe = new SysExe();
  
  public static TipcSrv srv;
  DecimalFormat fmt = new DecimalFormat("#0.00");
  public int[] trgs ;
  public long[] ltrgs = new long[80];
  public long[] ltrgs_diff = new long[80];
  public long[] ltrgs_prev = new long[80];
  public double triglive [][] = { {5}, {25},{75}};
  public double clocklive[][] = { {5}, {25},{75}};
  public int[] s1st ;
  public double eb_evtrate;
  public double roc_datarate [] = new double[32];
  public int ebbuf [] = new int[32];
  public static double er_evtrate; 
  public static double er_datarate;
  public static double l2pf [][] = new double[2][3];
  
  String[] stpie = {"x","y","z"}; 
  
  
  String[] stgraph = {"x","y"};
  
  
  // constructor initializes all IPC stuff
  public Daqtlc() {
    try {
      for(int j=0; j<80; j++) ltrgs_prev[j] = 0;
    } catch (Exception e) {}
I 5

    //initialize online monitor process status check vars
    estmon = DataPool.CheckIfUndefined[0];
    ipcb   = DataPool.CheckIfUndefined[1];
    ss     = DataPool.CheckIfUndefined[2];
    trgm   = DataPool.CheckIfUndefined[3]; 
    evtm   = DataPool.CheckIfUndefined[4]; 
    recs   = DataPool.CheckIfUndefined[5];
E 5
    
D 5
    
E 5
    try {
      // create server connection
      //     srv = TipcSvc.getSrv();
      //      Tut.assert(null != srv, "didn't get srv");
      
      System.out.println("we got the server");
      // subscribe to subjects
      srv.setSubjectSubscribe("info_server/in/update_run_control/clasprod", true);
      srv.setSubjectSubscribe("info_server/in/evt_status_monitor", true);
      srv.setSubjectSubscribe("info_server/in/scaler_server/data", true);
      srv.setSubjectSubscribe("info_server/in/scaler_server/hardware", true);
      srv.setSubjectSubscribe("info_server/in/diskcheck/raid", true);
      srv.setSubjectSubscribe("info_server/in/hardware_monitor/ts", true);
      srv.setSubjectSubscribe("info_server/in/hardware_monitor/roc", true);
      srv.setSubjectSubscribe("info_server/in/hardware_monitor/eber", true);
I 19
      srv.setSubjectSubscribe("info_server/in/ipc_check", true);
E 19
      
      // create message types and callbacks
      srv.addProcessCb(this,"info_server/in/...",null);
      
      // flush all messages
      srv.flush();
      
    } catch (TipcException e) {
      Tut.fatal(e);
    }
    setVisible(true);

    t_clt = new clt_Thread();
    data_clt = live_time_chart.addData(new DTsource_clt());
    t_tlt = new tlt_Thread();
    data_tlt = live_time_chart.addData(new DTsource_tlt());
    t_flt = new flt_Thread();
    data_flt = live_time_chart.addData(new DTsource_flt());
D 28

E 28
    live_time_chart.getYAxis().setRange(0.0, 100.0);
D 28

E 28
    t_dr = new dr_Thread();
    data_dr = data_rate_chart.addData(new DTsource_dr());
    t_evtr = new evtr_Thread();
    data_evtr = event_rate_chart.addData(new DTsource_evtr());
    t_l2pf = new l2pf_Thread();
    data_l2pf = l2_pasfail_chart.addData(new DTsource_l2pf());
D 28

E 28
    l2_pasfail_chart.getYAxis().setRange(0.0, 1.0);
D 28

E 28
    data_ebbuf = eb_buffer_comp.addData(new DTsource_ebbuf());
    eb_buffer_comp.getYAxis().setRange(0.0, 9.0);
D 28

E 28
    data_drroc = data_rate_comp.addData(new DTsource_drroc());
    

    data_clt.show(true);
    data_tlt.show(true);
    data_flt.show(true);
    data_dr.show(true);
    data_evtr.show(true);
    data_l2pf.show(true);
D 28

E 28
    data_ebbuf.show(true);
    data_drroc.show(true);
D 28
    
E 28

D 28

E 28
  }
  
  
  // run method for IPC mainloop
  public void run() {
    System.out.println("Ipc loop is active");    
I 28

E 28
    try {
      ipc_init.check(TipcDefs.TIMEOUT_FOREVER);
    }
    catch (Exception e) {}
D 28

E 28
  }



    // message process callback
    public void process(TipcMsg msg, Object arg) {
      String s = null;
      String bank_name;
      String process_name;
      String process_status;
      int nevt_rec;
      double rec_rate;

      try {
	msg.setCurrent(0);
	s = msg.nextStr();
	//Debug printout for the message types
	//        System.out.println(s);
      } catch (TipcException e) {} 	

	if(s.equals("hardware_monitor/ts")) visual_ts(msg);
	else if(s.equals("hardware_monitor/roc")) visual_roc(msg);
	else if(s.equals("hardware_monitor/eber")) visual_eber(msg);
	else if(s.equals("evt_status_monitor")) visual_online(msg);
	else if(s.equals("scaler_server")) visual_scaler(msg);
	else if(s.equals("diskcheck/raid")) visual_raid(msg);
	else if(s.equals("update_run_control/clasprod")) visual_run(msg);
I 19
	else if(s.equals("ipc_check")) visual_online2(msg);
E 19
	recover_daq_button();
    }

  public void recover_daq_button() {
D 19
    if(eb_evtrate > 1.0) {
E 19
I 19
D 21
      //      System.out.println(ts_flag + " "+ roc_flag+ " "+ eb_flag+ " "+ er_flag+" ++++++"+ DataPool.daq_recover_clicked);
      //(eb_evtrate < 1.0) ||
E 21
I 21
D 22
      // System.out.println(ts_flag + " "+ roc_flag+ " "+ eb_flag+ " "+ er_flag+" ++++++"+ DataPool.daq_recover_clicked);
      
E 21
    if(((ts_flag ==  0) || (roc_flag == 0) || (eb_flag == 0) || (er_flag == 0))) {
	if(DataPool.daq_recover_clicked == 0)
	          daq_recover.setEnabled(true);
	else 
      daq_recover.setEnabled(false);
E 22
I 22
D 28
      System.out.println(ts_flag + " "+ roc_flag+ " "+ eb_flag+ " "+ er_flag+" ++++++"+ DataPool.daq_recover_clicked);
E 28
I 28
      System.out.println(ts_flag + " "+ roc_flag+ " "+ eb_flag+ " "+ er_flag+" +++"+ DataPool.daq_recover_clicked);
E 28
E 22

D 22
      daq_recover.setBackground(Color.red);
      daq_recover.repaint();
      

    }
    else {
D 21
	if ((DataPool.daq_recover_clicked == 1) && (ts_flag ==1) && (roc_flag == 1) && (eb_flag == 1) && (er_flag == 1)) {
E 21
I 21
	if ((ts_flag ==1) && (roc_flag == 1) && (eb_flag == 1) && (er_flag == 1)) {
E 21
E 19
      daq_recover.setForeground(new Color(0)); 
      daq_recover.setBackground(new Color(-3355444)); 
      daq_recover.setEnabled(false);
I 19
      daq_recover.repaint();
D 21
      DataPool.daq_recover_clicked =0;
E 21
I 21
      if((eb_evtrate < 1.0)) DataPool.daq_recover_clicked =0;
            System.out.println(ts_flag + " "+ roc_flag+ " "+ eb_flag+ " "+ er_flag+" ++++++"+ DataPool.daq_recover_clicked);

E 21
	}
E 19
    }
E 22
I 22
      if(((ts_flag ==  0) || (roc_flag == 0) || (eb_flag == 0) || (er_flag == 0))) {
	  if(DataPool.daq_recover_clicked == 0) {
	      daq_recover.setEnabled(true);
	      daq_recover.setBackground(Color.red);
	      daq_recover.repaint();
	  }
	  else {
	      daq_recover.setEnabled(false);
	      daq_recover.setBackground(Color.red);
	      daq_recover.repaint();
	  }
      }
      else if ((DataPool.daq_recover_clicked == 1) && (ts_flag ==1) && (roc_flag == 1) && (eb_flag == 1) && (er_flag == 1)) {
	  DataPool.daq_recover_clicked =0;
	  daq_recover.setForeground(new Color(0)); 
	  daq_recover.setBackground(new Color(-3355444)); 
	  daq_recover.setEnabled(false);
	  daq_recover.repaint();
      }
      else if((eb_evtrate > 1.0) && (ts_flag ==1) && (roc_flag == 1) && (eb_flag == 1) && (er_flag == 1)) {
	  DataPool.daq_recover_clicked =0;
	  daq_recover.setForeground(new Color(0)); 
	  daq_recover.setBackground(new Color(-3355444)); 
	  daq_recover.setEnabled(false);
	  daq_recover.repaint();
      }
E 22
  }
D 22
  
E 22
I 22
    
    
E 22

  public void visual_ts(TipcMsg msg) {
    String ts_status = null;
    int ts_go = 0;

    try {
    ts_status = msg.nextStr();
    if(ts_status.equals("UNDEFINED")) {
      for ( int i=0; i<=21;i++) {
	if(i!=14&&i!=15) {
	csr[i].setBackground(Color.red);
	csr[i].repaint();
      }
      }
      for ( int i=0; i<=31;i++) {
	st[i].setBackground(Color.red);
	st[i].repaint();
      }
I 19
      ts_flag = 0;
      if(DataPool.daq_recover_clicked == 0) {
      daq_recover.setEnabled(true);
      daq_recover.setBackground(Color.red);
      daq_recover.repaint();
      }
E 19
      return;
    }
D 19
    

E 19
I 19
    ts_flag = 1;    
E 19
    String ts_prescale = msg.nextStr();
    StringTokenizer tokens = new StringTokenizer(ts_prescale);
      for (int i=0;i<=7;i++) {
  	prescale[i].setText(tokens.nextToken());
      }
    
    String ts_scaler = msg.nextStr();
    
    long ts_control = msg.nextInt8();
    for (int i=1; i<=12;i++) {
      if (((mek << i)&ts_control)!=0) {
	bit[i-1].setBackground(Color.green);
	bit[i-1].repaint();
      } else {
	bit[i-1].setBackground(Color.lightGray);
	bit[i-1].repaint();
      }
    }

    long ts_cstatus = msg.nextInt8();
    for (int i=0; i<=21;i++) {
      if (((mek << i)&ts_cstatus)!=0) {
	if(i!=14&&i!=15) {
	csr[i].setBackground(Color.green);
	csr[i].repaint();
	}
	if(i==0) ts_go = 1;
      } else {
	csr[i].setBackground(Color.lightGray);
	csr[i].repaint();
	if(i==0) ts_go = 0;
      }	
    }
    
    long ts_state = msg.nextInt8();
    for (int i=0; i<=31;i++) {
      if (((mek << i)&ts_state)!=0) {
	st[i].setBackground(Color.green);
	st[i].repaint();
      } else {
	st[i].setBackground(Color.lightGray);
	st[i].repaint();
      }	
    }

D 14
    long ts_rocenable = msg.nextInt8();
E 14
I 14
    DataPool.ts_rocenable = msg.nextInt8();
E 14
    for (int i=0; i<=31;i++) {
D 14
      if (((mek << i)&ts_rocenable)!=0) {
E 14
I 14
      if (((mek << i)&DataPool.ts_rocenable)!=0) {
E 14
	roc_button[i].setBackground(Color.yellow);
	roc_button[i].repaint();
      } else {
	roc_button[i].setBackground(Color.lightGray);
	roc_button[i].repaint();
      }	
    }
    }catch (TipcException e) {}
    try {
      if(ts_go<=0 && ts_status.equals("active")) update_time.setText("Paused");
      else  update_time.setText(ts_status);
    } catch(java.beans.PropertyVetoException e) { } 
  }
  
  public void visual_roc(TipcMsg msg) {
    int roc_status [] = new int[39];
    int roc_numevt [] = new int[32];
    double roc_evtrate [] = new double[32];
I 22
	roc_flag = 1;
E 22
    
    try {
      roc_status = msg.nextInt4Array();
      roc_numevt = msg.nextInt4Array();
      roc_evtrate = msg.nextReal8Array();
      roc_datarate = msg.nextReal8Array();
    } catch (TipcException e) {}
    for (int i=0;i<=38;i++) {
      if(roc_status[i] == -1) {
	roc_button[i].setBackground(Color.red);
	roc_button[i].repaint();
	if(i<=31) {
D 14
	  daq_recover.setBackground(Color.red);
	  daq_recover.repaint();
	  daq_recover.setEnabled(true);
E 14
I 14
	    if (((mek << i)&DataPool.ts_rocenable)!=0) {
D 19
		daq_recover.setBackground(Color.red);
		daq_recover.repaint();
		daq_recover.setEnabled(true);
E 19
I 19
		roc_flag = 0;
		if(DataPool.daq_recover_clicked == 0) {
		    daq_recover.setEnabled(true);
		    daq_recover.setBackground(Color.red);
		    daq_recover.repaint();
		}
D 22
	    } else { 
		roc_flag = 1;
E 19
	    }
E 14
D 19
	}
E 19
I 19
 	}
E 22
I 22
	    } 
	}
E 22
E 19
      } else {
D 14
	roc_button[i].setBackground(Color.green);
	roc_button[i].repaint();
E 14
I 14
	  roc_button[i].setBackground(Color.green);
	  roc_button[i].repaint();
I 19
D 22
	  roc_flag = 1;
E 22
E 19
E 14
      }
    }
  }
D 14
  
E 14
I 14
    
E 14
  public void visual_eber(TipcMsg msg) {
    try {
      try {
      run_config.setText(msg.nextStr());
      } catch(java.beans.PropertyVetoException e) { } 
      String eb_status = msg.nextStr();
      if(eb_status.equals("UNDEFINED")) {
	EB.setBackground(Color.red);
	EB.repaint();
I 19
	eb_flag = 0;
      if(DataPool.daq_recover_clicked == 0) {
	daq_recover.setEnabled(true);
E 19
	daq_recover.setBackground(Color.red);
	daq_recover.repaint();
D 19
	daq_recover.setEnabled(true);
E 19
I 19
      }
E 19
D 6
	return;
E 6
I 6
	//	return;
E 6
      } else {
I 18
D 19

E 19
I 19
	  eb_flag = 1;
E 19
E 18
	EB.setBackground(Color.green);
	EB.repaint();
I 18
	//	if( (pm!=null) && (pm.isCanceled()==false)) pm.close();

E 18
      }

      int eb_numevt = msg.nextInt4();
      eb_evtrate = msg.nextReal8();
      double eb_datarate = msg.nextReal8();
      ebbuf = msg.nextInt4Array();
      
D 6
      EB.setBackground(Color.green);
      EB.repaint();
E 6
I 6
      //      EB.setBackground(Color.green);
      //      EB.repaint();
E 6

      try {
	eb_evt_rate.setText(Double.toString(eb_evtrate));
	eb_data_rate.setText(Double.toString(eb_datarate));
      } catch (PropertyVetoException e) {}
      
      String er_status = msg.nextStr();	  
      if(er_status.equals("UNDEFINED")) {
	ER.setBackground(Color.red);
	ER.repaint();
I 19
	er_flag = 0;
      if(DataPool.daq_recover_clicked == 0) {
	daq_recover.setEnabled(true);
E 19
	daq_recover.setBackground(Color.red);
	daq_recover.repaint();
D 19
	daq_recover.setEnabled(true);
E 19
I 19
      }
E 19
D 6
	return;
E 6
I 6
	//	return;
E 6
      } else {
I 19
	  er_flag = 1;
E 19
	ER.setBackground(Color.green);
	ER.repaint();
      }
      
      int er_numevt = msg.nextInt4();
      er_evtrate = msg.nextReal8();
      er_datarate = msg.nextReal8();

D 6
      ER.setBackground(Color.green);
      ER.repaint();
E 6
I 6
      //      ER.setBackground(Color.green);
      //      ER.repaint();
E 6
      try {
      er_evt_rate.setText(Double.toString(er_evtrate));
      er_data_rate.setText(Double.toString(er_datarate));
      number_events.setText(Integer.toString(er_numevt));
      } catch (PropertyVetoException e) {}
    } catch (TipcException e) {}
  }
  
  public void visual_online(TipcMsg msg) {
    int a=0;;
    String s = " ";
    String process_name = " ";
    int nevt_rec = 0; 
    double rec_rate = 0.0;
    int nevt_proc = 0; 
    double proc_rate = 0.0;
    int flag =0; 
    String process_status = " ";
D 5
    int recsis_restart = 0;
E 5
    int et10_flag = 0;
    int et00_flag = 0;
D 9
    double et00_rate = 0.0;
E 9
I 9
    int et04_flag = 0;
I 19
    int evstatmon_flag = 0;
    int scaler_server_flag = 0;
    int mon_flag = 0;
E 19
    double e00_rate = 0.0;
    double et04_rate = 0.0;
E 9
    double et10_rate = 0.0;
    //How many processes are in there?
    try {
      a = msg.nextInt4();
      Ipc2it.setBackground(Color.yellow);
      Ipc2it.repaint();
      It2ipc.setBackground(Color.yellow);
      It2ipc.repaint();
      ET.setBackground(Color.yellow);
      ET.repaint();
      Ipc2dd.setBackground(Color.yellow);
      Ipc2dd.repaint();
      Stadis.setBackground(Color.yellow);
      Stadis.repaint();
      Scaler.setBackground(Color.yellow);
      Scaler.repaint();
      Evt_monitor.setBackground(Color.yellow);
      Evt_monitor.repaint();
      Recsis.setBackground(Color.yellow);
      Recsis.repaint();
    } catch (TipcException e) {} 
    
    for(int i=1;i<=a;i++) {
      try {          
	//get the process name
	process_name = msg.nextStr();
	
	//process related info. We don't care.
	s = msg.nextStr();	
	s = msg.nextStr();
	s = msg.nextStr();
	
	//important process related information here.
	nevt_rec = msg.nextInt4();
	rec_rate = msg.nextReal8();
	nevt_proc = msg.nextInt4();
	proc_rate = Double.valueOf(fmt.format(msg.nextReal8())).doubleValue();
	flag = msg.nextInt4();
	process_status = msg.nextStr();
      } catch (TipcException e) {} 
      
      // What process it was?
      if(process_name.equals("ipcbank2et_10_clasprod")) {
	if(process_status.equals("unknown")) {
	  Ipc2it.setBackground(Color.red);
	  Ipc2it.repaint();
	  if(DataPool.auto_recover == 1) {
D 5
	  info_text.setText("ipcbank2et is not running.I will try to restart it.");
D 3
	  mySysExe.run_unix("remsh -n clon10 /usr/local/clas/bin/ipcbank2et -a clasprod -s clasprod >> & /usr/local/clas/parms/run_log/ipcbank2et_clon10.log &");
E 3
I 3
	  mySysExe.run_unix("remsh -n clon10 $CLON_BIN/ipcbank2et -a clasprod -s clasprod >> & $CLON_PARMS/run_log/ipcbank2et_clon10.log &");
E 5
I 5
	    ipcb--;
	    if(ipcb<=0) {
D 17
	      info_text.setText("ipcbank2et is not running.I will try to restart it.");
	      mySysExe.run_unix("remsh -n clon10 $CLON_BIN/ipcbank2et -a clasprod -s clasprod >> & $CLON_PARMS/run_log/ipcbank2et_clon10.log &");
	      ipcb = DataPool.CheckIfUndefined[1]; 
E 17
I 17
		info_text.setText("ipcbank2et is not running.I will try to restart it.");
		// mySysExe.run_unix("ipc_check -a clasprod -d ipcbank2et_10_clasprod");
		mySysExe.run_unix("remsh -n clon10 $CLON_BIN/ipcbank2et -a clasprod -s clasprod >> & $CLON_PARMS/run_log/ipcbank2et_clon10.log &");
		ipcb = DataPool.CheckIfUndefined[1]; 
E 17
	    }
E 5
E 3
	  }
	} else {
	  Ipc2it.setBackground(Color.green);
	  Ipc2it.repaint();
	}
      } 
      
      
      
D 7
      if(process_name.equals("et2ptp_10_clasprod:5678")) {
E 7
I 7
D 9
      if(process_name.equals("et2et_clasprod_clon10_clasprod_clon00-daq1")) {
E 9
I 9
      if(process_name.equals("et2et_clasprod_clon10_clasprod_clon04")) {
E 9
E 7
	if(process_status.equals("unknown")) {
	  It2ipc.setBackground(Color.red);
	  It2ipc.repaint();
	  ET.setBackground(Color.red);
	  ET.repaint();
	  et10_flag = 0;
I 7
	  Ipc2dd.setBackground(Color.red);
	  Ipc2dd.repaint();
D 11
	  et00_flag = 0;
E 11
I 11
	  et04_flag = 0;
I 19
	  /*
	  online_recover.setEnabled(true);
	  online_recover.setBackground(Color.red);
	  online_recover.repaint();
	  */

E 19
E 11
E 7
	} else {
	  It2ipc.setBackground(Color.green);
	  It2ipc.repaint();
	  ET.setBackground(Color.green);
	  ET.repaint();
	  et10_flag = 1;
D 7
	}
	et10_rate = proc_rate;
      } 
      
      
      if(process_name.equals("ptp2et_00_clasprod_10:5678")) {
	if(process_status.equals("unknown")) {
	  Ipc2dd.setBackground(Color.red);
	  Ipc2dd.repaint();
	  et00_flag = 0;
	} else {
E 7
	  Ipc2dd.setBackground(Color.green);
	  Ipc2dd.repaint();
D 11
	  et00_flag = 1;
E 11
I 11
	  et04_flag = 1;
E 11
	}
I 7
D 9
	et10_rate = proc_rate;
E 7
	et00_rate = proc_rate;
E 9
I 9
	//	et10_rate = proc_rate;
	et04_rate = proc_rate;
E 9
	try {
	  ipc2dd_rate.setText(Double.toString(proc_rate));
	} catch (PropertyVetoException e) {}
      } 
I 9

D 25
      if(process_name.equals("et2et_clasprod_clon10_clasprod_clon00-daq1")) {
E 25
I 25
      if(process_name.equals("et2et_clasprod_clon10_clasprod_clon00")) {
E 25
	if(process_status.equals("unknown")) {
I 23
	    /*
E 23
	  It2ipc.setBackground(Color.red);
	  It2ipc.repaint();
	  ET.setBackground(Color.red);
	  ET.repaint();
	  et10_flag = 0;
I 23
	    */
E 23
	  ET00.setBackground(Color.red);
	  ET00.repaint();
	  et00_flag = 0;
I 19
	  /*
	  online_recover.setEnabled(true);
	  online_recover.setBackground(Color.red);
	  online_recover.repaint();
	  */

E 19
	} else {
I 24
	    /*
E 24
	  It2ipc.setBackground(Color.green);
	  It2ipc.repaint();
	  ET.setBackground(Color.green);
	  ET.repaint();
	  et10_flag = 1;
I 24
	    */
E 24
	  ET00.setBackground(Color.green);
	  ET00.repaint();
	  et00_flag = 1;
	}
	et10_rate = proc_rate;
	e00_rate = proc_rate;
	try {
	  et00_rate.setText(Double.toString(proc_rate));
	} catch (PropertyVetoException e) {}
      } 
E 9
      
      
      if(process_name.equals("evstatmon_clasprod")) {
	if(process_status.equals("unknown")) {
	  Stadis.setBackground(Color.red);
	  Stadis.repaint();
I 17
	  /*
E 17
	  if(DataPool.auto_recover == 1) {
D 5
	  info_text.setText("Event Statistics Monitor is not running. I will try to restart it.");
D 3
	  mySysExe.run_unix("remsh -n clon00 /usr/local/clas/bin/event_statistics_monitor -a clasprod -s clasprod >>& /usr/local/clas/parms/run_log/event_statistics_monitor.log &");
E 3
I 3
	  mySysExe.run_unix("remsh -n clon00 $CLON_BIN/event_statistics_monitor -a clasprod -s clasprod >>& $CLON_PARMS/run_log/event_statistics_monitor.log &");
E 5
I 5
	    estmon--;
	    if(estmon<=0) {
D 17
	      info_text.setText("Event Statistics Monitor is not running. I will try to restart it.");
D 8
	      mySysExe.run_unix("remsh -n clon00 $CLON_BIN/event_statistics_monitor -a clasprod -s clasprod >>& $CLON_PARMS/run_log/event_statistics_monitor.log &");
E 8
I 8
D 9
	      mySysExe.run_unix("remsh -n clon04 $CLON_BIN/event_statistics_monitor -a clasprod -s clasprod >>& $CLON_PARMS/run_log/event_statistics_monitor.log &");
E 9
I 9
	      mySysExe.run_unix("remsh -n clon04 $CLON_BIN/event_statistics_monitor -a clasprod -s clasprod >>&! $CLON_PARMS/run_log/event_statistics_monitor.log &");
E 17
I 17
		info_text.setText("Event Statistics Monitor is not running. I will try to restart it.");
		mySysExe.run_unix("remsh -n clon04 $CLON_BIN/event_statistics_monitor -a clasprod -s clasprod >>&! $CLON_PARMS/run_log/event_statistics_monitor.log &");
E 17
E 9
E 8
	      estmon = DataPool.CheckIfUndefined[0];
	    }
E 5
E 3
	  }
I 17
	  */
I 19
	  evstatmon_flag = 0;

E 19
E 17
	} else {
	  Stadis.setBackground(Color.green);
	  Stadis.repaint();
I 19
	  evstatmon_flag = 1;
E 19
	}
      } 
      
      
      if(process_name.equals("scaler_server_clasprod")) {
	if(process_status.equals("unknown")) {
	  Scaler.setBackground(Color.red);
	  Scaler.repaint();
I 17
	  /*
E 17
	  if(DataPool.auto_recover == 1) {
D 5
	  info_text.setText("Scaler Server is not running. I will try to restart it.");
D 3
	  mySysExe.run_unix("remsh -n clon00 /usr/local/clas/bin/scaler_server -a clasprod -s clasprod >>& /usr/local/clas/parms/run_log/scaler_server.log &");
E 3
I 3
	  mySysExe.run_unix("remsh -n clon00 $CLON_BIN/scaler_server -a clasprod -s clasprod >>& $CLON_PARMS/run_log/scaler_server.log &");
E 5
I 5
	    ss--;
	    if(ss<=0) {
D 17
	      info_text.setText("Scaler Server is not running. I will try to restart it.");
D 8
	      mySysExe.run_unix("remsh -n clon00 $CLON_BIN/scaler_server -a clasprod -s clasprod >>& $CLON_PARMS/run_log/scaler_server.log &");
E 8
I 8
D 9
	      mySysExe.run_unix("remsh -n clon10 $CLON_BIN/scaler_server -a clasprod -s clasprod >>& $CLON_PARMS/run_log/scaler_server.log &");
E 9
I 9
	      mySysExe.run_unix("remsh -n clon10 $CLON_BIN/scaler_server -a clasprod -s clasprod >>&! $CLON_PARMS/run_log/scaler_server.log &");
E 17
I 17
		info_text.setText("Scaler Server is not running. I will try to restart it.");
		mySysExe.run_unix("remsh -n clon10 $CLON_BIN/scaler_server -a clasprod -s clasprod >>&! $CLON_PARMS/run_log/scaler_server.log &");
E 17
E 9
E 8
	      ss     = DataPool.CheckIfUndefined[2];
	    }
E 5
E 3
	  }
D 17
	  
E 17
I 17
	  */
I 19
	  scaler_server_flag = 0;
E 19
E 17
	} else {
	  Scaler.setBackground(Color.green);
	  Scaler.repaint();
I 19
	  scaler_server_flag = 1;
E 19
	}
      } 
      
D 8
      if(process_name.equals("trigmon_00_clasprod")) {
E 8
I 8
D 12
      if(process_name.equals("trigmon_04_clasprod")) {
E 12
I 12
      if(process_name.equals("trigmon_clasprod")) {
E 12
E 8
	if(process_status.equals("unknown")) {
    	  Trg_monitor.setBackground(Color.red);
	  Trg_monitor.repaint();
I 13
	  /*
E 13
	  if(DataPool.auto_recover == 1) {
D 5
	  info_text.setText("Trigmon started.");
D 3
	  mySysExe.run_unix("remsh -n clon00 /usr/local/clas/bin/start_trig_mon  >>& /usr/local/clas/parms/run_log/trigmon.log &");
E 3
I 3
	  mySysExe.run_unix("remsh -n clon00 $CLON_BIN/start_trig_mon  >>& $CLON_PARMS/run_log/trigmon.log &");
E 5
I 5
	    trgm--;
	    if(trgm<=0) {
	      info_text.setText("Trigmon started.");
D 8
	      mySysExe.run_unix("remsh -n clon00 $CLON_BIN/start_trig_mon  >>& $CLON_PARMS/run_log/trigmon.log &");
E 8
I 8
D 9
	      mySysExe.run_unix("remsh -n clon04 $CLON_BIN/start_trig_mon  >>& $CLON_PARMS/run_log/trigmon.log &");
E 9
I 9
	      mySysExe.run_unix("remsh -n clon04 $CLON_BIN/start_trig_mon  >>&! $CLON_PARMS/run_log/trigmon.log &");
E 9
E 8
	      trgm   = DataPool.CheckIfUndefined[3];
	    }	  
E 5
E 3
	  }
I 13
	  */
E 13
	} else {
	  Trg_monitor.setBackground(Color.green);
	  Trg_monitor.repaint();
	}
D 5
  }

E 5
I 5
      }
      
E 5
D 8
      if(process_name.equals("mon_00_clasprod")) {
E 8
I 8
D 12
      if(process_name.equals("mon_04_clasprod")) {
E 12
I 12
      if(process_name.equals("mon_clasprod")) {
E 12
E 8
	if(process_status.equals("unknown")) {
	  Evt_monitor.setBackground(Color.red);
	  Evt_monitor.repaint();
I 17
	  /*
E 17
	  if(DataPool.auto_recover == 1) {
D 5
	  info_text.setText("Event Monitor is not running. I will try to restart it.");
D 3
	  mySysExe.run_unix("remsh -n clon00 /usr/local/clas/bin/event_monitor -a clasprod -s clasprod -r -g MON -adir /hist/monitor -t /usr/local/clas/bin/event_monitor.init >>& /usr/local/clas/parms/run_log/monitor_clasprod.log &");
E 3
I 3
	  mySysExe.run_unix("remsh -n clon00 $CLON_BIN/event_monitor -a clasprod -s clasprod -r -g MON -adir /hist/monitor -t $CLON_BIN/event_monitor.init >>& $CLON_PARMS/run_log/monitor_clasprod.log &");
E 5
I 5
	    evtm--;
	    if(evtm<=0) {
D 17
	      info_text.setText("Event Monitor is not running. I will try to restart it.");
D 8
	      mySysExe.run_unix("remsh -n clon00 $CLON_BIN/event_monitor -a clasprod -s clasprod -r -g MON -adir /hist/monitor -t $CLON_BIN/event_monitor.init >>& $CLON_PARMS/run_log/monitor_clasprod.log &");
E 8
I 8
D 9
	      mySysExe.run_unix("remsh -n clon04 $CLON_BIN/event_monitor -a clasprod -s clasprod -r -g MON -adir /hist/monitor -t $CLON_BIN/event_monitor.init >>& $CLON_PARMS/run_log/monitor_clasprod.log &");
E 9
I 9
	      mySysExe.run_unix("remsh -n clon04 $CLON_BIN/event_monitor -a clasprod -s clasprod -r -g MON -adir /hist/monitor -t $CLON_BIN/event_monitor.init >>&! $CLON_PARMS/run_log/monitor_clasprod.log &");
E 17
I 17
		info_text.setText("Event Monitor is not running. I will try to restart it.");
		mySysExe.run_unix("remsh -n clon04 $CLON_BIN/event_monitor -a clasprod -s clasprod -r -g MON -adir /hist/monitor -t $CLON_BIN/event_monitor.init >>&! $CLON_PARMS/run_log/monitor_clasprod.log &");
E 17
E 9
E 8
	      evtm   = DataPool.CheckIfUndefined[4];
	    }  
E 5
E 3
	  }
I 17
	  */
I 19
	  mon_flag = 0;
E 19
E 17
	} else {
	  Evt_monitor.setBackground(Color.green);
	  Evt_monitor.repaint();
I 19
	  mon_flag = 1;
E 19
	}
	try {
	  evmon_rate.setText(Double.toString(proc_rate));
	} catch (PropertyVetoException e) {}
      } 
      
      if(process_name.equals("recsis_clasprod")) {
	if(process_status.equals("unknown")) {
	  Recsis.setBackground(Color.red);
	  Recsis.repaint();
I 17
	  /*
E 17
D 5
	  recsis_restart = recsis_restart -1;
	  if(recsis_restart <= 0) {
E 5
	  if(DataPool.auto_recover == 1) {
D 5
	  info_text.setText("Recsis is not running. I will try to restart it.");
//	  mySysExe.run_unix("ipc_control -a clasprod -d recsis_clasprod quit >& /dev/null &");
D 3
	  mySysExe.run_unix("remsh -n clon00 /usr/local/clas/bin/run_online_recsis clasprod >>& /usr/local/clas/parms/run_log/recon.log &");
E 3
I 3
	  mySysExe.run_unix("remsh -n clon00 $CLON_BIN/run_online_recsis clasprod >>& $CLON_PARMS/run_log/recon.log &");
E 5
I 5
	    recs--;
	    if(recs<=0) {
D 16
	      info_text.setText("Recsis is not running. I will try to restart it.");
	      //	  mySysExe.run_unix("ipc_control -a clasprod -d recsis_clasprod quit >& /dev/null &");
D 8
	      mySysExe.run_unix("remsh -n clon00 $CLON_BIN/run_online_recsis clasprod >>& $CLON_PARMS/run_log/recon.log &");
E 8
I 8
D 9
	      mySysExe.run_unix("remsh -n clon04 $CLON_BIN/run_online_recsis clasprod >>& $CLON_PARMS/run_log/recon.log &");
E 9
I 9
	      mySysExe.run_unix("remsh -n clon04 $CLON_BIN/run_online_recsis clasprod >>&! $CLON_PARMS/run_log/recon.log &");
E 16
I 16
D 17
		//info_text.setText("Recsis is not running. I will try to restart it.");
E 17
I 17
	       info_text.setText("Recsis is not running. I will try to restart it.");
E 17
	      //old staff	  mySysExe.run_unix("ipc_control -a clasprod -d recsis_clasprod quit >& /dev/null &");
D 17
		//mySysExe.run_unix("remsh -n clon04 $CLON_BIN/run_online_recsis clasprod >>&! $CLON_PARMS/run_log/recon.log &");
E 17
I 17
		mySysExe.run_unix("remsh -n clon04 $CLON_BIN/run_online_recsis clasprod >>&! $CLON_PARMS/run_log/recon.log &");
E 17
E 16
E 9
E 8
	      recs   = DataPool.CheckIfUndefined[5];
	    }
E 5
E 3
	  }
I 17
	  */
E 17
D 5
	  recsis_restart = 7;
  }	  
	  } else {
E 5
I 5
	} else {
E 5
          Recsis.setBackground(Color.green);
	  Recsis.repaint();
D 5
	  recsis_restart = 3;
	  }
      
      try {
	recsis_rate.setText(Double.toString(proc_rate));
E 5
I 5
	}
	
	try {
	  recsis_rate.setText(Double.toString(proc_rate));
E 5
	} catch (PropertyVetoException e) {}
      } 
    }
D 11
    //System.out.println(et10_flag+" "+et00_flag+" "+et10_rate+" "+et00_rate);
E 11
I 11
D 20
    //System.out.println(et10_flag+" "+et00_flag+" "+et04_flag+" "+et10_rate+" "+et00_rate+" "+et04_rate);
E 20
I 20
D 21
    System.out.println(eb_evtrate+" "+et04_rate+" "+etsys04_flag);
E 21
I 21
    //System.out.println(eb_evtrate+" "+et04_rate+" "+etsys04_flag);
E 21
E 20
E 11
D 9
    if (((et10_flag==0)&&(et00_flag==0)) || ((et10_flag==1)&&(et00_flag==0)) || ((et10_rate>0)&&(et00_rate<=0))) {
E 9
I 9
D 19
    if ( (et10_flag==0) || (et00_flag==0) || (et04_flag==0) || ((et10_rate>0)&&(e00_rate<=0)) || ((et10_rate>0)&&(et04_rate<=0)) ) {
E 9
      online_recover.setEnabled(true);
      online_recover.setBackground(Color.red);
      online_recover.repaint();
E 19
I 19
    //    if ( (et10_flag==0) || (et00_flag==0) || (et04_flag==0) || ((et10_rate>0)&&(e00_rate<=0)) || ((et10_rate>0)&&(et04_rate<=0)) || (evstatmon_flag==0) || (scaler_server_flag ==0) || (mon_flag == 0)) {
D 20
    if ( ((et10_rate>0)&&(et04_rate<=0)) || (etsys04_flag ==0) ) {      
E 20
I 20
    if ( ((eb_evtrate>0)&&(et04_rate<=0)) || (etsys04_flag ==0) ) {      
E 20
	online_recover.setEnabled(true);
	online_recover.setBackground(Color.red);
	online_recover.repaint();
E 19
    }
    else {
      online_recover.setBackground(new Color(-3355444));
      online_recover.setEnabled(false);
      online_recover.repaint();
    }
  }
D 19
  
E 19
I 19
    //==========================================================================
  public void visual_online2(TipcMsg msg) {
      String message = null;
    try {
    
	while( (message = msg.nextStr())!=null) {
	    if(message.equals("etsys_04_clasprod")){
		//		System.out.println("HEYHEY => " +message);
		return;
	    }
	}
    } catch (TipcException e) {} 
D 24
    if(message.equals("etsys_04_clasprod")){ etsys04_flag = 1;}
    else {
E 24
I 24
    if(message.equals("etsys_04_clasprod")){ 
	  It2ipc.setBackground(Color.green);
	  It2ipc.repaint();
	  etsys04_flag = 1;
} else {
	It2ipc.setBackground(Color.red);
	It2ipc.repaint();
E 24
	etsys04_flag = 0;
    }
  }
    
    //==========================================================================  

E 19
  public void visual_scaler(TipcMsg msg) {

    double l1InputRates [][] = null;
    double acceptedRates [][] = null;
    double LiveTime [][] = null;
    double l2sec [][] = null;

    
    //TS input and output rate statistics and livetime bars
    try { 
      l1InputRates = new double[2][12];
      acceptedRates = new double[2][12];
      LiveTime = new double[2][3];
      l2sec = new double[2][6];

    } catch (Exception e) {}
    for(int i=0; i<=11;i++) {
      l1InputRates[0][i] = i+1;
      acceptedRates[0][i] = i+1;
    }
    for(int i=0; i<=2;i++)LiveTime[0][i]=i+1; 
    for(int i=0; i<=5;i++)l2sec[0][i]=i+1; 
    for(int i=0; i<=2;i++)l2pf[0][i]=i+1; 
    

      try{	    
	for (;;) {
	//get the bank name ( message name )
	String bank_name = msg.nextStr();
	//System.out.println("DEBUG-------------> " +bank_name); 
	//is it TRGS?
	if(bank_name.equals("TRGS00")) {
	  trgs = msg.nextInt4Array();
	  for(int j=0; j<80; j++) {
	    if(trgs[j]<0) ltrgs[j] = (trgs[j] & 2147483647) | (Long.valueOf("2147483648").longValue());
	    else ltrgs[j] = trgs[j];
	    ltrgs_diff[j] = ltrgs[j] - ltrgs_prev[j];
	    ltrgs_prev[j] = ltrgs[j];
	  }
	  //integral live-time
	  /*	  
		  live_trig  = (ltrgs[60]>0)?(float)ltrgs[61]/ltrgs[60]:0.0;
		  live_clock = (ltrgs[16]>0)?(float)ltrgs[32]/ltrgs[16]:0.0;
		  live_fcup  = (ltrgs[17]>0)?(float)ltrgs[33]/ltrgs[17]:0.0;
		  */
	  
	  //instantaneous live-time
	  live_trig  = (ltrgs_diff[60]>0)?(float)ltrgs_diff[61]/ltrgs_diff[60]:0.0;
	  live_clock = (ltrgs_diff[16]>0)?(float)ltrgs_diff[32]/ltrgs_diff[16]:0.0;
	  live_fcup  = (ltrgs_diff[17]>0)?(float)ltrgs_diff[33]/ltrgs_diff[17]:0.0;
	  
	  //Fill the LiveTime bar chart array
	  LiveTime[1][0] = live_trig;
	  LiveTime[1][1] = live_clock;
	  LiveTime[1][2] = live_fcup;
	  
	  ChartDataView arrlt =   roc_fe_buffer.getDataView(0);
	  arrlt.setDataSource(new ArrayData(LiveTime,3,stgraph));
	  ChartDataViewSeries seriesListlt[] = arrlt.getSeries();
	  seriesListlt[0].setLabel("T= "+Integer.toString((int)(live_trig *100)) + " C= " +Integer.toString((int)(live_clock *100)) + " F= " +Integer.toString((int)(live_fcup *100)));
	  roc_fe_buffer.getDataView(0).getSeries(0).getStyle().getFillStyle().setColor(new Color(32896));
	  
	  triglive[1][0] = live_trig*100;
	  triglive[2][0] = (100.0 - live_trig*100);
	  
	  clocklive[1][0] = live_clock*100;
	  clocklive[2][0] = (100.0 - live_clock*100);
	  
	  //calculate level1 input rates
	  try {
	    if (ltrgs_diff[30] > 0) {
	      //level1 input rate numbers
D 10
	      l1input1.setText(fmt.format((ltrgs_diff[48]*10000.0/ltrgs_diff[30])/1000.0));
	      l1input2.setText(fmt.format((ltrgs_diff[49]*10000.0/ltrgs_diff[30])/1000.0));
	      l1input3.setText(fmt.format((ltrgs_diff[50]*10000.0/ltrgs_diff[30])/1000.0));
	      l1input4.setText(fmt.format((ltrgs_diff[51]*10000.0/ltrgs_diff[30])/1000.0));
	      l1input5.setText(fmt.format((ltrgs_diff[52]*10000.0/ltrgs_diff[30])/1000.0));
	      l1input6.setText(fmt.format((ltrgs_diff[53]*10000.0/ltrgs_diff[30])/1000.0));
	      l1input7.setText(fmt.format((ltrgs_diff[54]*10000.0/ltrgs_diff[30])/1000.0));
	      l1input8.setText(fmt.format((ltrgs_diff[55]*10000.0/ltrgs_diff[30])/1000.0));
	      // l1input9.setText(fmt.format((ltrgs_diff[56]*10000.0/ltrgs_diff[30])/1000.0));
	      l1input10.setText(fmt.format((ltrgs_diff[57]*10000.0/ltrgs_diff[30])/1000.0));
	      // l1input11.setText(fmt.format((ltrgs_diff[58]*10000.0/ltrgs_diff[30])/1000.0));
	      l1input12.setText(fmt.format((ltrgs_diff[59]*10000.0/ltrgs_diff[30])/1000.0));
E 10
I 10
	      l1input1.setText(fmt.format((ltrgs_diff[48]*100000.0/ltrgs_diff[30])/1000.0));
	      l1input2.setText(fmt.format((ltrgs_diff[49]*100000.0/ltrgs_diff[30])/1000.0));
	      l1input3.setText(fmt.format((ltrgs_diff[50]*100000.0/ltrgs_diff[30])/1000.0));
	      l1input4.setText(fmt.format((ltrgs_diff[51]*100000.0/ltrgs_diff[30])/1000.0));
	      l1input5.setText(fmt.format((ltrgs_diff[52]*100000.0/ltrgs_diff[30])/1000.0));
	      l1input6.setText(fmt.format((ltrgs_diff[53]*100000.0/ltrgs_diff[30])/1000.0));
	      l1input7.setText(fmt.format((ltrgs_diff[54]*100000.0/ltrgs_diff[30])/1000.0));
	      l1input8.setText(fmt.format((ltrgs_diff[55]*100000.0/ltrgs_diff[30])/1000.0));
	      // l1input9.setText(fmt.format((ltrgs_diff[56]*100000.0/ltrgs_diff[30])/1000.0));
	      l1input10.setText(fmt.format((ltrgs_diff[57]*100000.0/ltrgs_diff[30])/1000.0));
	      // l1input11.setText(fmt.format((ltrgs_diff[58]*100000.0/ltrgs_diff[30])/1000.0));
	      l1input12.setText(fmt.format((ltrgs_diff[59]*100000.0/ltrgs_diff[30])/1000.0));
E 10
	      // Fill level1 input array
D 10
	      l1InputRates[1][0] = ltrgs_diff[48]*10000.0/ltrgs_diff[30];
	      l1InputRates[1][1] = ltrgs_diff[49]*10000.0/ltrgs_diff[30];
	      l1InputRates[1][2] = ltrgs_diff[50]*10000.0/ltrgs_diff[30];
	      l1InputRates[1][3] = ltrgs_diff[51]*10000.0/ltrgs_diff[30];
	      l1InputRates[1][4] = ltrgs_diff[52]*10000.0/ltrgs_diff[30];
	      l1InputRates[1][5] = ltrgs_diff[53]*10000.0/ltrgs_diff[30];
	      l1InputRates[1][6] = ltrgs_diff[54]*10000.0/ltrgs_diff[30];
	      l1InputRates[1][7] = ltrgs_diff[55]*10000.0/ltrgs_diff[30];
	      l1InputRates[1][8] = ltrgs_diff[56]*10000.0/ltrgs_diff[30];
	      l1InputRates[1][9] = ltrgs_diff[57]*10000.0/ltrgs_diff[30];
	      l1InputRates[1][10] = ltrgs_diff[58]*10000.0/ltrgs_diff[30];
	      l1InputRates[1][11] = ltrgs_diff[59]*10000.0/ltrgs_diff[30];
E 10
I 10
	      l1InputRates[1][0] = ltrgs_diff[48]*100000.0/ltrgs_diff[30];
	      l1InputRates[1][1] = ltrgs_diff[49]*100000.0/ltrgs_diff[30];
	      l1InputRates[1][2] = ltrgs_diff[50]*100000.0/ltrgs_diff[30];
	      l1InputRates[1][3] = ltrgs_diff[51]*100000.0/ltrgs_diff[30];
	      l1InputRates[1][4] = ltrgs_diff[52]*100000.0/ltrgs_diff[30];
	      l1InputRates[1][5] = ltrgs_diff[53]*100000.0/ltrgs_diff[30];
	      l1InputRates[1][6] = ltrgs_diff[54]*100000.0/ltrgs_diff[30];
	      l1InputRates[1][7] = ltrgs_diff[55]*100000.0/ltrgs_diff[30];
	      l1InputRates[1][8] = ltrgs_diff[56]*100000.0/ltrgs_diff[30];
	      l1InputRates[1][9] = ltrgs_diff[57]*100000.0/ltrgs_diff[30];
	      l1InputRates[1][10] = ltrgs_diff[58]*100000.0/ltrgs_diff[30];
	      l1InputRates[1][11] = ltrgs_diff[59]*100000.0/ltrgs_diff[30];
E 10
	      
	      // pass/fail/clear
	      
D 10
	  l2pf[1][0] = ltrgs_diff[65]*10000.0/ltrgs_diff[30];
	  l2pf[1][1] = ltrgs_diff[64]*10000.0/ltrgs_diff[30];
	  l2pf[1][2] = ltrgs_diff[67]*10000.0/ltrgs_diff[30];    
E 10
I 10
	  l2pf[1][0] = ltrgs_diff[65]*100000.0/ltrgs_diff[30];
	  l2pf[1][1] = ltrgs_diff[64]*100000.0/ltrgs_diff[30];
	  l2pf[1][2] = ltrgs_diff[67]*100000.0/ltrgs_diff[30];    
E 10

	  //sector based L2
D 10
	    l2sec[1][0] = ltrgs_diff[72]*10000.0/ltrgs_diff[30];
	    l2sec[1][1] = ltrgs_diff[73]*10000.0/ltrgs_diff[30];
	    l2sec[1][2] = ltrgs_diff[74]*10000.0/ltrgs_diff[30];
	    l2sec[1][3] = ltrgs_diff[75]*10000.0/ltrgs_diff[30];
	    l2sec[1][4] = ltrgs_diff[76]*10000.0/ltrgs_diff[30];
	    l2sec[1][5] = ltrgs_diff[77]*10000.0/ltrgs_diff[30];
E 10
I 10
	    l2sec[1][0] = ltrgs_diff[72]*100000.0/ltrgs_diff[30];
	    l2sec[1][1] = ltrgs_diff[73]*100000.0/ltrgs_diff[30];
	    l2sec[1][2] = ltrgs_diff[74]*100000.0/ltrgs_diff[30];
	    l2sec[1][3] = ltrgs_diff[75]*100000.0/ltrgs_diff[30];
	    l2sec[1][4] = ltrgs_diff[76]*100000.0/ltrgs_diff[30];
	    l2sec[1][5] = ltrgs_diff[77]*100000.0/ltrgs_diff[30];
E 10


	  //ploting pass/fail/clear
	  ChartDataView l2pfdat = level2_pass_fail.getDataView(0);    
	  l2pfdat.setDataSource(new ArrayData(l2pf,3,stgraph));
	  ChartDataViewSeries seriesl2pf[] = l2pfdat.getSeries();
	  seriesl2pf[0].setLabel(Integer.toString((int)(l2pf[1][0])) + " " +Integer.toString((int)(l2pf[1][1])) + " " +Integer.toString((int)(l2pf[1][2])));
	  level2_pass_fail.getDataView(0).getSeries(0).getStyle().getFillStyle().setColor(new Color(32960));
	  

  	  // Ploting sector based level2 data
	  ChartDataView l2secdat =   level2_sectors.getDataView(0);
	  l2secdat.setDataSource(new ArrayData(l2sec,6,stgraph));
	  ChartDataViewSeries seriesl2sec[] = l2secdat.getSeries();
	  seriesl2sec[0].setLabel("Sector Based Level2.");
	  level2_sectors.getDataView(0).getSeries(0).getStyle().getFillStyle().setColor(new Color(32960));


	      //Plot the level1 input rates
	      ChartDataView arrdr =   roc_data_rate.getDataView(0);
	      arrdr.setDataSource(new ArrayData(l1InputRates,12,stgraph));
	      ChartDataViewSeries seriesListdr[] = arrdr.getSeries();
	      seriesListdr[0].setLabel("Level1 Input Rates");
	      roc_data_rate.getDataView(0).getSeries(0).getStyle().getFillStyle().setColor(new Color(32960));
	    }
	  } catch(java.beans.PropertyVetoException e) { }
	  //exit the loop for ever
	} //here is end of TRGS00 bank readout

	/*      Reading s1st bank	
	//is it S1ST?
	else if (bank_name.equals("S1ST00")) {
	  s1st = msg.nextInt4Array();
	  if(s1st[12] > 0){
	    l2sec[1][0] = s1st[17];
	    l2sec[1][1] = s1st[18];
	    l2sec[1][2] = s1st[19];
	    l2sec[1][3] = s1st[20];
	    l2sec[1][4] = s1st[21];
	    l2sec[1][5] = s1st[22];
	  }

	  
	  l2pf[1][0] = s1st[23];
	  l2pf[1][1] = s1st[24];
	  l2pf[1][2] = 0;    
	  //ploting pass/fail/clear
	  ChartDataView l2pfdat = level2_pass_fail.getDataView(0);    
	  l2pfdat.setDataSource(new ArrayData(l2pf,3,stgraph));
	  ChartDataViewSeries seriesl2pf[] = l2pfdat.getSeries();
	  seriesl2pf[0].setLabel("Level2 Pass/Fail/Clear");
	  level2_pass_fail.getDataView(0).getSeries(0).getStyle().getFillStyle().setColor(new Color(32960));
	  
  	  // Ploting level2 data
	  ChartDataView l2secdat =   level2_sectors.getDataView(0);
	  l2secdat.setDataSource(new ArrayData(l2sec,6,stgraph));
	  ChartDataViewSeries seriesl2sec[] = l2secdat.getSeries();
	  seriesl2sec[0].setLabel("Sector Based Level2.");
	  level2_sectors.getDataView(0).getSeries(0).getStyle().getFillStyle().setColor(new Color(32960));
	  */
	
	else {
 trgs = msg.nextInt4Array();
	}
	



	} // end of for loop
      } catch (TipcException e) {
      }
      
  }
  


  public void visual_raid(TipcMsg msg) {
    String raidnum = null;
    try {
I 15
	msg.setCurrent(8);
E 15
      String raid1_status = msg.nextStr();
      //	System.out.println(" DEBUG--> " +raid1_status );

	if(raid1_status.indexOf("NOT BACKED UP") >=0) {
	    raid1.setBackground( new Color(16711935)); 
	    raid1.repaint();
	    raid1silo.setForeground(Color.black);
	    raid1silo.repaint();
	    raid1_line.setForeground(Color.black);
	    raid1_line.repaint();
	  }
	  else if(raid1_status.indexOf("BACKED UP")>=0) {  
	    raid1.setBackground( new Color(32960));
	    raid1.repaint();
	    raid1silo.setForeground(Color.black);
	    raid1silo.repaint();
	    raid1_line.setForeground(Color.black);
	    raid1_line.repaint();
	  }
	else if(raid1_status.equals("MOVING")) {
	  raid1.setBackground(Color.yellow);
	  raid1.repaint();
	  raid1silo.setForeground(Color.yellow);
	  raid1silo.repaint();
	}
	else if(raid1_status.equals("ACTIVE")) { 
	  raid1.setBackground(Color.green);
	  raid1.repaint();
	  raid1_line.setForeground(Color.green);
	  raid1_line.repaint();
	}
	else { 
	  raid1.setBackground(Color.cyan);
	  raid1.repaint();
	  raid1silo.setForeground(Color.black);
	  raid1silo.repaint();
	  raid1_line.setForeground(Color.black);
	  raid1_line.repaint();
	}

      String raid2_status = msg.nextStr();
	if(raid2_status.indexOf("NOT BACKED UP") >=0) {
	    raid2.setBackground( new Color(16711935)); 
	    raid2.repaint();
	    raid2silo.setForeground(Color.black);
	    raid2silo.repaint();
	    raid2_line.setForeground(Color.black);
	    raid2_line.repaint();
	  }
	  else if(raid2_status.indexOf("BACKED UP")>=0) {  
	    raid2.setBackground( new Color(32960));
	    raid2.repaint();
	    raid2silo.setForeground(Color.black);
	    raid2silo.repaint();
	    raid2_line.setForeground(Color.black);
	    raid2_line.repaint();
	  }
	else if(raid2_status.equals("MOVING")) {
	  raid2.setBackground(Color.yellow);
	  raid2.repaint();
	  raid2silo.setForeground(Color.yellow);
	  raid2silo.repaint();
	}
	else if(raid2_status.equals("ACTIVE")) { 
	  raid2.setBackground(Color.green);
	  raid2.repaint();
	  raid2_line.setForeground(Color.green);
	  raid2_line.repaint();
	}
	else { 
	  raid2.setBackground(Color.cyan);
	  raid2.repaint();
	  raid2silo.setForeground(Color.black);
	  raid2silo.repaint();
	  raid2_line.setForeground(Color.black);
	  raid2_line.repaint();
	}

      String raid3_status = msg.nextStr();
	if(raid3_status.indexOf("NOT BACKED UP") >=0) {
	    raid3.setBackground( new Color(16711935)); 
	    raid3.repaint();
	    raid3silo.setForeground(Color.black);
	    raid3silo.repaint();
	    raid3_line.setForeground(Color.black);
	    raid3_line.repaint();
	  }
	  else if(raid3_status.indexOf("BACKED UP")>=0) {  
	    raid3.setBackground( new Color(32960));
	    raid3.repaint();
	    raid3silo.setForeground(Color.black);
	    raid3silo.repaint();
	    raid3_line.setForeground(Color.black);
	    raid3_line.repaint();
	  }
	else if(raid3_status.equals("MOVING")) {
	  raid3.setBackground(Color.yellow);
	  raid3.repaint();
	  raid3silo.setForeground(Color.yellow);
	  raid3silo.repaint();
	}
	else if(raid3_status.equals("ACTIVE")) { 
	  raid3.setBackground(Color.green);
	  raid3.repaint();
	  raid3_line.setForeground(Color.green);
	  raid3_line.repaint();
	}
	else { 
	  raid3.setBackground(Color.cyan);
	  raid3.repaint();
	  raid3silo.setForeground(Color.black);
	  raid3silo.repaint();
	  raid3_line.setForeground(Color.black);
	  raid3_line.repaint();
	}

      String raid4_status = msg.nextStr();
	if(raid4_status.indexOf("NOT BACKED UP") >=0) {
	    raid4.setBackground( new Color(16711935)); 
	    raid4.repaint();
	    raid4silo.setForeground(Color.black);
	    raid4silo.repaint();
	    raid4_line.setForeground(Color.black);
	    raid4_line.repaint();
	  }
	  else if(raid4_status.indexOf("BACKED UP")>=0) {  
	    raid4.setBackground( new Color(32960));
	    raid4.repaint();
	    raid4silo.setForeground(Color.black);
	    raid4silo.repaint();
	    raid4_line.setForeground(Color.black);
	    raid4_line.repaint();
	  }
	else if(raid4_status.equals("MOVING")) {
	  raid4.setBackground(Color.yellow);
	  raid4.repaint();
	  raid4silo.setForeground(Color.yellow);
	  raid4silo.repaint();
	}
	else if(raid4_status.equals("ACTIVE")) { 
	  raid4.setBackground(Color.green);
	  raid4.repaint();
	  raid4_line.setForeground(Color.green);
	  raid4_line.repaint();
	}
	else { 
	  raid4.setBackground(Color.cyan);
	  raid4.repaint();
	  raid4silo.setForeground(Color.black);
	  raid4silo.repaint();
	  raid4_line.setForeground(Color.black);
	  raid4_line.repaint();
	}

      String raid5_status = msg.nextStr();
	if(raid5_status.indexOf("NOT BACKED UP") >=0) {
	    raid5.setBackground( new Color(16711935)); 
	    raid5.repaint();
	    raid5silo.setForeground(Color.black);
	    raid5silo.repaint();
	    raid5_line.setForeground(Color.black);
	    raid5_line.repaint();
	  }
	  else if(raid5_status.indexOf("BACKED UP")>=0) {  
	    raid5.setBackground( new Color(32960));
	    raid5.repaint();
	    raid5silo.setForeground(Color.black);
	    raid5silo.repaint();
	    raid5_line.setForeground(Color.black);
	    raid5_line.repaint();
	  }
	else if(raid5_status.equals("MOVING")) {
	  raid5.setBackground(Color.yellow);
	  raid5.repaint();
	  raid5silo.setForeground(Color.yellow);
	  raid5silo.repaint();
	}
	else if(raid5_status.equals("ACTIVE")) { 
	  raid5.setBackground(Color.green);
	  raid5.repaint();
	  raid5_line.setForeground(Color.green);
	  raid5_line.repaint();
	}
	else { 
	  raid5.setBackground(Color.cyan);
	  raid5.repaint();
	  raid5silo.setForeground(Color.black);
	  raid5silo.repaint();
	  raid5_line.setForeground(Color.black);
	  raid5_line.repaint();
	}

      String raid6_status = msg.nextStr();
	if(raid6_status.indexOf("NOT BACKED UP") >=0) {
	    raid6.setBackground( new Color(16711935)); 
	    raid6.repaint();
	    raid6silo.setForeground(Color.black);
	    raid6silo.repaint();
	    raid6_line.setForeground(Color.black);
	    raid6_line.repaint();
	  }
	  else if(raid6_status.indexOf("BACKED UP")>=0) {  
	    raid6.setBackground( new Color(32960));
	    raid6.repaint();
	    raid6silo.setForeground(Color.black);
	    raid6silo.repaint();
	    raid6_line.setForeground(Color.black);
	    raid6_line.repaint();
	  }
	else if(raid6_status.equals("MOVING")) {
	  raid6.setBackground(Color.yellow);
	  raid6.repaint();
	  raid6silo.setForeground(Color.yellow);
	  raid6silo.repaint();
	}
	else if(raid6_status.equals("ACTIVE")) { 
	  raid6.setBackground(Color.green);
	  raid6.repaint();
	  raid6_line.setForeground(Color.green);
	  raid6_line.repaint();
	}
	else { 
	  raid6.setBackground(Color.cyan);
	  raid6.repaint();
	  raid6silo.setForeground(Color.black);
	  raid6silo.repaint();
	  raid6_line.setForeground(Color.black);
	  raid6_line.repaint();
	}

I 15
      String raid7_status = msg.nextStr();
	if(raid7_status.indexOf("NOT BACKED UP") >=0) {
	    raid7.setBackground( new Color(16711935)); 
	    raid7.repaint();
	    raid7silo.setForeground(Color.black);
	    raid7silo.repaint();
	    raid7_line.setForeground(Color.black);
	    raid7_line.repaint();
	  }
	  else if(raid7_status.indexOf("BACKED UP")>=0) {  
	    raid7.setBackground( new Color(32960));
	    raid7.repaint();
	    raid7silo.setForeground(Color.black);
	    raid7silo.repaint();
	    raid7_line.setForeground(Color.black);
	    raid7_line.repaint();
	  }
	else if(raid7_status.equals("MOVING")) {
	  raid7.setBackground(Color.yellow);
	  raid7.repaint();
	  raid7silo.setForeground(Color.yellow);
	  raid7silo.repaint();
	}
	else if(raid7_status.equals("ACTIVE")) { 
	  raid7.setBackground(Color.green);
	  raid7.repaint();
	  raid7_line.setForeground(Color.green);
	  raid7_line.repaint();
	}
	else { 
	  raid7.setBackground(Color.cyan);
	  raid7.repaint();
	  raid7silo.setForeground(Color.black);
	  raid7silo.repaint();
	  raid7_line.setForeground(Color.black);
	  raid7_line.repaint();
	}

      String raid8_status = msg.nextStr();
	if(raid8_status.indexOf("NOT BACKED UP") >=0) {
	    raid8.setBackground( new Color(16711935)); 
	    raid8.repaint();
	    raid8silo.setForeground(Color.black);
	    raid8silo.repaint();
	    raid8_line.setForeground(Color.black);
	    raid8_line.repaint();
	  }
	  else if(raid8_status.indexOf("BACKED UP")>=0) {  
	    raid8.setBackground( new Color(32960));
	    raid8.repaint();
	    raid8silo.setForeground(Color.black);
	    raid8silo.repaint();
	    raid8_line.setForeground(Color.black);
	    raid8_line.repaint();
	  }
	else if(raid8_status.equals("MOVING")) {
	  raid8.setBackground(Color.yellow);
	  raid8.repaint();
	  raid8silo.setForeground(Color.yellow);
	  raid8silo.repaint();
	}
	else if(raid8_status.equals("ACTIVE")) { 
	  raid8.setBackground(Color.green);
	  raid8.repaint();
	  raid8_line.setForeground(Color.green);
	  raid8_line.repaint();
	}
	else { 
	  raid8.setBackground(Color.cyan);
	  raid8.repaint();
	  raid8silo.setForeground(Color.black);
	  raid8silo.repaint();
	  raid8_line.setForeground(Color.black);
	  raid8_line.repaint();
	}

E 15
    } catch (TipcException e) {
    }
  }
  
  public void visual_run(TipcMsg msg) {
    try {
      int rn = msg.nextInt4();
      try {
	run_number.setText(Integer.toString(rn));
      } catch(java.beans.PropertyVetoException e) { } 
    } catch (TipcException e) {}
  }
  
  

  public static void main(String argv[]) {
    //...........................initializing the IPC
    try {
      ipc_init.set_project("clasprod");
D 27
      ipc_init.set_server_names("clon10,clon00,jlabs2,db6");
E 27
I 27
      ipc_init.set_server_names("clon10,clon00,clon05");
E 27
      srv=ipc_init.init("daqtlc","daqtlc");
    } catch (Exception e) {
      Tut.exitFailure(e.toString());
    }
    new Thread ( new Daqtlc() ).start();


  }




public class DTsource_clt extends ObserverAdapter implements 
HasDataSource, Rebinnable1DHistogramData, HasStyle
{

  private int SECONDS = 60;
  
  private String m_title;


  public DTsource_clt()
    {
     
      super(t_clt);
      m_title = "Clock";
    }
  public DTsource_clt(String theTitle)
    {
      super(t_clt);
      m_title = theTitle;
    }
  public DataSource getDataSource(String param) {
    return new DTsource_clt(param);
  }
  public double[][] rebin(int nbin, double min, double max, boolean wantErrors, boolean hurry)
    {
      double[] result = t_clt.getSnapshot();
      double[][] r = { result };
      return r;
    }
  public double getMin()
    {
      return -SECONDS;
    }
  public double getMax()
    {
      return 0;
    }
  public int getBins()
    {
      return SECONDS;
    }
  public boolean isRebinnable()
    {
      return false;
    }
  public int getAxisType()
    {
      return Rebinnable1DHistogramData.DOUBLE;
    }
  public String[] getAxisLabels()
    {
      return null;
    }
  public Statistics getStatistics()
    {
      return null;
    }
  public String getTitle()
    {
      return m_title;
    }
  public JASHistStyle getStyle()
    {
      JASHist1DHistogramStyle style = new JASHist1DHistogramStyle();
      style.setShowErrorBars(false);
      style.setShowDataPoints(true);
      style.setShowLinesBetweenPoints(true);
      style.setShowHistogramBars(false);
      return style;
    }

}  

  public class clt_Thread extends Observable implements Runnable
  { 
    private int SECONDS = 60;
    private Thread thread;
    private int index = 0;
    private double[] bins = new double[SECONDS];
    private int thread_sleep = 10000;
    private int k = 0;

    clt_Thread()
      {
	thread = new Thread(this);
	thread.start();
      }
    public void run()
      {

	    for (;;)
	      {
		synchronized (this)
		  {
		    bins[index++] = live_clock*100.0;
		    if (index>=SECONDS) index = 0; 
		  }
		setChanged();
		notifyObservers(hdr_clt);
		try {
		thread.sleep(thread_sleep);            
		} catch (InterruptedException e) {}
	      }
	  }
  
    synchronized double[] getSnapshot()
      {
	double[] result = new double[SECONDS];
	int j=0;
	for (int i=index; i<SECONDS; i++) result[j++] = bins[i];
	for (int i=0; i<index; i++) result[j++] = bins[i];
	return result;
      }
  }

public class DTsource_tlt extends ObserverAdapter implements 
HasDataSource, Rebinnable1DHistogramData, HasStyle
{

  private int SECONDS = 60;
  
  private String m_title;


  public DTsource_tlt()
    {
     
      super(t_tlt);
      m_title = "Trigger";
    }
  public DTsource_tlt(String theTitle)
    {
      super(t_tlt);
      m_title = theTitle;
    }
  public DataSource getDataSource(String param) {
    return new DTsource_tlt(param);
  }
  public double[][] rebin(int nbin, double min, double max, boolean wantErrors, boolean hurry)
    {
      double[] result = t_tlt.getSnapshot();
      double[][] r = { result };
      return r;
    }
  public double getMin()
    {
      return -60;
    }
  public double getMax()
    {
      return 0;
    }
  public int getBins()
    {
      return SECONDS;
    }
  public boolean isRebinnable()
    {
      return false;
    }
  public int getAxisType()
    {
      return Rebinnable1DHistogramData.DOUBLE;
    }
  public String[] getAxisLabels()
    {
      return null;
    }
  public Statistics getStatistics()
    {
      return null;
    }
  public String getTitle()
    {
      return m_title;
    }
  public JASHistStyle getStyle()
    {
      JASHist1DHistogramStyle style = new JASHist1DHistogramStyle();
      style.setShowErrorBars(false);
      style.setShowDataPoints(true);
      style.setShowLinesBetweenPoints(true);
      style.setShowHistogramBars(false);
      return style;
    }

}  

  public class tlt_Thread extends Observable implements Runnable
  { 
    private int SECONDS = 60;
    private Thread thread;
    private int index = 0;
    private double[] bins = new double[SECONDS];
    private int thread_sleep = 10000;
    private int k = 0;

    tlt_Thread()
      {
	thread = new Thread(this);
	thread.start();
      }
    public void run()
      {

	    for (;;)
	      {
		synchronized (this)
		  {
		    bins[index++] = live_trig*100.0;
		    if (index>=SECONDS) index = 0; 
		  }
		setChanged();
		notifyObservers(hdr_tlt);
		try {
		thread.sleep(thread_sleep);            
		} catch (InterruptedException e) {}
	      }
	  }

    synchronized double[] getSnapshot()
      {
	double[] result = new double[SECONDS];
	int j=0;
	for (int i=index; i<SECONDS; i++) result[j++] = bins[i];
	for (int i=0; i<index; i++) result[j++] = bins[i];
	return result;
      }
  }

public class DTsource_flt extends ObserverAdapter implements 
HasDataSource, Rebinnable1DHistogramData, HasStyle
{

  private int SECONDS = 60;
  
  private String m_title;


  public DTsource_flt()
    {
     
      super(t_flt);
      m_title = "Fcup";
    }
  public DTsource_flt(String theTitle)
    {
      super(t_flt);
      m_title = theTitle;
    }
  public DataSource getDataSource(String param) {
    return new DTsource_flt(param);
  }
  public double[][] rebin(int nbin, double min, double max, boolean wantErrors, boolean hurry)
    {
      double[] result = t_flt.getSnapshot();
      double[][] r = { result };
      return r;
    }
  public double getMin()
    {
      return -60;
    }
  public double getMax()
    {
      return 0;
    }
  public int getBins()
    {
      return SECONDS;
    }
  public boolean isRebinnable()
    {
      return false;
    }
  public int getAxisType()
    {
      return Rebinnable1DHistogramData.DOUBLE;
    }
  public String[] getAxisLabels()
    {
      return null;
    }
  public Statistics getStatistics()
    {
      return null;
    }
  public String getTitle()
    {
      return m_title;
    }
  public JASHistStyle getStyle()
    {
      JASHist1DHistogramStyle style = new JASHist1DHistogramStyle();
      style.setShowErrorBars(false);
      style.setShowDataPoints(true);
      style.setShowLinesBetweenPoints(true);
      style.setShowHistogramBars(false);
      return style;
    }

}  

  public class flt_Thread extends Observable implements Runnable
  { 
    private int SECONDS = 60;
    private Thread thread;
    private int index = 0;
    private double[] bins = new double[SECONDS];
    private int thread_sleep = 10000;
    private int k = 0;

    flt_Thread()
      {
	thread = new Thread(this);
	thread.start();
      }
    public void run()
      {

	    for (;;)
	      {
		synchronized (this)
		  {
		    bins[index++] = live_fcup*100.0;
		    if (index>=SECONDS) index = 0; 
		  }
		setChanged();
		notifyObservers(hdr_flt);
		try {
		thread.sleep(thread_sleep);            
		} catch (InterruptedException e) {}
	      }
	  }
  
    synchronized double[] getSnapshot()
      {
	double[] result = new double[SECONDS];
	int j=0;
	for (int i=index; i<SECONDS; i++) result[j++] = bins[i];
	for (int i=0; i<index; i++) result[j++] = bins[i];
	return result;
      }
  }


  
public class DTsource_dr extends ObserverAdapter implements 
HasDataSource, Rebinnable1DHistogramData, HasStyle
{

  private int SECONDS = 60;
  
  private String m_title;

  public DTsource_dr()
    {
      super(t_dr);
      m_title = "Clock based livetime";
    }
  public DTsource_dr(String theTitle)
    {
      super(t_dr);
      m_title = theTitle;
    }
  public DataSource getDataSource(String param) {
    return new DTsource_dr(param);
  }
  public double[][] rebin(int nbin, double min, double max, boolean wantErrors, boolean hurry)
    {
      double[] result = t_dr.getSnapshot();
      double[][] r = { result };
      return r;
    }
  public double getMin()
    {
      return -60;
    }
  public double getMax()
    {
      return 0;
    }
  public int getBins()
    {
      return SECONDS;
    }
  public boolean isRebinnable()
    {
      return false;
    }
  public int getAxisType()
    {
      return Rebinnable1DHistogramData.DOUBLE;
    }
  public String[] getAxisLabels()
    {
      return null;
    }
  public Statistics getStatistics()
    {
      return null;
    }
  public String getTitle()
    {
      return m_title;
    }
  public JASHistStyle getStyle()
    {
      JASHist1DHistogramStyle style = new JASHist1DHistogramStyle();
      style.setShowErrorBars(false);
      style.setShowDataPoints(true);
      style.setShowLinesBetweenPoints(true);
      style.setShowHistogramBars(false);
      return style;
    }

}  
  public class dr_Thread extends Observable implements Runnable
  { 
    private int SECONDS = 60;
    private Thread thread;
    private int index = 0;
    private double[] bins = new double[SECONDS];
    private int thread_sleep = 10000;
    private int k = 0;

    dr_Thread()
      {
	thread = new Thread(this);
	thread.start();
      }
    public void run()
      {

	    for (;;)
	      {
		synchronized (this)
		  {
		    bins[index++] = er_datarate;
		    if (index>=SECONDS) index = 0; 
		  }
		setChanged();
		notifyObservers(hdr_dr);
		try {
		thread.sleep(thread_sleep);            
		} catch (InterruptedException e) {}
	      }
	  }
  
    synchronized double[] getSnapshot()
      {
	double[] result = new double[SECONDS];
	int j=0;
	for (int i=index; i<SECONDS; i++) result[j++] = bins[i];
	for (int i=0; i<index; i++) result[j++] = bins[i];
	return result;
      }
  }

  


public class DTsource_evtr extends ObserverAdapter implements 
HasDataSource, Rebinnable1DHistogramData, HasStyle
{

  private int SECONDS = 60;
  
  private String m_title;

  public DTsource_evtr()
    {
      super(t_evtr);
      m_title = "Clock based livetime";
    }
  public DTsource_evtr(String theTitle)
    {
      super(t_evtr);
      m_title = theTitle;
    }
  public DataSource getDataSource(String param) {
    return new DTsource_evtr(param);
  }
  public double[][] rebin(int nbin, double min, double max, boolean wantErrors, boolean hurry)
    {
      double[] result = t_evtr.getSnapshot();
      double[][] r = { result };
      return r;
    }
  public double getMin()
    {
      return -60;
    }
  public double getMax()
    {
      return 0;
    }
  public int getBins()
    {
      return SECONDS;
    }
  public boolean isRebinnable()
    {
      return false;
    }
  public int getAxisType()
    {
      return Rebinnable1DHistogramData.DOUBLE;
    }
  public String[] getAxisLabels()
    {
      return null;
    }
  public Statistics getStatistics()
    {
      return null;
    }
  public String getTitle()
    {
      return m_title;
    }
  public JASHistStyle getStyle()
    {
      JASHist1DHistogramStyle style = new JASHist1DHistogramStyle();
      style.setShowErrorBars(false);
      style.setShowDataPoints(true);
      style.setShowLinesBetweenPoints(true);
      style.setShowHistogramBars(false);
      return style;
    }

}  
  
  public class evtr_Thread extends Observable implements Runnable
  { 
    private int SECONDS = 60;
    private Thread thread;
    private int index = 0;
    private double[] bins = new double[SECONDS];
    private int thread_sleep = 10000;
    private int k = 0;

    evtr_Thread()
      {
	thread = new Thread(this);
	thread.start();
      }
    public void run()
      {

	    for (;;)
	      {
		synchronized (this)
		  {
		    bins[index++] = er_evtrate;
		    if (index>=SECONDS) index = 0; 
		  }
		setChanged();
		notifyObservers(hdr_evtr);
		try {
		thread.sleep(thread_sleep);            
		} catch (InterruptedException e) {}
	      }
	  }
  
    synchronized double[] getSnapshot()
      {
	double[] result = new double[SECONDS];
	int j=0;
	for (int i=index; i<SECONDS; i++) result[j++] = bins[i];
	for (int i=0; i<index; i++) result[j++] = bins[i];
	return result;
      }
  }



public class DTsource_l2pf extends ObserverAdapter implements 
HasDataSource, Rebinnable1DHistogramData, HasStyle
{

  private int SECONDS = 60;
  
  private String m_title;

  public DTsource_l2pf()
    {
      super(t_l2pf);
      m_title = " ";
    }
  public DTsource_l2pf(String theTitle)
    {
      super(t_l2pf);
      m_title = theTitle;
    }
  public DataSource getDataSource(String param) {
    return new DTsource_l2pf(param);
  }
  public double[][] rebin(int nbin, double min, double max, boolean wantErrors, boolean hurry)
    {
      double[] result = t_l2pf.getSnapshot();
      double[][] r = { result };
      return r;
    }
  public double getMin()
    {
      return -60;
    }
  public double getMax()
    {
      return 0;
    }
  public int getBins()
    {
      return SECONDS;
    }
  public boolean isRebinnable()
    {
      return false;
    }
  public int getAxisType()
    {
      return Rebinnable1DHistogramData.DOUBLE;
    }
  public String[] getAxisLabels()
    {
      return null;
    }
  public Statistics getStatistics()
    {
      return null;
    }
  public String getTitle()
    {
      return m_title;
    }
  public JASHistStyle getStyle()
    {
      JASHist1DHistogramStyle style = new JASHist1DHistogramStyle();
      style.setShowErrorBars(false);
      style.setShowDataPoints(true);
      style.setShowLinesBetweenPoints(true);
      style.setShowHistogramBars(false);
      return style;
    }

}  
  
  public class l2pf_Thread extends Observable implements Runnable
  { 
    private int SECONDS = 60;
    private Thread thread;
    private int index = 0;
    private double[] bins = new double[SECONDS];
    private int thread_sleep = 10000;
    private int k = 0;

    l2pf_Thread()
      {
	thread = new Thread(this);
	thread.start();
      }
    public void run()
      {

	    for (;;)
	      {
		synchronized (this)
		  {
		    if (l2pf[1][0] > 0 )
		      bins[index++] = l2pf[1][1]/l2pf[1][0];
		    else
		      bins[index++] = 0;
		    
		    if (index>=SECONDS) index = 0; 
		  }
		setChanged();
		notifyObservers(hdr_l2pf);
		try {
		thread.sleep(thread_sleep);            
		} catch (InterruptedException e) {}
	      }
	  }
  
    synchronized double[] getSnapshot()
      {
	double[] result = new double[SECONDS];
	int j=0;
	for (int i=index; i<SECONDS; i++) result[j++] = bins[i];
	for (int i=0; i<index; i++) result[j++] = bins[i];
	return result;
      }
  }

class DTsource_drroc extends Observable 
implements Rebinnable1DHistogramData, Runnable
{
  public String[] getAxisLabels() { return null; }
  public double getMin() { return 0; }
  public double getMax() { return data.length; }
  public boolean isRebinnable() { return false; }
  public int getBins() { return data.length; }
  public int getAxisType() { return DOUBLE; }
  public String getTitle() { return "Data Rate in ROCs"; }
  
  private double[] data;  
  private double[] newData = new double[22];  


  DTsource_drroc()
    {
      setData(); // Set initial data
      // Create a thread to update the data, it will call the run
      // method in this class.
      Thread t = new Thread(this);
      t.start(); 
    }
  private void setData()
    {
      for (int i=0; i<22; i++)
	{ 
	newData[i] = roc_datarate[i]/1000.0;
	//	System.out.println(newData[i] + " " + roc_datarate[i]);      
	}
      data = newData;
    }
  public void run()
    {
      try
	{
	  // Create the HistogramUpdate object that will be sent
	  // to all our Observers (specifically the JASHist). This
	  // HistogramUpdate will specify that both the bin contents
	  // (data) and the axis min/max (range) have changed. The 
	  // boolean argument to the HistogramUpdate constructor 
	  // indicates that this is a "final" update, which the 
	  
	  Thread.sleep(5000); // wait 5 seconds before first update
	  for (;;)
	    {
	      setData(); // update the data
	      setChanged(); // notifyObservers needs this
	      notifyObservers(hdr_drroc);
	      Thread.sleep(5000); // wait 1 second
	    }
	}
      catch (InterruptedException e) {}
    }

  public double[][] rebin(int rBins, double rMin, double rMax, 
			  boolean wantErrors, boolean hurry)
    {
      double[][] result = { data };
      return result;
    }
}

class DTsource_ebbuf extends Observable 
implements Rebinnable1DHistogramData, Runnable, HasStyle
{
  public String[] getAxisLabels() { return null; }
  public double getMin() { return 0; }
  public double getMax() { return data.length; }
  public boolean isRebinnable() { return false; }
  public int getBins() { return data.length; }
  public int getAxisType() { return DOUBLE; }
  public String getTitle() { return "EB buffer occupancy"; }
  
  private double[] data;  
  private double[] newData = new double[32];  

  public JASHistStyle getStyle()
    {
      JASHist1DHistogramStyle style = new JASHist1DHistogramStyle();
      style.setShowErrorBars(false);
      style.setShowDataPoints(true);
      style.setShowLinesBetweenPoints(false);
      style.setShowHistogramBars(true);
      return style;
    }


  DTsource_ebbuf()
    {
      setData(); // Set initial data
      // Create a thread to update the data, it will call the run
      // method in this class.
      Thread t = new Thread(this);
      t.start(); 
    }
  private void setData()
    {
      for (int i=0; i<32; i++)
	{ 
	newData[i] = ebbuf[i];
	//	System.out.println(newData[i] + " " + ebbuf[i]);      
	}
      data = newData;
    }
  public void run()
    {
      try
	{
	  // Create the HistogramUpdate object that will be sent
	  // to all our Observers (specifically the JASHist). This
	  // HistogramUpdate will specify that both the bin contents
	  // (data) and the axis min/max (range) have changed. The 
	  // boolean argument to the HistogramUpdate constructor 
	  // indicates that this is a "final" update, which the 
	  
	  Thread.sleep(5000); // wait 5 seconds before first update
	  for (;;)
	    {
	      setData(); // update the data
	      setChanged(); // notifyObservers needs this
	      notifyObservers(hdr_ebbuf);
	      Thread.sleep(5000); // wait 1 second
	    }
	}
      catch (InterruptedException e) {}
    }

  public double[][] rebin(int rBins, double rMin, double rMax, 
			  boolean wantErrors, boolean hurry)
    {
      double[][] result = { data };
      return result;
    }
}

}








E 1
