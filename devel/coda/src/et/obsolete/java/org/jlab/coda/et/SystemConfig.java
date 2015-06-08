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
  /** Number of events in each group. Used with mulitple producers who want to
   * guarantee available events for each producer. */
  int[]    groups;
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
   *  ET system. */
  int      multicastPort;
  /** Set of all multicast addresses to listen on (in String form). */
  HashSet<InetAddress>  multicastAddrs;

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
    multicastAddrs  = new HashSet<InetAddress>(10);
    // by default there is one group with all events in it
    groups          = new int[1];
    groups[0]       = numEvents;
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
    multicastAddrs  = new HashSet<InetAddress>(config.multicastAddrs);
    groups          = config.groups.clone();
  }

  // public gets

  /** Gets the total number of events.
   *  @return total number of events */
  public int getNumEvents()      {return numEvents;}
  /** Gets the size of the normal events in bytes.
   *  @return size of normal events in bytes */
  public int getEventSize()      {return eventSize;}
  /** Gets the array of how many events in each group.
   *  @return array of how many events in each group */
  public int[] getGroups()      {return (int []) groups.clone();}
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
  public Set<InetAddress> getMulticastAddrs() {return new HashSet<InetAddress>(multicastAddrs);}

  /** Gets the multicast addresses as a String array.
   *  @return multicast addresses as a String array */
  public String[] getMulticastStrings() {
    if (multicastAddrs == null) {
      return null;
    }
    int index = 0;
    String[] addrs = new String[multicastAddrs.size()];
    for (InetAddress addr : multicastAddrs) {
      addrs[index++] = addr.getHostAddress();
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

    if (!addr.isMulticastAddress()) {
      throw new EtException("not a multicast address");
    }
    multicastAddrs.add(addr);
    return;
  }

  /** Removes a multicast address from the set.
   *  @param addr multicast address
   */
  public void removeMulticastAddr(String addr) {
      InetAddress ad;
      try {ad = InetAddress.getByName(addr);}
      catch (UnknownHostException ex) {
        return;
      }
      multicastAddrs.remove(ad);
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
    if (groups.length ==1) groups[0] = num;
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

  /** Sets the number of events in each group. Used with mulitple producers who want to
   * guarantee available events for each producer.
   *
   *  @param groups array defining number of events in each group
   *  @exception org.jlab.coda.et.EtException
   *     if the groups array has length < 1 or values are not positive ints
   */
  public void setGroups(int[] groups) throws EtException {
      if (groups.length < 1) {
          throw new EtException("events must have at least one group");
      }
      for (int num : groups) {
          if (num < 1) {
              throw new EtException("each event group must contain at least one event");              
          }
      }

      this.groups = groups.clone();
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

    /**
     * Checks configuration settings for consistency.
     * @return true if consistent, else false
     */
    boolean selfConsistent() {
        // Check to see if the number of events in groups equal the total number of events
        int count = 0;
        for (int i : groups) {
            count += i;
        }
        if (count != numEvents) {
            System.out.println("events in groups != total event number");
            return false;
        }
        return true;
    }
}

