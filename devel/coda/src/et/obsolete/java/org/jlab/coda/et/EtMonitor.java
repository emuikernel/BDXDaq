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
import java.util.Map.*;
import java.io.*;
import java.net.*;

/**
 * This class implements a monitor of an ET system. It opens the system,
 * receives all relevant data over a tcp connection, and prints it out.
 *
 * @author Carl Timmer
 */

public class EtMonitor {

  static int  period = 3; // seconds
  static long prevGcOut;

  public EtMonitor() {
  }
  
  
  private static void usage() {
    System.out.println("\nUsage: java EtMonitor -f <et name> [-p <period>] [-port <server port>] [-h <host>]\n\n" +
	               "       -f     ET system's name\n" +
	               "       -p     period in seconds between data updates\n" +
	               "       -port  port number for a direct connection\n" +
	               "       -h     host the ET system resides on (defaults to local)\n" +
	               "        This monitor works by making a direct connection to the\n" +
		       "        ET system's tcp server port.\n");
  }
  
  
  public static void main(String[] args) {
      String etName = null, host = null;
      int port = Constants.serverPort;

      try {
          for (int i = 0; i < args.length; i++) {
              if (args[i].equalsIgnoreCase("-f")) {
                  etName = args[++i];
              }
              else if (args[i].equalsIgnoreCase("-h")) {
                  host = args[++i];
              }
              else if (args[i].equalsIgnoreCase("-port")) {
                  try {
                      port = Integer.parseInt(args[++i]);
                      if ((port < 1024) || (port > 65535)) {
                          System.out.println("Port number must be between 1024 and 65535.");
                          usage();
                          return;
                      }
                  }
                  catch (NumberFormatException ex) {
                      System.out.println("Did not specify a proper port number.");
                      usage();
                      return;
                  }
              }
              else if (args[i].equalsIgnoreCase("-p")) {
                  try {
                      period = Integer.parseInt(args[++i]);
                      if (period < 1) {
                          System.out.println("Period must be at least 1 second.");
                          usage();
                          return;
                      }
                  }
                  catch (NumberFormatException ex) {
                      System.out.println("Did not specify a proper period value.");
                      usage();
                      return;
                  }
              }
              else {
                  usage();
                  return;
              }
          }

          if (host == null) {
              try {
                  host = InetAddress.getLocalHost().getHostName();
              }
              catch (UnknownHostException ex) {
                  System.out.println("Host not specified and cannot find local host name.");
                  usage();
                  return;
              }
          }

          if (etName == null) {
              usage();
              return;
          }

          // make a direct connection to ET system's tcp server
          SystemOpenConfig config = new SystemOpenConfig(etName, host, port);

          // create ET system object with debugging output
          SystemUse sys = new SystemUse(config, Constants.debugError);
          AllData etData = new AllData();

/*
      // timing
      long t1, t2;
      for (int j=0; j < 10; j++) {
        t1 = System.currentTimeMillis();
        for(int i=0; i < 2000; i++) {
          etData = sys.getData();
        }
        t2 = System.currentTimeMillis();
        double rate = 1000.0*2000.0/((double)(t2-t1));
        System.out.println("rate = " + rate + " Hz");
      }
*/

          while (true) {
              try {
                  etData = sys.getData();
                  display(sys, etData);
              }
              catch (EtException ex) {
                  System.out.print("\n*****************************************\n");
                  System.out.print("*   Error getting data from ET system   *");
                  System.out.print("\n*****************************************\n");
              }
              Thread.sleep(period * 1000);
          }


      }
      catch (IOException ex) {
          System.out.println("Communication error with ET system:");
          ex.printStackTrace();
      }
      catch (Exception ex) {
          System.out.println("ERROR:");
          ex.printStackTrace();
      }

  }


    private static void display(SystemUse sys, AllData data)
  {
    int           end = 499, lang;
    boolean       blocking;
    double	  rate = 0.0;
    StringBuffer  str = new StringBuffer(end+1);

    str.append("  ET SYSTEM - (");
    str.append(data.sysData.etName);
    str.append(") (host ");
    str.append(sys.getHost());
    str.append(") (bits ");
    if (data.sysData.bit64) {
      str.append("64)\n");
    }
    else {
      str.append("32)\n");
    }
    str.append("              (tcp port ");
    str.append(data.sysData.tcpPort);
    str.append(") (udp port ");
    str.append(data.sysData.udpPort);
    str.append(") (multicast port ");
    str.append(data.sysData.multicastPort);
    str.append(")\n              (pid ");
    str.append(data.sysData.mainPid);
    str.append(") (lang ");
    lang = sys.getLanguage();
    if (lang == Constants.langJava) {
      str.append("Java) (period ");
    }
    else if (lang == Constants.langC) {
      str.append("C) (period ");
    }
    else if (lang == Constants.langCpp) {
      str.append("C++) (period ");
    }
    else {
      str.append("unknown) (period ");
    }
    str.append(period);
    str.append(" sec)\n");
    System.out.println(str.toString());
    str.delete(0, end);

    str.append("  STATIC INFO - maximum of:\n");
    str.append("    events(");
    str.append(data.sysData.events);
    str.append("), event size(");
    str.append(data.sysData.eventSize);
    str.append("), temps(");
    str.append(data.sysData.tempsMax);
    str.append(")\n");
    str.append("    stations(");
    str.append(data.sysData.stationsMax);
    str.append("), attaches(");
    str.append(data.sysData.attachmentsMax);
    str.append("), procs(");
    str.append(data.sysData.processesMax);
    str.append(")\n");

    if (data.sysData.interfaceCount > 0) {
      str.append("    network interfaces(");
      str.append(data.sysData.interfaceCount);
      str.append(")  ");
      for (int i=0; i < data.sysData.interfaceCount; i++) {
        str.append(data.sysData.interfaceAddresses[i]);
        str.append(", ");
      }
      str.append("\n");
    }
    else {
      str.append("    network interfaces(0): none\n");
    }

    if (data.sysData.multicastCount > 0) {
      str.append("    multicast addresses(");
      str.append(data.sysData.multicastCount);
      str.append(")  ");
      for (int i=0; i < data.sysData.multicastCount; i++) {
        str.append(data.sysData.multicastAddresses[i]);
        str.append(", ");
      }
      str.append("\n");
    }

    str.append("\n  DYNAMIC INFO - currently there are:\n");
    str.append("    processes(");
    str.append(data.sysData.processes);
    str.append("), attachments(");
    str.append(data.sysData.attachments);
    str.append("), temps(");
    str.append(data.sysData.temps);
    str.append(")\n    stations(");
    str.append(data.sysData.stations);
    str.append("), hearbeat(");
    str.append(data.sysData.heartbeat);
    str.append(")\n");
    System.out.println(str.toString());
    str.delete(0, end);

    str.append("  STATIONS:\n");

    for (int i=0; i < data.statData.length; i++) {
      str.append("    \"");
      str.append(data.statData[i].name);
      str.append("\" (id = ");
      str.append(data.statData[i].num);
      str.append(")\n      static info\n");

      if (data.statData[i].status == Constants.stationIdle)
        str.append("        status(IDLE), ");
      else
        str.append("        status(ACTIVE), ");

      if (data.statData[i].flowMode == Constants.stationSerial) {
        str.append("flow(SERIAL), ");
      }
      else {
        str.append("flow(PARALLEL), ");
      }

      if (data.statData[i].blockMode == Constants.stationBlocking) {
        str.append("blocking(YES), ");
        blocking = true;
      }
      else {
        str.append("blocking(NO), ");
        blocking = false;
      }

      if (data.statData[i].userMode == Constants.stationUserMulti) {
        str.append("user(MULTI), ");
      }
      else {
        str.append("user(");
	str.append(data.statData[i].userMode);
	str.append("), ");
      }

      if (data.statData[i].selectMode == Constants.stationSelectAll)
        str.append("select(ALL)\n");
      else if (data.statData[i].selectMode == Constants.stationSelectMatch)
        str.append("select(MATCH)\n");
      else if (data.statData[i].selectMode == Constants.stationSelectUser)
        str.append("select(USER)\n");
      else if (data.statData[i].selectMode == Constants.stationSelectRRobin)
        str.append("select(RROBIN)\n");
      else
        str.append("select(EQUALCUE)\n");

      if (data.statData[i].restoreMode == Constants.stationRestoreOut)
        str.append("        restore(OUT), ");
      else if (data.statData[i].restoreMode == Constants.stationRestoreIn)
        str.append("        restore(IN), ");
      else
        str.append("        restore(GC), ");

      str.append("prescale(");
      str.append(data.statData[i].prescale);
      str.append("), cue(");
      str.append(data.statData[i].cue);
      str.append("), ");

      str.append("select words(");
      for (int j=0; j < Constants.stationSelectInts; j++) {
          str.append(data.statData[i].select[j]);
          str.append(", ");
      }
      str.append(")");

      if (data.statData[i].selectMode == Constants.stationSelectUser) {
        str.append("\n        lib = ");
        str.append(data.statData[i].selectLibrary);
        str.append(",  function = ");
        str.append(data.statData[i].selectFunction);
        str.append(",  class = ");
        str.append(data.statData[i].selectClass);
        str.append("");
      }

      System.out.println(str.toString());
      str.delete(0, end);

      // dynamic station info or info on active stations
      if (data.statData[i].status != Constants.stationActive) {
        System.out.println();
        continue;
      }

      str.append("      dynamic info\n");
      str.append("        attachments: total#(");
      str.append(data.statData[i].attachments);
      str.append("),  ids(");

      for (int j=0; j < data.statData[i].attachments; j++) {
        str.append(data.statData[i].attIds[j]);
          str.append(", ");
      }
      str.append(")\n");

      str.append("        input  list: cnt = ");
      str.append(data.statData[i].inListCount);
      str.append(", events in = ");
      str.append(data.statData[i].inListIn);

      // if blocking station and not grandcentral ...
      if (blocking && (data.statData[i].num != 0)) {
        str.append(", events try = ");
        str.append(data.statData[i].inListTry);
      }
      str.append("\n");

      str.append("        output list: cnt = ");
      str.append(data.statData[i].outListCount);
      str.append(", events out = ");
      str.append(data.statData[i].outListOut);
      str.append("\n");

      System.out.println(str.toString());
      str.delete(0, end);

      // keep track of grandcentral data rate
      if (i==0) {
        rate = (data.statData[i].outListOut - prevGcOut)/period;
        prevGcOut = data.statData[i].outListOut;
      }
    } // for (int i=0; i < data.statData.length; i++) {


    // user processes
    if (data.procData.length > 0) {
      str.append("  LOCAL USERS:\n");
      for (int i=0; i < data.procData.length; i++) {
        if (data.procData[i].attachments < 1) {
          str.append("    process id# ");
          str.append(data.procData[i].num);
          str.append(", # attachments(0), ");
        }
        else {
          str.append("    process id# ");
          str.append(data.procData[i].num);
          str.append(", # attachments(");
          str.append(data.procData[i].attachments);
          str.append("), attach ids(");
          for (int j=0; j < data.procData[i].attachments; j++) {
            str.append(data.procData[i].attIds[j]);
            str.append(", ");
          }
          str.append("), ");
        }
        str.append("pid(");
        str.append(data.procData[i].pid);
        str.append("), hbeat(");
        str.append(data.procData[i].heartbeat);
        str.append(")\n");
      }
      System.out.println(str.toString());
      str.delete(0, end);
    }


    // user attachments
    if (data.attData.length > 0) {
        str.append("  ATTACHMENTS: len = ");
        str.append(data.attData.length);
        str.append("\n");
      for (int i=0; i < data.attData.length; i++) {
        str.append("    att #");
        str.append(data.attData[i].num);
        str.append(", is at station(");
        str.append(data.attData[i].stationName);
        str.append(") on host(");
        str.append(data.attData[i].host);
        str.append(") at pid(");
        str.append(data.attData[i].pid);
        str.append(")\n");
        str.append("    proc(");
        str.append(data.attData[i].proc);
        str.append("), ");
        if (data.attData[i].blocked == 1) {
          str.append("blocked(YES)");
        }
        else {
          str.append("blocked(NO)");
        }
        if (data.attData[i].quit == 1) {
          str.append(", told to quit");
        }
        str.append("\n      events:  make(");
        str.append(data.attData[i].eventsMake);
        str.append("), get(");
        str.append(data.attData[i].eventsGet);
        str.append("), put(");
        str.append(data.attData[i].eventsPut);
        str.append("), dump(");
        str.append(data.attData[i].eventsDump);
        str.append(")");
        System.out.println(str.toString());
        str.delete(0, end);
      }
    }

    str.append("\n  EVENTS OWNED BY:\n");
    str.append("    system (");
    str.append(data.sysData.eventsOwned);
    str.append("),");
    for (int i=0; i < data.attData.length; i++) {
      str.append("  att");
      str.append(data.attData[i].num);
      str.append(" (");
      str.append(data.attData[i].eventsOwned);
      str.append("),");
      if ((i+1)%6 == 0)
        str.append("\n    ");
    }
    str.append("\n\n");

    // Event rate
    str.append("  EVENT RATE of GC = ");
    str.append(rate);
    str.append(" events/sec\n\n");

    // idle stations
    str.append("  IDLE STATIONS:      ");
    for (int i=0; i < data.statData.length; i++) {
      if (data.statData[i].status == Constants.stationIdle) {
        str.append(data.statData[i].name);
        str.append(", ");
      }
    }
    str.append("\n");

    // stations linked list
    str.append("  STATION CHAIN:      ");
    for (int i=0; i < data.statData.length; i++) {
      str.append(data.statData[i].name);
      str.append(", ");
    }
    str.append("\n");


    if (lang != Constants.langJava) {
      // mutexes
      str.append("  LOCKED MUTEXES:     ");
      if (data.sysData.mutex == Constants.mutexLocked)
        str.append("system, ");
      if (data.sysData.statMutex == Constants.mutexLocked)
        str.append("station, ");
      if (data.sysData.statAddMutex == Constants.mutexLocked)
        str.append("add_station, ");

      for (int i=0; i < data.statData.length; i++) {
        if (data.statData[i].mutex == Constants.mutexLocked) {
          str.append(data.statData[i].name);
        }
        if (data.statData[i].inListMutex == Constants.mutexLocked) {
          str.append(data.statData[i].name);
          str.append("-in, ");
        }
        if (data.statData[i].outListMutex == Constants.mutexLocked) {
          str.append(data.statData[i].name);
          str.append("-out, ");
        }
      }
      str.append("\n");
    }

    str.append("\n*****************************************\n");
    System.out.println(str.toString());
    str.delete(0, end);

  }




}
