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
 * This interface defines a method to use for custom event selection in a
 * station.
 *
 * @author Carl Timmer
 */

public interface EventSelectable {

  /**
   * An event selection method must follow this form.
   * @param sys the ET system object
   * @param st the station using a user-defned selection method
   * @param ev event being evaluated for selection
   */
  public boolean select(SystemCreate sys, StationLocal st, Event ev);

}
