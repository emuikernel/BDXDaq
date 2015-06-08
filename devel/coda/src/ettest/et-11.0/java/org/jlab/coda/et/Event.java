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

/**
 * This class defines an ET event.
 *
 * @author Carl Timmer
 */

public class Event {

  // For efficiency in initializing event headers when passing
  // through GrandCentral, define some static final variables.
  private static final int   numSelectInts = Constants.stationSelectInts;
  private static final int[] controlInitValues = new int[numSelectInts];
  private static final int   newAge = Constants.eventNew;
  private static final int   system = Constants.system;
  private static final int   high = Constants.high;
  private static final int   low = Constants.low;
  private static final int   ok = Constants.dataOk;

  /** Unique id number. */
  long    id;
  /** Specifies whether the event was obtained as a new event (through
   *  newEvents), or as a "used" event (through getEvents). If the event is new,
   *  its value is {@link Constants#eventNew} otherwise
   *  {@link Constants#eventUsed}. */
  int     age;
  /** Group to which this event belongs. Used for multiple producers. */
  int     group;
  /** Event priority which is either high {@link Constants#high} or low
   *  {@link Constants#low}.
   */
  int     priority;
  /** The attachment id which owns or got the event. If it's owned by the
   *  system its value is {@link Constants#system}. */
  int     owner;
  /** Length of the valid data in bytes. */
  int     length;
  /** Size of the data buffer in bytes. */
  int     memSize;
  /** Size limit of events' data buffers in bytes. This is important to
   *  know when Java users connect to C-based ET systems. The C-based ET
   *  systems cannot allow users to increase an event's data size beyond
   *  what was originally allocated. In Java systems there is no size
   *  limit besides computer and JVM limits.
   */
  int     sizeLimit;
  /** Status of the data. It can be ok {@link Constants#dataOk}, corrupted
   *  {@link Constants#dataCorrupt}, or possibly corrupted
   *  {@link Constants#dataPossiblyCorrupt}. */
  int     dataStatus;
  /** An integer used to keep track of the data's byte ordering. */
  int     byteOrder;
  /** Specifies whether the user wants to read the event only, will modify
   *  only the event header, or will modify the data. */
  int     modify;
  /** An array of integers normally used by stations to filter events out of
   *  their input lists. It is used to control the flow of events through
   *  the ET system. */
  int[]   control;
  /** The event data is stored here. */
  byte[]  data;
  /** Flag specifying whether the ET system process is Java based or not. */
  boolean isJava;


  /** Creates an event object for users of Java-based ET systems or by the
   *  system itself. Event objects are only created once in the ET
   *  system process - when the ET system is started up.
   *  @param size size of the data array in bytes
   */
  Event(int size) {
    memSize = size;
    isJava  = true;
    data    = new byte[memSize];
    control = new int[numSelectInts];
    init();
  }

  /** Creates an event object for ET system users. 
   *  @param size size of the data array in bytes.
   *  @param limit limit on the size of the data array in bytes. Only used
   *         for C-based ET systems.
   *  @param isJavaSystem is ET system Java based?
   */
  Event(int size, int limit, boolean isJavaSystem) {
    memSize   = size;
    sizeLimit = limit;
    isJava    = isJavaSystem;
    data      = new byte[memSize];
    control   = new int[numSelectInts];
    init();
  }

  /** Initialize an event's fields. Called for an event each time it passes
   *  through GRAND_CENTRAL station. */
  void init() {
    age        = newAge;
    priority   = low;
    owner      = system;
    length     = 0;
    modify     = 0;
    byteOrder  = 0x04030201;
    dataStatus = ok;
    System.arraycopy(controlInitValues, 0, control, 0, numSelectInts);
  }

  // public gets

  /** Gets the event's id number.
   *  @return event's id number */
  public long   getId()   {return id;}
  /** Gets the event's priority.
   *  @return event's priority */
  public int    getPriority()   {return priority;}
  /** Gets the length of the data in bytes.
   *  @return length of the data in bytes */
  public int    getLength()     {return length;}
  /** Gets the size of the data buffer in bytes.
   *  @return size of the data buffer in bytes */
  public int    getMemSize()    {return memSize;}
  /** Gets the size limit of the data buffer in bytes when using a
   *  C-based ET system.
   *  @return size size limit of the data buffer in bytes */
  public int    getSizeLimit()    {return sizeLimit;}
  /** Gets the status of the data.
   *  @return status of the data */
  public int    getDataStatus() {return dataStatus;}
  /** Gets the event's modify value.
   *  @return event's modify value */
  public int    getModify()     {return modify;}
  /** Gets the event's control array.
   *  @return event's control array */
  public int[]  getControl()    {return (int[])control.clone();}
  /** Gets the event's data array.
   *  @return event's data array */
  public byte[] getData()       {return data;}
  /** Gets the event's data array.
   *  @return a clone of the event's data array */
  public byte[] copyData()      {return (byte[])data.clone();}

  // public sets

  /** Sets the event's data without copying. The length and memSize members of
   *  the event are automatically set to the data array's length.
   *  @param dat data array
   */
  public void setData(byte[] dat) throws EtException {
    if (!isJava) {
      // In C-based ET systems, user cannot increase data size beyond
      // what was initially allocated.
      if (dat.length > sizeLimit) {
        throw new EtException("data array is too big, limit is "
				+ sizeLimit + " bytes");
      }
    }
    data    = dat;
    length  = dat.length;
    memSize = dat.length;
  }

  /** Set the event's data by copying it in. The event's length member
   *  is set to the length of the argument array.
   *  @param dat data array
   *  @exception org.jlab.coda.et.EtException
   *     if the data array is the wrong size
   */
  public void copyDataIn(byte[] dat) throws EtException {
    if (dat.length > memSize) {
      throw new EtException("data array is too big, limit is "
				+ memSize + " bytes");
    }
    System.arraycopy(dat, 0, data, 0, dat.length);
    length = dat.length;
  }

  /** Set the event's data by copying it in. The event's length member
   *  is not changed.
   *  @param dat data array
   *  @param srcOff offset in "dat" byte array
   *  @param destOff offset in the event's byte array
   *  @param len bytes of data to copy
   *  @exception org.jlab.coda.et.EtException
   *     if the data array is the wrong size
   */
  public void copyDataIn(byte[] dat, int srcOff, int destOff, int len) throws EtException {
    if (len > data.length) {
      throw new EtException("too much data, try using \"setData\" method");
    }
    System.arraycopy(dat, srcOff, data, destOff, len);
  }

  /** Sets the event's priority.
   *  @param pri event priority
   *  @exception org.jlab.coda.et.EtException
   *     if argument is a bad value
   */
  public void setPriority(int pri) throws EtException {
    if (pri != low && pri != high) {
      throw new EtException("bad value for event priority");
    }
    priority = pri;
  }

  /** Sets the event's data length in bytes.
   *  @param len data length
   *  @exception org.jlab.coda.et.EtException
   *     if length is less than zero
   */
  public void setLength(int len) throws EtException {
    if (len < 0) {
      throw new EtException("bad value for event data length");
    }
    length = len;
  }

  /** Sets the event's control array by copying it in.
   *  @param con control array
   *  @exception org.jlab.coda.et.EtException
   *     if control array has the wrong number of elements
   */
  public void setControl(int[] con) throws EtException {
    if (con.length != numSelectInts) {
      throw new EtException("wrong number of elements in control array");
    }
    System.arraycopy(con, 0, control, 0, numSelectInts);
  }

  /** Sets the event's data status.
   *  @param status data status
   *  @exception org.jlab.coda.et.EtException
   *     if argument is a bad value
   */
  public void setDataStatus(int status) throws EtException {
    if ((status != Constants.dataOk) &&
	(status != Constants.dataCorrupt) &&
	(status != Constants.dataPossiblyCorrupt)) {
      throw new EtException("bad value for data status");
    }
    dataStatus = status;
  }

  // byte order stuff

  /** Gets the event's byte order - either {@link Constants#endianBig} or
   *  {@link Constants#endianLittle}.
   *  @return event's byte order */
  public int getByteOrder() {
    // java is always big endian
    return ((byteOrder == 0x04030201) ?
                  Constants.endianBig : Constants.endianLittle);
  }

  /** Set the event's byte order. Values can be {@link Constants#endianBig},
   *  {@link Constants#endianLittle}, {@link Constants#endianLocal},
   *  {@link Constants#endianNotLocal}, or {@link Constants#endianSwitch}
   *  @param endian endian value
   *  @exception org.jlab.coda.et.EtException
   *     if argument is a bad value
   */
  public void setByteOrder(int endian) throws EtException {
    if (endian == Constants.endianBig) {
      byteOrder = 0x04030201;
    }
    else if (endian == Constants.endianLittle) {
      byteOrder = 0x01020304;
    }
    else if (endian == Constants.endianLocal) {
      byteOrder = 0x04030201;
    }
    else if (endian == Constants.endianNotLocal) {
      byteOrder = 0x01020304;
    }
    else if (endian == Constants.endianSwitch) {
      byteOrder = swapInt(byteOrder);
    }
    else {
      throw new EtException("bad value for byte order");
    }
    return;
  }

  /** Tells caller if the event data needs to be swapped in order to be the
   *  correct byte order.
   *  @return <code>true</code> if swapping is needed, otherwise <code>false
   *  </code>
   *  @exception org.jlab.coda.et.EtException
   *     if the byte order has a bad value
   */
  public boolean needToSwap() throws EtException {
    if (byteOrder == 0x04030201)
      return false;
    else if (byteOrder == 0x01020304) {
      return true;
    }
    throw new EtException("byteOrder member has bad value");
  }

  /** Swaps data that is in the CODA format only. */
  public void codaSwap() {
    // See da.h for the following definitions & more
   int   dtBank = 0x10;
   int[] dtSwap = {0,2,2,0,1,1,0,0, 3,2,3,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0};

    // see et_network.c for algorithm
    int  firstInt, secondInt, dType, bLen;
    int intIndex = 0, byteIndex = 0;
    boolean sameEndian = (byteOrder == 0x04030201);

    // swap event byteOrder variable to mirror data
    byteOrder = swapInt(byteOrder);

    // swap CODA data
    while (intIndex < length/4) {
      byteIndex = 4*intIndex;
      firstInt  = bytesToInt(data, byteIndex);
      secondInt = bytesToInt(data, byteIndex+4);

      if (sameEndian) {
	bLen  = firstInt - 1;
	dType = ((secondInt)&0xff00)>>>8;
      }
      else {
	bLen  = swapInt(firstInt) - 1;
	dType = ((swapInt(secondInt))&0xff00)>>>8;
      }

      // Swap length
      intToBytes(swapInt(firstInt),  data, byteIndex);
      // Swap bank header
      intToBytes(swapInt(secondInt), data, byteIndex+4);

      intIndex  += 2;
      byteIndex += 8;

      if(dType != dtBank) {
	switch(dtSwap[dType]) {
	case 0:  // no swap
	  intIndex += bLen;
	  break;

	case 1:  // short swap
	  for(int j=0; j < bLen<<1; j++) {
	    swapArrayShort(data, byteIndex);
	    byteIndex += 2;
	  }
	  intIndex += bLen;
	  break;

	case 2:  // int swap (also long in "C")
	case 3:  // float swap (double in "C")
	  for (int j=0; j < bLen; j++) {
	    swapArrayInt(data, byteIndex);
	    byteIndex += 4;
	  }
	  intIndex += bLen;
	  break;

	default:  // no swap
	  intIndex += bLen;
	}
      }
    } // while
  }


  // static methods

    /**
     * Converts 4 bytes of a byte array into an integer.
     *
     * @param b   byte array
     * @param off offset into the byte array (0 = start at first element)
     * @return integer value
     */
    public static final int bytesToInt(byte[] b, int off) {
        return (int) ((b[off]     & 0xff) << 24 |
                      (b[off + 1] & 0xff) << 16 |
                      (b[off + 2] & 0xff) <<  8 |
                       b[off + 3] & 0xff);
    }

    /**
     * Copies an integer value into 4 bytes of a byte array.
     * @param intVal integer value
     * @param b byte array
     * @param off offset into the byte array
     */
    public static final void intToBytes(int intVal, byte[] b, int off) {
      b[off]   = (byte) ((intVal & 0xff000000) >>> 24);
      b[off+1] = (byte) ((intVal & 0x00ff0000) >>> 16);
      b[off+2] = (byte) ((intVal & 0x0000ff00) >>>  8);
      b[off+3] = (byte)  (intVal & 0x000000ff);
    }


    /**
     * Converts 8 bytes of a byte array into a long.
     * @param b byte array
     * @param off offset into the byte array (0 = start at first element)
     * @return long value
     */
    public static final long bytesToLong(byte[] b, int off) {
      return (long) ((b[off]   & 0xffL) << 56 |
                     (b[off+1] & 0xffL) << 48 |
                     (b[off+2] & 0xffL) << 40 |
                     (b[off+3] & 0xffL) << 32 |
                     (b[off+4] & 0xffL) << 24 |
                     (b[off+5] & 0xffL) << 16 |
                     (b[off+6] & 0xffL) <<  8 |
                      b[off+7] & 0xffL);
    }

    /**
     * Copies an long (64 bit) value into 8 bytes of a byte array.
     * @param longVal long value
     * @param b byte array
     * @param off offset into the byte array
     */
    public static final void longToBytes(long longVal, byte[] b, int off) {
        b[off]   = (byte) ((longVal & 0xff00000000000000L) >>> 56);
        b[off+1] = (byte) ((longVal & 0x00ff000000000000L) >>> 48);
        b[off+2] = (byte) ((longVal & 0x0000ff0000000000L) >>> 40);
        b[off+3] = (byte) ((longVal & 0x000000ff00000000L) >>> 32);
        b[off+4] = (byte) ((longVal & 0x00000000ff000000L) >>> 24);
        b[off+5] = (byte) ((longVal & 0x0000000000ff0000L) >>> 16);
        b[off+6] = (byte) ((longVal & 0x000000000000ff00L) >>>  8);
        b[off+7] = (byte)  (longVal & 0x00000000000000ffL);
    }


  /**
   * Swaps the byte order of an integer.
   * @param n integer to be swapped
   * @return swapped integer
   */
  public static final int swapInt(int n) {
    return  ((n << 24) | (n >>> 24)  |
            ((n & 0x0000ff00) <<  8) |
            ((n & 0x00ff0000) >>> 8));
  }

  /**
   * Swaps the byte order of a short.
   * @param n short to be swapped
   * @return swapped short
   */
  public static final short swapShort(short n) {
    return (short) ((n << 8) | (n >>> 8));
  }

  /**
   * Swaps 4 bytes of a byte array in place.
   * @param b byte array
   * @param off offset into the byte array
   */
  public static final void swapArrayInt(byte[] b, int off) {
    byte b1, b2, b3, b4;
    b1 = b[off];
    b2 = b[off+1];
    b3 = b[off+2];
    b4 = b[off+3];
    b[off+3] = b1;
    b[off+2] = b2;
    b[off+1] = b3;
    b[off]   = b4;
  }

  /**
   * Converts 2 bytes of a byte array into a short.
   * @param b byte array
   * @param off offset into the byte array (0 = start at first element)
   * @return short value
   */
  public static final short bytesToShort(byte[] b, int off) {
    return (short) (((b[off]&0xff) << 8) | (b[off+1]&0xff));
  }


  /**
   * Copies a short value into 2 bytes of a byte array.
   * @param shortVal short value
   * @param b byte array
   * @param off offset into the byte array
   */
  public static final void shortToBytes(short shortVal, byte[] b, int off) {
    b[off]   = (byte) ((shortVal & 0xff00) >>> 8);
    b[off+1] = (byte)  (shortVal & 0x00ff);
  }

  /**
   * Swaps 2 bytes of a byte array in place.
   * @param b byte array
   * @param off offset into the byte array
   */
  public static final void swapArrayShort(byte[] b, int off) {
    byte b1, b2;
    b1 = b[off];
    b2 = b[off+1];
    b[off+1] = b1;
    b[off]   = b2;
  }

}
