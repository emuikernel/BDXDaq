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
 * This class holds all information about an station. It parses
 * the information from a stream of data sent by an ET system.
 *
 * @author Carl Timmer
 */

public class StationData {

  /** Station's id number.
   *  @see Station#id
   *  @see StationLocal#id */
  int num;
  /** Station's status. It may have the values {@link Constants#stationUnused},
   *  {@link Constants#stationIdle}, or {@link Constants#stationActive}.
   *  @see StationLocal#status */
  int status;
  /** Transfer mutex status. It has the value {@link Constants#mutexLocked} if
   *  locked and {@link Constants#mutexUnlocked} otherwise. This is only
   *  relevant in C ET systems, since in Java, mutexes cannot be tested without
   *  possibility of blocking. This is not boolean for C ET system compatibility.
   *  @see StationLocal#stopTransfer */
  int mutex;
  /** Number of attachments to this station.
   *  @see StationLocal#attachments */
  int attachments;
  /** Array of attachment id numbers. Only the first "attachments" 
   *  number of elements are meaningful. 
   *  @see StationLocal#attachments */
  int attIds[] = new int[Constants.attachmentsMax];

  /** Input list mutex status. It has the value {@link Constants#mutexLocked} if
   *  locked and {@link Constants#mutexUnlocked} otherwise. This is only relevant
   *  in C ET systems, since in Java, mutexes cannot be tested without the
   *  chance of blocking. This is not boolean for C ET system compatibility. */
  int  inListMutex;
  /** Number of events in the input list.
   *  @see EventList#events */
  int  inListCount;
  /** Number of events that were attempted to be put into the input list. This is
   *  relevant only when there is prescaling.
   *  @see EventList#eventsTry */
  long inListTry;
  /** Number of events that were put into the input list.
   *  @see EventList#eventsIn */
  long inListIn;

  /** Output list mutex status. It has the value {@link Constants#mutexLocked} if
   *  locked and {@link Constants#mutexUnlocked} otherwise. This is only relevant
   *  in C ET systems, since in Java, mutexes cannot be tested without the
   *  chance of blocking. This is not boolean for C ET system compatibility. */
  int  outListMutex;
  /** Number of events in the output list.
   *  @see EventList#events */
  int  outListCount;
  /** Number of events that were taken out of the output list.
   *  @see EventList#eventsOut */
  long outListOut;

  // station configuration

  /** Station configuration's flow mode.
   *  @see StationConfig#userMode */
  int flowMode;
  /** Station configuration's user mode.
   *  @see StationConfig#userMode */
  int userMode;
  /** Station configuration's restore mode.
   *  @see StationConfig#restoreMode */
  int restoreMode;
  /** Station configuration's blocking mode.
   *  @see StationConfig#blockMode */
  int blockMode;
  /** Station configuration's prescale value.
   *  @see StationConfig#prescale */
  int prescale;
  /** Station configuration's input cue size.
   *  @see StationConfig#cue */
  int cue;
  /** Station configuration's select mode.
   *  @see StationConfig#selectMode */
  int selectMode;
  /** Station configuration's select array.
   *  @see StationConfig#select */
  int select[] = new int[Constants.stationSelectInts];

  /** Name of user select function in C version ET library.
   *  @see StationConfig#selectFunction */
  String selectFunction;
  /** Name of C library containing user select function in C version ET system.
   *  @see StationConfig#selectLibrary */
  String selectLibrary;
  /** Name of Java class containing user select method in Java version ET system.
   *  @see StationConfig#selectClass */
  String selectClass;
  /** Name of station.
   *  @see Station#name
   *  @see StationLocal#name */
  String name;

  // get methods

  /** Get the station's id number.
   *  @see Station#id
   *  @see StationLocal#id */
  public int getId() {return num;}
  /** Get the station's status. It may have the values {@link Constants#stationUnused},
   *  {@link Constants#stationIdle}, or {@link Constants#stationActive}.
   *  @see StationLocal#status */
  public int getStatus() {return status;}
  /** Get the transfer mutex status. It has the value {@link Constants#mutexLocked}
   *  if locked and {@link Constants#mutexUnlocked} otherwise. This is only
   *  relevant in C ET systems. */
  public int getMutex() {return mutex;}
  /** Get the number of attachments to this station.
   *  @see StationLocal#attachments */
  public int getAttachments() {return attachments;}
  /** Get the array of attachment id numbers.
   *  @see StationLocal#attachments */
  public int[] getAttachmentIds() {return (int[]) attIds.clone();}

  /** Get the input list mutex status. It has the value {@link Constants#mutexLocked}
   *  if locked and {@link Constants#mutexUnlocked} otherwise. This is only relevant
   *  in C ET systems. */
  public int  getInListMutex() {return inListMutex;}
  /** Get the number of events in the input list.
   *  @see EventList#events */
  public int  getInListCount() {return inListCount;}
  /** Get the number of events that were attempted to be put into the input
   *  list. This is relevant only when there is prescaling.
   * @see EventList#eventsTry */
  public long getInListTry() {return inListTry;}
  /** Get the number of events that were put into the input list.
   *  @see EventList#eventsIn */
  public long getInListIn() {return inListIn;}

  /** Get the output list mutex status. It has the value {@link Constants#mutexLocked}
   *  if locked and {@link Constants#mutexUnlocked} otherwise. This is only relevant
   *  in C ET systems. */
  public int  getOutListMutex() {return outListMutex;}
  /** Get the number of events in the output list.
   *  @see EventList#events */
  public int  getOutListCount() {return outListCount;}
  /** Get the number of events that were taken out of the output list.
   *  @see EventList#eventsOut */
  public long getOutListOut() {return outListOut;}

  // station configuration parameters ...

  /** Get the station configuration's flow mode.
   *  @see StationConfig#flowMode */
  public int getFlowMode() {return flowMode;}
  /** Get the station configuration's user mode.
   *  @see StationConfig#userMode */
  public int getUserMode() {return userMode;}
  /** Get the station configuration's restore mode.
   *  @see StationConfig#restoreMode */
  public int getRestoreMode() {return restoreMode;}
  /** Get the station configuration's blocking mode.
   *  @see StationConfig#blockMode */
  public int getBlockMode() {return blockMode;}
  /** Get the station configuration's prescale value.
   *  @see StationConfig#prescale */
  public int getPrescale() {return prescale;}
  /** Get the station configuration's input cue size.
   *  @see StationConfig#cue */
  public int getCue() {return cue;}
  /** Get the station configuration's select mode.
   *  @see StationConfig#selectMode */
  public int getSelectMode() {return selectMode;}
  /** Get the station configuration's select array.
   *  @see StationConfig#select */
  public int[] getSelect() {return (int[]) select.clone();}

  /** Get the name of the user select function in the C version ET library.
   *  @see StationConfig#selectFunction */
  public String getSelectFunction() {return selectFunction;}
  /** Get the name of the C library containing the user select function in
   *  the C version ET system.
   *  @see StationConfig#selectLibrary */
  public String getSelectLibrary() {return selectLibrary;}
  /** Get the name of the Java class containing the user select method in
   *  the Java version ET system.
   *  @see StationConfig#selectClass */
  public String getSelectClass() {return selectClass;}
  /** Get the name of the station.
   * @see Station#name
   * @see StationLocal#name */
  public String getName() {return name;}


  /**
   *  Reads the station information from a data stream which is sent out by
   *  an ET system over the network.
   *  @param dis data input stream
   *  @exception java.io.IOException
   *     if data stream read error
   */
  void read(DataInputStream dis) throws IOException {
    attachments  = dis.readInt();
    num          = dis.readInt();
    status       = dis.readInt();
    mutex        = dis.readInt();
    for (int i=0; i < attachments; i++) {
      attIds[i]  = dis.readInt();
    }

    inListMutex  = dis.readInt();
    inListCount  = dis.readInt();
    inListTry    = dis.readLong();
    inListIn     = dis.readLong();
    outListMutex = dis.readInt();
    outListCount = dis.readInt();
    outListOut   = dis.readLong();

    flowMode     = dis.readInt();
    userMode     = dis.readInt();
    restoreMode  = dis.readInt();
    blockMode    = dis.readInt();
    prescale     = dis.readInt();
    cue          = dis.readInt();
    selectMode   = dis.readInt();
//System.out.println("station read: userMode = " + userMode + ", restore = " + restoreMode +
//", block = " + blockMode + ", pre = " + prescale + ", cue = " + cue + ", selectM = " + selectMode);
    for (int i=0; i < Constants.stationSelectInts; i++) {
      select[i]  = dis.readInt();
    }

    // read strings, lengths first
    int length1 = dis.readInt();
    int length2 = dis.readInt();
    int length3 = dis.readInt();
    int length4 = dis.readInt();
//System.out.println("station read: len 1-4 = " + length1 + ", " + length2 +
//                   ", " + length3 + ", " + length4);
    int length  = length1 + length2 + length3 + length4;

    byte[] buf = new byte[length];
    dis.readFully(buf, 0, length);
    int off = 0;

    if (length1 > 0) {
      selectFunction = new String(buf, off, length1-1, "ASCII");
      off += length1;
    }
    if (length2 > 0) {
      selectLibrary = new String(buf, off, length2-1, "ASCII");
      off += length2;
    }
    if (length3 > 0) {
      selectClass = new String(buf, off, length3-1, "ASCII");
      off += length3;
    }
    if (length4 > 0) {
      name = new String(buf, off, length4-1, "ASCII");
//System.out.println("station read: station name = " + stationName);
    }

  }
}

