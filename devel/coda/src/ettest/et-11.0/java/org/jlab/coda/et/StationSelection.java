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
 * This class contains an example of a user-defined method used to
 * select events for a station.
 *
 * @author Carl Timmer
 */

public class StationSelection implements EventSelectable {

  public StationSelection() {
  }

  public boolean select(SystemCreate sys, StationLocal st, Event ev) {
    if (ev.id%2 == 0) {
      //System.out.println("  Select Func: accept ev.id = " + ev.id);
      return true;
    }
    //System.out.println("  Select Func: reject ev.id = " + ev.id);
    return false;
  }
}
