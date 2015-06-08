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

/**
 * This class defines a station for the ET system user.
 *
 * @author Carl Timmer
 */

public class Station {

  /** Unique id number. */
  int        id;
  /** Flag telling whether this station object is usable or the station it
   *  represents has been removed. Set by the user's ET system object. */
  boolean    usable;
  /** Name of the station. */
  String     name;
  /** User's ET system object. */
  SystemUse  sys;

  // userMode = attachmentLimit; 0=multi, 1 = single, etc...

  /** Creates a station object. Done by the ET system object only.
   *  @param _name station name
   *  @param _id station id number
   *  @param _sys user's ET system object */
  Station(String _name, int _id, SystemUse _sys) {
    id   = _id;
    name = _name;
    sys  = _sys;
  }

  // public gets

  /** Gets the station name.
   * @return station name */
  public String  getName()  {return name;}
  /** Gets the station id.
   * @return station id */
  public int     getId()    {return id;}
  /** Tells if this station object is usable.
   * @return <code>true</code> if station object is usable and <code>false
   * </code> otherwise */
  public boolean usable()   {return usable;}

  /**
   * Gets the station's select array - used for filtering events.
   * @see StationConfig#select
   * @return array of select integers
   * @exception org.jlab.coda.et.EtException
   *     if the station has been removed or cannot be found
   */
  public int[] getSelectWords() throws IOException, EtException {
    if (!usable) {throw new EtException("station has been removed");}

    int err = Constants.error;
    int[] select = new int[Constants.stationSelectInts];

    synchronized(sys) {
      sys.out.writeInt(Constants.netStatGSw);
      sys.out.writeInt(id);
      sys.out.flush();

      err = sys.in.readInt();
      for (int i=0; i < select.length; i++) {select[i] = sys.in.readInt();}
    }

    if (err != Constants.ok) {
      throw new EtException("cannot find station");
    }

    return select;
  }

  /**
   * Sets the station's select array - used for filtering events.
   * @see StationConfig#select
   * @param select array of select integers
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station has been removed or cannot be found,
   *     if wrong size array, or if the station is GRAND_CENTRAL
   */
  public void setSelectWords(int[] select) throws IOException, EtException {
    if (!usable) {throw new EtException("station has been removed");}
    if (id == 0) {
      throw new EtException("cannot modify GRAND_CENTRAL station");
    }
    
    int err = Constants.error;
    
    if (select.length != Constants.stationSelectInts) {
      throw new EtException("wrong number of elements in select array");
    }
    
    synchronized(sys) {
      sys.out.writeInt(Constants.netStatSSw);
      sys.out.writeInt(id);
      for (int i=0; i < select.length; i++) {
        sys.out.writeInt(select[i]);
      }
      sys.out.flush();
      err = sys.in.readInt();
    }
    if (err != Constants.ok) {
      throw new EtException("this station has been removed from ET system");
    }

    return;
  }

  /**
   * This gets "String" station parameter data over the network.
   * @param command coded command to send to the TCP server thread.
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station cannot be found
   */
  private String getStringValue(int command) throws IOException, EtException {
    byte[] buf = null;
    String val = null;
    int err = Constants.error, length = 0;

    synchronized(sys) {
      sys.out.writeInt(command);
      sys.out.writeInt(id);
      sys.out.flush();
      err = sys.in.readInt();
      length = sys.in.readInt();

      if (err == Constants.ok) {
        buf = new byte[length];
        sys.in.readFully(buf, 0, length);
      }
    }

    if (err == Constants.ok) {
      try {val = new String(buf, 0, length-1, "ASCII");}
      catch (UnsupportedEncodingException ex) {}
    }
    else {
      if (length == 0) {return null;}
      throw new EtException("cannot find station");
    }

    return val;
  }

  /**
   * Gets the name of the library containing the station's user-defined select
   * function. This is only relevant for station's on C language ET systems.
   * @see StationConfig#selectLibrary
   * @return station's user-defined select function library
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station has been removed
   */
  public String getSelectLibrary() throws IOException, EtException {
    if (!usable) {throw new EtException("station has been removed");}
    return getStringValue(Constants.netStatLib);
  }

  /**
   * Gets the name of the station's user-defined select function.
   * This is only relevant for station's on C language ET systems.
   * @see StationConfig#selectFunction
   * @return station's user-defined select function
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station has been removed
   */
  public String getSelectFunction() throws IOException, EtException {
    if (!usable) {throw new EtException("station has been removed");}
    return getStringValue(Constants.netStatFunc);
  }

  /**
   * Gets the name of the class containing the station's user-defined select
   * method. This is only relevant for station's on Java language ET systems.
   * @see StationConfig#selectClass
   * @return station's user-defined select method class
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station has been removed
   */
  public String getSelectClass() throws IOException, EtException {
    if (!usable) {throw new EtException("station has been removed");}
    return getStringValue(Constants.netStatClass);
  }


  /**
   * This gets "integer" station parameter data over the network.
   * @param cmd coded command to send to the TCP server thread.
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station cannot be found
   */
  private int getIntValue(int cmd) throws IOException, EtException {
    int err = Constants.error;
    int val = 0;

    synchronized(sys) {
      sys.out.writeInt(cmd);
      sys.out.writeInt(id);
      sys.out.flush();
      err = sys.in.readInt();
      val = sys.in.readInt();
    }
    if (err != Constants.ok) {
      throw new EtException("this station has been removed from ET system");
    }

    return val;
  }

  /**
   * This sets "integer" station parameter data over the network.
   * @param cmd coded command to send to the TCP server thread.
   * @param val value to set.
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station cannot be found
   */
  private void setIntValue(int cmd, int val) throws IOException, EtException {
    int err = Constants.error;

    synchronized(sys) {
      sys.out.writeInt(cmd);
      sys.out.writeInt(id);
      sys.out.writeInt(val);
      sys.out.flush();
      err = sys.in.readInt();
    }
    if (err != Constants.ok) {
      throw new EtException("this station has been removed from ET system");
    }

    return;
  }

  /**
   * Gets the station's number of attachments.
   * @return station's number of attachments
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station has been removed
   */
  public int getNumAttachments() throws IOException, EtException {
    if (!usable) {throw new EtException("station has been removed");}
    return getIntValue(Constants.netStatGAtts);
  }

  /**
   * Gets the station's status. It may have the values
   * {@link Constants#stationUnused}, {@link Constants#stationCreating},
   * {@link Constants#stationIdle}, and {@link Constants#stationActive}.
   * @return station's status
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station has been removed
   */
  public int getStatus() throws IOException, EtException {
    if (!usable) {throw new EtException("station has been removed");}
    return getIntValue(Constants.netStatStatus);
  }

  /**
   * Gets the number of events in the station's input list.
   * @return number of events in the station's input list
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station has been removed
   */
  public int getInputCount() throws IOException, EtException {
    if (!usable) {throw new EtException("station has been removed");}
    return getIntValue(Constants.netStatInCnt);
  }

  /**
   * Gets the number of events in the station's output list.
   * @return number of events in the station's output list
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station has been removed
   */
  public int getOutputCount() throws IOException, EtException {
    if (!usable) {throw new EtException("station has been removed");}
    return getIntValue(Constants.netStatOutCnt);
  }

  /**
   * Gets the station configuration's block mode.
   * @see StationConfig#blockMode
   * @return station's block mode
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station has been removed
   */
  public int getBlockMode() throws IOException, EtException {
    if (!usable) {throw new EtException("station has been removed");}
    return getIntValue(Constants.netStatGBlock);
  }

  /**
   * Sets the station's block mode dynamically.
   * @see StationConfig#blockMode
   * @param mode  block mode value
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station has been removed, bad mode value, or
   *     the station is GRAND_CENTRAL
   */
  public void setBlockMode(int mode) throws IOException, EtException {
    if (!usable) {throw new EtException("station has been removed");}
    if (id == 0) {
      throw new EtException("cannot modify GRAND_CENTRAL station");
    }
    if ((mode != Constants.stationBlocking)     &&
	(mode != Constants.stationNonBlocking))    {
      throw new EtException("bad block mode value");
    }
    setIntValue(Constants.netStatSBlock, mode);
    return;
  }

  /**
   * Gets the station configuration's user mode.
   * @see StationConfig#userMode
   * @return station's user mode
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station has been removed
   */
  public int getUserMode() throws IOException, EtException {
    if (!usable) {throw new EtException("station has been removed");}
    return getIntValue(Constants.netStatGUser);
  }

  /**
   * Sets the station's user mode dynamically.
   * @see StationConfig#userMode
   * @param mode  user mode value
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station has been removed, bad mode value, or
   *     the station is GRAND_CENTRAL
   */
  public void setUserMode(int mode) throws IOException, EtException {
    if (!usable) {throw new EtException("station has been removed");}
    if (id == 0) {
      throw new EtException("cannot modify GRAND_CENTRAL station");
    }
    if (mode < 0) {
      throw new EtException("bad user mode value");
    }
    setIntValue(Constants.netStatSUser, mode);
    return;
  }

  /**
   * Gets the station configuration's restore mode.
   * @see StationConfig#restoreMode
   * @return station's restore mode
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station has been removed
   */
  public int getRestoreMode() throws IOException, EtException {
    if (!usable) {throw new EtException("station has been removed");}
    return getIntValue(Constants.netStatGRestore);
  }

  /**
   * Sets the station's restore mode dynamically.
   * @see StationConfig#restoreMode
   * @param mode  restore mode value
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station has been removed, bad mode value, or
   *     the station is GRAND_CENTRAL
   */
  public void setRestoreMode(int mode) throws IOException, EtException {
    if (!usable) {throw new EtException("station has been removed");}
    if (id == 0) {
      throw new EtException("cannot modify GRAND_CENTRAL station");
    }
    if ((mode != Constants.stationRestoreOut) &&
	(mode != Constants.stationRestoreIn)  &&
	(mode != Constants.stationRestoreGC))    {
       throw new EtException("bad restore mode value");
    }
    setIntValue(Constants.netStatSRestore, mode);
    return;
  }

  /**
   * Gets the station configuration's select mode.
   * @see StationConfig#selectMode
   * @return station's select mode
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station has been removed
   */
  public int getSelectMode() throws IOException, EtException {
    if (!usable) {throw new EtException("station has been removed");}
    return getIntValue(Constants.netStatGSelect);
  }

  /**
   * Gets the station configuration's cue.
   * @see StationConfig#cue
   * @return station's cue
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station has been removed
   */
  public int getCue() throws IOException, EtException {
    if (!usable) {throw new EtException("station has been removed");}
    return getIntValue(Constants.netStatGCue);
  }

  /**
   * Sets the station's cue size dynamically.
   * @see StationConfig#cue
   * @param cue  cue value
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station has been removed, bad cue value, or
   *     the station is GRAND_CENTRAL
   */
  public void setCue(int cue) throws IOException, EtException {
    if (!usable) {throw new EtException("station has been removed");}
    if (id == 0) {
      throw new EtException("cannot modify GRAND_CENTRAL station");
    }
    if (cue < 1) {
       throw new EtException("bad cue value");
    }
    setIntValue(Constants.netStatSCue, cue);
    return;
  }

  /**
   * Gets the station configuration's prescale.
   * @see StationConfig#prescale
   * @return station's prescale
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station has been removed
   */
  public int getPrescale() throws IOException, EtException {
    if (!usable) {throw new EtException("station has been removed");}
    return getIntValue(Constants.netStatGPre);
  }
  
  /**
   * Sets the station's prescale dynamically.
   * @see StationConfig#prescale
   * @param prescale  prescale value
   * @exception java.io.IOException
   *     if there are problems with network communication
   * @exception org.jlab.coda.et.EtException
   *     if the station has been removed, bad prescale value, or
   *     the station is GRAND_CENTRAL
   */
  public void setPrescale(int prescale) throws IOException, EtException {
    if (!usable) {throw new EtException("station has been removed");}
    if (id == 0) {
      throw new EtException("cannot modify GRAND_CENTRAL station");
    }
    if (prescale < 1) {
       throw new EtException("bad prescale value");
    }
    setIntValue(Constants.netStatSPre, prescale);
    return;
  }


}
