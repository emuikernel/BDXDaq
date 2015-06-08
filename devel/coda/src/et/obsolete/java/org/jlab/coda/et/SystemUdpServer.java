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
   *  @param sys ET system object */
  SystemUdpServer(SystemCreate sys) {
      this.sys = sys;
      config   = sys.config;
      port     = config.serverPort;
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

      // If we're broadcasting, then we use 1 thread with 1 socket,
      // bound to the wildcard address, to listen to broadcasts from all local
      // subnets.
      //
      // If we're multicasting and the specified multicast port is the same as the
      // broadcast port, then we use 1 thread to listen to multicasts and broadcasts
      // on one socket.
      //
      // If we're multicasting with a different port than the broadcasting/direct
      // port, then multicasting is treated separately from everything else and has
      // its own socket and thread.

      if (config.getMulticastAddrs().size() > 0) {
          try {
System.out.println("setting up for multicast on port " + config.getMulticastPort());
              MulticastSocket sock = new MulticastSocket(config.getMulticastPort());
              sock.setReceiveBufferSize(512);
              sock.setSendBufferSize(512);
              ListeningThread lis = new ListeningThread(sys, sock);
              lis.start();
          }
          catch (IOException e) {
              System.out.println("cannot listen on port " + config.getMulticastPort() + " for multicasting");
              e.printStackTrace();
          }

          if (config.getMulticastPort() == config.getUdpPort()) {
              // only need to listen on the multicast socket, so we're done
              return;
          }
      }

      try {
System.out.println("setting up for broadcast on port " + config.getUdpPort());
          DatagramSocket sock = new DatagramSocket(config.getUdpPort());
          sock.setBroadcast(true);
          sock.setReceiveBufferSize(512);
          sock.setSendBufferSize(512);
          ListeningThread lis = new ListeningThread(sys, sock);
          lis.start();
      }
      catch (SocketException e) {
          e.printStackTrace();
      }
      catch (UnknownHostException e) {
          e.printStackTrace();
      }
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

  /** ET system object. */
  private SystemCreate sys;
  /** ET system configuration object. */
  private SystemConfig config;
  /** Setup a socket for receiving udp packets. */
  private DatagramSocket sock = null;
  /** Is this thread responding to a multicast or broadcast or perhaps either. */
  private int cast;
  /** Don't know which address the broad/multicast was sent to since we're using
   * "INADDR_ANY" so just return this. */
  private String incomingAddress = "0.0.0.0";

    /**
     *  Creates a new ListeningThread object for a UDP multicasts.
     *
     *  @param sys ET system object
     *  @param mSock multicast udp socket
     */
    ListeningThread(SystemCreate sys, MulticastSocket mSock) throws IOException {
        this.sys = sys;
        config   = sys.config;
        for (InetAddress address : config.getMulticastAddrs()) {
            if (address.isMulticastAddress()) {
                mSock.joinGroup(address);
            }
        }
        sock = mSock;
        cast = Constants.broadAndMulticast;
    }

    /**
     *  Creates a new ListeningThread object for a UDP broadcasts.
     *
     *  @param sys ET system object
     *  @param sock udp socket
     */
    ListeningThread(SystemCreate sys, DatagramSocket sock) throws UnknownHostException {
        this.sys  = sys;
        config    = sys.config;
        this.sock = sock;
        cast = Constants.broadcast;
    }


  /** Starts a single thread to listen for udp packets at a specific address
   *  and respond with ET system information. */
  public void run() {

      // packet & buffer to receive UDP packets
      byte[] rBuffer = new byte[512]; // much larger than needed
      DatagramPacket rPacket = new DatagramPacket(rBuffer, 512);

      // Prepare output buffer we send in answer to inquiries:
      // (0)  ET magic numbers (3 ints)
      // (1)  ET version #
      // (2)  port of tcp server thread (not udp config->port)
      // (3)  ET_BROADCAST or ET_MULTICAST (int)
      // (4)  length of next string
      // (5)    broadcast address (dotted-dec) if broadcast received or
      //        multicast address (dotted-dec) if multicast received
      //        (see int #3)
      // (6)  length of next string
      // (7)    hostname given by "uname" (used as a general
      //        identifier of this host no matter which interface is used)
      // (8)  number of names for this IP addr starting with canonical
      // (9)    32bit, net-byte ordered IPv4 address assoc with following name
      // (10)   length of next string
      // (11)       first name = canonical
      // (12)   32bit, net-byte ordered IPv4 address assoc with following name
      // (13)   length of next string
      // (14)       first alias ...
      //
      // All aliases are sent here.
      //

      // Put outgoing packet into byte array
      ByteArrayOutputStream baos = null;

      try {
          InetAddress addr = InetAddress.getLocalHost();
          String canon = addr.getCanonicalHostName();
          String hostName = addr.getHostName();

          // the send buffer needs to be of byte size ...
          int bufferSize = 11*4 + incomingAddress.length() + hostName.length() + canon.length() + 3;
          for (InetAddress netAddress : sys.netAddresses) {
              bufferSize += 8 + netAddress.getHostName().length() + 1;
          }

          baos = new ByteArrayOutputStream(bufferSize);
          DataOutputStream dos = new DataOutputStream(baos);

          // magic #s
          dos.writeInt(Constants.magicNumbers[0]);
          dos.writeInt(Constants.magicNumbers[1]);
          dos.writeInt(Constants.magicNumbers[2]);

          dos.writeInt(Constants.version);
          dos.writeInt(config.serverPort);
          dos.writeInt(cast);

          // 0.0.0.0
          dos.writeInt(incomingAddress.length() + 1);
          dos.write(incomingAddress.getBytes("ASCII"));
          dos.writeByte(0);

          // Local host name (equivalent to uname?)
          dos.writeInt(hostName.length() + 1);
          dos.write(hostName.getBytes("ASCII"));
          dos.writeByte(0);

          // number of names/addrs to follow
          dos.writeInt(sys.netAddresses.length);

          // Send all names and 32 bit addresses associated with this host, starting w/ canonical name
          int addr32 = 0;
          for (int j = 0; j < 4; j++) {
              addr32 = addr32 << 8 | (((int) (addr.getAddress())[j]) & 0xFF);
          }
// System.out.println("sending addr32 = " + addr32 + ", canon = " + canon);
          dos.writeInt(addr32);
          dos.writeInt(canon.length() + 1);
          dos.write(canon.getBytes("ASCII"));
          dos.writeByte(0);

          for (InetAddress netAddress : sys.netAddresses) {
              // convert array of 4 bytes into 32 bit network byte-ordered address
              addr32 = 0;
              for (int j = 0; j < 4; j++) {
                  addr32 = addr32 << 8 | (((int) (netAddress.getAddress())[j]) & 0xFF);
              }
// System.out.println("sending addr32 = " + addr32 + ", name = " + netAddress.getHostName());
              dos.writeInt(addr32);
              dos.writeInt(netAddress.getHostName().length() + 1);
              dos.write(netAddress.getHostName().getBytes("ASCII"));
              dos.writeByte(0);
          }

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
//System.out.println("Waiting to receive packet, sock broadcast = " + sock.getBroadcast());
                      sock.receive(rPacket);
//System.out.println("Received packet ...");
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
              // (1) ET magic numbers (3 ints),
              // (2) ET version #,
              // (3) length of string,
              // (4) ET file name

              ByteArrayInputStream bais = new ByteArrayInputStream(rPacket.getData());
              DataInputStream dis = new DataInputStream(bais);

              int magic1 = dis.readInt();
              int magic2 = dis.readInt();
              int magic3 = dis.readInt();
              if (magic1 != Constants.magicNumbers[0] ||
                  magic2 != Constants.magicNumbers[1] ||
                  magic3 != Constants.magicNumbers[2])  {
//System.out.println("SystemUdpServer:  Magic numbers did NOT match");
                  continue;
              }

              int version = dis.readInt();
              int length = dis.readInt();
//System.out.println("et_listen_thread: received packet version =  " + version +
//                    ", length = " + length);

              // reject incompatible ET versions
              if (version != Constants.version) {
                  continue;
              }
              // reject improper formats
              if ((length < 1) || (length > Constants.fileNameLength)) {
                  continue;
              }

              String etName = new String(rPacket.getData(), 8, length - 1, "ASCII");

//System.out.println("et_listen_thread: received packet version =  " + version +
//                                  ", ET = " + etName);
              if (config.debug >= Constants.debugInfo) {
                  System.out.println("et_listen_thread: received packet from " +
                          rPacket.getAddress().getHostName() +
                          " @ " + rPacket.getAddress().getHostAddress() +
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

