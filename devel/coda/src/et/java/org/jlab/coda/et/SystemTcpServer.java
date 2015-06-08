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
 * This class implements a thread which listens for users trying to connect to
 * the ET system. It starts another thread for each tcp socket connection
 * established to a user of the system.
 *
 * @author Carl Timmer
 */

public class SystemTcpServer extends Thread {

  /** Port number to listen on. */
  private int port;
  /** Et system object. */
  private SystemCreate sys;

  /** Createes a new SystemTcpServer object.
   *  @param _sys ET system object */
  SystemTcpServer(SystemCreate _sys) {
      sys  = _sys;
      port = sys.config.serverPort;
  }

  /** Start thread to listen for connections and spawn off communication
   *  handling threads. */
  public void run() {
    if (sys.config.debug >= Constants.debugInfo) {
      System.out.println("Running TCP Server Thread");
    }

    // use the default port number since one wasn't specified
    if (port < 1) {
      port = Constants.serverPort;
    }

    // let exceptions propagate up a level

    // open a listening socket
    try {
      ServerSocket listeningSocket = new ServerSocket(port);  // IOEx
      // 2 second accept timeout
      listeningSocket.setSoTimeout(2000);

      while (true) {
	// socket to client created
	Socket sock;
	while (true) {
	  try {
	    sock = listeningSocket.accept();
	    break;
	  }
          // server socket accept timeout
	  catch (InterruptedIOException ex) {
	    // check to see if we've been commanded to die
	    if (sys.killAllThreads) {
	      return;
	    }
	  }
	}
	// Set reading timeout to 1/2 second so dead clients
	// can be found by reading on a socket.
	sock.setSoTimeout(500);
        // Set tcpNoDelay so no packets are delayed
        sock.setTcpNoDelay(true);
        // set buffer size
        sock.setReceiveBufferSize(65535);
        sock.setSendBufferSize(65535);
	// create thread to deal with client
	ClientThread connection = new ClientThread(sys, sock);
	connection.start();
      }
    }
    catch (SocketException ex) {
    }
    catch (IOException ex) {
    }
    return;
  }

}


/**
 * This class handles all communication between an ET system and a user who has
 * opened that ET system.
 *
 * @author Carl Timmer
 * @version 6.0
 */

class ClientThread extends Thread {

  /** Tcp socket. */
  private Socket sock;
  /** ET system object. */
  private SystemCreate sys;
  /** ET system configuration object. */
  private SystemConfig config;
  /** Input stream from the socket. */
  private InputStream sockIn;
  /** Output stream from the socket. */
  private OutputStream sockOut;
  /** Data input stream built on top of the socket's input stream (with an
   *  intervening buffered input stream). */
  private DataInputStream  in;
  /** Data output stream built on top of the socket's output stream (with an
   *  intervening buffered output stream). */
  private DataOutputStream out;

  /**
   *  Create a new ClientThread object.
   *  @param _sys ET system object.
   *  @param _sock Tcp socket.
   */
  ClientThread(SystemCreate _sys, Socket _sock) {
    sys    = _sys;
    config = sys.config;
    sock   = _sock;
  }

  /** Start thread to handle communications with user. */
  public void run() {

    try {
      // buffered communication streams for efficiency
      sockIn  = sock.getInputStream();
      sockOut = sock.getOutputStream();
      in  = new DataInputStream(new BufferedInputStream(sock.getInputStream(), 65535));
      out = new DataOutputStream(new BufferedOutputStream(sock.getOutputStream(), 65535));

      int endian = in.readInt();
      int length = in.readInt();
      in.readLong();

      byte[] buf = new byte[length];
      in.readFully(buf, 0, length);
      String etName = new String(buf, 0, length-1, "ASCII");

      // see if the ET system that the client is
      // trying to connect to is this one.
      if (!etName.equals(sys.name)) {
	if (config.debug >= Constants.debugError) {
	  System.out.println("Tcp Server: client trying to connect to " + etName);
	}
	// send error to client
        out.writeInt(Constants.error);
        out.flush();
	return;
      }

      // send ET system info back to client
      out.writeInt(Constants.ok);
      out.writeInt(Constants.endianBig);
      out.writeInt(config.numEvents);
      out.writeInt(config.eventSize);
      out.writeInt(Constants.version);
      out.writeInt(Constants.stationSelectInts);
      out.writeInt(Constants.langJava);
      out.writeLong(0L);
      out.flush();

      /* wait for and process client requests */
      commandLoop();

      return;
    }
    catch (IOException ex) {
      if (config.debug >= Constants.debugError) {
	System.out.println("Tcp Server: IO error in client etOpen");
      }
    }
    finally {
      // we are done with the socket
      try {sock.close();}
      catch (IOException ex) {}
    }
  }

  /**
   * Converts 4 bytes of a byte array into an integer.
   * @param b byte array
   * @param off offset into the byte array (0 = start at first element)
   * @return integer value
   */
  private static final int bytesToInt(byte[] b, int off) {
    int result = ((b[off]  &0xff) << 24) |
                 ((b[off+1]&0xff) << 16) |
                 ((b[off+2]&0xff) <<  8) |
                  (b[off+3]&0xff);
    return result;
  }

  /**
   * Copies an integer value into 4 bytes of a byte array.
   * @param intVal integer value
   * @param b byte array
   * @param off offset into the byte array
   */
  private static final void intToBytes(int intVal, byte[] b, int off) {
    b[off]   = (byte) ((intVal & 0xff000000) >>> 24);
    b[off+1] = (byte) ((intVal & 0x00ff0000) >>> 16);
    b[off+2] = (byte) ((intVal & 0x0000ff00) >>>  8);
    b[off+3] = (byte)  (intVal & 0x000000ff);
  }


  /**  Wait for and implement commands from the user. */
  private void commandLoop() {

    // Keep track of all the attachments this client makes
    // as they may need to be detached if the client dies
    // without cleanly disconnecting itself. Detaching
    // takes care of all events that were sent to clients
    // as events to be modified, but were never put back.

    // for efficiency, keep local copy of constants
    final int selectInts   = Constants.stationSelectInts;
    final int dataShift    = Constants.dataShift;
    final int priorityMask = Constants.priorityMask;
    final int dataMask     = Constants.dataMask;
    final int modify       = Constants.modify;
    final int ok           = Constants.ok;

    int command;
    ArrayList eventList = null;
    Event[] evs = null;
    HashMap attachments = new HashMap(sys.config.attachmentsMax+1, 1);
    // buffer for sending events to users
    byte[] buffer = new byte[65535];
    // buffer for reading command parameters (6 ints worth)
    byte[] params = new byte[24+4*selectInts];

    // The Command Loop ...
    try {
      while (true) {
	// First, read the remote command. Remember, the
	// socket has a read timeout of 1/2 second.
	while (true) {
	  try {
	    command = in.readInt();
	    break;
	  }
          // socket read timeout
	  catch (InterruptedIOException ex) {
	    // check to see if we've been commanded to die
	    if (sys.killAllThreads) {
	      return;
	    }
	  }
	}

	// Since there are so many commands, break up things up a bit,
	// start off with commands for local clients for use in Linux
	// or other non-mutex sharing operating systems.

	if (command < Constants.netEvGet) {
	  // No local Linux stuff in Java implementation
          if (config.debug >= Constants.debugError) {
            System.out.println("No Java support for local Linux");
	  }
	  throw new EtReadException();
	}

	else if (command < Constants.netAlive) {

	  switch (command) {

            case  Constants.netEvGet:
            {
	      in.readFully(params, 0, 20);
	      int err   = ok;
	      int attId = bytesToInt(params, 0);
	      int mode  = bytesToInt(params, 4);
	      int mod   = bytesToInt(params, 8);
	      int sec   = bytesToInt(params, 12);
	      int nsec  = bytesToInt(params, 16);
              AttachmentLocal att = (AttachmentLocal) attachments.get(new Integer(attId));

              try {
		if (mode == Constants.timed) {
	          int uSec = sec*1000000 + nsec/1000;
        	  evs = sys.getEvents(att, mode, uSec, 1);
		}
		else if (mode == Constants.sleep) {
		  // There's a problem if we have a remote client that is waiting
		  // for another event by sleeping and the events stop flowing. In
		  // that case, the client can be killed and the ET system does NOT
		  // know about it. Since this thread will be stuck in "getEvents",
		  // it will not immediately detect the break in the socket - at least
		  // not until events start flowing again. To circumvent this, implement
		  // "sleep" by repeats of "timed" every few seconds to allow
		  // detection of broken socket between calls to "getEvents".

        	  // Store the fact we're trying to sleep - necessary when
		  // told to wake up.
		  att.sleepMode = true;
		  
		  tryToGetEvents: while (true) {
		    // try a 4 second wait for an event
		    try {
                      if (att.wakeUp == true) {
		        att.wakeUp = false;
			throw new EtWakeUpException("attachment " + att.id + " woken up");
		      }
        	      evs = sys.getEvents(att, Constants.timed, 4000000, 1);
	              // no longer in sleep mode
	              att.sleepMode = false;
	              // may have been told to wake up between last 2 statements.
	              att.wakeUp = false;
		      break;
		    }
		    // if timeout, check socket to see if still open
		    catch (EtTimeoutException tx) {
		      while (true) {
			try {
			  // 1/2 second max delay on read
			  in.readInt();
			  // should never be able to get here
			  att.sleepMode = false;
			  throw new EtException("communication protocol error");
			}
			// if there's an interrupted ex, socket is OK
			catch (InterruptedIOException ex) {
			  continue tryToGetEvents;
			}
			// if there's an io ex, socket is closed
			catch (IOException ex) {
			  throw ex;
			}
        	      }
		    }
		  }

		}
		else {
        	  evs = sys.getEvents(att, mode, 0, 1);
		}

	      }
              catch (EtException ex)    {err = Constants.error;}
	      catch (EtBusyException ex) {err = Constants.errorBusy;}
	      catch (EtEmptyException ex) {err = Constants.errorEmpty;}
	      catch (EtWakeUpException ex) {err = Constants.errorWakeUp;
	                                    att.sleepMode = false;}
	      catch (EtTimeoutException ex) {err = Constants.errorTimeout;}

	      if (err != ok) {
		out.writeInt(err);
		out.flush();
		break;
	      }

	      Event ev = evs[0];

              // handle buffering by hand
              byte[] buf = new byte[4*(7+selectInts) + ev.length];

              // first send error
              intToBytes(err, buf, 0);
              intToBytes(ev.length, buf, 4);
              intToBytes(ev.memSize, buf, 8);
              intToBytes(ev.priority | ev.dataStatus << dataShift, buf, 12);
              intToBytes(ev.id, buf, 16);
              intToBytes(ev.byteOrder, buf, 20);
              // arrays are initialized to zero so skip 0 values elements
              int index = 24;
              for (int i=0; i < selectInts; i++) {
                intToBytes(ev.control[i], buf, index+=4);
              }
              System.arraycopy(ev.data, 0, buf, index+=4, ev.length);

              sockOut.write(buf);
              sockOut.flush();

              ev.modify = mod;
	      if (mod == 0) {
                sys.putEvents(att, evs);
	      }
              evs = null;
	    }
	    break;


            case  Constants.netEvsGet:
            {
	      in.readFully(params, 0, 24);
	      int err   = ok;
	      int attId = bytesToInt(params, 0);
	      int mode  = bytesToInt(params, 4);
	      int mod   = bytesToInt(params, 8);
	      int count = bytesToInt(params, 12);
	      int sec   = bytesToInt(params, 16);
	      int nsec  = bytesToInt(params, 20);
              AttachmentLocal att = (AttachmentLocal) attachments.get(new Integer(attId));

              try {
		if (mode == Constants.timed) {
	          int uSec = sec*1000000 + nsec/1000;
        	  evs = sys.getEvents(att, mode, uSec, count);
		}
		else if (mode == Constants.sleep) {
		  // There's a problem if we have a remote client that is waiting
		  // for another event by sleeping and the events stop flowing. In
		  // that case, the client can be killed and the ET system does NOT
		  // know about it. Since this thread will be stuck in "getEvents",
		  // it will not immediately detect the break in the socket - at least
		  // not until events start flowing again. To circumvent this, implement
		  // "sleep" by repeats of "timed" every few seconds to allow
		  // detection of broken socket between calls to "getEvents".

        	  // Store the fact we're trying to sleep - necessary when
		  // told to wake up.
		  att.sleepMode = true;
		  
		  tryToGetEvents: while (true) {
		    // try a 4 second wait for events
		    try {
                      if (att.wakeUp == true) {
		        att.wakeUp = false;
			throw new EtWakeUpException("attachment " + att.id + " woken up");
		      }
		      evs = sys.getEvents(att, Constants.timed, 4000000, count);
	              // no longer in sleep mode
	              att.sleepMode = false;
	              // may have been told to wake up between last 2 statements.
	              att.wakeUp = false;
		      break;
		    }
		    // if timeout, check socket to see if still open
		    catch (EtTimeoutException tx) {
		      while (true) {
			try {
			  // 1/2 second max delay on read
			  in.readInt();
			  // should never be able to get here
			  att.sleepMode = false;
			  throw new EtException("communication protocol error");
			}
			// if there's an interrupted ex, socket is OK
			catch (InterruptedIOException ex) {
			  continue tryToGetEvents;
			}
			// if there's an io ex, socket is closed
			catch (IOException ex) {
			  throw ex;
			}
        	      }
		    }
		  }

		}
		else {
        	  evs = sys.getEvents(att, mode, 0, count);
		}

	      }
              catch (EtException ex)    {err = Constants.error;}
	      catch (EtBusyException ex) {err = Constants.errorBusy;}
	      catch (EtEmptyException ex) {err = Constants.errorEmpty;}
	      catch (EtWakeUpException ex) {err = Constants.errorWakeUp;
	                                    att.sleepMode = false;}
	      catch (EtTimeoutException ex) {err = Constants.errorTimeout;}

	      if (err != ok) {
		out.writeInt(err);
		out.flush();
		break;
	      }
/*
	      // use buffered output
              // first send number of events
	      out.writeInt(evs.length);
              int size = evs.length*4*(6+selectInts);
              for (int j=0; j < evs.length; j++) {
                size += evs[j].length;
              }
              out.writeInt(size);
	      for (int j=0; j < evs.length; j++) {
        	evs[j].modify = mod;
        	out.writeInt(evs[j].length);
        	out.writeInt(evs[j].memSize);
        	out.writeInt(evs[j].priority | evs[j].dataStatus << dataShift);
        	out.writeInt(evs[j].id);
        	out.writeInt(evs[j].byteOrder);
        	out.writeInt(0);
		for (int i=0; i < selectInts; i++) {
		  out.writeInt(evs[j].control[i]);
		}
		out.write(evs[j].data, 0, evs[j].length);
	      }
              out.flush();
*/

              // handle buffering by hand
              int length, index=8;
              int headerSize = 4*(6+selectInts);
              int size = evs.length*headerSize;
              for (int j=0; j < evs.length; j++) {
                size += evs[j].length;
              }

              intToBytes(evs.length, buffer, 0);
              intToBytes(size, buffer, 4);
	      for (int j=0; j < evs.length; j++) {
        	evs[j].modify = mod;
                length = evs[j].length;
        	intToBytes(length, buffer, index);
        	intToBytes(evs[j].memSize, buffer, index+=4);
        	intToBytes(evs[j].priority | evs[j].dataStatus << dataShift, buffer, index+=4);
        	intToBytes(evs[j].id, buffer, index+=4);
        	intToBytes(evs[j].byteOrder, buffer, index+=4);
        	intToBytes(0, buffer, index+=4);
		for (int i=0; i < selectInts; i++) {
		  intToBytes(evs[j].control[i], buffer, index+=4);
		}
                index+=4;
                if (index + headerSize + length > buffer.length) {
                  sockOut.write(buffer, 0, index);
                  index = 0;
                  if (headerSize + length > buffer.length/2) {
                    sockOut.write(evs[j].data, 0, length);
                    sockOut.flush();
                    continue;
                  }
                  sockOut.flush();
                }
                System.arraycopy(evs[j].data, 0, buffer, index, length);
                index += length;
	      }
              if (index > 0) {
                sockOut.write(buffer, 0, index);
                sockOut.flush();
              }

	      if (mod == 0) {
                sys.putEvents(att, evs);
	      }
              evs = null;
	    }
	    break;


            case  Constants.netEvPut:
            {
	      in.readFully(params, 0, 24+4*selectInts);
	      int attId = bytesToInt(params, 0);
              AttachmentLocal att = (AttachmentLocal) attachments.get(new Integer(attId));
	      int id    = bytesToInt(params, 4);
	      Event ev  = (Event) sys.events.get(new Integer(id));
	      ev.length      = bytesToInt(params, 8);
	      int priAndStat = bytesToInt(params, 12);
	      ev.priority    =  priAndStat & priorityMask;
	      ev.dataStatus  = (priAndStat & dataMask) >> dataShift;
	      ev.byteOrder   = bytesToInt(params, 16);
              // last parameter is ignored

	      int index = 16;
	      for (int i=0; i < selectInts; i++) {
		ev.control[i] = bytesToInt(params, index+=4);
              }
	      // only read data if modifying everything
	      if (ev.modify == modify) {
		in.readFully(ev.data, 0, ev.length);
              }

              Event[] evArray = new Event[1];
              evArray[0] = ev;

              sys.putEvents(att, evArray);

	      out.writeInt(ok);
	      out.flush();
	    }
	    break;


            case  Constants.netEvsPut:
            {
	      in.readFully(params, 0, 12);
	      int attId = bytesToInt(params, 0);
              AttachmentLocal att = (AttachmentLocal) attachments.get(new Integer(attId));
	      int numEvents = bytesToInt(params, 4);
	      int size = bytesToInt(params, 8);

	      int id, priAndStat, length, index;
	      int byteChunk = 20+4*selectInts;
	      evs           = new Event[numEvents];

	      for (int j=0; j < numEvents; j++) {
	        in.readFully(params, 0, byteChunk);

		id     = bytesToInt(params, 0);
		evs[j] = (Event) sys.events.get(new Integer(id));
		evs[j].length     = bytesToInt(params, 4);
                priAndStat        = bytesToInt(params, 8);
		evs[j].priority   = priAndStat & priorityMask;
		evs[j].dataStatus = (priAndStat & dataMask) >> dataShift;
		evs[j].byteOrder  = bytesToInt(params, 12);
		index = 16;
		for (int i=0; i < selectInts; i++) {
		  evs[j].control[i] = bytesToInt(params, index+=4);
        	}
		if (evs[j].modify == modify) {
		  // If user increased data length beyond memSize,
		  // use more memory.
		  if (evs[j].length > evs[j].memSize) {
		    evs[j].data = new byte[evs[j].length];
		    evs[j].memSize = evs[j].length;
		  }
		  in.readFully(evs[j].data, 0, evs[j].length);
        	}
              }

              sys.putEvents(att, evs);
	      out.writeInt(ok);
	      out.flush();
	    }
	    break;


            case  Constants.netEvNew:
            {
	      in.readFully(params, 0, 20);
	      int err   = ok;
	      int attId = bytesToInt(params, 0);
	      int mode  = bytesToInt(params, 4);
	      int size  = bytesToInt(params, 8);
	      int sec   = bytesToInt(params, 12);
	      int nsec  = bytesToInt(params, 16);
              AttachmentLocal att = (AttachmentLocal) attachments.get(new Integer(attId));

              try {

		if (mode == Constants.timed) {
	          int uSec = sec*1000000 + nsec/1000;
        	  evs = sys.newEvents(att, mode, uSec, 1, size);
		}
		else if (mode == Constants.sleep) {
		  // There's a problem if we have a remote client that is waiting
		  // for another event by sleeping and the events stop flowing. In
		  // that case, the client can be killed and the ET system does NOT
		  // know about it. Since this thread will be stuck in "getEvents",
		  // it will not immediately detect the break in the socket - at least
		  // not until events start flowing again. To circumvent this, implement
		  // "sleep" by repeats of "timed" every few seconds to allow
		  // detection of broken socket between calls to "getEvents".

        	  // Store the fact we're trying to sleep - necessary when
		  // told to wake up.
		  att.sleepMode = true;
		  
		  tryToGetEvents: while (true) {
		    // try a 4 second wait for an event
		    try {
                      if (att.wakeUp == true) {
		        att.wakeUp = false;
			throw new EtWakeUpException("attachment " + att.id + " woken up");
		      }
		      evs = sys.newEvents(att, Constants.timed, 4000000, 1, size);
	              // no longer in sleep mode
	              att.sleepMode = false;
	              // may have been told to wake up between last 2 statements.
	              att.wakeUp = false;
		      break;
		    }
		    // if timeout, check socket to see if still open
		    catch (EtTimeoutException tx) {
		      while (true) {
			try {
			  // 1/2 second max delay on read
			  in.readInt();
			  // should never be able to get here
			  att.sleepMode = false;
			  throw new EtException("communication protocol error");
			}
			// if there's an interrupted ex, socket is OK
			catch (InterruptedIOException ex) {
			  continue tryToGetEvents;
			}
			// if there's an io ex, socket is closed
			catch (IOException ex) {
			  throw ex;
			}
        	      }
		    }
		  }

		}
		else {
        	  evs = sys.newEvents(att, mode, 0, 1, size);
		}
	      }
              catch (EtException ex) {err = Constants.error;}
	      catch (EtBusyException ex) {err = Constants.errorBusy;}
	      catch (EtEmptyException ex) {err = Constants.errorEmpty;}
	      catch (EtWakeUpException ex) {err = Constants.errorWakeUp;
	                                    att.sleepMode = false;}
	      catch (EtTimeoutException ex) {err = Constants.errorTimeout;}
	      
	      if (err != ok) {
		out.writeInt(err);
		out.writeInt(0);
		out.flush();
		break;
	      }

	      Event ev = evs[0];
              ev.modify = modify;

              out.writeInt(err);
              out.writeInt(ev.id);
 	      out.flush();
              evs = null;
	    }
	    break;


            case  Constants.netEvsNew:
            {
	      in.readFully(params, 0, 24);
	      int err   = ok;
	      int attId = bytesToInt(params, 0);
	      int mode  = bytesToInt(params, 4);
	      int size  = bytesToInt(params, 8);
	      int count = bytesToInt(params, 12);
	      int sec   = bytesToInt(params, 16);
	      int nsec  = bytesToInt(params, 20);
              AttachmentLocal att = (AttachmentLocal) attachments.get(new Integer(attId));

              try {
		if (mode == Constants.timed) {
	          int uSec = sec*1000000 + nsec/1000;
        	  evs = sys.newEvents(att, mode, uSec, count, size);
		}
		else if (mode == Constants.sleep) {
		  // There's a problem if we have a remote client that is waiting
		  // for another event by sleeping and the events stop flowing. In
		  // that case, the client can be killed and the ET system does NOT
		  // know about it. Since this thread will be stuck in "getEvents",
		  // it will not immediately detect the break in the socket - at least
		  // not until events start flowing again. To circumvent this, implement
		  // "sleep" by repeats of "timed" every few seconds to allow
		  // detection of broken socket between calls to "getEvents".

        	  // Store the fact we're trying to sleep - necessary when
		  // told to wake up.
		  att.sleepMode = true;
		  
		  tryToGetEvents: while (true) {
		    // try a 4 second wait for events
		    try {
                      if (att.wakeUp == true) {
		        att.wakeUp = false;
			throw new EtWakeUpException("attachment " + att.id + " woken up");
		      }
        	      evs = sys.newEvents(att, Constants.timed, 4000000, count, size);
	              // no longer in sleep mode
	              att.sleepMode = false;
	              // may have been told to wake up between last 2 statements.
	              att.wakeUp = false;
		      break;
		    }
		    // if timeout, check socket to see if still open
		    catch (EtTimeoutException tx) {
		      while (true) {
			try {
			  // 1/2 second max delay on read
			  in.readInt();
			  // should never be able to get here
			  att.sleepMode = false;
			  throw new EtException("communication protocol error");
			}
			// if there's an interrupted ex, socket is OK
			catch (InterruptedIOException ex) {
			  continue tryToGetEvents;
			}
			// if there's an io ex, socket is closed
			catch (IOException ex) {
			  throw ex;
			}
        	      }
		    }
		  }

		}
		else {
        	  evs = sys.newEvents(att, mode, 0, count, size);
		}

	      }
              catch (EtException ex) {err = Constants.error;}
	      catch (EtBusyException ex) {err = Constants.errorBusy;}
	      catch (EtEmptyException ex) {err = Constants.errorEmpty;}
	      catch (EtWakeUpException ex) {err = Constants.errorWakeUp;
	                                    att.sleepMode = false;}
	      catch (EtTimeoutException ex) {err = Constants.errorTimeout;}

	      if (err != ok) {
		out.writeInt(err);
		out.flush();
		break;
	      }

              // handle buffering by hand
              int index=0;
              byte[] buf = new byte[4 + 4*evs.length];

              // first send number of events
              intToBytes(evs.length, buf, 0);
	      for (int j=0; j < evs.length; j++) {
        	evs[j].modify = modify;
              	intToBytes(evs[j].id, buf, index+=4);
	      }
              sockOut.write(buf);
              sockOut.flush();

              evs = null;
	    }
	    break;


            case  Constants.netEvDump:
            {
	      int attId = in.readInt();
	      int id    = in.readInt();

              AttachmentLocal att = (AttachmentLocal) attachments.get(new Integer(attId));
	      Event ev = (Event) sys.events.get(new Integer(id));
              Event[] evArray = new Event[1];
              evArray[0] = ev;
              sys.dumpEvents(att, evArray);

	      out.writeInt(ok);
	      out.flush();
	    }
	    break;


            case  Constants.netEvsDump:
            {
	      int attId     = in.readInt();
	      int numEvents = in.readInt();
	      evs           = new Event[numEvents];
              AttachmentLocal att = (AttachmentLocal) attachments.get(new Integer(attId));

	      int id;
	      byte[] buf = new byte[4*numEvents];
	      in.readFully(buf, 0, 4*numEvents);
	      int index = -4;

	      for (int j=0; j < numEvents; j++) {
		id = bytesToInt(buf, index+=4);
		evs[j] = (Event) sys.events.get(new Integer(id));
              }

              sys.dumpEvents(att, evs);

	      out.writeInt(ok);
	      out.flush();
	    }
	    break;


            default:
	      break;
	  } // switch(command)
	}   // if (command < Constants.netAlive)


	else if (command < Constants.netStatGAtts) {
	  switch (command) {
            case  Constants.netAlive:
            {
	      // we must be alive by definition as this is in the ET process
	      out.writeInt(1);
	      out.flush();
            }
            break;


            case  Constants.netWait:
            {
              // We are alive by definition and in Java there is no
	      // routine comparable to et_wait_for_alive(). This is
	      // to talk to "C" ET systems.
              out.writeInt(ok);
	      out.flush();
 	    }
            break;


            case  Constants.netClose:
            case  Constants.netFClose:
            {
              out.writeInt(ok);

	      // detach all attachments
	      Entry ent;
	      for (Iterator i = attachments.entrySet().iterator(); i.hasNext(); ) {
		ent = (Entry) i.next();
		sys.detach((AttachmentLocal) ent.getValue());
	      }
              if (config.debug >= Constants.debugInfo) {
        	  java.lang.System.out.println("commandLoop: remote client closing");
              }
	      return;
 	    }
            // break;


            case  Constants.netWakeAtt:
            {
              int attId = in.readInt();
	      // look locally for attachments
              AttachmentLocal att = (AttachmentLocal) attachments.get(new Integer(attId));
              if (att != null) {
		att.station.inputList.wakeUp(att);
		if (att.sleepMode == true) {att.wakeUp = true;}
	      }
	    }
            break;


            case Constants.netWakeAll:
            {
              int statId = in.readInt();
	      AttachmentLocal att;
              StationLocal stat;
	      // Stations are stored in a linked list. Find one w/ this id.
              synchronized (sys.stationLock) {
	        for (ListIterator i = sys.stations.listIterator(); i.hasNext(); ) {
		  stat = (StationLocal) i.next();
		  if (stat.id == statId) {
                    // Since attachments which sleep when getting events don't
		    // really sleep but do a timed wait, they occasionally are
		    // not in a get method but are checking the status of the
		    // tcp connection. This means they don't know to wake up.
		    // Solve this problem by setting all the station's
		    // attachment's wake up flags, so that the next call to
		    // getEvents will make them all wake up.
		    
		    for (Iterator j = stat.attachments.iterator(); j.hasNext(); ) {
		      att = (AttachmentLocal) j.next();
		      if (att.sleepMode == true) {att.wakeUp = true;}
		    }
		    
		    stat.inputList.wakeUpAll();
		    break;
                  }
                }
              }
	    }
            break;


            case  Constants.netStatAtt:
            {
	      int err    = ok;
	      int statId = in.readInt();
	      int pid    = in.readInt();
	      int length = in.readInt();
	      String host = null;
              AttachmentLocal att = null;

              if (length > 0) {
		byte buf[] = new byte[length];
		in.readFully(buf, 0, length);
		host = new String(buf, 0, length-1, "ASCII");
	      }

	      try {
		att = sys.attach(statId);
		att.pid = pid;
		if (length > 0) {
		  att.host = host;
		}
		// keep track of all attachments locally
		attachments.put(new Integer(att.id), att);
	      }
	      catch (EtException ex) {err = Constants.error;}
	      catch (EtTooManyException ex) {err = Constants.errorTooMany;}

	      out.writeInt(err);
	      if (err == ok) {
		out.writeInt(att.id);
	      }
	      else {
		out.writeInt(0);
	      }
	      out.flush();
            }
            break;


            case  Constants.netStatDet:
            {
	      int attId = in.readInt();
              AttachmentLocal att = (AttachmentLocal) attachments.get(new Integer(attId));

 	      sys.detach(att);

	      // keep track of all detachments locally
              attachments.remove(new Integer(att.id));
              out.writeInt(ok);
	      out.flush();
	    }
	    break;


            case  Constants.netStatCrAt:
	    {
	      int err = ok;
	      StationLocal stat = null;
              StationConfig statConfig = new StationConfig();

	      // read in station config info
	      int init               = in.readInt(); // not used in Java
	      statConfig.flowMode    = in.readInt();
	      statConfig.userMode    = in.readInt();
	      statConfig.restoreMode = in.readInt();
	      statConfig.blockMode   = in.readInt();
	      statConfig.prescale    = in.readInt();
	      statConfig.cue         = in.readInt();
	      statConfig.selectMode  = in.readInt();
	      for (int i=0; i < Constants.stationSelectInts; i++) {
		statConfig.select[i] = in.readInt();
	      }

	      // If both a function name and library name are sent,
	      // the user thinks he's talking to a C system when
	      // it's really a Java java.lang.System. If only a single name
	      // (class) is supplied, the user knows what he's doing.
	      int lengthFunc       = in.readInt();
	      int lengthLib        = in.readInt();
	      int lengthClass      = in.readInt();
	      int lengthName       = in.readInt();
              int position         = in.readInt();
              int parallelPosition = in.readInt();

              int length  = (lengthClass > lengthLib) ? lengthClass : lengthLib;
              length = (length > lengthFunc) ? length : lengthFunc;
              length = (length > lengthName) ? length : lengthName;
              byte[] buf = new byte[length];

	      if (lengthFunc > 0) {
		in.readFully(buf, 0, lengthFunc);
		statConfig.selectFunction = new String(buf, 0, lengthFunc-1, "ASCII");
	      }
	      if (lengthLib > 0) {
		in.readFully(buf, 0, lengthLib);
		statConfig.selectLibrary = new String(buf, 0, lengthLib-1, "ASCII");
	      }
	      if (lengthClass > 0) {
		in.readFully(buf, 0, lengthClass);
		statConfig.selectClass = new String(buf, 0, lengthClass-1, "ASCII");
	      }

	      in.readFully(buf, 0, lengthName);
	      String name = new String(buf, 0, lengthName-1, "ASCII");

	      try {
		stat = sys.createStation(statConfig, name, position, parallelPosition);
              }
	      catch (EtTooManyException ex) {err = Constants.errorTooMany;}
	      catch (EtExistsException ex) {err = Constants.errorExists;}
	      catch (EtException ex) {err = Constants.error;}

              out.writeInt(err);
	      if (err != ok) {out.writeInt(0);}
	      else {out.writeInt(stat.id);}
	      out.flush();
	    }
	    break;


            case  Constants.netStatRm:
	    {
	      int err = ok;
              int statId = in.readInt();

	      try {
		sys.removeStation(statId);
	      }
	      catch (EtException ex) {err = Constants.error;}

	      out.writeInt(err);
	      out.flush();
	    }
	    break;

            case  Constants.netStatSPos:
	    {
	      int err = ok;
              int statId = in.readInt();
              int position = in.readInt();
              int pposition = in.readInt();

	      try {
        	sys.setStationPosition(statId, position, pposition);
	      }
	      catch (EtException ex) {err = Constants.error;}

	      out.writeInt(err);
	      out.flush();
	    }
	    break;


            case  Constants.netStatGPos:
	    {
	      int position = -1, pPosition = 0;
	      int err = ok;
              int statId = in.readInt();

	      try {
        	position  = sys.getStationPosition(statId);
		pPosition = sys.getStationParallelPosition(statId);
	      }
	      catch (EtException ex) {err = Constants.error;}

	      out.writeInt(err);
	      out.writeInt(position);
	      out.writeInt(pPosition);
	      out.flush();
	    }
	    break;


            case  Constants.netStatIsAt:
	    {
	      int attached = 0; // not attached by default
              int statId = in.readInt();
              int attId  = in.readInt();

	      try {
        	attached = sys.stationAttached(statId, attId) ? 1 : 0;
	      }
	      catch (EtException ex) {attached = Constants.error;}

	      out.writeInt(attached);
	      out.flush();
	    }
	    break;


            case  Constants.netStatEx:
	    {
              boolean exists = true;
              int statId = 0;
	      int length = in.readInt();
	      byte[] buf = new byte[length];
	      in.readFully(buf, 0, length);
	      String name = new String(buf, 0, length-1, "ASCII");

              // in equivalent "C" function, station id is also returned
              try {
                statId = sys.stationNameToObject(name).id;
              }
              catch (EtException ex) {
                exists = false;
              }

	      out.writeInt(exists ? (int)1 : (int)0);
	      out.writeInt(statId);
	      out.flush();
	    }
	    break;


            case  Constants.netStatSSw:
            {
              StationLocal stat = null;
	      int[] select = new int[selectInts];
	      int   statId = in.readInt();
	      
              for (int i=0; i < selectInts; i++) {
                select[i] = in.readInt();
              }
	      
              try {stat = sys.stationIdToObject(statId);}
              catch (EtException ex) {}

	      if (stat != null) {
                stat.setSelectWords(select);
		out.writeInt(ok);
	      }
              else {
		out.writeInt(Constants.error);
              }
	      out.flush();
	    }
	    break;


            case  Constants.netStatGSw:
            {
	      int statId = in.readInt();
              StationLocal stat = null;

              try {stat = sys.stationIdToObject(statId);}
              catch (EtException ex) {}

	      if (stat != null) {
                out.writeInt(ok);
                for (int i=0; i < selectInts; i++) {
                  out.writeInt(stat.config.select[i]);
                }
	      }
              else {
		out.writeInt(Constants.error);
              }
	      out.flush();
	    }
	    break;


            case  Constants.netStatFunc:
            case  Constants.netStatLib:
            case  Constants.netStatClass:
            {
	      int statId = in.readInt();
	      StationLocal stat = null;

              try {
	        stat = sys.stationIdToObject(statId);
		String returnString = null;
		if (command == Constants.netStatFunc) {
		  returnString = stat.config.selectFunction;
		}
		else if (command == Constants.netStatLib) {
		  returnString = stat.config.selectLibrary;
		}
		else {
		  returnString = stat.config.selectClass;
		}

		if (returnString == null)  {
                  out.writeInt(Constants.error);
                  out.writeInt(0);
        	}
		else {
                  out.writeInt(ok);
                  out.writeInt(returnString.length() + 1);
		  try {
		    out.write(returnString.getBytes("ASCII"));
		  }
		  catch (UnsupportedEncodingException ex) {}
		  out.writeByte(0); // C null terminator
		}
	      }
	      catch (EtException ex) {
        	out.writeInt(Constants.error);
        	out.writeInt(-1);
	      }

	      out.flush();

	    }
	    break;


            default :
	     ;
	  } // switch(command)
	}   // if (command < Constants.netStatGAtts)


	// the following commands get values associated with stations
	else if (command < Constants.netStatSBlock) {
	  int val = 0;
          int statId = in.readInt();

	  StationLocal stat = null;
          try {stat = sys.stationIdToObject(statId);}
          catch (EtException ex) {}

	  if (stat == null) {
	    out.writeInt(Constants.error);
	  }
          else {
	    if (command == Constants.netStatGAtts) {
              synchronized (sys.stationLock) {
	        val = stat.attachments.size();
              }
            }
	    else if (command == Constants.netStatStatus)
              val = stat.status;
	    else if (command == Constants.netStatInCnt) {
              synchronized (stat.inputList) {
                val = stat.inputList.events.size();
              }
            }
	    else if (command == Constants.netStatOutCnt) {
              synchronized (stat.outputList) {
                val = stat.outputList.events.size();
              }
            }
	    else if (command == Constants.netStatGBlock)
              val = stat.config.blockMode;
	    else if (command == Constants.netStatGUser)
              val = stat.config.userMode;
	    else if (command == Constants.netStatGRestore)
              val = stat.config.restoreMode;
	    else if (command == Constants.netStatGPre)
              val = stat.config.prescale;
	    else if (command == Constants.netStatGCue)
              val = stat.config.cue;
	    else if (command == Constants.netStatGSelect)
              val = stat.config.selectMode;
	    else {
              if (config.debug >= Constants.debugError) {
        	  java.lang.System.out.println("commandLoop: bad command value");
              }
	      throw new EtReadException("bad command value");
	    }
	    out.writeInt(ok);
          }
          out.writeInt(val);
	  out.flush();
	}


	// the following commands set values associated with stations
	else if (command < Constants.netAttPut) {
          int statId = in.readInt();
	  int val    = in.readInt();

	  StationLocal stat = null;
          try {stat = sys.stationIdToObject(statId);}
          catch (EtException ex) {}

	  if (stat == null) {
	    out.writeInt(Constants.error);
	  }
          else {
	    if (command == Constants.netStatSBlock)
              stat.setBlockMode(val);
	    else if (command == Constants.netStatSUser)
              stat.setUserMode(val);
	    else if (command == Constants.netStatSRestore)
              stat.setRestoreMode(val);
	    else if (command == Constants.netStatSPre)
              stat.setPrescale(val);
	    else if (command == Constants.netStatSCue)
              stat.setCue(val);
	    else {
              if (config.debug >= Constants.debugError) {
        	  java.lang.System.out.println("commandLoop: bad command value");
              }
	      throw new EtReadException("bad command value");
	    }
	    out.writeInt(ok);
          }
	  out.flush();
	}


	// the following commands get values associated with attachments
	else if (command < Constants.netSysTmp) {
          int attId = in.readInt();
          // look locally for attachments
          AttachmentLocal att = (AttachmentLocal) attachments.get(new Integer(attId));
          if (att == null) {
            out.writeInt(Constants.error);
            out.writeLong(0);
          }
          else {
            out.writeInt(ok);
            if (command == Constants.netAttPut)
              out.writeLong(att.eventsPut);
            else if (command == Constants.netAttGet)
              out.writeLong(att.eventsGet);
            else if (command == Constants.netAttDump)
              out.writeLong(att.eventsDump);
            else if (command == Constants.netAttMake)
              out.writeLong(att.eventsMake);
          }
          out.flush();
        }


	// the following commands get values associated with the system
	else if (command <= Constants.netSysPid) {
	  int  val;

	  if (command == Constants.netSysTmp)
	    val = 0; // no temps (or all temps) by definition
	  else if (command == Constants.netSysTmpMax)
	    val = 0; // no max # of temps
	  else if (command == Constants.netSysStat) {
            synchronized (sys.stationLock) {
              val = sys.stations.size(); // # stations active or idle
            }
          }
	  else if (command == Constants.netSysStatMax)
            val = sys.config.stationsMax; // max # stations allowed
	  else if (command == Constants.netSysProc)
	    val = 0; // no processes since no shared memory
	  else if (command == Constants.netSysProcMax)
	    val = 0; // no max # of processes since no shared memory
	  else if (command == Constants.netSysAtt) {
            synchronized (sys.systemLock) {
              val = sys.attachments.size(); // # attachments
            }
          }
	  else if (command == Constants.netSysAttMax)
            val = sys.config.attachmentsMax; // max # attachments allowed
	  else if (command == Constants.netSysHBeat)
            val = 0; // no heartbeat since no shared mem
	  else if (command == Constants.netSysPid) {
	    val = -1; // no pids in Java
	  }
	  else {
            if (config.debug >= Constants.debugError) {
        	java.lang.System.out.println("commandLoop: bad command value");
            }
	    throw new EtReadException("bad command value");
	  }

	  out.writeInt(ok);
	  out.writeInt(val);
	  out.flush();
	}


	else if (command <= Constants.netSysHist) {
	  // command to distribute data about this ET system over the network
      	  if (command == Constants.netSysData) {
            // allow only 1 thread at a time a crack at updating information
            synchronized (sys.infoArray) {
              int err = sys.gatherSystemData();
              out.writeInt(err);
              if (err == ok) {
                // write size of data at beginning of array
                intToBytes(sys.dataLength, sys.infoArray, 0);
                // Send data + int holding data size
                out.write(sys.infoArray, 0, sys.dataLength+4);
              }
            }
            out.flush();
	  }

	  // send histogram data
	  else if (command == Constants.netSysHist) {
	    // not supported under Java (yet)
	    out.writeInt(Constants.error);
	    out.flush();
	  }

	}

	else {
          if (config.debug >= Constants.debugError) {
              java.lang.System.out.println("commandLoop: bad command value");
          }
	  throw new EtReadException("bad command value");
	}

      } // while(true)
    }  // try

    catch (EtReadException ex) {
    }
    catch (IOException ex) {
    }

    // We only end up down here if there's an error.
    // The client has crashed, therefore we must detach all
    // attachments or risked stopping the ET java.lang.System. The client
    // will not be asking for or processing any more events.

    Entry ent;
    for (Iterator i = attachments.entrySet().iterator(); i.hasNext(); ) {
      ent = (Entry) i.next();
      sys.detach((AttachmentLocal) ent.getValue());
    }

    if (config.debug >= Constants.debugError) {
        java.lang.System.out.println("commandLoop: remote client connection broken");
    }

    return;
  }
}
