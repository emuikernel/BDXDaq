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
 * This class defines an ET system user's attachment to a station.
 * Attachments can only be created by an ET system's
 * {@link SystemUse#attach} method. Attachments are means of designating the
 * ownership of events and keeping track of events.
 *
 * @author Carl Timmer
 */

public class Attachment {

  // keep a list or set of events we currently have out?

  /** Unique id number */
  int       id;
  /** Station the attachment is associated with */
  Station   station;
  /** Flag telling whether this attachment object is usable or the attachment it
   *  represents has been detached. Set by the user's ET system object. */
  boolean   usable;
  /** ET system the attachment is associated with */
  SystemUse sys;


  /**
   * Creates an attachment to a specific ET system and station. Attachments can
   * only be created by an ET system's {@link SystemUse#attach} method.
   *
   * @param _station   station object
   * @param _id        unique attachment id number
   * @param _sys       ET system object
   */
  Attachment(Station _station, int _id, SystemUse _sys) {
    id         = _id;
    station    = _station;
    sys        = _sys;
  }

  /**
   * Gets the object of the station attached to.
   * @return object of station attached to
   */
  public Station getStation() {return station;}
  /**
   * Gets the id number of this attachment.
   * @return id number of this attachment
   */
  public int getId() {return id;}
  /**
   * Tells if this attachment object is usable.
   * @return <code>true</code> if attachment object is usable and <code>false
   * </code> otherwise
   */
  public boolean usable()     {return usable;}

  /**
   * Gets the value of an attachment's eventsPut, eventsGet, eventsDump, or
   * eventsMake by network communication with the ET system.
   *
   * @param cmd command number
   * @see Constants
   * @return value of requested parameter
   *
   * @exception java.io.IOException
   *     if there are network communication problems
   * @exception org.jlab.coda.et.EtException
   *     if the station no longer exists
   */
  private long getLongValue(int cmd) throws IOException, EtException {
    int  err = 0;
    long val = 0;

    synchronized(sys) {
      sys.out.writeInt(cmd);
      sys.out.writeInt(id);
      sys.out.flush();
      err = sys.in.readInt();
      val = sys.in.readLong();
    }

    if (err != Constants.ok) {
      throw new EtException("this station has been revmoved from ET system");
    }

    return val;
  }

  /**
   * Gets the number of events put into the ET system by the attachment
   * @return number of events put into the ET system by the attachment
   */
  public long getEventsPut() throws IOException, EtException {
    return getLongValue(Constants.netAttPut);
  }

  /**
   * Gets the number of events gotten from the ET system by the attachment
   * @return number of events gotten from the ET system by the attachment
   */
  public long getEventsGet() throws IOException, EtException {
    return getLongValue(Constants.netAttGet);
  }

  /**
   * Gets the number of events dumped (recycled by returning to GRAND_CENTRAL
   * station) through the attachment
   * @return number of events dumped into the ET system by this attachment
   */
  public long getEventsDump() throws IOException, EtException {
    return getLongValue(Constants.netAttDump);
  }

  /**
   * Gets the number of new events gotten from the ET system by the attachment
   * @return number of new events gotten from the ET system by the attachment
   */
  public long getEventsMake() throws IOException, EtException {
    return getLongValue(Constants.netAttMake);
  }
}



