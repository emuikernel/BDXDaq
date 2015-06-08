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
 * This class defines parameters used to open an ET system.
 *
 * @author Carl Timmer
 */

public class SystemOpenConfig {

  /** ET system name. */
  String   name;
  /** Either ET system host name or destination of broadcasts and multicasts. */
  String   host;
  /** Are we broadcasting on all local subnets to find ET system? */
  boolean broadcasting;
  /** Broadcast address. */
  static public final String broadcastIP = "255.255.255.255";
  /** Multicast addresses. */
  HashSet<String>  multicastAddrs;
  /**
   * Means used to contact an ET system. The possible values are
   *  {@link Constants#broadcast} for broadcasting, {@link Constants#multicast}
   *  for multicasting, {@link Constants#direct} for connecting directly to the
   *  ET tcp server, and {@link Constants#broadAndMulticast} for using both
   *  broadcasting and multicasting.
   */
  int  contactMethod;
  /** UDP port number for broadcasting or sending udp packets to known hosts. */
  int  udpPort;
  /** TCP server port number of the ET system. */
  int  tcpPort;
  /** Port number to multicast to. In Java, a multicast socket cannot have same
   *  port number as another datagram socket. */
  int  multicastPort;
  /** Time-to_live value for multicasting. */
  int  ttl;
  /**
   *  Policy on what to do about multiple responding ET systems to a broadcast
   *  or multicast. The possible values are {@link Constants#policyFirst} which
   *  chooses the first ET system to respond, {@link Constants#policyLocal}
   *  which chooses the first local ET system to respond and if none then the
   *  first response, or {@link Constants#policyError} which throws an
   *  EtTooManyException exception.
   */
  int  responsePolicy;



  /**
   * Most general constructor for creating a new SystemOpenConfig object.
   *
   * @param etName ET system name
   * @param hostName ET system host name or destination of broad/multicasts
   * @param broadcasting do we UDP broadcast to find ET system?
   * @param mAddrs Collection of multicast addresses (as Strings)
   * @param method Means used to contact an ET system
   * @param tPort TCP server port number of the ET system
   * @param uPort UDP port number for broadcasting or sending udp packets to known hosts
   * @param mPort Port number to multicast to
   * @param ttlNum Time-to_live value for multicasting
   * @param policy Policy on what to do about multiple responding ET systems to
   *               a broadcast or multicast
   *
   * @exception org.jlab.coda.et.EtException
   *     if method is not {@link Constants#broadcast},
   *     {@link Constants#multicast}, {@link Constants#direct}, or
   *     {@link Constants#broadAndMulticast}.
   * @exception org.jlab.coda.et.EtException
   *     if method is not direct and no broad/multicast addresses were
   *     specified, or if method is direct and no actual host name was
   *     specified.
   * @exception org.jlab.coda.et.EtException
   *     if port numbers are < 1024 or > 65535
   * @exception org.jlab.coda.et.EtException
   *     if ttl is < 0 or > 254
   * @exception org.jlab.coda.et.EtException
   *     if policy is not {@link Constants#policyFirst},
   *     {@link Constants#policyLocal}, or {@link Constants#policyError}
   */
  public SystemOpenConfig (String etName, String hostName,
                           boolean broadcasting, Collection<String> mAddrs,
			               int method, int tPort, int uPort, int mPort,
			               int ttlNum, int policy)
			   throws EtException {

    name = etName;
    if (etName == null || etName.equals("")) {
      throw new EtException("Bad ET system name");
    }
    
    host = hostName;
    if (host == null || host.equals("")) {
        if (method != Constants.broadcast) {
            throw new EtException("Bad host or location name");
        }
    }

    this.broadcasting = broadcasting;

    boolean noMulticastAddrs = true;
    if ((mAddrs == null) || (mAddrs.size() < 1)) {
      multicastAddrs = new HashSet<String>(10);
    }
    else {
      multicastAddrs = new HashSet<String>(mAddrs);
      noMulticastAddrs = false;
    }


    if ((method != Constants.multicast) &&
        (method != Constants.broadcast) &&
        (method != Constants.broadAndMulticast) &&
        (method != Constants.direct))     {
      throw new EtException("Bad contact method value");
    }
    else {
      contactMethod = method;
    }


    if (contactMethod == Constants.direct) {
        if (host.equals(Constants.hostRemote) ||
	        host.equals(Constants.hostAnywhere)) {
          throw new EtException("Need to specify an actual host name");
	    }
    }
    else if ( ((contactMethod == Constants.multicast) ||
               (contactMethod == Constants.broadAndMulticast))
            && noMulticastAddrs) {
      throw new EtException("Need to specify a multicast address");
    }


    if ((uPort < 1024) || (uPort > 65535)) {
      throw new EtException("Bad UDP port value");
    }
    udpPort = uPort;

    if ((tPort < 1024) || (tPort > 65535)) {
      throw new EtException("Bad TCP port value");
    }
    tcpPort = tPort;

    if ((mPort < 1024) || (mPort > 65535)) {
      throw new EtException("Bad multicast port value");
    }
    multicastPort = mPort;


    if ((ttlNum < 0) || (ttlNum > 254)) {
      throw new EtException("Bad TTL value");
    }
    ttl = ttlNum;


    if ((policy != Constants.policyFirst) &&
        (policy != Constants.policyLocal) &&
        (policy != Constants.policyError))  {
      throw new EtException("Bad policy value");
    }
    
    if ((host.equals(Constants.hostRemote)) &&
        (policy == Constants.policyLocal)) {
      // stupid combination of settings
      throw new EtException("Policy value cannot be local if host is remote");
    }
    responsePolicy = policy;
  }

    /**
     * Constructor for broadcasting. First responder is chosen.
     *
     * @param etName ET system name
     * @param destination  destination of broadcasts
     *
     * @exception org.jlab.coda.et.EtException
     *     if no broadcast addresses were specified
     * @exception org.jlab.coda.et.EtException
     *     if port number is < 1024 or > 65535
     */
    public SystemOpenConfig (String etName, String destination)
                 throws EtException {
      this (etName, destination, true, null, Constants.broadcast,
            Constants.serverPort, Constants.broadcastPort, Constants.multicastPort,
            Constants.multicastTTL, Constants.policyFirst);
    }


    /**
     * Constructor for broadcasting. First responder is chosen.
     *
     * @param etName ET system name
     * @param uPort UDP port number to broadcast to
     * @param destination  destination of broadcasts
     *
     * @exception org.jlab.coda.et.EtException
     *     if no broadcast addresses were specified
     * @exception org.jlab.coda.et.EtException
     *     if port number is < 1024 or > 65535
     */
    public SystemOpenConfig (String etName, int uPort, String destination)
                 throws EtException {
      this (etName, destination, true, null, Constants.broadcast,
            Constants.serverPort, uPort, Constants.multicastPort,
            Constants.multicastTTL, Constants.policyFirst);
    }


  /**
   * Constructor for multicasting. First responder is chosen.
   *
   * @param etName ET system name
   * @param hostName ET system host name or destination of multicasts
   * @param mAddrs Collection of multicast addresses (as Strings)
   * @param mPort Port number to multicast to
   * @param ttlNum Time-to_live value for multicasting
   *
   * @exception org.jlab.coda.et.EtException
   *     if no multicast addresses were specified
   * @exception org.jlab.coda.et.EtException
   *     if port number is < 1024 or > 65535, or ttl is < 0 or > 254
   */
  public SystemOpenConfig (String etName, String hostName,
                           Collection<String> mAddrs, int mPort, int ttlNum)
			   throws EtException {
    this (etName, hostName, false, mAddrs, Constants.multicast,
          Constants.serverPort, Constants.broadcastPort, mPort,
          ttlNum, Constants.policyFirst);
  }


  /**
   * Constructor for multicasting. First responder is chosen.
   *
   * @param etName ET system name
   * @param hostName ET system host name or destination of multicasts
   * @param mAddrs Collection of multicast addresses (as Strings)
   * @param uPort Port number to send direct udp packet to
   * @param mPort Port number to multicast to
   * @param ttlNum Time-to_live value for multicasting
   *
   * @exception org.jlab.coda.et.EtException
   *     if no multicast addresses were specified
   * @exception org.jlab.coda.et.EtException
   *     if port numbers are < 1024 or > 65535, or ttl is < 0 or > 254
   */
  public SystemOpenConfig (String etName, String hostName,
                           Collection<String> mAddrs, int uPort, int mPort, int ttlNum)
			   throws EtException {
    this (etName, hostName, false, mAddrs, Constants.multicast,
          Constants.serverPort, uPort, mPort,
          ttlNum, Constants.policyFirst);
  }


  /**
   * Constructor for connecting directly to tcp server. First responder is
   * chosen.
   *
   * @param etName ET system name
   * @param hostName ET system host name
   * @param tPort TCP server port number of the ET system
   *
   * @exception org.jlab.coda.et.EtException
   *     if no actual host name was specified.
   * @exception org.jlab.coda.et.EtException
   *     if port number is < 1024 or > 65535
   */
  public SystemOpenConfig (String etName, String hostName, int tPort)
			   throws EtException {
      this (etName, hostName, false, null, Constants.direct,
          tPort, Constants.broadcastPort, Constants.multicastPort,
	      Constants.multicastTTL, Constants.policyFirst);
  }

  /** Constructor to create a new SystemOpenConfig object from another. */
  public SystemOpenConfig (SystemOpenConfig config) {
    name           = config.name;
    host           = config.host;
    multicastAddrs = config.getMulticastAddrs();
    contactMethod  = config.contactMethod;
    udpPort        = config.udpPort;
    tcpPort        = config.tcpPort;
    multicastPort  = config.multicastPort;
    ttl            = config.ttl;
    responsePolicy = config.responsePolicy;
  }

  // Get methods

  /** Gets the ET system name.
   *  @return ET system name */
  public String   getEtName()            {return name;}
  /** Gets the ET system host name or broad/multicast destination.
   *  @return ET system host name or broad/multicast destination */
  public String   getHost()              {return host;}
  /** Gets multicast addresses (as set of Strings).
   *  @return multicast addresses (as set of Strings) */
  public HashSet<String>  getMulticastAddrs()    {return (HashSet<String>) multicastAddrs.clone();}
  /** Gets the means used to contact an ET system.
   *  @return means used to contact an ET system */
  public int      getContactMethod()     {return contactMethod;}
  /** Gets policy on what to do about multiple responding ET systems to a
   *  broadcast or multicast.
   *  @return policy on what to do about multiple responding ET systems to
   *  a broadcast or multicast */
  public int      getResponsePolicy()    {return responsePolicy;}
  /** Gets UDP port number for broadcasting or sending udp packets to known hosts.
   *  @return UDP port number for broadcast or sending udp packets to known hosts */
  public int      getUdpPort()           {return udpPort;}
  /** Gets TCP server port number of the ET system.
   *  @return TCP server port number of the ET system */
  public int      getTcpPort()           {return tcpPort;}
  /** Gets port number to multicast to.
   *  @return port number to multicast to */
  public int      getMulticastPort()     {return multicastPort;}
  /** Gets time-to_live value for multicasting.
   *  @return time-to_live value for multicasting */
  public int      getTTL()               {return ttl;}
  /** Gets the number of multicast addresses.
   *  @return the number of multicast addresses */
  public int      getNumMulticastAddrs() {return multicastAddrs.size();}

  /** Are we listening for broadcasts?
   *  @return boolean indicating wether we are listening for broadcasts */
  public boolean isBroadcasting() {return broadcasting;}
  /** Set true if we're listening for broadcasts. */
  public void broadcasting(boolean on) {broadcasting = on;}


  // Set methods

  /** Sets the ET system name.
   *  @param etName ET system name  */
  public void setEtName(String etName)            {name = etName;}
  /** Sets the ET system host name or broad/multicast destination.
   *  @param hostName system host name or broad/multicast destination */
  public void setHost(String hostName)            {host = hostName;}
  /** Removes a multicast address from the set.
   *  @param addr multicast address to be removed */
  public void removeMulticastAddr(String addr)    {multicastAddrs.remove(addr);}


  /**
   *  Adds a multicast address to the set.
   *
   *  @param addr multicast address to be added
   *  @exception org.jlab.coda.et.EtException
   *     if the address is not a multicast address
   */
  public void addMulticastAddr(String addr) throws EtException {
    InetAddress inetAddr;
    try {inetAddr = InetAddress.getByName(addr);}
    catch (UnknownHostException ex) {
      throw new EtException("not a multicast address");
    }

    if (!inetAddr.isMulticastAddress()) {
      throw new EtException("not a multicast address");
    }
    multicastAddrs.add(addr);
  }


  /**
   *  Adds a collection of multicast addresses to the set.
   *
   *  @param addrs collection of multicast addresses to be added (as Strings)
   *  @exception org.jlab.coda.et.EtException
   *     if one of the addresses is not a multicast address
   */
  public void setMulticastAddrs(Collection<String> addrs) throws EtException {
    InetAddress inetAddr;
    for (String addr : addrs) {
      try {inetAddr = InetAddress.getByName(addr);}
      catch (UnknownHostException ex) {
        throw new EtException("not a broadcast address");
      }
      if (!inetAddr.isMulticastAddress()) {
        throw new EtException(addr + " is not a multicast address");
      }
    }
    multicastAddrs = new HashSet<String>(addrs);
  }

  /**
   *  Sets the means or method of contacting an ET system. Its values may be
   *  {@link Constants#broadcast} for broadcasting, {@link Constants#multicast}
   *  for multicasting, {@link Constants#direct} for connecting directly to the
   *  ET tcp server, and {@link Constants#broadAndMulticast} for using both
   *  broadcasting and multicasting.
   *
   *  @param method means or method of contacting an ET system
   *  @exception org.jlab.coda.et.EtException
   *     if the argument has a bad value
   */
  public void setContactMethod(int method) throws EtException {
    if ((method != Constants.multicast) &&
        (method != Constants.broadcast) &&
        (method != Constants.broadAndMulticast) &&
        (method != Constants.direct))     {
      throw new EtException("bad contact method value");
    }
    contactMethod = method;
  }

  /**
   *  Sets the policy on what to do about multiple responding ET systems to a
   *  broadcast or multicast. The possible values are
   *  {@link Constants#policyFirst} which chooses the first ET system to
   *  respond, {@link Constants#policyLocal} which chooses the first local ET
   *  system to respond and if none then the first response, or
   *  {@link Constants#policyError} which throws an EtTooManyException
   *  exception.
   *
   *  @param policy policy on what to do about multiple responding ET systems
   *  @exception org.jlab.coda.et.EtException
   *     if the argument has a bad value or if the policy says to choose a local
   *     ET system but the host is set to chose a remote system.
   */
  public void setResponsePolicy(int policy) throws EtException {
    if ((policy != Constants.policyFirst) &&
        (policy != Constants.policyLocal) &&
        (policy != Constants.policyError))  {
      throw new EtException("bad policy value");
    }
    if ((host.equals(Constants.hostRemote)) &&
        (policy == Constants.policyLocal)) {
      // stupid combination of settings
      throw new EtException("policy value cannot be local if host is remote");
    }
    responsePolicy = policy;
  }

  /**
   *  Sets the UDP port number for broadcastiong and sending udp packets to known hosts.
   *
   *  @param port UDP port number for broadcasting and sending udp packets to known hosts
   *  @exception org.jlab.coda.et.EtException
   *     if the port number is < 1024 or > 65535
   */
  public void setUdpPort(int port) throws EtException {
    if ((port < 1024) || (port > 65535)) {
      throw new EtException("bad UDP port value");
    }
    udpPort = port;
  }

  /**
   *  Sets the TCP server port number of the ET system.
   *
   *  @param port TCP server port number of the ET system
   *  @exception org.jlab.coda.et.EtException
   *     if the port number is < 1024 or > 65535
   */
  public void setTcpPort(int port) throws EtException {
    if ((port < 1024) || (port > 65535)) {
      throw new EtException("bad TCP port value");
    }
    tcpPort = port;
  }

  /**
   *  Sets the port number to multicast to.
   *
   *  @param port port number to multicast to
   *  @exception org.jlab.coda.et.EtException
   *     if the port number is < 1024 or > 65535
   */
  public void setMulticastPort(int port) throws EtException {
    if ((port < 1024) || (port > 65535)) {
      throw new EtException("bad multicast port value");
    }
    multicastPort = port;
  }

  /**
   *  Sets the Time-to_live value for multicasting.
   *
   *  @param ttlNum time-to_live value for multicasting
   *  @exception org.jlab.coda.et.EtException
   *     if the port number is < 0 or > 254
   */
  public void setTTL(int ttlNum) throws EtException {
    if ((ttlNum < 0) || (ttlNum > 254)) {
      throw new EtException("bad TTL value");
    }
    ttl = ttlNum;
  }
}
