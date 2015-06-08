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
 * This class implements a thread which starts other threads - each of which
 * listen at a different IP address for users trying to find the ET system by
 * broadcasting, multicasting, or the direct sending of a udp packet.
 *
 * @author Carl Timmer
 */

public class SystemUdpServer extends Thread {

  /* Udp port to listen on. */
  private int port;
  /** ET system object. */
  private SystemCreate sys;
  /** ET system configuration. */
  private SystemConfig config;

  /** Createes a new SystemUdpServer object.
   *  @param _sys ET system object */
  SystemUdpServer(SystemCreate _sys) {
      sys    = _sys;
      config = sys.config;
      port   = config.serverPort;
  }

  /** Starts threads to listen for packets at a different addresses. */
  public void run() {
    if (config.debug >= Constants.debugInfo) {
      System.out.println("Running UDP Listening Threads");
    }

    // use the default port number since one wasn't specified
    if (port < 1) {
      port = Constants.serverPort;
    }

    // This thread starts one thread for each local IP address
    // (from one or more interfaces), each local subnet broadcast
    // address, and each desired multicast address. The aim is that
    // the ET system can be listening on each for a UDP packet
    // looking to find this ET system.

    // thread for each network interface IP addr - listen for broadcasts
    InetAddress[] ipAddrs;
    try {
      ipAddrs = InetAddress.getAllByName(InetAddress.getLocalHost().getHostName());
      sys.netAddresses = ipAddrs;
    }
    catch (UnknownHostException ex) {
      if (config.debug >= Constants.debugError) {
        System.out.println("cannot find local IP addresses");
        ex.printStackTrace();
      }
      return;
    }

    ListeningThread lis;

    for (int i=0; i < ipAddrs.length; i++) {
      // ignore loopback address - don't need to listen on that
      //if (ipAddrs[i].getHostAddress().equals("127.0.0.1")) {
      //  continue;
      //}
      if (config.debug >= Constants.debugInfo) {
        System.out.println("Listening on interface " +
	                   ipAddrs[i].getHostAddress() +
			   ", port " + config.serverPort);
      }
      lis = new ListeningThread(sys, ipAddrs[i], true);
      lis.start();
    }

    // thread for each multicast address
    InetAddress addr;
    for (Iterator i = config.multicastAddrs.iterator(); i.hasNext(); ) {
      addr = (InetAddress) i.next();
      if (config.debug >= Constants.debugInfo) {
        System.out.println("Listening for multicasts at " +
	                   addr.getHostAddress() +
			   ", port " + config.multicastPort);
      }
      lis = new ListeningThread(sys, addr);
      lis.start();
    }

    // thread for each broadcast address
    for (Iterator i = config.broadcastAddrs.iterator(); i.hasNext(); ) {
      addr = (InetAddress) i.next();
      if (config.debug >= Constants.debugInfo) {
        System.out.println("Listening for broadcasts at " +
	                   addr.getHostAddress() +
			   ", port " + config.udpPort);
      }
      lis = new ListeningThread(sys, addr);
      lis.start();
    }

    return;
  }
}


/**
 * This class implements a thread which listens on a particular address for a
 * udp packet. It sends back a udp packet with the tcp server port, host name,
 * and other information necessary to establish a tcp connection between the
 * tcp server thread of the ET system and the user.
 *
 * @author Carl Timmer
 * @version 6.0
 */

class ListeningThread extends Thread {

  /** Flag telling if the address is a host's address (true) or a
   *  broad/multicasting address (false). */
  private boolean      isIpAddr;
  /** Address to listen on. */
  private InetAddress  addr;
  /** ET system object. */
  private SystemCreate sys;
  /** ET system configuration object. */
  private SystemConfig config;

  /**
   *  Creates a new ListeningThread object. Assumes address is a host's address.
   *
   *  @param _sys ET system object
   *  @param _add address to listen on
   */
  ListeningThread(SystemCreate _sys, InetAddress _addr) {
    sys      = _sys;
    config   = sys.config;
    addr     = _addr;
  }

  /**
   *  Creates a new ListeningThread object.
   *
   *  @param _sys ET system object
   *  @param _add address to listen on
   *  @param _isIpAddr flag telling if the address is a host's address (true) or
   *  a broad/multicasting address (false)
   */
  ListeningThread(SystemCreate _sys, InetAddress _addr, boolean _isIpAddr) {
    sys      = _sys;
    config   = sys.config;
    addr     = _addr;
    isIpAddr = _isIpAddr;
  }

  /** Starts a single thread to listen for udp packets at a specific address
   *  and respond with ET system information. */
  public void run() {
    // Setup a socket for receiving udp packets.
    // MulticastSockets are also DatagramSockets.
    DatagramSocket   sock = null;
    MulticastSocket mSock = null;

    try {
      // if ip address, listen on one interface only
      if (isIpAddr) {
        sock = new DatagramSocket(config.udpPort, addr);
      }
      // else if broad/multicasting listen on all interfaces
      else {
        // if multicasting ...
	if (addr.isMulticastAddress()) {
          mSock = new MulticastSocket(config.multicastPort);
	  mSock.joinGroup(addr);
	  sock = (DatagramSocket) mSock;
        }
        // else if broadcasting ...
	else {
          sock = new DatagramSocket(config.udpPort, addr);
	}
      }

      sock.setReceiveBufferSize(512);
      sock.setSendBufferSize(512);
    }
    catch (SocketException ex) {
      if (config.debug >= Constants.debugError) {
        System.out.println("Port " + config.udpPort + " is taken or bad interface address");
        System.out.println("Try making the udp port different from the multicast port");
      }
      return;
    }
    catch (IOException ex) {
      if (config.debug >= Constants.debugError) {
        System.out.println("cannot listen on multicast address " + addr);
        ex.printStackTrace();
      }
      return;
    }


    // packet & buffer to receive UDP packets
    byte[] rBuffer = new byte[512]; // much larger than needed
    DatagramPacket rPacket = new DatagramPacket(rBuffer, 512);

    // Prepare output buffer we send in answer to inquiries:
    // (1) ET version #,
    // (2) port of tcp server thread (not udp port),
    // (3) length of next string,
    // (4) hostname of this interface address
    //     (may or may not be fully qualified),
    // (5) length of next string
    // (6) this interface's address in dotted-decimal form,
    // (7) length of next string,
    // (8) default hostname from getLocalHost (used as a
    //     general identifier of this host no matter which
    //     interface is used). May or may not be fully qualified.

    // Put outgoing packet into byte array
    ByteArrayOutputStream baos = null;

    try {
      String localHost = InetAddress.getLocalHost().getHostName();

      // the send buffer needs to be of byte size ...
      int bufferSize = 3*4 + addr.getHostName().length() + 1 +
                         4 + addr.getHostAddress().length() + 1 +
		         4 + localHost.length() + 1;

      baos = new ByteArrayOutputStream(bufferSize);
      DataOutputStream dos = new DataOutputStream(baos);
      
      dos.writeInt(Constants.version);
      dos.writeInt(config.serverPort);
      
      // if interface has host name associated with it, send it
      if (isIpAddr) {
        dos.writeInt(addr.getHostName().length() + 1);
	dos.write(addr.getHostName().getBytes("ASCII"));
	dos.writeByte(0);
      }
      // else send local host name
      else {
        dos.writeInt(localHost.length() + 1);
        dos.write(localHost.getBytes("ASCII"));
        dos.writeByte(0);
      }
      
      // send interface's IP address
      if (isIpAddr) {
        dos.writeInt(addr.getHostAddress().length() + 1);
	dos.write(addr.getHostAddress().getBytes("ASCII"));
	dos.writeByte(0);
      }
      // else if broad/multicasting, send local host IP address
      else {
        String localAddr = InetAddress.getLocalHost().getHostAddress();
        dos.writeInt(localAddr.length() + 1);
        dos.write(localAddr.getBytes("ASCII"));
        dos.writeByte(0);
      }
      
      // send local host name (results of uname in UNIX)
      dos.writeInt(localHost.length() + 1);
      dos.write(localHost.getBytes("ASCII"));
      dos.writeByte(0);
      dos.flush();
    }
    catch (UnsupportedEncodingException ex) {
      // this will never happen.
    }
    catch (UnknownHostException ex) {
      // local host is always known
    }
    catch (IOException ex) {
      // this will never happen since we're writing to array
    }

    // construct byte array to send over a socket
    byte[] sBuffer = baos.toByteArray();

    while (true) {
      try {
	// read incoming data without blocking forever
	while (true) {
	  try {
	    sock.receive(rPacket);
	    break;
	  }
          // socket receive timeout
	  catch (InterruptedIOException ex) {
	    // check to see if we've been commanded to die
	    if (sys.killAllThreads) {
	      return;
	    }
	  }
	}

	// decode the data:
	// (1) ET version #,
	// (2) length of string,
	// (3) ET file name

	ByteArrayInputStream bais = new ByteArrayInputStream(rPacket.getData());
	DataInputStream dis  = new DataInputStream(bais);

	int version = dis.readInt();
	int length  = dis.readInt();

	// reject incompatible ET versions
	if (version != Constants.version) {
	  continue;
	}
	// reject improper formats
	if ((length < 1) || (length > Constants.fileNameLength)) {
	  continue;
	}

	String etName = new String(rPacket.getData(), 8, length-1, "ASCII");
        
	if (config.debug >= Constants.debugInfo) {
	  System.out.println("et_listen_thread: received packet from " +
               rPacket.getAddress().getHostName() +
	       " @ " +  rPacket.getAddress().getHostAddress() +
	       " for " + etName);
	}

	// check if the ET system the client wants is ours
	if (etName.equals(sys.name)) {
	  // we're the one the client is looking for, send a reply
          DatagramPacket sPacket = new DatagramPacket(sBuffer, sBuffer.length,
	                	   rPacket.getAddress(), rPacket.getPort());
	  if (config.debug >= Constants.debugInfo) {
	    System.out.println("et_listen_thread: send return packet");
	  }
	  sock.send(sPacket);
	}
      }
      catch (IOException ex) {
        if (config.debug >= Constants.debugError) {
          System.out.println("error handling UDP packets");
	  ex.printStackTrace();
	}
      }
    }
  }


}

