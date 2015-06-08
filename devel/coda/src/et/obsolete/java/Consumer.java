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
import java.net.*;
import org.jlab.coda.et.*;


/**
 * This class is an example of an event consumer for an ET system.
 *
 * @author Carl Timmer
 * @version 7.0
 */
public class Consumer {

  public Consumer() {
  }

  // for getting integer data from a byte array - useful when testing
  private static final int bytesToInt(byte[] b, int off) {
    int result = ((b[off]  &0xff) << 24) |
                 ((b[off+1]&0xff) << 16) |
                 ((b[off+2]&0xff) <<  8) |
                  (b[off+3]&0xff);
    return result;
  }
  
  
  private static void usage() {
    System.out.println("\nUsage: java Consumer -f <et name> -s <station> [-p <server port>] [-h <host>]\n\n" +
                        "       -f  ET system's name\n" +
                        "       -s  station name\n" +
                        "       -p  port number for a udp broadcast\n" +
                        "       -pos  position in station list (GC=0)\n" +
                        "       -ppos position in group of parallel staton (-1=end, -2=head)\n" +
                        "       -h  host the ET system resides on (defaults to anywhere)\n" +
                        "       -a  # of attachments\n" +
                        "       -nb make station non-blocking\n" +
                        "        This consumer works by making a direct connection to the\n" +
                        "        ET system's tcp server port.\n");
  }
  

  public static void main(String[] args) {

      int numAttachments = 0, position = 1, pposition = 0, blocking = 1;
      String etName = null, host = null, statName = null;
      //int port = Constants.serverPort;
      int port = Constants.broadcastPort;

      try {
          for (int i = 0; i < args.length; i++) {
              if (args[i].equalsIgnoreCase("-f")) {
                  etName = args[++i];
              }
              else if (args[i].equalsIgnoreCase("-h")) {
                  host = args[++i];
              }
              else if (args[i].equalsIgnoreCase("-nb")) {
                  blocking = 0;
              }
              else if (args[i].equalsIgnoreCase("-a")) {
                  try {
                      numAttachments = Integer.parseInt(args[++i]);
                  }
                  catch (NumberFormatException ex) {
                      numAttachments = 1;
                  }
              }
              else if (args[i].equalsIgnoreCase("-s")) {
                  statName = args[++i];
              }
              else if (args[i].equalsIgnoreCase("-p")) {
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
              else if (args[i].equalsIgnoreCase("-pos")) {
                  try {
                      position = Integer.parseInt(args[++i]);
                  }
                  catch (NumberFormatException ex) {
                      System.out.println("Did not specify a proper position number.");
                      usage();
                      return;
                  }
              }
              else if (args[i].equalsIgnoreCase("-ppos")) {
                  try {
                      pposition = Integer.parseInt(args[++i]);
                  }
                  catch (NumberFormatException ex) {
                      System.out.println("Did not specify a proper parallel position number.");
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
              host = Constants.hostAnywhere;
              /*
              try {
                  host = InetAddress.getLocalHost().getHostName();
              }
              catch (UnknownHostException ex) {
                  System.out.println("Host not specified and cannot find local host name.");
                  usage();
                  return;
              }
              */
          }

      if (etName == null) {
        usage();
        return;
      }
      else if (statName == null) {
        usage();
        return;
      }
      
      // make a direct connection to ET system's tcp server
      //SystemOpenConfig config = new SystemOpenConfig(etName, host, port);

      // broadcast to ET system's tcp server
      SystemOpenConfig config = new SystemOpenConfig(etName, port, host);
      
      // create ET system object with verbose debugging output
      SystemUse sys = new SystemUse(config, Constants.debugInfo);
      // configuration of a new station
      StationConfig statConfig = new StationConfig();
      //statConfig.setFlowMode(Constants.stationParallel);
      if (blocking == 0) {
        statConfig.setBlockMode(Constants.stationNonBlocking);
      }
      //statConfig.setCue(100);
      
      // create station at position 3 (2nd station past grandcentral)
System.out.println("Try to create " + statName + ", at pos = " + position + 
", ppos = " + pposition);
      Station stat = sys.createStation(statConfig, statName, position, pposition);
System.out.println("Created station " + stat.getName() + ", of id = " + stat.getId()); 
      
      // attach to new station
      Attachment att = sys.attach(stat);
System.out.println("Station attachment = " + att.getId()); 
      Attachment atts[] = null;
      if (numAttachments > 1) {
        atts = new Attachment[numAttachments-1];
         for (int i=0; i < numAttachments-1; i++) {
           atts[i] = sys.attach(stat);
         }
      }
      
      // array of events
      Event[] mevs;
      
      int chunk = 100, count = 0;
      long t1, t2;
      int num;
      
      // initialize
      t1 = System.currentTimeMillis();
      
      for(int i=0; i < 10; i++) {
        while (count < 300000L) {
          // get events from ET system
           mevs = sys.getEvents(att, Constants.sleep, 0, chunk);
           //Thread.sleep(10000);
          // start keeping track of time here since getEvents
           // may sleep for a while
          if (count == 0) t1 = System.currentTimeMillis();

           // example of reading & printing event data
           /*
           if (false) {
            for (int j=0; j < mevs.length; j++) {
              // get one integer's worth of data
               num = bytesToInt(mevs[j].getData(), 0);
              System.out.println("data = " + num);
            }
           }
           */
           // put events back into ET system
          sys.putEvents(att, mevs);
           count += mevs.length;
        }

         // calculate the event rate
        t2 = System.currentTimeMillis();
        double rate = 1000.0 * ((double)count) / ((double)(t2-t1));
        System.out.println("rate = " + rate + " Hz");
        count = 0;
      }
      sys.detach(att);
      if (numAttachments > 1) {
         for (int i=0; i < numAttachments-1; i++) {
           sys.detach(atts[i]);
         }
      }
      sys.removeStation(stat);
      sys.close();
    }
    catch (Exception ex) {
      System.out.println("ERROR USING ET SYSTEM AS CONSUMER");
      ex.printStackTrace();
    }

  }
}
