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

import java.io.*;
import java.util.*;
import java.awt.*;
import javax.swing.JSplitPane;
import javax.xml.parsers.SAXParserFactory;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.validation.Schema;
import javax.xml.validation.SchemaFactory;
import javax.xml.XMLConstants;

import org.jlab.coda.et.*;
import org.xml.sax.*;
import org.xml.sax.helpers.DefaultHandler;

/**
 * A SAX2 ContentHandler.
 *
 */
public class MonitorConfiguration extends DefaultHandler {
    // Defaults
    private SAXParser parser;
    private static boolean setValidation        = true;
    private static boolean setNameSpaces        = true;
    private static boolean setSchemaSupport     = true;
    private static boolean setSchemaFullSupport = true;
    
    private StringBuffer buffer = new StringBuffer(100);
    private HashMap<String,Object>  dataStorage = new HashMap<String,Object>(100);
    
    private Monitor   monitor;
    private boolean   isColor;
    private boolean   readWindowParametersOnly;
    private boolean   finishedReadingWindowParameters;
    private String    currentElement;
    private String    findMethod;
    private Color[]   mainColors = new Color[6];
    private Point     windowLocation;
    private Dimension windowSize;
    

    /** Constructor. */
    public MonitorConfiguration(Monitor mon) {
        monitor = mon;
	
        // Use the validating parser
        SAXParserFactory factory = SAXParserFactory.newInstance();
        factory.setNamespaceAware(true);
        SchemaFactory sf = SchemaFactory.newInstance(XMLConstants.W3C_XML_SCHEMA_NS_URI);
        Schema schema = null;
        try {
            schema = sf.newSchema(new File("monitorConfiguration.xsd"));
        }
        catch (SAXException e) {
            e.printStackTrace();
        }

        factory.setSchema(schema);

        // Parse the input
        try {
            parser = factory.newSAXParser();
        }
        catch (ParserConfigurationException e) {
            e.printStackTrace();
        }
        catch (SAXException e) {
            e.printStackTrace();
        }
    }
    
    

    public void setMonitor(Monitor mon) {monitor = mon;}
    
    // Methods for getting application window & color data from
    // window parameter reading of config file.
    public Point getWindowLocation() {return new Point(windowLocation);}
    public Dimension getWindowSize() {return new Dimension(windowSize);}
    public Color[] getWindowColors() {return (Color[]) mainColors.clone();}
    
    // Methods for parsing window & color data from config file.
    public void loadWindowParameters(File file) throws IOException, SAXException {
      loadWindowParameters(file.getPath());
    }
    public void loadWindowParameters(String fileName) throws IOException, SAXException {
        File f = new File(fileName);

        readWindowParametersOnly = true;
        parser.parse(f,this);
        readWindowParametersOnly = false;
        finishedReadingWindowParameters = false;
        return;
    }
    
    // Methods for reading the rest of the config information.
    public void load(String fileName) throws IOException, SAXException {
        File f = new File(fileName);
        parser.parse(f, this);
    }
    public void load(File file) throws IOException, SAXException {
      load(file.getPath());
    }


    //=============================
    // SAX DocumentHandler methods
    //=============================
    // Start element.
    public void startElement(String uri, String local, String qname,
                             Attributes attrs) {
        // keep track of the current element
        currentElement = local;
        // If we're setting a color, its attributes give red, green, & blue.
        if (local.indexOf("Color") > -1) {
            try {
                // Get rgb components.
                Color c = new Color(Integer.parseInt(attrs.getValue("red")),
                                    Integer.parseInt(attrs.getValue("green")),
                                    Integer.parseInt(attrs.getValue("blue")));
                dataStorage.put(currentElement, c);
                isColor = true;
            }
            catch (NumberFormatException ex) {
            }
        }
        // Keep track of method used to find ET system.
        else if (local.equals("broadcasting") ||
                local.equals("multicasting") ||
                local.equals("broadAndMulticasting") ||
                local.equals("direct") ||
                local.equals("udpToHost")) {
            findMethod = currentElement;
        }
    }

    // Characters. This may be called more than once for each item.
    public void characters(char ch[], int start, int length) {
      if (finishedReadingWindowParameters || ch == null || length == 0) {
        return;
      }
      // put data into a buffer
      buffer.append(ch, start, length);
    }
    
    
    // End element. Note that white space is ignored when validating an element's
    // value. So, white space gets passed on to the user and must be trimmed off.
    public void endElement(String uri, String local, String qname) {
        if (finishedReadingWindowParameters) {
            return;
        }

        if (isColor) {
            isColor = false;
        }
        else if (buffer.length() > 0) {
            // put data (as string) into hash table
            dataStorage.put(currentElement, buffer.toString());
            // erase buffer for next element
            buffer.setLength(0);
        }

        // adjust GUI parameters
        if (local.equals("graphics")) {
            // First, handle case of only reading main application's window's parameters.
            if (readWindowParametersOnly) {
                // optional elements
                if (dataStorage.containsKey("titleColor")) {
                    mainColors[0] = (Color) dataStorage.get("titleColor");
                }
                if (dataStorage.containsKey("backgroundColor")) {
                    mainColors[1] = (Color) dataStorage.get("backgroundColor");
                }
                if (dataStorage.containsKey("selectedTabColor")) {
                    mainColors[2] = (Color) dataStorage.get("selectedTabColor");
                }
                if (dataStorage.containsKey("tabsBackgroundColor")) {
                    mainColors[3] = (Color) dataStorage.get("tabsBackgroundColor");
                }
                if (dataStorage.containsKey("textColor")) {
                    mainColors[4] = (Color) dataStorage.get("textColor");
                }
                if (dataStorage.containsKey("textBackgroundColor")) {
                    mainColors[5] = (Color) dataStorage.get("textBackgroundColor");
                }

                // mandatory elements
                int w = Integer.parseInt(((String) dataStorage.get("width")).trim());
                int h = Integer.parseInt(((String) dataStorage.get("height")).trim());
                int x = Integer.parseInt(((String) dataStorage.get("xPosition")).trim());
                int y = Integer.parseInt(((String) dataStorage.get("yPosition")).trim());
                windowLocation = new Point(x, y);
                windowSize = new Dimension(w, h);
                finishedReadingWindowParameters = true;

                dataStorage.clear();
                return;
            }

            // optional elements
            if (dataStorage.containsKey("fileNameList")) {
                // Divide list - items separated by white space - into component parts.
                StringTokenizer tok = new StringTokenizer((String) dataStorage.get("fileNameList"));
                while (tok.hasMoreTokens()) {
                    monitor.addFileName(tok.nextToken());
                }
            }
            if (dataStorage.containsKey("hostList")) {
                StringTokenizer tok = new StringTokenizer((String) dataStorage.get("hostList"));
                while (tok.hasMoreTokens()) {
                    monitor.addHostname(tok.nextToken());
                }
            }

            dataStorage.clear();
        }

        // try to make connection to ET system
        else if (local.equals("etConnection")) {
            String etSystem = (String) dataStorage.get("fileName");
            int period = Integer.parseInt((String) dataStorage.get("period"));
            int divider = Integer.parseInt((String) dataStorage.get("dividerPosition"));
            int orientation = JSplitPane.HORIZONTAL_SPLIT;
            if (((String) dataStorage.get("orientation")).equals("vertical")) {
                orientation = JSplitPane.VERTICAL_SPLIT;
            }

            Color[] colors = new Color[10];
            if (dataStorage.containsKey("stationColor")) {
                colors[0] = (Color) dataStorage.get("stationColor");
            }
            if (dataStorage.containsKey("stationIdleColor")) {
                colors[1] = (Color) dataStorage.get("stationIdleColor");
            }
            if (dataStorage.containsKey("attachmentColor")) {
                colors[2] = (Color) dataStorage.get("attachmentColor");
            }
            if (dataStorage.containsKey("eventColor")) {
                colors[3] = (Color) dataStorage.get("eventColor");
            }
            if (dataStorage.containsKey("lineColor")) {
                colors[4] = (Color) dataStorage.get("lineColor");
            }
            if (dataStorage.containsKey("textColor")) {
                colors[5] = (Color) dataStorage.get("textColor");
            }
            if (dataStorage.containsKey("textBackgroundColor")) {
                colors[6] = (Color) dataStorage.get("textBackgroundColor");
            }
            if (dataStorage.containsKey("backgroundColor")) {
                colors[7] = (Color) dataStorage.get("backgroundColor");
            }
            if (dataStorage.containsKey("treeTextColor")) {
                colors[8] = (Color) dataStorage.get("treeTextColor");
            }
            if (dataStorage.containsKey("treeBackgroundColor")) {
                colors[9] = (Color) dataStorage.get("treeBackgroundColor");
            }

            int index = 0, ttl = 0, dummy = 11111;
            int broadcastPort = 0, multicastPort = 0, port = 0;
            String host = null;
            SystemOpenConfig config = null;

            try {
                if (findMethod.equals("broadcasting")) {
                    if (dataStorage.containsKey("location")) {
                        host = (String) dataStorage.get("location");
                        if (host.equals("local")) {
                            host = Constants.hostLocal;
                        }
                        else if (host.equals("remote")) {
                            host = Constants.hostRemote;
                        }
                        else {
                            host = Constants.hostAnywhere;
                        }
                    }
                    else {
                        host = (String) dataStorage.get("host");
                        monitor.addHostname(host);
                    }
                    broadcastPort = Integer.parseInt((String) dataStorage.get("broadcastPort"));
                    // Can dispense with the address list which now only contains "255.255.255.255"
                    StringTokenizer tok = new StringTokenizer((String) dataStorage.get("broadcastAddressList"));
                    String[] addrs = new String[tok.countTokens()];
                    while (tok.hasMoreTokens()) {
                        addrs[index++] = tok.nextToken();
                    }
                    config = new SystemOpenConfig(etSystem, broadcastPort, host);
                }
                else if (findMethod.equals("multicasting")) {
                    if (dataStorage.containsKey("location")) {
                        host = (String) dataStorage.get("location");
                        if (host.equals("local")) {
                            host = Constants.hostLocal;
                        }
                        else if (host.equals("remote")) {
                            host = Constants.hostRemote;
                        }
                        else {
                            host = Constants.hostAnywhere;
                        }
                    }
                    else {
                        host = (String) dataStorage.get("host");
                        monitor.addHostname(host);
                    }
                    ttl = Integer.parseInt((String) dataStorage.get("ttl"));
                    multicastPort = Integer.parseInt((String) dataStorage.get("multicastPort"));
                    StringTokenizer tok = new StringTokenizer((String) dataStorage.get("multicastAddressList"));
                    String[] addrs = new String[tok.countTokens()];
                    while (tok.hasMoreTokens()) {
                        addrs[index++] = tok.nextToken();
                    }

                    int udpPort = Constants.broadcastPort;
                    if (dataStorage.containsKey("udpPort")) {
                        udpPort = Integer.parseInt((String) dataStorage.get("udpPort"));
                    }
                    config = new SystemOpenConfig(etSystem, host,
                                                  Arrays.asList(addrs),
                                                  udpPort, multicastPort, ttl);
                }
                else if (findMethod.equals("broadAndMulticasting")) {
                    if (dataStorage.containsKey("location")) {
                        host = (String) dataStorage.get("location");
                        if (host.equals("local")) {
                            host = Constants.hostLocal;
                        }
                        else if (host.equals("remote")) {
                            host = Constants.hostRemote;
                        }
                        else {
                            host = Constants.hostAnywhere;
                        }
                    }
                    else {
                        host = (String) dataStorage.get("host");
                        monitor.addHostname(host);
                    }
                    ttl = Integer.parseInt((String) dataStorage.get("ttl"));
                    broadcastPort = Integer.parseInt((String) dataStorage.get("broadcastPort"));
                    multicastPort = Integer.parseInt((String) dataStorage.get("multicastPort"));
                    StringTokenizer tok = new StringTokenizer((String) dataStorage.get("broadcastAddressList"));
                    String[] bAddrs = new String[tok.countTokens()];
                    while (tok.hasMoreTokens()) {
                        bAddrs[index++] = tok.nextToken();
                    }
                    index = 0;
                    tok = new StringTokenizer((String) dataStorage.get("multicastAddressList"));
                    String[] mAddrs = new String[tok.countTokens()];
                    while (tok.hasMoreTokens()) {
                        mAddrs[index++] = tok.nextToken();
                    }
                    config = new SystemOpenConfig(etSystem, host, true,
                                                  Arrays.asList(mAddrs),
                                                  Constants.broadAndMulticast,
                                                  dummy, broadcastPort, multicastPort, ttl,
                                                  Constants.policyError);
                }
                else if (findMethod.equals("direct")) {
                    if (dataStorage.containsKey("location")) {
                        host = (String) dataStorage.get("location");
                        if (host.equals("local")) {
                            host = Constants.hostLocal;
                        }
                    }
                    else {
                        host = (String) dataStorage.get("host");
                        monitor.addHostname(host);
                    }
                    port = Integer.parseInt((String) dataStorage.get("tcpPort"));
                    config = new SystemOpenConfig(etSystem, host, port);
                }

            }
            catch (EtException ex) {
                // Should never occur. All problems should be caught by schema validation.
            }

            monitor.addFileName(etSystem);
            monitor.addEtSystem(config, period, divider, orientation, colors);
            dataStorage.clear();
        }
    }


    // Warning.
    public void warning(SAXParseException ex) {
        System.err.println("[Warning] "+
                           getLocationString(ex)+": "+
                           ex.getMessage());
    }


    // Error.
    public void error(SAXParseException ex) throws SAXException {
        System.err.println("[Error] "+
                           getLocationString(ex)+": "+
                           ex.getMessage());
        throw ex;
    }


    // Fatal error.
    public void fatalError(SAXParseException ex) throws SAXException {
        System.err.println("[Fatal Error] "+
                           getLocationString(ex)+": "+
                           ex.getMessage());
        throw ex;
    }

    //===================================
    // End of SAX DocumentHandler methods
    //===================================
    
    

    // Returns a string of the location.
    private String getLocationString(SAXParseException ex) {
        StringBuffer str = new StringBuffer();
        String systemId  = ex.getSystemId();
        if (systemId != null) {
            int index = systemId.lastIndexOf('/');
            if (index != -1)
                systemId = systemId.substring(index + 1);
            str.append(systemId);
        }
        str.append(": line ");
        str.append(ex.getLineNumber());
        str.append(" :col ");
        str.append(ex.getColumnNumber());

        return str.toString();
    }

    
    // Saves data nto a proper xml format configuration file.
    public void save(File file) throws FileNotFoundException {
      String fileName = file.getPath();
      try {
	FileOutputStream fos   = new FileOutputStream(fileName);
	OutputStreamWriter osw = new OutputStreamWriter(fos, "ASCII");

	StringBuffer text = new StringBuffer(1000);

	// Configuration file is in XML format.
	text.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n");
        text.append("<configuration xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n");
	text.append("            xsi:noNamespaceSchemaLocation=\"monitorConfiguration.xsd\">\n\n");
        text.append("  <graphics>\n    <width>");
	text.append(monitor.getMonitorWidth());
	text.append("</width>\n    <height>");
	text.append(monitor.getMonitorHeight());
	text.append("</height>\n    <xPosition>");
	text.append(monitor.getX());
	text.append("</xPosition>\n    <yPosition>");
	text.append(monitor.getY());
	text.append("</yPosition>\n");
	
	// Only put non-default colors into config file.
	Color monColor = monitor.getTitleColor();
	if (monColor.getRGB() != monitor.titleColorDefault.getRGB()) {
	  text.append("    <titleColor red=\"");
	  text.append(monColor.getRed());
	  text.append("\" green=\"");
	  text.append(monColor.getGreen());
	  text.append("\" blue=\"");
	  text.append(monColor.getBlue());
	  text.append("\"/>\n");
	}
	monColor = monitor.getBackgroundColor();
	if (monColor.getRGB() != monitor.backgroundColorDefault.getRGB()) {
	  text.append("    <backgroundColor red=\"");
	  text.append(monColor.getRed());
	  text.append("\" green=\"");
	  text.append(monColor.getGreen());
	  text.append("\" blue=\"");
	  text.append(monColor.getBlue());
	  text.append("\"/>\n");
	}
	monColor = monitor.getSelectedTabColor();
	if (monColor.getRGB() != monitor.selectedTabColorDefault.getRGB()) {
	  text.append("    <selectedTabColor red=\"");
	  text.append(monColor.getRed());
	  text.append("\" green=\"");
	  text.append(monColor.getGreen());
	  text.append("\" blue=\"");
	  text.append(monColor.getBlue());
	  text.append("\"/>\n");
	}
	monColor = monitor.getTabsBackgroundColor();
	if (monColor.getRGB() != monitor.tabsBackgroundColorDefault.getRGB()) {
	  text.append("    <tabsBackgroundColor red=\"");
	  text.append(monColor.getRed());
	  text.append("\" green=\"");
	  text.append(monColor.getGreen());
	  text.append("\" blue=\"");
	  text.append(monColor.getBlue());
	  text.append("\"/>\n");
	}
	monColor = monitor.getTextBackgroundColor();
	if (monColor.getRGB() != monitor.textBackgroundColorDefault.getRGB()) {
	  text.append("    <textBackgroundColor red=\"");
	  text.append(monColor.getRed());
	  text.append("\" green=\"");
	  text.append(monColor.getGreen());
	  text.append("\" blue=\"");
	  text.append(monColor.getBlue());
	  text.append("\"/>\n");
	}
	monColor = monitor.getTextColor();
	if (monColor.getRGB() != monitor.textColorDefault.getRGB()) {
	  text.append("    <textColor red=\"");
	  text.append(monColor.getRed());
	  text.append("\" green=\"");
	  text.append(monColor.getGreen());
	  text.append("\" blue=\"");
	  text.append(monColor.getBlue());
	  text.append("\"/>\n");
	}
	
	osw.write(text.toString());
	text.setLength(0);

	// Lists of host and ET names
	String[] names = monitor.getHostnames();
	if (names != null) {
	  text.append("    <hostList>");
	  for (int i=0; i < names.length; i++) {
	    text.append("\n      ");
	    text.append(names[i]);
	  }
	  text.append("\n    </hostList>\n");
	}
		
	names = monitor.getFileNames();
	if (names != null) {
	  text.append("    <fileNameList>");
	  for (int i=0; i < names.length; i++) {
	    text.append("\n      ");
	    text.append(names[i]);
	  }
	  text.append("\n    </fileNameList>\n");
	}
	text.append("  </graphics>\n\n");
	
	osw.write(text.toString());
	text.setLength(0);

	// Connections to ET systems
	String key;
	SystemUse use=null;
	SystemOpenConfig config;
    for (Map.Entry<String,SystemUse> entry : monitor.connections.entrySet()) {
      // Get object with connection info in it.
	  use = entry.getValue();
	  key = entry.getKey();
	  config = use.getConfig();
	  
	  text.append("  <etConnection>\n    <fileName>");
	  // ET name.
	  text.append(config.getEtName());
	  text.append("</fileName>\n");
	  
	  // Method of finding ET system.
	  int method = config.getContactMethod();
	  if (method == Constants.broadcast) {
	      text.append("    <broadcasting>\n");
	      // Location or host?
	      String host = config.getHost();
	      if (host.equals(Constants.hostLocal)) {
		    text.append("      <location>local</location>\n");
	      }
	      else if (host.equals(Constants.hostRemote)) {
		    text.append("      <location>remote</location>\n");
	      }
	      else if (host.equals(Constants.hostAnywhere)) {
		    text.append("      <location>anywhere</location>\n");
	      }
	      else {
        	text.append("      <host>");
		    text.append(host);
        	text.append("</host>\n");
	      }
	      // List of subnet addresses to broadcast on.
	      text.append("      <broadcastAddressList>");
		  text.append("\n        255.255.255.255");
	      text.append("\n      </broadcastAddressList>\n      <broadcastPort>");
	      text.append(config.getUdpPort());
	      text.append("</broadcastPort>\n    </broadcasting>\n");
	  }
	  else if (method == Constants.multicast) {
	      text.append("    <multicasting>\n");
	      // Location or host?
	      String host = config.getHost();
	      if (host.equals(Constants.hostLocal)) {
		text.append("      <location>local</location>\n");
	      }
	      else if (host.equals(Constants.hostRemote)) {
		text.append("      <location>remote</location>\n");
	      }
	      else if (host.equals(Constants.hostAnywhere)) {
		text.append("      <location>anywhere</location>\n");
	      }
	      else {
        	text.append("      <host>");
		text.append(host);
        	text.append("</host>\n");
	      }
	      // List of multicast addresses to multicast on.
	      text.append("      <multicastAddressList>");
	      for (Iterator j=config.getMulticastAddrs().iterator(); j.hasNext();) {
		text.append("\n        ");
		text.append((String)j.next());
	      }
	      text.append("\n      </multicastAddressList>\n      <multicastPort>");
	      text.append(config.getMulticastPort());
	      text.append("</multicastPort>\n      <ttl>");
	      text.append(config.getTTL());
	      text.append("</ttl>\n    </multicasting>\n");
	  }
	  else if (method == Constants.broadAndMulticast) {
	      text.append("    <broadAndMulticasting>\n");
	      // Location or host?
	      String host = config.getHost();
	      if (host.equals(Constants.hostLocal)) {
		text.append("      <location>local</location>\n");
	      }
	      else if (host.equals(Constants.hostRemote)) {
		text.append("      <location>remote</location>\n");
	      }
	      else if (host.equals(Constants.hostAnywhere)) {
		text.append("      <location>anywhere</location>\n");
	      }
	      else {
        	text.append("      <host>");
		text.append(host);
        	text.append("</host>\n");
	      }
	      // List of subnet addresses to broadcast on.
	      text.append("      <broadcastAddressList>");
          text.append("\n        255.255.255.255");
	      text.append("\n      </broadcastAddressList>\n      <broadcastPort>");
	      text.append(config.getUdpPort());
	      text.append("</broadcastPort>\n");
	      // List of multicast addresses to multicast on.
	      text.append("      <multicastAddressList>");
	      for (Iterator j=config.getMulticastAddrs().iterator(); j.hasNext();) {
		text.append("\n        ");
		text.append((String)j.next());
	      }
	      text.append("\n      </multicastAddressList>\n      <multicastPort>");
	      text.append(config.getMulticastPort());
	      text.append("</multicastPort>\n      <ttl>");
	      text.append(config.getTTL());
	      text.append("</ttl>\n    </broadAndMulticasting>\n");
	  }
	  else if (method == Constants.direct) {
	      text.append("    <direct>\n");
	      String host = config.getHost();
	      if (host.equals(Constants.hostLocal)) {
		text.append("      <location>local</location>\n");
	      }
	      else {
        	text.append("      <host>");
		text.append(host);
        	text.append("</host>\n");
	      }
              text.append("      <tcpPort>");
	      text.append(config.getTcpPort());
	      text.append("</tcpPort>\n");
	      text.append("    </direct>\n");
	  }
	  else {
	      text.append("    <udpToHost>\n");
	      String host = config.getHost();
	      if (host.equals(Constants.hostLocal)) {
		text.append("      <location>local</location>\n");
	      }
	      else {
        	text.append("      <host>");
		text.append(host);
        	text.append("</host>\n");
	      }
              text.append("      <udpPort>");
	      text.append(config.getUdpPort());
	      text.append("</udpPort>\n");
	      text.append("    </udpToHost>\n");
	  }
	  
	  // Update period & splitPane divider position & orientation
          MonitorSingleSystem singleMonitor = (MonitorSingleSystem) monitor.monitors.get(key);
	  text.append("    <period>");
	  text.append(singleMonitor.getUpdatePeriod());
	  text.append("</period>\n    <dividerPosition>");
	  text.append(singleMonitor.getDividerPosition());
	  text.append("</dividerPosition>\n    <orientation>");
	  if (singleMonitor.getOrientation() == JSplitPane.HORIZONTAL_SPLIT) {
	    text.append("horizontal");
	  }
	  else {
	    text.append("vertical");
	  }
	  text.append("</orientation>\n");
	  
	  // Only put non-default colors into config file.
	  monColor = singleMonitor.getEventColor();
	  if (monColor.getRGB() != singleMonitor.eventColorDefault.getRGB()) {
	    text.append("    <eventColor red=\"");
	    text.append(monColor.getRed());
	    text.append("\" green=\"");
	    text.append(monColor.getGreen());
	    text.append("\" blue=\"");
	    text.append(monColor.getBlue());
	    text.append("\"/>\n");
	  }
	  monColor = singleMonitor.getStationColor();
	  if (monColor.getRGB() != singleMonitor.stationColorDefault.getRGB()) {
	    text.append("    <stationColor red=\"");
	    text.append(monColor.getRed());
	    text.append("\" green=\"");
	    text.append(monColor.getGreen());
	    text.append("\" blue=\"");
	    text.append(monColor.getBlue());
	    text.append("\"/>\n");
	  }
	  monColor = singleMonitor.getStationIdleColor();
	  if (monColor.getRGB() != singleMonitor.stationIdleColorDefault.getRGB()) {
	    text.append("    <stationIdleColor red=\"");
	    text.append(monColor.getRed());
	    text.append("\" green=\"");
	    text.append(monColor.getGreen());
	    text.append("\" blue=\"");
	    text.append(monColor.getBlue());
	    text.append("\"/>\n");
	  }
	  monColor = singleMonitor.getAttachmentColor();
	  if (monColor.getRGB() != singleMonitor.attachColorDefault.getRGB()) {
	    text.append("    <attachmentColor red=\"");
	    text.append(monColor.getRed());
	    text.append("\" green=\"");
	    text.append(monColor.getGreen());
	    text.append("\" blue=\"");
	    text.append(monColor.getBlue());
	    text.append("\"/>\n");
	  }
	  monColor = singleMonitor.getLineColor();
	  if (monColor.getRGB() != singleMonitor.lineColorDefault.getRGB()) {
	    text.append("    <lineColor red=\"");
	    text.append(monColor.getRed());
	    text.append("\" green=\"");
	    text.append(monColor.getGreen());
	    text.append("\" blue=\"");
	    text.append(monColor.getBlue());
	    text.append("\"/>\n");
	  }
	  monColor = singleMonitor.getTextColor();
	  if (monColor.getRGB() != singleMonitor.textColorDefault.getRGB()) {
	    text.append("    <textColor red=\"");
	    text.append(monColor.getRed());
	    text.append("\" green=\"");
	    text.append(monColor.getGreen());
	    text.append("\" blue=\"");
	    text.append(monColor.getBlue());
	    text.append("\"/>\n");
	  }
	  monColor = singleMonitor.getTextBackgroundColor();
	  if (monColor.getRGB() != singleMonitor.textBackgroundColorDefault.getRGB()) {
	    text.append("    <textBackgroundColor red=\"");
	    text.append(monColor.getRed());
	    text.append("\" green=\"");
	    text.append(monColor.getGreen());
	    text.append("\" blue=\"");
	    text.append(monColor.getBlue());
	    text.append("\"/>\n");
	  }
	  
	  text.append("  </etConnection>\n\n");
	  osw.write(text.toString());
	  text.setLength(0);
	}
	
	text.append("</configuration>\n");
	
	osw.write(text.toString());
	osw.close();
	fos.close();
      }
      catch (UnsupportedEncodingException ex) {}
      catch (IOException ex) {}
    }
   
    
}
