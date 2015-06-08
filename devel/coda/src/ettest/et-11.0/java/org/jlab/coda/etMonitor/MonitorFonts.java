/*----------------------------------------------------------------------------*
 *  Copyright (c) 2002        Southeastern Universities Research Association, *
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

package org.jlab.coda.etMonitor;

import java.lang.*;
import java.awt.*;

public class MonitorFonts {
  // cannot construct object
  private MonitorFonts () {
  }

  private static final Font default30 = new Font((String)null, Font.BOLD,  30);
  private static final Font default16 = new Font((String)null, Font.BOLD,  16);
  private static final Font default14 = new Font((String)null, Font.BOLD,  14);
  private static final Font default12 = new Font((String)null, Font.PLAIN, 12);
  
  public  static final Font titleFont = default16;
  public  static final Font inputFont = default14;
  public  static final Font helpFont  = default12;
  public  static final Font treeFont  = default12;
  public  static final Font graphFont = null;
  public  static final Font buttonTabMenuFont = default14;
}
