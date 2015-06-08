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
import java.net.*;

/**
 * This class defines a configuration for the creation of an ET system.
 *
 * @author Carl Timmer
 */

public class SystemConfig {

  /** Total number of events. */
  int      numEvents;
  /** Size of the "normal" event in bytes. This is the memory allocated to each
   *  event upon starting up the ET system. */
  int      eventSize;
  /** Maximum number of station. */
  int      stationsMax;
  /** Maximum number of attachments. */
  int      attachmentsMax;
  /**
   *  Debug level. This may have values of {@link Constants#debugNone} meaning
   *  print nothing, {@link Constants#debugSevere} meaning print only the
   *  severest errors, {@link Constants#debugError} meaning print all errors,
   *  {@link Constants#debugWarn} meaning print all errors and warnings, and
   *  finally {@link Constants#debugInfo} meaning print all errors, warnings,
   *  and informative messages.
   */
  int      debug;
  /** UDP port number for thread responding to users' broadcasts looking for the
   *  ET system. */
  int      udpPort;
  /** TCP port number for the thread establishing connections with users,
   *  otherwise referred to as the ET server thread. */
  int      serverPort;
  /** UDP port number for thread responding to users' multicasts looking for the
   *  ET system. In Java, a multicast socket cannot have same port number as
   *  another datagram socket. */
  int      multicastPort;
  /** Set of all multicast addresses to listen on (in String form). */
  HashSet  multicastAddrs;
  /** Set of all local broadcast addresses to listen on (in String form). */
  HashSet  broadcastAddrs;


  /**
   * Creates a new SystemConfig object using default parameters.
   * The default parameters are:
   *      number of events          = {@link Constants#defaultNumEvents},
   *      event size                = {@link Constants#defaultEventSize},
   *      max number of stations    = {@link Constants#defaultStationsMax},
   *      max number of attachments = {@link Constants#defaultAttsMax},
   *      debug level               = {@link Constants#debugError},
   *      udp port                  = {@link Constants#broadcastPort},
   *      server (tcp) port         = {@link Constants#serverPort}, and
   *      multicasting port         = {@link Constants#multicastPort}.
   */
  public SystemConfig () {
    numEvents       = Constants.defaultNumEvents;
    eventSize       = Constants.defaultEventSize;
    stationsMax     = Constants.defaultStationsMax;
    attachmentsMax  = Constants.defaultAttsMax;
    debug           = Constants.debugError;
    udpPort         = Constants.broadcastPort;
    serverPort      = Constants.serverPort;
    multicastPort   = Constants.multicastPort;
    multicastAddrs  = new HashSet(10);
    broadcastAddrs  = new HashSet(10);
  }

  /** Creates a new SystemConfig object from an existing one. */
  public SystemConfig (SystemConfig config) {
    numEvents       = config.numEvents;
    eventSize       = config.eventSize;
    stationsMax     = config.stationsMax;
    attachmentsMax  = config.attachmentsMax;
    debug           = config.debug;
    udpPort         = config.udpPort;
    serverPort      = config.serverPort;
    multicastPort   = config.multicastPort;
    multicastAddrs  = new HashSet(config.multicastAddrs);
    broadcastAddrs  = new HashSet(config.broadcastAddrs);
  }

  // public gets

  /** Gets the total number of events.
   *  @return total number of events */
  public int getNumEvents()      {return numEvents;}
  /** Gets the size of the normal events in bytes.
   *  @return size of normal events in bytes */
  public int getEventSize()      {return eventSize;}
  /** Gets the maximum number of stations.
   *  @return maximum number of stations */
  public int getStationsMax()    {return stationsMax;}
  /** Gets the maximum number of attachments.
   *  @return maximum number of attachments */
  public int getAttachmentsMax() {return attachmentsMax;}
  /** Gets the debug level.
   *  @return debug level */
  public int getDebug()          {return debug;}
  /** Gets the udp port number.
   *  @return udp port number */
  public int getUdpPort()        {return udpPort;}
  /** Gets the tcp server port number.
   *  @return tcp server port number */
  public int getServerPort()     {return serverPort;}
  /** Gets the multicast port number.
   *  @return multicast port number */
  public int getMulticastPort()  {return multicastPort;}
  /** Gets the set of multicast addresses.
   *  @return set of multicast addresses */
  public Set getMulticastAddrs() {return new HashSet(multicastAddrs);}
  /** Gets the set of broadcast addresses.
   *  @return set of broadcast addresses */
  public Set getBroadcastAddrs() {return new HashSet(broadcastAddrs);}

  /** Gets the multicast addresses as a String array.
   *  @return multicast addresses as a String array */
  public String[] getMulticastStrings() {
    if (multicastAddrs == null) {
      return null;
    }
    int index = 0;
    String[] addrs = new String[multicastAddrs.size()];
    for (Iterator i = multicastAddrs.iterator(); i.hasNext(); ) {
      addrs[index++] = ((InetAddress) i.next()).getHostAddress();
    }
    return addrs;
  }

  /** Gets the broadcast addresses as a String array.
   *  @return broadcast addresses as a String array */
  public String[] getBroadcastStrings() {
    if (broadcastAddrs == null) {
      return null;
    }
    int index = 0;
    String[] addrs = new String[broadcastAddrs.size()];
    for (Iterator i = broadcastAddrs.iterator(); i.hasNext(); ) {
      addrs[index++] = ((InetAddress) i.next()).getHostAddress();
    }
    return addrs;
  }


  // public adds, removes

  /** Adds a multicast address to the set.
   *  @param mCastAddr multicast address
   *  @exception org.jlab.coda.et.EtException
   *     if the argument is not a multicast address
   */
  public void addMulticastAddr(String mCastAddr) throws EtException {
    InetAddress addr;
    try {addr = InetAddress.getByName(mCastAddr);}
    catch (UnknownHostException ex) {
      throw new EtException("not a multicast address");
    }

    if (addr.isMulticastAddress() == false) {
      throw new EtException("not a multicast address");
    }
    multicastAddrs.add(addr);
    return;
  }

  /** Adds a broadcast address to the set.
   *  @param bCastAddr broadcast address
   *  @exception org.jlab.coda.et.EtException
   *     if the argument is not a broadcast address
   */
  public void addBroadcastAddr(String bCastAddr) throws EtException {
    InetAddress addr;
    try {addr = InetAddress.getByName(bCastAddr);}
    catch (UnknownHostException ex) {
      throw new EtException("not a broadcast address");
    }

    // There is no way in Java to find or check broadcast addresses.
    // Programming such requires native methods.
    broadcastAddrs.add(addr);
    return;
  }

  /** Removes a multicast address from the set.
   *  @param addr multicast address
   */
  public void removeMulticastAddr(String addr) {
    multicastAddrs.remove(addr);
    return;
  }

  /** Removes a broadcast address from the set.
   *  @param addr broadcast address
   */
  public void removeBroadcastAddr(String addr) {
    multicastAddrs.remove(addr);
    return;
  }


  // public sets

  /** Sets the total number of events.
   *  @param num total number of events
   *  @exception org.jlab.coda.et.EtException
   *     if the argument is less than 1
   */
  public void setNumEvents(int num) throws EtException {
    if (num < 1) {
      throw new EtException("must have 1 or more events");
    }
    numEvents = num;
  }

  /** Sets the event size in bytes.
   *  @param size event size in bytes
   *  @exception org.jlab.coda.et.EtException
   *     if the argument is less than 1 byte
   */
  public void setEventSize(int size) throws EtException {
    if (size < 1) {
      throw new EtException("events must have at least one byte");
    }
    eventSize = size;
  }

  /** Sets the maximum number of stations.
   *  @param num maximum number of stations
   *  @exception org.jlab.coda.et.EtException
   *     if the argument is less than 2
   */
  public void setStationsMax(int num) throws EtException {
    if (num < 2) {
      throw new EtException("must have at least 2 stations");
    }
    stationsMax = num;
  }

  /** Sets the maximum number of attachments.
   *  @param num maximum number of attachments
   *  @exception org.jlab.coda.et.EtException
   *     if the argument is less than 1
   */
  public void setAttachmentsMax(int num) throws EtException {
    if (num < 1) {
      throw new EtException("must be able to have at least one attachment");
    }
    attachmentsMax = num;
  }

  /** Sets the debug level.
   *  @param level debug level
   *  @exception org.jlab.coda.et.EtException
   *     if the argument has a bad value
   */
  public void setDebug(int level) throws EtException {
    if ((level != Constants.debugNone)   &&
	(level != Constants.debugInfo)   &&
	(level != Constants.debugWarn)   &&
	(level != Constants.debugError)  &&
	(level != Constants.debugSevere))  {
      throw new EtException("bad debug value");
    }
    debug = level;
  }

  /** Sets the udp port number.
   *  @param port udp port number
   *  @exception org.jlab.coda.et.EtException
   *     if the argument is less than 1024
   */
  public void setUdpPort(int port) throws EtException {
    if (port < 1024) {
      throw new EtException("port number must be greater than 1023");
    }
    udpPort = port;
  }

  /** Sets the tcp server port number.
   *  @param port tcp server port number
   *  @exception org.jlab.coda.et.EtException
   *     if the argument is less than 1024
   */
  public void setServerPort(int port) throws EtException {
    if (port < 1024) {
      throw new EtException("port number must be greater than 1023");
    }
    serverPort = port;
  }

  /** Sets the multicast port number.
   *  @param port multicast port number
   *  @exception org.jlab.coda.et.EtException
   *     if the argument is less than 1024
   */
  public void setMulticastPort(int port) throws EtException {
    if (port < 1024) {
      throw new EtException("port number must be greater than 1023");
    }
//    else if (port == udpPort) {
//      throw new EtException("multicast port must be different than udp port");
//    }
    multicastPort = port;
  }


}

