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
 * This class defines a linked list of events for use as either a station's
 * input or output list in a station.
 *
 * @author Carl Timmer
 */

class EventList {

  /** Linked list of events. */
  LinkedList events;
  /** Number of events put into this list. */
  long       eventsIn;
  /** Number of events taken out of this list. */
  long       eventsOut;

  // input list members only

  /** Number of events tried to put into this list when used with prescaling. */
  long       eventsTry;
  /** Flag telling if the list is locked by a user reading events from it. */
  volatile boolean    locked;
  /** Flag telling the list to wake up the user waiting to read events. */
  private  boolean    wakeAll;
  /** Count of the number of users sleeping on reading events. Last one to wake
   *  up needs to reset wakeAll. */
  private int        waitingCount;

  // output list members only

  /** Position of the last high priority event in the linked list. */
  private int        lastHigh;


  /** Creates a new EventList object. */
  EventList() {
    events = new LinkedList();
  }

  // methods for waking up reading attachments on input lists

  /** Wake up an attachment waiting to read events from this list.
   *  @param att attachment to be woken up */
  synchronized void wakeUp(AttachmentLocal att) {
    if (att.waiting == false) {
      return;
    }
    att.wakeUp = true;
    notifyAll();
  }

  /** Wake up all attachments waiting to read events from this list. */
  synchronized void wakeUpAll() {
    if (waitingCount < 1) {
      return;
    }
    wakeAll = true;
    notifyAll();
  }


  /**
   * Put all events into a station's input list as low priority. This is
   * synchronized and used in conductor threads and in the initial filling of
   * GRAND_CENTRAL station.
   * @param newEvents list of events to put
   */
  void putInLow(List newEvents) {
    // add all events to list's end
    events.addAll(newEvents);
    // keep stats
    eventsIn += newEvents.size();
  }

  /**
   * Synchronized version of putInLow for user to dump events into
   * GRAND_CENTRAL station.
   * @param newEvents array of events to put
   */
  synchronized void putInGC(Event[] newEvents) {
    // convert array to list and put as low priority events
    putInLow(Arrays.asList(newEvents));
  }

  /**
   * Synchronized version of putInLow for user to dump events into
   * GRAND_CENTRAL station.
   * @param newEvents list of events to put
   */
  synchronized void putInGC(List newEvents) {
    putInLow(newEvents);
  }

  /**
   * Put all events into the list regardless of how many are already in it.
   * Synchronized and used only in conductor threads to put events into a
   * station's input list. All high priority events are listed first in
   * newEvents.
   * @param newEvents list of events to put
   */
  void putAll(List newEvents) {
      // number of incoming events
      int num = newEvents.size();

      // all incoming events' priorities are low or no events in this EventList
      if (events.size() == 0) {
	// adds new events to the end
//System.out.println("  putAll in as is as list EventList empty, " + newEvents.size());
	events.addAll(newEvents);
      }
      else if (((Event)newEvents.get(0)).priority == Constants.low) {
	// adds new events to the end
//System.out.println("  putAll in as is as incoming are all low pri, " + newEvents.size());
	events.addAll(newEvents);
      }

      // if any high pri events (count != 0) ...
      else {
	// find last high priority event already in list
	int highCount = 0;
//        int size = events.size();
//        for (int i=0; i < size; i++) {
//          if (((Event)events.get(i)).priority != Constants.high) {
//	    break;
//	    }
//	    highCount++;
//        }
	for (ListIterator i = events.listIterator(); i.hasNext(); ) {
	  if (((Event)i.next()).priority != Constants.high) {
	    break;
	  }
	  highCount++;
	}
//System.out.println("  putAll last high of input list = " + highCount);

	// add new high pri items
	Event ev;
	int newHighCount = 0;
        int size = newEvents.size();
        for (int i=0; i < size; i++) {
	  ev = (Event)newEvents.get(i);
	  if (ev.priority != Constants.high) {
	    break;
          }
//System.out.println("  putAll add high " + ev.id + " at " + (highCount + newHighCount));
	  events.add(highCount + newHighCount++, ev);
        }

//	for (ListIterator i = newEvents.listIterator(); i.hasNext(); ) {
//	  ev = (Event)i.next();
//	  if (ev.priority != Constants.high) {
//	    break;
//	  }
//System.out.println("  putAll add high " + ev.id + " at " + (highCount + newHighCount));
//	  events.add(highCount + newHighCount++, ev);
//	}

	// rest are low pri, add to end
	if (newHighCount < num) {
//System.out.println("  putAll add " + (num - newHighCount) + " lows at end");
	  events.addAll(newEvents.subList(newHighCount, num));
	}
      }
      // keep stats
      eventsIn += num;
      return;
  }

  /**
   * For user to put all events into station's output list. High & low
   * priorities may be mixed up in the newEvents list. May also be used to
   * restore events to the input list when user connection is broken.
   * @param newEvents array of events to put
   */
  synchronized void put(Event[] newEvents) {
      // if no events in list, initialize lastHigh
      if (events.size() == 0) {
	lastHigh = 0;
      }

      // put events in one-by-one - with place depending on priority
      int size = newEvents.length;
      for (int i=0; i < size; i++) {
        // if low priority event, add to the list end
        if (newEvents[i].priority == Constants.low) {
//System.out.println(" put in low - " + ev.id);
	  events.addLast(newEvents[i]);
        }
        // else if high pri event, add after other high priority events
        else {
//System.out.println(" put in high - " + ev.id);
          events.add(lastHigh++, newEvents[i]);
        }
      }
      notify();
      return;
  }

  /**
   * For user to put all events into station's output list. High & low
   * priorities may be mixed up in the newEvents list. May also be used to
   * restore events to the input list when user connection is broken.
   * @param newEvents list of events to put
   */
  synchronized void put(ArrayList newEvents) {
      // if no events in list, initialize lastHigh
      if (events.size() == 0) {
	lastHigh = 0;
      }

      // put events in one-by-one - with place depending on priority
      Event ev;
      int size = newEvents.size();
      for (int i=0; i < size; i++) {
        ev = (Event) newEvents.get(i);
        // if low priority event, add to the list end
        if (ev.priority == Constants.low) {
//System.out.println(" put in low - " + ev.id);
	  events.addLast(ev);
        }
        // else if high pri event, add after other high priority events
        else {
//System.out.println(" put in high - " + ev.id);
          events.add(lastHigh++, ev);
        }
      }
      notify();
      return;
  }


  /**
   * Used only by conductor to get all events from a station's output list.
   * @param eventsToGo list of event to get
   */
  synchronized void get(ArrayList eventsToGo) {
      eventsToGo.addAll(events);
      eventsOut += events.size();
      events.clear();
      return;
  }


  /**
   * For an attachment (in TcpServer thread) to get an array of events.
   * @param att attachment
   * @param mode wait mode
   * @param microSec time in microseconds to wait if timed wait mode
   * @param quantity number of events desired
   *
   * @exception org.jlab.coda.et.EtEmptyException
   *     if the mode is asynchronous and the station's input list is empty
   * @exception org.jlab.coda.et.EtTimeoutException
   *     if the mode is timed wait and the time has expired
   * @exception org.jlab.coda.et.EtWakeUpException
   *     if the attachment has been commanded to wakeup,
   */
  synchronized Event[] get(AttachmentLocal att, int mode, int microSec, int quantity)
               throws EtEmptyException, EtWakeUpException, EtTimeoutException {

      locked = true;
      int count = events.size();

      // Sleep mode is never used since it is implemented in the TcpServer
      // thread by repeated calls in timed mode.
      if (count == 0) {
	if (mode == Constants.sleep) {
	  while (count < 1) {
	    waitingCount++;
	    att.waiting = true;
	    try {wait();}
	    catch (InterruptedException ex) {}

	    // if we've been told to wakeup & exit ...
	    if (att.wakeUp || wakeAll) {
	      att.wakeUp  = false;
	      att.waiting = false;
	      // last man to wake resets variable
	      if (--waitingCount < 1) {
	        wakeAll = false;
	      }
              throw new EtWakeUpException("attachment " + att.id + " woken up");
	    }

	    att.waiting = false;
	    waitingCount--;
	    count = events.size();
	  }
	}
	else if (mode == Constants.timed) {
	  long milliSec = microSec/1000;
          int nanos = 1000*(microSec - (int)(milliSec*1000));

	  waitingCount++;
	  att.waiting = true;
//System.out.println("  get" + att.id + ": wait " + milliSec + " ms and " +
//               nanos  + " nsec");
	  try {wait(milliSec, nanos);}
	  catch (InterruptedException ex) {}

	  // if we've been told to wakeup & exit ...
	  if (att.wakeUp || wakeAll) {
	    att.wakeUp  = false;
	    att.waiting = false;
	    // last man to wake resets variable
	    if (--waitingCount < 1) {
	      wakeAll = false;
	    }
            throw new EtWakeUpException("attachment " + att.id + " woken up");
	  }

	  att.waiting = false;
	  waitingCount--;
	  count = events.size();
//System.out.println("  get" + att.id + ": woke up and counts = " + count);
	  if (count < 1) {
	    throw new EtTimeoutException("timed out");
	  }
	}
	else if (mode == Constants.async) {
	  throw new EtEmptyException("no events in list");
	}
      }

      if (quantity > count) {
        quantity = count;
      }
//System.out.println("  get"+ att.id + ": quantity = " + quantity);

      List deleteList = events.subList(0, quantity);
      Event[] eventsToGo = new Event[quantity];
      deleteList.toArray(eventsToGo);
      deleteList.clear();

      eventsOut += quantity;
      locked = false;
      return eventsToGo;
  }

}
