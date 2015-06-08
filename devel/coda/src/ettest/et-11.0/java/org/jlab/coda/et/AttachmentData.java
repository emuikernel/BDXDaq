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
 * This class holds all information about an attachment. It parses
 * the information from a stream of data sent by an ET system.
 *
 * @author Carl Timmer
 */

public class AttachmentData {

  /** Attachment's id number.
   *  @see Attachment#id
   *  @see AttachmentLocal#id */
  int num;
  /** Id number of ET process that created this attachment (only relevant in C
   *  ET systems). */
  int proc;
  /** Id number of the attachment's station.
   *  @see Attachment#station
   *  @see AttachmentLocal#station */
  int stat;
  /** Unix process id of the program that created this attachment (only
   *  relevant in C ET systems).
   *  @see AttachmentLocal#pid */
  int pid;
  /** Flag indicating if this attachment is blocked waiting to read events. Its
   *  value is {@link Constants#attBlocked} if blocked and
   *  {@link Constants#attUnblocked} otherwise.
   *  This is not boolean for C ET system compatibility.
   *  @see AttachmentLocal#waiting */
  int blocked;
  /** Flag indicating if this attachment has been told to quit trying to read
   *  events and return. Its value is {@link Constants#attQuit} if it has been
   *  told to quit and {@link Constants#attContinue} otherwise.
   *  This is not boolean for C ET system compatibility.
   *  @see AttachmentLocal#wakeUp */
  int quit;
  /** The number of events owned by this attachment */
  int eventsOwned;

  /** Number of events put back into the station.
   *  @see Attachment#getEventsPut
   *  @see AttachmentLocal#eventsPut */
  long eventsPut;
  /** Number of events gotten from the station.
   *  @see Attachment#getEventsGet
   *  @see AttachmentLocal#eventsGet */
  long eventsGet;
  /** Number of events dumped (recycled by returning to GRAND_CENTRAL) through
   *  the station.
   *  @see Attachment#getEventsDump
   *  @see AttachmentLocal#eventsDump */
  long eventsDump;
  /** Number of new events gotten from the station.
   *  @see Attachment#getEventsMake
   *  @see AttachmentLocal#eventsMake */
  long eventsMake;

  /** Name of the host running this attachment.
   *  @see AttachmentLocal#host */
  String host;
  /**  Name of the station this attachment is associated with. */
  String stationName;

  // get methods

  /** Get the attachment's id number.
   *  @see Attachment#id
   *  @see AttachmentLocal#id */
  public int getId() {return num;}
  /** Get the id number of ET process that created this attachment
   *  (only relevant in C ET systems). */
  public int getProc() {return proc;}
  /** Get the id number of the station to which this attachment belongs.
   *  @see Attachment#station
   *  @see AttachmentLocal#station */
  public int getStationId() {return stat;}
  /** Get the Unix process id of the program that created this attachment (only
   *  relevant in C ET systems).
   *  @see AttachmentLocal#pid */
  public int getPid() {return pid;}
  
  /** Indicates if this attachment is blocked waiting to read events.
   *  @see AttachmentLocal#waiting */
  public boolean blocked() {if (blocked == Constants.attBlocked) return true;
                            return false;}
  /** Indicates if this attachment has been told to quit trying to read
   *  events and return.
   *  @see AttachmentLocal#wakeUp */
  public boolean quitting() {if (quit == Constants.attQuit) return true;
                            return false;}
			    
  /** Get the number of events owned by this attachment */
  public int getEventsOwned() {return eventsOwned;}
  /** Get the number of events put back into the station.
   *  @see Attachment#getEventsPut
   *  @see AttachmentLocal#eventsPut */
  public long getEventsPut() {return eventsPut;}
  /** Get the number of events gotten from the station.
   *  @see Attachment#getEventsGet
   *  @see AttachmentLocal#eventsGet */
  public long getEventsGet() {return eventsGet;}
  /** Get the number of events dumped (recycled by returning to GRAND_CENTRAL)
   *  through the station.
   *  @see Attachment#getEventsDump
   *  @see AttachmentLocal#eventsDump */
  public long getEventsDump() {return eventsDump;}
  /** Get the number of new events gotten from the station.
   *  @see Attachment#getEventsMake
   *  @see AttachmentLocal#eventsMake */
  public long getEventsMake() {return eventsMake;}

  /** Get the name of the host running this attachment.
   *  @see AttachmentLocal#host */
  public String getHost() {return host;}
  /**  Get the name of the station this attachment is associated with. */
  public String getStationName() {return stationName;}


  /**
   *  Reads the attachment information from a data stream which is sent out by
   *  an ET system over the network.
   *  @param dis data input stream
   *  @exception java.io.IOException
   *     if data stream read error
   */
  void read(DataInputStream dis) throws IOException {
    byte[] info = new byte[68];
    dis.readFully(info);

    num         = Event.bytesToInt(info,   0);
    proc        = Event.bytesToInt(info,   4);
    stat        = Event.bytesToInt(info,   8);
    pid         = Event.bytesToInt(info,  12);
    blocked     = Event.bytesToInt(info,  16);
    quit        = Event.bytesToInt(info,  20);
    eventsOwned = Event.bytesToInt(info,  24);
    eventsPut   = Event.bytesToLong(info, 28);
    eventsGet   = Event.bytesToLong(info, 36);
    eventsDump  = Event.bytesToLong(info, 44);
    eventsMake  = Event.bytesToLong(info, 52);

    // read strings, lengths first
    int length1 = Event.bytesToInt(info, 60);
    int length2 = Event.bytesToInt(info, 64);

    if (length1+length2 > 68) {
      info = new byte[length1+length2];
    }
    dis.readFully(info, 0 , length1+length2);
    host = new String(info, 0, length1-1, "ASCII");
    stationName = new String(info, length1, length2-1, "ASCII");
  }
}



