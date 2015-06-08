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

/**
 * This class defines a station for ET system use.
 *
 * @author Carl Timmer
 */

public class StationLocal extends Thread implements EventSelectable {

  /** ET system object. */
  private SystemCreate  sys;
  /** Unique id number. */
  int                   id;
  /** Unique station name. */
  String                name;
  /** Station configuration object. */
  StationConfig         config;
  /** Station status. It may have the values {@link Constants#stationUnused},
   *  {@link Constants#stationCreating}, {@link Constants#stationIdle}, and
   *  {@link Constants#stationActive}. */
  volatile int          status;
  /** Flag telling this station to kill the conductor thread */
  volatile boolean      killConductor;
  /** Flag telling if this station was the last to receive an event
   *  when using the round-robin selection method for a parallel group
   *  of stations. */
  volatile boolean      wasLast;
  /** Object used to lock a mutex when events are being transferred by the
   *  conductor thread. */
  private byte[]        stopTransfer;
  /** If this station is the first in a linked list of parallel stations,
   *  this list contains all the parallel stations in that group.*/
  LinkedList<StationLocal> parallelStations;  // protected by stopTransfer & systemLock


  /** Input list of events. */
  EventList inputList;
  /** Output list of events. */
  EventList outputList;

  /** Set of attachments to this station. */
  HashSet<AttachmentLocal> attachments;

  /** Predefined event selection method used when the station's select mode
   *  is {@link Constants#stationSelectMatch}. */
  EventSelectable selector;

  /**
   * Creates a new StationLocal object.
   * @param _sys ET system object
   * @param _name station name
   * @param _config station configuration
   * @param _id unique station id number
   * @exception org.jlab.coda.et.EtException
   *     if the station cannot load the selectClass
   */
  StationLocal(SystemCreate _sys, String _name, StationConfig _config, int _id)
                                                throws EtException {
    id               = _id;
    sys              = _sys;
    name             = _name;
    config           = new StationConfig(_config);
    status           = Constants.stationUnused;
    stopTransfer     = new byte[0];
    parallelStations = new LinkedList<StationLocal>();

    inputList  = new EventList();
    outputList = new EventList();

    // attachments
    attachments = new HashSet<AttachmentLocal>(Constants.attachmentsMax);

    // user event selection routine
    selector = this;
    if (config.selectMode == Constants.stationSelectUser) {
      // instantiate object of proper class
      try {
	    Object f = Class.forName(config.selectClass).newInstance();
	    selector = (EventSelectable) f;
      }
      catch (ClassNotFoundException ex) {
        throw new EtException("station cannot load select class " + config.selectClass);
      }
      catch (InstantiationException ex) {
        throw new EtException("station cannot instantiate class " + config.selectClass);
      }
      catch (IllegalAccessException ex) {
        throw new EtException("station cannot load class " + config.selectClass);
      }

      if (sys.config.debug >= Constants.debugInfo) {
	    System.out.println(name + " loaded select class " + config.selectClass);
      }
    }
  }
  
  
  /** Gets the station id number.
   *  @return station id number */
  public int getStationId() {return id;}
  
  
  /**
   * Method used to add a new station to all the relevant linked lists of stations.
   * @param newStation station object
   * @param position the desired position in the main linked list of stations
   * @param parallelPosition the desired position of a parallel station in the
   *      group of parallel stations it's being added to
   * @exception org.jlab.coda.et.EtException
   *     if trying to add an incompatible parallel station to an existing group
   *     of parallel stations or to the head of an existing group of parallel
   *     stations.
   */
  private void insertStation(StationLocal newStation, int position, int parallelPosition)
          throws EtException {
      // If GRAND_CENTRAL is only existing station, or if we're at
      // or past the end of the linked list, put station on the end
      if ((sys.stations.size() < 2) ||
              (position >= sys.stations.size()) ||
              (position == Constants.end)) {

          sys.stations.add(newStation);
          if (newStation.config.flowMode == Constants.stationParallel) {
              newStation.parallelStations = new LinkedList<StationLocal>();
              newStation.parallelStations.add(newStation);
          }
      }
      // else, put the station in the desired position in the middle somewhere
      else {
          StationLocal stat = (StationLocal) sys.stations.get(position);

          // if the station in "position" and this station are both parallel ...
          if ((newStation.config.flowMode == Constants.stationParallel) &&
                  (stat.config.flowMode == Constants.stationParallel) &&
                  (parallelPosition != Constants.newHead)) {

              // If these 2 stations have imcompatible definitions or we're trying to place
              // a parallel station in the first (already taken) spot of its group ...
              if (!StationConfig.compatibleParallelConfigs(stat.config, newStation.config)) {
                  throw new EtException("trying to add incompatible parallel station\n");
              }
              else if (parallelPosition == 0) {
                  throw new EtException("trying to add parallel station to head of existing parallel group\n");
              }

              // Add this parallel station in the "parallelPosition" slot in the
              // parallel linked list or to the end if parallelPosition = Constants.end.
              if ((parallelPosition == Constants.end) ||
                      (parallelPosition > parallelStations.size())) {
                  stat.parallelStations.add(newStation);
              }
              else {
                  stat.parallelStations.add(parallelPosition, newStation);
              }
          }
          else {
              sys.stations.add(position, newStation);
              if (newStation.config.flowMode == Constants.stationParallel) {
                  newStation.parallelStations = new LinkedList<StationLocal>();
                  newStation.parallelStations.add(newStation);
              }
          }
      }
  }


    /**
   * Method used to remove a station from all relevant linked lists of stations.
   * @param station station object
   */
    private void deleteStation(StationLocal station) {
        // The only tricky part in removing a station is to remember that it may not
        // be in the main linked list if it is a parallel station.

        // if the station is in the main linked list ...
        if (sys.stations.contains(station)) {
            // remember where the station was located
            int index = sys.stations.indexOf(station);
            // remove it from main list
            sys.stations.remove(station);

            // if it's not a parallel station, we're done
            if (station.config.flowMode == Constants.stationSerial) {
                return;
            }

            // if the station is parallel, it's the head of another linked list.
            station.parallelStations.removeFirst();

            // if no other stations in the group, we're done
            if (station.parallelStations.size() < 1) {
                station.parallelStations = null;
                return;
            }

            // If there are other stations in the group, make sure that the linked
            // list of parallel stations is passed on to the next member. And put
            // the new head of the parallel list into the main list.
            StationLocal nextStation = (StationLocal) station.parallelStations.getFirst();
            nextStation.parallelStations = station.parallelStations;
            station.parallelStations = null;
            sys.stations.add(index, nextStation);
        }

        // else if it's not in the main linked list, we'll have to hunt it down
        else {
            // loop thru all stations in main list
            for (StationLocal nextStation : sys.stations) {
                // If it's a parallel station, try to remove "station" from the
                // list of parallel stations registered with it.
                if (nextStation.parallelStations != null) {
                    if (nextStation.parallelStations.remove(station)) {
                        // we got it
                        return;
                    }
                }
            }
        }
    }

    // A series of methods to encapsulate the methods used to modify the
  // position of a station in the linked lists of stations follows.
  // These are necessary in Java because mutexes cannot be grabbed
  // and released - only sections of code can be synchronized.
  // Thus the successive grabbing of a list of mutexes needs to be
  // done recursively starting with GRAND_CENTRAL.
  
  /**
   * Method for use by {@link SystemCreate#createStation} to grab all stations'
   * transfer locks and stop all event transfer before adding a new station to
   * the ET system's linked lists of stations. This method is called recursively.
   * @param newStation station object
   * @param position the desired position in the main linked list of stations
   * @param parallelPosition the desired position of a parallel station in the
   *     group of parallel stations it's being added to
   * @exception org.jlab.coda.et.EtException
   *     if trying to add an incompatible parallel station to an existing group
   *     of parallel stations or to the head of an existing group of parallel
   *     stations.
   */
  void addStation(StationLocal newStation, int position, int parallelPosition)
                                                             throws EtException {
    StationLocal nextStation;
    int nextIndex = sys.stations.indexOf(this) + 1;
    ListIterator i = sys.stations.listIterator(nextIndex);
    if (i.hasNext()) {
      nextStation = (StationLocal) i.next();
      synchronized(stopTransfer) {
        nextStation.addStation(newStation, position, parallelPosition);
      }
    }
    else {
      synchronized(stopTransfer) {
        insertStation(newStation, position, parallelPosition);
      }
    }
  }

  /**
   * Method for use by {@link SystemCreate#removeStation} to grab all stations'
   * transfer locks and stop all event transfer before removing a station from
   * the ET system's linked lists of stations. This method is called recursively.
   * @param station station object
   */
  void removeStation(StationLocal station) {
    StationLocal nextStation;
    int nextIndex = sys.stations.indexOf(this) + 1;
    ListIterator i = sys.stations.listIterator(nextIndex);
    if (i.hasNext()) {
      nextStation = (StationLocal) i.next();
      synchronized(stopTransfer) {
        nextStation.removeStation(station);
      }
    }
    else {
      synchronized(stopTransfer) {
        deleteStation(station);
      }
    }
  }

  /**
   * Method for use by {@link SystemCreate#removeStation} to grab all stations'
   * transfer locks and stop all event transfer before moving a station in
   * the ET system's linked lists of stations. This method is called recursively.
   * @param station station object
   * @param position the desired position in the main linked list of stations
   * @param parallelPosition the desired position of a parallel station in the
   *      group of parallel stations it's being added to
   * @exception org.jlab.coda.et.EtException
   *     if trying to move an incompatible parallel station to an existing group
   *     of parallel stations or to the head of an existing group of parallel
   *     stations.
   */
  void moveStation(StationLocal station, int position, int parallelPosition)
                                                             throws EtException {
    StationLocal nextStation;
    int nextIndex = sys.stations.indexOf(this) + 1;
    ListIterator i = sys.stations.listIterator(nextIndex);
    if (i.hasNext()) {
      nextStation = (StationLocal) i.next();
      synchronized(stopTransfer) {
        nextStation.moveStation(station, position, parallelPosition);
      }
    }
    else {
      synchronized(stopTransfer) {
        deleteStation(station);
        insertStation(station, position, parallelPosition);
      }
    }
  }


  /**
   * Method for use by {@link SystemCreate#detach} and {@link SystemCreate#detach}
   * to grab all stations' transfer locks and stop all event transfer before
   * changing a station's status. This method is called recursively.
   * @param station station object
   * @param status the desired status of the station
   */
  void changeStationStatus(StationLocal station, int status) {
    StationLocal nextStation;
    int nextIndex = sys.stations.indexOf(this) + 1;
    ListIterator i = sys.stations.listIterator(nextIndex);
    if (i.hasNext()) {
      nextStation = (StationLocal) i.next();
      synchronized(stopTransfer) {
        nextStation.changeStationStatus(station, status);
      }
    }
    else {
      synchronized(stopTransfer) {
        station.status = status;
      }
    }
  }


  /**
   * Method to dynamically set a station's blocking mode.
   * @param mode blocking mode value
   */
  void setBlockMode(int mode) {
    if (config.blockMode == mode) return;
    synchronized(sys.stationLock) {
      synchronized(inputList) {
        config.blockMode = mode;
      }
    }
  }
  
  /**
   * Method to dynamically set a station's cue.
   * @param cue cue value
   */
  void setCue(int cue) {
    if (config.cue == cue) return;
    synchronized(sys.stationLock) {
      synchronized(inputList) {
	    config.cue = cue;
      }
    }
  }
  
  /**
   * Method to dynamically set a station's prescale.
   * @param prescale prescale value
   */
  void setPrescale(int prescale) {
    if (config.prescale == prescale) return;
    synchronized(sys.stationLock) {
      synchronized(inputList) {
	    config.prescale = prescale;
      }
    }
  }
  

  /**
   * Method to dynamically set a station's select integers.
   * @param select array of selection integers
   */
  void setSelectWords(int[] select) {
    if (config.select == select) return;
    synchronized(sys.stationLock) {
      synchronized(inputList) {
	    config.select = (int[]) select.clone();
      }
    }
  }
  
  /**
   * Method to dynamically set a station's user mode.
   * @param mode user mode value
   */
  void setUserMode(int mode) {
    if (config.userMode == mode) return;
    synchronized(sys.stationLock) {
      config.userMode = mode;
    }
  }
  

  /**
   * Method to dynamically set a station's restore mode.
   * @param mode restore mode value
   */
  void setRestoreMode(int mode) {
    if (config.restoreMode == mode) return;
    synchronized(sys.stationLock) {
      config.restoreMode = mode;
    }
  }
  

  /**
   * When selectMode equals {@link Constants#stationSelectMatch}, this
   * becomes the station's selection method.
   * @param sys ET system object
   * @param stat station object
   * @param ev event object being evaluated
   * @see EventSelectable
   */
  public boolean select(SystemCreate sys, StationLocal stat, Event ev) {
    boolean result = false;

    for (int i=0; i < Constants.stationSelectInts ; i++) {
      if (i%2 == 0) {
	    result = result || ((stat.config.select[i] != -1) &&
                            (stat.config.select[i] == ev.control[i]));
      }
      else {
	    result = result || ((stat.config.select[i] != -1) &&
                           ((stat.config.select[i] & ev.control[i]) != 0));
      }
    }
    return result;
  }

/**
 * Shell's method of sorting from "Numerical Recipes" slightly modified.
 * It is assumed that a and b have indexes from 1 to n. Since the input
 * arrays will start at 0 index, put nonsense in the first element.
 */
private void shellSort(int n, int[] a, int[] b) {
    int i, j, inc, v, w;
    inc = 1;
    do {
        inc *= 3;
        inc++;
    } while (inc <= n);

    do {
        inc /= 3;
        for (i = inc + 1; i <= n; i++) {
            v = a[i];
            w = b[i];
            j = i;
            while (a[j - inc] > v) {
                a[j] = a[j - inc];
                b[j] = b[j - inc];
                j -= inc;
                if (j <= inc) break;
            }
            a[j] = v;
            b[j] = w;
        }
    } while (inc > 1);
}


    /**
   * Method to implement thread conducting events between stations. This
   * conductor places all events that go into a single station into one list
   * then writes it. It looks downstream, one station at a time, and repeats
   * the process.
   * It optimizes for cases in which the next station is GRAND_CENTRAL or one
   * which takes all events. In those cases, it dumps everything in that
   * station's input list without bothering to sort or filter it.
   */
    public void run() {
        int count, prescale, available, getListSize, position;
        long listTry;
        Event ev = null;
        boolean writeAll, parallelIsActive = false, rrobinOrEqualcue = false;
        StationLocal currentStat = null, stat = null, firstActive, startStation;
        List<Event> subList = null;
        ListIterator statIterator, pIterator = null;

        // inputList of next station
        EventList inList = null;
        // events read from station's outputList
        ArrayList<Event> getList = new ArrayList<Event>(sys.config.numEvents);
        // events to be put into the next station's inputList
        ArrayList<Event> putList = new ArrayList<Event>(sys.config.numEvents);

        // store some constants in stack variables for greater speed
        final int idle = Constants.stationIdle;
        final int active = Constants.stationActive;
        final int blocking = Constants.stationBlocking;
        final int nonBlocking = Constants.stationNonBlocking;
        final int selectAll = Constants.stationSelectAll;
        final int parallel = Constants.stationParallel;

        if (name.equals("GRAND_CENTRAL")) {
            status = active;
        }
        else {
            status = idle;
        }

        while (true) {
            // wait for events
            synchronized (outputList) {
                while (outputList.events.size() < 1) {
                    try {
                        outputList.wait();
                    }
                    catch (InterruptedException ex) {
                    }
                    if (killConductor) {
                        return;
                    }
                }
            }

            // grab all events in station's outputList
            outputList.get(getList);

            // reinit items
            writeAll = false;

            // allow no change to linked list of created stations
            synchronized (stopTransfer) {
                // find next station in main linked list
                position = sys.stations.indexOf(this);
                // If we're a parallel station which is NOT the head of its group,
                // find our position in the main linked list
                if (position < 0) {
                    position = 1;
                    for (ListIterator i = sys.stations.listIterator(1); i.hasNext();) {
                        stat = (StationLocal) i.next();
                        if (stat.config.flowMode == parallel) {
                            // we've found the group of parallel stations we belong to & our position
                            if (stat.parallelStations.indexOf(this) > -1) {
                                break;
                            }
                        }
                        position++;
                    }
                }

                statIterator = sys.stations.listIterator(position + 1);
                if (statIterator.hasNext()) {
                    currentStat = (StationLocal) statIterator.next();
                }
                else {
                    // the next station is GrandCentral, put everything in it
                    currentStat = (StationLocal) sys.stations.getFirst();
                    inList = currentStat.inputList;
                    synchronized (inList) {
                        inList.putInLow(getList);
                        getList.clear();
                        inList.notifyAll();
                    }
                    continue;
                }

                inList = currentStat.inputList;

                while (getList.size() > 0) {
                    parallelIsActive = false;
                    rrobinOrEqualcue = false;
                    startStation = null;
                    firstActive = null;

                    // if this is a parallel station ...
                    if (currentStat.config.flowMode == Constants.stationParallel) {
                        // Are any of the parallel stations active or can we skip the bunch?
                        pIterator = currentStat.parallelStations.listIterator();
                        while (pIterator.hasNext()) {
                            stat = (StationLocal) pIterator.next();
                            if (stat.status == Constants.stationActive) {
                                parallelIsActive = true;
                                firstActive = stat;
                                break;
                            }
                        }
                        // At this point pIterator will give the station after firstActive
                        // with the following next().

                        // Which algorithm are we using?
                        if (parallelIsActive &&
                                ((currentStat.config.selectMode == Constants.stationSelectRRobin) ||
                                 (currentStat.config.selectMode == Constants.stationSelectEqualCue))) {
                            rrobinOrEqualcue = true;
                        }
                    }

                    // if not rrobin/equalcue & station(s) is(are) active ...
                    if (!rrobinOrEqualcue &&
                            (parallelIsActive || (currentStat.status == Constants.stationActive))) {

                        if (currentStat.config.flowMode == Constants.stationParallel) {
                            // Skip to first active parallel station
                            currentStat = firstActive;
                            inList = currentStat.inputList;
                        }

                        // Loop through all the active parallel stations if necessary.
                        parallelDo:
                        do {
                            // allow no exterior change to inputList
                            synchronized (inList) {
                                // if GrandCentral, put everything into it ...
                                if (currentStat.id == 0) {
                                    writeAll = true;
                                }

                                // all events, blocking
                                else if ((currentStat.config.selectMode == selectAll) &&
                                         (currentStat.config.blockMode == blocking)) {

                                    // if prescale=1, dump everything into station
                                    getListSize = getList.size();
                                    if (currentStat.config.prescale == 1) {
                                        writeAll = true;
                                    }
                                    else {
                                        prescale = currentStat.config.prescale;
                                        listTry = inList.eventsTry;
                                        subList = getList.subList(0, (int) ((listTry + getListSize) / prescale - listTry / prescale));
                                        putList.addAll(subList);
                                        subList.clear();
                                    }
                                    inList.eventsTry += getListSize;
                                }

                                // all events, nonblocking
                                else if ((currentStat.config.selectMode == selectAll) &&
                                        (currentStat.config.blockMode == nonBlocking)) {
                                    if (inList.events.size() < currentStat.config.cue) {
                                        count = currentStat.config.cue - inList.events.size();
                                        available = getList.size();
                                        subList = getList.subList(0, (count > available) ? available : count);
                                        putList.addAll(subList);
                                        subList.clear();
                                    }
                                }

                                //  condition (user or match), blocking
                                else if (currentStat.config.blockMode == blocking) {
                                    prescale = currentStat.config.prescale;
                                    for (ListIterator i = getList.listIterator(); i.hasNext();) {
                                        ev = (Event) i.next();
                                        // apply selection method
                                        if (currentStat.selector.select(sys, currentStat, ev)) {
                                            // apply prescale
                                            if (((inList.eventsTry++) % prescale) == 0) {
                                                putList.add(ev);
                                                i.remove();
                                            }
                                        }
                                    }
                                }

                                // condition (user or match) + nonblocking
                                else if (currentStat.config.blockMode == nonBlocking) {
                                    if (inList.events.size() < currentStat.config.cue) {
                                        count = currentStat.config.cue - inList.events.size();
                                        for (ListIterator i = getList.listIterator(); i.hasNext();) {
                                            ev = (Event) i.next();
                                            // apply selection method
                                            if (currentStat.selector.select(sys, currentStat, ev)) {
                                                putList.add(ev);
                                                i.remove();
                                                if (--count < 1) {
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }

                                // if items go in this station ...
                                if ((putList.size() > 0) || (writeAll)) {
                                    // if grandcentral
                                    if (currentStat.id == 0) {
                                        inList.putInLow(getList);
                                        getList.clear();
                                        writeAll = false;
                                    }

                                    else {
                                        if (writeAll) {
                                            inList.putAll(getList);
                                            getList.clear();
                                            writeAll = false;
                                        }
                                        else {
                                            inList.putAll(putList);
                                            putList.clear();
                                        }
                                    }
                                    // signal reader that new events are here
                                    inList.notifyAll();
                                } // if items go in this station
                            } // end of inputList synchronization

                            // go to next active parallel station, if there is one
                            if (parallelIsActive) {
                                do {
                                    if (pIterator.hasNext()) {
                                        stat = (StationLocal) pIterator.next();
                                        if (stat.status == Constants.stationActive) {
                                            currentStat = stat;
                                            inList = currentStat.inputList;
                                            break;
                                        }
                                    }
                                    else {
                                        break parallelDo;
                                    }
                                } while (stat.status != Constants.stationActive);
                            }

                            // loop through active parallel stations if necessary
                        } while (parallelIsActive && (getList.size() > 0));

                    } // if station active and not rrobin or equalcue

                    // Implement the round-robin & equal-cue algorithms for dispensing
                    // events to a single group of parallel stations.
                    else if (rrobinOrEqualcue && parallelIsActive) {

                        int num, extra, lastEventIndex = 0, eventsAlreadyPut, numActiveStations = 0;
                        int index, numOfEvents, min, eventsToPut, eventsLeft;
                        int eventsPerStation, nextHigherCue, eventsDoledOut, stationsWithSameCue;
                        int[] numEvents;

                        if (currentStat.config.selectMode == Constants.stationSelectRRobin) {
                            // Flag to start looking for station that receives first round-robin event
                            boolean startLooking = false;
                            stat = currentStat;
                            pIterator = currentStat.parallelStations.listIterator(1);

                            while (true) {
                                // for each active station ...
                                if (stat.status == Constants.stationActive) {
                                    if (startLooking) {
                                        // This is the first active station after
                                        // the last station to receive an event.
                                        startStation = stat;
                                        startLooking = false;
                                    }
                                    numActiveStations++;
                                }

                                // Find last station to receive a round-robin event and start looking
                                // for the next active station to receive the first one.
                                if (stat.wasLast) {
                                    stat.wasLast = false;
                                    startLooking = true;
                                }

                                // find next station in the parallel linked list
                                if (pIterator.hasNext()) {
                                    stat = (StationLocal) pIterator.next();
                                }
                                // else if we're at the end of the list ...
                                else {
                                    // If we still haven't found a place to start the round-robin
                                    // event dealing, make it the first active station.
                                    if (startStation == null) {
                                        startStation = firstActive;
                                    }
                                    break;
                                }
                            }

                            // Find the number of events going into each station
                            num = getList.size() / numActiveStations;
                            // Find the number of events left over (not enough for another round). */
                            extra = getList.size() % numActiveStations;
                            eventsAlreadyPut = count = 0;
                            numEvents = new int[numActiveStations];

                            // Rearrange events so all those destined for a particular
                            // station are grouped together in the new array.
                            for (int i = 0; i < numActiveStations; i++) {
                                if (i < extra) {
                                    numEvents[i] = num + 1;
                                    if (i == (extra - 1)) {
                                        lastEventIndex = i;
                                    }
                                }
                                else {
                                    numEvents[i] = num;
                                }

                                if (extra == 0) {
                                    lastEventIndex = numActiveStations - 1;
                                }

                                numOfEvents = numEvents[i];

                                index = i;
                                for (int j = 0; j < numOfEvents; j++) {
                                    putList.add(getList.get(index));
                                    index += numActiveStations;
                                }
                            }

                            // Place the first event with the station after the one which
                            // received the last event in the previous round.
                            stat = startStation;
                            inList = stat.inputList;
                            count = 0;

                            // set iterator to start with the station following startStation
                            index = currentStat.parallelStations.indexOf(startStation) + 1;
                            pIterator = currentStat.parallelStations.listIterator(index);

                            while (true) {
                                // For each active parallel station ...
                                if (stat.status == Constants.stationActive) {
                                    // Mark station that got the last event
                                    if (count == lastEventIndex) {
                                        stat.wasLast = true;
                                    }

                                    // Put "eventsToPut" number of events in the next active station
                                    eventsToPut = numEvents[count++];

                                    if (eventsToPut > 0) {
                                        synchronized (inList) {
                                            subList = putList.subList(eventsAlreadyPut, eventsAlreadyPut + eventsToPut);
                                            inList.putAll(subList);
                                            inList.eventsTry += eventsToPut;
                                            // signal reader that new events are here
                                            inList.notifyAll();
                                        }

                                        eventsAlreadyPut += eventsToPut;
                                    }
                                }

                                // Find next active station
                                if (count >= numActiveStations) {
                                    break;
                                }
                                else if (pIterator.hasNext()) {
                                    stat = (StationLocal) pIterator.next();
                                    inList = stat.inputList;
                                }
                                else {
                                    // Go back to the first active parallel station
                                    stat = firstActive;
                                    inList = stat.inputList;
                                    index = currentStat.parallelStations.indexOf(stat) + 1;
                                    pIterator = currentStat.parallelStations.listIterator(index);
                                }
                            } // while (forever)

                            putList.clear();

                        } // if round-robin

                        // else if equal-cue algorithm ...
                        else {
                            eventsLeft = getList.size();
                            eventsDoledOut = 0;
                            eventsAlreadyPut = 0;
                            stationsWithSameCue = 0;

                            // Array that keeps track of original station order, and
                            // one that contains input list counts.
                            // Give 'em an extra element as the sorting routine
                            // assumes a starting index of 1.
                            int[] place = new int[sys.config.stationsMax + 1];
                            int[] inListCount = new int[sys.config.stationsMax + 1];
                            for (int i = 1; i <= sys.config.stationsMax; i++) {
                                place[i] = i;
                            }

                            stat = firstActive;
                            while (true) {
                                // For each active station ...
                                if (stat.status == Constants.stationActive) {
                                    // Find total # of events in stations' input lists.
                                    // Store this information as it will change and we don't
                                    // really want to grab all the input mutexes to make
                                    // sure these values don't change.
                                    inListCount[numActiveStations + 1] = stat.inputList.events.size();

                                    // Total number of active stations
                                    numActiveStations++;
                                }

                                // find next station in the parallel linked list
                                if (pIterator.hasNext()) {
                                    stat = (StationLocal) pIterator.next();
                                }
                                else {
                                    break;
                                }
                            }

                            // Sort the input lists (cues) according to number of events. The "place"
                            // array remembers the place in the presorted array of input lists.
                            // Arrays to be sorted are assumed to have indexes from 1 to n,
                            // so the first element contains nonsense.
                            shellSort(numActiveStations, inListCount, place);

                            // To determine which stations get how many events:
                            // Take the lowest cues, add enough to make them equal
                            // to the next higher cue. Continue doing this until all
                            // are equal. Evenly divide any remaining events.
                            nextHigherCue = 0;
                            min = inListCount[1];
                            numEvents = new int[numActiveStations];

                            while (eventsDoledOut < eventsLeft) {
                                // Find how many cues have the lowest # of events in them
                                stationsWithSameCue = 0;
                                for (int i = 1; i <= numActiveStations; i++) {
                                    // Does events in cue + events we've just given it = min?
                                    if (min == inListCount[i] + numEvents[place[i] - 1]) {
                                        stationsWithSameCue++;
                                    }
                                    else {
                                        nextHigherCue = inListCount[i];
                                        break;
                                    }
                                }

                                // If all stations have same # of events, or if there are not enough
                                // events to fill each lowest cue to level of the next higher cue,
                                // we spread available events between them all ...
                                if ((stationsWithSameCue == numActiveStations) ||
                                        ((eventsLeft - eventsDoledOut) < ((nextHigherCue - min) * stationsWithSameCue)))
                                {
                                    eventsToPut = eventsLeft - eventsDoledOut;
                                    eventsPerStation = eventsToPut / stationsWithSameCue;
                                    extra = eventsToPut % stationsWithSameCue;
                                    count = 0;
                                    for (int i = 1; i <= stationsWithSameCue; i++) {
                                        if (count++ < extra) {
                                            numEvents[place[i] - 1] += eventsPerStation + 1;
                                        }
                                        else {
                                            numEvents[place[i] - 1] += eventsPerStation;
                                        }
                                    }
                                    break;
                                }
                                // Else, fill the lowest cues to the level of the next higher cue
                                // and repeat the cycle.
                                else {
                                    eventsPerStation = nextHigherCue - min;
                                    for (int i = 1; i <= stationsWithSameCue; i++) {
                                        numEvents[place[i] - 1] += eventsPerStation;
                                    }
                                    min = nextHigherCue;
                                }
                                eventsDoledOut += eventsPerStation * stationsWithSameCue;
                            }

                            stat = firstActive;
                            count = 0;
                            index = currentStat.parallelStations.indexOf(stat) + 1;
                            pIterator = currentStat.parallelStations.listIterator(index);

                            while (true) {
                                // for each active parallel station ...
                                if (stat.status == Constants.stationActive) {

                                    if ((eventsToPut = numEvents[count++]) < 1) {
                                        // find next station in the parallel linked list
                                        if (pIterator.hasNext()) {
                                            stat = (StationLocal) pIterator.next();
                                            continue;
                                        }
                                        else {
                                            break;
                                        }
                                    }

                                    // Put "eventsToPut" number of events in the next active station
                                    inList = stat.inputList;
                                    synchronized (inList) {
                                        subList = getList.subList(eventsAlreadyPut, eventsAlreadyPut + eventsToPut);
                                        inList.putAll(subList);
                                        inList.eventsTry += eventsToPut;
                                        // signal reader that new events are here
                                        inList.notifyAll();
                                    }

                                    eventsAlreadyPut += eventsToPut;
                                }

                                // Find next station in the parallel linked list
                                if (pIterator.hasNext()) {
                                    stat = (StationLocal) pIterator.next();
                                }
                                else {
                                    break;
                                }
                            } // while(true)
                        } // else if equal-cue algorithm

                        getList.clear();

                    } // Implement the round-robin & equal-cue algorithms

                    if (currentStat.id == 0) {
                        break;
                    }

                    // find next station
                    if (statIterator.hasNext()) {
                        currentStat = (StationLocal) statIterator.next();
                    }
                    else {
                        currentStat = (StationLocal) sys.stations.getFirst();
                    }
                    inList = currentStat.inputList;

                } // while(getList.size() > 0), events left to put
            } // stop transfer synchronization
        } // while(true)
    } // run method


}
