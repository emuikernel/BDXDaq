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

import java.lang.*;
import org.jlab.coda.et.*;

/**
 * This class is an example of starting up an ET system.
 *
 * @author Carl Timmer
 * @version 7.0
 */
public class StartEt {

  public StartEt() {
  }

  public static void main(String[] args) {
    try {
      // ET system configuration object
      SystemConfig config = new SystemConfig();
      // listen on local subnet for broadcasts
      config.addBroadcastAddr("129.57.35.255");
      // listen for multicasts at this address
      config.addMulticastAddr(Constants.multicastAddr);
      // set tcp server port
      config.setServerPort(11111);
      // set port for listening for udp packets
      config.setUdpPort(11111);
      // set port for listening for multicast udp packets
      // (on Java this must be different than the udp port)
      config.setMulticastPort(11112);
      // set total number of events
      config.setNumEvents(3000);
      // set size of events in bytes
      config.setEventSize(32);
      // set debug level
      config.setDebug(Constants.debugInfo);
      
      // create an active ET system
      SystemCreate sys = new SystemCreate("/tmp/yourEtSystem", config);
    }
    catch (Exception ex) {
      System.out.println("ERROR STARTING ET SYSTEM");
      ex.printStackTrace();
    }

  }
}
