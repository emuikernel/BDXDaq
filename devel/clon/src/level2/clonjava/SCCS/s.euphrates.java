h36652
s 00001/00001/01633
d D 1.2 00/09/07 10:40:52 wolin 3 1
c Moved gif area
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 euphrates/euphrates.java
e
s 01634/00000/00000
d D 1.1 00/07/31 14:57:18 gurjyan 1 0
c date and time created 00/07/31 14:57:18 by gurjyan
e
u
U
f e 0
t
T
I 1
/*
  Euphrates: Level2 trigger programming program.
  By V. Gyurjyan
  Hall B online group
  5 August 1999
  */ 

import java.awt.*; 
import java.text.*; 
import java.io.*;
import java.util.*;
import com.sun.java.swing.*; 
import symantec.itools.awt.shape.Rect; 
import com.sun.java.swing.JLabel; 
import com.sun.java.swing.JTextField; 
import com.sun.java.swing.JRadioButton; 
import symantec.itools.multimedia.ScrollingText; 
import symantec.itools.awt.shape.Circle; 

public class euphrates extends Frame 
{ 
  public boolean complex_flag = false;
  
  public euphrates() 
    { 
      //{{INIT_CONTROLS 
      setLayout(null); 
      setVisible(false); 
      setSize(927,676); 
      setBackground(new Color(12632256)); 
      openFileDialog1 = new java.awt.FileDialog(this); 
      openFileDialog1.setMode(FileDialog.LOAD); 
      openFileDialog1.setTitle("Open"); 
      //$$ openFileDialog1.move(900,624); 
D 3
      ImageIcon img1 = new ImageIcon("4_anim_10s.gif");
E 3
I 3
      ImageIcon img1 = new ImageIcon("/clasweb/gif/4_anim_10s.gif");
E 3
      logo = new com.sun.java.swing.JButton(img1);
      logo.setBounds(400,5,125,117);
      logo.setForeground(new Color(0)); 
      logo.setBackground(new Color(-3355444)); 
      add(logo); 
      
      
      level1_sec1 = new com.sun.java.swing.JButton(); 
      level1_sec1.setText("S1/B1"); 
      level1_sec1.setBounds(48,168,97,35); 
      level1_sec1.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level1_sec1.setForeground(new Color(0)); 
      level1_sec1.setBackground(new Color(-3355444)); 
      add(level1_sec1); 
      level1_sec2 = new com.sun.java.swing.JButton(); 
      level1_sec2.setText("S2/B2"); 
      level1_sec2.setBounds(156,132,97,35); 
      level1_sec2.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level1_sec2.setForeground(new Color(0)); 
      level1_sec2.setBackground(new Color(-3355444)); 
      add(level1_sec2); 
      level1_sec3 = new com.sun.java.swing.JButton(); 
      level1_sec3.setText("S3/B3"); 
      level1_sec3.setBounds(264,168,97,35); 
      level1_sec3.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level1_sec3.setForeground(new Color(0)); 
      level1_sec3.setBackground(new Color(-3355444)); 
      add(level1_sec3); 
      level1_sec4 = new com.sun.java.swing.JButton(); 
      level1_sec4.setText("S4/B4"); 
      level1_sec4.setBounds(264,216,97,35); 
      level1_sec4.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level1_sec4.setForeground(new Color(0)); 
      level1_sec4.setBackground(new Color(-3355444)); 
      add(level1_sec4); 
      level1_sec6 = new com.sun.java.swing.JButton(); 
      level1_sec6.setText("S6/B6"); 
      level1_sec6.setBounds(48,216,97,35); 
      level1_sec6.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level1_sec6.setForeground(new Color(0)); 
      level1_sec6.setBackground(new Color(-3355444)); 
      add(level1_sec6); 
      level1_sec5 = new com.sun.java.swing.JButton(); 
      level1_sec5.setText("S5/B5"); 
      level1_sec5.setBounds(156,264,97,35); 
      level1_sec5.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level1_sec5.setForeground(new Color(0)); 
      level1_sec5.setBackground(new Color(-3355444)); 
      add(level1_sec5); 
      level1_b7 = new com.sun.java.swing.JButton(); 
      level1_b7.setText("B7"); 
      level1_b7.setBounds(48,384,60,36); 
      level1_b7.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level1_b7.setForeground(new Color(0)); 
      level1_b7.setBackground(new Color(-3355444)); 
      add(level1_b7); 
      level1_b8 = new com.sun.java.swing.JButton(); 
      level1_b8.setText("B8"); 
      level1_b8.setBounds(132,384,60,36); 
      level1_b8.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level1_b8.setForeground(new Color(0)); 
      level1_b8.setBackground(new Color(-3355444)); 
      add(level1_b8); 
      level1_b9 = new com.sun.java.swing.JButton(); 
      level1_b9.setText("B9"); 
      level1_b9.setBounds(216,384,60,36); 
      level1_b9.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level1_b9.setForeground(new Color(0)); 
      level1_b9.setBackground(new Color(-3355444)); 
      add(level1_b9); 
      level1_b10 = new com.sun.java.swing.JButton(); 
      level1_b10.setText("B10"); 
      level1_b10.setBounds(300,384,60,36); 
      level1_b10.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level1_b10.setForeground(new Color(0)); 
      level1_b10.setBackground(new Color(-3355444)); 
      add(level1_b10); 
      rect1 = new symantec.itools.awt.shape.Rect(); 
      try { 
	rect1.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
      } 
      catch(java.beans.PropertyVetoException e) { } 
      rect1.setBounds(24,72,360,241); 
      add(rect1); 
      rect2 = new symantec.itools.awt.shape.Rect(); 
      try { 
	rect2.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
      } 
      catch(java.beans.PropertyVetoException e) { } 
      rect2.setBounds(24,324,360,110); 
      add(rect2); 
      or = new com.sun.java.swing.JButton(); 
      or.setText("OR"); 
      or.setBounds(432,264,62,61); 
      or.setFont(new Font("Dialog", Font.BOLD, 12)); 
      or.setForeground(new Color(0)); 
      or.setBackground(new Color(-3355444)); 
      add(or); 
      and = new com.sun.java.swing.JButton(); 
      and.setText("AND"); 
      and.setBounds(432,168,62,61); 
      and.setFont(new Font("Dialog", Font.BOLD, 10)); 
      and.setForeground(new Color(0)); 
      and.setBackground(new Color(-3355444)); 
      add(and); 
      rect4 = new symantec.itools.awt.shape.Rect(); 
      try { 
	rect4.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
      } 
      catch(java.beans.PropertyVetoException e) { } 
      rect4.setBounds(420,132,86,228); 
      add(rect4); 
      level2_sec1 = new com.sun.java.swing.JButton(); 
      level2_sec1.setText("SEC1"); 
      level2_sec1.setBounds(564,168,97,35); 
      level2_sec1.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level2_sec1.setForeground(new Color(0)); 
      level2_sec1.setBackground(new Color(-3355444)); 
      add(level2_sec1); 
      level2_sec2 = new com.sun.java.swing.JButton(); 
      level2_sec2.setText("SEC2"); 
      level2_sec2.setBounds(672,132,97,35); 
      level2_sec2.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level2_sec2.setForeground(new Color(0)); 
      level2_sec2.setBackground(new Color(-3355444)); 
      add(level2_sec2); 
      level2_sec3 = new com.sun.java.swing.JButton(); 
      level2_sec3.setText("SEC3"); 
      level2_sec3.setBounds(780,168,97,35); 
      level2_sec3.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level2_sec3.setForeground(new Color(0)); 
      level2_sec3.setBackground(new Color(-3355444)); 
      add(level2_sec3); 
      level2_sec4 = new com.sun.java.swing.JButton(); 
      level2_sec4.setText("SEC4"); 
      level2_sec4.setBounds(780,216,97,35); 
      level2_sec4.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level2_sec4.setForeground(new Color(0)); 
      level2_sec4.setBackground(new Color(-3355444)); 
      add(level2_sec4); 
      level2_sec6 = new com.sun.java.swing.JButton(); 
      level2_sec6.setText("SEC6"); 
      level2_sec6.setBounds(564,216,97,35); 
      level2_sec6.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level2_sec6.setForeground(new Color(0)); 
      level2_sec6.setBackground(new Color(-3355444)); 
      add(level2_sec6); 
      level2_sec5 = new com.sun.java.swing.JButton(); 
      level2_sec5.setText("SEC5"); 
      level2_sec5.setBounds(672,264,97,35); 
      level2_sec5.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level2_sec5.setForeground(new Color(0)); 
      level2_sec5.setBackground(new Color(-3355444)); 
      add(level2_sec5); 
      level2_any2 = new com.sun.java.swing.JButton(); 
      level2_any2.setText("ANY2"); 
      level2_any2.setBounds(672,354,97,35); 
      level2_any2.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level2_any2.setForeground(new Color(0)); 
      level2_any2.setBackground(new Color(-3355444)); 
      add(level2_any2); 
      level2_any3 = new com.sun.java.swing.JButton(); 
      level2_any3.setText("ANY3"); 
      level2_any3.setBounds(780,354,97,35); 
      level2_any3.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level2_any3.setForeground(new Color(0)); 
      level2_any3.setBackground(new Color(-3355444)); 
      add(level2_any3); 
      level2_any4 = new com.sun.java.swing.JButton(); 
      level2_any4.setText("ANY4"); 
      level2_any4.setBounds(564,402,97,35); 
      level2_any4.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level2_any4.setForeground(new Color(0)); 
      level2_any4.setBackground(new Color(-3355444)); 
      add(level2_any4); 
      level2_any5 = new com.sun.java.swing.JButton(); 
      level2_any5.setText("ANY5"); 
      level2_any5.setBounds(672,402,97,35); 
      level2_any5.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level2_any5.setForeground(new Color(0)); 
      level2_any5.setBackground(new Color(-3355444)); 
      add(level2_any5); 
      level2_all6 = new com.sun.java.swing.JButton(); 
      level2_all6.setText("ALL6"); 
      level2_all6.setBounds(780,402,97,35); 
      level2_all6.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level2_all6.setForeground(new Color(0)); 
      level2_all6.setBackground(new Color(-3355444)); 
      add(level2_all6); 
      
      //
      
      level2_atl1 = new com.sun.java.swing.JButton(); 
      level2_atl1.setText("ATL1"); 
      level2_atl1.setBounds(564,450,97,35); 
      level2_atl1.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level2_atl1.setForeground(new Color(0)); 
      level2_atl1.setBackground(new Color(-3355444)); 
      add(level2_atl1); 
      
      level2_atl2 = new com.sun.java.swing.JButton(); 
      level2_atl2.setText("ATL2"); 
      level2_atl2.setBounds(672,450,97,35); 
      level2_atl2.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level2_atl2.setForeground(new Color(0)); 
      level2_atl2.setBackground(new Color(-3355444)); 
      add(level2_atl2); 
      
      level2_atl3 = new com.sun.java.swing.JButton(); 
      level2_atl3.setText("ATL3"); 
      level2_atl3.setBounds(780,450,97,35); 
      level2_atl3.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level2_atl3.setForeground(new Color(0)); 
      level2_atl3.setBackground(new Color(-3355444)); 
      add(level2_atl3); 
      
      level2_atl4 = new com.sun.java.swing.JButton(); 
      level2_atl4.setText("ATL4"); 
      level2_atl4.setBounds(564,498,97,35); 
      level2_atl4.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level2_atl4.setForeground(new Color(0)); 
      level2_atl4.setBackground(new Color(-3355444)); 
      add(level2_atl4); 
      
      level2_atl5 = new com.sun.java.swing.JButton(); 
      level2_atl5.setText("ATL5"); 
      level2_atl5.setBounds(672,498,97,35); 
      level2_atl5.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level2_atl5.setForeground(new Color(0)); 
      level2_atl5.setBackground(new Color(-3355444)); 
      add(level2_atl5); 
      
      
      //
      
      
      
      rect5 = new symantec.itools.awt.shape.Rect(); 
      try { 
	rect5.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
      } 
      catch(java.beans.PropertyVetoException e) { } 
      rect5.setBounds(540,72,360,241); 
      add(rect5); 
      rect7 = new symantec.itools.awt.shape.Rect(); 
      try { 
	rect7.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
      } 
      catch(java.beans.PropertyVetoException e) { } 
      rect7.setBounds(540,324,361,230); 
      add(rect7); 
      level2_any1 = new com.sun.java.swing.JButton(); 
      level2_any1.setText("ANY1"); 
      level2_any1.setBounds(564,354,97,35); 
      level2_any1.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level2_any1.setForeground(new Color(0)); 
      level2_any1.setBackground(new Color(-3355444)); 
      add(level2_any1); 
      jLabel1 = new com.sun.java.swing.JLabel(); 
      jLabel1.setText("Level1 sector/TS_input_bit layout"); 
      jLabel1.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
      jLabel1.setHorizontalTextPosition(com.sun.java.swing.SwingConstants.CENTER); 
      jLabel1.setBounds(36,72,338,24); 
      jLabel1.setFont(new Font("Dialog", Font.BOLD, 13)); 
      jLabel1.setForeground(new Color(-10066279)); 
      jLabel1.setBackground(new Color(-3355444)); 
      add(jLabel1); 
      jLabel3 = new com.sun.java.swing.JLabel(); 
      jLabel3.setText("Level1 generic/calibration triggers"); 
      jLabel3.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
      jLabel3.setHorizontalTextPosition(com.sun.java.swing.SwingConstants.CENTER); 
      jLabel3.setBounds(36,324,338,24); 
      jLabel3.setFont(new Font("Dialog", Font.BOLD, 13)); 
      jLabel3.setForeground(new Color(-10066279)); 
      jLabel3.setBackground(new Color(-3355444)); 
      add(jLabel3); 
      jLabel4 = new com.sun.java.swing.JLabel(); 
      jLabel4.setText("Level2 sector layout"); 
      jLabel4.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
      jLabel4.setHorizontalTextPosition(com.sun.java.swing.SwingConstants.CENTER); 
      jLabel4.setBounds(552,72,345,25); 
      jLabel4.setFont(new Font("Dialog", Font.BOLD, 13)); 
      jLabel4.setForeground(new Color(-10066279)); 
      jLabel4.setBackground(new Color(-3355444)); 
      add(jLabel4); 
      jLabel5 = new com.sun.java.swing.JLabel(); 
      jLabel5.setText("Level2 track multiplicity"); 
      jLabel5.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
      jLabel5.setHorizontalTextPosition(com.sun.java.swing.SwingConstants.CENTER); 
      jLabel5.setBounds(552,324,336,24); 
      jLabel5.setFont(new Font("Dialog", Font.BOLD, 13)); 
      jLabel5.setForeground(new Color(-10066279)); 
      jLabel5.setBackground(new Color(-3355444)); 
      add(jLabel5); 
      /*
	rect6 = new symantec.itools.awt.shape.Rect(); 
	try { 
	rect6.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
	} 
	catch(java.beans.PropertyVetoException e) { } 
	rect6.setBounds(540,492,360,60); 
	add(rect6);
	*/ 
      level2_notl2 = new com.sun.java.swing.JButton(); 
      level2_notl2.setText("NOTL2"); 
      level2_notl2.setBounds(780,500,97,35); 
      level2_notl2.setFont(new Font("Dialog", Font.BOLD, 12)); 
      level2_notl2.setForeground(new Color(0)); 
      level2_notl2.setBackground(new Color(-3355444)); 
      add(level2_notl2); 
      resulting_trigger = new com.sun.java.swing.JTextField(); 
      resulting_trigger.setBounds(180,588,684,24); 
      resulting_trigger.setFont(new Font("SansSerif", Font.PLAIN, 12)); 
      resulting_trigger.setForeground(new Color(0)); 
      resulting_trigger.setBackground(new Color(16777215)); 
      resulting_trigger.setEditable(false);
      resulting_trigger.setText(" ");
      add(resulting_trigger); 
      rect8 = new symantec.itools.awt.shape.Rect(); 
      try { 
	rect8.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
      } 
      catch(java.beans.PropertyVetoException e) { } 
      rect8.setBounds(48,576,828,49); 
      add(rect8); 
      jLabel7 = new com.sun.java.swing.JLabel(); 
      jLabel7.setText("Resulting trigger"); 
      jLabel7.setHorizontalTextPosition(com.sun.java.swing.SwingConstants.LEFT); 
      jLabel7.setBounds(60,588,230,20); 
      jLabel7.setFont(new Font("Dialog", Font.BOLD, 13)); 
      jLabel7.setForeground(new Color(-10066279)); 
      jLabel7.setBackground(new Color(-3355444)); 
      add(jLabel7); 
      rect3 = new symantec.itools.awt.shape.Rect(); 
      try { 
	rect3.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_LOWERED); 
      } 
      catch(java.beans.PropertyVetoException e) { } 
      rect3.setBounds(12,30,385,415); 
      add(rect3); 
      rect9 = new symantec.itools.awt.shape.Rect(); 
      try { 
	rect9.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_LOWERED); 
      } 
      catch(java.beans.PropertyVetoException e) { } 
      rect9.setBounds(528,30,387,535); 
      add(rect9); 
      rect10 = new symantec.itools.awt.shape.Rect(); 
      try { 
	rect10.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_LOWERED); 
      } 
      catch(java.beans.PropertyVetoException e) { } 
      rect10.setBounds(12,456,505,108); 
      add(rect10); 
      rect11 = new symantec.itools.awt.shape.Rect(); 
      try { 
	rect11.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_RAISED); 
      } 
      catch(java.beans.PropertyVetoException e) { } 
      rect11.setBounds(156,468,352,86); 
      add(rect11); 
      jLabel2 = new com.sun.java.swing.JLabel(); 
      jLabel2.setText("Majority logic"); 
      jLabel2.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
      jLabel2.setHorizontalTextPosition(com.sun.java.swing.SwingConstants.CENTER); 
      jLabel2.setBounds(24,492,121,36); 
      jLabel2.setFont(new Font("Dialog", Font.BOLD, 13)); 
      jLabel2.setForeground(new Color(-10066279)); 
      jLabel2.setBackground(new Color(-3355444)); 
      add(jLabel2); 

      majs1 = new com.sun.java.swing.JTextField(); 
      majs1.setBounds(180,492,30,20); 
      majs1.setFont(new Font("SansSerif", Font.PLAIN, 12)); 
      majs1.setForeground(new Color(0)); 
      majs1.setBackground(new Color(16777215)); 
      majs1.setEditable(false);
      majs1.setText(" ");
      add(majs1); 

      majs2 = new com.sun.java.swing.JTextField(); 
      majs2.setBounds(235,492,30,20); 
      majs2.setFont(new Font("SansSerif", Font.PLAIN, 12)); 
      majs2.setForeground(new Color(0)); 
      majs2.setBackground(new Color(16777215)); 
      majs2.setEditable(false);
      majs2.setText(" ");
      add(majs2); 

      majs3 = new com.sun.java.swing.JTextField(); 
      majs3.setBounds(290,492,30,20); 
      majs3.setFont(new Font("SansSerif", Font.PLAIN, 12)); 
      majs3.setForeground(new Color(0)); 
      majs3.setBackground(new Color(16777215)); 
      majs3.setEditable(false);
      majs3.setText(" ");
      add(majs3); 

      majs4 = new com.sun.java.swing.JTextField(); 
      majs4.setBounds(350,492,30,20); 
      majs4.setFont(new Font("SansSerif", Font.PLAIN, 12)); 
      majs4.setForeground(new Color(0)); 
      majs4.setBackground(new Color(16777215)); 
      majs4.setEditable(false);
      majs4.setText(" ");
      add(majs4); 

      majs5 = new com.sun.java.swing.JTextField(); 
      majs5.setBounds(405,492,30,20); 
      majs5.setFont(new Font("SansSerif", Font.PLAIN, 12)); 
      majs5.setForeground(new Color(0)); 
      majs5.setBackground(new Color(16777215)); 
      majs5.setEditable(false);
      majs5.setText(" ");
      add(majs5); 

      majs6 = new com.sun.java.swing.JTextField(); 
      majs6.setBounds(460,492,30,20); 
      majs6.setFont(new Font("SansSerif", Font.PLAIN, 12)); 
      majs6.setForeground(new Color(0)); 
      majs6.setBackground(new Color(16777215)); 
      majs6.setEditable(false);
      majs6.setText(" ");
      add(majs6); 
 

      majslabel1 = new com.sun.java.swing.JLabel(); 
      majslabel1.setText("S1"); 
      majslabel1.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
      majslabel1.setHorizontalTextPosition(com.sun.java.swing.SwingConstants.CENTER); 
      majslabel1.setBounds(180,530,30,20); 
      majslabel1.setFont(new Font("Dialog", Font.BOLD, 12)); 
      majslabel1.setForeground(new Color(-10066279)); 
      majslabel1.setBackground(new Color(-3355444)); 
      add(majslabel1); 


      majslabel2 = new com.sun.java.swing.JLabel(); 
      majslabel2.setText("S2"); 
      majslabel2.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
      majslabel2.setHorizontalTextPosition(com.sun.java.swing.SwingConstants.CENTER); 
      majslabel2.setBounds(235,530,30,20); 
      majslabel2.setFont(new Font("Dialog", Font.BOLD, 12)); 
      majslabel2.setForeground(new Color(-10066279)); 
      majslabel2.setBackground(new Color(-3355444)); 
      add(majslabel2); 

      majslabel3 = new com.sun.java.swing.JLabel(); 
      majslabel3.setText("S3"); 
      majslabel3.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
      majslabel3.setHorizontalTextPosition(com.sun.java.swing.SwingConstants.CENTER); 
      majslabel3.setBounds(290,530,30,20); 
      majslabel3.setFont(new Font("Dialog", Font.BOLD, 12)); 
      majslabel3.setForeground(new Color(-10066279)); 
      majslabel3.setBackground(new Color(-3355444)); 
      add(majslabel3); 

      majslabel4 = new com.sun.java.swing.JLabel(); 
      majslabel4.setText("S4"); 
      majslabel4.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
      majslabel4.setHorizontalTextPosition(com.sun.java.swing.SwingConstants.CENTER); 
      majslabel4.setBounds(350,530,30,20); 
      majslabel4.setFont(new Font("Dialog", Font.BOLD, 12)); 
      majslabel4.setForeground(new Color(-10066279)); 
      majslabel4.setBackground(new Color(-3355444)); 
      add(majslabel4); 

      majslabel5 = new com.sun.java.swing.JLabel(); 
      majslabel5.setText("S5"); 
      majslabel5.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
      majslabel5.setHorizontalTextPosition(com.sun.java.swing.SwingConstants.CENTER); 
      majslabel5.setBounds(405,530,30,20); 
      majslabel5.setFont(new Font("Dialog", Font.BOLD, 12)); 
      majslabel5.setForeground(new Color(-10066279)); 
      majslabel5.setBackground(new Color(-3355444)); 
      add(majslabel5); 

      majslabel6 = new com.sun.java.swing.JLabel(); 
      majslabel6.setText("S6"); 
      majslabel6.setHorizontalAlignment(com.sun.java.swing.SwingConstants.CENTER); 
      majslabel6.setHorizontalTextPosition(com.sun.java.swing.SwingConstants.CENTER); 
      majslabel6.setBounds(460,530,30,20); 
      majslabel6.setFont(new Font("Dialog", Font.BOLD, 12)); 
      majslabel6.setForeground(new Color(-10066279)); 
      majslabel6.setBackground(new Color(-3355444)); 
      add(majslabel6); 
     

      rect12 = new symantec.itools.awt.shape.Rect(); 
      try { 
	rect12.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_LOWERED); 
      } 
      catch(java.beans.PropertyVetoException e) { } 
      rect12.setBounds(408,120,110,276); 
      add(rect12); 
      jLabel8 = new com.sun.java.swing.JLabel(); 
      jLabel8.setText("LEVEL1"); 
      jLabel8.setBounds(24,40,144,30); 
      jLabel8.setFont(new Font("Dialog", Font.BOLD, 18)); 
      jLabel8.setForeground(new Color(-10066279)); 
      jLabel8.setBackground(new Color(-3355444)); 
      add(jLabel8); 
      jLabel9 = new com.sun.java.swing.JLabel(); 
      jLabel9.setText("LEVEL2"); 
      jLabel9.setBounds(540,40,144,30); 
      jLabel9.setFont(new Font("Dialog", Font.BOLD, 18)); 
      jLabel9.setForeground(new Color(-10066279)); 
      jLabel9.setBackground(new Color(-3355444)); 
      add(jLabel9); 
      jLabel10 = new com.sun.java.swing.JLabel(); 
      jLabel10.setText("Level2"); 
      jLabel10.setBounds(24,456,144,30); 
      jLabel10.setFont(new Font("Dialog", Font.BOLD, 18)); 
      jLabel10.setForeground(new Color(-10066279)); 
      jLabel10.setBackground(new Color(-3355444)); 
      add(jLabel10); 
      scrollingText1 = new symantec.itools.multimedia.ScrollingText(); 
      try { 
	java.lang.String[] tempString = new java.lang.String[4]; 
	tempString[0] = new java.lang.String("*** EUPHRATES  *** (R)       Designed to program level2 triggers."); 
	tempString[1] = new java.lang.String("Remember to select the logic from left to right (ex. S1/B1 AND SEC1)."); 
	tempString[2] = new java.lang.String("Use OR to create multiple level2 triggers."); 
	tempString[3] = new java.lang.String("No more triggers or logic after Level2 multiplicity or NOTL2 selection."); 
	scrollingText1.setMessageList(tempString); 
      } 
      catch(java.beans.PropertyVetoException e) { } 
      scrollingText1.setBounds(24,648,876,22); 
      scrollingText1.setFont(new Font("Dialog", Font.ITALIC, 15)); 
      scrollingText1.setForeground(new Color(16512)); 
      add(scrollingText1); 
      rect13 = new symantec.itools.awt.shape.Rect(); 
      try { 
	rect13.setBevelStyle(symantec.itools.awt.shape.Rect.BEVEL_LOWERED); 
      } 
      catch(java.beans.PropertyVetoException e) { } 
      rect13.setBounds(12,636,900,36); 
      add(rect13); 
      setTitle("Euphrates"); 
      //}} 
      
      //{{INIT_MENUS 
      mainMenuBar = new java.awt.MenuBar(); 
      menu1 = new java.awt.Menu("File"); 
      miLoad = new java.awt.MenuItem("Load"); 
      miLoad.setShortcut(new MenuShortcut(java.awt.event.KeyEvent.VK_L,false)); 
      menu1.add(miLoad); 
      miOpen = new java.awt.MenuItem("Open..."); 
      miOpen.setShortcut(new MenuShortcut(java.awt.event.KeyEvent.VK_O,false)); 
      menu1.add(miOpen); 
      miSave = new java.awt.MenuItem("Save"); 
      miSave.setShortcut(new MenuShortcut(java.awt.event.KeyEvent.VK_S,false)); 
      menu1.add(miSave); 
      miSaveAs = new java.awt.MenuItem("Save As..."); 
      miSaveAs.setShortcut(new MenuShortcut(java.awt.event.KeyEvent.VK_W,false)); 
      menu1.add(miSaveAs);
      miSaveMaj = new java.awt.MenuItem("Save MLF"); 
      miSaveMaj.setShortcut(new MenuShortcut(java.awt.event.KeyEvent.VK_M,false)); 
      menu1.add(miSaveMaj);
      menu1.addSeparator(); 
      miExit = new java.awt.MenuItem("Exit"); 
      menu1.add(miExit); 
      mainMenuBar.add(menu1); 
      menu2 = new java.awt.Menu("Edit"); 
      enEdit = new java.awt.MenuItem("Enable Edit"); 
      enEdit.setShortcut(new MenuShortcut(java.awt.event.KeyEvent.VK_E,false)); 
      menu2.add(enEdit); 
      disEdit = new java.awt.MenuItem("Disable Edit"); 
      disEdit.setShortcut(new MenuShortcut(java.awt.event.KeyEvent.VK_D,false)); 
      menu2.add(disEdit); 
      miClear = new java.awt.MenuItem("Clear"); 
      miClear.setShortcut(new MenuShortcut(java.awt.event.KeyEvent.VK_C,false)); 
      menu2.add(miClear); 
      mainMenuBar.add(menu2); 
      menu3 = new java.awt.Menu("Help"); 
      mainMenuBar.setHelpMenu(menu3); 
      miAbout = new java.awt.MenuItem("About.."); 
      menu3.add(miAbout); 
      mainMenuBar.add(menu3); 
      setMenuBar(mainMenuBar); 
      //$$ mainMenuBar.move(876,624); 
      //}} 
      
      //{{REGISTER_LISTENERS 
      SymWindow aSymWindow = new SymWindow(); 
      this.addWindowListener(aSymWindow); 
      SymAction lSymAction = new SymAction(); 
      miLoad.addActionListener(lSymAction);
      miOpen.addActionListener(lSymAction);
      miSave.addActionListener(lSymAction);
      miSaveAs.addActionListener(lSymAction);
      miSaveMaj.addActionListener(lSymAction);
 
      miAbout.addActionListener(lSymAction); 
      miExit.addActionListener(lSymAction);
      miClear.addActionListener(lSymAction);
      enEdit.addActionListener(lSymAction);
      disEdit.addActionListener(lSymAction);

      level1_b7.addActionListener(lSymAction);
      level1_b8.addActionListener(lSymAction);
      level1_b9.addActionListener(lSymAction);
      level1_b10.addActionListener(lSymAction);
      or.addActionListener(lSymAction);
      and.addActionListener(lSymAction);
      
      level1_sec1.addActionListener(lSymAction);
      level1_sec2.addActionListener(lSymAction);
      level1_sec3.addActionListener(lSymAction);
      level1_sec4.addActionListener(lSymAction);
      level1_sec5.addActionListener(lSymAction);
      level1_sec6.addActionListener(lSymAction);
      
      level2_sec1.addActionListener(lSymAction);
      level2_sec2.addActionListener(lSymAction);
      level2_sec3.addActionListener(lSymAction);
      level2_sec4.addActionListener(lSymAction);
      level2_sec5.addActionListener(lSymAction);
      level2_sec6.addActionListener(lSymAction);
      
      level2_any1.addActionListener(lSymAction);
      level2_any2.addActionListener(lSymAction);
      level2_any3.addActionListener(lSymAction);
      level2_any4.addActionListener(lSymAction);
      level2_any5.addActionListener(lSymAction);
      level2_all6.addActionListener(lSymAction);
      level2_atl1.addActionListener(lSymAction);
      level2_atl2.addActionListener(lSymAction);
      level2_atl3.addActionListener(lSymAction);
      level2_atl4.addActionListener(lSymAction);
      level2_atl5.addActionListener(lSymAction);
      level2_notl2.addActionListener(lSymAction);
      //}} 
    } 
  
  public euphrates(String title) 
    { 
      this(); 
      setTitle(title); 
    } 
  
  /** 
   * Shows or hides the component depending on the boolean flag b. 
   * @param b  if true, show the component; otherwise, hide the component. 
   * @see java.awt.Component#isVisible 
   */ 
  public  int run_unix (String command) {
    int status = 0;
    Runtime r = Runtime.getRuntime();
    try {
      Process p = r.exec (command);
      
    } catch (IOException e) {
      System.out.println("run_unix exception: " +e);
      status = -1;
    }
    System.out.println(status);
    return status;
  }
  
  
  public void setVisible(boolean b) 
    { 
      if(b) 
	{ 
	  setLocation(50, 50); 
	}	 
      super.setVisible(b); 
    } 
  
  static public void main(String args[]) 
    { 
      (new euphrates()).setVisible(true); 
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
  com.sun.java.swing.JButton level1_sec1; 
  com.sun.java.swing.JButton level1_sec2; 
  com.sun.java.swing.JButton level1_sec3; 
  com.sun.java.swing.JButton level1_sec4; 
  com.sun.java.swing.JButton level1_sec6; 
  com.sun.java.swing.JButton level1_sec5; 
  com.sun.java.swing.JButton level1_b7; 
  com.sun.java.swing.JButton level1_b8; 
  com.sun.java.swing.JButton level1_b9; 
  com.sun.java.swing.JButton level1_b10; 
  symantec.itools.awt.shape.Rect rect1; 
  symantec.itools.awt.shape.Rect rect2; 
  com.sun.java.swing.JButton or; 
  com.sun.java.swing.JButton and; 
  symantec.itools.awt.shape.Rect rect4; 
  com.sun.java.swing.JButton level2_sec1; 
  com.sun.java.swing.JButton level2_sec2; 
  com.sun.java.swing.JButton level2_sec3; 
  com.sun.java.swing.JButton level2_sec4; 
  com.sun.java.swing.JButton level2_sec6; 
  com.sun.java.swing.JButton level2_sec5; 
  com.sun.java.swing.JButton level2_any2; 
  com.sun.java.swing.JButton level2_any3; 
  com.sun.java.swing.JButton level2_any4; 
  com.sun.java.swing.JButton level2_any5; 
  com.sun.java.swing.JButton level2_all6; 
  symantec.itools.awt.shape.Rect rect5; 
  symantec.itools.awt.shape.Rect rect7; 
  com.sun.java.swing.JButton level2_any1; 
  com.sun.java.swing.JButton level2_atl1; 
  com.sun.java.swing.JButton level2_atl2; 
  com.sun.java.swing.JButton level2_atl3; 
  com.sun.java.swing.JButton level2_atl4; 
  com.sun.java.swing.JButton level2_atl5; 
  com.sun.java.swing.JLabel jLabel1; 
  com.sun.java.swing.JLabel jLabel3; 
  com.sun.java.swing.JLabel jLabel4; 
  com.sun.java.swing.JLabel jLabel5; 
  symantec.itools.awt.shape.Rect rect6; 
  com.sun.java.swing.JButton level2_notl2; 
  com.sun.java.swing.JLabel jLabel6; 
  com.sun.java.swing.JTextField resulting_trigger; 
  com.sun.java.swing.JTextField majs1; 
  com.sun.java.swing.JTextField majs2; 
  com.sun.java.swing.JTextField majs3; 
  com.sun.java.swing.JTextField majs4; 
  com.sun.java.swing.JTextField majs5; 
  com.sun.java.swing.JTextField majs6; 
  com.sun.java.swing.JLabel majslabel1; 
  com.sun.java.swing.JLabel majslabel2; 
  com.sun.java.swing.JLabel majslabel3; 
  com.sun.java.swing.JLabel majslabel4; 
  com.sun.java.swing.JLabel majslabel5; 
  com.sun.java.swing.JLabel majslabel6; 
  symantec.itools.awt.shape.Rect rect8; 
  com.sun.java.swing.JLabel jLabel7; 
  symantec.itools.awt.shape.Rect rect3; 
  symantec.itools.awt.shape.Rect rect9; 
  symantec.itools.awt.shape.Rect rect10; 
  symantec.itools.awt.shape.Rect rect11; 
  com.sun.java.swing.JLabel jLabel2; 
  java.awt.Checkbox late_fail; 
  CheckboxGroup Group1; 
  java.awt.Checkbox fast_clear; 
  symantec.itools.awt.shape.Rect rect12; 
  com.sun.java.swing.JLabel jLabel8; 
  com.sun.java.swing.JLabel jLabel9; 
  com.sun.java.swing.JLabel jLabel10; 
  symantec.itools.multimedia.ScrollingText scrollingText1; 
  symantec.itools.awt.shape.Rect rect13; 
  com.sun.java.swing.JButton logo; 
  //}} 
  
  //{{DECLARE_MENUS 
  java.awt.MenuBar mainMenuBar; 
  java.awt.Menu menu1; 
  java.awt.MenuItem miLoad; 
  java.awt.MenuItem miOpen; 
  java.awt.MenuItem miSave; 
  java.awt.MenuItem miSaveAs;
  java.awt.MenuItem miSaveMaj;
  java.awt.MenuItem miExit; 
  java.awt.Menu menu2; 
  java.awt.MenuItem enEdit; 
  java.awt.MenuItem disEdit; 
  java.awt.MenuItem miClear; 
  java.awt.Menu menu3; 
  java.awt.MenuItem miAbout; 
  //}} 
  
  class SymWindow extends java.awt.event.WindowAdapter 
  { 
    public void windowClosing(java.awt.event.WindowEvent event) 
      { 
	Object object = event.getSource(); 
	if (object == euphrates.this) 
	  euphrates_WindowClosing(event); 
      } 
  } 
  
  void euphrates_WindowClosing(java.awt.event.WindowEvent event) 
    { 
      setVisible(false);	// hide the Frame 
      dispose();			// free the system resources 
      System.exit(0);		// close the application 
    } 
  
  class SymAction implements java.awt.event.ActionListener 
  { 
    
    public void actionPerformed(java.awt.event.ActionEvent event) 
      { 
	Object object = event.getSource(); 
	if (object == miOpen) 
	  miOpen_Action(event); 
	else if (object == miLoad) 
	  miLoad_Action(); 
	else if (object == miSave) 
	  miSave_Action(); 
	else if (object == miSaveAs) 
	  miSaveAs_Action(); 
	else if (object == miSaveMaj) 
	  miSaveMaj_Action(); 
	else if (object == miAbout) 
	  miAbout_Action(event); 
	else if (object == miExit) 
	  miExit_Action(event); 
	else if (object == miClear) 
	  miClear_Action(); 
	else if (object == enEdit) 
	  enEdit_Action(); 
	else if (object == disEdit) 
	  disEdit_Action(); 
	
	else if (object == level1_b7) 
	  level1_b7_Action(); 
	else if (object == level1_b8) 
	  level1_b8_Action(); 
	else if (object == level1_b9) 
	  level1_b9_Action(); 
	else if (object == level1_b10) 
	  level1_b10_Action(); 
	else if (object == or) 
	  or_Action(); 
	else if (object == and) 
	  and_Action(); 
	
	
	else if (object == level1_sec1) 
	  level1_sec1_Action(); 
	else if (object == level1_sec2) 
	  level1_sec2_Action(); 
	else if (object == level1_sec3) 
	  level1_sec3_Action(); 
	else if (object == level1_sec4) 
	  level1_sec4_Action(); 
	else if (object == level1_sec5) 
	  level1_sec5_Action(); 
	else if (object == level1_sec6) 
	  level1_sec6_Action(); 
	
	else if (object == level2_sec1) 
	  level2_sec1_Action(); 
	else if (object == level2_sec2) 
	  level2_sec2_Action(); 
	else if (object == level2_sec3) 
	  level2_sec3_Action(); 
	else if (object == level2_sec4) 
	  level2_sec4_Action(); 
	else if (object == level2_sec5) 
	  level2_sec5_Action(); 
	else if (object == level2_sec6) 
	  level2_sec6_Action(); 
	
	else if (object == level2_any1) 
	  level2_any1_Action(); 
	else if (object == level2_any2) 
	  level2_any2_Action(); 
	else if (object == level2_any3) 
	  level2_any3_Action(); 
	else if (object == level2_any4) 
	  level2_any4_Action(); 
	else if (object == level2_any5) 
	  level2_any5_Action(); 
	else if (object == level2_all6) 
	  level2_all6_Action(); 
	else if (object == level2_atl1) 
	  level2_atl1_Action(); 
	else if (object == level2_atl2) 
	  level2_atl2_Action(); 
	else if (object == level2_atl3) 
	  level2_atl3_Action(); 
	else if (object == level2_atl4) 
	  level2_atl4_Action(); 
	else if (object == level2_atl5) 
	  level2_atl5_Action(); 
	else if (object == level2_notl2) 
	  level2_notl2_Action(); 
	
      } 
  } 
  
  // actions for caqlibration bits and logic
  void level1_b7_Action() 
    { 
      char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
      System.out.println(ctmp);
      if(ctmp == ' ' | ctmp == '|' | ctmp == 'x') {
	String tmp = resulting_trigger.getText();
	resulting_trigger.setText(tmp+"L1B7");
      }
      else System.out.println("ERROR");
    } 
  
  void level1_b8_Action() 
    { 
      char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
      System.out.println(ctmp);
      if(ctmp == ' ' | ctmp == '|' | ctmp == 'x') {
	String tmp = resulting_trigger.getText();
	resulting_trigger.setText(tmp+"L1B8");
      }
      else System.out.println("ERROR");
    } 
  
  void level1_b9_Action() 
    { 
      char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
      System.out.println(ctmp);
      if(ctmp == ' ' | ctmp == '|' | ctmp == 'x') {
	String tmp = resulting_trigger.getText();
	resulting_trigger.setText(tmp+"L1B9");
      }
      else System.out.println("ERROR");
    } 
  
  void level1_b10_Action() 
    { 
      char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
      System.out.println(ctmp);
      if(ctmp == ' ' | ctmp == '|' | ctmp == 'x') {
	String tmp = resulting_trigger.getText();
	resulting_trigger.setText(tmp+"L1B10");
      }
      else System.out.println("ERROR");
    } 
  
  void or_Action() 
    { 
      char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
      System.out.println(ctmp);
      if(ctmp == ' ' | ctmp == '|' | ctmp == 'x') 
	System.out.println("ERROR");
      else  {
	String tmp = resulting_trigger.getText();
	resulting_trigger.setText(tmp+"||");
	complex_flag = false;
      }
    } 
  
  void and_Action() 
    { 
      char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
      System.out.println(ctmp);
      if(ctmp == ' ' | ctmp == '|' | ctmp == 'x') 
	System.out.println("ERROR");
      else  {
	String tmp = resulting_trigger.getText();
	resulting_trigger.setText(tmp+"x");
      }
    } 
  
  
  // actions for sector based level1
  void level1_sec1_Action() 
    { 
      char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
      System.out.println(ctmp);
      if(ctmp == ' ' | ctmp == '|' | ctmp == 'x') {
	String tmp = resulting_trigger.getText();
	resulting_trigger.setText(tmp+"L1S1");
      }
      else System.out.println("ERROR");
    } 
  
  void level1_sec2_Action() 
    { 
      char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
      System.out.println(ctmp);
      if(ctmp == ' ' | ctmp == '|' | ctmp == 'x') {
	String tmp = resulting_trigger.getText();
	resulting_trigger.setText(tmp+"L1S2");
      }
      else System.out.println("ERROR");
    } 
  
  void level1_sec3_Action() 
    { 
      char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
      System.out.println(ctmp);
      if(ctmp == ' ' | ctmp == '|' | ctmp == 'x') {
	String tmp = resulting_trigger.getText();
	resulting_trigger.setText(tmp+"L1S3");
      }
      else System.out.println("ERROR");
    } 
  
  void level1_sec4_Action() 
    { 
      char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
      System.out.println(ctmp);
      if(ctmp == ' ' | ctmp == '|' | ctmp == 'x') {
	String tmp = resulting_trigger.getText();
	resulting_trigger.setText(tmp+"L1S4");
      }
      else System.out.println("ERROR");
    } 
  
  void level1_sec5_Action() 
    { 
      char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
      System.out.println(ctmp);
      if(ctmp == ' ' | ctmp == '|' | ctmp == 'x') {
	String tmp = resulting_trigger.getText();
	resulting_trigger.setText(tmp+"L1S5");
      }
      else System.out.println("ERROR");
    } 
  
  void level1_sec6_Action() 
    { 
      char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
      System.out.println(ctmp);
      if(ctmp == ' ' | ctmp == '|' | ctmp == 'x') {
	String tmp = resulting_trigger.getText();
	resulting_trigger.setText(tmp+"L1S6");
      }
      else System.out.println("ERROR");
    } 
  
  
  // actions for sector based level2
  void level2_sec1_Action() 
    { 
      char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
      System.out.println(ctmp);
      if(ctmp == ' ' | ctmp == '|' | ctmp == 'x') {
	String tmp = resulting_trigger.getText();
	resulting_trigger.setText(tmp+"L2S1");
	complex_flag = true;
      }
      else System.out.println("ERROR");
    } 
  
  void level2_sec2_Action() 
    { 
      char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
      System.out.println(ctmp);
      if(ctmp == ' ' | ctmp == '|' | ctmp == 'x') {
	String tmp = resulting_trigger.getText();
	resulting_trigger.setText(tmp+"L2S2");
	complex_flag = true;
      }
      else System.out.println("ERROR");
    } 
  
  void level2_sec3_Action() 
    { 
      char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
      System.out.println(ctmp);
      if(ctmp == ' ' | ctmp == '|' | ctmp == 'x') {
	String tmp = resulting_trigger.getText();
	resulting_trigger.setText(tmp+"L2S3");
	complex_flag = true;
      }
      else System.out.println("ERROR");
    } 
  
  void level2_sec4_Action() 
    { 
      char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
      System.out.println(ctmp);
      if(ctmp == ' ' | ctmp == '|' | ctmp == 'x') {
	String tmp = resulting_trigger.getText();
	resulting_trigger.setText(tmp+"L2S4");
	complex_flag = true;
      }
      else System.out.println("ERROR");
    } 
  
  void level2_sec5_Action() 
    { 
      char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
      System.out.println(ctmp);
      if(ctmp == ' ' | ctmp == '|' | ctmp == 'x') {
	String tmp = resulting_trigger.getText();
	resulting_trigger.setText(tmp+"L2S5");
	complex_flag = true;
      }
      else System.out.println("ERROR");
    } 
  
  void level2_sec6_Action() 
    { 
      char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
      System.out.println(ctmp);
      if(ctmp == ' ' | ctmp == '|' | ctmp == 'x') {
	String tmp = resulting_trigger.getText();
	resulting_trigger.setText(tmp+"L2S6");
	complex_flag = true;
      }
      else System.out.println("ERROR");
    } 
  
  // actions for level2 track mutiplicity buttons
  void level2_any1_Action() 
    { 
      if(complex_flag == false) {
	char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
	System.out.println(ctmp);
	if(ctmp == 'x') {
	  String tmp = resulting_trigger.getText();
	  resulting_trigger.setText(tmp+"ANY1||");
	}
	else System.out.println("ERROR");
      }
      else System.out.println("What?");
    }

  void level2_any2_Action() 
    { 
      if(complex_flag == false) {
	char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
	System.out.println(ctmp);
	if(ctmp == 'x') {
	  String tmp = resulting_trigger.getText();
	  resulting_trigger.setText(tmp+"ANY2||");
	}
	else System.out.println("ERROR");
      }
      else System.out.println("What?");
    }
  
  void level2_any3_Action() 
    { 
      if(complex_flag == false) {
	char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
	System.out.println(ctmp);
	if(ctmp == 'x') {
	  String tmp = resulting_trigger.getText();
	  resulting_trigger.setText(tmp+"ANY3||");
	}
	else System.out.println("ERROR");
      }
      else System.out.println("What?");
    }
  
  void level2_any4_Action() 
    { 
      if(complex_flag == false) {
	char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
	System.out.println(ctmp);
	if(ctmp == 'x') {
	  String tmp = resulting_trigger.getText();
	  resulting_trigger.setText(tmp+"ANY4||");
	}
	else System.out.println("ERROR");
      }
      else System.out.println("What?");
    }
  
  void level2_any5_Action() 
    { 
      if(complex_flag == false) {
	char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
	System.out.println(ctmp);
	if(ctmp == 'x') {
	  String tmp = resulting_trigger.getText();
	  resulting_trigger.setText(tmp+"ANY5||");
	}
	else System.out.println("ERROR");
      }
      else System.out.println("What?");
    }
  
  void level2_all6_Action() 
    { 
      if(complex_flag == false) {
	char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
	System.out.println(ctmp);
	if(ctmp == 'x') {
	  String tmp = resulting_trigger.getText();
	  resulting_trigger.setText(tmp+"ANY6||");
	}
	else System.out.println("ERROR");
      }
      else System.out.println("What?");
    }
  
  void level2_atl1_Action() 
    { 
      if(complex_flag == false) {
	char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
	System.out.println(ctmp);
	if(ctmp == 'x') {
	  String tmp = resulting_trigger.getText();
	  resulting_trigger.setText(tmp+"ATL1||");
	}
	else System.out.println("ERROR");
      }
      else System.out.println("What?");
    }
  
  void level2_atl2_Action() 
    { 
      if(complex_flag == false) {
	char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
	System.out.println(ctmp);
	if(ctmp == 'x') {
	  String tmp = resulting_trigger.getText();
	  resulting_trigger.setText(tmp+"ATL2||");
	}
	else System.out.println("ERROR");
      }
      else System.out.println("What?");
    }
  
  void level2_atl3_Action() 
    { 
      if(complex_flag == false) {
	char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
	System.out.println(ctmp);
	if(ctmp == 'x') {
	  String tmp = resulting_trigger.getText();
	  resulting_trigger.setText(tmp+"ATL3||");
	}
	else System.out.println("ERROR");
      }
      else System.out.println("What?");
    }
  
  void level2_atl4_Action() 
    { 
      if(complex_flag == false) {
	char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
	System.out.println(ctmp);
	if(ctmp == 'x') {
	  String tmp = resulting_trigger.getText();
	  resulting_trigger.setText(tmp+"ATL4||");
	}
	else System.out.println("ERROR");
      }
      else System.out.println("What?");
    }
  
  void level2_atl5_Action() 
    { 
      if(complex_flag == false) {
	char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
	System.out.println(ctmp);
	if(ctmp == 'x') {
	  String tmp = resulting_trigger.getText();
	  resulting_trigger.setText(tmp+"ATL5||");
	}
	else System.out.println("ERROR");
      }
      else System.out.println("What?");
    }
  
  void level2_notl2_Action() 
    { 
      if(complex_flag == false) {
	char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
	System.out.println(ctmp);
	if(ctmp == 'x') {
	  String tmp = resulting_trigger.getText();
	  resulting_trigger.setText(tmp+"NOTL2||");
	}
      else System.out.println("ERROR");
      }
      else System.out.println("What?");
    }
  
  
  
  
  void miAbout_Action(java.awt.event.ActionEvent event) 
    { 
      //{{CONNECTION 
      // Action from About Create and show as modal 
      //(new AboutDialog(this, true)).setVisible(true); 
      //}} 
    } 
  
  void miExit_Action(java.awt.event.ActionEvent event) 
    { 

      String defFile	   = openFileDialog1.getFile(); 
      System.out.println(defFile);
		//{{CONNECTION 
      // Action from Exit Create and show as modal 
      //(new QuitDialog(this, true)).setVisible(true); 
      //}} 
    } 
  
  void miClear_Action() 
    { 
      try { 
	java.lang.String[] tempString = new java.lang.String[4]; 
	tempString[0] = new java.lang.String("*** EUPHRATES  *** (R)       Designed to program level2 triggers."); 
	tempString[1] = new java.lang.String("Remember to select the logic from left to right (ex. S1/B1 AND SEC1)."); 
	tempString[2] = new java.lang.String("Use OR to create multiple level2 triggers."); 
	tempString[3] = new java.lang.String("No more triggers or logic after Level2 multiplicity or NOTL2 selection."); 
	scrollingText1.setMessageList(tempString); 
      } 
      catch(java.beans.PropertyVetoException e) { } 
      resulting_trigger.setText(" ");
    } 

  void enEdit_Action() 
    { 
      jLabel7.setForeground(new Color(66279)); 
      resulting_trigger.setEditable(true);
      majs1.setEditable(true);
      majs2.setEditable(true);
      majs3.setEditable(true);
      majs4.setEditable(true);
      majs5.setEditable(true);
      majs6.setEditable(true);
    } 

  void disEdit_Action() 
    { 
      jLabel7.setForeground(new Color(-10066279)); 
      resulting_trigger.setEditable(false);
      majs1.setEditable(false);
      majs2.setEditable(false);
      majs3.setEditable(false);
      majs4.setEditable(false);
      majs5.setEditable(false);
      majs6.setEditable(false);
    } 

  void miLoad_Action() 
    {
      // Action from Open... Show the OpenFileDialog 
      int    defMode	   = openFileDialog1.getMode(); 
      String defTitle	   = openFileDialog1.getTitle(); 
      String defDirectory  = openFileDialog1.getDirectory(); 
      String defFile	   = openFileDialog1.getFile(); 
      
      openFileDialog1 = new java.awt.FileDialog(this, defTitle, defMode); 
      openFileDialog1.setDirectory(defDirectory);
      openFileDialog1.setFile(defFile); 
      openFileDialog1.setVisible(true);
      LoadIt();
    }


  void LoadIt() {
    // majority logic loading
      run_unix("KKK");
      //loading the level2 staff
    String tempFile	   = openFileDialog1.getFile();
    String tempDir       = openFileDialog1.getDirectory();
    String extension = " ";
    StringTokenizer tokens = new StringTokenizer(tempFile,".");
    while(tokens.hasMoreTokens() == true) 
      extension  = tokens.nextToken();
    
    if(tempDir == null | tempFile == null) {
      try {
	String[] tempString = new java.lang.String[3]; 
	tempString[0] = new String("Nothing to load.  ");
	tempString[1] = new java.lang.String("Try another time. In case of the problem contact the author.");
	tempString[2] = new java.lang.String("*** EUPHRATES  *** (R)       Designed to program level2 triggers."); 
	scrollingText1.setMessageList(tempString);
      } catch(java.beans.PropertyVetoException g) { }
      return;
    }
    if (extension.equals("trg")) { 
      run_unix("rm /usr/local/clas/parms/euphrates/current.trg");
      run_unix("euphrates "+tempDir+tempFile);
      System.out.println("euphrates "+tempDir+tempFile);
      run_unix("ln -s "+tempDir+tempFile+" /usr/local/clas/parms/euphrates/current.trg"); 
      System.out.println("ln -s "+tempDir+tempFile+" /usr/local/clas/parms/euphrates/current.trg");
      try {
	String[] tempString = new java.lang.String[3]; 
	tempString[0] = new String("The trigger file " +tempDir+tempFile + " was loaded at " +(new Date()));
	tempString[1] = new java.lang.String("To create the new trigger file use 'Edit' menu 'Clear' button.");
	
	tempString[2] = new java.lang.String("*** EUPHRATES  *** (R)       Designed to program level2 triggers."); 
	scrollingText1.setMessageList(tempString);
      } catch(java.beans.PropertyVetoException e) { }
    }	
    else 
      {
	try {
	  String[] tempString = new java.lang.String[3]; 
	  tempString[0] = new String("Wrong file!!!.  Attention level2 trigger files should have 'trg' extension.  ");
	  tempString[1] = new java.lang.String("Try another time. In case of the problem contact the author.");
	  tempString[2] = new java.lang.String("*** EUPHRATES  *** (R)       Designed to program level2 triggers."); 
	  scrollingText1.setMessageList(tempString);
	} catch(java.beans.PropertyVetoException g) { }
      }   
  } 
  
  void miOpen_Action(java.awt.event.ActionEvent event) 
    { 
      // Action from Open... Show the OpenFileDialog 
      int    defMode	   = openFileDialog1.getMode(); 
      String defTitle	   = openFileDialog1.getTitle(); 
      String defDirectory  = openFileDialog1.getDirectory(); 
      String defFile	   = openFileDialog1.getFile(); 
      
      openFileDialog1 = new java.awt.FileDialog(this, defTitle, defMode); 
      openFileDialog1.setDirectory("/usr/local/clas/parms/euphrates");
      openFileDialog1.setFile(defFile); 
      openFileDialog1.setVisible(true);
      ViewIt();
      //}} 
    } 

  void miSave_Action() 
    { 
      // Action from Open... Show the OpenFileDialog 
      int    defMode	   = openFileDialog1.getMode(); 
      String defTitle	   = openFileDialog1.getTitle(); 
      String defDirectory  = openFileDialog1.getDirectory(); 
      String defFile	   = openFileDialog1.getFile(); 
      
      openFileDialog1 = new java.awt.FileDialog(this, defTitle, defMode); 
      openFileDialog1.setDirectory(defDirectory);
      openFileDialog1.setFile(defFile); 
      openFileDialog1.setVisible(true);
      SaveIt();
    }
  void miSaveAs_Action() 
    { 
      // Action from Open... Show the OpenFileDialog 
      int    defMode	   = openFileDialog1.getMode(); 
      String defTitle	   = openFileDialog1.getTitle(); 
      String defDirectory  = openFileDialog1.getDirectory(); 
      String defFile	   = openFileDialog1.getFile(); 
      
      openFileDialog1 = new java.awt.FileDialog(this, defTitle, defMode); 
      openFileDialog1.setDirectory(defDirectory);
      openFileDialog1.setVisible(true);
      SaveIt();
    }
  
  void miSaveMaj_Action() {
    if((Integer.parseInt(majs1.getText())<0 | Integer.parseInt(majs1.getText())>7) |(Integer.parseInt(majs2.getText())<0 | Integer.parseInt(majs2.getText())>7) |(Integer.parseInt(majs3.getText())<0 | Integer.parseInt(majs3.getText())>7) |(Integer.parseInt(majs4.getText())<0 | Integer.parseInt(majs4.getText())>7) |(Integer.parseInt(majs5.getText())<0 | Integer.parseInt(majs5.getText())>7) |(Integer.parseInt(majs6.getText())<0 | Integer.parseInt(majs6.getText())>7) )
      {
	try {
	  String[] tempString = new java.lang.String[3]; 
	  tempString[0] = new String("Wrong majority logic request! ");
	  tempString[1] = new java.lang.String("Try another time. In case of the problem contact the author.");
	  tempString[2] = new java.lang.String("*** EUPHRATES  *** (R)       Designed to program level2 triggers."); 
	  scrollingText1.setMessageList(tempString);
	} catch(java.beans.PropertyVetoException g) { }
	return; 
      }
    else {
      try {
	PrintWriter writer = new PrintWriter(new FileWriter("/usr/local/clas/parms/euphrates/majlogic.dat"));
	writer.println("1 "+majs1.getText()+" 0");
	writer.println("2 "+majs2.getText()+" 0");
	writer.println("3 "+majs3.getText()+" 0");
	writer.println("4 "+majs4.getText()+" 0");
	writer.println("5 "+majs5.getText()+" 0");
	writer.println("6 "+majs6.getText()+" 0");
	writer.close();
	try {
	  String[] tempString = new java.lang.String[3]; 
	  tempString[0] = new java.lang.String("Majority logic settings are saved in the /usr/local/clas/parms/euphrates/majlogic.dat ");
	  tempString[1] = new java.lang.String("*** EUPHRATES  *** (R)       Designed to program level2 triggers."); 
	  scrollingText1.setMessageList(tempString);
	} catch(java.beans.PropertyVetoException g) { }

      } catch (IOException e) {}
    }
  }

  void SaveIt() {
    String tempFile	   = openFileDialog1.getFile();
    String tempDir       = openFileDialog1.getDirectory(); 
    
    char ctmp = (new StringCharacterIterator(resulting_trigger.getText())).last();
    if ( ctmp == ' ') {
      try {
	String[] tempString = new java.lang.String[3]; 
	tempString[0] = new String("Nothing to save. ");
	tempString[1] = new java.lang.String("Try another time. In case of the problem contact the author.");
	tempString[2] = new java.lang.String("*** EUPHRATES  *** (R)       Designed to program level2 triggers."); 
	scrollingText1.setMessageList(tempString);
      } catch(java.beans.PropertyVetoException g) { }
return; 
    }
    try {
      PrintWriter writer = new PrintWriter(new FileWriter(tempDir+tempFile));
      if(ctmp !='|')writer.println(resulting_trigger.getText()+"||");
      else writer.println(resulting_trigger.getText());
      writer.close();
    } catch (IOException e) { 
      System.out.println(e);
      resulting_trigger.setText(" ");
      try {
	String[] tempString = new java.lang.String[3]; 
	tempString[0] = new String("Error saving the file " +tempDir+tempFile);
	tempString[1] = new java.lang.String("Try another time. In case of the problem contact the author.");
	tempString[2] = new java.lang.String("*** EUPHRATES  *** (R)       Designed to program level2 triggers."); 
	scrollingText1.setMessageList(tempString);
      } catch(java.beans.PropertyVetoException g) { }

      return;
    }
    try {
      String[] tempString = new java.lang.String[3]; 
      tempString[0] = new String("The resulting trigger configuration was saved in " +tempDir+tempFile);
      tempString[1] = new java.lang.String("To create the new trigger file use 'Edit' menu 'Clear' button.");
      
      tempString[2] = new java.lang.String("*** EUPHRATES  *** (R)       Designed to program level2 triggers."); 
      scrollingText1.setMessageList(tempString);
    } catch(java.beans.PropertyVetoException e) { }
  }
  
  void ViewIt() 
    {
      String sec;
      String[] majlev = new String[6]; 
      String tempLine = " ";
      StringTokenizer majtokens;
      String tempFile	   = openFileDialog1.getFile();
      String tempDir       = openFileDialog1.getDirectory(); 
      System.out.println(tempDir+tempFile);

      try {
	BufferedReader majreader = new BufferedReader( new FileReader("/usr/local/clas/parms/euphrates/majlogic.dat"));
	for( int i =0; i<=5;i++) {
	tempLine = majreader.readLine();
	System.out.println(tempLine);
	majtokens = new StringTokenizer(tempLine);
	sec = majtokens.nextToken();
	System.out.println(sec);
	majlev[i] =  majtokens.nextToken();
	System.out.println(majlev[0]);
	}

	majs1.setText(majlev[0]);
	majs2.setText(majlev[1]);
	majs3.setText(majlev[2]);
	majs4.setText(majlev[3]);
	majs5.setText(majlev[4]);
	majs6.setText(majlev[5]);
	majreader.close();
      } catch (IOException g) { 
	System.out.println(g);
      }

      try {
	BufferedReader reader = new BufferedReader( new FileReader(tempDir+tempFile));
	tempLine = reader.readLine();
	resulting_trigger.setText(tempLine);
	reader.close();
      } catch (IOException e) { 
	resulting_trigger.setText(" ");
	System.out.println(e);
      try {
	String[] tempString = new java.lang.String[3]; 
	tempString[0] = new String("Error oppening the file " +tempDir+tempFile);
	tempString[1] = new java.lang.String("Check the path and level2 trigger configuration file name.");
	
	tempString[2] = new java.lang.String("*** EUPHRATES  *** (R)       Designed to program level2 triggers."); 
	scrollingText1.setMessageList(tempString);
      } catch(java.beans.PropertyVetoException g) { }

	return;
      }
      try {
	String[] tempString = new java.lang.String[3]; 
	tempString[0] = new String("The following trigger was opened from " +tempDir+tempFile);
	tempString[1] = new java.lang.String("To load the trigger use 'File' menu 'load' button. ");
	
	tempString[2] = new java.lang.String("*** EUPHRATES  *** (R)       Designed to program level2 triggers."); 
	scrollingText1.setMessageList(tempString);
      } catch(java.beans.PropertyVetoException e) { }
    }
  
  
  
} 






E 1
