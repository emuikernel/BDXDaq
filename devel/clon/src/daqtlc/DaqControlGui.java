/* 
   This simple extension of the java.repsis.Frame class 
   contains all the elements necessary to act as the 
   main window of an application. 
   */ 

import java.awt.*; 
import com.sun.java.swing.*; 
import java.lang.*; 
import java.io.*; 
import java.util.*; 
import jclass.chart.*; 
import jclass.bwt.*;

import symantec.itools.multimedia.ImageViewer; 
import symantec.itools.awt.ImagePanel; 
import symantec.itools.awt.shape.HorizontalLine; 
import symantec.itools.awt.shape.Square; 
import symantec.itools.awt.shape.Rect; 
import symantec.itools.awt.shape.VerticalLine; 
import symantec.itools.awt.shape.Line; 
import symantec.itools.awt.util.spinner.ListSpinner; 

import jas.hist.*;

public class DaqControlGui extends Frame  
{ 
    //public ProgressMonitor pm = null;
    //public int progressCount = 0;

  public  UnixExe mySysExe = new UnixExe();
  
  public JButton[] csr = new JButton[32]; 
  public JButton[] st = new JButton[32]; 
  public JCheckBox[] chbit = new JCheckBox[12];
  public JButton[] bit = new JButton[12];  
  public JTextField[] prescale = new JTextField[8]; 
  public JButton[] roc_button = new JButton[39];
  public JButton camac1; 
  public JButton camac2; 
  public JButton camac3; 
  public JButton pretrig1; 
  public JButton pretrig2; 
  public JButton dccntrl; 
  public JButton sclaser1; 
  public JButton EB; 
  public JButton ER; 
  public JButton ET; 
  public JButton ET00; 
  public JButton It2ipc; 
  public JButton Ipc2it; 
  public JButton Ipc2dd; 
  public JButton Recsis;
  public JButton Stadis;
  public JButton Trg_monitor;  
  public JButton Scaler; 
  public JButton Evt_monitor; 
  public JButton raid1; 
  public JButton raid2; 
  public JButton raid3; 
  public JButton raid4; 
  public JButton raid5; 
  public JButton raid6; 
  public JButton raid7; 
  public JButton raid8; 
  public symantec.itools.awt.shape.HorizontalLine dc2_line; 
  public symantec.itools.awt.shape.HorizontalLine dc8_line; 
  public symantec.itools.awt.shape.HorizontalLine dc5_line; 
  public symantec.itools.awt.shape.HorizontalLine dc1_line; 
  public symantec.itools.awt.shape.HorizontalLine dc7_line; 
  public symantec.itools.awt.shape.HorizontalLine dc6_line; 
  public symantec.itools.awt.shape.HorizontalLine dc11_line; 
  public symantec.itools.awt.shape.HorizontalLine polar_line; 
  public symantec.itools.awt.shape.HorizontalLine dc9_line; 
  public symantec.itools.awt.shape.HorizontalLine dc3_line; 
  public symantec.itools.awt.shape.HorizontalLine dc4_line; 
  public symantec.itools.awt.shape.HorizontalLine dc10_line; 
  public symantec.itools.awt.shape.HorizontalLine tage_line; 
  public symantec.itools.awt.shape.HorizontalLine lac1_line; 
  public symantec.itools.awt.shape.HorizontalLine sc1_line; 
  public symantec.itools.awt.shape.HorizontalLine cc1_line; 
  public symantec.itools.awt.shape.HorizontalLine ec1_line; 
  public symantec.itools.awt.shape.HorizontalLine ec2_line; 
  public symantec.itools.awt.shape.HorizontalLine scaler4_line; 
  public symantec.itools.awt.shape.HorizontalLine scaler3_line; 
  public symantec.itools.awt.shape.HorizontalLine scaler1_line; 
  public symantec.itools.awt.shape.HorizontalLine scaler2_line; 
  public symantec.itools.awt.shape.HorizontalLine er_line; 
  public symantec.itools.awt.shape.HorizontalLine raid2_line; 
  public symantec.itools.awt.shape.HorizontalLine raid3_line; 
  public symantec.itools.awt.shape.HorizontalLine raid4_line; 
  public symantec.itools.awt.shape.HorizontalLine raid5_line; 
  public symantec.itools.awt.shape.HorizontalLine raid6_line; 
  public symantec.itools.awt.shape.HorizontalLine raid7_line; 
  public symantec.itools.awt.shape.HorizontalLine raid8_line; 
  public symantec.itools.awt.shape.HorizontalLine raid1_line; 
  public symantec.itools.awt.shape.HorizontalLine raid1silo; 
  public symantec.itools.awt.shape.HorizontalLine raid2silo; 
  public symantec.itools.awt.shape.HorizontalLine raid3silo; 
  public symantec.itools.awt.shape.HorizontalLine raid4silo;
  public symantec.itools.awt.shape.HorizontalLine raid5silo;
  public symantec.itools.awt.shape.HorizontalLine raid6silo;
  public symantec.itools.awt.shape.HorizontalLine raid7silo;
  public symantec.itools.awt.shape.HorizontalLine raid8silo;
  public symantec.itools.awt.Label3D run_number; 
  public symantec.itools.awt.Label3D number_events; 
  public symantec.itools.awt.Label3D run_config; 
  public JButton daq_recover; 
  public JButton online_recover; 
  public symantec.itools.awt.Label3D update_time; 
  public JTextArea info_text; 
  public symantec.itools.awt.Label3D l1input1; 
  public symantec.itools.awt.Label3D l1input2; 
  public symantec.itools.awt.Label3D l1input3; 
  public symantec.itools.awt.Label3D l1input4; 
  public symantec.itools.awt.Label3D l1input5; 
  public symantec.itools.awt.Label3D l1input6; 
  public symantec.itools.awt.Label3D l1input7; 
  public symantec.itools.awt.Label3D l1input8; 
  public symantec.itools.awt.Label3D l1input10; 
  public symantec.itools.awt.Label3D l1input12; 
  public symantec.itools.awt.Label3D eb_evt_rate; 
  public symantec.itools.awt.Label3D eb_data_rate; 
  public symantec.itools.awt.Label3D ipc2dd_rate; 
  public symantec.itools.awt.Label3D et00_rate; 
  public symantec.itools.awt.Label3D et00_drate; 
  public symantec.itools.awt.Label3D recsis_rate; 
  public symantec.itools.awt.Label3D evmon_rate; 
  public symantec.itools.awt.Label3D er_evt_rate; 
  public symantec.itools.awt.Label3D er_data_rate; 
  
  public jclass.chart.JCChart roc_data_rate; 
  public jclass.chart.JCChart roc_fe_buffer; 
  public jclass.chart.JCChart level2_sectors; 
  public jclass.chart.JCChart level2_pass_fail; 
  
  public static JASHist data_rate_comp = new JASHist();
  public static JASHist eb_buffer_comp = new JASHist();

  public static JASHist l2_pasfail_chart = new JASHist();
  public static JASHist event_rate_chart = new JASHist();  
  public static JASHist data_rate_chart = new JASHist();  
  public static JASHist live_time_chart = new JASHist();    



  public DaqControlGui() 
    { 
      
      
      for (int i = 0; i<=31; i++) {
	csr[i] = new JButton();
	st[i] = new JButton();
      }
      
      
      for (int i = 0; i<=38; i++) {
	roc_button[i] = new JButton();
      }	
      
      for (int i = 0; i<=11; i++) {
	chbit[i] = new JCheckBox();
	bit[i] = new JButton();
      }	
      
      for (int i = 0; i<=7; i++) {
	prescale[i] = new JTextField();
	prescale[i].setText("0"); 
      }	
      
      //{{INIT_CONTROLS 
      setLayout(null); 
      setVisible(false); 
      setSize(1265,965); 
      setFont(new Font("Dialog", Font.PLAIN, 12)); 
      setForeground(new Color(0)); 
      setBackground(new Color(12632256)); 
      openFileDialog1 = new java.awt.FileDialog(this); 
      openFileDialog1.setMode(FileDialog.LOAD); 
      openFileDialog1.setTitle("Open"); 
      //$$ openFileDialog1.move(972,648); 
      
      csr[0].setBounds(36,84,12,24); 
      csr[0].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[0].setForeground(new Color(0)); 
      csr[0].setBackground(new Color(-3355444)); 
      add(csr[0]); 
      
      csr[1].setBounds(48,84,12,24); 
      csr[1].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[1].setForeground(new Color(0)); 
      csr[1].setBackground(new Color(-3355444)); 
      add(csr[1]); 
      
      csr[2].setBounds(60,84,12,24); 
      csr[2].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[2].setForeground(new Color(0)); 
      csr[2].setBackground(new Color(-3355444)); 
      add(csr[2]); 
      
      csr[3].setBounds(72,84,12,24); 
      csr[3].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[3].setForeground(new Color(0)); 
      csr[3].setBackground(new Color(-3355444)); 
      add(csr[3]); 
      
      csr[4].setBounds(84,84,12,24); 
      csr[4].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[4].setForeground(new Color(0)); 
      csr[4].setBackground(new Color(-3355444)); 
      add(csr[4]); 
      
      csr[5].setBounds(96,84,12,24); 
      csr[5].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[5].setForeground(new Color(0)); 
      csr[5].setBackground(new Color(-3355444)); 
      add(csr[5]); 
      
      csr[6].setBounds(108,84,12,24); 
      csr[6].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[6].setForeground(new Color(0)); 
      csr[6].setBackground(new Color(-3355444)); 
      add(csr[6]); 
      
      csr[7].setBounds(120,84,12,24); 
      csr[7].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[7].setForeground(new Color(0)); 
      csr[7].setBackground(new Color(-3355444)); 
      add(csr[7]); 
      
      csr[8].setBounds(132,84,12,24); 
      csr[8].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[8].setForeground(new Color(0)); 
      csr[8].setBackground(new Color(-3355444)); 
      add(csr[8]); 
      
      csr[9].setBounds(144,84,12,24); 
      csr[9].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[9].setForeground(new Color(0)); 
      csr[9].setBackground(new Color(-3355444)); 
      add(csr[9]); 
      
      csr[10].setBounds(156,84,12,24); 
      csr[10].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[10].setForeground(new Color(0)); 
      csr[10].setBackground(new Color(-3355444)); 
      add(csr[10]); 
      
      csr[11].setBounds(168,84,12,24); 
      csr[11].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[11].setForeground(new Color(0)); 
      csr[11].setBackground(new Color(-3355444)); 
      add(csr[11]); 
      
      csr[12].setBounds(180,84,12,24); 
      csr[12].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[12].setForeground(new Color(0)); 
      csr[12].setBackground(new Color(-3355444)); 
      add(csr[12]); 
      
      csr[13].setBounds(192,84,12,24); 
      csr[13].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[13].setForeground(new Color(0)); 
      csr[13].setBackground(new Color(-3355444)); 
      add(csr[13]); 
      
      csr[14].setBounds(204,84,12,24); 
      csr[14].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[14].setForeground(new Color(0)); 
      csr[14].setBackground(new Color(-3355444)); 
      add(csr[14]); 
      
      csr[15].setBounds(216,84,12,24); 
      csr[15].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[15].setForeground(new Color(0)); 
      csr[15].setBackground(new Color(-3355444)); 
      add(csr[15]); 
      
      csr[16].setBounds(228,84,12,24); 
      csr[16].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[16].setForeground(new Color(0)); 
      csr[16].setBackground(new Color(-3355444)); 
      add(csr[16]); 
      
      csr[17].setBounds(240,84,12,24); 
      csr[17].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[17].setForeground(new Color(0)); 
      csr[17].setBackground(new Color(-3355444)); 
      add(csr[17]); 
      
      csr[18].setBounds(252,84,12,24); 
      csr[18].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[18].setForeground(new Color(0)); 
      csr[18].setBackground(new Color(-3355444)); 
      add(csr[18]); 
      
      csr[19].setBounds(264,84,12,24); 
      csr[19].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[19].setForeground(new Color(0)); 
      csr[19].setBackground(new Color(-3355444)); 
      add(csr[19]); 
      
      csr[20].setBounds(276,84,12,24); 
      csr[20].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[20].setForeground(new Color(0)); 
      csr[20].setBackground(new Color(-3355444)); 
      add(csr[20]); 
      
      csr[21].setBounds(288,84,12,24); 
      csr[21].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[21].setForeground(new Color(0)); 
      csr[21].setBackground(new Color(-3355444)); 
      add(csr[21]); 
      
      csr[22].setBounds(300,84,12,24); 
      csr[22].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[22].setForeground(new Color(0)); 
      csr[22].setBackground(new Color(-3355444)); 
      add(csr[22]); 
      
      csr[23].setBounds(312,84,12,24); 
      csr[23].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[23].setForeground(new Color(0)); 
      csr[23].setBackground(new Color(-3355444)); 
      add(csr[23]); 
      
      csr[24].setBounds(324,84,12,24); 
      csr[24].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[24].setForeground(new Color(0)); 
      csr[24].setBackground(new Color(-3355444)); 
      add(csr[24]); 
      
      csr[25].setBounds(336,84,12,24); 
      csr[25].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[25].setForeground(new Color(0)); 
      csr[25].setBackground(new Color(-3355444)); 
      add(csr[25]); 
      
      csr[26].setBounds(348,84,12,24); 
      csr[26].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[26].setForeground(new Color(0)); 
      csr[26].setBackground(new Color(-3355444)); 
      add(csr[26]); 
      
      csr[27].setBounds(360,84,12,24); 
      csr[27].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[27].setForeground(new Color(0)); 
      csr[27].setBackground(new Color(-3355444)); 
      add(csr[27]); 
      
      csr[28].setBounds(372,84,12,24); 
      csr[28].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[28].setForeground(new Color(0)); 
      csr[28].setBackground(new Color(-3355444)); 
      add(csr[28]); 
      
      csr[29].setBounds(384,84,12,24); 
      csr[29].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[29].setForeground(new Color(0)); 
      csr[29].setBackground(new Color(-3355444)); 
      add(csr[29]); 
      
      csr[30].setBounds(396,84,12,24); 
      csr[30].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[30].setForeground(new Color(0)); 
      csr[30].setBackground(new Color(-3355444)); 
      add(csr[30]); 
      
      csr[31].setBounds(408,84,12,24); 
      csr[31].setFont(new Font("Dialog", Font.BOLD, 12)); 
      csr[31].setForeground(new Color(0)); 
      csr[31].setBackground(new Color(-3355444)); 
      add(csr[31]);
      
      
      st[0].setBounds(36,108,12,24); 
      st[0].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[0].setForeground(new Color(0)); 
      st[0].setBackground(new Color(-3355444)); 
      add(st[0]); 
      
      st[1].setBounds(48,108,12,24); 
      st[1].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[1].setForeground(new Color(0)); 
      st[1].setBackground(new Color(-3355444)); 
      add(st[1]); 
      
      st[2].setBounds(60,108,12,24); 
      st[2].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[2].setForeground(new Color(0)); 
      st[2].setBackground(new Color(-3355444)); 
      add(st[2]); 
      
      st[3].setBounds(72,108,12,24); 
      st[3].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[3].setForeground(new Color(0)); 
      st[3].setBackground(new Color(-3355444)); 
      add(st[3]); 
      
      st[4].setBounds(84,108,12,24); 
      st[4].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[4].setForeground(new Color(0)); 
      st[4].setBackground(new Color(-3355444)); 
      add(st[4]); 
      
      st[5].setBounds(96,108,12,24); 
      st[5].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[5].setForeground(new Color(0)); 
      st[5].setBackground(new Color(-3355444)); 
      add(st[5]); 
      
      st[6].setBounds(108,108,12,24); 
      st[6].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[6].setForeground(new Color(0)); 
      st[6].setBackground(new Color(-3355444)); 
      add(st[6]); 
      
      st[7].setBounds(120,108,12,24); 
      st[7].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[7].setForeground(new Color(0)); 
      st[7].setBackground(new Color(-3355444)); 
      add(st[7]); 
      
      st[8].setBounds(132,108,12,24); 
      st[8].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[8].setForeground(new Color(0)); 
      st[8].setBackground(new Color(-3355444)); 
      add(st[8]); 
      
      st[9].setBounds(144,108,12,24); 
      st[9].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[9].setForeground(new Color(0)); 
      st[9].setBackground(new Color(-3355444)); 
      add(st[9]); 
      
      st[10].setBounds(156,108,12,24); 
      st[10].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[10].setForeground(new Color(0)); 
      st[10].setBackground(new Color(-3355444)); 
      add(st[10]); 
      
      st[11].setBounds(168,108,12,24); 
      st[11].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[11].setForeground(new Color(0)); 
      st[11].setBackground(new Color(-3355444)); 
      add(st[11]); 
      
      st[12].setBounds(180,108,12,24); 
      st[12].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[12].setForeground(new Color(0)); 
      st[12].setBackground(new Color(-3355444)); 
      add(st[12]); 
      
      st[13].setBounds(192,108,12,24); 
      st[13].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[13].setForeground(new Color(0)); 
      st[13].setBackground(new Color(-3355444)); 
      add(st[13]); 
      
      st[14].setBounds(204,108,12,24); 
      st[14].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[14].setForeground(new Color(0)); 
      st[14].setBackground(new Color(-3355444)); 
      add(st[14]); 
      
      st[15].setBounds(216,108,12,24); 
      st[15].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[15].setForeground(new Color(0)); 
      st[15].setBackground(new Color(-3355444)); 
      add(st[15]); 
      
      st[16].setBounds(228,108,12,24); 
      st[16].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[16].setForeground(new Color(0)); 
      st[16].setBackground(new Color(-3355444)); 
      add(st[16]); 
      
      st[17].setBounds(240,108,12,24); 
      st[17].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[17].setForeground(new Color(0)); 
      st[17].setBackground(new Color(-3355444)); 
      add(st[17]); 
      
      st[18].setBounds(252,108,12,24); 
      st[18].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[18].setForeground(new Color(0)); 
      st[18].setBackground(new Color(-3355444)); 
      add(st[18]); 
      
      st[19].setBounds(264,108,12,24); 
      st[19].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[19].setForeground(new Color(0)); 
      st[19].setBackground(new Color(-3355444)); 
      add(st[19]); 
      
      st[20].setBounds(276,108,12,24); 
      st[20].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[20].setForeground(new Color(0)); 
      st[20].setBackground(new Color(-3355444)); 
      add(st[20]); 
      
      st[21].setBounds(288,108,12,24); 
      st[21].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[21].setForeground(new Color(0)); 
      st[21].setBackground(new Color(-3355444)); 
      add(st[21]); 
      
      st[22].setBounds(300,108,12,24); 
      st[22].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[22].setForeground(new Color(0)); 
      st[22].setBackground(new Color(-3355444)); 
      add(st[22]); 
      
      st[23].setBounds(312,108,12,24); 
      st[23].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[23].setForeground(new Color(0)); 
      st[23].setBackground(new Color(-3355444)); 
      add(st[23]); 
      
      st[24].setBounds(324,108,12,24); 
      st[24].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[24].setForeground(new Color(0)); 
      st[24].setBackground(new Color(-3355444)); 
      add(st[24]); 
      
      st[25].setBounds(336,108,12,24); 
      st[25].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[25].setForeground(new Color(0)); 
      st[25].setBackground(new Color(-3355444)); 
      add(st[25]); 
      
      st[26].setBounds(348,108,12,24); 
      st[26].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[26].setForeground(new Color(0)); 
      st[26].setBackground(new Color(-3355444)); 
      add(st[26]); 
      
      st[27].setBounds(360,108,12,24); 
      st[27].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[27].setForeground(new Color(0)); 
      st[27].setBackground(new Color(-3355444)); 
      add(st[27]); 
      
      st[28].setBounds(372,108,12,24); 
      st[28].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[28].setForeground(new Color(0)); 
      st[28].setBackground(new Color(-3355444)); 
      add(st[28]); 
      
      st[29].setBounds(384,108,12,24); 
      st[29].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[29].setForeground(new Color(0)); 
      st[29].setBackground(new Color(-3355444)); 
      add(st[29]); 
      
      st[30].setBounds(396,108,12,24); 
      st[30].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[30].setForeground(new Color(0)); 
      st[30].setBackground(new Color(-3355444)); 
      add(st[30]); 
      
      st[31].setBounds(408,108,12,24); 
      st[31].setFont(new Font("Dialog", Font.BOLD, 12)); 
      st[31].setForeground(new Color(0)); 
      st[31].setBackground(new Color(-3355444)); 
      add(st[31]);
      
      
      chbit[0].setBounds(48,30,12,24); 
      chbit[0].setFont(new Font("Dialog", Font.BOLD, 12)); 
      chbit[0].setForeground(new Color(0)); 
      chbit[0].setBackground(new Color(-3355444)); 
      add(chbit[0]); 
      
      
      chbit[1].setBounds(84,30,12,24); 
      chbit[1].setFont(new Font("Dialog", Font.BOLD, 12)); 
      chbit[1].setForeground(new Color(0)); 
      chbit[1].setBackground(new Color(-3355444)); 
      add(chbit[1]); 
      
      
      chbit[2].setBounds(120,30,12,24); 
      chbit[2].setFont(new Font("Dialog", Font.BOLD, 12)); 
      chbit[2].setForeground(new Color(0)); 
      chbit[2].setBackground(new Color(-3355444)); 
      add(chbit[2]);
      
      
      chbit[3].setBounds(156,30,12,24); 
      chbit[3].setFont(new Font("Dialog", Font.BOLD, 12)); 
      chbit[3].setForeground(new Color(0)); 
      chbit[3].setBackground(new Color(-3355444)); 
      add(chbit[3]);
      
      
      chbit[4].setBounds(192,30,12,24); 
      chbit[4].setFont(new Font("Dialog", Font.BOLD, 12)); 
      chbit[4].setForeground(new Color(0)); 
      chbit[4].setBackground(new Color(-3355444)); 
      add(chbit[4]);
      
      
      chbit[5].setBounds(228,30,12,24); 
      chbit[5].setFont(new Font("Dialog", Font.BOLD, 12)); 
      chbit[5].setForeground(new Color(0)); 
      chbit[5].setBackground(new Color(-3355444)); 
      add(chbit[5]);
      
      
      chbit[6].setBounds(264,30,12,24); 
      chbit[6].setFont(new Font("Dialog", Font.BOLD, 12)); 
      chbit[6].setForeground(new Color(0)); 
      chbit[6].setBackground(new Color(-3355444)); 
      add(chbit[6]);
      
      chbit[7].setBounds(300,30,12,24); 
      chbit[7].setFont(new Font("Dialog", Font.BOLD, 12)); 
      chbit[7].setForeground(new Color(0)); 
      chbit[7].setBackground(new Color(-3355444)); 
      add(chbit[7]); 
      
      
      chbit[8].setBounds(336,30,12,24); 
      chbit[8].setFont(new Font("Dialog", Font.BOLD, 12)); 
      chbit[8].setForeground(new Color(0)); 
      chbit[8].setBackground(new Color(-3355444)); 
      add(chbit[8]);
      
      chbit[9].setBounds(360,30,12,24); 
      chbit[9].setFont(new Font("Dialog", Font.BOLD, 12)); 
      chbit[9].setForeground(new Color(0)); 
      chbit[9].setBackground(new Color(-3355444)); 
      add(chbit[9]);
      
      chbit[10].setBounds(384,30,12,24); 
      chbit[10].setFont(new Font("Dialog", Font.BOLD, 12)); 
      chbit[10].setForeground(new Color(0)); 
      chbit[10].setBackground(new Color(-3355444)); 
      add(chbit[10]); 
      
      chbit[11].setBounds(408,30,12,24); 
      chbit[11].setFont(new Font("Dialog", Font.BOLD, 12)); 
      chbit[11].setForeground(new Color(0)); 
      chbit[11].setBackground(new Color(-3355444)); 
      add(chbit[11]); 
      
      
      bit[0].setBounds(48,36,12,24); 
      bit[0].setFont(new Font("Dialog", Font.BOLD, 12)); 
      bit[0].setForeground(new Color(0)); 
      bit[0].setBackground(new Color(-3355444)); 
      add(bit[0]); 
      
      
      bit[1].setBounds(84,36,12,24); 
      bit[1].setFont(new Font("Dialog", Font.BOLD, 12)); 
      bit[1].setForeground(new Color(0)); 
      bit[1].setBackground(new Color(-3355444)); 
      add(bit[1]); 
      
      
      bit[2].setBounds(120,36,12,24); 
      bit[2].setFont(new Font("Dialog", Font.BOLD, 12)); 
      bit[2].setForeground(new Color(0)); 
      bit[2].setBackground(new Color(-3355444)); 
      add(bit[2]);
      
      
      bit[3].setBounds(156,36,12,24); 
      bit[3].setFont(new Font("Dialog", Font.BOLD, 12)); 
      bit[3].setForeground(new Color(0)); 
      bit[3].setBackground(new Color(-3355444)); 
      add(bit[3]);
      
      
      bit[4].setBounds(192,36,12,24); 
      bit[4].setFont(new Font("Dialog", Font.BOLD, 12)); 
      bit[4].setForeground(new Color(0)); 
      bit[4].setBackground(new Color(-3355444)); 
      add(bit[4]);
      
      
      bit[5].setBounds(228,36,12,24); 
      bit[5].setFont(new Font("Dialog", Font.BOLD, 12)); 
      bit[5].setForeground(new Color(0)); 
      bit[5].setBackground(new Color(-3355444)); 
      add(bit[5]);
      
      
      bit[6].setBounds(264,36,12,24); 
      bit[6].setFont(new Font("Dialog", Font.BOLD, 12)); 
      bit[6].setForeground(new Color(0)); 
      bit[6].setBackground(new Color(-3355444)); 
      add(bit[6]);
      
      bit[7].setBounds(300,36,12,24); 
      bit[7].setFont(new Font("Dialog", Font.BOLD, 12)); 
      bit[7].setForeground(new Color(0)); 
      bit[7].setBackground(new Color(-3355444)); 
      add(bit[7]); 
      
      
      bit[8].setBounds(336,36,12,48); 
      bit[8].setFont(new Font("Dialog", Font.BOLD, 12)); 
      bit[8].setForeground(new Color(0)); 
      bit[8].setBackground(new Color(-3355444)); 
      add(bit[8]);
      
      bit[9].setBounds(360,36,12,48); 
      bit[9].setFont(new Font("Dialog", Font.BOLD, 12)); 
      bit[9].setForeground(new Color(0)); 
      bit[9].setBackground(new Color(-3355444)); 
      add(bit[9]);
      
      bit[10].setBounds(384,36,12,48); 
      bit[10].setFont(new Font("Dialog", Font.BOLD, 12)); 
      bit[10].setForeground(new Color(0)); 
      bit[10].setBackground(new Color(-3355444)); 
      add(bit[10]); 
      
      bit[11].setBounds(408,36,12,48); 
      bit[11].setFont(new Font("Dialog", Font.BOLD, 12)); 
      bit[11].setForeground(new Color(0)); 
      bit[11].setBackground(new Color(-3355444)); 
      add(bit[11]); 
      
      
      prescale[0].setMargin(new java.awt.Insets(0,0,0,0)); 
      prescale[0].setBounds(36,60,36,24); 
      prescale[0].setFont(new Font("SansSerif", Font.PLAIN, 12)); 
      prescale[0].setForeground(new Color(0)); 
      prescale[0].setBackground(new Color(16777215)); 
      add(prescale[0]); 
      
      prescale[1].setMargin(new java.awt.Insets(0,0,0,0)); 
      prescale[1].setBounds(72,60,36,24); 
      prescale[1].setFont(new Font("SansSerif", Font.PLAIN, 12)); 
      prescale[1].setForeground(new Color(0)); 
      prescale[1].setBackground(new Color(16777215)); 
      add(prescale[1]); 
      
      prescale[2].setMargin(new java.awt.Insets(0,0,0,0)); 
      prescale[2].setBounds(108,60,36,24); 
      prescale[2].setFont(new Font("SansSerif", Font.PLAIN, 12)); 
      prescale[2].setForeground(new Color(0)); 
      prescale[2].setBackground(new Color(16777215)); 
      add(prescale[2]); 
      
      prescale[3].setMargin(new java.awt.Insets(0,0,0,0)); 
      prescale[3].setBounds(144,60,36,24); 
      prescale[3].setFont(new Font("SansSerif", Font.PLAIN, 12)); 
      prescale[3].setForeground(new Color(0)); 
      prescale[3].setBackground(new Color(16777215)); 
      add(prescale[3]); 
      
      prescale[4].setMargin(new java.awt.Insets(0,0,0,0)); 
      prescale[4].setBounds(180,60,36,24); 
      prescale[4].setFont(new Font("SansSerif", Font.PLAIN, 12)); 
      prescale[4].setForeground(new Color(0)); 
      prescale[4].setBackground(new Color(16777215)); 
      add(prescale[4]); 
      
      prescale[5].setMargin(new java.awt.Insets(0,0,0,0)); 
      prescale[5].setBounds(216,60,36,24); 
      prescale[5].setFont(new Font("SansSerif", Font.PLAIN, 12)); 
      prescale[5].setForeground(new Color(0)); 
      prescale[5].setBackground(new Color(16777215)); 
      add(prescale[5]); 
      
      prescale[6].setMargin(new java.awt.Insets(0,0,0,0)); 
      prescale[6].setBounds(252,60,36,24); 
      prescale[6].setFont(new Font("SansSerif", Font.PLAIN, 12)); 
      prescale[6].setForeground(new Color(0)); 
      prescale[6].setBackground(new Color(16777215)); 
      add(prescale[6]); 
      
      prescale[7].setMargin(new java.awt.Insets(0,0,0,0)); 
      prescale[7].setBounds(288,60,36,24); 
      prescale[7].setFont(new Font("SansSerif", Font.PLAIN, 12)); 
      prescale[7].setForeground(new Color(0)); 
      prescale[7].setBackground(new Color(16777215)); 
      add(prescale[7]); 
      
      
      roc_button[8].setText("dc2"); 
      roc_button[8].setActionCommand("dc2"); 
      roc_button[8].setBounds(156,180,61,25); 
      roc_button[8].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[8].setForeground(new Color(0)); 
      roc_button[8].setBackground(new Color(-3355444)); 
      add(roc_button[8]); 
      
      roc_button[9].setText("dc8"); 
      roc_button[9].setActionCommand("dc8"); 
      roc_button[9].setBounds(156,204,61,25); 
      roc_button[9].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[9].setForeground(new Color(0)); 
      roc_button[9].setBackground(new Color(-3355444)); 
      add(roc_button[9]); 
      
      
      roc_button[10].setText("dc5"); 
      roc_button[10].setActionCommand("dc5"); 
      roc_button[10].setBounds(156,228,61,25); 
      roc_button[10].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[10].setForeground(new Color(0)); 
      roc_button[10].setBackground(new Color(-3355444)); 
      add(roc_button[10]); 
      
      roc_button[11].setText("dc1"); 
      roc_button[11].setActionCommand("dc1"); 
      roc_button[11].setBounds(156,252,61,25); 
      roc_button[11].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[11].setForeground(new Color(0)); 
      roc_button[11].setBackground(new Color(-3355444)); 
      add(roc_button[11]); 
      
      roc_button[12].setText("dc7"); 
      roc_button[12].setActionCommand("dc7"); 
      roc_button[12].setBounds(156,276,61,25); 
      roc_button[12].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[12].setForeground(new Color(0)); 
      roc_button[12].setBackground(new Color(-3355444)); 
      add(roc_button[12]); 
      
      roc_button[13].setText("dc6"); 
      roc_button[13].setActionCommand("dc6"); 
      roc_button[13].setBounds(156,300,61,25); 
      roc_button[13].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[13].setForeground(new Color(0)); 
      roc_button[13].setBackground(new Color(-3355444)); 
      add(roc_button[13]); 
      
      roc_button[14].setText("dc11"); 
      roc_button[14].setActionCommand("dc11"); 
      roc_button[14].setBounds(156,324,61,25); 
      roc_button[14].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[14].setForeground(new Color(0)); 
      roc_button[14].setBackground(new Color(-3355444)); 
      add(roc_button[14]); 
      
      roc_button[15].setText("polar"); 
      roc_button[15].setActionCommand("polar"); 
      roc_button[15].setBounds(156,348,61,25); 
      roc_button[15].setFont(new Font("Serif", Font.BOLD, 10)); 
      roc_button[15].setForeground(new Color(0)); 
      roc_button[15].setBackground(new Color(-3355444)); 
      add(roc_button[15]); 
      
      roc_button[16].setText("sc1"); 
      roc_button[16].setActionCommand("sc1"); 
      roc_button[16].setBounds(252,180,61,25); 
      roc_button[16].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[16].setForeground(new Color(0)); 
      roc_button[16].setBackground(new Color(-3355444)); 
      add(roc_button[16]); 
      
      roc_button[17].setText("cc1"); 
      roc_button[17].setActionCommand("cc1"); 
      roc_button[17].setBounds(252,204,61,25); 
      roc_button[17].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[17].setForeground(new Color(0)); 
      roc_button[17].setBackground(new Color(-3355444)); 
      add(roc_button[17]); 
      
      roc_button[18].setText("ec1"); 
      roc_button[18].setActionCommand("ec1"); 
      roc_button[18].setBounds(252,228,61,25); 
      roc_button[18].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[18].setForeground(new Color(0)); 
      roc_button[18].setBackground(new Color(-3355444)); 
      add(roc_button[18]); 
      
      roc_button[19].setText("ec2"); 
      roc_button[19].setActionCommand("ec2"); 
      roc_button[19].setBounds(252,252,61,25); 
      roc_button[19].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[19].setForeground(new Color(0)); 
      roc_button[19].setBackground(new Color(-3355444)); 
      add(roc_button[19]); 
      
      roc_button[20].setText("tlv1"); 
      roc_button[20].setActionCommand("tlv1"); 
      roc_button[20].setBounds(252,276,61,25); 
      roc_button[20].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[20].setForeground(new Color(0)); 
      roc_button[20].setBackground(new Color(-3355444)); 
      add(roc_button[20]); 
      
      roc_button[0].setText("dc9"); 
      roc_button[0].setActionCommand("dc9"); 
      //roc_button[0].setToolTipText("This is roc_button[0] crate"); 
      roc_button[0].setBounds(60,180,61,25); 
      roc_button[0].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[0].setForeground(new Color(0)); 
      roc_button[0].setBackground(new Color(-3355444)); 
      add(roc_button[0]); 
      
      roc_button[1].setText("dc3"); 
      roc_button[1].setActionCommand("dc3"); 
      roc_button[1].setBounds(60,204,61,25); 
      roc_button[1].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[1].setForeground(new Color(0)); 
      roc_button[1].setBackground(new Color(-3355444)); 
      add(roc_button[1]); 
      
      roc_button[2].setText("dc4"); 
      roc_button[2].setActionCommand("dc4"); 
      roc_button[2].setBounds(60,228,61,25); 
      roc_button[2].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[2].setForeground(new Color(0)); 
      roc_button[2].setBackground(new Color(-3355444)); 
      add(roc_button[2]); 
      
      roc_button[3].setText("dc10"); 
      roc_button[3].setActionCommand("dc10"); 
      roc_button[3].setBounds(60,252,61,25); 
      roc_button[3].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[3].setForeground(new Color(0)); 
      roc_button[3].setBackground(new Color(-3355444)); 
      add(roc_button[3]); 
      
      roc_button[4].setText("tage"); 
      roc_button[4].setActionCommand("tage"); 
      roc_button[4].setBounds(60,276,61,25); 
      roc_button[4].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[4].setForeground(new Color(0)); 
      roc_button[4].setBackground(new Color(-3355444)); 
      add(roc_button[4]); 
      
      roc_button[7].setText("lac1"); 
      roc_button[7].setActionCommand("lac1"); 
      roc_button[7].setBounds(60,348,61,25); 
      roc_button[7].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[7].setForeground(new Color(0)); 
      roc_button[7].setBackground(new Color(-3355444)); 
      add(roc_button[7]); 
      
      roc_button[28].setText("scl4"); 
      roc_button[28].setActionCommand("scaler4"); 
      roc_button[28].setBounds(348,276,61,25); 
      roc_button[28].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[28].setForeground(new Color(0)); 
      roc_button[28].setBackground(new Color(-3355444)); 
      add(roc_button[28]); 
      
      roc_button[29].setText("scl3"); 
      roc_button[29].setActionCommand("scaler3"); 
      roc_button[29].setBounds(348,300,61,25); 
      roc_button[29].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[29].setForeground(new Color(0)); 
      roc_button[29].setBackground(new Color(-3355444)); 
      add(roc_button[29]); 
      
      roc_button[30].setText("scl1"); 
      roc_button[30].setActionCommand("scaler1"); 
      roc_button[30].setBounds(348,324,61,25); 
      roc_button[30].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[30].setForeground(new Color(0)); 
      roc_button[30].setBackground(new Color(-3355444)); 
      add(roc_button[30]); 
      
      roc_button[31].setText("scl2"); 
      roc_button[31].setActionCommand("scaler2"); 
      roc_button[31].setBounds(348,348,61,25); 
      roc_button[31].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[31].setForeground(new Color(0)); 
      roc_button[31].setBackground(new Color(-3355444)); 
      add(roc_button[31]); 
      
      roc_button[32].setText("cam1"); 
      roc_button[32].setActionCommand("camac1"); 
      roc_button[32].setBounds(480,216,61,25); 
      roc_button[32].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[32].setForeground(new Color(0)); 
      roc_button[32].setBackground(new Color(-3355444)); 
      add(roc_button[32]); 
      
      roc_button[33].setText("cam2"); 
      roc_button[33].setActionCommand("camac2"); 
      roc_button[33].setBounds(480,240,61,25); 
      roc_button[33].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[33].setForeground(new Color(0)); 
      roc_button[33].setBackground(new Color(-3355444)); 
      add(roc_button[33]); 
      
      roc_button[34].setText("cam3"); 
      roc_button[34].setActionCommand("camac3"); 
      roc_button[34].setBounds(480,264,61,25); 
      roc_button[34].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[34].setForeground(new Color(0)); 
      roc_button[34].setBackground(new Color(-3355444)); 
      add(roc_button[34]); 
      
      roc_button[35].setText("prt1"); 
      roc_button[35].setActionCommand("pretrig1"); 
      roc_button[35].setBounds(480,288,61,25); 
      roc_button[35].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[35].setForeground(new Color(0)); 
      roc_button[35].setBackground(new Color(-3355444)); 
      add(roc_button[35]); 
      
      roc_button[36].setText("prt2"); 
      roc_button[36].setActionCommand("pretrig2"); 
      roc_button[36].setBounds(480,312,61,25); 
      roc_button[36].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[36].setForeground(new Color(0)); 
      roc_button[36].setBackground(new Color(-3355444)); 
      add(roc_button[36]); 
      
      roc_button[37].setText("dchv"); 
      roc_button[37].setActionCommand("dccntrl"); 
      roc_button[37].setBounds(480,336,61,25); 
      roc_button[37].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[37].setForeground(new Color(0)); 
      roc_button[37].setBackground(new Color(-3355444)); 
      add(roc_button[37]); 
      
      roc_button[38].setText("sclas"); 
      roc_button[38].setActionCommand("sclaser"); 
      roc_button[38].setBounds(480,360,61,25); 
      roc_button[38].setFont(new Font("Serif", Font.BOLD, 11)); 
      roc_button[38].setForeground(new Color(0)); 
      roc_button[38].setBackground(new Color(-3355444)); 
      add(roc_button[38]); 
      
      
      
      jLabel1 = new com.sun.java.swing.JLabel(); 
      jLabel1.setText("TS.CSR"); 
      jLabel1.setBounds(444,84,48,24); 
      jLabel1.setFont(new Font("Serif", Font.BOLD, 12)); 
      jLabel1.setForeground(new Color(-10066279)); 
      jLabel1.setBackground(new Color(-3355444)); 
      add(jLabel1); 
      jLabel2 = new com.sun.java.swing.JLabel(); 
      jLabel2.setText("TS.State"); 
      jLabel2.setBounds(444,108,84,24); 
      jLabel2.setFont(new Font("Serif", Font.BOLD, 12)); 
      jLabel2.setForeground(new Color(-10066279)); 
      jLabel2.setBackground(new Color(-3355444)); 
      add(jLabel2); 
      jLabel4 = new com.sun.java.swing.JLabel(); 
      jLabel4.setText("Branch1"); 
      jLabel4.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
      jLabel4.setBounds(48,372,60,24); 
      jLabel4.setFont(new Font("Serif", Font.BOLD, 12)); 
      jLabel4.setForeground(new Color(-10066279)); 
      jLabel4.setBackground(new Color(-3355444)); 
      add(jLabel4); 
      jLabel5 = new com.sun.java.swing.JLabel(); 
      jLabel5.setText("Branch2"); 
      jLabel5.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
      jLabel5.setBounds(144,372,60,24); 
      jLabel5.setFont(new Font("Serif", Font.BOLD, 12)); 
      jLabel5.setForeground(new Color(-10066279)); 
      jLabel5.setBackground(new Color(-3355444)); 
      add(jLabel5); 
      jLabel6 = new com.sun.java.swing.JLabel(); 
      jLabel6.setText("Branch3"); 
      jLabel6.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
      jLabel6.setBounds(240,372,60,24); 
      jLabel6.setFont(new Font("Serif", Font.BOLD, 12)); 
      jLabel6.setForeground(new Color(-10066279)); 
      jLabel6.setBackground(new Color(-3355444)); 
      add(jLabel6); 
      jLabel7 = new com.sun.java.swing.JLabel(); 
      jLabel7.setText("Branch4"); 
      jLabel7.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
      jLabel7.setBounds(336,372,60,24); 
      jLabel7.setFont(new Font("Serif", Font.BOLD, 12)); 
      jLabel7.setForeground(new Color(-10066279)); 
      jLabel7.setBackground(new Color(-3355444)); 
      add(jLabel7); 
      EB = new com.sun.java.swing.JButton(); 
      EB.setText("EB"); 
      EB.setActionCommand("EB"); 
      EB.setBounds(36,444,60,36); 
      EB.setFont(new Font("Serif", Font.BOLD, 12)); 
      EB.setForeground(new Color(0)); 
      EB.setBackground(new Color(-3355444)); 
      add(EB); 
      ER = new com.sun.java.swing.JButton(); 
      ER.setText("ER"); 
      ER.setActionCommand("ER"); 
      ER.setBounds(324,444,60,36); 
      ER.setFont(new Font("Serif", Font.BOLD, 12)); 
      ER.setForeground(new Color(0)); 
      ER.setBackground(new Color(-3355444)); 
      add(ER); 
      Emptyb1 = new com.sun.java.swing.JButton(); 
      Emptyb1.setBounds(60,300,61,25); 
      Emptyb1.setFont(new Font("Dialog", Font.BOLD, 11)); 
      Emptyb1.setForeground(new Color(0)); 
      Emptyb1.setBackground(new Color(-3355444)); 
      add(Emptyb1); 
      Emptyb1.setEnabled(false); 
      Emptyb2 = new com.sun.java.swing.JButton(); 
      Emptyb2.setBounds(60,324,61,25); 
      Emptyb2.setFont(new Font("Dialog", Font.BOLD, 11)); 
      Emptyb2.setForeground(new Color(0)); 
      Emptyb2.setBackground(new Color(-3355444)); 
      add(Emptyb2); 
      Emptyb2.setEnabled(false); 
      Emptyb3 = new com.sun.java.swing.JButton(); 
      Emptyb3.setBounds(252,276,61,25); 
      Emptyb3.setFont(new Font("Dialog", Font.BOLD, 11)); 
      Emptyb3.setForeground(new Color(0)); 
      Emptyb3.setBackground(new Color(-3355444)); 
      add(Emptyb3); 
      Emptyb3.setEnabled(false); 
      Emptyb4 = new com.sun.java.swing.JButton(); 
      Emptyb4.setBounds(252,300,61,25); 
      Emptyb4.setFont(new Font("Dialog", Font.BOLD, 11)); 
      Emptyb4.setForeground(new Color(0)); 
      Emptyb4.setBackground(new Color(-3355444)); 
      add(Emptyb4); 
      Emptyb4.setEnabled(false); 
      Emptyb5 = new com.sun.java.swing.JButton(); 
      Emptyb5.setBounds(252,324,61,25); 
      Emptyb5.setFont(new Font("Dialog", Font.BOLD, 11)); 
      Emptyb5.setForeground(new Color(0)); 
      Emptyb5.setBackground(new Color(-3355444)); 
      add(Emptyb5); 
      Emptyb5.setEnabled(false); 
      Emptyb6 = new com.sun.java.swing.JButton(); 
      Emptyb6.setBounds(252,348,61,25); 
      Emptyb6.setFont(new Font("Dialog", Font.BOLD, 11)); 
      Emptyb6.setForeground(new Color(0)); 
      Emptyb6.setBackground(new Color(-3355444)); 
      add(Emptyb6); 
      Emptyb6.setEnabled(false); 
      Emptyb7 = new com.sun.java.swing.JButton(); 
      Emptyb7.setBounds(348,180,61,25); 
      Emptyb7.setFont(new Font("Dialog", Font.BOLD, 11)); 
      Emptyb7.setForeground(new Color(0)); 
      Emptyb7.setBackground(new Color(-3355444)); 
      add(Emptyb7); 
      Emptyb7.setEnabled(false); 
      Emptyb8 = new com.sun.java.swing.JButton(); 
      Emptyb8.setBounds(348,204,61,25); 
      Emptyb8.setFont(new Font("Dialog", Font.BOLD, 11)); 
      Emptyb8.setForeground(new Color(0)); 
      Emptyb8.setBackground(new Color(-3355444)); 
      add(Emptyb8); 
      Emptyb8.setEnabled(false); 
      Emptyb9 = new com.sun.java.swing.JButton(); 
      Emptyb9.setBounds(348,228,61,25); 
      Emptyb9.setFont(new Font("Dialog", Font.BOLD, 11)); 
      Emptyb9.setForeground(new Color(0)); 
      Emptyb9.setBackground(new Color(-3355444)); 
      add(Emptyb9); 
      Emptyb9.setEnabled(false); 
      Emptyb10 = new com.sun.java.swing.JButton(); 
      Emptyb10.setBounds(348,252,61,25); 
      Emptyb10.setFont(new Font("Dialog", Font.BOLD, 11)); 
      Emptyb10.setForeground(new Color(0)); 
      Emptyb10.setBackground(new Color(-3355444)); 
      add(Emptyb10); 
      Emptyb10.setEnabled(false); 
      jLabel8 = new com.sun.java.swing.JLabel(); 
      jLabel8.setText("Prescale"); 
      jLabel8.setBounds(444,60,60,24); 
      jLabel8.setFont(new Font("Serif", Font.BOLD, 12)); 
      jLabel8.setForeground(new Color(-10066279)); 
      jLabel8.setBackground(new Color(-3355444)); 
      add(jLabel8); 
      horizontalLine1 = new symantec.itools.awt.shape.HorizontalLine(); 
      horizontalLine1.setBounds(84,156,288,2); 
      add(horizontalLine1); 
      verticalLine1 = new symantec.itools.awt.shape.VerticalLine(); 
      verticalLine1.setBounds(216,132,2,24); 
      add(verticalLine1); 
      verticalLine2 = new symantec.itools.awt.shape.VerticalLine(); 
      verticalLine2.setBounds(84,156,2,24); 
      add(verticalLine2); 
      verticalLine3 = new symantec.itools.awt.shape.VerticalLine(); 
      verticalLine3.setBounds(180,156,2,24); 
      add(verticalLine3); 
      verticalLine4 = new symantec.itools.awt.shape.VerticalLine(); 
      verticalLine4.setBounds(276,156,2,24); 
      add(verticalLine4); 
      verticalLine5 = new symantec.itools.awt.shape.VerticalLine(); 
      verticalLine5.setBounds(372,156,2,24); 
      add(verticalLine5); 
      verticalLine6 = new symantec.itools.awt.shape.VerticalLine(); 
      verticalLine6.setBounds(240,192,2,204); 
      add(verticalLine6); 
      horizontalLine2 = new symantec.itools.awt.shape.HorizontalLine(); 
      horizontalLine2.setBounds(60,396,372,2); 
      add(horizontalLine2); 
      dc2_line = new symantec.itools.awt.shape.HorizontalLine(); 
      dc2_line.setBounds(216,192,24,2); 
      add(dc2_line); 
      dc8_line = new symantec.itools.awt.shape.HorizontalLine(); 
      dc8_line.setBounds(216,216,24,2); 
      add(dc8_line); 
      dc5_line = new symantec.itools.awt.shape.HorizontalLine(); 
      dc5_line.setBounds(216,240,24,2); 
      add(dc5_line); 
      dc1_line = new symantec.itools.awt.shape.HorizontalLine(); 
      dc1_line.setBounds(216,264,24,2); 
      add(dc1_line); 
      dc7_line = new symantec.itools.awt.shape.HorizontalLine(); 
      dc7_line.setBounds(216,288,24,2); 
      add(dc7_line); 
      dc6_line = new symantec.itools.awt.shape.HorizontalLine(); 
      dc6_line.setBounds(216,312,24,2); 
      add(dc6_line); 
      dc11_line = new symantec.itools.awt.shape.HorizontalLine(); 
      dc11_line.setBounds(216,336,24,2); 
      add(dc11_line); 
      polar_line = new symantec.itools.awt.shape.HorizontalLine(); 
      polar_line.setBounds(216,360,24,2); 
      add(polar_line); 
      verticalLine7 = new symantec.itools.awt.shape.VerticalLine(); 
      verticalLine7.setBounds(144,192,2,204); 
      add(verticalLine7); 
      dc9_line = new symantec.itools.awt.shape.HorizontalLine(); 
      dc9_line.setBounds(120,192,24,2); 
      add(dc9_line); 
      dc3_line = new symantec.itools.awt.shape.HorizontalLine(); 
      dc3_line.setBounds(120,216,24,2); 
      add(dc3_line); 
      dc4_line = new symantec.itools.awt.shape.HorizontalLine(); 
      dc4_line.setBounds(120,240,24,2); 
      add(dc4_line); 
      dc10_line = new symantec.itools.awt.shape.HorizontalLine(); 
      dc10_line.setBounds(120,264,24,2); 
      add(dc10_line); 
      tage_line = new symantec.itools.awt.shape.HorizontalLine(); 
      tage_line.setBounds(120,288,24,2); 
      add(tage_line); 
      horizontalLine16 = new symantec.itools.awt.shape.HorizontalLine(); 
      horizontalLine16.setBounds(120,312,24,2); 
      horizontalLine16.setForeground(new Color(8421504)); 
      add(horizontalLine16); 
      horizontalLine16.setEnabled(false); 
      horizontalLine17 = new symantec.itools.awt.shape.HorizontalLine(); 
      horizontalLine17.setBounds(120,336,24,2); 
      horizontalLine17.setForeground(new Color(8421504)); 
      add(horizontalLine17); 
      horizontalLine17.setEnabled(false); 
      lac1_line = new symantec.itools.awt.shape.HorizontalLine(); 
      lac1_line.setBounds(120,360,24,2); 
      add(lac1_line); 
      verticalLine8 = new symantec.itools.awt.shape.VerticalLine(); 
      verticalLine8.setBounds(336,192,2,204); 
      add(verticalLine8); 
      sc1_line = new symantec.itools.awt.shape.HorizontalLine(); 
      sc1_line.setBounds(312,192,24,2); 
      add(sc1_line); 
      cc1_line = new symantec.itools.awt.shape.HorizontalLine(); 
      cc1_line.setBounds(312,216,24,2); 
      add(cc1_line); 
      ec1_line = new symantec.itools.awt.shape.HorizontalLine(); 
      ec1_line.setBounds(312,240,24,2); 
      add(ec1_line); 
      ec2_line = new symantec.itools.awt.shape.HorizontalLine(); 
      ec2_line.setBounds(312,264,24,2); 
      add(ec2_line); 
      horizontalLine23 = new symantec.itools.awt.shape.HorizontalLine(); 
      horizontalLine23.setBounds(312,288,24,2); 
      horizontalLine23.setForeground(new Color(8421504)); 
      add(horizontalLine23); 
      horizontalLine23.setEnabled(false); 
      horizontalLine24 = new symantec.itools.awt.shape.HorizontalLine(); 
      horizontalLine24.setBounds(312,312,24,2); 
      horizontalLine24.setForeground(new Color(8421504)); 
      add(horizontalLine24); 
      horizontalLine24.setEnabled(false); 
      horizontalLine25 = new symantec.itools.awt.shape.HorizontalLine(); 
      horizontalLine25.setBounds(312,336,24,2); 
      horizontalLine25.setForeground(new Color(8421504)); 
      add(horizontalLine25); 
      horizontalLine25.setEnabled(false); 
      horizontalLine26 = new symantec.itools.awt.shape.HorizontalLine(); 
      horizontalLine26.setBounds(312,360,24,2); 
      horizontalLine26.setForeground(new Color(8421504)); 
      add(horizontalLine26); 
      horizontalLine26.setEnabled(false); 
      verticalLine9 = new symantec.itools.awt.shape.VerticalLine(); 
      verticalLine9.setBounds(432,192,2,204); 
      add(verticalLine9); 
      horizontalLine27 = new symantec.itools.awt.shape.HorizontalLine(); 
      horizontalLine27.setBounds(408,192,24,2); 
      horizontalLine27.setForeground(new Color(8421504)); 
      add(horizontalLine27); 
      horizontalLine27.setEnabled(false); 
      horizontalLine28 = new symantec.itools.awt.shape.HorizontalLine(); 
      horizontalLine28.setBounds(408,216,24,2); 
      horizontalLine28.setForeground(new Color(8421504)); 
      add(horizontalLine28); 
      horizontalLine28.setEnabled(false); 
      horizontalLine29 = new symantec.itools.awt.shape.HorizontalLine(); 
      horizontalLine29.setBounds(408,240,24,2); 
      horizontalLine29.setForeground(new Color(8421504)); 
      add(horizontalLine29); 
      horizontalLine29.setEnabled(false); 
      horizontalLine30 = new symantec.itools.awt.shape.HorizontalLine(); 
      horizontalLine30.setBounds(408,264,24,2); 
      horizontalLine30.setForeground(new Color(8421504)); 
      add(horizontalLine30); 
      horizontalLine30.setEnabled(false); 
      scaler4_line = new symantec.itools.awt.shape.HorizontalLine(); 
      scaler4_line.setBounds(408,288,24,2); 
      add(scaler4_line); 
      scaler3_line = new symantec.itools.awt.shape.HorizontalLine(); 
      scaler3_line.setBounds(408,312,24,2); 
      add(scaler3_line); 
      scaler1_line = new symantec.itools.awt.shape.HorizontalLine(); 
      scaler1_line.setBounds(408,336,24,2); 
      add(scaler1_line); 
      scaler2_line = new symantec.itools.awt.shape.HorizontalLine(); 
      scaler2_line.setBounds(408,360,24,2); 
      add(scaler2_line); 
      verticalLine10 = new symantec.itools.awt.shape.VerticalLine(); 
      verticalLine10.setBounds(60,396,2,48); 
      add(verticalLine10); 
      ET = new com.sun.java.swing.JButton(); 
      ET.setText("E04"); 
      ET.setActionCommand("ET"); 
      ET.setBounds(252,444,60,36); 
      ET.setFont(new Font("Serif", Font.BOLD, 12)); 
      ET.setForeground(new Color(0)); 
      ET.setBackground(new Color(-3355444)); 
      add(ET); 

      ET00 = new com.sun.java.swing.JButton(); 
      ET00.setText("E0"); 
      ET00.setActionCommand("E00"); 
      ET00.setBounds(260,490,60,36); 
      ET00.setFont(new Font("Serif", Font.BOLD, 12)); 
      ET00.setForeground(new Color(0)); 
      ET00.setBackground(new Color(-3355444)); 
      add(ET00); 

      jLabel10 = new com.sun.java.swing.JLabel(); 
      jLabel10.setText("1"); 
      jLabel10.setBounds(36,36,12,12); 
      jLabel10.setFont(new Font("Serif", Font.BOLD, 12)); 
      jLabel10.setForeground(new Color(-10066279)); 
      jLabel10.setBackground(new Color(-3355444)); 
      add(jLabel10); 
      jLabel11 = new com.sun.java.swing.JLabel(); 
      jLabel11.setText("12"); 
      jLabel11.setBounds(420,36,24,12); 
      jLabel11.setFont(new Font("Serif", Font.BOLD, 12)); 
      jLabel11.setForeground(new Color(-10066279)); 
      jLabel11.setBackground(new Color(-3355444)); 
      add(jLabel11); 
      jLabel12 = new com.sun.java.swing.JLabel(); 
      jLabel12.setText("1"); 
      jLabel12.setBounds(36,132,12,12); 
      jLabel12.setFont(new Font("Serif", Font.BOLD, 12)); 
      jLabel12.setForeground(new Color(-10066279)); 
      jLabel12.setBackground(new Color(-3355444)); 
      add(jLabel12); 
      jLabel13 = new com.sun.java.swing.JLabel(); 
      jLabel13.setText("32"); 
      jLabel13.setBounds(408,132,24,12); 
      jLabel13.setFont(new Font("Serif", Font.BOLD, 12)); 
      jLabel13.setForeground(new Color(-10066279)); 
      jLabel13.setBackground(new Color(-3355444)); 
      add(jLabel13); 
      raid1 = new com.sun.java.swing.JButton(); 
      raid1.setText("d1"); 
      raid1.setActionCommand("RD1"); 
      raid1.setBounds(504,422,48,17); 
      raid1.setFont(new Font("Dialog", Font.BOLD, 9)); 
      raid1.setForeground(new Color(0)); 
      raid1.setBackground(new Color(-3355444)); 
      add(raid1); 
      raid2 = new com.sun.java.swing.JButton(); 
      raid2.setText("d2"); 
      raid2.setActionCommand("RD2"); 
      raid2.setBounds(504,439,48,17); 
      raid2.setFont(new Font("Dialog", Font.BOLD, 9)); 
      raid2.setForeground(new Color(0)); 
      raid2.setBackground(new Color(-3355444)); 
      add(raid2); 
      raid3 = new com.sun.java.swing.JButton(); 
      raid3.setText("d3"); 
      raid3.setActionCommand("RD3"); 
      raid3.setBounds(504,456,48,17); 
      raid3.setFont(new Font("Dialog", Font.BOLD, 9)); 
      raid3.setForeground(new Color(0)); 
      raid3.setBackground(new Color(-3355444)); 
      add(raid3); 
      raid4 = new com.sun.java.swing.JButton(); 
      raid4.setText("d4"); 
      raid4.setActionCommand("RD4"); 
      raid4.setBounds(504,473,48,17); 
      raid4.setFont(new Font("Dialog", Font.BOLD, 9)); 
      raid4.setForeground(new Color(0)); 
      raid4.setBackground(new Color(-3355444)); 
      add(raid4); 
      raid5 = new com.sun.java.swing.JButton(); 
      raid5.setText("d5"); 
      raid5.setActionCommand("RD5"); 
      raid5.setBounds(504,490,48,17); 
      raid5.setFont(new Font("Dialog", Font.BOLD, 9)); 
      raid5.setForeground(new Color(0)); 
      raid5.setBackground(new Color(-3355444)); 
      add(raid5); 
      raid6 = new com.sun.java.swing.JButton(); 
      raid6.setText("d6"); 
      raid6.setActionCommand("RD6"); 
      raid6.setBounds(504,507,48,17); 
      raid6.setFont(new Font("Dialog", Font.BOLD, 9)); 
      raid6.setForeground(new Color(0)); 
      raid6.setBackground(new Color(-3355444)); 
      add(raid6); 
      raid7 = new com.sun.java.swing.JButton(); 
      raid7.setText("d7"); 
      raid7.setActionCommand("RD7"); 
      raid7.setBounds(504,524,48,17); 
      raid7.setFont(new Font("Dialog", Font.BOLD, 9)); 
      raid7.setForeground(new Color(0)); 
      raid7.setBackground(new Color(-3355444)); 
      add(raid7); 
      raid8 = new com.sun.java.swing.JButton(); 
      raid8.setText("d8"); 
      raid8.setActionCommand("RD8"); 
      raid8.setBounds(504,541,48,17); 
      raid8.setFont(new Font("Dialog", Font.BOLD, 9)); 
      raid8.setForeground(new Color(0)); 
      raid8.setBackground(new Color(-3355444)); 
      add(raid8); 
      er_line = new symantec.itools.awt.shape.HorizontalLine(); 
      er_line.setBounds(384,468,12,2); 
      add(er_line); 
      raid2_line = new symantec.itools.awt.shape.HorizontalLine(); 
      raid2_line.setBounds(480,444,24,2); 
      add(raid2_line); 
      raid3_line = new symantec.itools.awt.shape.HorizontalLine(); 
      raid3_line.setBounds(480,461,24,2); 
      add(raid3_line); 
      raid4_line = new symantec.itools.awt.shape.HorizontalLine(); 
      raid4_line.setBounds(480,478,24,2); 
      add(raid4_line); 
      raid5_line = new symantec.itools.awt.shape.HorizontalLine(); 
      raid5_line.setBounds(480,495,24,2); 
      add(raid5_line); 
      raid6_line = new symantec.itools.awt.shape.HorizontalLine(); 
      raid6_line.setBounds(480,512,24,2); 
      add(raid6_line); 
      raid7_line = new symantec.itools.awt.shape.HorizontalLine(); 
      raid7_line.setBounds(480,529,24,2); 
      add(raid7_line); 
      raid8_line = new symantec.itools.awt.shape.HorizontalLine(); 
      raid8_line.setBounds(480,546,24,2); 
      add(raid8_line); 
      raid1_line = new symantec.itools.awt.shape.HorizontalLine(); 
      raid1_line.setBounds(480,427,24,2); 
      add(raid1_line); 
      verticalLine11 = new symantec.itools.awt.shape.VerticalLine(); 
      verticalLine11.setBounds(480,427,2,120); 
      add(verticalLine11); 
      raid1silo = new symantec.itools.awt.shape.HorizontalLine(); 
      raid1silo.setBounds(552,427,24,2); 
      add(raid1silo); 
      raid2silo = new symantec.itools.awt.shape.HorizontalLine(); 
      raid2silo.setBounds(552,444,24,2); 
      add(raid2silo); 
      raid3silo = new symantec.itools.awt.shape.HorizontalLine(); 
      raid3silo.setBounds(552,461,24,2); 
      add(raid3silo); 
      raid4silo = new symantec.itools.awt.shape.HorizontalLine(); 
      raid4silo.setBounds(552,478,24,2); 
      add(raid4silo); 
      raid5silo = new symantec.itools.awt.shape.HorizontalLine(); 
      raid5silo.setBounds(552,495,24,2); 
      add(raid5silo); 
      raid6silo = new symantec.itools.awt.shape.HorizontalLine(); 
      raid6silo.setBounds(552,512,24,2); 
      add(raid6silo); 
      raid7silo = new symantec.itools.awt.shape.HorizontalLine(); 
      raid7silo.setBounds(552,529,24,2); 
      add(raid7silo); 
      raid8silo = new symantec.itools.awt.shape.HorizontalLine(); 
      raid8silo.setBounds(552,546,24,2); 
      add(raid8silo); 
      jLabel9 = new com.sun.java.swing.JLabel(); 
      jLabel9.setText("S"); 
		jLabel9.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel9.setBounds(576,432,36,24); 
		jLabel9.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel9.setForeground(new Color(-10066279)); 
		jLabel9.setBackground(new Color(-3355444)); 
		add(jLabel9); 
		jLabel14 = new com.sun.java.swing.JLabel(); 
		jLabel14.setText("I"); 
		jLabel14.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel14.setBounds(576,466,36,24); 
		jLabel14.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel14.setForeground(new Color(-10066279)); 
		jLabel14.setBackground(new Color(-3355444)); 
		add(jLabel14); 
		jLabel15 = new com.sun.java.swing.JLabel(); 
		jLabel15.setText("L"); 
		jLabel15.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel15.setBounds(576,500,36,24); 
		jLabel15.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel15.setForeground(new Color(-10066279)); 
		jLabel15.setBackground(new Color(-3355444)); 
		add(jLabel15); 
		jLabel16 = new com.sun.java.swing.JLabel(); 
		jLabel16.setText("O"); 
		jLabel16.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel16.setBounds(576,534,36,24); 
		jLabel16.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel16.setForeground(new Color(-10066279)); 
		jLabel16.setBackground(new Color(-3355444)); 
		add(jLabel16); 
		jLabel3 = new com.sun.java.swing.JLabel(); 
		jLabel3.setText("Level1 [KHz]"); 
		jLabel3.setBounds(444,12,108,24); 
		jLabel3.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel3.setForeground(new Color(-10066279)); 
		jLabel3.setBackground(new Color(-3355444)); 
		add(jLabel3); 
		jLabel17 = new com.sun.java.swing.JLabel(); 
		jLabel17.setText("TS.Input"); 
		jLabel17.setBounds(444,36,60,24); 
		jLabel17.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel17.setForeground(new Color(-10066279)); 
		jLabel17.setBackground(new Color(-3355444)); 
		add(jLabel17); 
		horizontalLine3 = new symantec.itools.awt.shape.HorizontalLine(); 
		horizontalLine3.setBounds(468,468,12,2); 
      		add(horizontalLine3); 
		horizontalLine4 = new symantec.itools.awt.shape.HorizontalLine(); 
		horizontalLine4.setBounds(96,468,12,2); 
		add(horizontalLine4); 
		horizontalLine5 = new symantec.itools.awt.shape.HorizontalLine(); 
		horizontalLine5.setBounds(168,468,84,2); 
		add(horizontalLine5); 
		horizontalLine6 = new symantec.itools.awt.shape.HorizontalLine(); 
		horizontalLine6.setBounds(312,468,12,2); 
		add(horizontalLine6); 
		verticalLine12 = new symantec.itools.awt.shape.VerticalLine(); 
		verticalLine12.setBounds(288,480,2,24); 
		add(verticalLine12); 
		jLabel18 = new com.sun.java.swing.JLabel(); 
		jLabel18.setText("Hz"); 
		jLabel18.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel18.setBounds(120,432,24,12); 
		jLabel18.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel18.setForeground(new Color(-10066279)); 
		jLabel18.setBackground(new Color(-3355444)); 
		add(jLabel18); 
		jLabel19 = new com.sun.java.swing.JLabel(); 
		jLabel19.setText("Hz"); 
		jLabel19.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel19.setBounds(408,420,24,12); 
		jLabel19.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel19.setForeground(new Color(-10066279)); 
		jLabel19.setBackground(new Color(-3355444)); 
		add(jLabel19); 
		jLabel20 = new com.sun.java.swing.JLabel(); 
		jLabel20.setText("KByte/s"); 
		jLabel20.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel20.setBounds(108,492,60,12); 
		jLabel20.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel20.setForeground(new Color(-10066279)); 
		jLabel20.setBackground(new Color(-3355444)); 
		add(jLabel20); 
		jLabel21 = new com.sun.java.swing.JLabel(); 
		jLabel21.setText("KByte/s"); 
		jLabel21.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel21.setBounds(396,504,60,12); 
		jLabel21.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel21.setForeground(new Color(-10066279)); 
		jLabel21.setBackground(new Color(-3355444)); 
		add(jLabel21); 
		It2ipc = new com.sun.java.swing.JButton(); 
		It2ipc.setText("E4"); 
		It2ipc.setActionCommand("ET"); 
		It2ipc.setBounds(36,564,60,36); 
		It2ipc.setFont(new Font("Serif", Font.BOLD, 12)); 
		It2ipc.setForeground(new Color(0)); 
		It2ipc.setBackground(new Color(-3355444)); 
		add(It2ipc); 
		horizontalLine7 = new symantec.itools.awt.shape.HorizontalLine(); 
		horizontalLine7.setBounds(60,530,195,2); 
       		add(horizontalLine7); 
		verticalLine13 = new symantec.itools.awt.shape.VerticalLine(); 
		verticalLine13.setBounds(60,530,2,60); 
		add(verticalLine13); 
	
		verticalLine113 = new symantec.itools.awt.shape.VerticalLine(); 
		verticalLine113.setBounds(255,479,2,52); 
		add(verticalLine113); 
		
		horizontalLine88 = new symantec.itools.awt.shape.HorizontalLine(); 
		horizontalLine88.setBounds(239,475,15,2); 
		add(horizontalLine88); 

		horizontalLine99 = new symantec.itools.awt.shape.HorizontalLine(); 
		horizontalLine99.setBounds(315,509,15,2); 
		add(horizontalLine99); 

		horizontalLine8 = new symantec.itools.awt.shape.HorizontalLine(); 
		horizontalLine8.setBounds(96,588,36,2); 
		add(horizontalLine8); 
		horizontalLine9 = new symantec.itools.awt.shape.HorizontalLine(); 
		horizontalLine9.setBounds(192,588,12,2); 
		add(horizontalLine9); 
		Ipc2dd = new com.sun.java.swing.JButton(); 
		Ipc2dd.setText("ET"); 
		Ipc2dd.setActionCommand("ET"); 
		Ipc2dd.setBounds(204,564,60,36); 
		Ipc2dd.setFont(new Font("Serif", Font.BOLD, 12)); 
		Ipc2dd.setForeground(new Color(0)); 
		Ipc2dd.setBackground(new Color(-3355444)); 
		add(Ipc2dd); 
		horizontalLine10 = new symantec.itools.awt.shape.HorizontalLine(); 
		horizontalLine10.setBounds(336,588,12,2); 
		add(horizontalLine10); 
		Recsis = new com.sun.java.swing.JButton(); 
		Recsis.setText("Rec"); 
		Recsis.setActionCommand("ET"); 
		//		Recsis.setHorizontalAlignment(com.sun.java.swing.SwingConstants.LEFT); 
		Recsis.setBounds(348,564,60,36); 
		Recsis.setFont(new Font("Serif", Font.BOLD, 12)); 
		Recsis.setForeground(new Color(0)); 
		Recsis.setBackground(new Color(-3355444)); 
		add(Recsis); 
		horizontalLine11 = new symantec.itools.awt.shape.HorizontalLine(); 
		horizontalLine11.setBounds(264,588,12,2); 
		add(horizontalLine11); 
		jLabel22 = new com.sun.java.swing.JLabel(); 
		jLabel22.setText("Hz"); 
		jLabel22.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel22.setBounds(144,564,24,12); 
		jLabel22.setFont(new Font("Dialog", Font.BOLD, 12)); 
		jLabel22.setForeground(new Color(-10066279)); 
		jLabel22.setBackground(new Color(-3355444)); 
		add(jLabel22); 
		jLabel23 = new com.sun.java.swing.JLabel(); 
		jLabel23.setText("Hz"); 
		jLabel23.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel23.setBounds(288,564,24,12); 
		jLabel23.setFont(new Font("Dialog", Font.BOLD, 12)); 
		jLabel23.setForeground(new Color(-10066279)); 
		jLabel23.setBackground(new Color(-3355444)); 
		add(jLabel23); 
		horizontalLine12 = new symantec.itools.awt.shape.HorizontalLine(); 
		horizontalLine12.setBounds(100,612,200,2); 
		add(horizontalLine12); 
		verticalLineTrg = new symantec.itools.awt.shape.VerticalLine(); 
		verticalLineTrg.setBounds(100,612,2,12); 
		add(verticalLineTrg); 
		verticalLine14 = new symantec.itools.awt.shape.VerticalLine(); 
		verticalLine14.setBounds(225,600,2,12); 
		add(verticalLine14); 
		Ipc2it = new com.sun.java.swing.JButton(); 
		Ipc2it.setText("IPC"); 
		Ipc2it.setActionCommand("ET"); 
		Ipc2it.setBounds(180,432,60,24); 
		Ipc2it.setFont(new Font("Serif", Font.BOLD, 12)); 
		Ipc2it.setForeground(new Color(0)); 
		Ipc2it.setBackground(new Color(-3355444)); 
		add(Ipc2it); 
		verticalLine15 = new symantec.itools.awt.shape.VerticalLine(); 
		verticalLine15.setBounds(204,456,2,12); 
		add(verticalLine15); 
		Stadis = new com.sun.java.swing.JButton(); 
		Stadis.setText("EsM"); 
		Stadis.setActionCommand("ET"); 
		//		Stadis.setHorizontalAlignment(com.sun.java.swing.SwingConstants.LEFT); 
		Stadis.setBounds(156,624,60,36); 
		Stadis.setFont(new Font("Serif", Font.BOLD, 12)); 
		Stadis.setForeground(new Color(0)); 
		Stadis.setBackground(new Color(-3355444)); 
		add(Stadis);
		Trg_monitor = new com.sun.java.swing.JButton(); 
		Trg_monitor.setText("Trg"); 
		Trg_monitor.setActionCommand("ET"); 
		//		Trg_monitor.setHorizontalAlignment(com.sun.java.swing.SwingConstants.LEFT); 
		Trg_monitor.setBounds(76,624,60,36); 
		Trg_monitor.setFont(new Font("Serif", Font.BOLD, 12)); 
		Trg_monitor.setForeground(new Color(0)); 
		Trg_monitor.setBackground(new Color(-3355444)); 
		add(Trg_monitor); 
		Scaler = new com.sun.java.swing.JButton(); 
		Scaler.setText("SCS"); 
		Scaler.setActionCommand("ET"); 
		//		Scaler.setHorizontalAlignment(com.sun.java.swing.SwingConstants.LEFT); 
		Scaler.setBounds(180,475,60,24); 
		Scaler.setFont(new Font("Serif", Font.BOLD, 12)); 
		Scaler.setForeground(new Color(0)); 
		Scaler.setBackground(new Color(-3355444)); 
		add(Scaler); 
		Evt_monitor = new com.sun.java.swing.JButton(); 
		Evt_monitor.setText("Mon"); 
		Evt_monitor.setActionCommand("ET"); 
		//		Evt_monitor.setHorizontalAlignment(com.sun.java.swing.SwingConstants.LEFT); 
		Evt_monitor.setBounds(372,624,60,36); 
		Evt_monitor.setFont(new Font("Serif", Font.BOLD, 12)); 
		Evt_monitor.setForeground(new Color(0)); 
		Evt_monitor.setBackground(new Color(-3355444)); 
		add(Evt_monitor); 
		verticalLine16 = new symantec.itools.awt.shape.VerticalLine(); 
		verticalLine16.setBounds(408,612,2,12); 
       		add(verticalLine16); 
		verticalLine17 = new symantec.itools.awt.shape.VerticalLine(); 
		verticalLine17.setBounds(264,612,2,12); 
		//	add(verticalLine17); 
		verticalLine18 = new symantec.itools.awt.shape.VerticalLine(); 
		verticalLine18.setBounds(180,612,2,12); 
		add(verticalLine18); 
		rect1 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect1.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect1.setBounds(12,552,444,120); 
		add(rect1); 
		rect2 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect2.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect2.setBounds(12,420,552,120); 
		add(rect2); 
		rect3 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect3.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect3.setBounds(2,686,420,270); 
		//add(rect3); 
		rect4 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect4.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect4.setBounds(12,168,444,240); 
		add(rect4); 
		rect6 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect6.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect6.setBounds(576,420,37,170); 
		add(rect6); 
		rect16 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect16.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect16.setBounds(464,539,100,50); 
		add(rect16); 
		jLabel24 = new com.sun.java.swing.JLabel(); 
		jLabel24.setText("clon10"); 
		jLabel24.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel24.setBounds(12,516,48,24); 
		jLabel24.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel24.setForeground(new Color(-10066279)); 
		jLabel24.setBackground(new Color(-3355444)); 
		add(jLabel24); 
		jLabel25 = new com.sun.java.swing.JLabel(); 
		jLabel25.setText("clon04"); 
		jLabel25.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel25.setBounds(12,648,48,24); 
		jLabel25.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel25.setForeground(new Color(-10066279)); 
		jLabel25.setBackground(new Color(-3355444)); 
		add(jLabel25); 
		jLabel26 = new com.sun.java.swing.JLabel(); 
		jLabel26.setText("clon04"); 
		jLabel26.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel26.setBounds(300,648,48,24); 
		jLabel26.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel26.setForeground(new Color(-10066279)); 
		jLabel26.setBackground(new Color(-3355444)); 
		//add(jLabel26); 
		jLabel27 = new com.sun.java.swing.JLabel(); 
		jLabel27.setText("D"); 
		jLabel27.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel27.setBounds(12,192,36,24); 
		jLabel27.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel27.setForeground(new Color(-10066279)); 
		jLabel27.setBackground(new Color(-3355444)); 
		add(jLabel27); 
		jLabel28 = new com.sun.java.swing.JLabel(); 
		jLabel28.setText("A"); 
		jLabel28.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel28.setBounds(12,216,36,24); 
		jLabel28.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel28.setForeground(new Color(-10066279)); 
		jLabel28.setBackground(new Color(-3355444)); 
		add(jLabel28); 
		jLabel29 = new com.sun.java.swing.JLabel(); 
		jLabel29.setText("Q"); 
		jLabel29.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel29.setBounds(12,240,36,24); 
		jLabel29.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel29.setForeground(new Color(-10066279)); 
		jLabel29.setBackground(new Color(-3355444)); 
		add(jLabel29); 
		jLabel30 = new com.sun.java.swing.JLabel(); 
		jLabel30.setText("R"); 
		jLabel30.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel30.setBounds(12,288,36,24); 
		jLabel30.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel30.setForeground(new Color(-10066279)); 
		jLabel30.setBackground(new Color(-3355444)); 
		add(jLabel30); 
		jLabel31 = new com.sun.java.swing.JLabel(); 
		jLabel31.setText("O"); 
		jLabel31.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel31.setBounds(12,312,36,24); 
		jLabel31.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel31.setForeground(new Color(-10066279)); 
		jLabel31.setBackground(new Color(-3355444)); 
		add(jLabel31); 
		jLabel32 = new com.sun.java.swing.JLabel(); 
		jLabel32.setText("C"); 
		jLabel32.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel32.setBounds(12,336,36,24); 
		jLabel32.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel32.setForeground(new Color(-10066279)); 
		jLabel32.setBackground(new Color(-3355444)); 
		add(jLabel32); 
		jLabel33 = new com.sun.java.swing.JLabel(); 
		jLabel33.setText("S"); 
		jLabel33.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel33.setBounds(12,360,36,24); 
		jLabel33.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel33.setForeground(new Color(-10066279)); 
		jLabel33.setBackground(new Color(-3355444)); 
		add(jLabel33); 
		horizontalLine13 = new symantec.itools.awt.shape.HorizontalLine(); 
		horizontalLine13.setBounds(360,612,48,2); 
		add(horizontalLine13); 
		jLabel34 = new com.sun.java.swing.JLabel(); 
		jLabel34.setText("Hz"); 
		jLabel34.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel34.setBounds(324,624,24,12); 
		jLabel34.setFont(new Font("Dialog", Font.BOLD, 12)); 
		jLabel34.setForeground(new Color(-10066279)); 
		jLabel34.setBackground(new Color(-3355444)); 
		add(jLabel34);
 
		rect7 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect7.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect7.setBounds(468,168,87,237); 
		add(rect7); 
		jLabel35 = new com.sun.java.swing.JLabel(); 
		jLabel35.setText("Control"); 
		jLabel35.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel35.setBounds(468,168,84,24); 
		jLabel35.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel35.setForeground(new Color(-10066279)); 
		jLabel35.setBackground(new Color(-3355444)); 
		add(jLabel35); 
		jLabel36 = new com.sun.java.swing.JLabel(); 
		jLabel36.setText("ROCS"); 
		jLabel36.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel36.setBounds(468,192,84,24); 
		jLabel36.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel36.setForeground(new Color(-10066279)); 
		jLabel36.setBackground(new Color(-3355444)); 
		add(jLabel36); 
		run_number = new symantec.itools.awt.Label3D(); 
		try { 
			run_number.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			run_number.setText("178345"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			run_number.setTextColor(new java.awt.Color(-16760704)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		run_number.setBounds(588,24,108,36); 
		run_number.setFont(new Font("Serif", Font.BOLD, 24)); 
		add(run_number); 
		number_events = new symantec.itools.awt.Label3D(); 
		try { 
			number_events.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			number_events.setText("11123123"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			number_events.setTextColor(new java.awt.Color(-16760704)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		number_events.setBounds(852,24,120,36); 
		number_events.setFont(new Font("Serif", Font.BOLD, 24)); 
		add(number_events); 
		rect8 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect8.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect8.setBounds(564,0,422,82); 
		add(rect8); 
		jLabel37 = new com.sun.java.swing.JLabel(); 
		jLabel37.setText("RUN"); 
		jLabel37.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel37.setBounds(756,0,36,24); 
		jLabel37.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel37.setForeground(new Color(-10066279)); 
		jLabel37.setBackground(new Color(-3355444)); 
		add(jLabel37); 
		jLabel38 = new com.sun.java.swing.JLabel(); 
		jLabel38.setText("number"); 
		jLabel38.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel38.setBounds(612,60,72,24); 
		jLabel38.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel38.setForeground(new Color(-10066279)); 
		jLabel38.setBackground(new Color(-3355444)); 
		add(jLabel38); 
		jLabel39 = new com.sun.java.swing.JLabel(); 
		jLabel39.setText("configuration"); 
		jLabel39.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel39.setBounds(720,60,108,24); 
		jLabel39.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel39.setForeground(new Color(-10066279)); 
		jLabel39.setBackground(new Color(-3355444)); 
		add(jLabel39); 
		jLabel40 = new com.sun.java.swing.JLabel(); 
		jLabel40.setText("accepted triggers"); 
		jLabel40.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel40.setBounds(840,60,144,24); 
		jLabel40.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel40.setForeground(new Color(-10066279)); 
		jLabel40.setBackground(new Color(-3355444)); 
		add(jLabel40); 
		daq_recover = new com.sun.java.swing.JButton(); 
		daq_recover.setText("Recove"); 
		daq_recover.setActionCommand("Recover"); 
		daq_recover.setBounds(888,624,84,36); 
		daq_recover.setFont(new Font("Serif", Font.BOLD, 12)); 
		daq_recover.setForeground(new Color(0)); 
		daq_recover.setBackground(new Color(-3355444)); 
		add(daq_recover); 
		daq_recover.setEnabled(false);
		run_config = new symantec.itools.awt.Label3D(); 
		try { 
			run_config.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			run_config.setText("11123123"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			run_config.setTextColor(new java.awt.Color(-16760704)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		run_config.setBounds(708,24,132,36); 
		run_config.setFont(new Font("Serif", Font.BOLD, 12)); 
		add(run_config); 
		rect10 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect10.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect10.setBounds(840,612,146,60); 
		add(rect10); 
		jLabel41 = new com.sun.java.swing.JLabel(); 
		jLabel41.setText("DAQ"); 
		jLabel41.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel41.setBounds(840,636,48,24); 
		jLabel41.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel41.setForeground(new Color(-10066279)); 
		jLabel41.setBackground(new Color(-3355444)); 
		add(jLabel41); 
		online_recover = new com.sun.java.swing.JButton(); 
		online_recover.setText("Recover"); 
		online_recover.setActionCommand("Recover"); 
		online_recover.setBounds(732,624,84,36); 
		online_recover.setFont(new Font("Serif", Font.BOLD, 12)); 
		online_recover.setForeground(new Color(0)); 
		online_recover.setBackground(new Color(-3355444)); 
		add(online_recover);
		online_recover.setEnabled(false); 
		rect12 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect12.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect12.setBounds(684,612,146,60); 
		add(rect12); 
		jLabel42 = new com.sun.java.swing.JLabel(); 
		jLabel42.setText("Online"); 
		jLabel42.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel42.setBounds(684,636,48,24); 
		jLabel42.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel42.setForeground(new Color(-10066279)); 
		jLabel42.setBackground(new Color(-3355444)); 
		add(jLabel42); 
		jLabel43 = new com.sun.java.swing.JLabel(); 
		jLabel43.setText("Online"); 
		jLabel43.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel43.setBounds(408,552,48,24); 
		jLabel43.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel43.setForeground(new Color(-10066279)); 
		jLabel43.setBackground(new Color(-3355444)); 
		add(jLabel43); 
		jLabel44 = new com.sun.java.swing.JLabel(); 
		jLabel44.setText("Online"); 
		jLabel44.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel44.setBounds(12,648,48,24); 
		jLabel44.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel44.setForeground(new Color(-10066279)); 
		jLabel44.setBackground(new Color(-3355444)); 
		//		add(jLabel44); 
		jLabel45 = new com.sun.java.swing.JLabel(); 
		jLabel45.setText("Run Status"); 
		jLabel45.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel45.setBounds(468,602,144,24); 
		jLabel45.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel45.setForeground(new Color(-10066279)); 
		jLabel45.setBackground(new Color(-3355444)); 
		add(jLabel45); 
		update_time = new symantec.itools.awt.Label3D(); 
		try { 
			update_time.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			update_time.setText("---------"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			update_time.setTextColor(new java.awt.Color(-16760704)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		update_time.setBounds(480,626,120,36); 
		update_time.setFont(new Font("Serif", Font.BOLD, 15)); 
		add(update_time); 
		rect13 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect13.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect13.setBounds(468,602,144,72); 
		add(rect13); 
		daqtlc_image = new symantec.itools.multimedia.ImageViewer(); 
		daqtlc_image.setBounds(468,636,206,38); 
		add(daqtlc_image); 
		jLabel46 = new com.sun.java.swing.JLabel(); 
		jLabel46.setText("TB livetime"); 
		jLabel46.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel46.setBounds(660,576,108,24); 
		jLabel46.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel46.setForeground(new Color(-10066279)); 
		jLabel46.setBackground(new Color(-3355444)); 
		//		add(jLabel46); 
		jLabel47 = new com.sun.java.swing.JLabel(); 
		jLabel47.setText("CB livetime"); 
		jLabel47.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel47.setBounds(864,576,84,24); 
		jLabel47.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel47.setForeground(new Color(-10066279)); 
		jLabel47.setBackground(new Color(-3355444)); 
		//		add(jLabel47); 
		/*
		jLabel48 = new com.sun.java.swing.JLabel(); 
		jLabel48.setText("live-time"); 
		jLabel48.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel48.setBounds(768,420,72,24); 
		jLabel48.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel48.setForeground(new Color(-10066279)); 
		jLabel48.setBackground(new Color(-3355444)); 
		add(jLabel48);
		*/ 
		rect9 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect9.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect9.setBounds(624,420,361,182); 
		add(rect9); 
		rect11 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect11.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect11.setBounds(564,240,422,169); 
		add(rect11); 
		jLabel49 = new com.sun.java.swing.JLabel(); 
		jLabel49.setText("Level1 Input Rates and Livetime."); 
		jLabel49.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
		jLabel49.setBounds(650,240,196,24); 
		jLabel49.setFont(new Font("Serif", Font.BOLD, 12)); 
		jLabel49.setForeground(new Color(-10066279)); 
		jLabel49.setBackground(new Color(-3355444)); 
		add(jLabel49); 

//============ event_rate_chart ============//	
		rect33 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect33.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect33.setBounds(10,686,413,270); 
		add(rect33); 
		event_rate_chart.setForeground(new Color(0)); 
		event_rate_chart.setBackground(new Color(12632256)); 

		event_rate_chart.setTitle("Accepted Event Rate");
		event_rate_chart.setDataAreaBorderType(event_rate_chart.ETCHED);
		event_rate_chart.getYAxis().setLabel("Event Rate [ Hz ]");
		//		event_rate_chart.getYAxis().setRange(0.0, 100.0);
		event_rate_chart.getXAxis().setLabel("Time (seconds*10)");
		event_rate_chart.setAllowUserInteraction(false);
		event_rate_chart.setBounds(15,693,403,255);
		add(event_rate_chart);
 
//========================================//		

//============ data_rate_chart ============//	
		rect34 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect34.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect34.setBounds(425,686,413,270); 
		add(rect34); 
		

		data_rate_chart.setTitle("Accepted Data Rate");
		data_rate_chart.setDataAreaBorderType(data_rate_chart.ETCHED);
		data_rate_chart.getYAxis().setLabel("Data Rate [ KByte ]");
		//		data_rate_chart.getYAxis().setRange(0.0, 100.0);
		data_rate_chart.getXAxis().setLabel("Time (seconds*10)");
		data_rate_chart.setAllowUserInteraction(false);
		data_rate_chart.setBackground(Color.lightGray);
		data_rate_chart.setForeground(Color.green);
		data_rate_chart.setBounds(430,693,403,255);
		add(data_rate_chart);
 
//========================================//		
//============ live_time_chart ============//	
		rect35 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect35.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect35.setBounds(840,686,413,270); 
		add(rect35); 
		

		live_time_chart.setTitle("DAQ Live Time");
		live_time_chart.setDataAreaBorderType(live_time_chart.ETCHED);
		live_time_chart.getYAxis().setLabel("Live Time [ % ]");
		live_time_chart.getYAxis().setRange(0.0, 100.0);
		live_time_chart.getXAxis().setLabel("Time (seconds*10)");
		live_time_chart.setAllowUserInteraction(false);
		live_time_chart.setBounds(845,693,403,255);
		add(live_time_chart);
 
//========================================//		
//============ Data_rate in each component Bar ============//	
		rect36 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect36.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect36.setBounds(1000,6,253,226); 
		add(rect36); 
		

		data_rate_comp.setTitle("Data Rate in the ROCs");
		data_rate_comp.setDataAreaBorderType(data_rate_comp.ETCHED);
		data_rate_comp.getYAxis().setLabel("Data Rate [ KByte ]");
		//		data_rate_comp.getYAxis().setRange(0.0, 100.0);
		data_rate_comp.getXAxis().setLabel("ROC");
		data_rate_comp.setAllowUserInteraction(false);
		data_rate_comp.setBackground(Color.lightGray);
		data_rate_comp.setForeground(Color.green);
		data_rate_comp.setBounds(1005,11,243,216);
	       	add(data_rate_comp);
 
//========================================//		

//============ EB buffers for each component Bar ============//	
		rect37 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect37.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect37.setBounds(1000,233,253,226); 
		add(rect37); 
		

		eb_buffer_comp.setTitle("EB Buffer Occupancy");
		eb_buffer_comp.setDataAreaBorderType(eb_buffer_comp.ETCHED);
		eb_buffer_comp.getYAxis().setLabel("Number of buffers");
		eb_buffer_comp.getYAxis().setRange(0.0, 9.0);
		eb_buffer_comp.getXAxis().setLabel("ROC");
		eb_buffer_comp.setAllowUserInteraction(false);
		eb_buffer_comp.setBackground(Color.lightGray);
		eb_buffer_comp.setForeground(Color.green);
		eb_buffer_comp.setBounds(1005,238,243,216);
		add(eb_buffer_comp);
 
//========================================//		
//============ L2_pasfail_chart ============//	
		rect38 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect38.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect38.setBounds(1000,459,253,226); 
		add(rect38); 
		

		l2_pasfail_chart.setTitle("L2 Fail/Pass Ratio");
		l2_pasfail_chart.setDataAreaBorderType(l2_pasfail_chart.ETCHED);
		l2_pasfail_chart.getYAxis().setLabel("Fail/Pass");
		l2_pasfail_chart.getYAxis().setRange(0.0, 1.0);
		l2_pasfail_chart.getXAxis().setLabel("Time (seconds*10)");
		l2_pasfail_chart.setAllowUserInteraction(false);
		l2_pasfail_chart.setBackground(Color.lightGray);
		l2_pasfail_chart.setForeground(Color.green);
		l2_pasfail_chart.setBounds(1005,464,243,216);
		add(l2_pasfail_chart);
 
//========================================//		

	
roc_data_rate = new jclass.chart.JCChart(JCChart.BAR); 

roc_data_rate.setBounds(580,260,200,140); 
    // Chart colors
    roc_data_rate.setBackground(Color.lightGray);
    roc_data_rate.setForeground(Color.black);

    // Set up legend.
    roc_data_rate.getLegend().setIsShowing(true);
    roc_data_rate.getLegend().setOrientation(JCLegend.VERTICAL);
    roc_data_rate.getLegend().setAnchor(JCLegend.SOUTHWEST);
    
    
    // 3D bar charts - use drag to rotate; shift-drag to set the depth
    roc_data_rate.getChartArea().setDepth(10);
    roc_data_rate.getChartArea().setElevation(20);
    roc_data_rate.getChartArea().setRotation(20);
 
    roc_data_rate.setTrigger(0, new EventTrigger(0, EventTrigger.ROTATE));
    roc_data_rate.setTrigger(1, new EventTrigger(Event.SHIFT_MASK, EventTrigger.DEPTH));
   
            add(roc_data_rate); 
    // Set up data source
   double dtdr [][] = { {1.0,2.0,3.0,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20},{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} };
	String[] stdr = {"x","y"};

 ChartDataView arrdr = roc_data_rate.getDataView(0);
 arrdr.setDataSource(new ArrayData(dtdr,12,stdr));
  
  ChartDataViewSeries seriesListdr[] = arrdr.getSeries();
 seriesListdr[0].setLabel("Level1 Input Rates .");

JCAxis x = roc_data_rate.getDataView(0).getXAxis();
x.setAnnotationMethod(JCAxis.VALUE_LABELS);
for (int i = 1; i < 13; i++) {
x.addValueLabel(new JCValueLabel(i,new String(Integer.toString(i))));
 }

//============================================================//		

roc_fe_buffer = new jclass.chart.JCChart(JCChart.BAR); 
roc_fe_buffer.setBounds(790,260,170,140); 

    // Chart colors
    roc_fe_buffer.setBackground(Color.lightGray);
    roc_fe_buffer.setForeground(Color.black);

    // Set up legend.
    roc_fe_buffer.getLegend().setIsShowing(true);
    roc_fe_buffer.getLegend().setOrientation(JCLegend.VERTICAL);
    roc_fe_buffer.getLegend().setAnchor(JCLegend.SOUTH);
    
    
    // 3D bar charts - use drag to rotate; shift-drag to set the depth
    roc_fe_buffer.getChartArea().setDepth(10);
    roc_fe_buffer.getChartArea().setElevation(20);
    roc_fe_buffer.getChartArea().setRotation(20);
 
    roc_fe_buffer.setTrigger(0, new EventTrigger(0, EventTrigger.ROTATE));
    roc_fe_buffer.setTrigger(1, new EventTrigger(Event.SHIFT_MASK, EventTrigger.DEPTH));
   
        add(roc_fe_buffer); 
    // Set up data source
   double dtfe [][] = { {1.0,2.0,3.0,4},{0,0,0,0} };
	String[] stfe = {"x","y"};

 ChartDataView arrfe = roc_fe_buffer.getDataView(0);
 arrfe.setDataSource(new ArrayData(dtfe,3,stfe));
  
  ChartDataViewSeries seriesListfe[] = arrfe.getSeries();
  seriesListfe[0].setLabel("Live Time .");

JCAxis xfe = roc_fe_buffer.getDataView(0).getXAxis();
xfe.setAnnotationMethod(JCAxis.VALUE_LABELS);
JCAxis yfe = roc_fe_buffer.getDataView(0).getYAxis();
yfe.setMax(1.0);
yfe.setMaxIsDefault(false);
yfe.setNumSpacing(0.2);
yfe.setNumSpacingIsDefault(false);

for (int i = 1; i < 4; i++) {
  //xfe.addValueLabel(new JCValueLabel(i,new String(Integer.toString(i))));
xfe.addValueLabel(new JCValueLabel(1,"T"));
xfe.addValueLabel(new JCValueLabel(2,"C"));
xfe.addValueLabel(new JCValueLabel(3,"F"));
 }

//============================================================//		
//============================================================//		

level2_sectors = new jclass.chart.JCChart(JCChart.BAR); 
level2_sectors.setBounds(650,425,155,155);  

    // Chart colors
    level2_sectors.setBackground(Color.lightGray);
    level2_sectors.setForeground(Color.black);

    // Set up legend.
    level2_sectors.getLegend().setIsShowing(true);
    level2_sectors.getLegend().setOrientation(JCLegend.VERTICAL);
    level2_sectors.getLegend().setAnchor(JCLegend.SOUTH);
    
    
    // 3D bar charts - use drag to rotate; shift-drag to set the depth
    level2_sectors.getChartArea().setDepth(10);
    level2_sectors.getChartArea().setElevation(20);
    level2_sectors.getChartArea().setRotation(20);
 
    level2_sectors.setTrigger(0, new EventTrigger(0, EventTrigger.ROTATE));
    level2_sectors.setTrigger(1, new EventTrigger(Event.SHIFT_MASK, EventTrigger.DEPTH));
   
        add(level2_sectors); 
    // Set up data source
   double l2sec [][] = { {1.0,2.0,3.0,4,5,6},{0,0,0,0,0,0} };
	String[] lev2sec = {"x","y"};

 ChartDataView l2arr = level2_sectors.getDataView(0);
 l2arr.setDataSource(new ArrayData(l2sec,6,lev2sec));
  
  ChartDataViewSeries seriesl2sec[] = l2arr.getSeries();
  seriesl2sec[0].setLabel("Sector Based Level2.");

JCAxis lesecx = level2_sectors.getDataView(0).getXAxis();
lesecx.setAnnotationMethod(JCAxis.VALUE_LABELS);
for (int i = 1; i < 7; i++) {
  //lesecx.addValueLabel(new JCValueLabel(i,new String(Integer.toString(i))));
lesecx.addValueLabel(new JCValueLabel(1,"S1"));
lesecx.addValueLabel(new JCValueLabel(2,"S2"));
lesecx.addValueLabel(new JCValueLabel(3,"S3"));
lesecx.addValueLabel(new JCValueLabel(4,"S4"));
lesecx.addValueLabel(new JCValueLabel(5,"S5"));
lesecx.addValueLabel(new JCValueLabel(6,"S6"));
 }

//============================================================//		

//============================================================//		

level2_pass_fail = new jclass.chart.JCChart(JCChart.BAR); 
level2_pass_fail.setBounds(800,425,155,155);   

    // Chart colors
    level2_pass_fail.setBackground(Color.lightGray);
    level2_pass_fail.setForeground(Color.black);

    // Set up legend.
    level2_pass_fail.getLegend().setIsShowing(true);
    level2_pass_fail.getLegend().setOrientation(JCLegend.VERTICAL);
    level2_pass_fail.getLegend().setAnchor(JCLegend.SOUTH);
    
    
    // 3D bar charts - use drag to rotate; shift-drag to set the depth
    level2_pass_fail.getChartArea().setDepth(10);
    level2_pass_fail.getChartArea().setElevation(20);
    level2_pass_fail.getChartArea().setRotation(20);
 
    level2_pass_fail.setTrigger(0, new EventTrigger(0, EventTrigger.ROTATE));
    level2_pass_fail.setTrigger(1, new EventTrigger(Event.SHIFT_MASK, EventTrigger.DEPTH));
   
        add(level2_pass_fail); 
    // Set up data source
   double dtl2pas [][] = { {1.0,2.0,3.0,4,5,6},{0,0,0,0,0,0} };
	String[] lev2pas = {"x","y"};

 ChartDataView l2pasarr = level2_pass_fail.getDataView(0);
l2pasarr.setDataSource(new ArrayData(dtl2pas,3,lev2pas));
  
  ChartDataViewSeries seriesl2pass[] =l2pasarr.getSeries();
  seriesl2pass[0].setLabel("Level2 Pass/Fail/Clear");

JCAxis l2passx = level2_pass_fail.getDataView(0).getXAxis();
l2passx.setAnnotationMethod(JCAxis.VALUE_LABELS);
for (int i = 1; i < 4; i++) {
  //l2passx.addValueLabel(new JCValueLabel(i,new String(Integer.toString(i))));
l2passx.addValueLabel(new JCValueLabel(1,"P"));
l2passx.addValueLabel(new JCValueLabel(2,"F"));
l2passx.addValueLabel(new JCValueLabel(3,"C"));
 }

//============================================================//		

 
		rect5 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect5.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect5.setBounds(12,0,540,156); 
		roc_data_rate.add(rect5); 
		rect14 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect14.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect14.setBounds(12,0,540,156); 
		add(rect14); 
		info_text = new JTextArea(); 
		info_text.setText(""); 
		//	info_text.setInsets(new Insets(0,0,0,0)); 
		info_text.setBounds(576,108,396,108); 
		add(info_text); 
		rect15 = new symantec.itools.awt.shape.Rect(); 
		try { 
			rect15.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		rect15.setBounds(564,96,423,135); 
		add(rect15); 
		l1input1 = new symantec.itools.awt.Label3D(); 
		try { 
			l1input1.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			l1input1.setText("1234"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			l1input1.setTextColor(new java.awt.Color(16512)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		l1input1.setBounds(36,12,36,24); 
		l1input1.setFont(new Font("Serif", Font.BOLD, 12)); 
		l1input1.setForeground(new Color(0)); 
		add(l1input1); 
		l1input2 = new symantec.itools.awt.Label3D(); 
		try { 
			l1input2.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			l1input2.setText("1234"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			l1input2.setTextColor(new java.awt.Color(16512)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		l1input2.setBounds(72,12,36,24); 
		l1input2.setFont(new Font("Serif", Font.BOLD, 12)); 
		l1input2.setForeground(new Color(0)); 
		add(l1input2); 
		l1input3 = new symantec.itools.awt.Label3D(); 
		try { 
			l1input3.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			l1input3.setText("1234"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			l1input3.setTextColor(new java.awt.Color(16512)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		l1input3.setBounds(108,12,36,24); 
		l1input3.setFont(new Font("Serif", Font.BOLD, 12)); 
		l1input3.setForeground(new Color(0)); 
		add(l1input3); 
		l1input4 = new symantec.itools.awt.Label3D(); 
		try { 
			l1input4.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			l1input4.setText("1234"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			l1input4.setTextColor(new java.awt.Color(16512)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		l1input4.setBounds(144,12,36,24); 
		l1input4.setFont(new Font("Serif", Font.BOLD, 12)); 
		l1input4.setForeground(new Color(0)); 
		add(l1input4); 
		l1input5 = new symantec.itools.awt.Label3D(); 
		try { 
			l1input5.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			l1input5.setText("1234"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			l1input5.setTextColor(new java.awt.Color(16512)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		l1input5.setBounds(180,12,36,24); 
		l1input5.setFont(new Font("Serif", Font.BOLD, 12)); 
		l1input5.setForeground(new Color(0)); 
		add(l1input5); 
		l1input6 = new symantec.itools.awt.Label3D(); 
		try { 
			l1input6.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			l1input6.setText("1234"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			l1input6.setTextColor(new java.awt.Color(16512)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		l1input6.setBounds(216,12,36,24); 
		l1input6.setFont(new Font("Serif", Font.BOLD, 12)); 
		l1input6.setForeground(new Color(0)); 
		add(l1input6); 
		l1input7 = new symantec.itools.awt.Label3D(); 
		try { 
			l1input7.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			l1input7.setText("1234"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			l1input7.setTextColor(new java.awt.Color(16512)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		l1input7.setBounds(252,12,36,24); 
		l1input7.setFont(new Font("Serif", Font.BOLD, 12)); 
		l1input7.setForeground(new Color(0)); 
		add(l1input7); 
		l1input8 = new symantec.itools.awt.Label3D(); 
		try { 
			l1input8.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			l1input8.setText("1234"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			l1input8.setTextColor(new java.awt.Color(16512)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		l1input8.setBounds(288,12,36,24); 
		l1input8.setFont(new Font("Serif", Font.BOLD, 12)); 
		l1input8.setForeground(new Color(0)); 
		add(l1input8); 
		l1input10 = new symantec.itools.awt.Label3D(); 
		try { 
			l1input10.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			l1input10.setText("1234"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			l1input10.setTextColor(new java.awt.Color(16512)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		l1input10.setBounds(348,12,36,24); 
		l1input10.setFont(new Font("Serif", Font.BOLD, 12)); 
		l1input10.setForeground(new Color(0)); 
		add(l1input10); 
		l1input12 = new symantec.itools.awt.Label3D(); 
		try { 
			l1input12.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			l1input12.setText("1234"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			l1input12.setTextColor(new java.awt.Color(16512)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		l1input12.setBounds(396,12,36,24); 
		l1input12.setFont(new Font("Serif", Font.BOLD, 12)); 
		l1input12.setForeground(new Color(0)); 
		add(l1input12); 
		eb_evt_rate = new symantec.itools.awt.Label3D(); 
		try { 
			eb_evt_rate.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			eb_evt_rate.setText("1234"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			eb_evt_rate.setTextColor(new java.awt.Color(16512)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		eb_evt_rate.setBounds(108,444,60,24); 
		eb_evt_rate.setFont(new Font("Serif", Font.BOLD, 12)); 
		eb_evt_rate.setForeground(new Color(0)); 
		add(eb_evt_rate); 
		eb_data_rate = new symantec.itools.awt.Label3D(); 
		try { 
			eb_data_rate.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			eb_data_rate.setText("1234"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			eb_data_rate.setTextColor(new java.awt.Color(16512)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		eb_data_rate.setBounds(108,468,60,24); 
		eb_data_rate.setFont(new Font("Serif", Font.BOLD, 12)); 
		eb_data_rate.setForeground(new Color(0)); 
		add(eb_data_rate); 
		ipc2dd_rate = new symantec.itools.awt.Label3D(); 
		try { 
			ipc2dd_rate.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			ipc2dd_rate.setText("1234"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			ipc2dd_rate.setTextColor(new java.awt.Color(16512)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		ipc2dd_rate.setBounds(132,576,60,24); 
		ipc2dd_rate.setFont(new Font("Serif", Font.BOLD, 12)); 
		ipc2dd_rate.setForeground(new Color(0)); 
		add(ipc2dd_rate); 
		recsis_rate = new symantec.itools.awt.Label3D(); 
		try { 
			recsis_rate.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			recsis_rate.setText("1234"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			recsis_rate.setTextColor(new java.awt.Color(16512)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		recsis_rate.setBounds(276,576,60,24); 
		recsis_rate.setFont(new Font("Serif", Font.BOLD, 12)); 
		recsis_rate.setForeground(new Color(0)); 
		add(recsis_rate); 

		et00_rate = new symantec.itools.awt.Label3D(); 
		try { 
			et00_rate.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			et00_rate.setText("1234"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			et00_rate.setTextColor(new java.awt.Color(16512)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		et00_rate.setBounds(330,500,60,24); 
		et00_rate.setFont(new Font("Serif", Font.BOLD, 12)); 
		et00_rate.setForeground(new Color(0)); 
		add(et00_rate); 

		et00_drate = new symantec.itools.awt.Label3D(); 
		try { 
			et00_drate.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			et00_drate.setText("1234"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			et00_drate.setTextColor(new java.awt.Color(16512)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		et00_drate.setBounds(330,509,60,24); 
		et00_drate.setFont(new Font("Serif", Font.BOLD, 12)); 
		et00_drate.setForeground(new Color(0)); 
		//		add(et00_drate); 

		evmon_rate = new symantec.itools.awt.Label3D(); 
		try { 
			evmon_rate.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			evmon_rate.setText("1234"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			evmon_rate.setTextColor(new java.awt.Color(16512)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		evmon_rate.setBounds(300,600,60,24); 
		evmon_rate.setFont(new Font("Serif", Font.BOLD, 12)); 
		evmon_rate.setForeground(new Color(0)); 
		add(evmon_rate); 
		er_evt_rate = new symantec.itools.awt.Label3D(); 
		try { 
			er_evt_rate.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			er_evt_rate.setText("12345"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			er_evt_rate.setTextColor(new java.awt.Color(16512)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		er_evt_rate.setBounds(396,432,72,36); 
		er_evt_rate.setFont(new Font("Serif", Font.BOLD, 16)); 
		er_evt_rate.setForeground(new Color(0)); 
		add(er_evt_rate); 
		er_data_rate = new symantec.itools.awt.Label3D(); 
		try { 
			er_data_rate.setBevelStyle(symantec.itools.awt.Label3D.BEVEL_LOWERED); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			er_data_rate.setText("12345"); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		try { 
			er_data_rate.setTextColor(new java.awt.Color(16512)); 
		} 
		catch(java.beans.PropertyVetoException e) { } 
		er_data_rate.setBounds(396,468,72,36); 
		er_data_rate.setFont(new Font("Serif", Font.BOLD, 16)); 
		er_data_rate.setForeground(new Color(0)); 
		add(er_data_rate); 
		setTitle("DAQTLC"); 
		//}} 
		 
		//{{INIT_MENUS 
		mainMenuBar = new java.awt.MenuBar(); 
		menu1 = new java.awt.Menu("File"); 
		miExit = new java.awt.MenuItem("Exit"); 
		menu1.add(miExit); 
		mainMenuBar.add(menu1); 
		menuStartup = new java.awt.Menu("Startup"); 
		StartEtDaq = new java.awt.MenuItem("Restart DAQ"); 
		menuStartup.add(StartEtDaq); 
		//		StartER = new java.awt.MenuItem("Start Event Recorder"); 
		//		menuStartup.add(StartER); 
		//		StartRunControl = new java.awt.MenuItem("Start Run Control"); 
		//		menuStartup.add(StartRunControl); 
		SetLevel1Trigger = new java.awt.MenuItem("Start Tigris"); 
		menuStartup.add(SetLevel1Trigger); 
		SetTriggerThresholds = new java.awt.MenuItem("Start Diman"); 
		menuStartup.add(SetTriggerThresholds); 
		mainMenuBar.add(menuStartup); 
		menuControl = new java.awt.Menu("Control"); 
		PauseDaq = new java.awt.MenuItem("Pause DAQ"); 
		menuControl.add(PauseDaq); 
		ResumeDaq = new java.awt.MenuItem("Resume DAQ"); 
		menuControl.add(ResumeDaq); 
		EnableRawBanks = new java.awt.MenuItem("Enable Raw Banks"); 
		menuControl.add(EnableRawBanks); 
		DisableRawBanks = new java.awt.MenuItem("Disable Raw Banks"); 
		menuControl.add(EnableRawBanks); 
		SetPrescaleFactors = new java.awt.MenuItem("Set Prescale Factors"); 
		menuControl.add(SetPrescaleFactors); 
		SetTriggerBits = new java.awt.MenuItem("Set Trigger Bits"); 
		menuControl.add(SetTriggerBits); 
		DisableForceSync = new java.awt.MenuItem("Disable Force Sync"); 
		menuControl.add(DisableForceSync); 
		EnableForceSync = new java.awt.MenuItem("Enable Force Sync"); 
		menuControl.add(EnableForceSync); 
		StartPulser = new java.awt.MenuItem("Start Pulser"); 
		menuControl.add(StartPulser); 
		StopPulser = new java.awt.MenuItem("Stop Pulser"); 
		menuControl.add(StopPulser); 

		mainMenuBar.add(menuControl); 
		menCheckout = new java.awt.Menu("Checkout"); 
		TrigTest = new java.awt.MenuItem("Trigger test"); 
		menCheckout.add(TrigTest); 
		mainMenuBar.add(menCheckout); 
		menuRepair = new java.awt.Menu("Repair"); 
		ExitDaq = new java.awt.MenuItem("Exit DAQ"); 
		menuRepair.add(ExitDaq); 
		ExitRocs = new java.awt.MenuItem("Exit ROC's"); 
		menuRepair.add(ExitRocs); 
		RebootRocs = new java.awt.MenuItem("Reboot ROC's"); 
		menuRepair.add(RebootRocs); 
		Recover = new java.awt.MenuItem("Recover"); 
		menuRepair.add(Recover); 
		mainMenuBar.add(menuRepair); 
		menuOnline = new java.awt.Menu("Online"); 
		StopAutoControl = new java.awt.MenuItem("Disable Online Auto-Recovery"); 
		menuOnline.add(StopAutoControl); 
		StartAutoControl = new java.awt.MenuItem("Enable Online Auto-Recovery"); 
		menuOnline.add(StartAutoControl); 

		StopOnline = new java.awt.MenuItem("Recover Online on clon04"); 
		menuOnline.add(StopOnline); 
		StartOnline = new java.awt.MenuItem("Recover Online on clon00"); 
		menuOnline.add(StartOnline); 

		mainMenuBar.add(menuOnline); 
		menu3 = new java.awt.Menu("Help"); 
		mainMenuBar.setHelpMenu(menu3); 
		miAbout = new java.awt.MenuItem("About.."); 
		menu3.add(miAbout); 
		mainMenuBar.add(menu3); 
		setMenuBar(mainMenuBar); 
		//$$ mainMenuBar.move(948,648); 
		//}} 
		 
		//{{REGISTER_LISTENERS 
		SymWindow aSymWindow = new SymWindow(); 
		this.addWindowListener(aSymWindow); 
		SymAction lSymAction = new SymAction();
		//Startup menu actions
		StartEtDaq.addActionListener(lSymAction);
		SetLevel1Trigger.addActionListener(lSymAction);
		SetTriggerThresholds.addActionListener(lSymAction);
		//Control menu actions
		PauseDaq.addActionListener(lSymAction);
		ResumeDaq.addActionListener(lSymAction);
		EnableRawBanks.addActionListener(lSymAction);
		DisableRawBanks.addActionListener(lSymAction);
		SetPrescaleFactors.addActionListener(lSymAction);
		SetTriggerBits.addActionListener(lSymAction);
		DisableForceSync.addActionListener(lSymAction);
		EnableForceSync.addActionListener(lSymAction);
		StartPulser.addActionListener(lSymAction);
		StopPulser.addActionListener(lSymAction);
		//checkout menu
		TrigTest.addActionListener(lSymAction);
		//Repair menu actions
		ExitDaq.addActionListener(lSymAction);
		ExitRocs.addActionListener(lSymAction);
		RebootRocs.addActionListener(lSymAction);
		Recover.addActionListener(lSymAction);

		miAbout.addActionListener(lSymAction); 
		miExit.addActionListener(lSymAction); 

		//Online menu
		StartAutoControl.addActionListener(lSymAction); 
		StopAutoControl.addActionListener(lSymAction); 
		StopOnline.addActionListener(lSymAction);
		StartOnline.addActionListener(lSymAction);



		//DAQ recover button action
		daq_recover.addActionListener(lSymAction);
		online_recover.addActionListener(lSymAction);

		//TS.CSR button actions
     for (int i = 0; i<=31; i++) {
       csr[i].addActionListener(lSymAction);
       st[i].addActionListener(lSymAction); 
     }

     //ROC button actions
     for (int i = 0; i<=38; i++) {
     roc_button[i].addActionListener(lSymAction);
     }

     //EB button action
     EB.addActionListener(lSymAction);
     //ER button action
     ER.addActionListener(lSymAction);
     //IPC on clon10 action
     Ipc2it.addActionListener(lSymAction);
     //point to point IPC
     It2ipc.addActionListener(lSymAction);
     //Stadis action
     Stadis.addActionListener(lSymAction);
     //Trg_monitor action
     Trg_monitor.addActionListener(lSymAction);
     //Scaler action
     Scaler.addActionListener(lSymAction);
     //EventMonitor action
     Evt_monitor.addActionListener(lSymAction);
     //ET on clon10
     ET.addActionListener(lSymAction);
     //ET on clon00
     Ipc2dd.addActionListener(lSymAction);
     //Recsis action
     Recsis.addActionListener(lSymAction);
     //raid partitions
     raid1.addActionListener(lSymAction);
     raid2.addActionListener(lSymAction);
     raid3.addActionListener(lSymAction);
     raid4.addActionListener(lSymAction);
     raid5.addActionListener(lSymAction);
     raid6.addActionListener(lSymAction);
     raid7.addActionListener(lSymAction);
     raid8.addActionListener(lSymAction);
		//}} 

		setVisible(true); 

	} 
	 
	public DaqControlGui(String title) 
	{ 
		this(); 
		setTitle(title); 
	} 
	 

    /** 
     * Shows or hides the component depending on the boolean flag b. 
     * @param b  if true, show the component; otherwise, hide the component. 
     * @see java.awt.Component#isVisible 
     */ 
    public void setVisible(boolean b) 
	{ 
		if(b) 
		{ 
			setLocation(0, 0); 
		}	 
		super.setVisible(b); 
	} 
	 
	static public void main(String args[]) 
	{ 
		(new DaqControlGui()).setVisible(true); 
	} 
	 
	public void addNotify() 
	{ 
		// Record the size of the window prior to calling parents addNotify. 
		Dimension d = getSize(); 
		 
		super.addNotify(); 
	 
		if (fComponentsAdjusted) 
			return; 
	 
		// Adjust components according to the insets 
		setSize(insets().left + insets().right + d.width, insets().top + insets().bottom + d.height); 
		Component components[] = getComponents(); 
		for (int i = 0; i < components.length; i++) 
		{ 
			Point p = components[i].getLocation(); 
			p.translate(insets().left, insets().top); 
			components[i].setLocation(p); 
		} 
		fComponentsAdjusted = true; 
	} 
	 
	// Used for addNotify check. 
	boolean fComponentsAdjusted = false; 
	 
	//{{DECLARE_CONTROLS 
	java.awt.FileDialog openFileDialog1;
	com.sun.java.swing.JLabel jLabel1; 
	com.sun.java.swing.JLabel jLabel2; 
	com.sun.java.swing.JLabel jLabel4; 
	com.sun.java.swing.JLabel jLabel5; 
	com.sun.java.swing.JLabel jLabel6; 
	com.sun.java.swing.JLabel jLabel7; 
	com.sun.java.swing.JButton Emptyb1; 
	com.sun.java.swing.JButton Emptyb2; 
	com.sun.java.swing.JButton Emptyb3; 
	com.sun.java.swing.JButton Emptyb4; 
	com.sun.java.swing.JButton Emptyb5; 
	com.sun.java.swing.JButton Emptyb6; 
	com.sun.java.swing.JButton Emptyb7; 
	com.sun.java.swing.JButton Emptyb8; 
	com.sun.java.swing.JButton Emptyb9; 
	com.sun.java.swing.JButton Emptyb10; 
	com.sun.java.swing.JLabel jLabel8; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine1; 
	symantec.itools.awt.shape.VerticalLine verticalLine1; 
	symantec.itools.awt.shape.VerticalLine verticalLine2; 
	symantec.itools.awt.shape.VerticalLine verticalLine3; 
	symantec.itools.awt.shape.VerticalLine verticalLine4; 
	symantec.itools.awt.shape.VerticalLine verticalLine5; 
	symantec.itools.awt.shape.VerticalLine verticalLine6; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine2; 
	symantec.itools.awt.shape.VerticalLine verticalLine7; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine16; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine17; 
	symantec.itools.awt.shape.VerticalLine verticalLine8;
	symantec.itools.awt.shape.HorizontalLine horizontalLine99; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine23; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine24; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine25; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine26; 
	symantec.itools.awt.shape.VerticalLine verticalLine9; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine27; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine28; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine29; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine30; 
	symantec.itools.awt.shape.VerticalLine verticalLine10; 
	com.sun.java.swing.JLabel jLabel10; 
	com.sun.java.swing.JLabel jLabel11; 
	com.sun.java.swing.JLabel jLabel12; 
	com.sun.java.swing.JLabel jLabel13; 
	symantec.itools.awt.shape.VerticalLine verticalLine11; 
	com.sun.java.swing.JLabel jLabel9; 
	com.sun.java.swing.JLabel jLabel14; 
	com.sun.java.swing.JLabel jLabel15; 
	com.sun.java.swing.JLabel jLabel16; 
	com.sun.java.swing.JLabel jLabel3; 
	com.sun.java.swing.JLabel jLabel17; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine3; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine4; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine5; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine6; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine88; 
	symantec.itools.awt.shape.VerticalLine verticalLine12; 
	com.sun.java.swing.JLabel jLabel18; 
	com.sun.java.swing.JLabel jLabel19; 
	com.sun.java.swing.JLabel jLabel20; 
	com.sun.java.swing.JLabel jLabel21; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine7; 
	symantec.itools.awt.shape.VerticalLine verticalLine13; 
	symantec.itools.awt.shape.VerticalLine verticalLine113; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine8; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine9; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine10; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine11; 
	com.sun.java.swing.JLabel jLabel22; 
	com.sun.java.swing.JLabel jLabel23; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine12; 
	symantec.itools.awt.shape.VerticalLine verticalLine14;
 	symantec.itools.awt.shape.VerticalLine verticalLineTrg; 
	symantec.itools.awt.shape.VerticalLine verticalLine15; 
	symantec.itools.awt.shape.VerticalLine verticalLine16; 
	symantec.itools.awt.shape.VerticalLine verticalLine17; 
	symantec.itools.awt.shape.VerticalLine verticalLine18; 
	symantec.itools.awt.shape.Rect rect1; 
	symantec.itools.awt.shape.Rect rect2; 
	symantec.itools.awt.shape.Rect rect3; 
	symantec.itools.awt.shape.Rect rect4; 
	symantec.itools.awt.shape.Rect rect6;
	symantec.itools.awt.shape.Rect rect33;
	symantec.itools.awt.shape.Rect rect34;
	symantec.itools.awt.shape.Rect rect35;
	symantec.itools.awt.shape.Rect rect36;
	symantec.itools.awt.shape.Rect rect37;
	symantec.itools.awt.shape.Rect rect38;
	com.sun.java.swing.JLabel jLabel24; 
	com.sun.java.swing.JLabel jLabel25; 
	com.sun.java.swing.JLabel jLabel26; 
	com.sun.java.swing.JLabel jLabel27; 
	com.sun.java.swing.JLabel jLabel28; 
	com.sun.java.swing.JLabel jLabel29; 
	com.sun.java.swing.JLabel jLabel30; 
	com.sun.java.swing.JLabel jLabel31; 
	com.sun.java.swing.JLabel jLabel32; 
	com.sun.java.swing.JLabel jLabel33; 
	symantec.itools.awt.shape.HorizontalLine horizontalLine13; 
	com.sun.java.swing.JLabel jLabel34; 
	symantec.itools.awt.shape.Rect rect7; 
	com.sun.java.swing.JLabel jLabel35; 
	com.sun.java.swing.JLabel jLabel36; 
	symantec.itools.awt.shape.Rect rect8; 
	com.sun.java.swing.JLabel jLabel37; 
	com.sun.java.swing.JLabel jLabel38; 
	com.sun.java.swing.JLabel jLabel39; 
	com.sun.java.swing.JLabel jLabel40; 
	symantec.itools.awt.shape.Rect rect10; 
	com.sun.java.swing.JLabel jLabel41; 
	symantec.itools.awt.shape.Rect rect12; 
	com.sun.java.swing.JLabel jLabel42; 
	com.sun.java.swing.JLabel jLabel43; 
	com.sun.java.swing.JLabel jLabel44; 
	com.sun.java.swing.JLabel jLabel45; 
	symantec.itools.awt.shape.Rect rect13; 
	symantec.itools.multimedia.ImageViewer daqtlc_image; 
	com.sun.java.swing.JLabel jLabel46; 
	com.sun.java.swing.JLabel jLabel47; 
	com.sun.java.swing.JLabel jLabel48; 
	symantec.itools.awt.shape.Rect rect9; 
	symantec.itools.awt.shape.Rect rect11; 
	com.sun.java.swing.JLabel jLabel49; 
	symantec.itools.awt.shape.Rect rect5; 
	symantec.itools.awt.shape.Rect rect14; 
	symantec.itools.awt.shape.Rect rect15; 
	symantec.itools.awt.shape.Rect rect16; 

 	//}} 
	 
	//{{DECLARE_MENUS 
	java.awt.MenuBar mainMenuBar; 
	java.awt.Menu menu1; 
	java.awt.MenuItem miExit; 
	java.awt.Menu menuStartup; 
	java.awt.MenuItem StartEtDaq; 
	java.awt.MenuItem SetLevel1Trigger; 
	java.awt.MenuItem SetTriggerThresholds; 
	java.awt.Menu menuControl; 
	java.awt.MenuItem PauseDaq;
 	java.awt.MenuItem ResumeDaq;
	java.awt.MenuItem EnableRawBanks;
	java.awt.MenuItem DisableRawBanks; 
	java.awt.MenuItem SetPrescaleFactors; 
	java.awt.MenuItem SetTriggerBits; 
	java.awt.MenuItem DisableForceSync; 
	java.awt.MenuItem EnableForceSync;
	java.awt.MenuItem StartPulser;
	java.awt.MenuItem StopPulser;
	java.awt.Menu menCheckout; 
	java.awt.MenuItem TrigTest; 
	java.awt.Menu menuRepair; 
	java.awt.MenuItem ExitDaq; 
	java.awt.MenuItem ExitRocs; 
	java.awt.MenuItem RebootRocs; 
	java.awt.MenuItem Recover; 
	java.awt.Menu menuOnline; 
	java.awt.MenuItem StartAutoControl; 
	java.awt.MenuItem StopAutoControl; 
	java.awt.MenuItem StopOnline; 
	java.awt.MenuItem StartOnline; 
	java.awt.Menu menu3; 
	java.awt.MenuItem miAbout; 
	//}} 
	 
	class SymWindow extends java.awt.event.WindowAdapter 
	{ 
		public void windowClosing(java.awt.event.WindowEvent event) 
		{ 
			Object object = event.getSource(); 
			if (object == DaqControlGui.this) 
				Frame1_WindowClosing(event); 
		} 
	} 
	 
	void Frame1_WindowClosing(java.awt.event.WindowEvent event) 
	{ 
		setVisible(false);	// hide the Frame 
		dispose();			// free the system resources 
		System.exit(0);		// close the application 
	} 
	 


	class SymAction implements java.awt.event.ActionListener {

		public void actionPerformed(java.awt.event.ActionEvent event) 
		{ 
			Object object = event.getSource(); 
			if (object == miAbout) 
				miAbout_Action(event); 
			else if (object == miExit) 
				miExit_Action(event); 
			else if (object == StartEtDaq) 
				StartEtDaq_Action(); 
			else if (object == SetLevel1Trigger) 
				SetLevel1Trigger_Action();
			else if (object == SetTriggerThresholds) 
				SetTriggerThresholds_Action(); 
			else if (object == PauseDaq) 
				PauseDaq_Action(); 
			else if (object == ResumeDaq) 
				ResumeDaq_Action(); 
			else if (object == EnableRawBanks) 
				EnableRawBanks_Action(); 
			else if (object == DisableRawBanks) 
				DisableRawBanks_Action(); 
 			else if (object == SetPrescaleFactors) 
				SetPrescaleFactors_Action(); 
			else if (object == SetTriggerBits) 
				SetTriggerBits_Action(); 
			else if (object == DisableForceSync) 
				DisableForceSync_Action(); 
			else if (object == EnableForceSync) 
 				EnableForceSync_Action();  

			else if (object == StartPulser) 
 				StartPulser_Action();  
			else if (object == StopPulser) 
 				StopPulser_Action();  


			else if (object == TrigTest) 
 				Trigger_test_Action();  
	 
			else if (object == ExitDaq)
				ExitDaq_Action();
			else if (object == ExitRocs)
				ExitRocs_Action();
			else if (object == RebootRocs)
				RebootRocs_Action();
			else if (object == Recover)
				Recover_Action();

			else if (object == StopAutoControl)
				StopAutoControl_Action();
			else if (object == StartAutoControl)
				StartAutoControl_Action();
			else if (object == StopOnline)
				StopOnline_Action();
			else if (object == StartOnline)
				StartOnline_Action();



			else if ( object == raid1)
			  raid1_action();

			else if ( object == raid2)
			  raid2_action();

			else if ( object == raid3)
			  raid3_action();

			else if ( object == raid4)
			  raid4_action();

			else if ( object == raid5)
			  raid5_action();

			else if ( object == raid6)
			  raid6_action();

			else if ( object == raid6)
			  raid7_action();

			else if ( object == raid6)
			  raid8_action();
			
                        else if (object == daq_recover)
			  daq_recover_Action();
                        else if (object == online_recover)
			  online_recover_Action();
			
			else if (object == EB)
			  EB_action();
			else if (object == ER)
			  ER_action();
		        else if ( object == Ipc2it)
			  ipcbanktoet_action();
			else if ( object == It2ipc)
			  point2point_action();
			else if ( object == Stadis)
			  evtStatistic_action();
			else if ( object == Trg_monitor)
			  Trg_monitor_action();
			else if ( object == Scaler)
			  scalerserv_action();
			else if ( object == Evt_monitor)
			  evtmonitor_action();
			else if ( object == ET)
			  eton10_action();
			else if ( object == Ipc2dd)
			  eton00_action();
			else if ( object == Recsis)
			  recsis_action();
			



			for (int i = 0; i<=31; i++) {
			  if (object == csr[i]) csr_action(i);
			  else if (object == st[i]) st_action(i);

			}

			for (int i = 0; i<=38; i++) {
			  if ( object == roc_button[i]) roc_action(i);
			}

			
		} 
	} 
  
  //Action methods for the startup menu
	 

  //action methods for raid
  void raid1_action() {
info_text.setText("/mnt/raid0  61GByte local raid partition.\n Light Blue - Empty, Dark Blue -  Full and Backed Up \n Magenta - Full Not Backed Up, Green - Active \n Yellow - Moving to SILO ");
  } 
  void raid2_action() {
info_text.setText("/mnt/raid1  61GByte local raid partition.\n Light Blue - Empty, Dark Blue -  Full and Backed Up \n Magenta - Full Not Backed Up, Green - Active \n Yellow - Moving to SILO ");

  } 
  void raid3_action() {
info_text.setText("/mnt/raid2  61GByte local raid partition.\n Light Blue - Empty, Dark Blue -  Full and Backed Up \n Magenta - Full Not Backed Up, Green - Active \n Yellow - Moving to SILO ");

  } 
  void raid4_action() {
info_text.setText("/mnt/raid3  61GByte local raid partition.\n Light Blue - Empty, Dark Blue -  Full and Backed Up \n Magenta - Full Not Backed Up, Green - Active \n Yellow - Moving to SILO ");

  } 
  void raid5_action() {
info_text.setText("/mnt/raid4  61GByte local raid partition.\n Light Blue - Empty, Dark Blue -  Full and Backed Up \n Magenta - Full Not Backed Up, Green - Active \n Yellow - Moving to SILO ");

  } 
  void raid6_action() {
info_text.setText("/mnt/raid5  61GByte local raid partition.\n Light Blue - Empty, Dark Blue -  Full and Backed Up \n Magenta - Full Not Backed Up, Green - Active \n Yellow - Moving to SILO ");

  } 

  void raid7_action() {
info_text.setText("/mnt/raid6  90GByte local raid partition.\n Light Blue - Empty, Dark Blue -  Full and Backed Up \n Magenta - Full Not Backed Up, Green - Active \n Yellow - Moving to SILO ");

  } 
  void raid8_action() {
info_text.setText("/mnt/raid7  90GByte local raid partition.\n Light Blue - Empty, Dark Blue -  Full and Backed Up \n Magenta - Full Not Backed Up, Green - Active \n Yellow - Moving to SILO ");

  } 



  //action methods for online
  void EB_action() {
    info_text.setText("CODA Event Builder");
  }
  void ER_action() {
    info_text.setText("CODA Event Recorder");
  }
  void ipcbanktoet_action() {
    info_text.setText("Special event producer.\n Inserts special events into datastream (epics events, others).");
  }

  void point2point_action() {
    info_text.setText("Station on the main ET,\n IPC client requiring  sample of the events \nto start secondary ET on clon00.");
  }

  void evtStatistic_action() {
info_text.setText("Event Transfer system status monitor, \n Polls every 10 seconds for EVT status information,\n ships to info server, \nwrites to file ($CLON_PARMS/evt/evt_status_monitor.txt)");
  }
  
  void scalerserv_action() {
info_text.setText("Scaler server, \n Archives scaler events and ships them to info_server,other programs. \nWrites to file \n($CLON_PARMS/scalers/archive/scaler_server_<session>_<run>.txt)");
  }

  void evtmonitor_action() {
info_text.setText("Event monitor \nAnalyzes events, creates hist in global section MON, \narchives hist to /hist/monitor");
  }

  void eton10_action() {
info_text.setText("Main ET system");
  }

  void eton00_action() {
info_text.setText("Remote ET systems. \nReceives events from et2et (on clon10) and \ninserts them into local ET system.\n  Creates ET system via clas_et_start \nbefore launching stations");
  }

  void recsis_action() {
info_text.setText(" Online recsis \nAnalyzes events much more extensively than event_monitor");
  }

  void Trg_monitor_action() {
info_text.setText(" Trigmon \nLevel1 and Level2 trigger analyzes based on data stream information.");
  }



  void csr_action(int i) {
    info_text.setText(DataPool.ts_csr_def[i]+ "\nGreen - Bit is active \n Red - Trigger Supervisor is dead. Rebooting is in progress.");
      }
  
  void st_action(int i) {
    info_text.setText(DataPool.ts_state_def[i]+ "\nGreen - Bit is active \n Red - Trigger Supervisor is dead. Rebooting is in progress.");
      }
  
  void roc_action(int i) {
info_text.setText(DataPool.ts_rocbr_def[i] +" crate. Motorola CPU running vxWorks.\n Green - ROC is active \n Yellow - Participating in the datat production \n Red - ROC is dead. Rebooting is in progress.\n" + DataPool.roc_description[i]);
  }



	void StartEtDaq_Action() { 
      DataPool.daq_recover_clicked = 1;
      info_text.setText(" ");
	  mySysExe.run_unix("daq_recover");
      info_text.setText("Command sent");
      	} 



  void SetLevel1Trigger_Action() { 
      info_text.setText(" ");
       mySysExe.run_unix("start_tigris.sh");
      info_text.setText("Command sent");
}

  void SetTriggerThresholds_Action() { 
      info_text.setText(" ");
       mySysExe.run_unix("start_diman.sh");
      info_text.setText("Command sent");
}
	 

  //Action methods for the control menu

  void PauseDaq_Action () {
      info_text.setText(" ");
       mySysExe.run_unix("coda_pause");
      info_text.setText("Command sent");
}

  void ResumeDaq_Action () {
      info_text.setText(" ");
       mySysExe.run_unix("coda_resume");
      info_text.setText("Command sent");
}

  void EnableRawBanks_Action () {
      info_text.setText(" ");
       mySysExe.run_unix("s_raw_true");
      info_text.setText("Command sent");
}


  void StartPulser_Action () {
      info_text.setText(" ");
       mySysExe.run_unix("pulser_100");
      info_text.setText("Command sent");
}

  void StopPulser_Action () {
      info_text.setText(" ");
       mySysExe.run_unix("pulser_stop");
      info_text.setText("Command sent");
}



  void DisableRawBanks_Action () {
      info_text.setText(" ");
       mySysExe.run_unix("s_raw_false");
      info_text.setText("Command sent");
}

  void SetPrescaleFactors_Action () {
      info_text.setText(" ");
       mySysExe.run_unix("s_ts_prescale " + " " +prescale[0].getText()+ " "  + prescale[1].getText()+ " "  + prescale[2].getText()+ " "  + prescale[3].getText()+ " "  + prescale[4].getText()+ " "  + prescale[5].getText()+ " "  + prescale[6].getText()+ " "  + prescale[7].getText());
      info_text.setText("Command sent");
}


  void SetTriggerBits_Action () {
    info_text.setText(" ");
    int bitset = 1;
    for (int i = 0; i<=11; i++) {
      if((chbit[i].isSelected())) {
	int myval = 1;
	for (int j=0;j<=i;j++)myval = myval*2;
	bitset = bitset + myval;
      }
    }
       mySysExe.run_unix("s_ts_bits "+Integer.toString(bitset));
      info_text.setText("Command sent");
  }
  
  void DisableForceSync_Action () {
      info_text.setText(" ");
       mySysExe.run_unix("s_exclude_scaler");
      info_text.setText("Command sent");
}

  void EnableForceSync_Action () {
      info_text.setText(" ");
       mySysExe.run_unix("s_include_scaler");
      info_text.setText("Command sent");
}

  //Action methods for checkout menu
  void Trigger_test_Action () {
info_text.setText("Attention: After this procedure yopu need \n           to end the run and start the new one.");
       mySysExe.run_unix("$CLON_BIN/trigger_test");
  }

  //Action methods for online menu
  void StopAutoControl_Action() {
      info_text.setText("Online auto recovery is disabled.");
      mySysExe.run_unix("control_procmgr_manager stop &; control_process_manager stop &; control_ipc_process_manager stop &");      
//DataPool.auto_recover = 0;
  }
  void StartAutoControl_Action() {
      info_text.setText("Online auto recovery is enabled.");
      mySysExe.run_unix("control_process_manager start &");
  }
  void StopOnline_Action() {       
       mySysExe.run_unix("mon2_recover");
      // Add here the script

  }
  void StartOnline_Action() {
      // Add here the script
       mySysExe.run_unix("mon_recover");
  }
  
  



  //Action methods for the repair menu
  void ExitDaq_Action () {
      info_text.setText(" ");
       mySysExe.run_unix("daq_exit");
      info_text.setText("Command sent" + " Your name will be reported...");
}

  void ExitRocs_Action () {
      info_text.setText(" ");
//       mySysExe.run_unix("close_roc_terms");
	 //      info_text.setText("Command sent");
}

  void RebootRocs_Action () {
      info_text.setText(" ");
      mySysExe.run_unix("reboot_all");
	 //      info_text.setText("Command sent");
}

  void Recover_Action () {
      DataPool.daq_recover_clicked = 1;
      info_text.setText(" ");
       mySysExe.run_unix("daq_recover");
      info_text.setText("Daq_recover process is in progress. Please wait.");
}

  void daq_recover_Action () {
      DataPool.daq_recover_clicked = 1;
      info_text.setText(" ");
      mySysExe.run_unix("daq_recover");
      info_text.setText("Daq_recover process is in progress. Please wait.");

      /*
      // Start progress bar
      pm = new ProgressMonitor(container, "DAQ recovery process is in progress...", "Note: Please wait until Runcontrol GUI is up.", 0, 100);
      pm.setMaximum(DataPool.MaxProgress);
      pm.setMillisToPopup(5000);
      progressCount = 0;
      pm.setProgress(progressCount+=DataPool.ProgressStep);
      //if( (pm!=null) && (pm.isCanceled()==false)) pm.setProgress(progressCount+=DataPool.ProgressStep);
      for(int l=51; l<900;l++){	
	try {
	Thread.sleep(3000);
	} catch (InterruptedException e) {
	  System.out.println("interruped exception: " +e);
	}
	if( (pm!=null) && (pm.isCanceled()==false)) pm.setProgress(l);	
  */
	try {
	Thread.sleep(40000);
	} catch (InterruptedException e) {
	  System.out.println("interruped exception: " +e);
	}
	  
	//      daq_recover.setForeground(new Color(0)); 
	//      daq_recover.setBackground(new Color(-3355444)); 
      daq_recover.setEnabled(false);
      }


  void online_recover_Action () {
      info_text.setText("Online Auto recovery");

      DataPool.auto_recover = 0;      
      mySysExe.run_unix("mon2_recover");
	try {
	Thread.sleep(30000);
	} catch (InterruptedException e) {
	  System.out.println("interruped exception: " +e);
	}
	DataPool.auto_recover = 1;
		online_recover.setForeground(new Color(0)); 
		online_recover.setBackground(new Color(-3355444)); 
	       online_recover.setEnabled(false);

}







	void miAbout_Action(java.awt.event.ActionEvent event) 
	{ 
	    	    System.out.println("KUKU");
		//{{CONNECTION 

		// Action from About Create and show as modal 
		    //(new AboutDialog(this, true)).setVisible(true); 
		//}} 
	} 
	 
	void miExit_Action(java.awt.event.ActionEvent event) 
	{ 
		//{{CONNECTION 
		// Action from Exit Create and show as modal 
	  (new QuitDialog(this, true)).setVisible(true); 
		//}} 
	} 
	 
	void miOpen_Action(java.awt.event.ActionEvent event) 
	{ 
		//{{CONNECTION 
		// Action from Open... Show the OpenFileDialog 
		int		defMode			= openFileDialog1.getMode(); 
		String	defTitle		= openFileDialog1.getTitle(); 
		String defDirectory	= openFileDialog1.getDirectory(); 
		String defFile			= openFileDialog1.getFile(); 
 
		openFileDialog1 = new java.awt.FileDialog(this, defTitle, defMode); 
		openFileDialog1.setDirectory(defDirectory); 
		openFileDialog1.setFile(defFile); 
		openFileDialog1.setVisible(true); 
		//}} 
	} 



}
 
 
