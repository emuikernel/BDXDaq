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
import java.io.*;
import java.net.*;

/**
 * This class creates an ET system.
 *
 * @author Carl Timmer
 */

public class SystemCreate {

  /** A copy of the ET system configuration. */
  SystemConfig     config;
  /** The name of the ET system file name. */
  String           name;
  /** A list of stations defining the flow of events. Only the first
   *  station of a single group of parallel stations is included in
   *  this list. The other parallel stations are available in a list
   *  kept by the first parallel station. */
  LinkedList<StationLocal> stations;            // protected by stopTransfer & systemLock
  /** The total number of idle and active stations. This consists
    * of the number of main stations given by the size of the "stations"
    * linked list (stations.size()) and the number of additional parallel
    * stations added together. */
  int              stationCount;
  /** GRAND_CENTRAL station object */
  private StationLocal gcStation;
  /** Table of all ET system attachments. */
  HashMap<Integer,AttachmentLocal> attachments;         // protected by systemLock
  /** Table of all ET system events. */
  HashMap<Long,Event>  events;
  /** All local IP addresses */
  InetAddress[]    netAddresses;
  /** Flag telling if the ET system is running. */
  private boolean  running;
  /** Mutex for system stuff. */
  byte[]           systemLock;
  /** Mutex for station stuff. */
  byte[]           stationLock;
  /** Flag for killing all threads started by ET system. */
  volatile boolean killAllThreads;

  // Variables for gathering system information for distribution.
  // Do it no more than once per second.

  /** Flag for specifying it's time to regather system information. */
  private boolean gather = true;
  /** Monitor time when gathering system information. */
  private long time1=0;
  /** Length of valid data in array storing system information. */
  int dataLength=0;
  /** Array for storing system information for distribution. */
  byte[] infoArray = new byte[6000];


  /**
   * Creates a new ET system using default parameters and starts it running.
   * The default parameters are:
   *      number of events          = {@link Constants#defaultNumEvents},
   *      event size                = {@link Constants#defaultEventSize},
   *      max number of stations    = {@link Constants#defaultStationsMax},
   *      max number of attachments = {@link Constants#defaultAttsMax},
   *      debug level               = {@link Constants#debugError},
   *      udp port                  = {@link Constants#broadcastPort},
   *      server (tcp) port         = {@link Constants#serverPort}, and
   *      multicasting port         = {@link Constants#multicastPort}
   *
   * @param _name   file name
   * @exception org.jlab.coda.et.EtException
   *     if the file already exists or cannot be created
   */
  public SystemCreate(String _name) throws EtException {
      this(_name, new SystemConfig());
  }

    /**
   * Creates a new ET system with specified parameters and starts it running.
   *
   * @param _name     file name
   * @param _config   ET system configuration
   * @exception org.jlab.coda.et.EtException
   *     if the file already exists or cannot be created
   */
  public SystemCreate (String _name, SystemConfig _config) throws EtException {
        // check config for consistency
        if (!_config.selfConsistent()) {
            if (config.debug >= Constants.debugInfo) {
                System.out.println("Number of events in groups does not equal total number of events");
            }
            throw new EtException("Number of events in groups does not equal total number of events");
        }
        name = _name;
        config = new SystemConfig(_config);
        attachments = new HashMap<Integer, AttachmentLocal>(Constants.attachmentsMax + 1, 1);
        events = new HashMap<Long, Event>(config.numEvents + 1, 1);
        stations = new LinkedList<StationLocal>();
        // netAddresses will be set in SystemUdpServer
        systemLock = new byte[0];
        stationLock = new byte[0];

        // The ET name is a file (which is really irrelevant in Java)
        // but is a convenient way to make all system names unique.
        File etFile = new File(name);
        try {
            // if file already exists ...
            if (!etFile.createNewFile()) {
                if (config.debug >= Constants.debugInfo) {
                    System.out.println("ET file already exists");
                }
                throw new EtException("ET file already exists");
            }
        }
        catch (IOException ex) {
            if (config.debug >= Constants.debugInfo) {
                System.out.println("cannot create ET file");
            }
            throw new EtException("Cannot create ET file");
        }
        etFile.deleteOnExit();

        // Write ascii into the file indicating a JAVA ET system
        // is creating and using it. This is for the benefit of
        // C-based ET systems which may try to open and read local
        // ET system files thinking they contain shared memory.
        try {
            FileOutputStream fos = new FileOutputStream(etFile);
            OutputStreamWriter osw = new OutputStreamWriter(fos, "ASCII");
            osw.write("JAVA ET SYSTEM FILE", 0, 19);
            osw.flush();
        }
        catch (FileNotFoundException ex) {
        }
        catch (UnsupportedEncodingException ex) {
        }
        catch (IOException ex) {
        }

        // store local IP addresses
        try {
            netAddresses = InetAddress.getAllByName(InetAddress.getLocalHost().getHostName());
        }
        catch (UnknownHostException ex) {
            if (config.debug >= Constants.debugError) {
                System.out.println("cannot find local IP addresses");
                ex.printStackTrace();
            }
            throw new EtException("Cannot find local IP addresses");
        }

        // start things running
        startUp();
    }


  /** Gets the ET system file name.
   *  @return ET system file name */
  public String getName() {return name;}
  /** Gets the ET system configuration.
   *  @return ET system configuration */
  public SystemConfig getConfig() {return new SystemConfig(config);}
  /** Tells if the ET system is running or not.
   *  @return <code>true</code> if the system is running and <code>false</code>
   *  if it is not */
  synchronized public boolean running() {return running;}


  /** Starts the ET system running. If the system is already running, nothing
   * is done. */
  synchronized public void startUp() {
    if (running) return;

    // make grandcentral
    gcStation = createGrandCentral();

    // fill GC with standard sized events
    Event ev;
    int index = 0, count = 0;
    ArrayList<Event> eventList = new ArrayList<Event>(config.numEvents);

    for (long i=0; i < config.numEvents; i++) {
      ev = new Event(config.eventSize);
      ev.id = i;
        
      // assign group numbers
      if (count < 1)
          count = config.groups[index++];
      ev.group = index;
      count--;

      eventList.add(ev);
      // add to hashTable for future easy access
      events.put(ev.id, ev);
    }

    gcStation.inputList.putInLow(eventList);
    // undo statistics keeping for inital event loading
    gcStation.inputList.eventsIn = 0;

    // run tcp server thread
    SystemTcpServer tcpServer = new SystemTcpServer(this);
    tcpServer.start();

    // run udp listening thread
    SystemUdpServer udpServer = new SystemUdpServer(this);
    udpServer.start();

    running = true;
  }


  /** Stops the ET system if it is running. All threads are stopped.
   * If the system is not running, nothing is done. */
  synchronized public void shutdown() {
    if (!running) return;
    // tell threads to kill themselves
    killAllThreads = true;
    // sockets on 2 second timeout so wait
    try {Thread.sleep(2500);}
    catch (InterruptedException ex) {}
    // delete file
    File etFile = new File(name);
    etFile.delete();

    // clear everything
    stations       = null;
    attachments    = null;
    events         = null;
    netAddresses   = null;
    stationLock    = null;
    killAllThreads = false;
    running        = false;
  }

  //
  // Station related methods
  //

  /**
   * Creates a new station placed at the end of the linked list of stations.
   *
   * @param stationConfig   station configuration
   * @param name            station name
   *
   * @return the new station object
   *
   * @exception org.jlab.coda.et.EtException
   *     if the select method's class cannot be loaded
   * @exception org.jlab.coda.et.EtExistsException
   *     if the station already exists but with a different configuration
   * @exception org.jlab.coda.et.EtTooManyException
   *     if the maximum number of stations has been created already
   */
  public StationLocal createStation(StationConfig stationConfig, String name)
                throws EtException, EtExistsException, EtTooManyException {
    synchronized(stationLock) {
      return createStation(stationConfig, name, stations.size(), Constants.end);
    }
  }

  /**
   * Creates a new station at a specified position in the linked list of
   * stations. Cannot exceed the maximum number of stations allowed in a system.
   *
   * @param stationConfig   station configuration
   * @param name            station name
   * @param position        position in the linked list to put the station.
   *
   * @return                the new station object
   *
   * @exception org.jlab.coda.et.EtException
   *     if the select method's class cannot be loaded
   * @exception org.jlab.coda.et.EtExistsException
   *     if the station already exists but with a different configuration
   * @exception org.jlab.coda.et.EtTooManyException
   *     if the maximum number of stations has been created already
   */
  StationLocal createStation(StationConfig stationConfig, String name,
                             int position, int parallelPosition)
          throws EtException, EtExistsException, EtTooManyException {


      int id = 0;
      StationLocal station;

      // grab station mutex
      synchronized (stationLock) {
          // check to see if hit maximum allowed # of stations
          if (stations.size() >= config.stationsMax) {
              throw new EtTooManyException("Maximum number of stations already created");
          }
          else if (position > stations.size()) {
              position = stations.size();
          }

          // check to see if it already exists
          StationLocal listStation;
          try {
              listStation = stationNameToObject(name);
              // it's got the same name, let's see if it's defined the same
              if (    (listStation.config.flowMode == stationConfig.flowMode) &&
                      (listStation.config.userMode == stationConfig.userMode) &&
                      (listStation.config.blockMode == stationConfig.blockMode) &&
                      (listStation.config.selectMode == stationConfig.selectMode) &&
                      (listStation.config.restoreMode == stationConfig.restoreMode) &&
                      (listStation.config.prescale == stationConfig.prescale) &&
                      (listStation.config.cue == stationConfig.cue) &&
                      (Arrays.equals(listStation.config.select, stationConfig.select))) {

                  if ((listStation.config.selectClass != null) &&
                          (!listStation.config.selectClass.equals(stationConfig.selectClass))) {
                      throw new EtExistsException("Station already exists with different configuration");
                  }
                  // station definitions are the same, use listStation
                  return listStation;
              }
              throw new EtExistsException("Station already exists with different configuration");
          }
          catch (EtException ex) {
              // station does NOT exist, continue on
          }

          // find smallest possible unique id number
          search:
          for (int i = 0; i < stationCount + 1; i++) {
              for (ListIterator j = stations.listIterator(); j.hasNext();) {
                  listStation = (StationLocal) j.next();
                  if (listStation.id == i) {
                      continue search;
                  }
                  if (listStation.config.flowMode == Constants.stationParallel) {
                      for (ListIterator k = listStation.parallelStations.listIterator(1); k.hasNext();) {
                          listStation = (StationLocal) k.next();
                          if (listStation.id == i) {
                              continue search;
                          }
                      }
                  }
              }
              // only get down here if "i" is not a used id number
              id = i;
              break;
          }

          // create station
          station = new StationLocal(this, name, stationConfig, id);

          // start its conductor thread
          station.start();
          // give up processor so thread can start
          Thread.yield();

          // make sure the conductor is started or we'll get race conditions
          while (station.status != Constants.stationIdle) {
              if (config.debug >= Constants.debugInfo) {
                  System.out.println("Waiting for " + name + "'s conductor thread to start");
              }
              // sleep for minimum amount of time (1 nsec haha)
              try {
                  Thread.sleep(0, 1);
              }
              catch (InterruptedException ex) {
              }
          }

          // put in linked list(s) - first grabbing stopTransfer mutexes
          gcStation.addStation(station, position, parallelPosition);
          // keep track of the total number of stations
          stationCount++;
      } // release station mutex

      return station;
  }

  /**
   * Creates the first station by the name of GRAND_CENTRAL and starts its
   * conductor thread.
   *
   * @return GRAND_CENTRAL station's object
   */
  private StationLocal createGrandCentral() {
    // use the default configuration
    StationConfig gcConfig = new StationConfig();
    StationLocal station = null;
    // create station
    try {
      station = new StationLocal(this, "GRAND_CENTRAL", gcConfig, 0);
    }
    catch (EtException ex) {}

    // put in linked list
    stations.clear();
    stations.addFirst(station);

    // start its conductor thread
    station.start();
    
    // keep track of the total number of stations
    stationCount++;
    return station;
  }

  /**
   * Removes an existing station.
   *
   * @param   statId station id
   * @exception org.jlab.coda.et.EtException
   *     if attachments to the station still exist or the station does not exist
   */
  void removeStation(int statId) throws EtException {
    StationLocal stat;
    // grab station mutex
    synchronized(stationLock) {
      stat = stationIdToObject(statId);
      // only remove if no attached processes
      if (stat.attachments.size() != 0) {
	    throw new EtException("Remove all attachments before removing station");
      }

      // remove from linked list - first grabbing stopTransfer mutexes
      gcStation.removeStation(stat);

      // kill conductor thread
      stat.killConductor = true;
      stat.interrupt();

      // set status
      stat.status = Constants.stationUnused;
      
      // keep track of the total number of stations
      stationCount--;
      return;
    }
  }

  /**
   * Changes the position of a station in the linked lists of stations.
   *
   * @param statId     station id
   * @param position   position in the main linked list of stations (starting at 0)
   * @param parallelPosition position of a parallel station in a group of
   *     parallel stations (starting at 0)
   * @exception org.jlab.coda.et.EtException
   *     if the station does not exist, or
   *     if trying to move an incompatible parallel station to an existing group
   *     of parallel stations or to the head of an existing group of parallel
   *     stations.
   */
  void setStationPosition(int statId, int position, int parallelPosition)
                                                          throws EtException {
    StationLocal stat;
    // grab station mutex
    synchronized(stationLock) {
      stat = stationIdToObject(statId);
      // change linked list - first grabbing stopTransfer mutexes
      gcStation.moveStation(stat, position, parallelPosition);
    }
  }

  /**
   * Gets the position of a station in the main linked list of stations.
   *
   * @param statId   station id
   * @return         the position of a station in the linked list of stations
   * @exception org.jlab.coda.et.EtException
   *     if the station does not exist
   */
  int getStationPosition(int statId) throws EtException {
      // GrandCentral is always first
      if (statId == 0) return 0;
      int position = 0;

      synchronized (stationLock) {
          for (StationLocal stat : stations) {
              if (stat.id == statId) {
                  return position;
              }
              if (stat.config.flowMode == Constants.stationParallel) {
                  for (StationLocal stat2 : stat.parallelStations) {
                      if (stat2.id == statId) {
                          return position;
                      }
                  }
              }
              position++;
          }
      }
      throw new EtException("cannot find station");
  }

  /**
   * Gets the position of a parallel station in its linked list of 
   * parallel stations.
   *
   * @param statId   station id
   * @return         the position of a parallel station in its linked list
   *      of parallel stations, or zero if station is serial
   * @exception org.jlab.coda.et.EtException
   *     if the station does not exist
   */
  int getStationParallelPosition(int statId) throws EtException {
      // parallel position is 0 for serial stations
      if (statId == 0) return 0;
      int pposition;

      synchronized (stationLock) {
          for (StationLocal stat : stations) {
              if (stat.id == statId) {
                  return 0;
              }
              if (stat.config.flowMode == Constants.stationParallel) {
                  pposition = 1;
                  for (StationLocal stat2 : stat.parallelStations) {
                      if (stat2.id == statId) {
                          return pposition;
                      }
                      pposition++;
                  }
              }
          }
      }
      throw new EtException("cannot find station");
  }

  /**
   * Tells if an attachment is attached to a station.
   *
   * @param statId   station id
   * @param attId    attachment id
   * @return         <code>true</code> if an attachment is attached to a station
   *                 and <code>false</code> otherwise
   * @exception org.jlab.coda.et.EtException
   *     if the station does not exist
   */
  boolean stationAttached(int statId, int attId) throws EtException {
      StationLocal stat;
      synchronized (stationLock) {
          stat = stationIdToObject(statId);
          for (AttachmentLocal att : stat.attachments) {
              if (att.id == attId) {
                  return true;
              }
          }
          return false;
      }
  }

    /**
   * Tells if a station exists.
   *
   * @param station station object
   * @return <code>true</code> if a station exists and
   *         <code>false</code> otherwise
   */
    boolean stationExists(StationLocal station) {
        synchronized (stationLock) {
            if (stations.contains(station)) {
                return true;
            }
            for (StationLocal listStation : stations) {
                if (listStation.config.flowMode == Constants.stationParallel) {
                    if (listStation.parallelStations.contains(station)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

  /**
   * Tells if a station exists.
   *
   * @param    name station name
   * @return   <code>true</code> if a station exists and
   *           <code>false</code> otherwise
   */
  boolean stationExists(String name) {
      try {
          stationNameToObject(name);
      }
      catch (EtException ex) {
          return false;
      }
      return true;
  }

  /**
   * Gets a station's object representation.
   *
   * @param    name station name
   * @return   a station's object
   * @exception org.jlab.coda.et.EtException
   *     if the station does not exist
   */
  StationLocal stationNameToObject(String name) throws EtException {
      synchronized (stationLock) {
          for (StationLocal listStation : stations) {
              if (listStation.name.equals(name)) {
                  return listStation;
              }
              if (listStation.config.flowMode == Constants.stationParallel) {
                  for (StationLocal listStation2 : listStation.parallelStations) {
                      if (listStation2.name.equals(name)) {
                          return listStation2;
                      }
                  }
              }
          }
      }
      throw new EtException("station " + name + " does not exist");
  }

  /**
   * Given a station id number, this method gets the corresponding
   * StationLocal object.
   *
   * @param     statId station id
   * @return    the station's object
   * @exception org.jlab.coda.et.EtException
   *     if the station does not exist
   */
  StationLocal stationIdToObject(int statId) throws EtException {
      synchronized (stationLock) {
          for (StationLocal stat : stations) {
              if (stat.id == statId) {
                  return stat;
              }
              if (stat.config.flowMode == Constants.stationParallel) {
                  for (StationLocal stat2 : stat.parallelStations) {
                      if (stat2.id == statId) {
                          return stat2;
                      }
                  }
              }
          }
      }
      throw new EtException("station  with id \"" + statId + "\" does not exist");
  }

  //
  // attachment related methods
  //

  /**
   * Create an attachment to a station.
   *
   * @param statId   station id
   * @return         an attachment object
   * @exception org.jlab.coda.et.EtException
   *     if the station does not exist
   * @exception org.jlab.coda.et.EtTooManyException
   *     if station does not exist, or
   *     if no more attachments are allowed to the station, or
   *     if no more attachments are allowed to ET system
   */
  AttachmentLocal attach(int statId) throws EtException, EtTooManyException {

      AttachmentLocal att;
      synchronized (stationLock) {
          StationLocal station = stationIdToObject(statId);

          // limit on # of attachments to station
          if ((station.config.userMode > 0) &&
                  (station.config.userMode <= station.attachments.size())) {
              throw new EtTooManyException("no more attachments allowed to station");
          }

          synchronized (systemLock) {
              // limit on number of attachments to ET system
              if (attachments.size() >= config.attachmentsMax) {
                  throw new EtTooManyException("no more attachments allowed to ET system");
              }

              // Server will overwrite id & host with true (remote) values
              att = new AttachmentLocal();
              att.station = station;
              // find smallest possible unique id number
              if (attachments.size() == 0) {
                  att.id = 0;
              }
              else {
                  search:
                  for (int i = 0; i < attachments.size() + 1; i++) {
                      for (Integer j : attachments.keySet()) {
                          if (j == i) continue search;
                      }
                      // only get down here if "i" is not a used id number
                      att.id = i;
                      break;
                  }
              }
              //att.status = Constants.attActive;
//System.out.println("attach att #" + att.id + " will put into system's map");
              attachments.put(att.id, att);
          }

          // keep att stats in station too?
          station.attachments.add(att);
          // station.status = Constants.stationActive;
          // change station status - first grabbing stopTransfer mutexes
          gcStation.changeStationStatus(station, Constants.stationActive);
//System.out.println("attach att #" + att.id + " put into station's map & active");
      }

      return att;
  }


  /**
   * Remove an attachment from a station.
   *
   * @param att   attachment object
   */
  void detach(AttachmentLocal att) {
//System.out.println("detach: IN");
        synchronized (stationLock) {
            // if last attachment & not GrandCentral - mark station idle
            if ((att.station.attachments.size() == 1) && (att.station.id != 0)) {
//System.out.println("detach: att #" + att.id + " and make idle");
                // att.station.status = Constants.stationIdle;
                // change station status - first grabbing stopTransfer mutexes
                gcStation.changeStationStatus(att.station, Constants.stationIdle);
                // give other threads a chance to finish putting events in
                Thread.yield();
                // flush any remaining events
                if (att.station.config.getRestoreMode() == Constants.stationRestoreRedist) {
                    // send to output list of previous station
                    try {
                        int pos = getStationPosition(att.station.id);
                        if (--pos < 0) return;
                        StationLocal prevStat = stations.get(pos);
                        try {
                            moveEvents(prevStat.outputList, Arrays.asList(getEvents(att, Constants.async, 0, config.numEvents)));
                        }
                        catch (Exception e) {
                        }
                    }
                    catch (Exception e) {
                        e.printStackTrace();
                        return;
                    }
                }
                else {
                    // send to output list
                    try {
                        putEvents(att, getEvents(att, Constants.async, 0, config.numEvents));
                    }
                    catch (Exception ex) { }
                }
            }
//System.out.println("detach att #" + att.id + " remove from station map");
            att.station.attachments.remove(att);

            // restore events gotten but not put back into system
            restoreEvents(att);

            synchronized (systemLock) {
                // get rid of attachment
//System.out.println("detach att #" + att.id + " remove from system map");
                attachments.remove(new Integer(att.id));
            }
        }
        return;
    }


  /**
   * Restore events gotten by an attachment but lost when its network connection
   * was broken. These events are not guaranteed to be restored in any
   * particular order.
   *
   * @param att   attachment object
   */
  private void restoreEvents(AttachmentLocal att) {
      // Split new events (which should be dumped) from used
      // events which go where directed by station configuration.
      ArrayList<Event> usedEvs = new ArrayList<Event>(config.numEvents);
      ArrayList<Event>  newEvs = new ArrayList<Event>(config.numEvents);
//System.out.println("into restoreEvents");

      // look at all events
      for (Event ev : events.values()) {
          // find those owned by this attachment
          if (ev.owner == att.id) {
              if (ev.age == Constants.eventNew) {
//System.out.println("found new ev " + ev.id + " owned by attachment " + att.id);
                  newEvs.add(ev);
              }
              else {
                  // Put high priority events first.
                  // Original order may get messed up here.
//System.out.println("found used ev " + ev.id + " owned by attachment " + att.id);
                  if (ev.priority == Constants.high) {
                      usedEvs.add(0, ev);
                  }
                  else {
                      usedEvs.add(ev);
                  }
              }
          }
      }

      if (newEvs.size() > 0) {
//System.out.println("dump " + newEvs.size() + " new events");
          dumpEvents(att, newEvs);
      }

      if (usedEvs.size() > 0) {
//System.out.println("restore " + usedEvs.size() + " used events");
          // if normal events are to be returned to GrandCentral
          if ((att.station.config.restoreMode == Constants.stationRestoreGC) ||
                  (att.station.id == 0)) {
//System.out.println("restore used events to GC (dump)");
              dumpEvents(att, usedEvs);
          }
          // Else if events are to be returned to station's outputList ...
          // Notice that if we are supposed to put things in the
          // inputList, but we are the last attachement (and in the
          // middle of detaching), then events put into the inputList
          // will be lost to the system. Place them into the outputList.
          else if ((att.station.config.restoreMode == Constants.stationRestoreOut) ||
                  ((att.station.config.restoreMode == Constants.stationRestoreIn) &&
                          (att.station.attachments.size() == 0))) {
//System.out.println("restore used events to output list");
              putEvents(att, usedEvs);
          }
          else if (att.station.config.restoreMode == Constants.stationRestoreIn) {
              // If the station is blocking, its inputList has room for all
              // the events and there's no problem putting them all.
              // Statistics don't get messed up here.
              if (att.station.config.blockMode == Constants.stationBlocking) {
//System.out.println("restore used events to input list of blocking station");
                  moveEvents(att.station.inputList, usedEvs);
              }
              // Else if nonblocking there may not be enough room.
              // Equivalent to putting events back into the station's inputList
              // is to put them into the previous station' outputList and letting
              // its conductor thread do the work - which is easy to program.
              // This has the unfortunate side effect of probably messing
              // up the statistics as some events may be counted twice.
              else {
                  try {
                      // Find previous station
                      int pos = getStationPosition(att.station.id);
                      if (--pos < 0) return;
                      StationLocal prevStat = stations.get(pos);
                      moveEvents(prevStat.outputList, usedEvs);
                  }
                  catch (EtException e) { return; }
//System.out.println("restore used events to input list of nonblocking station");
              }
          }
          else if (att.station.config.restoreMode == Constants.stationRestoreRedist) {
              // Put events into the previous station's outputList and let its
              // conductor thread do the work - redistributing them to the members
              // of the parallel station group.
              // This has the unfortunate side effect of probably messing
              // up the statistics as some events may be counted twice.
              try {
//System.out.println("TRY to restore used events to output list of previous station");
                  int pos = getStationPosition(att.station.id);
                  if (--pos < 0) return;
                  StationLocal prevStat = stations.get(pos);
//System.out.println("Found previous station -> " + prevStat.name + ", putting " + usedEvs.size() + " number of events");
                  moveEvents(prevStat.outputList, usedEvs);
              }
              catch (Exception e) { return; }
//System.out.println("DID restore used events to output list of previous station");
          }
      }
      return;
  }

  //
  // event related methods
  //

  /**
   * Get new or unused events from an ET system.
   *
   * @param att       attachment object
   * @param mode      if there are no events available, this parameter specifies
   *                  whether to wait for some by sleeping, by waiting for a set
   *                  time, or by returning immediately (asynchronous)
   * @param microSec  the number of microseconds to wait if a timed wait is
   *                  specified
   * @param count     the number of events desired
   * @param size      the size of events in bytes
   *
   * @return an array of events
   *
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
  Event[] newEvents(AttachmentLocal att, int mode, int microSec, int count, int size)
                    throws EtEmptyException, EtBusyException,
                           EtTimeoutException, EtWakeUpException {

//System.out.println("newEvents: get " + count + " events");

    // get events from GrandCentral Station's output list
    Event[] evs = gcStation.inputList.get(att, mode, microSec, count);
//System.out.println("newEvents: got events");

    // for each event ...
    for (Event ev : evs) {
      // initialize fields
      ev.init();
      // registered as owned by this attachment
      ev.owner = att.id;
      // if size is too small make it larger
      if (ev.memSize < size) {
        ev.data = new byte[size];
        ev.memSize = size;
      }
//System.out.println("newEvents: ev.id = "+ ev.id + ", size = " + ev.memSize);
    }

    // keep track of # of events made by this attachment
    att.eventsMake += evs.length;
//System.out.println("newEvents: att.eventsMake = "+ att.eventsMake);
    return evs;
  }




    /**
     * Get new or unused events from an ET system.
     *
     * @param att       attachment object
     * @param mode      if there are no events available, this parameter specifies
     *                  whether to wait for some by sleeping, by waiting for a set
     *                  time, or by returning immediately (asynchronous)
     * @param microSec  the number of microseconds to wait if a timed wait is
     *                  specified
     * @param count     the number of events desired
     * @param size      the size of events in bytes
     * @param group     the group number of events
     *
     * @return a list of events
     *
     * @exception org.jlab.coda.et.EtException
     *     if the group number is not meaningful
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
    List<Event> newEvents(AttachmentLocal att, int mode, int microSec, int count, int size, int group)
                      throws EtException, EtEmptyException, EtBusyException,
                             EtTimeoutException, EtWakeUpException {

//System.out.println("newEvents: try getting " + count + " events of group # " + group);
      // check to see if value of group is meaningful
      if (group > config.groups.length) {
          throw new EtException("group number is too high");
      }

      // get events from GrandCentral Station's output list
      List<Event> evs = gcStation.inputList.get(att, mode, microSec, count, group);
//System.out.println("newEvents: got events (# = " + evs.size() + ")");

      // for each event ...
      for (Event ev : evs) {
        // initialize fields
        ev.init();
        // registered as owned by this attachment
        ev.owner = att.id;
        // if size is too small make it larger
        if (ev.memSize < size) {
          ev.data = new byte[size];
          ev.memSize = size;
        }
//System.out.println("newEvents: ev.id = "+ ev.id + ", size = " + ev.memSize + ", group = " + ev.group);
      }

      // keep track of # of events made by this attachment
      att.eventsMake += evs.size();
//System.out.println("newEvents: att.eventsMake = "+ att.eventsMake);
      return evs;
    }




  /**
   * Get events from an ET system.
   *
   * @param att      attachment object
   * @param mode     if there are no events available, this parameter specifies
   *                 whether to wait for some by sleeping, by waiting for a set
   *                 time, or by returning immediately (asynchronous)
   * @param microSec the number of microseconds to wait if a timed wait is
   *                 specified
   * @param count    the number of events desired
   *
   * @return an array of events
   *
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
  Event[] getEvents(AttachmentLocal att, int mode, int microSec, int count)
                      throws EtEmptyException, EtBusyException,
                             EtTimeoutException, EtWakeUpException {

    Event[] evs = att.station.inputList.get(att, mode, microSec, count);

    // each event is registered as owned by this attachment
    for (Event ev : evs) {
      ev.owner = att.id;
    }

    // keep track of # of events gotten by this attachment
    att.eventsGet += evs.length;

    return evs;
  }



  /**
   * Place events into a station's input or output list.
   * Used when moving events stranded at a station due to
   * the last attachment having crashed. Does not change statistics.
   *
   * @param list        station's input or output list into which events are placed
   * @param eventList   list of event objects
   *
   */
  private void moveEvents(EventList list, List<Event> eventList) {
    // mark events as used and as owned by system
    for (Event ev : eventList) {
      ev.age   = Constants.eventUsed;
      ev.owner = Constants.system;
    }

    list.putReverse(eventList);

    return;
  }




    /**
     * Put events into an ET system.
     *
     * @param att          attachment object
     * @param eventArray   array of event objects
     *
     */
    void putEvents(AttachmentLocal att, Event[] eventArray) {
      // mark events as used and as owned by system
      for (Event ev : eventArray) {
//System.out.println("putEvents: set age & owner of event " + i);
        ev.age   = Constants.eventUsed;
        ev.owner = Constants.system;
      }

      att.station.outputList.put(eventArray);
      // keep track of # of events put by this attachment
      att.eventsPut += eventArray.length;

      return;
    }




  /**
   * Put events into an ET system.
   *
   * @param att         attachment object
   * @param eventList   list of event objects
   *
   */
  private void putEvents(AttachmentLocal att, ArrayList<Event> eventList) {
//System.out.println("putEvents: got in, array length = " + eventList.length);
    // mark events as used and as owned by system
    for (Event ev : eventList) {
//System.out.println("putEvents: set age & owner of event " + i);
      ev.age   = Constants.eventUsed;
      ev.owner = Constants.system;
    }

    att.station.outputList.put(eventList);
    // keep track of # of events put by this attachment
    att.eventsPut += eventList.size();

    return;
  }



  /**
   * Dispose of unwanted events in an ET system. The events are recycled and not
   * made available to any other user.
   *
   * @param att          attachment object
   * @param eventArray   array of event objects
   *
   */
  void dumpEvents(AttachmentLocal att, Event[] eventArray) {
    // mark as owned by system
    for (Event ev : eventArray) {
      ev.owner = Constants.system;
    }

    // put into GrandCentral Station
    gcStation.inputList.putInGC(eventArray);

    // keep track of # of events put by this attachment
    att.eventsDump += eventArray.length;

    return;
  }



  /**
   * Dispose of unwanted events in an ET system. The events are recycled and not
   * made available to any other user.
   *
   * @param att         attachment object
   * @param eventList   list of event objects
   *
   */
  private void dumpEvents(AttachmentLocal att, ArrayList<Event> eventList) {
    for (Event ev : eventList) {
      ev.owner = Constants.system;
    }
    gcStation.inputList.putInGC(eventList);
    att.eventsDump += eventList.size();
    return;
  }



  /**
   * Gather all ET system data for sending over the network and put it into a
   * single byte array. Putting the data into a byte array is done so that its
   * size is known beforehand and can be sent as the first int of data. It is
   * also done so that this gathering of data is done no more than once a second
   * and so that the data is shared by the many attachments that may want it.
   * It is far more efficient to send an existing array of data to an attachment
   * than it is to regather it for each request. The calling of this method is
   * mutex protected so only 1 thread/attachment can modify or access the
   * array at a time.
   *
   * @return <code>Constants.ok</code> if everything is fine and
   *         <code>Constants.error</code> otherwise
   */
  int gatherSystemData() {
      int err = Constants.ok;

      // regather all system information every second at most
      long time2 = System.currentTimeMillis();
      if (time2 - time1 > 1000) {
        gather = true;
      }

      if (gather) {
        time1 = System.currentTimeMillis();
        try {
          // Estimate the maximum space we need to store all the data
          // (assuming no more than 10 network interfaces & multicast addrs).
          // Use of LinkedList/HashMap is not mutex protected here so it
          // may fail. Catch all errors.
          int numStations = stationCount;	  
          int numAtts = attachments.size();
          int size = 600 + 570*numStations + 160*numAtts;

          if (size > infoArray.length) {
            infoArray = new byte[size];
          }

          int len1 = writeSystemData(infoArray, 4);
          int len2 = writeStationData(infoArray, 4+len1, numStations);
          int len3 = writeAttachmentData(infoArray, 4+len1+len2, numAtts);
//System.out.println("len1-3 = " + len1 + ", " + len2 + ", " + len3);
          // no process data in Java ET systems
          Event.intToBytes(0, infoArray, 4+len1+len2+len3);
          int len4 = 4;
          dataLength = len1+len2+len3+len4;

          // Write size of data (not including the length now being written)
          // in beginning of array.
          Event.intToBytes(dataLength, infoArray, 0);

          gather = false;
        }
        catch (Exception ex) {
            ex.printStackTrace();
            err = Constants.error;
        }
      }
      return err;
  }


  /**
   * Get ET system data for sending over the network.
   *
   * @param info byte array to hold system data
   * @param off offset of the byte array
   * @return size of the data in bytes placed into the byte array
   */
  private int writeSystemData(byte[] info, int off) {
    // values which can change
    Event.intToBytes(1, info, off); // alive by definition
    Event.intToBytes(0, info, off+=4); // no heartbeat
    Event.intToBytes(0, info, off+=4); // no temp events
    Event.intToBytes(stations.size(), info, off+=4); // aren't these redundant???
    Event.intToBytes(attachments.size(), info, off+=4); // aren't these redundant???
    Event.intToBytes(0, info, off+=4); // no processes sharing memory

    // find out how many events the system owns
    int eventsOwned = 0;
    for (Event ev : events.values()) {
        if (ev.owner == Constants.system) {
          eventsOwned++;
        }
    }
    Event.intToBytes(eventsOwned, info, off+=4);

    // no way to test and see if mutexes are locked in Java
    Event.intToBytes(Constants.mutexUnlocked, info, off+=4);
    Event.intToBytes(Constants.mutexUnlocked, info, off+=4);
    Event.intToBytes(Constants.mutexUnlocked, info, off+=4);

    // values which do NOT change
    Event.intToBytes(Constants.endianBig, info, off+=4);
    Event.intToBytes(Constants.mutexNoShare, info, off+=4);
    Event.intToBytes(-1, info, off+=4);
    Event.intToBytes(Constants.stationSelectInts, info, off+=4);
    Event.intToBytes(config.numEvents, info, off+=4);
    Event.longToBytes((long)config.eventSize, info, off+=4);
    Event.intToBytes(0, info, off+=8);   // send 0 if 32 bits, else 1 if 64 bits
    Event.intToBytes(0, info, off+=4);
    Event.intToBytes(config.stationsMax, info, off+=4);
    Event.intToBytes(config.attachmentsMax, info, off+=4);
    Event.intToBytes(0, info, off+=4);

    // tcp server port
    Event.intToBytes(config.serverPort, info, off+=4);
    // udp port
    Event.intToBytes(config.udpPort, info, off+=4);
    // multicast port
    Event.intToBytes(config.multicastPort, info, off+=4);

    // # of interfaces and multicast addresses
    Event.intToBytes(netAddresses.length, info, off+=4);
    Event.intToBytes(config.multicastAddrs.size(), info, off+=4);

    int len;
    int totalInts = 27;
    int totalStringLen = 0;

    // length of interface address strings
    for (InetAddress addr : netAddresses) {
      len = addr.getHostAddress().length() + 1;
      Event.intToBytes(len, info, off+=4);
      totalInts++;
      totalStringLen += len;
    }

    // what about broadcast address strings???

    // length of multicast address strings
    for (InetAddress addr : config.multicastAddrs) {
      len = addr.getHostAddress().length() + 1;
      Event.intToBytes(len, info, off+=4);
      totalInts++;
      totalStringLen += len;
    }

    // length ET file name
    len = name.length() + 1;
    Event.intToBytes(len, info, off+=4);
    totalInts++;
    totalStringLen += len;

    // total data size
    int byteSize = 4*totalInts + totalStringLen;

    // write strings into array
    off += 4;
    byte[] outString;

    for (InetAddress addr : netAddresses) {
      try {
        outString = addr.getHostAddress().getBytes("ASCII");
        System.arraycopy(outString, 0, info, off, outString.length);
        off += outString.length;
      }
      catch (UnsupportedEncodingException ex) {}
      info[off++] = 0; // C null terminator
    }

    for (InetAddress addr : config.multicastAddrs) {
      try {
        outString = addr.getHostAddress().getBytes("ASCII");
        System.arraycopy(outString, 0, info, off, outString.length);
        off += outString.length;
      }
      catch (UnsupportedEncodingException ex) {}
      info[off++] = 0; // C null terminator
    }

    try {
      outString = name.getBytes("ASCII");
      System.arraycopy(outString, 0, info, off, outString.length);
      off += outString.length;
    }
    catch (UnsupportedEncodingException ex) {}
    info[off] = 0; // C null terminator

    return byteSize;
  }


  /**
   * Get station data for sending over the network.
   *
   * @param info byte array to hold station data
   * @param offset offset of the byte array
   * @param stationsMax limit on number of stations to gather data on
   * @return size of the data in bytes placed into the byte array
   */
  private int writeStationData(byte[] info, int offset, int stationsMax) {
      // Since we're not grabbing any mutexes, the number of stations may have
      // changed between when the buffer size was set and now. Therefore, do not
      // exceed "stationsMax" number of stations.

      int len1, len2, len3, len4, numAtts, counter, offAtt;
      int off = offset;
      int byteSize = 0;
      int statCount = 0;
      boolean isHead = true;
      // save space at beginning to insert int containing # of stations
      byteSize += 4;
      off += 4;

      ListIterator parallelIterator = null;
      ListIterator mainIterator = stations.listIterator();
      StationLocal stat = (StationLocal) mainIterator.next();
//System.out.println("writeStationData: " + stationsMax + " stats, start with " + stat.name);      
      while (true) {

          // Since the number of attachments may change, be careful,
          // reserve a spot here to be filled later with num of attachments.
          offAtt = off;

          Event.intToBytes(stat.id, info, off += 4);
          Event.intToBytes(stat.status, info, off += 4);
          Event.intToBytes(Constants.mutexUnlocked, info, off += 4);

          // since the number of attachments may change, be careful
          counter = 0;
          for (AttachmentLocal att : stat.attachments) {
              Event.intToBytes(att.id, info, off += 4);
              counter++;
          }
          Event.intToBytes(counter, info, offAtt);
          numAtts = counter;

          Event.intToBytes(Constants.mutexUnlocked, info, off += 4);
          Event.intToBytes(stat.inputList.events.size(), info, off += 4);
          Event.longToBytes(stat.inputList.eventsTry, info, off += 4);
          Event.longToBytes(stat.inputList.eventsIn, info, off += 8);
          Event.intToBytes(Constants.mutexUnlocked, info, off += 8);
          Event.intToBytes(stat.outputList.events.size(), info, off += 4);
          Event.longToBytes(stat.outputList.eventsOut, info, off += 4);

          if (stat.config.flowMode == Constants.stationParallel && isHead) {
              Event.intToBytes(Constants.stationParallelHead, info, off += 8);
          }
          else {
              Event.intToBytes(stat.config.flowMode, info, off += 8);
          }
          Event.intToBytes(stat.config.userMode, info, off += 4);
          Event.intToBytes(stat.config.restoreMode, info, off += 4);
          Event.intToBytes(stat.config.blockMode, info, off += 4);
          Event.intToBytes(stat.config.prescale, info, off += 4);
          Event.intToBytes(stat.config.cue, info, off += 4);
          Event.intToBytes(stat.config.selectMode, info, off += 4);
          for (int j = 0; j < Constants.stationSelectInts; j++) {
              Event.intToBytes(stat.config.select[j], info, off += 4);
          }

          // write strings, lengths first
          len1 = 0;
          if (stat.config.selectFunction != null) {
              len1 = stat.config.selectFunction.length() + 1;
          }
          len2 = 0;
          if (stat.config.selectLibrary != null) {
              len2 = stat.config.selectLibrary.length() + 1;
          }
          len3 = 0;
          if (stat.config.selectClass != null) {
              len3 = stat.config.selectClass.length() + 1;
          }
          len4 = stat.name.length() + 1;

          Event.intToBytes(len1, info, off += 4);
          Event.intToBytes(len2, info, off += 4);
          Event.intToBytes(len3, info, off += 4);
          Event.intToBytes(len4, info, off += 4);

          // write strings into array
          off += 4;
          byte[] outString;

          try {
              if (len1 > 0) {
                  outString = stat.config.selectFunction.getBytes("ASCII");
                  System.arraycopy(outString, 0, info, off, outString.length);
                  off += outString.length;
                  info[off++] = 0; // C null terminator
              }
              if (len2 > 0) {
                  outString = stat.config.selectLibrary.getBytes("ASCII");
                  System.arraycopy(outString, 0, info, off, outString.length);
                  off += outString.length;
                  info[off++] = 0; // C null terminator
              }
              if (len3 > 0) {
                  outString = stat.config.selectClass.getBytes("ASCII");
                  System.arraycopy(outString, 0, info, off, outString.length);
                  off += outString.length;
                  info[off++] = 0; // C null terminator
              }

              outString = stat.name.getBytes("ASCII");
              System.arraycopy(outString, 0, info, off, outString.length);
              off += outString.length;
              info[off++] = 0; // C null terminator
          }
          catch (UnsupportedEncodingException ex) {
          }

          // track size of all data stored in buffer
          byteSize += 4 * (19 + numAtts + Constants.stationSelectInts) +
                  8 * 3 + len1 + len2 + len3 + len4;

          // if more stations now than space allowed for, skip rest
          if (++statCount >= stationsMax) {
//System.out.println("statCount = " + statCount + ", reached station limit, break out ");      
              break;
          }

          // if head of parallel linked list ...
          if ((stat.config.flowMode == Constants.stationParallel) &&
                  (stations.contains(stat))) {
              parallelIterator = stat.parallelStations.listIterator(1);
              if (parallelIterator.hasNext()) {
                  isHead = false;
                  stat = (StationLocal) parallelIterator.next();
//System.out.println("go to || stat " + stat.name);      
                  continue;
              }
          }
          // if in (but not head of) parallel linked list ...
          else if (stat.config.flowMode == Constants.stationParallel) {
              if (parallelIterator.hasNext()) {
                  isHead = false;
                  stat = (StationLocal) parallelIterator.next();
//System.out.println("go to || stat " + stat.name);      
                  continue;
              }
          }

          isHead = true;
          // if in main linked list ...
          if (mainIterator.hasNext()) {
              stat = (StationLocal) mainIterator.next();
//System.out.println("go to main stat " + stat.name);      
          }
      }

      // enter # of stations
      Event.intToBytes(statCount, info, offset);

    return byteSize;
  }


  /**
   * Get attachment data for sending over the network.
   *
   * @param info byte array to hold attachment data
   * @param offset offset of the byte array
   * @param attsMax limit on number of attachments to gather data on
   * @return size of the data in bytes placed into the byte array
   */
  private int writeAttachmentData(byte[] info, int offset, int attsMax) {
    // Since we're not grabbing any mutexes, the number of attachments may have
    // changed between when the buffer size was set and now. Therefore, do not
    // exceed "attsMax" number of attachments.

    if (attsMax == 0) {
      Event.intToBytes(0, info, offset);
      return 4;
    }

    int len1, len2;
    int off = offset;
    int eventsOwned;
    int byteSize = 0;
    int attCount = 0;

    off += 4;
    byteSize += 4;

    for (AttachmentLocal att :  attachments.values()) {

      Event.intToBytes(att.id, info, off);
      Event.intToBytes(-1, info, off+=4); // no ET "processes" in Java
      Event.intToBytes(att.station.id, info, off+=4);
      Event.intToBytes(-1, info, off+=4);
      Event.intToBytes((att.waiting? Constants.attBlocked : Constants.attUnblocked), info, off+=4);
      Event.intToBytes((att.wakeUp? Constants.attQuit : Constants.attContinue), info, off+=4);

      // find out how many events the attachment owns
      eventsOwned = 0;

      for (Event ev : events.values()) {
        if (ev.owner == att.id) {
          eventsOwned++;
        }
      }
      Event.intToBytes(eventsOwned, info, off+=4);

      Event.longToBytes(att.eventsPut,  info, off+=4);
      Event.longToBytes(att.eventsGet,  info, off+=8);
      Event.longToBytes(att.eventsDump, info, off+=8);
      Event.longToBytes(att.eventsMake, info, off+=8);

      // read strings, lengths first
      len1 = att.host.length() + 1;
      len2 = att.station.name.length() + 1;
      Event.intToBytes(len1, info, off+=8);
      Event.intToBytes(len2, info, off+=4);
//System.out.println("writeAttachments: len1 = " + len1 + ", len2 = " + len2);
      // write strings into array
      off += 4;
      byte[] outString;

      try {
        outString = att.host.getBytes("ASCII");
        System.arraycopy(outString, 0, info, off, outString.length);
        off += outString.length;
        info[off++] = 0; // C null terminator

        outString = att.station.name.getBytes("ASCII");
        System.arraycopy(outString, 0, info, off, outString.length);
        off += outString.length;
        info[off++] = 0; // C null terminator
      }
      catch (UnsupportedEncodingException ex) {}

      // track size of all data stored in buffer
      byteSize += 4*9 + 8*4 + len1 + len2;

      // if more attachments now than space allowed for, skip rest
      if (++attCount >= attsMax) {
        break;
      }

    }
//System.out.println("writeAttachments: #atts = " + attCount + ", byteSize = " + byteSize);

    // send # of attachments
    Event.intToBytes(attCount, info, offset);

    return byteSize;
  }





}




