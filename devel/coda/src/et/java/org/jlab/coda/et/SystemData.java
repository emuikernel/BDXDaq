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
import java.io.*;

/**
 * This class holds all system level information about an ET system. It parses
 * the information from a stream of data sent by an ET system.
 *
 * @author Carl Timmer
 */

public class SystemData {

  // values which can change

  /** Flag which specifying whether the ET system is alive. A value of 1 means
   *  alive and 0 means dead. */
  int alive;
  /** Heartbeat count of the ET system process. It is not relevant in Java ET
   *  systems. */
  int heartbeat;
  /** Count of the current amount of temporary events. It is not relevant in
   *  Java ET systems. */
  int temps;
  /** Count of the current number of stations in the linked list (are either
   *  active or idle).
   *  @see SystemCreate#stations */
  int stations;
  /** Count of the current number of attachments.
   *  @see SystemCreate#attachments */
  int attachments;
  /** Count of the current number of processes. It is not relevant in Java ET
   *  systems. */
  int processes;
  /** Number of events owned by the system (as opposed to attachments). */
  int eventsOwned;

  /** System mutex status. It has the value {@link Constants#mutexLocked} if
   *  locked and {@link Constants#mutexUnlocked} otherwise. This is only
   *  relevant in C ET systems, since in Java, mutexes cannot be tested without
   *  possibility of blocking. This is not boolean for C ET system compatibility.
   *  @see SystemCreate#systemLock. */
  int mutex;
  /** Station mutex status. It has the value {@link Constants#mutexLocked} if
   *  locked and {@link Constants#mutexUnlocked} otherwise. This is only
   *  relevant in C ET systems, since in Java, mutexes cannot be tested without
   *  possibility of blocking. This is not boolean for C ET system compatibility.
   *  @see SystemCreate#stationLock. */
  int statMutex;
  /** Add-station mutex status. It has the value {@link Constants#mutexLocked}
   *  if locked and {@link Constants#mutexUnlocked} otherwise. This is only
   *  relevant in C ET systems as this mutex is not used in Java systems. */
  int statAddMutex;

  // values which do NOT change

  /** Endian of host running the ET system. This can have values of either
   *  {@link Constants#endianBig} or {@link Constants#endianLittle}.
   */
  int endian;
  /** Flag specifying whether the operating system can share mutexes between
   *  processes. It has the value {@link Constants#mutexShare} if they can be
   *  shared and {@link Constants#mutexNoShare} otherwise. This is not
   *  relevant in Java ET systems. */
  int share;
  /** Unix pid of the ET system process. This is not relevant for Java ET
   *  systems, and C based ET systems on Linux may have several pids. */
  int mainPid;
  /** The number of ints in a station's select array.
   *  @see Constants#stationSelectInts */
  int selects;
  /** Total number of events in a system.
   *  @see SystemConfig#numEvents
   *  @see SystemCreate#events */
  int events;
  /** Size of "normal" events in bytes.
   *  @see SystemConfig#eventSize  */
  int eventSize;
  /** Maximum number of temporary events allowed in the ET system.  This is not
   *  relevant in Java ET systems. */
  int tempsMax;
  /** Maximum number of station allowed in the ET system.
   *  @see SystemConfig#stationsMax */
  int stationsMax;
  /** Maximum number of attachments allowed in the ET system.
   *  @see SystemConfig#attachmentsMax */
  int attachmentsMax;	// max # of attachments allowed
  /** Maximum number of processes allowed in the ET system. This is not
   *  relevant in Java ET systems. */
  int processesMax;

  /** Port number of the ET TCP server.
   *  @see SystemConfig#serverPort */
  int tcpPort;
  /** Port number of the ET UDP broadcast listening thread.
   *  @see SystemConfig#udpPort */
  int udpPort;
  /** Port number of the ET UDP multicast listening thread.
   *  @see SystemConfig#multicastPort */
  int multicastPort;

  /** Number of network interfaces on the host computer. */
  int interfaceCount;
  /** Number of multicast addresses the UDP server listens on. */
  int multicastCount;

  /** Dotted-decimal IP addresses of network interfaces on the host. */
  String interfaceAddresses[];
  /** Dotted-decimal multicast addresses the UDP server listens on.
   *  @see SystemConfig#getMulticastStrings
   *  @see SystemConfig#getMulticastAddrs
   *  @see SystemConfig#addMulticastAddr
   *  @see SystemConfig#removeMulticastAddr */
  String multicastAddresses[];
  /** The ET system (file) name.
   *  @see SystemCreate#SystemCreate
   *  @see SystemCreate#name */
  String etName;


  // Get methods

  /** Specifies whether the ET system is alive. */
  public boolean alive() {if (alive == 1) return true; return false;};
  /** Get the heartbeat count of the ET system process. It is not relevant
   *  in Java ET systems.*/
  public int getHeartbeat() {return heartbeat;}
  /** Get the current number of temporary events. */
  public int getTemps() {return temps;}
  /** Get the current number of stations in the linked list
   * (either active or idle). */
  public int getStations() {return stations;}
  /** Get the current number of attachments. */
  public int getAttachments() {return attachments;}
  /** Get the current number of processes. It is not relevant in Java ET
   *  systems.*/
  public int getProcesses() {return processes;}
  /** Get the number of events owned by the system (not by attachments). */
  public int getEventsOwned() {return eventsOwned;}

  /** Get the system mutex status. It has the value {@link Constants#mutexLocked}
   *  if locked and {@link Constants#mutexUnlocked} otherwise. This is only
   *  relevant in C ET systems as this mutex is not used in Java systems. */
  public int getMutex() {return mutex;}
  /** Get the station mutex status. It has the value {@link Constants#mutexLocked}
   *  if locked and {@link Constants#mutexUnlocked} otherwise. This is only
   *  relevant in C ET systems as this mutex is not used in Java systems. */
  public int getStatMutex() {return statMutex;}
  /** Get the add-station mutex status. It has the value {@link Constants#mutexLocked}
   *  if locked and {@link Constants#mutexUnlocked} otherwise. This is only
   *  relevant in C ET systems as this mutex is not used in Java systems. */
  public int getStatAddMutex() {return statAddMutex;}


  /** Get the endian value of the host running the ET system.  This can
   *  have values of either {@link Constants#endianBig} or
   *  {@link Constants#endianLittle}. */
  public int getEndian() {return endian;}
  /** Get the value specifying whether the operating system can share
   *  mutexes between processes. It has the value {@link Constants#mutexShare}
   *  if they can be shared and {@link Constants#mutexNoShare} otherwise.
   *  It is not relevant in Java ET systems.*/
  public int getShare() {return share;}
  /** Get the Unix pid of the ET system process. Java ET  systems return
   *  -1, and C based ET systems on Linux may have several, additional pids
   *  not given here. */
  public int getMainPid() {return mainPid;}
  /** Get the number of ints in a station's select array.
   *  @see Constants#stationSelectInts */
  public int getSelects() {return selects;}
  /** Get the total number of events in a system.
   *  @see SystemConfig#numEvents
   *  @see SystemCreate#events */
  public int getEvents() {return events;}
  /** Get the size of "normal" events in bytes.
   *  @see SystemConfig#eventSize  */
  public int getEventSize() {return eventSize;}
  /** Get the maximum number of temporary events allowed in the ET system.
   *  This is not relevant in Java ET systems. */
  public int getTempsMax() {return tempsMax;}
  /** Get the maximum number of station allowed in the ET system.
   *  @see SystemConfig#stationsMax */
  public int getStationsMax() {return stationsMax;}
  /** Get the maximum number of attachments allowed in the ET system.
   *  @see SystemConfig#attachmentsMax */
  public int getAttachmentsMax() {return attachmentsMax;}
  /** Get the maximum number of processes allowed in the ET system.
   *  This is not relevant in Java ET systems. */
  public int getProcessesMax() {return processesMax;}

  /** Get the port number of the ET TCP server.
   *  @see SystemConfig#serverPort */
  public int getTcpPort() {return tcpPort;}
  /** Get the port number of the ET UDP broadcast listening thread.
   *  @see SystemConfig#udpPort */
  public int getUdpPort() {return udpPort;}
  /** Get the port number of the ET UDP multicast listening thread.
   *  @see SystemConfig#multicastPort */
  public int getMulticastPort() {return multicastPort;}

  /** Get the number of network interfaces on the host computer. */
  public int getInterfaces() {return interfaceCount;}
  /** Get the number of multicast addresses the UDP server listens on. */
  public int getMulticasts() {return multicastCount;}

  /** Get the dotted-decimal IP addresses of network interfaces on the host. */
  public String[] getInterfaceAddresses() {return (String[]) interfaceAddresses.clone();}
  /** Get the dotted-decimal multicast addresses the UDP server listens on.
   *  @see SystemConfig#getMulticastStrings
   *  @see SystemConfig#getMulticastAddrs
   *  @see SystemConfig#addMulticastAddr
   *  @see SystemConfig#removeMulticastAddr */
  public String[] getMulticastAddresses() {return (String[]) multicastAddresses.clone();}
  /** Get the ET system (file) name.
   *  @see SystemCreate#SystemCreate
   *  @see SystemCreate#name */
  public String getEtName() {return etName;}


  /**
   * Converts 4 bytes of a byte array into an integer.
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
   *  Reads the system level information from a data stream which is sent out by
   *  an ET system over the network.
   *  @param dis data input stream
   *  @exception java.io.IOException
   *     if data stream read error
   */
  void read(DataInputStream dis) throws IOException {
    byte[] info = new byte[100];
    dis.readFully(info);

    alive          = bytesToInt(info, 0);
    heartbeat      = bytesToInt(info, 4);
    temps          = bytesToInt(info, 8);
    stations       = bytesToInt(info, 12);
    attachments    = bytesToInt(info, 16);
    processes      = bytesToInt(info, 20);
    eventsOwned    = bytesToInt(info, 24);
    mutex          = bytesToInt(info, 28);
    statMutex      = bytesToInt(info, 32);
    statAddMutex   = bytesToInt(info, 36);

    endian         = bytesToInt(info, 40);
    share          = bytesToInt(info, 44);
    mainPid        = bytesToInt(info, 48);
    selects        = bytesToInt(info, 52);
    events         = bytesToInt(info, 56);
    eventSize      = bytesToInt(info, 60);
    tempsMax       = bytesToInt(info, 64);
    stationsMax    = bytesToInt(info, 68);
    attachmentsMax = bytesToInt(info, 72);
    processesMax   = bytesToInt(info, 76);

    tcpPort        = bytesToInt(info, 80);
    udpPort        = bytesToInt(info, 84);
    multicastPort  = bytesToInt(info, 88);

    interfaceCount = bytesToInt(info, 92);
    multicastCount = bytesToInt(info, 96);

    // read string lengths first
    int off = 0, lengthTotal = 0;
    int lengths[] = new int[interfaceCount+multicastCount+1];
    for (int i=0; i < interfaceCount+multicastCount+1; i++) {
      lengths[i]   = dis.readInt();
      lengthTotal += lengths[i];
    }

    if (lengthTotal > 100) {
      info = new byte[lengthTotal];
    }
    dis.readFully(info, 0, lengthTotal);

    // read network interface addresses
    interfaceAddresses = new String[interfaceCount];
    for (int i=0; i < interfaceCount; i++) {
      interfaceAddresses[i] = new String(info, off, lengths[i]-1, "ASCII");
      off += lengths[i];
    }

    // read multicast addresses
    multicastAddresses = new String[multicastCount];
    for (int i=0; i < multicastCount; i++) {
      multicastAddresses[i] = new String(info, off, lengths[i+interfaceCount]-1, "ASCII");
      off += lengths[i+interfaceCount];
    }

    // read et name
    etName = new String(info, off, lengths[interfaceCount+multicastCount]-1, "ASCII");
  }

}










