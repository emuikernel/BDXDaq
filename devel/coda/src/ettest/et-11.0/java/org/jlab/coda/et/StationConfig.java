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
import java.io.Serializable;

/**
 * This class specifies a configuration used to create a new station.
 *
 * @author Carl Timmer
 */

public class StationConfig implements Serializable {

  /** Maximum number of events to store in this station's input list when the
   *  station is nonblocking. When the input list has reached this limit,
   *  additional events flowing through the ET system are passed to the next
   *  station in line. */
  int      cue;
  /** A value of N means selecting 1 out of every Nth event that meets this
   *  station's selection criteria. */
  int      prescale;
  /** Determine whether the station is part of a single group of stations
   *  through which events flow in parallel or is not. A value of
   *  {@link Constants#stationParallel} means it is a parallel station,
   *  while a value of {@link Constants#stationSerial} means it is not. */
  int      flowMode;
  /** The maximum number of users permitted to attach to this station. A value
   *  of 0 means any number of users may attach. It may be set to
   *  {@link Constants#stationUserMulti} or {@link Constants#stationUserSingle}
   *  meaning unlimited users and a single user respectively. */
  int      userMode;
  /** Determine the method of dealing with events obtained by a user through an
   *  attachment, but whose process has ended before putting the events back
   *  into the system. It may have the value {@link Constants#stationRestoreIn}
   *  which places the events in the station's input list,
   *  {@link Constants#stationRestoreOut} which places them in the output list,
   *  or {@link Constants#stationRestoreGC} which places them in GRAND_CENTRAL
   *  station.*/
  int      restoreMode;
  /** Determine whether all events will pass through the station (blocking) or
   *  whether events should fill a cue with additional events bypassing the
   *  station and going to the next (nonblocking). The permitted values are
   *  {@link Constants#stationBlocking} and {@link Constants#stationNonBlocking}.
   *  */
  int      blockMode;
  /** Determine the method of filtering events for selection into the station's
   *  input list. A value of {@link Constants#stationSelectAll} applies no
   *  filtering, {@link Constants#stationSelectMatch} applies a builtin
   *  method for selection ({@link StationLocal#select}), and
   *  {@link Constants#stationSelectUser} allows the user to define a selection
   *  method. If the station is part of a single group of parallel stations, a
   *  value of {@link Constants#stationSelectRRobin} distributes events among the
   *  parallel stations using a round robin algorithm. Similarly, if the station
   *  is part of a single group of parallel stations, a value of 
   *  {@link Constants#stationSelectEqualCue} distributes events among the
   *  parallel stations using an algorithm to keep the cues equal to eachother.*/
  int      selectMode;
  /** An array of integers used in the builtin selection method and available
   *  for any tasks the user desires. Its size is set by
   *  {@link Constants#stationSelectInts}. */
  int[]    select;
  /** Name of user-defined select function in a C library. It may be null. This
   *  is only relevant to C language ET systems. */
  String   selectFunction;
  /** Name of the C library containing the user-defined select function. It may
   *  be null. This is only relevant to C language ET systems. */
  String   selectLibrary;
  /** Name of the Java class containing the user-defined select method. It may
   *  be null. This is only relevant to Java language ET systems. */
  String   selectClass;

  /** Creates a new StationConfig object with default values for everything.
   *  The default values are:
   *      cue         = {@link Constants#defaultStationCue},
   *      prescale    = {@link Constants#defaultStationPrescale},
   *      flowMode    = {@link Constants#stationSerial},
   *      userMode    = {@link Constants#stationUserMulti},
   *      restoreMode = {@link Constants#stationRestoreOut},
   *      blockMode   = {@link Constants#stationBlocking},
   *      selectMode  = {@link Constants#stationSelectAll}, and
   *      select      = filled with -1's
 */
  public StationConfig() {
    cue         = Constants.defaultStationCue;
    prescale    = Constants.defaultStationPrescale;
    flowMode    = Constants.stationSerial;
    userMode    = Constants.stationUserMulti;
    restoreMode = Constants.stationRestoreOut;
    blockMode   = Constants.stationBlocking;
    selectMode  = Constants.stationSelectAll;
    select      = new int[Constants.stationSelectInts];
    Arrays.fill(select, -1);
  }

  /** Creates a new StationConfig object from an existing one. */
  public StationConfig(StationConfig config) {
    cue            = config.cue;
    prescale       = config.prescale;
    flowMode       = config.flowMode;
    userMode       = config.userMode;
    restoreMode    = config.restoreMode;
    blockMode      = config.blockMode;
    selectMode     = config.selectMode;
    select         = (int[]) config.select.clone();
    selectFunction = config.selectFunction;
    selectLibrary  = config.selectLibrary;
    selectClass    = config.selectClass;
  }
  
  
  /** Checks to see if station configurations are compatible when adding
   *  a parallel station to an existing group of parallel stations.
   
   *  @param group  station configuration of head of existing group of parallel stations
   *  @param config configuration of station seeking to be added to the group
   */
  public static boolean compatibleParallelConfigs(StationConfig group, StationConfig config) {

    // both must be parallel stations
    if ((group.flowMode  != Constants.stationParallel) ||
        (config.flowMode != Constants.stationParallel))  {
      return false;
    }

    // if group is roundrobin or equal-cue, then config must be same
    if (((group.selectMode  == Constants.stationSelectRRobin) &&
         (config.selectMode != Constants.stationSelectRRobin)) ||
        ((group.selectMode  == Constants.stationSelectEqualCue) &&
         (config.selectMode != Constants.stationSelectEqualCue))) {
      return false;
    }

    // If group is roundrobin or equal-cue, then config's blocking & prescale must be same.
    // BlockMode is forced to be blocking and prescale is forced to be 1
    // in the method SystemUse.configCheck.
    if (((group.selectMode == Constants.stationSelectRRobin) ||
         (group.selectMode == Constants.stationSelectEqualCue)) &&
        ((group.blockMode  != config.blockMode) ||
         (group.prescale   != config.prescale))) {
      return false;
    }

    // if group is NOT roundrobin or equal-cue, then config's cannot be either
    if (((group.selectMode  != Constants.stationSelectRRobin) &&
         (group.selectMode  != Constants.stationSelectEqualCue)) &&
        ((config.selectMode == Constants.stationSelectRRobin) ||
         (config.selectMode == Constants.stationSelectEqualCue))) {
      return false;
    }

    return true;
  } 

  // public gets

 /** Gets the cue size.
   * @return cue size */
  public int     getCue()            {return cue;}
 /** Gets the prescale value.
   * @return prescale value */
  public int     getPrescale()       {return prescale;}
 /** Gets the flow mode.
   * @return flow mode */
  public int     getFlowMode()       {return flowMode;}
 /** Gets the user mode.
   * @return user mode */
  public int     getUserMode()       {return userMode;}
 /** Gets the restore mode.
   * @return restore mode */
  public int     getRestoreMode()    {return restoreMode;}
 /** Gets the block mode.
   * @return block mode */
  public int     getBlockMode()      {return blockMode;}
 /** Gets the select mode.
   * @return select mode */
  public int     getSelectMode()     {return selectMode;}
 /** Gets the select integer array.
   * @return select integer array */
  public int[]   getSelect()         {return (int[])select.clone();}
 /** Gets the user-defined select function name.
   * @return selection function name */
  public String  getSelectFunction() {return selectFunction;}
 /** Gets the name of the library containing the user-defined select function.
   * @return library name */
  public String  getSelectLibrary()  {return selectLibrary;}
 /** Gets the name of the class containing the user-defined select method.
   * @return class name */
  public String  getSelectClass()    {return selectClass;}

  // public sets

  /** Sets the station's cue size.
   *  @param q cue size
   *  @exception org.jlab.coda.et.EtException
   *     if there is a bad cue size value
   */
  public void setCue(int q) throws EtException {
    if (q < 1) {
      throw new EtException("bad cue value");
    }
    cue = q;
  }

  /** Sets the station's prescale value.
   *  @param pre prescale value
   *  @exception org.jlab.coda.et.EtException
   *     if there is a bad prescale value
   */
  public void setPrescale(int pre) throws EtException {
    if (pre < 1) {
      throw new EtException("bad prescale value");
    }
    prescale = pre;
  }

  /** Sets the station's flow mode value.
   *  @param mode flow mode
   *  @exception org.jlab.coda.et.EtException
   *     if there is a bad flow mode value
   */
  public void setFlowMode(int mode) throws EtException {
    if ((mode != Constants.stationSerial) &&
	(mode != Constants.stationParallel))    {
      throw new EtException("bad flow mode value");
    }
    flowMode = mode;
  }

  /** Sets the station's user mode value.
   *  @param mode user mode
   *  @exception org.jlab.coda.et.EtException
   *     if there is a bad user mode value
   */
  public void setUserMode(int mode) throws EtException {
    if (mode < 0) {
      throw new EtException("bad user mode value");
    }
//    if ((mode != Constants.stationUserMulti)   &&
//	(mode != Constants.stationUserSingle))    {
//      throw new EtException("bad user mode value");
//    }
    userMode = mode;
  }

  /** Sets the station's restore mode value.
   *  @param mode restore mode
   *  @exception org.jlab.coda.et.EtException
   *     if there is a bad restore mode value
   */
  public void setRestoreMode(int mode) throws EtException {
    if ((mode != Constants.stationRestoreOut) &&
        (mode != Constants.stationRestoreIn)  &&
        (mode != Constants.stationRestoreGC)  &&
        (mode != Constants.stationRestoreRedist))    {
      throw new EtException("bad restore mode value");
    }
    restoreMode = mode;
  }

  /** Sets the station's block mode value.
   *  @param mode block mode
   *  @exception org.jlab.coda.et.EtException
   *     if there is a bad block mode value
   */
  public void setBlockMode(int mode) throws EtException {
    if ((mode != Constants.stationBlocking)     &&
	(mode != Constants.stationNonBlocking))    {
      throw new EtException("bad block mode value");
    }
    blockMode = mode;
  }

  /** Sets the station's select mode value.
   *  @param mode select mode
   *  @exception org.jlab.coda.et.EtException
   *     if there is a bad select mode value
   */
  public void setSelectMode(int mode) throws EtException {
    if ((mode != Constants.stationSelectAll)    &&
	(mode != Constants.stationSelectMatch)  &&
	(mode != Constants.stationSelectUser)   &&
	(mode != Constants.stationSelectRRobin) &&
	(mode != Constants.stationSelectEqualCue))    {
      throw new EtException("bad select mode value");
    }
    selectMode = mode;
  }

  /** Sets the station's select integer array.
   *  @param sel select integer array
   *  @exception org.jlab.coda.et.EtException
   *     if there are the wrong number of elements in the array
   */
  public void setSelect(int[] sel) throws EtException {
    if (sel.length != Constants.stationSelectInts) {
      throw new EtException("wrong number of elements in select array");
    }
    select = (int[])sel.clone();
  }

  /** Sets the station's user-defined select function.
   *  @param func name of the user-defined select function
   */
  public void setSelectFunction(String func) {
    selectFunction = func;
  }

  /** Sets the library containing the user-defined select function.
   *  @param lib name of the library containg the user-defined select function
   */
  public void setSelectLibrary(String lib) {
    selectLibrary = lib;
  }

  /** Sets the class containing the user-defined select method.
   *  @param sClass name of the class containg the user-defined select method
   */
  public void setSelectClass(String sClass) {
    selectClass = sClass;
  }
}
