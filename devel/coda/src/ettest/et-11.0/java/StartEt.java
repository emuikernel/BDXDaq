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

    /** Method to print out correct program command line usage. */
    private static void usage() {
        System.out.println("\nUsage:\n" +
                "   java StartEt [-n <# of events>]\n" +
                "                [-s <size of events (bytes)>]\n" +
                "                [-p <server port>]\n" +
                "                [-debug]\n" +
                "                [-h]\n" +
                "                -f <file name>\n");
    }


    public StartEt() {
    }

    public static void main(String[] args) {
        int numEvents = 3000, size = 128, serverPort = 11111;
        boolean debug = false;
        String file=null;

        // loop over all args
        for (int i = 0; i < args.length; i++) {
            if (args[i].equalsIgnoreCase("-h")) {
                usage();
                System.exit(-1);
            }
            else if (args[i].equalsIgnoreCase("-n")) {
                numEvents = Integer.parseInt(args[i + 1]);
                i++;
            }
            else if (args[i].equalsIgnoreCase("-f")) {
                file = args[i + 1];
                i++;
            }
            else if (args[i].equalsIgnoreCase("-p")) {
                serverPort = Integer.parseInt(args[i + 1]);
                i++;
            }
            else if (args[i].equalsIgnoreCase("-s")) {
                size = Integer.parseInt(args[i + 1]);
                i++;
            }
            else if (args[i].equalsIgnoreCase("-debug")) {
                debug = true;
            }
            else {
                usage();
                System.exit(-1);
            }
        }

        if (file == null) {
            usage();
            System.exit(-1);

        }
        
        try {
            System.out.println("STARTING ET SYSTEM");
            // ET system configuration object
            SystemConfig config = new SystemConfig();

            //int[] groups = {30,30,40};
            //config.setGroups(groups);

            // listen for multicasts at this address
            config.addMulticastAddr(Constants.multicastAddr);
            // set tcp server port
            config.setServerPort(serverPort);
            // set port for listening for udp packets
            config.setUdpPort(11111);
            // set port for listening for multicast udp packets
            // (on Java this must be different than the udp port)
            config.setMulticastPort(11112);
            // set total number of events
            config.setNumEvents(numEvents);
            // set size of events in bytes
            config.setEventSize(size);
            // set debug level
            if (debug)
                config.setDebug(Constants.debugInfo);
            // create an active ET system
            SystemCreate sys = new SystemCreate("/tmp/etet", config);
        }
        catch (Exception ex) {
            System.out.println("ERROR STARTING ET SYSTEM");
            ex.printStackTrace();
        }

    }
}
