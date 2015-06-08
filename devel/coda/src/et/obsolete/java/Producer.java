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
 * This class is an example of an event producer for an ET system.
 *
 * @author Carl Timmer
 * @version 7.0
 */
public class Producer {

  public Producer() {
  }
  
  // for putting integer data into a byte array - useful when testing
  private static final void intToBytes(int intVal, byte[] bytes, int offset) {
    bytes[offset]   = (byte) ((intVal & 0xff000000) >>> 24);
    bytes[1+offset] = (byte) ((intVal & 0x00ff0000) >>> 16);
    bytes[2+offset] = (byte) ((intVal & 0x0000ff00) >>>  8);
    bytes[3+offset] = (byte)  (intVal & 0x000000ff);
  }


  private static void usage() {
    System.out.println("\nUsage: java Producer -f <et name> [-p <server port>] [-h <host>]\n\n" +
	               "       -f  ET system's name\n" +
	               "       -p  port number for a udp broadcast\n" +
                   "       -h  host the ET system resides on (defaults to anywhere)\n" +
                   "       -d  delay in millisec between getting and putting events\n" +
                   "       -g  group number of events\n" +
	               "        This consumer works by making a direct connection to the\n" +
		       "        ET system's tcp server port.\n");
  }


    public static void main(String[] args) {

        String etName = null, host = null;
        //int port = Constants.serverPort;
        int port = Constants.broadcastPort;
        int group = 0;
        int delay = 0;

        try {
            for (int i = 0; i < args.length; i++) {
                if (args[i].equalsIgnoreCase("-f")) {
                    etName = args[++i];
                }
                else if (args[i].equalsIgnoreCase("-h")) {
                    host = args[++i];
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
                else if (args[i].equalsIgnoreCase("-g")) {
                    try {
                        group = Integer.parseInt(args[++i]);
                        if ((group < 0) || (group > 10)) {
                            System.out.println("Group number must be between 0 and 10.");
                            usage();
                            return;
                        }
                    }
                    catch (NumberFormatException ex) {
                        System.out.println("Did not specify a proper group number.");
                        usage();
                        return;
                    }
                }
                else if (args[i].equalsIgnoreCase("-d")) {
                    try {
                        delay = Integer.parseInt(args[++i]);
                        if (delay < 1) {
                            System.out.println("delay must be > 0.");
                            usage();
                            return;
                        }
                    }
                    catch (NumberFormatException ex) {
                        System.out.println("Did not specify a proper delay.");
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

            // make a direct connection to ET system's tcp server
            //SystemOpenConfig config = new SystemOpenConfig(etName, host, port);

            // broadcast to ET system's tcp server
            SystemOpenConfig config = new SystemOpenConfig(etName, port, host);

            // create ET system object with verbose debugging output
            SystemUse sys = new SystemUse(config, Constants.debugInfo);

            sys.setGroup(group);

            // get GRAND_CENTRAL station object
            Station gc = sys.stationNameToObject("GRAND_CENTRAL");

            // attach to grandcentral
            Attachment att = sys.attach(gc);

            // array of events
            Event[] mevs;

            int chunk = 100, count = 0;
            long t1, t2;
            int[] con = {-1, -1, -1, -1};

            // keep track of time for event rate calculations
            t1 = System.currentTimeMillis();

            for (int i = 0; i < 10; i++) {
                while (count < 300000L) {
                    // get array of new events
                    //mevs = sys.newEvents(att, Constants.sleep, 0, chunk, 32, group);
                    mevs = sys.newEvents(att, Constants.sleep, 0, chunk, 32);

                    if (delay > 0) Thread.sleep(delay);

                    // example of how to manipulate events
                    if (false) {
                        for (int j = 0; j < mevs.length; j++) {
                            // put integer (j) into front of data buffer
                            intToBytes(j, mevs[j].getData(), 0);
                            // set data length to be 4 bytes (1 integer)
                            mevs[j].setLength(4);
                            // set every other event's priority as high
                            if (j % 2 == 0) mevs[j].setPriority(Constants.high);
                            // set event's control array
                            mevs[j].setControl(con);
                        }
                    }

                    // put events back into ET system
                    sys.putEvents(att, mevs);
                    count += mevs.length;
                }

                // calculate the event rate
                t2 = System.currentTimeMillis();
                double rate = 1000.0 * ((double) count) / ((double) (t2 - t1));
                System.out.println("rate = " + rate + " Hz");
                count = 0;
                t1 = System.currentTimeMillis();
            }
            System.out.println("End of producing events, now close");
            sys.close();
        }
        catch (Exception ex) {
            System.out.println("ERROR USING ET SYSTEM AS PRODUCER");
            ex.printStackTrace();
        }

    } // end of main method
}
