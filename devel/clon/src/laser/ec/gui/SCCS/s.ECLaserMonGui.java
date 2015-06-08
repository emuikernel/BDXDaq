h61485
s 00001/00001/00523
d D 1.2 04/08/12 11:44:20 sergpozd 3 1
c camac3 --> camac1
e
s 00000/00000/00000
d R 1.2 01/03/10 17:24:23 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/gui/ECLaserMonGui.java
e
s 00524/00000/00000
d D 1.1 01/03/10 17:24:22 giovanet 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;

public class ECLaserMonGui extends JFrame {

    JPanel rocPan1 = new JPanel();
    JPanel rocPan2 = new JPanel();
    JButton[] rocBut = new JButton[6];

    JPanel progressPan = new JPanel();
    JTextArea progressTex = new JTextArea();

    JPanel laserPan1 = new JPanel();
    JPanel laserPan2 = new JPanel();
    JButton[] laserBut = new JButton[3];

    JPanel pulserPan1 = new JPanel();
    JPanel pulserPan2 = new JPanel();
    JButton[] pulserBut = new JButton[3];

    JPanel filterPan1 = new JPanel();
    JPanel filterPan2 = new JPanel();
    JButton filterBut1 = new JButton();
    JTextField filterTex = new JTextField();

    JPanel daqPan1 = new JPanel();
    JPanel daqPan2 = new JPanel();
    JPanel daqPan3 = new JPanel();
    JTextField[] daqTex = new JTextField[3];
    JLabel[] daqLab = new JLabel[3];

    JPanel sequencePan = new JPanel();
    JPanel sequencePan1 = new JPanel();
    JPanel sequencePan2 = new JPanel();
    JPanel sequencePan3 = new JPanel();
    JTextField sequenceTex = new JTextField();
    JLabel sequenceLab = new JLabel();

    JPanel currentSequencePan1 = new JPanel();
    JPanel currentSequencePan2 = new JPanel();
    JTextField currentSequenceTex = new JTextField();
    JLabel currentSequenceLab = new JLabel();
    JButton sequenceDoneBut = new JButton();

    JMenuBar menuBar;
    JMenu menu, submenu;
    JMenuItem menuItem; 
   
    Color buttonColor = Color.lightGray;
    Color labelColor = new Color(-10066279);
    Color valueColor = new Color(-10066279);

    Font labelFont = new Font("Serif", Font.BOLD, 10);
    Font buttonFont = new Font("Serif", Font.BOLD, 10);
    Font font12 = new Font("Serif", Font.BOLD, 12);
    Font font14 = new Font("Serif", Font.BOLD, 14);

    // Start main constructor
    //
    public ECLaserMonGui() {
	super("ECLaserMon"); 
	
	// Set ROC Status
	rocBut[0] = new JButton("clon10");
	rocBut[1] = new JButton("config_file");
	rocBut[2] = new JButton("coda_ready");
	rocBut[3] = new JButton("coda_config");
D 3
	rocBut[4] = new JButton("camac3");
E 3
I 3
	rocBut[4] = new JButton("camac1");
E 3
	rocBut[5] = new JButton("trig_bit");
	for (int i = 0; i < 6; i++) {
	    rocBut[i].setFont(buttonFont);
	    rocBut[i].setBackground(buttonColor);
	    rocBut[i].addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    rocDialog();
		}
	    });
	    rocPan2.add(rocBut[i]);
	}
 	rocPan1.add(rocPan2);

	// Set progress message
	JScrollPane progressScrollPane = new JScrollPane(progressTex);
	progressScrollPane.setVerticalScrollBarPolicy(
				 JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
        progressScrollPane.setPreferredSize(new Dimension(500, 125));
	progressTex.setLineWrap(true);
	progressTex.setWrapStyleWord(true);
	progressPan.add(progressScrollPane);

	// Set laser status
	laserPan2 = new JPanel();
	laserBut[0] = new JButton("Power");
	laserBut[1] = new JButton("Ready");
	laserBut[2] = new JButton("Enable");
	for (int j = 0; j < 3; j++) {
	    laserBut[j].setPreferredSize(new Dimension(80, 30));
	    laserBut[j].setBackground(buttonColor);
	    laserBut[j].setFont(buttonFont);
	    laserPan2.add(laserBut[j]);
	}
	laserBut[0].addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
		laserDialog1();
	    }
	});
	laserBut[1].addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
		laserDialog2();
	    }
	});
	laserBut[2].addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
		laserDialog3();
	    }
	});
	laserPan1.add(laserPan2);
	

	//Set Pulser status
	pulserPan2 = new JPanel();
	pulserBut[0] = new JButton("Ready");
	pulserBut[1] = new JButton("Jorway");
	pulserBut[2] = new JButton("Loaded");
	for (int j = 0; j < 3; j++) {
	    pulserBut[j].setPreferredSize(new Dimension(80, 30));
	    pulserBut[j].setBackground(buttonColor);
	    pulserBut[j].setFont(buttonFont);
	    pulserBut[j].addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    pulserDialog();
		}
		});
	    pulserPan2.add(pulserBut[j]);
	    pulserPan1.add(pulserPan2);
	}
	//Set Filter status
	filterBut1 = new JButton();
	filterBut1.setPreferredSize(new Dimension(80, 30));
	filterBut1.setBackground(buttonColor);
	filterBut1.setFont(buttonFont);
	filterBut1.setText("Ready");
	filterBut1.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent e) {
		filterDialog1();
	    }
	});

	filterTex = new JTextField("position");
	filterTex.setPreferredSize(new Dimension(80, 30));
	filterTex.setHorizontalAlignment(JTextField.CENTER);
	filterTex.setForeground(valueColor);
       	filterPan2 =  new JPanel();
	filterPan2.add(filterBut1);
	filterPan2.add(filterTex);
	filterPan1.add(filterPan2);
           
	// Set DAQ status
	daqTex[0] = new JTextField("run_number");
	daqTex[1] = new JTextField("config");
	daqTex[2] = new JTextField("run_status");
	daqLab[0] = new JLabel("Run #");
	daqLab[1] = new JLabel("Config");
	daqLab[2] = new JLabel("Status");
	for (int i = 0; i < 3; i++) { 
	    daqTex[i].setHorizontalAlignment(JTextField.CENTER);
	    daqTex[i].setFont(font12);
	    daqTex[i].setForeground(valueColor); 
	    daqTex[i].setPreferredSize(new Dimension(90, 40));
	    daqPan2.add(daqTex[i]);
	    daqLab[i].setPreferredSize(new Dimension(90, 20));
	    daqLab[i].setForeground(labelColor); 
	    daqLab[i].setFont(labelFont);
	    daqLab[i].setHorizontalAlignment(JLabel.CENTER);
	    daqPan3.add(daqLab[i]);
	}
	daqPan1.add(daqPan2);
	daqPan1.add(daqPan3); 
       
	// Set sequences
	sequenceTex = new JTextField("Seq_number");
	currentSequenceTex = new JTextField("Current_seq.");
	sequenceLab = new JLabel("Sequence #");
	currentSequenceLab = new JLabel("Current Seq.");
	sequenceTex.setHorizontalAlignment(JTextField.CENTER);
	sequenceTex.setFont(font12);
	sequenceTex.setForeground(valueColor); 
	sequenceTex.setPreferredSize(new Dimension(100, 40));
	sequencePan1.add(sequenceTex);
	currentSequenceTex.setHorizontalAlignment(JTextField.CENTER);
	currentSequenceTex.setFont(font12);
	currentSequenceTex.setForeground(valueColor); 
	currentSequenceTex.setPreferredSize(new Dimension(100, 40));
	currentSequencePan1.add(currentSequenceTex);
	
	sequenceLab.setPreferredSize(new Dimension(100, 20));
	sequenceLab.setForeground(labelColor); 
	sequenceLab.setFont(font12);
	sequenceLab.setHorizontalAlignment(JLabel.RIGHT);
	sequencePan2.add(sequenceLab);
	currentSequenceLab.setPreferredSize(new Dimension(100, 20));
	currentSequenceLab.setForeground(labelColor); 
	currentSequenceLab.setFont(font12);
	currentSequenceLab.setHorizontalAlignment(JLabel.RIGHT);
	currentSequencePan2.add(currentSequenceLab);
       
	sequenceDoneBut.setText("Done!!");
	sequenceDoneBut.setPreferredSize(new Dimension(200, 30));
	sequenceDoneBut.setBackground(buttonColor);
	sequenceDoneBut.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    sequenceDoneDialog();
		}
	    });
	sequencePan3.add(sequenceDoneBut);
	sequencePan.add(sequencePan1);
	sequencePan.add(sequencePan2);
	sequencePan.add(currentSequencePan1);
	sequencePan.add(currentSequencePan2);
	sequencePan.add(sequencePan3);



	// Set property of each panel.
	//	prePan.setLayout(new FlowLayout(FlowLayout.CENTER,12,10));
	rocPan1.setLayout(null);
	rocPan1.setBorder(BorderFactory.createTitledBorder(
			          BorderFactory.createRaisedBevelBorder(),
			          "ROC Status",
				  TitledBorder.CENTER,
				  TitledBorder.BELOW_TOP));
	rocPan1.setBounds(10, 10, 250, 110);
	rocPan2.setLayout(new GridLayout(0,2,5,5));
	rocPan2.setBounds(12, 35, 230, 62);	  

	progressPan.setLayout(new FlowLayout(FlowLayout.CENTER,10,10));
	progressPan.setBorder(BorderFactory.createTitledBorder(
				  BorderFactory.createRaisedBevelBorder(),
				  "Progress message",
				  TitledBorder.CENTER,
				  TitledBorder.BELOW_TOP));
	progressPan.setBounds(10, 130, 570, 175);

	laserPan1.setLayout(new FlowLayout());
	laserPan1.setBorder(BorderFactory.createTitledBorder(
			          BorderFactory.createRaisedBevelBorder(),
			          "Laser status",
				  TitledBorder.CENTER,
				  TitledBorder.BELOW_TOP));
	laserPan1.setBounds(10, 310, 300, 75);
	laserPan2.setLayout(new GridLayout(1,0,5,0));
	laserPan2.setBorder(BorderFactory.createEmptyBorder(5,0,0,0));
	   
	pulserPan1.setLayout(new FlowLayout());
	pulserPan1.setBorder(BorderFactory.createTitledBorder(
			          BorderFactory.createRaisedBevelBorder(),
			          "Pulser status",
				  TitledBorder.CENTER,
				  TitledBorder.BELOW_TOP));
	pulserPan1.setBounds(10, 385, 300, 75);
	pulserPan2.setLayout(new GridLayout(1,0,5,0));
	pulserPan2.setBorder(BorderFactory.createEmptyBorder(5,0,0,0));
	
	filterPan1.setLayout(new FlowLayout());
	filterPan1.setBorder(BorderFactory.createTitledBorder(
			          BorderFactory.createRaisedBevelBorder(),
			          "Filter status",
				  TitledBorder.CENTER,
				  TitledBorder.BELOW_TOP));
	filterPan1.setBounds(10, 460, 300, 75);
	filterPan2.setLayout(new GridLayout(1,0,5,0));
	filterPan2.setBorder(BorderFactory.createEmptyBorder(5,0,0,0));

	daqPan1.setLayout(null);
	daqPan1.setBorder(BorderFactory.createTitledBorder(
			          BorderFactory.createRaisedBevelBorder(),
			          "DAQ status",
				  TitledBorder.CENTER,
				  TitledBorder.BELOW_TOP));
	daqPan1.setBounds(260, 10, 320, 110);
	daqPan2.setLayout(new FlowLayout(FlowLayout.CENTER,5,10));
	daqPan2.setBounds(5, 30, 300,50);
	daqPan3.setLayout(new FlowLayout(FlowLayout.CENTER,5,10));
	daqPan3.setBounds(5, 75, 300,30);

	sequencePan.setLayout(null);
	sequencePan.setBorder(BorderFactory.createTitledBorder(
				  BorderFactory.createRaisedBevelBorder(),
				  "Sequence status",
				  TitledBorder.CENTER,
				  TitledBorder.BELOW_TOP));
	sequencePan.setBounds(320, 310, 260, 225);
	sequencePan1.setLayout(new FlowLayout(FlowLayout.CENTER,5,0));
	sequencePan1.setBounds(110, 50, 140,30);
	sequencePan2.setLayout(new FlowLayout(FlowLayout.CENTER,5,0));
	sequencePan2.setBounds(5, 54, 110,30);
	currentSequencePan1.setLayout(new FlowLayout(FlowLayout.CENTER,5,0));
	currentSequencePan1.setBounds(110, 100, 140,30);
	currentSequencePan2.setLayout(new FlowLayout(FlowLayout.CENTER,5,0));
	currentSequencePan2.setBounds(5, 104, 110,30);
	sequencePan3.setLayout(new FlowLayout(FlowLayout.CENTER,5,0));
	sequencePan3.setBounds(5, 160, 250,30);
	

	// Set container panel and add each panel to it.
	Container contentPane = getContentPane();
	contentPane.setLayout(null);
	contentPane.add(rocPan1);
	contentPane.add(laserPan1);
	contentPane.add(pulserPan1);
        contentPane.add(filterPan1);
	contentPane.add(daqPan1);
	contentPane.add(progressPan);
	contentPane.add(sequencePan);


	// Set Menu bar.
	menuBar = new JMenuBar();
        setJMenuBar(menuBar);
        menu = new JMenu("Tools"); 
        menuBar.add(menu);
	menu.setFont(new Font("Dialog", Font.BOLD, 10));
        menuItem = new JMenuItem("Recover");
	menuItem.setFont(new Font("Dialog", Font.BOLD, 10));
	menuItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
	       recover();
	    }
        });
        menu.add(menuItem);
	submenu = new JMenu("Expert");
	submenu.setFont(new Font("Dialog", Font.BOLD, 10));
	menuItem.setFont(new Font("Dialog", Font.BOLD, 10));
        menuItem = new JMenuItem("not yet");
	menuItem.setFont(new Font("Dialog", Font.BOLD, 10));
        submenu.add(menuItem);
        menuItem = new JMenuItem("not yet");
	menuItem.setFont(new Font("Dialog", Font.BOLD, 10));
        submenu.add(menuItem);
        menu.add(submenu);
	menu.addSeparator();
        menuItem = new JMenuItem("Exit");
	menuItem.setFont(new Font("Dialog", Font.BOLD, 10));
	menuItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
		quitDialog();
	    }
        });
        menu.add(menuItem);

	menuBar.add(Box.createHorizontalGlue());
	menu = new JMenu("Help"); 
	menuBar.add(menu);
	menuItem = new JMenuItem("About...");
	menuItem.setFont(new Font("Dialog", Font.BOLD, 10));
	menuItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
		aboutDialog();
	    }
        });
        menu.add(menuItem);
	
	addWindowListener(new WindowAdapter() {
	    public void windowClosing(WindowEvent e) {
		System.exit(0);	
	    }
	});
	pack();
	//	setVisible(true);
    }
    
    // Dialog
    private void quitDialog() {
	int n = JOptionPane.showConfirmDialog(
		          this, "Would you like to quit it?",
			  "Question Dialog",
			  JOptionPane.YES_NO_OPTION);
	if (n == JOptionPane.YES_OPTION) {
	    System.exit(0);	
	} 
    }
    private void aboutDialog() {
	JOptionPane.showMessageDialog(this,
				      "This was designed to monitor a status of \n"
				      +"the EC laser system when the EC laser \n"
				      +"is operated for automatic process.\n"
				      +"\n"
				      +"Made by Moohyoung Song(mhsong@jlab.org) \n"
				      +"Jan.24.2001 \n",
				      "Informational dialog",
				      JOptionPane.INFORMATION_MESSAGE);
    }
  
    private void rocDialog() {
	JOptionPane.showMessageDialog(this,
				      "This buttons give the status of the ROC.\n"
				      +"\n"
				      +"Color indicates: \n"
				      +"red  - busy or error\n"
				      +"green - O. K. \n",
				      "Informational dialog",
				      JOptionPane.INFORMATION_MESSAGE);
    }
    private void laserDialog1() {
	JOptionPane.showMessageDialog(this,
				      "This button gives the status of the AC power \n"
				      +"for the laser operating.\n"
				      +"\n"
				      +"Color indicates:\n"
				      +"orange - power off\n"
				      +"green  - power on\n",
				      "informational dialog",
				      JOptionPane.INFORMATION_MESSAGE);
    }
    private void laserDialog2() {
	JOptionPane.showMessageDialog(this,
				      "This button gives the status of the laser.\n"
				      +"\n"
				      +"Color indicates:\n"
				      +"orange - laser not ready\n"
				      +"green  - laser ready\n"
				      +"\n"
				      +"-> In order to operate the laser it is\n"
				      +"     needed to the AC power and suitable\n"
				      +"     pressure of gas.\n",
				      "informational dialog",
				      JOptionPane.INFORMATION_MESSAGE);
    }
    private void laserDialog3() {
	JOptionPane.showMessageDialog(this,
				      "This button gives the status of laser.\n"
				      +"\n"
				      +"Color indicates:\n"
				      +"orange - laser disabled\n"
				      +"green  - laser enabled\n"
				      +"\n"
				      +"-> It means the laser can emit the beam\n"
				      +"     whenever be triggered.\n",
				      "informational dialog",
				      JOptionPane.INFORMATION_MESSAGE);
    }
    private void pulserDialog() {
	JOptionPane.showMessageDialog(this,
				      "This button gives the status of pulser.\n\n"
				      +"Color means:\n"
				      +"orange - not yet\n"
				      +"green  - working\n",
				      "informational dialog",
				      JOptionPane.INFORMATION_MESSAGE);
    }
    private void filterDialog1() {
	JOptionPane.showMessageDialog(this,
				      "This button gives the status of communication\n"
				      +"for the filter.\n"
				      +"\n"
				      +"Color indicates:\n"
				      +"red - error or busy condition\n"
				      +"green - normal operation\n",
				      "informational dialog",
				      JOptionPane.INFORMATION_MESSAGE);
    }
    private void filterDialog2() {
	JOptionPane.showMessageDialog(this,
				      "This button gives the status of movement\n"
				      +"for the filter.\n"
				      +"\n"
				      +"Color indicates:\n"
				      +"orange - unknown status\n"
				      +"green - normal operation\n",
				      "informational dialog",
				      JOptionPane.INFORMATION_MESSAGE);
    }
    private void sequenceDoneDialog() {
	JOptionPane.showMessageDialog(this,
				      "This button gives the status of the sequence.\n"
				      +"\n"
				      +"Color indicates:\n"
				 +"green - all process for the calibration are done.\n",
				      "informational dialog",
				      JOptionPane.INFORMATION_MESSAGE);
    }


    // Re-initialize the Gui.
    public void recover() {
	for (int i = 0; i < 6; i++) {
	    rocBut[i].setBackground(buttonColor);
	}
	daqTex[0].setText("Unknown");
	daqTex[1].setText("Unknown");
	daqTex[2].setText("Unknown");
	for (int j = 0; j < 3; j++) {
	    laserBut[j].setBackground(buttonColor);
	}

	progressTex.setText(null);
	
	filterBut1.setBackground(buttonColor);
	filterTex = new JTextField("position");	

	sequenceTex.setText("Unknown");
       	currentSequenceTex.setText("Unknown");

	sequenceDoneBut.setBackground(buttonColor);

    } // recover	

    // Set the size of this GUI.
    public Dimension getPreferredSize() {
	return new  Dimension(600,600);
    }
    

    /*    public static void main(String [] args)
    {
	new ECLaserMonGui();
    }
    */


} // ECLaserMonGui
E 1
