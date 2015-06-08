/*----------------------------------------------------------------------------*
 *  Copyright (c) 2002        Southeastern Universities Research Association, *
 *                            Thomas Jefferson National Accelerator Facility  *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 *    Author:  Carl Timmer                                                    *
 *             timmer@jlab.org                   Jefferson Lab, MS-12H        *
 *             Phone: (757) 269-5130             12000 Jefferson Ave.         *
 *             Fax:   (757) 269-5800             Newport News, VA 23606       *
 *                                                                            *
 *----------------------------------------------------------------------------*/

package org.jlab.coda.etMonitor;

import java.lang.*;
import java.net.*;
import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import javax.swing.tree.*;
import javax.swing.border.*;
import javax.swing.event.*;

import org.xml.sax.SAXException;
import org.jlab.coda.et.*;

/**
 * This class implements a monitor of ET systems.
 *
 * @author Carl Timmer
 */

public class Monitor extends JFrame {
    // static variables
    private static MonitorConfiguration config;
    private static File configurationFile;

    // important widgets' names
    private final JTabbedPane tabbedPane;
    private final JFrame openFrame;
    private final JMenu disconnectMenu, loadConnectionParametersMenu;
    private JComboBox bAddress, mAddress, etName, hostname, cast;
    private WholeNumberField ttl, udpPort, mcastPort, tcpPort, period;
    private JButton connect;

    // other variables
    private String currentMonitorKey;
    private int defaultPeriod;

    // keep track of connections to & monitors of ET systems
    public final Map<String,SystemUse> connections =
            Collections.synchronizedMap(new HashMap<String,SystemUse>(20));
    public final Map<String,MonitorSingleSystem> monitors =
            Collections.synchronizedMap(new HashMap<String,MonitorSingleSystem>(20));

    // Default colors
    public final Color textColorDefault = Color.black;
    public final Color textBackgroundColorDefault = Color.white;
    public final Color titleColorDefault = Color.black;
    public final Color backgroundColorDefault = new Color(238, 220, 130); // lightGoldenrod2
    public final Color selectedTabColorDefault = Color.yellow;
    public final Color tabsBackgroundColorDefault = Color.cyan;
    // Colors used
    private Color textColor = textColorDefault;
    private Color textBackgroundColor = textBackgroundColorDefault;
    private Color titleColor = titleColorDefault;
    private Color backgroundColor = backgroundColorDefault;
    private Color selectedTabColor = selectedTabColorDefault;
    private Color tabsBackgroundColor = tabsBackgroundColorDefault;


    public Monitor() {
        this(null, null, null);
    }


    public Monitor(Color[] colors, Dimension frameSize, Point frameLocation) {
        super("ET System Monitor");

        // Set application colors.
        if (colors != null) {
            if (colors[0] != null) titleColor = colors[0];
            if (colors[1] != null) backgroundColor = colors[1];
            if (colors[2] != null) selectedTabColor = colors[2];
            if (colors[3] != null) tabsBackgroundColor = colors[3];
            if (colors[4] != null) textColor = colors[4];
            if (colors[5] != null) textBackgroundColor = colors[5];
        }
        // Set window location.
        if (frameLocation != null) {
            setLocation(frameLocation);
        }
        // Default data update period in seconds.
        defaultPeriod = 5;

        // To change some colors, the following is the only way to do it.
        UIManager.put("ComboBox.foreground", textColor);
        UIManager.put("ComboBox.background", textBackgroundColor);
        // UIManager.put("ComboBox.disabledForeground", Color.blue);
        // UIManager.put("ComboBox.selectionForeground", Color.cyan);
        // UIManager.put("ComboBox.selectionBackground", Color.magenta);

        // tabbedPane stuff
        UIManager.put("TabbedPane.selected", selectedTabColor);
        tabbedPane = new JTabbedPane();
        tabbedPane.setFont(MonitorFonts.buttonTabMenuFont);
        tabbedPane.setBackground(backgroundColor);
        tabbedPane.setForeground(titleColor);
        if (frameSize == null) {
            frameSize = new Dimension(1100, 700);
        }
        tabbedPane.setPreferredSize(frameSize);
        // Keep track of which ET system we're currently looking at.
        tabbedPane.addChangeListener(
                new ChangeListener() {
                    public void stateChanged(ChangeEvent e) {
                        JTabbedPane source = (JTabbedPane) e.getSource();
                        int tabIndex = source.getSelectedIndex();
                        // if help pane is showing, reset period & return
                        if (tabIndex < 1) {
                            period.setValue(defaultPeriod);
                            currentMonitorKey = "Help";
                            return;
                        }
                        currentMonitorKey = source.getTitleAt(tabIndex);
                        int updatePeriod = ((MonitorSingleSystem) (monitors.get(currentMonitorKey))).getUpdatePeriod();
                        period.setValue(updatePeriod);
                    }
                }
        );
        getContentPane().setBackground(tabsBackgroundColor);
        getContentPane().add(tabbedPane, BorderLayout.CENTER);

        // Final members need to be initialized in all constructors.
        openFrame = new JFrame("Open ET System");
        disconnectMenu = new JMenu("Disconnect");
        loadConnectionParametersMenu = new JMenu("Load Connection Parameters");

        // Make window used to input data needed to connect to an ET system.
        makeEtOpenWindow();
        // Define this window's menubar.
        makeMenubar();
        // Add to help screen to main window's tabbed pane
        tabbedPane.addTab("Help", null, makeHelpPane(), "help");
        currentMonitorKey = "Help";
    }

    // Change the update period of current single system monitor.
    private void setUpdatePeriod() {
        if (currentMonitorKey.equals("Help")) return;
        int updatePeriod = period.getValue();
        MonitorSingleSystem mon = (MonitorSingleSystem) monitors.get(currentMonitorKey);
        mon.setUpdatePeriod(updatePeriod);
        return;
    }

    //===================
    // Getters & Setters
    //===================

    // add ET file names to combo box
    public void addFileName(String name) {
        boolean nameIsThere = false;
        int count = etName.getItemCount();

        for (int i = 0; i < count; i++) {
            if (name.equals((String) etName.getItemAt(i))) {
                return;
            }
        }
        if (!nameIsThere) {
            etName.addItem(name);
        }
        return;
    }

    // add host names to combo box
    public boolean addHostname(String name) {
        if (name.equals(Constants.hostLocal) ||
                name.equals(Constants.hostRemote) ||
                name.equals(Constants.hostAnywhere)) {
            return false;
        }
        boolean nameIsThere = false;
        int count = hostname.getItemCount();
        for (int i = 0; i < count; i++) {
            if (name.equals((String) hostname.getItemAt(i))) {
                return true;
            }
        }
        if (!nameIsThere) {
            hostname.addItem(name);
        }
        return true;
    }

    // add addresses to combo boxes
    public void addBroadcastAddress(String addr) {
        bAddress.addItem(addr);
    }

    public void addMulticastAddress(String addr) {
        mAddress.addItem(addr);
    }

    //get ET names from combo box
    public String[] getFileNames() {
        int count = etName.getItemCount();
        if (count == 0) return null;
        String[] names = new String[count];
        for (int i = 0; i < count; i++) {
            names[i] = (String) etName.getItemAt(i);
        }
        return names;
    }

    //get host names from combo box
    public String[] getHostnames() {
        // Skip the first 3 items as they never change.
        int count = hostname.getItemCount() - 3;
        if (count < 1) return null;
        String[] names = new String[count];
        for (int i = 0; i < count; i++) {
            names[i] = (String) hostname.getItemAt(i + 3);
        }
        return names;
    }

    public int getMonitorWidth() {
        return tabbedPane.getWidth();
    }

    public int getMonitorHeight() {
        return tabbedPane.getHeight();
    }

    public Color getTextColor() {
        return new Color(textColor.getRGB());
    }

    public Color getTextBackgroundColor() {
        return new Color(textBackgroundColor.getRGB());
    }

    public Color getTitleColor() {
        return new Color(titleColor.getRGB());
    }

    public Color getBackgroundColor() {
        return new Color(backgroundColor.getRGB());
    }

    public Color getSelectedTabColor() {
        return new Color(selectedTabColor.getRGB());
    }

    public Color getTabsBackgroundColor() {
        return new Color(tabsBackgroundColor.getRGB());
    }

    private boolean isValidIpAddress(String addr) {
        StringTokenizer tok = new StringTokenizer(addr, ".");
        if (tok.countTokens() != 4) {
            return false;
        }

        int number;
        String num;
        try {
            while (tok.hasMoreTokens()) {
                num = tok.nextToken();
                number = Integer.parseInt(num);
                if (number < 0 || number > 255) {
                    return false;
                }
                if (num.charAt(0) == '0' && (number != 0 || num.length() > 1)) {
                    return false;
                }
            }
        }
        catch (NumberFormatException ex) {
            return false;
        }
        return true;
    }

    private boolean isValidMulticastAddress(String addr) {
        InetAddress address = null;
        try {
            address = InetAddress.getByName(addr);
        }
        catch (UnknownHostException e) {
            return false;
        }

        return (address.isMulticastAddress());

        /*
        StringTokenizer tok = new StringTokenizer(addr, ".");
        if (tok.countTokens() != 4) {
            return false;
        }

        int number, round = 1;
        String num;
        try {
            while (tok.hasMoreTokens()) {
                num = tok.nextToken();
                number = Integer.parseInt(num);
                if ((round++ == 1) && (number < 224 || number > 239)) {
                    return false;
                }
                if (number < 0 || number > 255) {
                    return false;
                }
                if (num.charAt(0) == '0' && (number != 0 || num.length() > 1)) {
                    return false;
                }
            }
        }
        catch (NumberFormatException ex) {
            return false;
        }
        return true;
        */
    }

    public static void main(String[] args) {
        try {
            Monitor frame = null;

            // allow for a configuration file argument
            if (args.length > 0) {
                if (args.length != 2) {
                    System.out.println("Usage: java Monitor [-f,-file <configFile>]");
                    return;
                }
                if (!(args[0].equalsIgnoreCase("-f") || args[0].equalsIgnoreCase("-file"))) {
                    System.out.println("Usage: java Monitor [-f,-file <configFile>]");
                    return;
                }
                configurationFile = new File(args[1]);

                // Read config file once to get main application window &
                // color data only.
                // This is done because the frame needs to have the colors,
                // size, and position BEFORE it displays anything.
                config = new MonitorConfiguration(null);
                config.loadWindowParameters(configurationFile);
                Color[] colors = config.getWindowColors();
                Dimension size = config.getWindowSize();
                Point location = config.getWindowLocation();
                frame = new Monitor(colors, size, location);
                // Read config file again to get the rest of the data.
                // This needs the application to have already started
                // (as in the previous line) - the reason being that
                // connections to ET systems need to be made etc.
                config.setMonitor(frame);
                config.load(configurationFile);
            }
            else {
                frame = new Monitor();
                config = new MonitorConfiguration(frame);
            }
            frame.addWindowListener(new WindowAdapter() {
                public void windowClosing(WindowEvent e) {
                    System.exit(0);
                }
            });

            frame.pack();
            frame.setVisible(true);

            MonitorSingleSystem mon = null;
            DefaultMutableTreeNode monNode = null;

            // Class designed to run graphics commands in the Swing thread.
            class Updater extends Thread {
                MonitorSingleSystem mon;  // single system monitor of interest

                public void setMonitor(MonitorSingleSystem m) {
                    mon = m;
                }

                public Updater(MonitorSingleSystem m) {
                    mon = m;
                }

                public void run() {
                    if (mon.isInitialized()) {
                        mon.updateDisplay();
                        mon.treeDidChange();
                    }
                    else {
                        mon.staticDisplay();
                        mon.updateDisplay();
                        mon.updateUI();
                    }
                }
            }

            Updater updater = new Updater(mon);

            while (true) {
                // While we're in the iterator, we CANNOT have monitors added
                // (and thereby change the structure of the HashMap).
                synchronized (frame.monitors) {
                    for (Iterator i = frame.monitors.entrySet().iterator(); i.hasNext();) {
                        // get monitor object
                        mon = (MonitorSingleSystem) (((Map.Entry) i.next()).getValue());
                        monNode = mon.getNode();

                        try {
                            // only update if enough time has elapsed
                            if (mon.timeToUpdate()) {
                                // get data
                                mon.getData();
                                updater.setMonitor(mon);
                                // display new data
                                SwingUtilities.invokeLater(updater);
                            }
                        }
                        catch (EtException ex) {
                            //System.out.print("\n*****************************************\n");
                            //System.out.print("*   Error getting data from ET system   *");
                            //System.out.print("\n*****************************************\n");
                            //ex.printStackTrace();
                        }
                        catch (Exception ex) {
                            //System.out.print("\n*****************************************\n");
                            //System.out.print("* I/O error getting data from ET system *");
                            //System.out.print("\n*****************************************\n");
                            String key = mon.getKey();
                            // Remove connection with the IO problem.
                            frame.removeConnection(frame, mon, key, false);
                            // Remove single system monitor from hash table.
                            i.remove();
                            // Remove SystemUse object from hash table.
                            frame.connections.remove(key);

                            //ex.printStackTrace();
                        }
                    }
                }
                Thread.sleep(500);
            }
        }

        catch (Exception ex) {
            System.out.println("Unrecoverable error in ET monitor:");
            ex.printStackTrace();
        }

    }


    private void makeMenubar() {

        JMenuBar menuBar = new JMenuBar();
        menuBar.setBackground(backgroundColor);
        setJMenuBar(menuBar);

        // file menu
        JMenu fileMenu = new JMenu("File");
        fileMenu.setFont(MonitorFonts.buttonTabMenuFont);
        fileMenu.setBackground(backgroundColor);
        fileMenu.setForeground(titleColor);
        menuBar.add(fileMenu);

        // Create a file chooser
        final JFileChooser fc = new JFileChooser(System.getProperty("user.dir"));

        // file menu item to save configuration
        JMenuItem menuItem = new JMenuItem("Save Configuration");
        menuItem.setFont(MonitorFonts.buttonTabMenuFont);
        menuItem.setBackground(backgroundColor);
        menuItem.setForeground(titleColor);
        fileMenu.add(menuItem);
        menuItem.addActionListener(
                new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        File file;
                        if (configurationFile == null) {
                            if (fc.showSaveDialog(Monitor.this) == JFileChooser.CANCEL_OPTION) {
                                return;
                            }
                            file = fc.getSelectedFile();
                        }
                        else {
                            file = configurationFile;
                        }

                        try {
                            config.save(file);
                        }
                        catch (IOException ex) {
                            JOptionPane.showMessageDialog(new JFrame(),
                                                          "Cannot write to file \"" + file.getName() + "\"",
                                                          "Error",
                                                          JOptionPane.ERROR_MESSAGE);
                            return;
                        }
                        configurationFile = file;

                    }
                }
        );

        // file menu item to save configuration
        menuItem = new JMenuItem("Save Configuration As");
        menuItem.setFont(MonitorFonts.buttonTabMenuFont);
        menuItem.setBackground(backgroundColor);
        menuItem.setForeground(titleColor);
        fileMenu.add(menuItem);
        menuItem.addActionListener(
                new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        int returnVal = fc.showSaveDialog(Monitor.this);

                        if (returnVal == JFileChooser.APPROVE_OPTION) {
                            File file = fc.getSelectedFile();
                            try {
                                if (file.exists()) {
                                    int n = JOptionPane.showConfirmDialog(
                                            new JFrame(),
                                            "Overwrite existing file?",
                                            "WARNING",
                                            JOptionPane.YES_NO_OPTION);
                                    if (n == JOptionPane.NO_OPTION) return;
                                }
                                config.save(file);
                            }
                            catch (IOException ex) {
                                JOptionPane.showMessageDialog(new JFrame(),
                                                              "Cannot write to file \"" + file.getName() + "\"",
                                                              "Error",
                                                              JOptionPane.ERROR_MESSAGE);
                                return;
                            }
                            configurationFile = file;
                        }
                    }
                }
        );

        // file menu item to load configuration
        menuItem = new JMenuItem("Load Configuration");
        menuItem.setFont(MonitorFonts.buttonTabMenuFont);
        menuItem.setBackground(backgroundColor);
        menuItem.setForeground(titleColor);
        fileMenu.add(menuItem);
        menuItem.addActionListener(
                new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        int returnVal = fc.showDialog(Monitor.this, "Load");

                        if (returnVal == JFileChooser.APPROVE_OPTION) {
                            File file = fc.getSelectedFile();
                            try {
                                config.load(file);
                            }
                            catch (SAXException ex) {
                                JOptionPane.showMessageDialog(new JFrame(),
                                                              "Cannot load file \"" + file.getName() + "\"",
                                                              "Error",
                                                              JOptionPane.ERROR_MESSAGE);
                                return;
                            }
                            catch (IOException ex) {
                                JOptionPane.showMessageDialog(new JFrame(),
                                                              "Cannot load file \"" + file.getName() + "\"",
                                                              "Error",
                                                              JOptionPane.ERROR_MESSAGE);
                                return;
                            }
                        }
                    }
                }
        );

        // File menu item to quit.
        menuItem = new JMenuItem("Quit");
        menuItem.setFont(MonitorFonts.buttonTabMenuFont);
        menuItem.setBackground(backgroundColor);
        menuItem.setForeground(titleColor);
        fileMenu.add(menuItem);
        menuItem.addActionListener(
                new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        System.exit(0);
                    }
                }
        );

        // View menu to change update period of monitored ET system.
        JMenu viewMenu = new JMenu("View");
        viewMenu.setFont(MonitorFonts.buttonTabMenuFont);
        viewMenu.setBackground(backgroundColor);
        viewMenu.setForeground(titleColor);
        menuBar.add(viewMenu);

        period = new WholeNumberField(defaultPeriod, 5, 1, Integer.MAX_VALUE);
        period.setFont(MonitorFonts.inputFont);
        period.setAlignmentX(Component.LEFT_ALIGNMENT);
        period.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                WholeNumberField source = (WholeNumberField) e.getSource();
                source.correctValue();
                setUpdatePeriod();
            }
        }
        );
        period.addMouseListener(new MouseAdapter() {
            public void mouseExited(MouseEvent e) {
                WholeNumberField source = (WholeNumberField) e.getSource();
                source.correctValue();
                setUpdatePeriod();
            }
        }
        );

        JMenu updatePeriod = new JMenu("Update Period (sec)");
        updatePeriod.setFont(MonitorFonts.buttonTabMenuFont);
        updatePeriod.setBackground(backgroundColor);
        updatePeriod.setForeground(titleColor);
        updatePeriod.add(period);
        viewMenu.add(updatePeriod);

        // menu to load connection parameters from a specific, existing connection
        loadConnectionParametersMenu.setFont(MonitorFonts.buttonTabMenuFont);
        loadConnectionParametersMenu.setBackground(backgroundColor);
        loadConnectionParametersMenu.setForeground(titleColor);
        viewMenu.add(loadConnectionParametersMenu);

        // menuitem to switch JSplitPane orientation
        menuItem = new JMenuItem("Change Orientation");
        menuItem.setFont(MonitorFonts.buttonTabMenuFont);
        menuItem.setBackground(backgroundColor);
        menuItem.setForeground(titleColor);
        viewMenu.add(menuItem);
        menuItem.addActionListener(
                new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        if (currentMonitorKey.equals("Help")) return;
                        MonitorSingleSystem mon = (MonitorSingleSystem) monitors.get(currentMonitorKey);
                        int orient = mon.getOrientation();
                        if (orient == JSplitPane.HORIZONTAL_SPLIT) {
                            mon.setOrientation(JSplitPane.VERTICAL_SPLIT);
                        }
                        else {
                            mon.setOrientation(JSplitPane.HORIZONTAL_SPLIT);
                        }
                    }
                }
        );

        // connect menu
        JMenu connectMenu = new JMenu("Connections");
        connectMenu.setFont(MonitorFonts.buttonTabMenuFont);
        connectMenu.setBackground(backgroundColor);
        connectMenu.setForeground(titleColor);
        menuBar.add(connectMenu);

        menuItem = new JMenuItem("Connect to ET System");
        menuItem.setFont(MonitorFonts.buttonTabMenuFont);
        menuItem.setBackground(backgroundColor);
        menuItem.setForeground(titleColor);
        connectMenu.add(menuItem);
        menuItem.addActionListener(
                new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        openFrame.setVisible(true);
                        openFrame.setState(Frame.NORMAL);
                    }
                }
        );

        // menu to disconnect existing connections
        disconnectMenu.setFont(MonitorFonts.buttonTabMenuFont);
        disconnectMenu.setBackground(backgroundColor);
        disconnectMenu.setForeground(titleColor);
        connectMenu.add(disconnectMenu);
    }


    private JScrollPane makeHelpPane() {
        // Put this into the tabbedPane.
        JTextArea text = new JTextArea(10, 200);
        text.setFont(MonitorFonts.helpFont);
        text.setLineWrap(true);
        text.setWrapStyleWord(true);
        text.setTabSize(3);
        text.setEditable(false);
        text.setBorder(new EmptyBorder(20, 20, 20, 20));
        JScrollPane pane = new JScrollPane(text);

        // Put stuff into the text area.
        text.append(
                "CONNECTING TO AN ET SYSTEM\n" +

                        "Select the \"Connect to ET System\" option of the \"Connections\" menu. " +
                        "There are a number of options on the appearing window which must be set.\n\n" +

                        "1) ET Name\nThis is the name of the ET system (actually, its file) that you " +
                        "want to connect to. The names of several ET systems can be stored in its " +
                        "list. The \"X\" button is for removing unwanted entries.\n\n" +

                        "2) ET Location\nTo look for the named ET system on the local computer or host, " +
                        "select \"local\". To look only on another computer, select \"remote\", " +
                        "or \"anywhere\" if you don't care where the ET system is. If you know the " +
                        "name of the computer, this is the place to type it in.\n\n" +

                        "3) Find ET by\n" +
                        "There are several ways to connect to an ET system. The following list showing " +
                        "the available choices:\n\n" +

                        "\ta) broadcasting\nThis selection is generally chosen when the name of the host " +
                        "that the ET system is residing on is unknown or if the user wants to write a very " +
                        "general application with no hostnames \"hardcoded\" or input in some fashion. " +
                        "(If a specific hostname is used, a UDP packet is sent directly to that host in " +
                        "addition to a broadcast being made.)\n" +
                        "A UDP broadcast is made on all the subnet broadcast addresses listed in the " +
                        "\"Subnet Addresses\" entry. Items can be removed with the \"X\" button. " +
                        "This broadcast is sent to the port found in the \"UDP Port\" entry. " +
                        "Once an ET system receives the broadcast, it responds by sending its host name " +
                        "and the tcp port on which it is listening. This information is used " +
                        "to establish a permanent tcp connection.\n\n" +

                        "\tb) multicasting\nAs in broadcasting, this selection is generally chosen when " +
                        "the name of the host that the ET system is residing on is unknown or if the user " +
                        "wants to write a very general application with no hostnames \"hardcoded\" or input " +
                        "in some fashion. (If a specific hostname is used, a UDP packet is sent directly to " +
                        "that host on the port in the \"UDP Port\" entry in addition to a multicast " +
                        "being made.)\n" +
                        "A UDP multicast is made on all the multicast addresses " +
                        "listed in the \"Multicast Addresses\" entry. Items can be removed with the \"X\" " +
                        "button. This multicast is sent to the port found in the \"Multicast Port\" " +
                        "entry, and its \"ttl\" value can be set as well. (It defaults to \"1\" which " +
                        "should limit its scope to the local subnets.) " +
                        "Once an ET system receives the multicast, it responds by sending its host name " +
                        "and the tcp port on which it is listening. This information is used " +
                        "to establish a permanent tcp connection.\n\n" +

                        "\tc) broad & multicasting\nThis selection can simultaneously UDP broadcast " +
                        "and UDP multicast.\n\n" +

                        "\td) direct connection\nA direct, permanent tcp connection is made between the ET " +
                        "system and the user. In this case, a specific hostname must be used (not \"local\", " +
                        "\"remote\", or \"anywhere\"). The \"TCP Port\" entry is used for the port number.\n\n" +

                        "RESETTING CONNECTION PARAMETERS\n" +
                        "Reseting all connection parameters to those previously used to make an actual " +
                        "connection can be done by selecting the \"Load connection parameters\" item from " +
                        "the \"View\" menu. Simply select from the list of existing connections.\n\n\n\n" +

                        "VIEWING AN ET SYSTEM\n" +

                        "After connecting to an ET system, a tab appears with the ET system's name on it. " +
                        "By selecting this tab, the user can see all the system parameters in text form on " +
                        "the left side of the window and a visual representation on the right side. Not all " +
                        "text information is relevant for all systems. For example, the ET systems written in " +
                        "Java do not have process or mutex information available. Text information " +
                        "is divided into sections with a short explanation of each following:\n\n" +

                        "1) System - general ET system parameters\n" +
                        "\ta) Static Info - information that does NOT change\n" +
                        "\t\tHost - host system is running on, language code was written in, and unix pid\n" +
                        "\t\tPorts - the tcp, udp, and multicast port numbers\n" +
                        "\t\tEvents - total # of events, size of each, # of temporary (extra large) events\n" +
                        "\t\tMax - maximum number of stations, attachments, and processes allowed\n" +
                        "\t\tNetwork interfaces - list of host's network interfaces\n" +
                        "\t\tMulticast addreses - list of multicast addresses the system is listening on\n\n" +

                        "\tb) Dynamic Info - information that can or will change in time\n" +
                        "\t\tEvents rate - rate of events leaving GRAND_CENTRAL station\n" +
                        "\t\tEvents owned by - number of events owned by each attachment & system.\n" +
                        "\t\tIdle stations - list of stations with no attachments (receive no events)\n" +
                        "\t\tAll stations - list of all stations in proper order\n" +
                        "\t\tStations - current number of stations, attachments, and temporary events\n" +
                        "\t\tProcesses - # of non-system, unix processes with access to shared memory (Solaris)\n" +
                        "\t\tHeartbeat - value of non-Java system's counter in shared memory (changes if alive)\n" +
                        "\t\tLocked Mutexes - on non-Java systems, locked pthread mutexes.\n\n" +

                        "2) Stations - stations are listed by name under this heading\n" +
                        "\ta) Configuration - parameters which define station behavior\n" +
                        "\t\t- active or idle,  blocking or nonblocking,  prescale & cue values\n" +
                        "\t\t- single user, multiple users, or the exact number of allowed users\n" +
                        "\t\t- events restored to station's input, output, or to GRAND_CENTRAL station\n" +
                        "\t\t- select all events, those matching default condition, or matching user condition\n" +
                        "\t\t- values of integers in selection array\n" +
                        "\t\t- class or library & function of user's matching condition\n\n" +

                        "\tb) Statistics - \n" +
                        "\t\t- total number of attachments and their id numbers\n" +
                        "\t\t- current # of events in input list, total # put in input, # tried to put in input\n" +
                        "\t\t- current # of events in output list, total # put in output list\n\n" +

                        "3) Processes - on Solaris, local unix processes with attachments are listed by id #\n" +
                        "\t- total # of attachments, list of attachments' ids, unix pid, current heartbeat value\n\n" +

                        "4) Attachments - attachments are listed by their id numbers\n" +
                        "\t- name of station attached to, host attachment is running on\n" +
                        "\t- is attachment blocked waiting to read events?\n" +
                        "\t- has attachment been told to quit reading events and return?\n" +
                        "\t- unix pid and process id (non-Java)\n" +
                        "\t- # events currently owned, total #: newly made, gotten, put, and dumped\n\n\n\n" +

                        "SETTING AN UPDATE PERIOD\n" +
                        "Each ET system has its information updated at a regular period which can be set " +
                        "by selecting the \"View\" menu item and typing in the period value.\n\n\n\n" +

                        "DISCONNECTING AN ET SYSTEM\n" +
                        "Each ET system can be removed from the monitor by selecting the \"Connections\" " +
                        "menu item followed by selecting the \"Disconnect\" item, and then selecting the " +
                        "system to be removed.\n\n\n\n" +

                        "CONFIGURATION FILES\n" +
                        "Configuration files can be created, saved, and loaded through the \"File\" menu item. " +
                        "The configuration files are in XML format and use a schema defined in " +
                        "the file \"monitorConfiguration.xsd\". Without going into great detail, configuration " +
                        "files store all current connections and the current state of the application. Colors " +
                        "used in this application, as well as the " +
                        "main window's size and placement, can be set in the configuration file. These particular " +
                        "parameters, however, will only be set in the application if the configuration file is " +
                        "given on the command line (-f or -file). Once the monitor is up and running, loading a " +
                        "configuration file simply adds any additional ET system connections listed there as well " +
                        "as adding items to the \"ET Name\" or \"ET Location\" lists.\n\n" +

                        "Setting colors in a configuration file can only be done by hand-editing it. Modifying the colors " +
                        "in the main application can be done by inserting the following lines (without the explanation) " +
                        "in any order, under the \"<graphics>\" element. Simply change the red, green, and blue " +
                        "values (between 0 and 255 inclusive) to suit:\n\n" +

                        "\tText on menus, titles, buttons, tabs (default black):\n" +
                        "\t\t<titleColor  red=\"0\" green=\"0\" blue=\"0\"/>\n\n" +
                        "\tGeneral background: (default lightGoldenrod2)\n" +
                        "\t\t<backgroundColor  red=\"238\" green=\"220\" blue=\"130\"/>\n\n" +
                        "\tSelected tab background (default yellow):\n" +
                        "\t\t<selectedTabColor  red=\"255\" green=\"255\" blue=\"0\"/>\n\n" +
                        "\tBehind all tabs background (default cyan):\n" +
                        "\t\t<tabsBackgroundColor  red=\"150\" green=\"255\" blue=\"255\"/>\n\n" +
                        "\tText in entry widgets (default black):\n" +
                        "\t\t<textColor  red=\"0\" green=\"0\" blue=\"0\"/>\n\n" +
                        "\tText entry widget background (default white):\n" +
                        "\t\t<textBackgroundColor  red=\"255\" green=\"255\" blue=\"255\"/>\n\n" +

                        "Colors may also be changed in the view of a monitored ET system. To do so, the following " +
                        "lines may be added. HOWEVER, THEY MAY ONLY BE ADDED AT THE END OF EACH \"<etConnection>\" " +
                        "ELEMENT AND ONLY IN THE GIVEN ORDER:\n\n" +

                        "\tEvents (default red):\n" +
                        "\t\t<eventColor  red=\"255\" green=\"0\" blue=\"0\"/>\n\n" +
                        "\tStations Active (default cyan):\n" +
                        "\t\t<stationColor  red=\"0\" green=\"255\" blue=\"255\"/>\n\n" +
                        "\tStations Idle (default pink):\n" +
                        "\t\t<stationIdleColor  red=\"255\" green=\"192\" blue=\"203\"/>\n\n" +
                        "\tAttachments (default magenta):\n" +
                        "\t\t<attachmentColor  red=\"255\" green=\"0\" blue=\"255\"/>\n\n" +
                        "\tLines between stations and attachments (default black):\n" +
                        "\t\t<lineColor  red=\"0\" green=\"0\" blue=\"0\"/>\n\n" +
                        "\tText in stations and attachments (default black):\n" +
                        "\t\t<textColor  red=\"0\" green=\"0\" blue=\"0\"/>\n\n" +
                        "\tStation and attachment text background (default white):\n" +
                        "\t\t<textBackgroundColor  red=\"255\" green=\"255\" blue=\"255\"/>\n\n" +
                        "\tGraph background (default white):\n" +
                        "\t\t<backgroundColor  red=\"255\" green=\"255\" blue=\"255\"/>\n\n" +
                        "\tText of tree widget (default black):\n" +
                        "\t\t<treeTextColor  red=\"0\" green=\"0\" blue=\"0\"/>\n\n" +
                        "\tTree widget background (default white):\n" +
                        "\t\t<treeBackgroundColor  red=\"255\" green=\"255\" blue=\"255\"/>"

        );
        return pane;
    }


    private void makeEtOpenWindow() {
        // widget sizes & spacings
        int edge1 = 20,
                edge2 = 10,
                edge3 = 5,
                prefWidth = 500,
                maxWidth = 800,
                indent = 15,
                horSpace = 10,
                verSpace = 10,
                prefRemBut = 50,
                maxRemBut = 70,
                prefHeight1 = 50,
                maxHeight = 60,
                prefHeight2 = 40;
        // convenient sizes
        int prefHalf = prefWidth / 2 - edge2 - horSpace / 2,
                maxHalf = maxWidth / 2 - edge2 - horSpace / 2;

        // Several combo boxes use this to filter input.
        ActionListener al = new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                JComboBox jcb = (JComboBox) e.getSource();
                String listItem;
                String selectedItem = (String) jcb.getSelectedItem();
                int numItems = jcb.getItemCount();
                boolean addNewItem = true;

                if (selectedItem == null || selectedItem.equals("")) {
                    addNewItem = false;
                }
                else if (numItems == 0) {
                    addNewItem = true;
                }
                else {
                    for (int i = 0; i < numItems; i++) {
                        listItem = (String) jcb.getItemAt(i);
                        if (listItem.equals(selectedItem) == true) {
                            addNewItem = false;
                            break;
                        }
                    }
                }

                if (addNewItem) {
                    jcb.addItem(selectedItem);
                }
            }
        };

        // put main panel into one main window
        JPanel openPanel = new JPanel();
        openPanel.setBackground(backgroundColor);
        openPanel.setLayout(new BoxLayout(openPanel, BoxLayout.Y_AXIS));
        openPanel.setBorder(new EmptyBorder(edge1, edge1, edge1, edge1));

        // setting ET name
        TitledBorder border1 = new TitledBorder(new EmptyBorder(0, 0, 0, 0),
                                                "ET Name",
                                                TitledBorder.LEFT,
                                                TitledBorder.ABOVE_TOP,
                                                MonitorFonts.titleFont,
                                                titleColor);

        JPanel p1 = new JPanel();
        p1.setLayout(new BoxLayout(p1, BoxLayout.X_AXIS));
        p1.setBorder(border1);
        p1.setBackground(backgroundColor);
        p1.setPreferredSize(new Dimension(prefWidth, prefHeight1 + edge3));
        p1.setMaximumSize(new Dimension(maxWidth, maxHeight + edge3));
        p1.setAlignmentX(Component.LEFT_ALIGNMENT);

        etName = new JComboBox();
        etName.setBackground(textBackgroundColor);
        etName.setEditable(true);
        etName.setFont(MonitorFonts.inputFont);
        etName.setAlignmentX(Component.CENTER_ALIGNMENT);
        etName.setPreferredSize(new Dimension(prefWidth - indent, prefHeight1));
        etName.setMaximumSize(new Dimension(maxWidth - indent, maxHeight));
        etName.addActionListener(al);
        // Set editable comboBox colors
        Component c = etName.getEditor().getEditorComponent();
        c.setBackground(textBackgroundColor);
        c.setForeground(textColor);

        // button for ET name removal
        final JButton removeName = new JButton("X");
        removeName.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                int index = etName.getSelectedIndex();
                if (index > -1) {
                    etName.removeItemAt(index);
                }
            }
        });
        removeName.setAlignmentX(Component.LEFT_ALIGNMENT);
        removeName.setForeground(titleColor);
        removeName.setBackground(backgroundColor);
        removeName.setPreferredSize(new Dimension(prefRemBut, prefHeight2));
        removeName.setMaximumSize(new Dimension(maxRemBut, maxHeight));

        p1.add(Box.createRigidArea(new Dimension(indent, 0)));
        p1.add(etName);
        p1.add(removeName);

        // setting ET location
        TitledBorder border2 = new TitledBorder(new EmptyBorder(0, 0, 0, 0),
                                                "ET Location",
                                                TitledBorder.LEFT,
                                                TitledBorder.ABOVE_TOP,
                                                MonitorFonts.titleFont,
                                                titleColor);

        JPanel p2 = new JPanel();
        p2.setLayout(new BoxLayout(p2, BoxLayout.X_AXIS));
        p2.setBorder(border2);
        p2.setBackground(backgroundColor);
        p2.setPreferredSize(new Dimension(prefWidth, prefHeight1 + edge3));
        p2.setMaximumSize(new Dimension(maxWidth, maxHeight + edge3));
        p2.setAlignmentX(Component.LEFT_ALIGNMENT);

        hostname = new JComboBox(new String[]{"local", "remote", "anywhere"});
        hostname.setEditable(true);
        hostname.setBackground(textBackgroundColor);
        hostname.setFont(MonitorFonts.inputFont);
        hostname.setPreferredSize(new Dimension(prefWidth - indent, prefHeight1));
        hostname.setMaximumSize(new Dimension(maxWidth - indent, maxHeight));
        hostname.addActionListener(al);
        // Set editable comboBox colors
        c = hostname.getEditor().getEditorComponent();
        c.setBackground(textBackgroundColor);
        c.setForeground(textColor);

        // button for ET name removal
        final JButton removeHost = new JButton("X");
        removeHost.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                int index = hostname.getSelectedIndex();
                if (index > 2) {
                    hostname.removeItemAt(index);
                }
            }
        });
        removeHost.setAlignmentX(Component.LEFT_ALIGNMENT);
        removeHost.setForeground(titleColor);
        removeHost.setBackground(backgroundColor);
        removeHost.setPreferredSize(new Dimension(prefRemBut, prefHeight2));
        removeHost.setMaximumSize(new Dimension(maxRemBut, maxHeight));

        p2.add(Box.createRigidArea(new Dimension(indent, 0)));
        p2.add(hostname);
        p2.add(removeHost);

        // panel for ports, ttl, & addresses
        JPanel p3 = new JPanel();
        p3.setLayout(new GridLayout(2, 2, horSpace, 0));
        p3.setBackground(backgroundColor);
        p3.setPreferredSize(new Dimension(prefWidth, prefHeight2 * 2));
        p3.setMaximumSize(new Dimension(maxWidth, maxHeight * 2));
        p3.setBorder(new EmptyBorder(edge2, edge2, edge2, edge2));
        p3.setAlignmentX(Component.LEFT_ALIGNMENT);

        // label for broadcast addresses
        JLabel l4 = new JLabel("Subnet Addresses", JLabel.LEFT);
        l4.setVerticalAlignment(JLabel.BOTTOM);
        l4.setFont(MonitorFonts.titleFont);
        l4.setForeground(titleColor);
        l4.setOpaque(true);
        l4.setBackground(backgroundColor);
        l4.setAlignmentX(Component.LEFT_ALIGNMENT);

        // panel for broadcast combo box & button
        JPanel p4 = new JPanel();
        p4.setLayout(new BoxLayout(p4, BoxLayout.X_AXIS));
        p4.setBackground(backgroundColor);
        p4.setPreferredSize(new Dimension(prefHalf, prefHeight2));
        p4.setMaximumSize(new Dimension(maxHalf, maxHeight));
        p4.setAlignmentX(Component.LEFT_ALIGNMENT);

        // comboBox for broadcast address
        bAddress = new JComboBox();
        bAddress.addItem("129.57.35.255");
        bAddress.setEditable(true);
        bAddress.setBackground(textBackgroundColor);
        bAddress.setFont(MonitorFonts.inputFont);
        bAddress.setAlignmentX(Component.LEFT_ALIGNMENT);
        bAddress.setPreferredSize(new Dimension(prefHalf - prefRemBut, prefHeight2));
        bAddress.setMaximumSize(new Dimension(maxHalf - maxRemBut, maxHeight));
        bAddress.addActionListener(al);
        // Set editable comboBox colors
        c = bAddress.getEditor().getEditorComponent();
        c.setBackground(textBackgroundColor);
        c.setForeground(textColor);

        // button for multicast address removal
        final JButton remove1 = new JButton("X");
        remove1.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                int index = bAddress.getSelectedIndex();
                if (index > -1) {
                    bAddress.removeItemAt(index);
                }
            }
        });
        remove1.setAlignmentX(Component.LEFT_ALIGNMENT);
        remove1.setForeground(titleColor);
        remove1.setBackground(backgroundColor);
        remove1.setPreferredSize(new Dimension(prefRemBut, prefHeight2));
        remove1.setMaximumSize(new Dimension(maxRemBut, maxHeight));

        p4.add(bAddress);
        p4.add(remove1);

        // label for multicast addresses
        JLabel l5 = new JLabel("Multicast Addresses", JLabel.LEFT);
        l5.setVerticalAlignment(JLabel.BOTTOM);
        l5.setFont(MonitorFonts.titleFont);
        l5.setForeground(titleColor);
        l5.setOpaque(true);
        l5.setBackground(backgroundColor);
        l5.setAlignmentX(Component.LEFT_ALIGNMENT);

        // panel for multicast combo box & button
        JPanel p5 = new JPanel();
        p5.setLayout(new BoxLayout(p5, BoxLayout.X_AXIS));
        p5.setBackground(backgroundColor);
        p5.setPreferredSize(new Dimension(prefHalf, prefHeight2));
        p5.setMaximumSize(new Dimension(maxHalf, maxHeight));
        p5.setAlignmentX(Component.LEFT_ALIGNMENT);

        // comboBox for multicast address
        mAddress = new JComboBox();
        mAddress.setEditable(true);
        mAddress.setBackground(textBackgroundColor);
        mAddress.setFont(MonitorFonts.inputFont);
        mAddress.setAlignmentX(Component.LEFT_ALIGNMENT);
        mAddress.setPreferredSize(new Dimension(prefHalf - prefRemBut, prefHeight2));
        mAddress.setMaximumSize(new Dimension(maxHalf - maxRemBut, maxHeight));
        mAddress.addActionListener(al);
        // Set editable comboBox colors
        c = mAddress.getEditor().getEditorComponent();
        c.setBackground(textBackgroundColor);
        c.setForeground(textColor);

        // button for multicast address removal
        final JButton remove2 = new JButton("X");
        remove2.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                int index = mAddress.getSelectedIndex();
                if (index > -1) {
                    mAddress.removeItemAt(index);
                }
            }
        });
        remove2.setAlignmentX(Component.LEFT_ALIGNMENT);
        remove2.setForeground(titleColor);
        remove2.setBackground(backgroundColor);
        remove2.setPreferredSize(new Dimension(prefRemBut, prefHeight2));
        remove2.setMaximumSize(new Dimension(maxRemBut, maxHeight));

        p5.add(mAddress);
        p5.add(remove2);

        // add to parent panel
        p3.add(l4);
        p3.add(l5);
        p3.add(p4);
        p3.add(p5);

        // panels for ports & ttl
        JPanel p6 = new JPanel();
        p6.setLayout(new GridLayout(1, 1, horSpace, 0));
        p6.setBackground(backgroundColor);
        p6.setPreferredSize(new Dimension(prefWidth, prefHeight2 * 2));
        p6.setMaximumSize(new Dimension(maxWidth, maxHeight * 2));
        p6.setBorder(new EmptyBorder(edge2, edge2, 0, edge2));
        p6.setAlignmentX(Component.LEFT_ALIGNMENT);

        JPanel p7 = new JPanel();
        p7.setLayout(new BoxLayout(p7, BoxLayout.X_AXIS));
        p7.setBackground(backgroundColor);
        p7.setAlignmentX(Component.LEFT_ALIGNMENT);

        JPanel p8 = new JPanel();
        p8.setLayout(new GridLayout(2, 2, 0, 2));
        p8.setBackground(backgroundColor);
        p8.setAlignmentX(Component.LEFT_ALIGNMENT);

        JPanel p9 = new JPanel();
        p9.setLayout(new GridLayout(2, 1, 0, 2));
        p9.setBackground(backgroundColor);
        p9.setPreferredSize(new Dimension((int) (prefHalf * .67), prefHeight1 * 2));
        p9.setMaximumSize(new Dimension((int) (maxHalf * .67), maxHeight * 2));

        JPanel p10 = new JPanel();
        p10.setLayout(new GridLayout(2, 1, 0, 2));
        p10.setBackground(backgroundColor);
        p10.setPreferredSize(new Dimension((int) (prefHalf * .33), prefHeight1 * 2));
        p10.setMaximumSize(new Dimension((int) (maxHalf * .33), maxHeight * 2));

        // ports & ttl labels
        JLabel l0 = new JLabel("UDP Port: ", JLabel.RIGHT);
        JLabel l1 = new JLabel("TCP Port: ", JLabel.RIGHT);
        JLabel l2 = new JLabel("Multicast Port: ", JLabel.RIGHT);
        JLabel l3 = new JLabel("TTL Value: ", JLabel.RIGHT);
        l0.setFont(MonitorFonts.titleFont);
        l1.setFont(MonitorFonts.titleFont);
        l2.setFont(MonitorFonts.titleFont);
        l3.setFont(MonitorFonts.titleFont);
        l0.setForeground(titleColor);
        l1.setForeground(titleColor);
        l2.setForeground(titleColor);
        l3.setForeground(titleColor);

        // text input for udp/broadcast port number
        udpPort = new WholeNumberField(Constants.broadcastPort, 8, 1024, 65535);
        udpPort.setAlignmentX(Component.LEFT_ALIGNMENT);
        udpPort.setFont(MonitorFonts.inputFont);
        udpPort.setForeground(textColor);
        udpPort.setBackground(textBackgroundColor);
        // make sure there's a valid value entered
        udpPort.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                WholeNumberField source = (WholeNumberField) e.getSource();
                source.correctValue();
            }
        }
        );
        udpPort.addFocusListener(new FocusAdapter() {
            public void focusLost(FocusEvent e) {
                WholeNumberField source = (WholeNumberField) e.getSource();
                source.correctValue();
            }
        }
        );
        udpPort.addMouseListener(new MouseAdapter() {
            public void mouseExited(MouseEvent e) {
                WholeNumberField source = (WholeNumberField) e.getSource();
                source.correctValue();
            }
        }
        );

        // text input for udp multicast port number
        mcastPort = new WholeNumberField(Constants.multicastPort, 8, 1024, 65535);
        mcastPort.setAlignmentX(Component.LEFT_ALIGNMENT);
        mcastPort.setFont(MonitorFonts.inputFont);
        mcastPort.setForeground(textColor);
        mcastPort.setBackground(textBackgroundColor);
        // make sure there's a valid value entered
        mcastPort.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                WholeNumberField source = (WholeNumberField) e.getSource();
                source.correctValue();
            }
        }
        );
        mcastPort.addFocusListener(new FocusAdapter() {
            public void focusLost(FocusEvent e) {
                WholeNumberField source = (WholeNumberField) e.getSource();
                source.correctValue();
            }
        }
        );
        mcastPort.addMouseListener(new MouseAdapter() {
            public void mouseExited(MouseEvent e) {
                WholeNumberField source = (WholeNumberField) e.getSource();
                source.correctValue();
            }
        }
        );

        // text input for tcp server port number
        tcpPort = new WholeNumberField(Constants.serverPort, 8, 1024, 65535);
        tcpPort.setFont(MonitorFonts.inputFont);
        tcpPort.setAlignmentX(Component.LEFT_ALIGNMENT);
        tcpPort.setForeground(textColor);
        tcpPort.setBackground(textBackgroundColor);
        tcpPort.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                WholeNumberField source = (WholeNumberField) e.getSource();
                source.correctValue();
            }
        }
        );
        tcpPort.addFocusListener(new FocusAdapter() {
            public void focusLost(FocusEvent e) {
                WholeNumberField source = (WholeNumberField) e.getSource();
                source.correctValue();
            }
        }
        );
        tcpPort.addMouseListener(new MouseAdapter() {
            public void mouseExited(MouseEvent e) {
                WholeNumberField source = (WholeNumberField) e.getSource();
                source.correctValue();
            }
        }
        );

        // text input for TTL value
        ttl = new WholeNumberField(Constants.multicastTTL, 6, 0, 255);
        ttl.setFont(MonitorFonts.inputFont);
        ttl.setAlignmentX(Component.LEFT_ALIGNMENT);
        ttl.setForeground(textColor);
        ttl.setBackground(textBackgroundColor);
        ttl.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                WholeNumberField source = (WholeNumberField) e.getSource();
                source.correctValue();
            }
        }
        );
        ttl.addFocusListener(new FocusAdapter() {
            public void focusLost(FocusEvent e) {
                WholeNumberField source = (WholeNumberField) e.getSource();
                source.correctValue();
            }
        }
        );
        ttl.addMouseListener(new MouseAdapter() {
            public void mouseExited(MouseEvent e) {
                WholeNumberField source = (WholeNumberField) e.getSource();
                source.correctValue();
            }
        }
        );

        // add to parent panel
        p9.add(l2);
        p9.add(l3);
        p10.add(mcastPort);
        p10.add(ttl);

        p7.add(p9);
        p7.add(p10);

        p8.add(l0);
        p8.add(udpPort);
        p8.add(l1);
        p8.add(tcpPort);

        p6.add(p8);
        p6.add(p7);

        // broadcast, multicast, both, direct, or direct udp connections
        JPanel p14 = new JPanel();
        p14.setLayout(new BoxLayout(p14, BoxLayout.X_AXIS));
        p14.setBackground(backgroundColor);
        p14.setPreferredSize(new Dimension(prefWidth, prefHeight2));
        p14.setMaximumSize(new Dimension(maxWidth, maxHeight));
        p14.setAlignmentX(Component.LEFT_ALIGNMENT);

        // comboBox for host name
        cast = new JComboBox(new String[]{"broadcasting",
                "multicasting",
                "broad & multicasting",
                "direct connection"
        });
        cast.setEditable(false);
        cast.setFont(MonitorFonts.inputFont);
        cast.setBackground(textBackgroundColor);
        cast.setPreferredSize(new Dimension(prefWidth - 2 * edge2, prefHeight2));
        cast.setMaximumSize(new Dimension(maxWidth - 2 * edge2, maxHeight));
        cast.addActionListener(
                new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        JComboBox jcb = (JComboBox) e.getSource();
                        String selecteditem = (String) jcb.getSelectedItem();

                        if (selecteditem.equals("broadcasting")) {
                            bAddress.setEnabled(true);
                            udpPort.setEnabled(true);
                            remove1.setEnabled(true);

                            mcastPort.setEnabled(false);
                            ttl.setEnabled(false);
                            tcpPort.setEnabled(false);
                            mAddress.setEnabled(false);
                            remove2.setEnabled(false);
                        }
                        else if (selecteditem.equals("multicasting")) {
                            ttl.setEnabled(true);
                            mcastPort.setEnabled(true);
                            mAddress.setEnabled(true);
                            remove2.setEnabled(true);
                            udpPort.setEnabled(true);

                            bAddress.setEnabled(false);
                            tcpPort.setEnabled(false);
                            remove1.setEnabled(false);
                        }
                        else if (selecteditem.equals("broad & multicasting")) {
                            tcpPort.setEnabled(false);

                            udpPort.setEnabled(true);
                            mcastPort.setEnabled(true);
                            mAddress.setEnabled(true);
                            bAddress.setEnabled(true);
                            remove1.setEnabled(true);
                            remove2.setEnabled(true);
                            ttl.setEnabled(true);
                        }
                        else if (selecteditem.equals("direct connection")) {
                            tcpPort.setEnabled(true);

                            udpPort.setEnabled(false);
                            mcastPort.setEnabled(false);
                            bAddress.setEnabled(false);
                            mAddress.setEnabled(false);
                            remove1.setEnabled(false);
                            remove2.setEnabled(false);
                            ttl.setEnabled(false);
                        }
                        else {
                            udpPort.setEnabled(true);

                            bAddress.setEnabled(false);
                            mcastPort.setEnabled(false);
                            remove1.setEnabled(false);
                            ttl.setEnabled(false);
                            tcpPort.setEnabled(false);
                            mAddress.setEnabled(false);
                            remove2.setEnabled(false);
                        }
                    }
                }
        );

        // default is broadcasting
        cast.setSelectedIndex(0);

        p14.add(Box.createRigidArea(new Dimension(edge2, 0)));
        p14.add(cast);
        p14.add(Box.createRigidArea(new Dimension(edge2, 0)));

        // panel to hold to "Find ET By" stuff
        TitledBorder border3 = new TitledBorder(new EtchedBorder(),
                                                "Find ET by",
                                                TitledBorder.LEFT,
                                                TitledBorder.ABOVE_TOP,
                                                MonitorFonts.titleFont,
                                                titleColor);

        JPanel p15 = new JPanel();
        p15.setLayout(new BoxLayout(p15, BoxLayout.Y_AXIS));
        p15.setBorder(border3);
        p15.setBackground(backgroundColor);
        p15.setPreferredSize(new Dimension(prefWidth, 5 * prefHeight2 + 2 * edge2));
        p15.setMaximumSize(new Dimension(maxWidth, 5 * maxHeight + 2 * edge2));
        p15.add(Box.createRigidArea(new Dimension(0, edge2)));
        p15.add(p14);
        p15.add(p3);
        p15.add(p6);
        p15.add(Box.createRigidArea(new Dimension(0, edge2)));

        // buttons to connect to ET system or not
        JPanel p16 = new JPanel();
        p16.setLayout(new GridLayout(1, 2, 10, 0));
        p16.setBackground(backgroundColor);
        p16.setPreferredSize(new Dimension(prefWidth, prefHeight2));
        p16.setMaximumSize(new Dimension(maxWidth, maxHeight));
        p16.setAlignmentX(Component.LEFT_ALIGNMENT);

        // button for connecting to ET system
        connect = new JButton("Connect");
        connect.setFont(MonitorFonts.buttonTabMenuFont);
        connect.setForeground(titleColor);
        connect.setBackground(backgroundColor);
        connect.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                SystemOpenConfig config = getEtSystemConfig();
                if (config == null) return;
                addEtSystem(config);
            }
        });

        // button for dismissing window
        JButton dismiss = new JButton("Dismiss");
        dismiss.setFont(MonitorFonts.buttonTabMenuFont);
        dismiss.setForeground(titleColor);
        dismiss.setBackground(backgroundColor);
        dismiss.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                openFrame.setVisible(false);
            }
        });

        p16.add(connect);
        p16.add(dismiss);

        // pack the components
        openPanel.add(Box.createRigidArea(new Dimension(0, verSpace)));
        openPanel.add(p1);
        openPanel.add(Box.createRigidArea(new Dimension(0, verSpace)));
        openPanel.add(p2);
        openPanel.add(Box.createRigidArea(new Dimension(0, verSpace)));
        openPanel.add(p15);
        openPanel.add(Box.createRigidArea(new Dimension(0, verSpace)));
        openPanel.add(p16);

        openFrame.getContentPane().add(openPanel);
        openFrame.pack();
    }


    /**
     * Do everything to add another monitored ET system to this application.
     */
    private void addEtSystem(final SystemOpenConfig config) {
        ConnectionThread t = new ConnectionThread(Monitor.this, config);
        t.start();
    }


    /**
     * Do everything to add another monitored ET system to this application.
     */
    public void addEtSystem(final SystemOpenConfig config, int updatePeriod,
                            int dividerLocation, int orientation, Color[] colors) {
        ConnectionThread t = new ConnectionThread(Monitor.this, config,
                                                  updatePeriod,
                                                  dividerLocation,
                                                  orientation,
                                                  colors);
        t.start();
    }


    /**
     * Gather data about which ET system and how to connect to it.
     */
    private SystemOpenConfig getEtSystemConfig() {

        try {
            boolean specifingHostname = false;
            SystemOpenConfig config = null;

            // Get ET system name.
            String etSystem = (String) etName.getSelectedItem();
            // Get host name.
            String host = (String) hostname.getSelectedItem();
            // Find out how we're connecting with the ET system.
            String howToConnect = (String) cast.getSelectedItem();

            if (host.equals("local")) {
                host = Constants.hostLocal;
                specifingHostname = true;
            }
            else if (host.equals("remote")) {
                host = Constants.hostRemote;
            }
            else if (host.equals("anywhere")) {
                host = Constants.hostAnywhere;
            }
            else {
                specifingHostname = true;
            }

            if (howToConnect.equals("broadcasting")) {
                // find subnet broadcast addresses

                // This no longer is necessary
                /*
                int nAddrs = bAddress.getItemCount();
                String[] addresses = new String[nAddrs];
                for (int i = 0; i < nAddrs; i++) {
                    addresses[i] = (String) bAddress.getItemAt(i);
                    if (!isValidIpAddress(addresses[i])) {
                        throw new EtException("Invalid ip address \"" +
                                addresses[i] + "\" in list.");
                    }
                }
                int port = udpPort.getValue();
                config = new SystemOpenConfig(etSystem, host, Arrays.asList(addresses), port);
                */
                int port = udpPort.getValue();
                config = new SystemOpenConfig(etSystem, port, host);
            }

            else if (howToConnect.equals("multicasting")) {
                // find multicast addresses
                int nAddrs = mAddress.getItemCount();
                String[] addresses = new String[nAddrs];
                for (int i = 0; i < nAddrs; i++) {
                    addresses[i] = (String) mAddress.getItemAt(i);
                    if (!isValidMulticastAddress(addresses[i])) {
                        throw new EtException("Invalid multicast address \"" +
                                addresses[i] + "\" in list.");
                    }
                }
                // get port & ttl #s
                int uPort = udpPort.getValue();
                int mPort = mcastPort.getValue();
                int ttlval = ttl.getValue();
                // Use the multicast specific constructor allowing one to set the udp
                // port. This is significant since going local or to a specific host,
                // a direct udp packet is sent to that host on the udp port
                // (as well as the specified multicast).
                config = new SystemOpenConfig(etSystem, host,
                                              Arrays.asList(addresses),
                                              uPort, mPort, ttlval);
            }

            else if (howToConnect.equals("broad & multicasting")) {
                int nAddrs = 0;
                String[] mAddresses = null;

                // find multicast addresses
                nAddrs = mAddress.getItemCount();
                mAddresses = new String[nAddrs];
                for (int i = 0; i < nAddrs; i++) {
                    mAddresses[i] = (String) mAddress.getItemAt(i);
                    if (!isValidMulticastAddress(mAddresses[i])) {
                        throw new EtException("Invalid multicast address \"" +
                                mAddresses[i] + "\" in list.");
                    }
                }

                // get port & ttl #s
                int uPort = udpPort.getValue();
                int mPort = mcastPort.getValue();
                int tPort = tcpPort.getValue();
                int ttlval = ttl.getValue();

                config = new SystemOpenConfig(etSystem, host, true,
                                              Arrays.asList(mAddresses),
                                              Constants.broadAndMulticast,
                                              tPort, uPort, mPort, ttlval,
                                              Constants.policyError);
            }

            else if (howToConnect.equals("direct connection")) {
                // Since we've making a direct connection, a host name
                // (not remote, or anywhere) must be specified. The selection
                // of "local" can be easily resolved into an actual host name.
                if (!specifingHostname) {
                    throw new EtException("Specify a host's name (not remote, or anywhere) to make a direct connection.");
                }
                int port = tcpPort.getValue();
                config = new SystemOpenConfig(etSystem, host, port);
            }

            return config;
        }

        catch (EtException ex) {
            JOptionPane.showMessageDialog(new JFrame(),
                                          ex.getMessage(),
                                          "Error",
                                          JOptionPane.ERROR_MESSAGE);
        }

        return null;
    }


    /**
     * Make a connection to an ET system & record it.
     */
    public SystemUse makeConnection(final SystemOpenConfig config) {
        if (config == null) {
            return null;
        }

        // Make a connection. Use SystemOpen object directly here
        // instead of SystemUse object so we can see exactly who
        // responded to a broad/multicast if there were multiple
        // responders.
        SystemOpen open = new SystemOpen(config);

        // Change cursor & disable button for waiting.
        openFrame.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
        connect.setEnabled(false);

        try {
            open.connect();
        }
        catch (UnknownHostException ex) {
            JOptionPane.showMessageDialog(new JFrame(),
                                          config.getHost() + " is an unknown host",
                                          "Error",
                                          JOptionPane.ERROR_MESSAGE);
            openFrame.setCursor(Cursor.getDefaultCursor());
            connect.setEnabled(true);
            return null;
        }
        catch (IOException ex) {
            JOptionPane.showMessageDialog(new JFrame(),
                                          "Communication problems with " +
                                                  config.getEtName() + " on " +
                                                  config.getHost() + ":\n" + ex.getMessage(),
                                          "Error",
                                          JOptionPane.ERROR_MESSAGE);
            openFrame.setCursor(Cursor.getDefaultCursor());
            connect.setEnabled(true);
            return null;
        }
        catch (EtTooManyException ex) {
            // This can only happen if specifying "anywhere"
            // or "remote" for the host name.

            String host = null;
            String[] hosts = open.getAllHosts();
            int port = 0;
            int[]    ports = open.getAllPorts();

            if (hosts.length > 1) {
                host = (String) JOptionPane.showInputDialog(
                        new JFrame(),
                        "Choose the ET system responding from host:",
                        "ET System Choice",
                        JOptionPane.PLAIN_MESSAGE,
                        null,
                        hosts,
                        hosts[0]
                );

                if (host == null) {
                    return null;
                }

                for (int i = 0; i < hosts.length; i++) {
                    if (host.equals(hosts[i])) {
                        port = ports[i];
                    }
                }

                // now connect to specified host & port
                try {
                    config.setHost(host);
                    config.setTcpPort(port);
                    config.setContactMethod(Constants.direct);
                    open.connect();
                }
                catch (Exception except) {
                    JOptionPane.showMessageDialog(new JFrame(),
                                                  "Communication problems with " +
                                                          config.getEtName() + " on " +
                                                          config.getHost() + ":\n" + ex.getMessage(),
                                                  "Error",
                                                  JOptionPane.ERROR_MESSAGE);
                    openFrame.setCursor(Cursor.getDefaultCursor());
                    connect.setEnabled(true);
                    return null;
                }
            }
        }
        catch (EtException ex) {
            JOptionPane.showMessageDialog(new JFrame(),
                                          "Cannot find or connect to " + config.getEtName(),
                                          "Error",
                                          JOptionPane.ERROR_MESSAGE);
            openFrame.setCursor(Cursor.getDefaultCursor());
            connect.setEnabled(true);
            return null;
        }

        // Change cursor back to the default & enable button.
        openFrame.setCursor(Cursor.getDefaultCursor());
        connect.setEnabled(true);

        // Keep track of connections since you only want to connect
        // once to each system.
        // Create unique name & enter into Map (hash table)
        String key;
        if (open.getHost().indexOf(".") < 0) {
            key = new String(config.getEtName() + " (" + open.getHost() + ")");
        }
        else {
            key = new String(config.getEtName() + " (" +
                    open.getHost().substring(0, open.getHost().indexOf(".")) + ")");
        }
        if (connections.containsKey(key)) {
            open.disconnect();
            // pop up Dialog box
            JOptionPane.showMessageDialog(new JFrame(),
                                          "You are already connected to " +
                                                  config.getEtName() + " on " +
                                                  config.getHost(),
                                          "ERROR",
                                          JOptionPane.ERROR_MESSAGE);
            return null;
        }

        // Return a SystemUse object - create from SystemOpen object
        SystemUse use = null;
        try {
            use = new SystemUse(open, Constants.debugNone);
        }
        catch (Exception ex) {
            open.disconnect();
            JOptionPane.showMessageDialog(new JFrame(),
                                          "Communication problems with " +
                                                  config.getEtName() + " on " +
                                                  config.getHost() + ":\n" + ex.getMessage(),
                                          "Error",
                                          JOptionPane.ERROR_MESSAGE);
        }

        // Put connection into hash table
        connections.put(key, use);

        // Finally, put an item into the "Load Connection Parameters" menu
        final SystemUse useObject = use;
        final JMenuItem menuItem = new JMenuItem(key);
        menuItem.setFont(MonitorFonts.buttonTabMenuFont);
        menuItem.setBackground(backgroundColor);
        menuItem.setForeground(titleColor);
        loadConnectionParametersMenu.add(menuItem);

        menuItem.addActionListener(
                new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        SystemOpenConfig config = useObject.getConfig();

                        // Add ET system name (if not listed already).
                        addFileName(config.getEtName());
                        // Select ET system name.
                        etName.setSelectedItem(config.getEtName());
                        // Add hostname (if not listed already).
                        String host = config.getHost();
                        if (host.equals(Constants.hostLocal)) {
                            host = "local";
                        }
                        else if (host.equals(Constants.hostRemote)) {
                            host = "remote";
                        }
                        else if (host.equals(Constants.hostAnywhere)) {
                            host = "anywhere";
                        }
                        if (addHostname(host)) {
                            // and select it (only if it's a proper name).
                            hostname.setSelectedItem(host);
                        }

                        // select contact method
                        int method = config.getContactMethod();
                        if (method == Constants.broadcast) {
                            cast.setSelectedItem("broadcasting");
                            // set broadcast addresses
                            bAddress.removeAllItems();
                            bAddress.addItem(SystemOpenConfig.broadcastIP);
                            // broadcast port
                            udpPort.setValue(config.getUdpPort());
                        }
                        else if (method == Constants.multicast) {
                            cast.setSelectedItem("multicasting");
                            // set multicast addresses
                            mAddress.removeAllItems();
                            HashSet set = config.getMulticastAddrs();
                            for (Iterator i = set.iterator(); i.hasNext();) {
                                mAddress.addItem((String) i.next());
                            }
                            // multicast port
                            mcastPort.setValue(config.getMulticastPort());
                            // broadcast port
                            udpPort.setValue(config.getUdpPort());
                            // ttl value
                            ttl.setValue(config.getTTL());
                        }
                        else if (method == Constants.broadAndMulticast) {
                            cast.setSelectedItem("broad & multicasting");
                            // set broadcast addresses
                            bAddress.removeAllItems();
                            bAddress.addItem(SystemOpenConfig.broadcastIP);
                            // set multicast addresses
                            mAddress.removeAllItems();
                            HashSet set = config.getMulticastAddrs();
                            for (Iterator i = set.iterator(); i.hasNext();) {
                                mAddress.addItem((String) i.next());
                            }
                            // multicast port
                            mcastPort.setValue(config.getMulticastPort());
                            // broadcast port
                            udpPort.setValue(config.getUdpPort());
                            // ttl value
                            ttl.setValue(config.getTTL());
                        }
                        else if (method == Constants.direct) {
                            cast.setSelectedItem("direct connection");
                            // tcp port
                            tcpPort.setValue(config.getTcpPort());
                        }
                    }
                }
        );

        return use;
    }


    /**
     * Make a connection to an ET system & record it.
     */
    private void removeConnection(Monitor monitor, MonitorSingleSystem mon,
                                  String key, boolean notInIterator) {
        // Remove display for this connection.
        monitor.tabbedPane.remove(mon.getDisplayPane());
        // Remove menu item used to disconnect it.
        JMenuItem mi;
        int count = monitor.disconnectMenu.getItemCount();
        for (int j = 0; j < count; j++) {
            mi = monitor.disconnectMenu.getItem(j);
            // If we got the right menu item, delete it.
            if (mi.getText().equals(key)) {
                monitor.disconnectMenu.remove(mi);
                break;
            }
        }
        // Remove menu item used to load connection parameters.
        count = monitor.loadConnectionParametersMenu.getItemCount();
        for (int j = 0; j < count; j++) {
            mi = monitor.loadConnectionParametersMenu.getItem(j);
            // If we got the right menu item, delete it.
            if (mi.getText().equals(key)) {
                monitor.loadConnectionParametersMenu.remove(mi);
                break;
            }
        }

        // Close network connection to ET.
        mon.close();

        // Only change HashMap structures if in safe programming environment.
        if (notInIterator) {
            // Remove single system monitor from hash table.
            monitor.monitors.remove(key);
            // Remove SystemUse object from hash table.
            monitor.connections.remove(key);
        }

        // Find currently selected tab (and therefore the key).
        // Set the currentMonitorKey and update period.
        int index = monitor.tabbedPane.getSelectedIndex();
        key = monitor.tabbedPane.getTitleAt(index);
        monitor.currentMonitorKey = key;
        if (key.equals("Help")) {
            monitor.period.setValue(monitor.defaultPeriod);
        }
        else {
            mon = (MonitorSingleSystem) monitor.monitors.get(key);
            monitor.period.setValue(mon.getUpdatePeriod());
        }
        return;
    }


    /**
     * Display a new ET system connection.
     */
    public void displayEtSystem(final SystemOpenConfig config, final SystemUse use) {
        displayEtSystem(config, use, defaultPeriod, tabbedPane.getWidth() / 2,
                        JSplitPane.HORIZONTAL_SPLIT, null);
    }


    /**
     * Display a new ET system connection.
     */
    public void displayEtSystem(final SystemOpenConfig config, final SystemUse use,
                                int updatePeriod, int dividerLocation,
                                int orientation, Color[] colors) {

        // Create monitor for single ET system
        final MonitorSingleSystem et = new MonitorSingleSystem(use,
                                                               tabbedPane,
                                                               updatePeriod,
                                                               dividerLocation,
                                                               orientation,
                                                               colors);
        // Get it's node for tree display of info
        final DefaultMutableTreeNode etNode = et.getNode();

        // Create name used for menuitem & hashtable key
        // Do not include domain name.
        final String key;
        if (use.getHost().indexOf(".") < 0) {
            key = new String(config.getEtName() + " (" + use.getHost() + ")");
        }
        else {
            key = new String(config.getEtName() + " (" +
                    use.getHost().substring(0, use.getHost().indexOf(".")) + ")");
        }
        // Add monitor to hash table for access by other methods - but
        // only if the HashMap is not being iterated through. This can
        // be a problem since this method is called in a separate thread.
        // We solve the problem by synchronizing on "monitors".
        synchronized (monitors) {
            monitors.put(key, et);
        }

        // Create menuitem to remove ET system from monitor.
        final JMenuItem menuItem = new JMenuItem(key);
        menuItem.setFont(MonitorFonts.buttonTabMenuFont);
        menuItem.setBackground(backgroundColor);
        menuItem.setForeground(titleColor);
        disconnectMenu.add(menuItem);

        menuItem.addActionListener(
                new ActionListener() {
                    public void actionPerformed(ActionEvent e) {
                        removeConnection(Monitor.this, et, key, true);
                    }
                }
        );

        return;
    }


}

/**
 * This class is used to start an independent
 * thread that connects to an ET system. The
 * point of doing this is to avoid doing it in
 * the Swing graphics thread (thereby blocking
 * the GUI for long periods of time).
 */

class ConnectionThread extends Thread {
    private SystemUse use;
    private final Monitor monitor;
    private final Runnable runnable;
    private final SystemOpenConfig config;

    public SystemUse getSystemUse() {
        return use;
    }

    public ConnectionThread(final Monitor mon, SystemOpenConfig con) {
        monitor = mon;
        config = con;
        runnable = new Runnable() {
            public void run() {
                monitor.displayEtSystem(config, use);
            }
        };
    }

    public ConnectionThread(final Monitor mon, SystemOpenConfig con,
                            final int updatePeriod, final int dividerLocation,
                            final int orientation, final Color[] colors) {
        monitor = mon;
        config = con;
        runnable = new Runnable() {
            public void run() {
                monitor.displayEtSystem(config, use, updatePeriod,
                                        dividerLocation, orientation, colors);
            }
        };
    }

    public void run() {
        // Make a connection to the ET system.
        use = monitor.makeConnection(config);
        if (use == null) return;
        // Run displayEtSystem (graphics stuff) in swing thread.
        SwingUtilities.invokeLater(runnable);
    }
}




