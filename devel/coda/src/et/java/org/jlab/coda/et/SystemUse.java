/*----------------------------------------------------------------------------*
 *  Copyright (c) 2001        Southeastern Universities Research Association, *
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

package org.jlab.coda.et;

import java.lang.*;
import java.util.*;
import java.util.Map.*;
import java.io.*;
import java.net.*;


/**
 * This class implements an object which allows a user to interact with an ET
 * system. It is not the ET system itself, but rather a proxy which communicates
 * over the network with the real ET system.
 *
 * @author Carl Timmer
 */

public class SystemUse {

  /** Object to specify how to open the ET system of interest. */
  private SystemOpenConfig  openConfig;
  /** Object used to connect to a real ET system. */
  private SystemOpen        sys;
  /** Flag telling whether the real ET system is currently opened or not. */
  private boolean           open;
  /** Debug level. */
  private int               debug;
  /** Tcp socket connected to ET system's tcp server. */
  private Socket            sock;
  /** Socket's input stream. */
  private InputStream       sockIn;
  /** Socket's output stream. */
  private OutputStream      sockOut;
  /** Flag specifying whether the ET system process is Java based or not. */
  private boolean           isJava;
  /** Data input stream built on top of the socket's input stream (with an
   *  intervening buffered input stream). */
  DataInputStream           in;
  /** Data output stream built on top of the socket's output stream (with an
   *  intervening buffered output stream). */
  DataOutputStream          out;


  /**
   * Create a new SystemUse object.
   *
   * @param _config SystemOpenConfig object to specify how to open the ET
   *                system of interest
   * @param _debug  debug level (e.g. {@link Constants#debugInfo})
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception java.net.UnknownHostException
   *     if the host address(es) is(are) unknown
   * @exception org.jlab.coda.et.EtException
   *     if the responding ET system has the wrong name, runs a different
   *     version of ET, or has a different value for
   *     {@link Constants#stationSelectInts}
   * @exception org.jlab.coda.et.EtTooManyException
   *     if there were more than one valid response when policy is set to
   *     {@link Constants#policyError} and we are looking either
   *     remotely or anywhere for the ET system.
   */
  public SystemUse(SystemOpenConfig _config, int _debug) throws
                        IOException, UnknownHostException,
                        EtException, EtTooManyException {
    openConfig = new SystemOpenConfig(_config);
    sys = new SystemOpen(openConfig);
    
    if ((_debug != Constants.debugNone)   &&
        (_debug != Constants.debugSevere) &&
        (_debug != Constants.debugError)  &&
        (_debug != Constants.debugWarn)   &&
        (_debug != Constants.debugInfo))    {
      debug = Constants.debugError;
    }
    else {
      debug = _debug;
    }
    sys.debug = debug;
    
    open();
  }

  /**
   * Create a new SystemUse object. Debug level set to print only errors.
   *
   * @param _config SystemOpenConfig object to specify how to open the ET
   *                system of interest
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception java.net.UnknownHostException
   *     if the host address(es) is(are) unknown
   * @exception org.jlab.coda.et.EtException
   *     if the responding ET system has the wrong name, runs a different
   *     version of ET, or has a different value for
   *     {@link Constants#stationSelectInts}
   * @exception org.jlab.coda.et.EtTooManyException
   *     if there were more than one valid response when policy is set to
   *     {@link Constants#policyError} and we are looking either
   *     remotely or anywhere for the ET system.
   */
  public SystemUse(SystemOpenConfig _config) throws
                        IOException, UnknownHostException,
                        EtException, EtTooManyException {
    openConfig = new SystemOpenConfig(_config);
    sys        = new SystemOpen(openConfig);
    debug      = Constants.debugError;
    sys.debug  = debug;
    open();
  }

  /**
   * Create a new SystemUse object.
   *
   * @param _sys SystemOpen object to specify a connection to the ET
   *                system of interest
   * @param _debug  debug level (e.g. {@link Constants#debugInfo})
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception java.net.UnknownHostException
   *     if the host address(es) is(are) unknown
   * @exception org.jlab.coda.et.EtException
   *     if the responding ET system has the wrong name, runs a different
   *     version of ET, or has a different value for
   *     {@link Constants#stationSelectInts}
   * @exception org.jlab.coda.et.EtTooManyException
   *     if there were more than one valid response when policy is set to
   *     {@link Constants#policyError} and we are looking either
   *     remotely or anywhere for the ET system.
   */
  public SystemUse(SystemOpen _sys, int _debug)  throws
                        IOException, UnknownHostException,
                        EtException, EtTooManyException {
    sys = _sys;
    openConfig = sys.getConfig();
    
    if ((_debug != Constants.debugNone)   &&
        (_debug != Constants.debugSevere) &&
        (_debug != Constants.debugError)  &&
        (_debug != Constants.debugWarn)   &&
        (_debug != Constants.debugInfo))    {
      debug = Constants.debugError;
    }
    else {
      debug = _debug;
    }
    
    if (sys.isConnected()) {
      if (sys.getLanguage() == Constants.langJava) {isJava = true;}

      // buffer communication streams for efficiency
      sock    = sys.getSocket();
      sockIn  = sock.getInputStream();
      sockOut = sock.getOutputStream();
      in  = new DataInputStream(new BufferedInputStream(sock.getInputStream(), 65535));
      out = new DataOutputStream(new BufferedOutputStream(sock.getOutputStream(), 65535));
      open = true;
    }
    else {
      open();
    }

  }


  /**
   * Converts 4 bytes of a byte array into an integer.
   *
   * @param b byte array
   * @param off offset into the byte array (0 = start at first element)
   * @return integer value
   */
  private static final int bytesToInt(byte[] b, int off) {
    int result = ((b[off]  &0xff) << 24) |
                 ((b[off+1]&0xff) << 16) |
                 ((b[off+2]&0xff) <<  8) |
                  (b[off+3]&0xff);
    return result;
  }

  /**
   * Copies an integer value into 4 bytes of a byte array.
   *
   * @param intVal integer value
   * @param b byte array
   * @param off offset into the byte array
   */
  private static final void intToBytes(int intVal, byte[] b, int off) {
    b[off]   = (byte) ((intVal & 0xff000000) >>> 24);
    b[off+1] = (byte) ((intVal & 0x00ff0000) >>> 16);
    b[off+2] = (byte) ((intVal & 0x0000ff00) >>>  8);
    b[off+3] = (byte)  (intVal & 0x000000ff);
  }


  /**
   * Open the ET system and set up buffered communication.
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception java.net.UnknownHostException
   *     if the host address(es) is(are) unknown
   * @exception org.jlab.coda.et.EtException
   *     if the responding ET system has the wrong name, runs a different
   *     version of ET, or has a different value for
   *     {@link Constants#stationSelectInts}
   * @exception org.jlab.coda.et.EtTooManyException
   *     if there were more than one valid response when policy is set to
   *     {@link Constants#policyError} and we are looking either
   *     remotely or anywhere for the ET system.
   */
  synchronized private void open() throws IOException, UnknownHostException,
                                          EtException, EtTooManyException {
    try {
      sys.connect();
    }
    catch (EtTooManyException ex) {
      if (debug >= Constants.debugError) {
        System.out.println("The following hosts responded:");
        for (int i=0; i < sys.respondingHosts.size(); i++) {
          System.out.println("  " + (String) sys.respondingHosts.get(i) +
             " at port " + ((Integer) sys.respondingPorts.get(i)).intValue());
        }
      }
      throw ex;
    }
    
    if (sys.language == Constants.langJava) {isJava = true;}
    
    sock = sys.sock;

    // buffer communication streams for efficiency
    sockIn = sock.getInputStream();
    sockOut = sock.getOutputStream();
    in  = new DataInputStream(new BufferedInputStream(sock.getInputStream(), 65535));
    out = new DataOutputStream(new BufferedOutputStream(sock.getOutputStream(), 65535));
    open = true;
  }

  // ForcedClose and Close do the same thing in Java.

  /** Close the ET system. */
  synchronized public void close() {
    // if communication with ET system fails, we've already been "closed"
    try {
      out.writeInt(Constants.netClose);
      out.flush();
      in.readInt();
    }
    catch (IOException ex) {
      if (debug >= Constants.debugError) {
	System.out.println("network communication error");
      }
    }
    finally {
      try {
        in.close();
	out.close();
	sock.close();
      }
      catch (IOException ex) {}
    }
    open = false;
  }

  /**
   * Is the ET system alive - still up and running?
   *
   *  @return <code>true</code> if the ET system is alive, otherwise
   *  <code>false</code>
   */
  synchronized public boolean alive() {
    int alive;
    // If ET system is NOT alive, or if ET system was killed and restarted
    // (breaking tcp connection), we'll get a read or write error.
    try {
      out.writeInt(Constants.netAlive);
      out.flush();
      alive = in.readInt();
    }
    catch (IOException ex) {
      if (debug >= Constants.debugError) {
	System.out.println("network communication error");
      }
      return false;
    }

    return (alive == 1) ? true : false;
  }


  // No comparable routine to et_wait_for_system is necessary in Java

  /**
   * Wake up an attachment that is waiting to read events from a station's
   * empty input list.
   *
   * @param att attachment to wake up
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if the attachment object is invalid
   */
  synchronized public void wakeUpAttachment(Attachment att)
                                 throws IOException, EtException {
    if (!att.usable || att.sys != this) {
      throw new EtException("Invalid attachment");
    }

    out.writeInt(Constants.netWakeAtt);
    out.writeInt(att.id);
    out.flush();
  }

  /**
   * Wake up all attachments waiting to read events from a station's
   * empty input list.
   *
   * @param stat station whose attachments are to wake up
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if the station object is invalid
   */
  synchronized public void wakeUpAll(Station station)
                                 throws IOException, EtException {
    if (!station.usable || station.sys != this) {
      throw new EtException("Invalid station");
    }

    out.writeInt(Constants.netWakeAll);
    out.writeInt(station.id);
    out.flush();
  }

  //****************************************************
  //                      STATIONS                     *
  //****************************************************

  /**
   * Checks a station configuration for self-consistency.
   *
   * @param config   station configuration
   *
   * @exception org.jlab.coda.et.EtException
   *     if the station configuration is not self-consistent
   */
  private void configCheck(StationConfig config) throws EtException {    

    // USER mode means specifing a class 
    if ((config.selectMode  == Constants.stationSelectUser) &&
        (config.selectClass == null)) {
      throw new EtException("station config needs a select class name");
    }

    // Must be parallel, block, not prescale, and not restore to input list if rrobin or equal cue
    if (((config.selectMode  == Constants.stationSelectRRobin) ||
	 (config.selectMode  == Constants.stationSelectEqualCue)) &&
	((config.flowMode    == Constants.stationSerial) ||
	 (config.blockMode   == Constants.stationNonBlocking) ||
	 (config.restoreMode == Constants.stationRestoreIn)  ||
	 (config.prescale    != 1))) {
      
      throw new EtException("if flowMode = rrobin/equalcue, station must be parallel, nonBlocking, prescale=1, & not restoreIn\n");
    }

    if (config.cue > sys.numEvents) {
      throw new EtException("station configuraton cue size must be < max-#-of-events");
    }
  }



  /**
   * Creates a new station placed at the end of the linked list of stations.
   * If the station is added to a group of parallel stations,
   * it is placed at the end of the linked list of parallel stations.
   *
   * @param config   station configuration
   * @param name     station name
   *
   * @return         new station object
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if the select method's class cannot be loaded, the position is less
   *     than 1 (GRAND_CENTRAL's spot), the name is GRAND_CENTRAL (already
   *     taken), the configuration's cue size is too big, or the configuration
   *     needs a select class name
   * @exception org.jlab.coda.et.EtExistsException
   *     if the station already exists but with a different configuration
   * @exception org.jlab.coda.et.EtTooManyException
   *     if the maximum number of stations has been created already
   */
  public Station createStation(StationConfig config, String name)
                       throws IOException, EtException,
                              EtExistsException, EtTooManyException {
      return createStation(config, name, Constants.end, Constants.end);
  }


  /**
   * Creates a new station at a specified position in the linked list of
   * stations. If the station is added to a group of parallel stations,
   * it is placed at the end of the linked list of parallel stations.
   *
   * @param config   station configuration
   * @param name     station name
   *
   * @return         new station object
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if the select method's class cannot be loaded, the position is less
   *     than 1 (GRAND_CENTRAL's spot), the name is GRAND_CENTRAL (already
   *     taken), the configuration's cue size is too big, or the configuration
   *     needs a select class name
   * @exception org.jlab.coda.et.EtExistsException
   *     if the station already exists but with a different configuration
   * @exception org.jlab.coda.et.EtTooManyException
   *     if the maximum number of stations has been created already
   */
  public Station createStation(StationConfig config, String name, int position)
                       throws IOException, EtException,
                              EtExistsException, EtTooManyException {
      return createStation(config, name, position, Constants.end);
  }


  /**
   * Creates a new station at a specified position in the linked list of
   * stations and in a specified position in a linked list of parallel
   * stations if it is a parallel station.
   *
   * @param config     station configuration
   * @param name       station name
   * @param position   position in the linked list to put the station.
   *
   * @return                new station object
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if the select method's class cannot be loaded, the position is less
   *     than 1 (GRAND_CENTRAL's spot), the name is GRAND_CENTRAL (already
   *     taken), the configuration's cue size is too big, or the configuration
   *     needs a select class name
   * @exception org.jlab.coda.et.EtExistsException
   *     if the station already exists but with a different configuration
   * @exception org.jlab.coda.et.EtTooManyException
   *     if the maximum number of stations has been created already
   */
  synchronized public Station createStation(StationConfig config, String name,
                                            int position, int parallelPosition)
                       throws IOException, EtException,
		              EtExistsException, EtTooManyException {

    // cannot create GrandCentral
    if (name.equals("GRAND_CENTRAL")) {
      throw new EtException("Cannot create GRAND_CENTRAL station");
    }

    // check value of position
    if (position != Constants.end && position < 1) {
      throw new EtException("bad value for position");
    }
    
    // check value of parallel position
    if ((parallelPosition != Constants.end) &&
        (parallelPosition != Constants.newHead) &&
	(parallelPosition  < 0)) {
      throw new EtException("bad value for parallel position");
    }
    
    // check station configuration for self consistency
    configCheck(config);
    
    // command
    out.writeInt(Constants.netStatCrAt);

    // station configuration
    out.writeInt(Constants.structOk); // not used in Java
    out.writeInt(config.flowMode);
    out.writeInt(config.userMode);
    out.writeInt(config.restoreMode);
    out.writeInt(config.blockMode);
    out.writeInt(config.prescale);
    out.writeInt(config.cue);
    out.writeInt(config.selectMode);
    for (int i=0; i < Constants.stationSelectInts; i++) {
      out.writeInt(config.select[i]);
    }

    int functionLength = 0; // no function
    if (config.selectFunction != null) {
      functionLength = config.selectFunction.length() + 1;
    }
    out.writeInt(functionLength);

    int libraryLength = 0; // no lib
    if (config.selectLibrary != null) {
      libraryLength = config.selectLibrary.length() + 1;
    }
    out.writeInt(libraryLength);

    int classLength = 0; // no class
    if (config.selectClass != null) {
      classLength = config.selectClass.length() + 1;
    }
    out.writeInt(classLength);

    // station name and position
    int nameLength = name.length() + 1;
    out.writeInt(nameLength);
    out.writeInt(position);
    out.writeInt(parallelPosition);

    // write string(s)
    try {
      if (functionLength > 0) {
	out.write(config.selectFunction.getBytes("ASCII"));
	out.writeByte(0);
      }
      if (libraryLength > 0) {
	out.write(config.selectLibrary.getBytes("ASCII"));
	out.writeByte(0);
      }
      if (classLength > 0) {
	out.write(config.selectClass.getBytes("ASCII"));
	out.writeByte(0);
      }
      out.write(name.getBytes("ASCII"));
      out.writeByte(0);
    }
    catch (UnsupportedEncodingException ex) {}

    out.flush();

    int err = in.readInt();
    int statId = in.readInt();

    if (err ==  Constants.errorTooMany) {
      throw new EtTooManyException("maximum number of stations already created");
    }
    else if (err == Constants.errorExists) {
      throw new EtExistsException("station already exists with different definition");
    }
    else if (err ==  Constants.error) {
      throw new EtException("trying to add incompatible parallel station, or\n" +
                            "trying to add parallel station to head of existing parallel group, or\n" +
			    "cannot load select class");
    }

    // create station
    Station station = new Station(name, statId, this);
    station.usable = true;
    if (debug >= Constants.debugInfo) {
      System.out.println("creating station " + name + " is done");
    }
    return station;
  }


  /**
   * Removes an existing station.
   *
   * @param station station object
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if attachments to the station still exist, the station is GRAND_CENTRAL
   *     (which must always exist), the station does not exist, or the
   *     station object is invalid
   */
  synchronized public void removeStation(Station station)
                                         throws IOException, EtException {
    // cannot remove GrandCentral
    if (station.id == 0) {
      throw new EtException("Cannot remove GRAND_CENTRAL station");
    }
    // station object invalid
    if (!station.usable || station.sys != this) {
      throw new EtException("Invalid station");
    }

    out.writeInt(Constants.netStatRm);
    out.writeInt(station.id);
    out.flush();

    int err = in.readInt();
    if (err ==  Constants.error) {
      throw new EtException("Either no such station exists " +
                   "or remove all attachments before removing station");
    }
    station.usable = false;
  }


  /**
   * Changes the position of a station in the linked list of stations.
   *
   * @param station   station object
   * @param position  position in the main linked list (starting at 0)
   * @param position  position in a parallel linked list (starting at 0)
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if the station does not exist, trying to move GRAND_CENTRAL, position
   *     is < 1 (GRAND_CENTRAL is always first), parallelPosition < 0, 
   *     station object is invalid,
   *     trying to move an incompatible parallel station to an existing group
   *     of parallel stations or to the head of an existing group of parallel
   *     stations.
   */
  synchronized public void setStationPosition(Station station, int position,
                                              int parallelPosition)
                                              throws IOException, EtException {
    // cannot move GrandCentral
    if (station.id == 0) {
      throw new EtException("Cannot move GRAND_CENTRAL station");
    }

    if ((position != Constants.end) && (position < 0)) {
      throw new EtException("bad value for position");
    }
    else if (position == 0) {
      throw new EtException("GRAND_CENTRAL station is always first");
    }
    if ((parallelPosition != Constants.end) &&
        (parallelPosition != Constants.newHead) &&
	(parallelPosition < 0)) {
      throw new EtException("bad value for parallelPosition");
    }

    if (!station.usable || station.sys != this) {
      throw new EtException("Invalid station");
    }

    out.writeInt(Constants.netStatSPos);
    out.writeInt(station.id);
    out.writeInt(position);
    out.writeInt(parallelPosition);
    out.flush();

    int err = in.readInt();
    if (err ==  Constants.error) {
      station.usable = false;
      throw new EtException("station does not exist");
    }
  }


  /**
   * Gets the position of a station in the linked list of stations.
   *
   * @param station  station object
   * @return         position of a station in the main linked list of stations
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if the station does not exist, or station object is invalid
   */
  synchronized public int getStationPosition(Station station)
                                             throws IOException, EtException {
    // GrandCentral is always first
    if (station.id == 0) {
      return 0;
    }
    if (!station.usable || station.sys != this) {
      throw new EtException("Invalid station");
    }

    out.writeInt(Constants.netStatGPos);
    out.writeInt(station.id);
    out.flush();

    int err = in.readInt();
    int position = in.readInt();
    int pPosition = in.readInt();
    if (err ==  Constants.error) {
      station.usable = false;
      throw new EtException("station does not exist");
    }
    return position;
  }


  /**
   * Gets the position of a parallel station in its linked list of 
   * parallel stations.
   *
   * @param station  station object
   * @return         position of a station in the linked list of stations
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if the station does not exist, or station object is invalid
   */
  synchronized public int getStationParallelPosition(Station station)
                                             throws IOException, EtException {
    // parallel position is 0 for serial stations (like GrandCentral)
    if (station.id == 0) {
      return 0;
    }
    if (!station.usable || station.sys != this) {
      throw new EtException("Invalid station");
    }

    out.writeInt(Constants.netStatGPos);
    out.writeInt(station.id);
    out.flush();

    int err = in.readInt();
    int position = in.readInt();
    int pPosition = in.readInt();
    if (err ==  Constants.error) {
      station.usable = false;
      throw new EtException("station does not exist");
    }
    return pPosition;
  }


  /**
   * Create an attachment to a station.
   *
   * @param station    station object
   * @return           an attachment object
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if the station does not exist, or station object is invalid
   * @exception org.jlab.coda.et.EtTooManyException
   *     if no more attachments are allowed to the station, or
   *     if no more attachments are allowed to ET system
   */
  synchronized public Attachment attach(Station station)
                          throws IOException, EtException, EtTooManyException {

    if (!station.usable || station.sys != this) {
      throw new EtException("Invalid station");
    }

    // find name of our host
    String host = "unknown";
    try {host = InetAddress.getLocalHost().getHostName();}
    catch (UnknownHostException ex) {}

    out.writeInt(Constants.netStatAtt);
    out.writeInt(station.id);
    out.writeInt(-1); // no pid in Java
    out.writeInt(host.length() + 1);

    // write host string
    try {
      out.write(host.getBytes("ASCII"));
      out.writeByte(0);
    }
    catch (UnsupportedEncodingException ex) {}
    out.flush();

    int err = in.readInt();
    int attId = in.readInt();
    if (err ==  Constants.error) {
      station.usable = false;
      throw new EtException("station does not exist");
    }
    else if (err ==  Constants.errorTooMany) {
      throw new EtTooManyException("no more attachments allowed to either station or system");
    }

    Attachment att = new Attachment(station, attId, this);
    att.usable = true;
    return att;
  }



  /**
   * Remove an attachment from a station.
   *
   * @param att   attachment object
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if the attachment object is invalid
   */
  synchronized public void detach(Attachment att)
                                  throws IOException, EtException {
    if (!att.usable || att.sys != this) {
      throw new EtException("Invalid attachment");
    }

    out.writeInt(Constants.netStatDet);
    out.writeInt(att.id);
    out.flush();
    // always returns ok
    in.readInt();
    att.usable = false;
  }


  //*****************************************************
  //                STATION INFORMATION                 *
  //*****************************************************


  /**
   * Tells if an attachment is attached to a station.
   *
   * @param station  station object
   * @param att      attachment object
   *
   * @return         <code>true</code> if an attachment is attached to a station
   *                 and <code>false</code> otherwise
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if the station does not exist, station object is invalid, or attachment
   *     object is invalid
   */
  synchronized public boolean stationAttached(Station station, Attachment att)
                                   throws IOException, EtException {
    if (!station.usable || station.sys != this) {
      throw new EtException("Invalid station");
    }
    if (!att.usable || att.sys != this) {
      throw new EtException("Invalid attachment");
    }

    out.writeInt(Constants.netStatIsAt);
    out.writeInt(station.id);
    out.writeInt(att.id);
    out.flush();
    int err = in.readInt();
    if (err == Constants.error) {
      station.usable = false;
      throw new EtException("station does not exist");
    }
    return (err == 1) ? true : false;
  }


  /**
   * Tells if a station exists.
   *
   * @param name   station name
   * @return       <code>true</code> if a station exists and
   *               <code>false</code> otherwise
   * @exception java.io.IOException
   *     if problems with network comunications
   */
  synchronized public boolean stationExists(String name)
                                   throws IOException {

    out.writeInt(Constants.netStatEx);
    out.writeInt(name.length()+1);
    try {
      out.write(name.getBytes("ASCII"));
      out.writeByte(0);
    }
    catch (UnsupportedEncodingException ex) {}
    out.flush();
    int err = in.readInt();
    int statId = in.readInt();
    // id is ignored here since we can't return it as a C function can
    return (err == 1) ? true : false;
  }


  /**
   * Gets a station's object representation from its name.
   *
   * @param name   station name
   * @return       station object
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if the station does not exist
   */
  synchronized public Station stationNameToObject(String name)
                                   throws IOException, EtException {
    out.writeInt(Constants.netStatEx);
    out.writeInt(name.length()+1);
    try {
      out.write(name.getBytes("ASCII"));
      out.writeByte(0);
    }
    catch (UnsupportedEncodingException ex) {}
    out.flush();

    int err = in.readInt();
    int statId = in.readInt();

    if (err == 1) {
      Station stat = new Station(name, statId, this);
      stat.usable = true;
      return stat;
    }
    throw new EtException("station " + name + " does not exist");
  }

  //*****************************************************
  //                       EVENTS
  //*****************************************************


  /**
   * Get new or unused events from an ET system.
   *
   * @param att       attachment object
   * @param mode      if there are no events available, this parameter specifies
   *                  whether to wait for some by sleeping, by waiting for a set
   *                  time, or by returning immediately
   * @param microSec  the number of microseconds to wait if a timed wait is
   *                  specified
   * @param count     the number of events desired
   * @param size      the size of events in bytes
   *
   * @return an list of events
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if arguments have bad values or attachment object is invalid
   * @exception org.jlab.coda.et.EtEmptyException
   *     if the mode is asynchronous and the station's input list is empty
   * @exception org.jlab.coda.et.EtBusyException
   *     if the mode is asynchronous and the station's input list is being used
   *     (the mutex is locked)
   * @exception org.jlab.coda.et.EtTimeoutException
   *     if the mode is timed wait and the time has expired
   * @exception org.jlab.coda.et.EtWakeUpException
   *     if the attachment has been commanded to wakeup,
   *     {@link EventList#wakeUp}, {@link EventList#wakeUpAll}
   */
   synchronized public ArrayList newEventsList(Attachment att, int mode, int microSec,
                                               int count, int size)
                      throws IOException, EtException,
                             EtEmptyException, EtBusyException,
                             EtTimeoutException, EtWakeUpException  {

    if (!att.usable || att.sys != this) {
      throw new EtException("Invalid attachment");
    }
    if (count == 0) {
      return new ArrayList(0);
    }

    int wait = mode & Constants.waitMask;
    if ((wait != Constants.sleep) &&
        (wait != Constants.timed) &&
        (wait != Constants.async))  {
      throw new EtException("bad mode argument");
    }
    else if ((microSec < 0) && (wait == Constants.timed)) {
      throw new EtException("bad microSec argument");
    }
    else if (size < 1) {
      throw new EtException("bad size argument");
    }
    else if (count < 0) {
      throw new EtException("bad count argument");
    }
    else if (att == null) {
      throw new EtException("bad attachment argument");
    }

    int sec  = 0;
    int nsec = 0;
    if (microSec > 0) {
      sec = microSec/1000000;
      nsec = (microSec - sec*1000000) * 1000;
    }
    byte[] buffer = new byte[28];
    intToBytes(Constants.netEvsNew, buffer, 0);
    intToBytes(att.id, buffer, 4);
    intToBytes(mode,   buffer, 8);
    intToBytes(size,   buffer, 12);
    intToBytes(count,  buffer, 16);
    intToBytes(sec,    buffer, 20);
    intToBytes(nsec,   buffer, 24);
    out.write(buffer);
    out.flush();
        
    // ET system clients are liable to get stuck here if the ET
    // system crashes. So use the 2 second socket timeout to try
    // to read again. If the socket connection has been broken,
    // an IOException will be generated.
    int err;
    while (true) {
      try {
	err = in.readInt();
	break;
      }
      // If there's an interrupted ex, socket is OK, try again.
      catch (InterruptedIOException ex) {
      }
    }

    if (err < Constants.ok) {
      if (debug >= Constants.error) {
	System.out.println("error in ET system");
      }
      // throw some exceptions
      if (err == Constants.error) {
        throw new EtException("bad mode value" );
      }
      else if (err == Constants.errorBusy) {
        throw new EtBusyException("input list is busy");
      }
      else if (err == Constants.errorEmpty) {
        throw new EtEmptyException("no events in list");
      }
      else if (err == Constants.errorWakeUp) {
        throw new EtWakeUpException("attachment " + att.id + " woken up");
      }
      else if (err == Constants.errorTimeout) {
        throw new EtTimeoutException("timed out");
      }
    }
    
    // number of events to expect
    int numEvents = err;

    // list of events to return
    ArrayList evList = new ArrayList(numEvents);
    buffer = new byte[4*numEvents];
    in.readFully(buffer, 0, 4*numEvents);

    int   index = -4;
    int   sizeLimit = (size > sys.eventSize) ? size : sys.eventSize;
    Event ev;
    final int modify = Constants.modify;

    for (int j=0; j < numEvents; j++) {
      ev = new Event(size, sizeLimit, isJava);
      ev.id = bytesToInt(buffer, index+=4);
      ev.modify = modify;
      ev.owner = att.id;
      evList.add(ev);
    }
    
    return evList;
  }


  /**
   * Get new or unused events from an ET system.
   *
   * @param att       attachment object
   * @param mode      if there are no events available, this parameter specifies
   *                  whether to wait for some by sleeping, by waiting for a set
   *                  time, or by returning immediately
   * @param microSec  the number of microseconds to wait if a timed wait is
   *                  specified
   * @param count     the number of events desired
   * @param size      the size of events in bytes
   *
   * @return an array of events
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if arguments have bad values or attachment object is invalid
   * @exception org.jlab.coda.et.EtEmptyException
   *     if the mode is asynchronous and the station's input list is empty
   * @exception org.jlab.coda.et.EtBusyException
   *     if the mode is asynchronous and the station's input list is being used
   *     (the mutex is locked)
   * @exception org.jlab.coda.et.EtTimeoutException
   *     if the mode is timed wait and the time has expired
   * @exception org.jlab.coda.et.EtWakeUpException
   *     if the attachment has been commanded to wakeup,
   *     {@link EventList#wakeUp}, {@link EventList#wakeUpAll}
   */
  synchronized public Event[] newEvents(Attachment att, int mode, int microSec,
                                        int count, int size)
                      throws IOException, EtException,
                             EtEmptyException, EtBusyException,
                             EtTimeoutException, EtWakeUpException  {

    if (!att.usable || att.sys != this) {
      throw new EtException("Invalid attachment");
    }
    if (count == 0) {
      // bug bug can array have zero length ???
      return new Event[0];
    }

    int wait = mode & Constants.waitMask;
    if ((wait != Constants.sleep) &&
        (wait != Constants.timed) &&
        (wait != Constants.async))  {
      throw new EtException("bad mode argument");
    }
    else if ((microSec < 0) && (wait == Constants.timed)) {
      throw new EtException("bad microSec argument");
    }
    else if (size < 1) {
      throw new EtException("bad size argument");
    }
    else if (count < 0) {
      throw new EtException("bad count argument");
    }
    else if (att == null) {
      throw new EtException("bad attachment argument");
    }

    int sec  = 0;
    int nsec = 0;
    if (microSec > 0) {
      sec = microSec/1000000;
      nsec = (microSec - sec*1000000) * 1000;
    }
    byte[] buffer = new byte[28];
    intToBytes(Constants.netEvsNew, buffer, 0);
    intToBytes(att.id, buffer, 4);
    intToBytes(mode,   buffer, 8);
    intToBytes(size,   buffer, 12);
    intToBytes(count,  buffer, 16);
    intToBytes(sec,    buffer, 20);
    intToBytes(nsec,   buffer, 24);
    out.write(buffer);
    out.flush();

    // ET system clients are liable to get stuck here if the ET
    // system crashes. So use the 2 second socket timeout to try
    // to read again. If the socket connection has been broken,
    // an IOException will be generated.
    int err;
    while (true) {
      try {
	err = in.readInt();
	break;
      }
      // If there's an interrupted ex, socket is OK, try again.
      catch (InterruptedIOException ex) {
      }
    }

    if (err < Constants.ok) {
      if (debug >= Constants.error) {
	System.out.println("error in ET system");
      }
      // throw some exceptions
      if (err == Constants.error) {
        throw new EtException("bad mode value" );
      }
      else if (err == Constants.errorBusy) {
        throw new EtBusyException("input list is busy");
      }
      else if (err == Constants.errorEmpty) {
        throw new EtEmptyException("no events in list");
      }
      else if (err == Constants.errorWakeUp) {
        throw new EtWakeUpException("attachment " + att.id + " woken up");
      }
      else if (err == Constants.errorTimeout) {
        throw new EtTimeoutException("timed out");
      }
    }

    // number of events to expect
    int numEvents = err;

    // list of events to return
    Event[] evs = new Event[numEvents];
    buffer = new byte[4*numEvents];
    in.readFully(buffer, 0, 4*numEvents);

    int index=-4;
    int sizeLimit = (size > sys.eventSize) ? size : sys.eventSize;
    final int modify = Constants.modify;

    for (int j=0; j < numEvents; j++) {
      evs[j] = new Event(size, sizeLimit, isJava);
      evs[j].id = bytesToInt(buffer, index+=4);
      evs[j].modify = modify;
      evs[j].owner = att.id;
    }

    return evs;
  }


  /**
   * Get events from an ET system.
   *
   * @param att      attachment object
   * @param mode     if there are no events available, this parameter specifies
   *                 whether to wait for some by sleeping, by waiting for a set
   *                 time, or by returning immediately
   * @param microSec the number of microseconds to wait if a timed wait is
   *                 specified
   * @param count    the number of events desired
   *
   * @return an list of events
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if arguments have bad values, the attachment's station is
   *     GRAND_CENTRAL, or the attachment object is invalid
   * @exception org.jlab.coda.et.EtEmptyException
   *     if the mode is asynchronous and the station's input list is empty
   * @exception org.jlab.coda.et.EtBusyException
   *     if the mode is asynchronous and the station's input list is being used
   *     (the mutex is locked)
   * @exception org.jlab.coda.et.EtTimeoutException
   *     if the mode is timed wait and the time has expired
   * @exception org.jlab.coda.et.EtWakeUpException
   *     if the attachment has been commanded to wakeup,
   *     {@link EventList#wakeUp}, {@link EventList#wakeUpAll}
   */
  synchronized public ArrayList getEventsList(Attachment att, int mode, int microSec, int count)
                      throws IOException, EtException,
                             EtEmptyException, EtBusyException,
                             EtTimeoutException, EtWakeUpException  {

    if (!att.usable || att.sys != this) {
      throw new EtException("Invalid attachment");
    }

    // may not get events from GrandCentral
    if (att.station.id == 0) {
      throw new EtException("may not get events from GRAND_CENTRAL");
    }

    if (count == 0) {
      return new ArrayList(0);
    }

    int wait = mode & Constants.waitMask;
    if ((wait != Constants.sleep) &&
        (wait != Constants.timed) &&
        (wait != Constants.async))  {
      throw new EtException("bad mode argument");
    }
    else if ((microSec < 0) && (wait == Constants.timed)) {
      throw new EtException("bad microSec argument");
    }
    else if (count < 0) {
      throw new EtException("bad count argument");
    }
    else if (att == null) {
      throw new EtException("bad attachment argument");
    }

    // Modifying the whole event has precedence over modifying
    // only the header should the user specify both.
    int modify = mode & Constants.modify;
    if (modify == 0) {
      modify = mode & Constants.modifyHeader;
    }

    int sec  = 0;
    int nsec = 0;
    if (microSec > 0) {
      sec = microSec/1000000;
      nsec = (microSec - sec*1000000) * 1000;
    }
    byte[] buffer = new byte[28];
    intToBytes(Constants.netEvsGet, buffer, 0);
    intToBytes(att.id, buffer, 4);
    intToBytes(wait,   buffer, 8);
    intToBytes(modify, buffer, 12);
    intToBytes(count,  buffer, 16);
    intToBytes(sec,    buffer, 20);
    intToBytes(nsec,   buffer, 24);
    out.write(buffer);
    out.flush();
    
    // ET system clients are liable to get stuck here if the ET
    // system crashes. So use the 2 second socket timeout to try
    // to read again. If the socket connection has been broken,
    // an IOException will be generated.
    int err;
    while (true) {
      try {
	err = in.readInt();
	break;
      }
      // If there's an interrupted ex, socket is OK, try again.
      catch (InterruptedIOException ex) {
      }
    }

    if (err < Constants.ok) {
      if (debug >= Constants.error) {
	System.out.println("error in ET system");
      }
      // throw some exceptions
      if (err == Constants.error) {
        throw new EtException("bad mode value" );
      }
      else if (err == Constants.errorBusy) {
        throw new EtBusyException("input list is busy");
      }
      else if (err == Constants.errorEmpty) {
        throw new EtEmptyException("no events in list");
      }
      else if (err == Constants.errorWakeUp) {
        throw new EtWakeUpException("attachment " + att.id + " woken up");
      }
      else if (err == Constants.errorTimeout) {
        throw new EtTimeoutException("timed out");
      }
    }

    int size = in.readInt();

    final int selectInts   = Constants.stationSelectInts;
    final int dataShift    = Constants.dataShift;
    final int dataMask     = Constants.dataMask;
    final int priorityMask = Constants.priorityMask;

    // number of events to expect
    int numEvents = err;

    // list of events to return
    ArrayList evList = new ArrayList(numEvents);
    int byteChunk = 4*(6+Constants.stationSelectInts);
    buffer = new byte[byteChunk];
    int index;

    Event ev;
    int length, memSize, priAndStat;
    for (int j=0; j < numEvents; j++) {
      in.readFully(buffer, 0, byteChunk);

      length = bytesToInt(buffer, 0);
      memSize = bytesToInt(buffer, 4);
      ev = new Event(memSize, memSize, isJava);
      ev.length = length;
      priAndStat = bytesToInt(buffer, 8);
      ev.priority = priAndStat & priorityMask;
      ev.dataStatus = (priAndStat & dataMask) >> dataShift;
      ev.id = bytesToInt(buffer, 12);
      ev.byteOrder = bytesToInt(buffer, 16);
      index = 20;
      for (int i=0; i < selectInts; i++) {
	ev.control[i] = bytesToInt(buffer, index+=4);
      }
      ev.modify = modify;
      ev.owner = att.id;

      // read in data
      in.readFully(ev.data, 0, length);

      // add to eventList
      evList.add(ev);
    }
    
    return evList;
  }



  /**
   * Get events from an ET system.
   *
   * @param att      attachment object
   * @param mode     if there are no events available, this parameter specifies
   *                 whether to wait for some by sleeping, by waiting for a set
   *                 time, or by returning immediately
   * @param microSec the number of microseconds to wait if a timed wait is
   *                 specified
   * @param count    the number of events desired
   *
   * @return an array of events
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if arguments have bad values, the attachment's station is
   *     GRAND_CENTRAL, or the attachment object is invalid
   * @exception org.jlab.coda.et.EtEmptyException
   *     if the mode is asynchronous and the station's input list is empty
   * @exception org.jlab.coda.et.EtBusyException
   *     if the mode is asynchronous and the station's input list is being used
   *     (the mutex is locked)
   * @exception org.jlab.coda.et.EtTimeoutException
   *     if the mode is timed wait and the time has expired
   * @exception org.jlab.coda.et.EtWakeUpException
   *     if the attachment has been commanded to wakeup,
   *     {@link EventList#wakeUp}, {@link EventList#wakeUpAll}
   */
  synchronized public Event[] getEvents(Attachment att, int mode, int microSec, int count)
                      throws IOException, EtException,
                             EtEmptyException, EtBusyException,
                             EtTimeoutException, EtWakeUpException  {

    if (!att.usable || att.sys != this) {
      throw new EtException("Invalid attachment");
    }

    // may not get events from GrandCentral
    if (att.station.id == 0) {
      throw new EtException("may not get events from GRAND_CENTRAL");
    }

    if (count == 0) {
      return new Event[0];
    }

    int wait = mode & Constants.waitMask;
    if ((wait != Constants.sleep) &&
        (wait != Constants.timed) &&
        (wait != Constants.async))  {
      throw new EtException("bad mode argument");
    }
    else if ((microSec < 0) && (wait == Constants.timed)) {
      throw new EtException("bad microSec argument");
    }
    else if (count < 0) {
      throw new EtException("bad count argument");
    }
    else if (att == null) {
      throw new EtException("bad attachment argument");
    }

    // Modifying the whole event has precedence over modifying
    // only the header should the user specify both.
    boolean modifyData = false;
    int modify = mode & Constants.modify;
    if (modify != 0) {
      modifyData = true;
    }
    else {
      modify = mode & Constants.modifyHeader;
    }

    int sec  = 0;
    int nsec = 0;
    if (microSec > 0) {
      sec = microSec/1000000;
      nsec = (microSec - sec*1000000) * 1000;
    }
    byte[] buffer = new byte[28];
    intToBytes(Constants.netEvsGet, buffer, 0);
    intToBytes(att.id, buffer, 4);
    intToBytes(wait,   buffer, 8);
    intToBytes(modify, buffer, 12);
    intToBytes(count,  buffer, 16);
    intToBytes(sec,    buffer, 20);
    intToBytes(nsec,   buffer, 24);
    out.write(buffer);
    out.flush();

    // ET system clients are liable to get stuck here if the ET
    // system crashes. So use the 2 second socket timeout to try
    // to read again. If the socket connection has been broken,
    // an IOException will be generated.
    int err;
    while (true) {
      try {
	err = in.readInt();
	break;
      }
      // If there's an interrupted ex, socket is OK, try again.
      catch (InterruptedIOException ex) {
      }
    }

    if (err < Constants.ok) {
      if (debug >= Constants.error) {
	System.out.println("error in ET system");
      }
      if (err == Constants.error) {
        throw new EtException("bad mode value" );
      }
      else if (err == Constants.errorBusy) {
        throw new EtBusyException("input list is busy");
      }
      else if (err == Constants.errorEmpty) {
        throw new EtEmptyException("no events in list");
      }
      else if (err == Constants.errorWakeUp) {
        throw new EtWakeUpException("attachment " + att.id + " woken up");
      }
      else if (err == Constants.errorTimeout) {
        throw new EtTimeoutException("timed out");
      }
    }

    int size = in.readInt();

    final int selectInts   = Constants.stationSelectInts;
    final int dataShift    = Constants.dataShift;
    final int dataMask     = Constants.dataMask;
    final int priorityMask = Constants.priorityMask;

    int numEvents = err;
    Event[] evs = new Event[numEvents];
    int byteChunk = 4*(6+Constants.stationSelectInts);
    buffer = new byte[byteChunk];
    int index;

    int length, memSize, priAndStat;
    for (int j=0; j < numEvents; j++) {
      in.readFully(buffer, 0, byteChunk);

      length = bytesToInt(buffer, 0);
      memSize = bytesToInt(buffer, 4);
      evs[j] = new Event(memSize, memSize, isJava);
      evs[j].length = length;
      priAndStat = bytesToInt(buffer, 8);
      evs[j].priority = priAndStat & priorityMask;
      evs[j].dataStatus = (priAndStat & dataMask) >> dataShift;
      evs[j].id = bytesToInt(buffer, 12);
      evs[j].byteOrder = bytesToInt(buffer, 16);
      index = 20;
      for (int i=0; i < selectInts; i++) {
	evs[j].control[i] = bytesToInt(buffer, index+=4);
      }
      evs[j].modify = modify;
      evs[j].owner = att.id;

      in.readFully(evs[j].data, 0, length);
    }

    return evs;
  }


  /**
   * Get events from an ET system. Instead of using a buffered input stream,
   * this method uses the socket's input stream and implements its own
   * buffering scheme. This may or may not be faster than the getEvents
   * method - depending on the JVM implementation. On Linux running Sun's 1.3
   * JVM, it's 20% faster than getEvents.
   *
   * @param att      attachment object
   * @param mode     if there are no events available, this parameter specifies
   *                 whether to wait for some by sleeping, by waiting for a set
   *                 time, or by returning immediately
   * @param microSec the number of microseconds to wait if a timed wait is
   *                 specified
   * @param count    the number of events desired
   *
   * @return an array of events
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if arguments have bad values, the attachment's station is
   *     GRAND_CENTRAL, or the attachment object is invalid
   * @exception org.jlab.coda.et.EtEmptyException
   *     if the mode is asynchronous and the station's input list is empty
   * @exception org.jlab.coda.et.EtBusyException
   *     if the mode is asynchronous and the station's input list is being used
   *     (the mutex is locked)
   * @exception org.jlab.coda.et.EtTimeoutException
   *     if the mode is timed wait and the time has expired
   * @exception org.jlab.coda.et.EtWakeUpException
   *     if the attachment has been commanded to wakeup,
   *     {@link EventList#wakeUp}, {@link EventList#wakeUpAll}
   */
  synchronized public Event[] getEvents2(Attachment att, int mode, int microSec, int count)
                      throws EtException, EtEmptyException,
	                     EtBusyException, EtTimeoutException,
			     EtWakeUpException, IOException {

    if (!att.usable || att.sys != this) {
      throw new EtException("Invalid attachment");
    }

    // may not get events from GrandCentral
    if (att.station.id == 0) {
      throw new EtException("may not get events from GRAND_CENTRAL");
    }

    if (count == 0) {
      return new Event[0];
    }

    int wait = mode & Constants.waitMask;
    if ((wait != Constants.sleep) &&
        (wait != Constants.timed) &&
        (wait != Constants.async))  {
      throw new EtException("bad mode argument");
    }
    else if ((microSec < 0) && (wait == Constants.timed)) {
      throw new EtException("bad microSec argument");
    }
    else if (count < 0) {
      throw new EtException("bad count argument");
    }
    else if (att == null) {
      throw new EtException("bad attachment argument");
    }

    // Modifying the whole event has precedence over modifying
    // only the header should the user specify both.
    boolean modifyData = false;
    int modify = mode & Constants.modify;
    if (modify != 0) {
      modifyData = true;
    }
    else {
      modify = mode & Constants.modifyHeader;
    }

    int sec  = 0;
    int nsec = 0;
    if (microSec > 0) {
      sec = microSec/1000000;
      nsec = (microSec - sec*1000000) * 1000;
    }
    byte[] buffer = new byte[28];
    intToBytes(Constants.netEvsGet, buffer, 0);
    intToBytes(att.id, buffer, 4);
    intToBytes(wait,   buffer, 8);
    intToBytes(modify, buffer, 12);
    intToBytes(count,  buffer, 16);
    intToBytes(sec,    buffer, 20);
    intToBytes(nsec,   buffer, 24);
    out.write(buffer);
    out.flush();

    // ET system clients are liable to get stuck here if the ET
    // system crashes. So use the 2 second socket timeout to try
    // to read again. If the socket connection has been broken,
    // an IOException will be generated.
    int bytesRead, val;
    byte[] intHolder = new byte[4];
    
    while (true) {
      try {
	bytesRead = sockIn.read(intHolder, 0 , 4);
	if (bytesRead == -1) {
	  throw new IOException("Socket connection to ET system broken.");
	}
	while (bytesRead < 4) {
	  val = sockIn.read(intHolder, bytesRead, 4-bytesRead);
	  if (val == -1) {
	    throw new IOException("Socket connection to ET system broken.");
	  }
	  bytesRead += val;
	}
	break;
      }
      // If there's an interrupted ex, socket is OK, try again.
      catch (InterruptedIOException ex) {
      }
    }

    int err = bytesToInt(intHolder, 0);
    if (err < Constants.ok) {
      if (debug >= Constants.error) {
	System.out.println("error in ET system");
      }
      if (err == Constants.error) {
        throw new EtException("bad mode value" );
      }
      else if (err == Constants.errorBusy) {
        throw new EtBusyException("input list is busy");
      }
      else if (err == Constants.errorEmpty) {
        throw new EtEmptyException("no events in list");
      }
      else if (err == Constants.errorWakeUp) {
        throw new EtWakeUpException("attachment " + att.id + " woken up");
      }
      else if (err == Constants.errorTimeout) {
        throw new EtTimeoutException("timed out");
      }
    }

    bytesRead = sockIn.read(intHolder, 0 , 4);
    if (bytesRead == -1) {
      throw new IOException("Socket connection to ET system broken.");
    }
    while (bytesRead < 4) {
      val = sockIn.read(intHolder, bytesRead, 4-bytesRead);
      if (val == -1) {
	throw new IOException("Socket connection to ET system broken.");
      }
      bytesRead += val;
    }
    int size = bytesToInt(intHolder, 0);

    int numEvents = err;
    Event[] evs = new Event[numEvents];

    final int selectInts   = Constants.stationSelectInts;
    final int dataShift    = Constants.dataShift;
    final int dataMask     = Constants.dataMask;
    final int priorityMask = Constants.priorityMask;

    final int readLimit = (size > 65535) ? 65535 : size;
    byte[] input = new byte[readLimit];
    bytesRead = sockIn.read(input, 0, readLimit);
    if (bytesRead == -1) {
      throw new IOException("Socket connection to ET system broken.");
    }
    while (bytesRead < readLimit) {
      val = sockIn.read(input, bytesRead, readLimit-bytesRead);
      if (val == -1) {
	throw new IOException("Socket connection to ET system broken.");
      }
      bytesRead += val;
    }

    int headerSize = 4*(6+Constants.stationSelectInts);
    int length, memSize, priAndStat;
    int bytesLeft = bytesRead;
    int bytesCopied = 0;
    int offset = 0;

    for (int j=0; j < numEvents; j++) {
      while (bytesLeft < headerSize) {
        if (bytesLeft == 0) {
          bytesLeft = sockIn.read(input);
	  if (bytesLeft == -1) {
	    throw new IOException("Socket connection to ET system broken.");
	  }
        }
        else {
          if (offset != 0) {
            System.arraycopy(input, offset, input, 0, bytesLeft);
          }
          val = sockIn.read(input, bytesLeft, 65535-bytesLeft);
	  if (val == -1) {
	    throw new IOException("Socket connection to ET system broken.");
	  }
	  bytesLeft += val;
        }
        offset = 0;
      }

      length  = bytesToInt(input, offset);
      memSize = bytesToInt(input, offset+=4);
      evs[j] = new Event(memSize, memSize, isJava);
      evs[j].length = length;
      priAndStat = bytesToInt(input, offset+=4);
      evs[j].priority = priAndStat & priorityMask;
      evs[j].dataStatus = (priAndStat & dataMask) >> dataShift;
      evs[j].id = bytesToInt(input, offset+=4);
      evs[j].byteOrder = bytesToInt(input, offset+=4);
      offset += 4;
      for (int i=0; i < selectInts; i++) {
        evs[j].control[i] = bytesToInt(input, offset+=4);
      }
      offset +=4;
      evs[j].modify = modify;
      evs[j].owner = att.id;
      bytesLeft -= headerSize;


      if (bytesLeft >= length) {
        System.arraycopy(input, offset, evs[j].data, 0, length);
        bytesLeft -= length;
        offset += length;
      }
      else {
        System.arraycopy(input, offset, evs[j].data, 0, bytesLeft);
        bytesCopied = bytesLeft;
        while (bytesCopied < length) {
          val = sockIn.read(evs[j].data, bytesCopied, length-bytesCopied);
	  if (val == -1) {
	    throw new IOException("Socket connection to ET system broken.");
	  }
	  bytesCopied += val;
        }
        offset = 0;
        bytesLeft = 0;
      }

    }
    return evs;
  }


  /**
   * Put events into an ET system.
   *
   * @param att         attachment object
   * @param eventList   list of event objects
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if events are not owned by this attachment or the attachment object
   *     is invalid
   */
  synchronized public void putEventsList(Attachment att, List eventList)
                      throws IOException, EtException {

    if (!att.usable || att.sys != this) {
      throw new EtException("Invalid attachment");
    }

    final int selectInts = Constants.stationSelectInts;
    final int dataShift  = Constants.dataShift;
    final int modify     = Constants.modify;

    // find out how many events we're sending & total # bytes
    Event ev;
    int bytes = 0, numEvents = 0;
    int headerSize = 4*(5+selectInts);
    for (ListIterator i = eventList.listIterator(); i.hasNext(); ) {
      ev = (Event)i.next();
      // each event must be registered as owned by this attachment
      if (ev.owner != att.id) {
        throw new EtException("may not put event(s), not owner");
      }
      // if modifying header only or header & data ...
      if (ev.modify > 0) {
        numEvents++;
	bytes += headerSize;
	// if modifying data as well ...
	if (ev.modify == modify) {
          bytes += ev.length;
	}
      }
    }

    out.writeInt(Constants.netEvsPut);
    out.writeInt(att.id);
    out.writeInt(numEvents);
    out.writeInt(bytes);

    for (ListIterator j = eventList.listIterator(); j.hasNext(); ) {
      ev = (Event) j.next();

      // send only if modifying an event (data or header) ...
      if (ev.modify > 0) {
	out.writeInt(ev.id);
	out.writeInt(ev.length);
	out.writeInt(ev.priority | ev.dataStatus << dataShift);
	out.writeInt(ev.byteOrder);
	out.writeInt(0); // not used
	for (int i=0; i < selectInts; i++) {
          out.writeInt(ev.control[i]);
	}

	// send data only if modifying whole event
	if (ev.modify == modify) {
          out.write(ev.data, 0, ev.length);
	}
      }
    }
    out.flush();

    // err should always be = Constants.ok
    int err = in.readInt();

    return;
  }


  /**
   * Put events into an ET system.
   *
   * @param att   attachment object
   * @param evs   array of event objects
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if events are not owned by this attachment or the attachment object
   *     is invalid
   */
  synchronized public void putEvents(Attachment att, Event[] evs)
                      throws IOException, EtException {

    if (!att.usable || att.sys != this) {
      throw new EtException("Invalid attachment");
    }

    final int selectInts = Constants.stationSelectInts;
    final int dataShift  = Constants.dataShift;
    final int modify     = Constants.modify;

    // find out how many events we're sending & total # bytes
    int bytes = 0, numEvents = 0;
    int headerSize = 4*(5+selectInts);
    for (int i=0; i < evs.length; i++) {
      // each event must be registered as owned by this attachment
      if (evs[i].owner != att.id) {
        throw new EtException("may not put event(s), not owner");
      }
      // if modifying header only or header & data ...
      if (evs[i].modify > 0) {
        numEvents++;
	bytes += headerSize;
	// if modifying data as well ...
	if (evs[i].modify == modify) {
          bytes += evs[i].length;
	}
      }
    }

    out.writeInt(Constants.netEvsPut);
    out.writeInt(att.id);
    out.writeInt(numEvents);
    out.writeInt(bytes);

    for (int j=0; j < evs.length; j++) {
      if (evs[j].modify > 0) {
	out.writeInt(evs[j].id);
	out.writeInt(evs[j].length);
	out.writeInt(evs[j].priority | evs[j].dataStatus << dataShift);
	out.writeInt(evs[j].byteOrder);
	out.writeInt(0);
	for (int i=0; i < selectInts; i++) {
          out.writeInt(evs[j].control[i]);
	}

	if (evs[j].modify == modify) {
          out.write(evs[j].data, 0, evs[j].length);
	}
      }
    }
    out.flush();

    // err should always be = Constants.ok
    int err = in.readInt();

    return;
  }



  /**
   * Put events into an ET system. Instead of using a buffered output stream,
   * this method uses the socket's output stream and implements its own
   * buffering scheme. This may or may not be faster than the putEvents
   * method - depending on the JVM implementation.
   *
   * @param att   attachment object
   * @param evs   array of event objects
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if events are not owned by this attachment or the attachment object
   *     is invalid
   */
  synchronized public void putEvents2(Attachment att, Event[] evs)
                      throws IOException, EtException {

    if (!att.usable || att.sys != this) {
      throw new EtException("Invalid attachment");
    }

    final int selectInts = Constants.stationSelectInts;
    final int dataShift  = Constants.dataShift;
    final int modify     = Constants.modify;

    // find out how many events we're sending & total # bytes
    int bytes = 0, numEvents = 0;
    int headerSize = 4*(5+selectInts);
    for (int i=0; i < evs.length; i++) {
      // each event must be registered as owned by this attachment
      if (evs[i].owner != att.id) {
        throw new EtException("may not put event(s), not owner");
      }
      // if modifying header only or header & data ...
      if (evs[i].modify > 0) {
        numEvents++;
	bytes += headerSize;
	// if modifying data as well ...
	if (evs[i].modify == modify) {
          bytes += evs[i].length;
	}
      }
    }

    byte[] buffer = new byte[65535];
    int length, index=16;

    intToBytes(Constants.netEvsPut, buffer, 0);
    intToBytes(att.id, buffer, 4);
    intToBytes(numEvents, buffer, 8);
    intToBytes(bytes, buffer, 12);

    for (int j=0; j < evs.length; j++) {
      if (evs[j].modify <= 0) {continue;}

      length = evs[j].length;
      intToBytes(evs[j].id, buffer, index);
      intToBytes(length, buffer, index+=4);
      intToBytes(evs[j].priority | evs[j].dataStatus << dataShift, buffer, index+=4);
      intToBytes(evs[j].byteOrder, buffer, index+=4);
      intToBytes(0, buffer, index+=4);
      for (int i=0; i < selectInts; i++) {
        intToBytes(evs[j].control[i], buffer, index+=4);
      }
      index+=4;

      if (evs[j].modify == modify) {
	if (index + headerSize + length > buffer.length) {
          sockOut.write(buffer, 0, index);
          index = 0;
          if (headerSize + length > buffer.length/2) {
            sockOut.write(evs[j].data, 0, length);
            sockOut.flush();
            continue;
          }
          sockOut.flush();
	}
	System.arraycopy(evs[j].data, 0, buffer, index, length);
	index += length;
      }
      else if (index + headerSize > buffer.length) {
        sockOut.write(buffer, 0, index);
        index = 0;
        sockOut.flush();
      }
    }

    if (index > 0) {
      sockOut.write(buffer, 0, index);
      sockOut.flush();
    }

    // err should always be = Constants.ok
    int err = in.readInt();

    return;
  }



  /**
   * Dispose of unwanted events in an ET system. The events are recycled and not
   * made available to any other user.
   *
   * @param att         attachment object
   * @param eventList   list of event objects
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if events are not owned by this attachment or the attachment object
   *     is invalid
   */
  synchronized public void dumpEventsList(Attachment att, List eventList)
                      throws IOException, EtException {

    if (!att.usable || att.sys != this) {
      throw new EtException("Invalid attachment");
    }

    // find out how many we're sending
    Event ev;
    int numEvents = 0;
    for (ListIterator i = eventList.listIterator(); i.hasNext(); ) {
      ev = (Event)i.next();
      // each event must be registered as owned by this attachment
      if (ev.owner != att.id) {
        throw new EtException("may not put event(s), not owner");
      }
      if (ev.modify > 0) numEvents++;
    }

    out.writeInt(Constants.netEvsDump);
    out.writeInt(att.id);
    out.writeInt(numEvents);

    for (ListIterator j = eventList.listIterator(); j.hasNext(); ) {
      ev = (Event) j.next();
      // send only if modifying an event (data or header) ...
      if (ev.modify > 0) {
	out.writeInt(ev.id);
      }
    }
    out.flush();

    // err should always be = Constants.ok
    int err = in.readInt();

    return;
  }


  /**
   * Dispose of unwanted events in an ET system. The events are recycled and not
   * made available to any other user.
   *
   * @param att   attachment object
   * @param evs   array of event objects
   *
   * @exception java.io.IOException
   *     if problems with network comunications
   * @exception org.jlab.coda.et.EtException
   *     if events are not owned by this attachment or the attachment object
   *     is invalid
   */
  synchronized public void dumpEvents(Attachment att, Event[] evs)
                      throws IOException, EtException {

    if (!att.usable || att.sys != this) {
      throw new EtException("Invalid attachment");
    }

    // find out how many we're sending
    int numEvents = 0;
    for (int i=0; i < evs.length; i++) {
      // each event must be registered as owned by this attachment
      if (evs[i].owner != att.id) {
        throw new EtException("may not put event(s), not owner");
      }
      if (evs[i].modify > 0) numEvents++;
    }

    out.writeInt(Constants.netEvsDump);
    out.writeInt(att.id);
    out.writeInt(numEvents);

    byte[] buffer = new byte[4*numEvents];
    int index = -4;

    for (int j=0; j < evs.length; j++) {
      // send only if modifying an event (data or header) ...
      if (evs[j].modify > 0) {
        intToBytes(evs[j].id, buffer, index+=4);
	//out.writeInt(evs[j].id);
      }
    }
    out.write(buffer, 0, index+4);
    out.flush();

    // err should always be = Constants.ok
    int err = in.readInt();

    return;
  }


  /**
   * Gets "integer" format ET system data over the network.
   *
   * @param cmd coded command to send to the TCP server thread.
   * @return integer value
   * @exception java.io.IOException
   *     if there are problems with network communication
   */
  synchronized private int getIntValue(int cmd) throws IOException {
    out.writeInt(cmd);
    out.flush();
    int err = in.readInt();
    int val = in.readInt();

    // err should always be = Constants.ok
    return val;
  }


  /**
   * Gets the number of stations in the ET system.
   *
   * @return number of stations in the ET system
   * @exception java.io.IOException
   *     if there are problems with network communication
   */
  public int getNumStations() throws IOException {
    return getIntValue(Constants.netSysStat);
  }


  /**
   * Gets the maximum number of stations allowed in the ET system.
   *
   * @return maximum number of stations allowed in the ET system
   * @exception java.io.IOException
   *     if there are problems with network communication
   */
  public int getStationsMax() throws IOException {
    return getIntValue(Constants.netSysStatMax);
  }


  /**
   * Gets the number of attachments in the ET system.
   *
   * @return number of attachments in the ET system
   * @exception java.io.IOException
   *     if there are problems with network communication
   */
  public int getNumAttachments() throws IOException {
    return getIntValue(Constants.netSysAtt);
  }


  /**
   * Gets the maximum number of attachments allowed in the ET system.
   *
   * @return maximum number of attachments allowed in the ET system
   * @exception java.io.IOException
   *     if there are problems with network communication
   */
  public int getAttachmentsMax() throws IOException {
    return getIntValue(Constants.netSysAttMax);
  }

  /**
   * Gets the number of local processes in the ET system.
   * This is only relevant in C-language, Solaris systems.
   *
   * @return number of processes in the ET system
   * @exception java.io.IOException
   *     if there are problems with network communication
   */
  public int getNumProcesses() throws IOException {
    return getIntValue(Constants.netSysProc);
  }


  /**
   * Gets the maximum number of local processes allowed in the ET system.
   * This is only relevant in C-language, Solaris systems.
   *
   * @return maximum number of processes allowed in the ET system
   * @exception java.io.IOException
   *     if there are problems with network communication
   */
  public int getProcessesMax() throws IOException {
    return getIntValue(Constants.netSysProcMax);
  }


  /**
   * Gets the number of temp events in the ET system.
   * This is only relevant in C-language systems.
   *
   * @return number of temp events in the ET system
   * @exception java.io.IOException
   *     if there are problems with network communication
   */
  public int getNumTemps() throws IOException {
    return getIntValue(Constants.netSysTmp);
  }


  /**
   * Gets the maximum number of temp events allowed in the ET system.
   * This is only relevant in C-language systems.
   *
   * @return maximum number of temp events in the ET system
   * @exception java.io.IOException
   *     if there are problems with network communication
   */
  public int getTempsMax() throws IOException {
    return getIntValue(Constants.netSysTmpMax);
  }


  /**
   * Gets the ET system heartbeat. This is only relevant in
   * C-language systems.
   *
   * @return ET system heartbeat
   * @exception java.io.IOException
   *     if there are problems with network communication
   */
  public int getHeartbeat() throws IOException {
    return getIntValue(Constants.netSysHBeat);
  }


  /**
   * Gets the UNIX pid of the ET system process.
   * This is only relevant in C-language systems.
   *
   * @return UNIX pid of the ET system process
   * @exception java.io.IOException
   *     if there are problems with network communication
   */
  public int getPid() throws IOException {
    return getIntValue(Constants.netSysPid);
  }


  /** Gets the number of events in the ET system.
   *  @return number of events in the ET system */
  public int  getNumEvents() {return sys.numEvents;}
  /** Gets the "normal" event size in bytes.
   *  @return normal event size in bytes */
  public int  getEventSize() {return sys.eventSize;}
  /** Gets the ET system's implementation language.
   *  @return ET system's implementation language */
  public int  getLanguage()   {return sys.language;}
  /** Gets the ET system's host name.
   *  @return ET system's host name */
  public String  getHost()   {return sys.host;}
  /** Gets the tcp server port number.
   *  @return tcp server port number */
  public int  getTcpPort()   {return sys.tcpPort;}
  /** Gets the debug output level.
   *  @return debug output level */
  public int  getDebug()     {return debug;}
  /** Sets the debug output level. Must be either {@link Constants#debugNone},
   *  {@link Constants#debugSevere}, {@link Constants#debugError},
   *  {@link Constants#debugWarn}, or {@link Constants#debugInfo}.
   *  @param val debug level
   *  @exception org.jlab.coda.et.EtException
   *     if bad argument value
   */
  public void setDebug(int val) throws EtException {
    if ((val != Constants.debugNone)   &&
        (val != Constants.debugSevere) &&
        (val != Constants.debugError)  &&
        (val != Constants.debugWarn)   &&
        (val != Constants.debugInfo))    {
      throw new EtException("bad debug argument");
    }
    debug = val;
  }
  /** Gets a copy of the configuration used to specify how to open the ET
   *  system.
   *  @return SystemOpenConfig object used to specify how to open the ET
   *  system.
   */
  public SystemOpenConfig  getConfig() {return new SystemOpenConfig(openConfig);}



  /**
   * Gets all information about the ET system.
   *
   * @return object containing ET system information
   * @exception java.io.IOException
   *     if there are problems with network communication
   */
  synchronized public AllData getData() throws EtException, IOException {
    AllData data = new AllData();
    out.writeInt(Constants.netSysData);
    out.flush();

    // receive error
    int error = in.readInt();
    if (error != Constants.ok) {
      throw new EtException("error getting ET system data");
    }
    
    // receive incoming data
    int dataSize = in.readInt();

    // read everything at once (4X faster that way),
    // put it into a byte array, and read from that
    // byte[] bytes = new byte[dataSize];
    // ByteArrayInputStream bis = new ByteArrayInputStream(bytes);
    // DataInputStream dis2 = new DataInputStream(bis);
    // in.readFully(bytes);

    // system data
    // data.sysData.read(dis2);
    data.sysData.read(in);
//System.out.println("getData:  read in system data");

    // station data
    int count = in.readInt();
//System.out.println("getData:  # of stations = " + count);
    data.statData = new StationData[count];
    for (int i=0; i < count; i++) {
      data.statData[i] = new StationData();
      data.statData[i].read(in);
    }
//System.out.println("getData:  read in station data");

    // attachment data
    count = in.readInt();
//System.out.println("getData:  # of attachments = " + count);
    data.attData = new AttachmentData[count];
    for (int i=0; i < count; i++) {
      data.attData[i] = new AttachmentData();
      data.attData[i].read(in);
    }
//System.out.println("getData:  read in attachment data");

    // process data
    count = in.readInt();
//System.out.println("getData:  # of processes = " + count);
    data.procData = new ProcessData[count];
    for (int i=0; i < count; i++) {
      data.procData[i] = new ProcessData();
      data.procData[i].read(in);
    }
//System.out.println("getData:  read in process data");

    return data;
  }


  /**
   * Gets histogram containing data showing how many events in GRAND_CENTRAL's
   * input list when new events are requested by users. This feature is not
   * available on Java ET systems.
   *
   * @return integer array containing histogram
   * @exception java.io.IOException
   *     if there are problems with network communication
   */
  synchronized public int[] getHistogram() throws IOException, EtException {
    byte[] data = new byte[4*(sys.numEvents+1)];
    int[]  hist = new int[sys.numEvents+1];

    out.writeInt(Constants.netSysHist);
    out.flush();

    // receive error code
    if (in.readInt() != Constants.ok) {
      throw new EtException("cannot get histogram");
    }

    in.readFully(data);
    for (int i=0; i < sys.numEvents+1; i++) {
      hist[i] = bytesToInt(data, i*4);
    }
    return hist;
  }
}

