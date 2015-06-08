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

/**
 * This class defines some useful constants.
 *
 * @author Carl Timmer
 */

public final class Constants {

  /** This constructor does nothing. */
  private Constants () {
  }

  /** Ints representing ascii for "cMsg is cool", used to filter out portscanning software. */
  public static final int[] magicNumbers = {0x45543269, 0x73324772, 0x72656174};

  // constants from et.h

  /** A convenient multicast address used for finding an ET system.
   * @see SystemOpenConfig#addMulticastAddr */
  public static final String multicastAddr       = "239.200.0.0";
  /** Specify a local host when opening an ET system.
   *  @see SystemOpenConfig#setHost */
  public static final String hostLocal           = ".local";
  /** Specify a remote host when opening an ET system.
   *  @see SystemOpenConfig#setHost */
  public static final String hostRemote          = ".remote";
  /** Allow any host when opening an ET system.
   *  @see SystemOpenConfig#setHost */
  public static final String hostAnywhere        = ".anywhere";
  /** Discover an ET system by multicasting.
   *  @see SystemOpenConfig#setContactMethod */
  public static final int    multicast           = 0;
  /** Discover an ET system by broadcasting.
   *  @see SystemOpenConfig#setContactMethod */
  public static final int    broadcast           = 1;
  /** Open an ET system by specifying host and port.
   *  @see SystemOpenConfig#setContactMethod */
  public static final int    direct              = 2;
  /** Discover an ET system by broadcasting and multicasting.
   *  @see SystemOpenConfig#setContactMethod */
  public static final int    broadAndMulticast   = 3;
  /** A default port on which to broadcast when finding an ET system. */
  public static final int    broadcastPort       = 11111;
  /** A default port on which to multicast when finding an ET system.
   *  @see SystemOpenConfig#addMulticastAddr
   *  @see SystemOpenConfig#setMulticastAddrs */
  public static final int    multicastPort       = 11112;
  /** A default port on which ET system TCP server makes connections with users.
   *  @see SystemOpenConfig#setTcpPort */
  public static final int    serverPort          = 11111;
  /** A default time-to-live value for multicasting.
   *  @see SystemOpenConfig#setTTL */
  public static final int    multicastTTL        = 1;
  /** A default value for the number of integers associated with each event that
   *  atations may use to select or filter that event. If this is changed, a
   *  recompilation of ET is necessary and communication with ET systems in
   *  which this value differs is impossible.
   */
  public static final int    stationSelectInts   = 4;
  /** A limit on the length of the ET system (or file) name's length. This limit
   *  is due to the C implementation. */
         static final int    fileNameLength      = 101;


  /** A policy that chooses the first ET system to respond when broadcasting
   *  and/or multicasting to find a system anywhere or remotely.
   *  @see SystemOpenConfig#setResponsePolicy */
  public static final int    policyFirst         = 0;
  /** A policy that chooses the first local ET system to respond when
   *  broadcasting and/or multicasting to find a system anywhere or remotely.
   *  If a local system does not respond, the first response is chosen.
   *  @see SystemOpenConfig#setResponsePolicy */
  public static final int    policyLocal         = 1;
  /** A policy that throws an EtTooManyException when multiple ET systems
   *  respond when broadcasting and/or multicasting to find an ET system
   *  anywhere or remotely.
   *  @see SystemOpenConfig#setResponsePolicy */
  public static final int    policyError         = 2;


  // system defaults

  /** An ET system's default number of events. */
  public static final int    defaultNumEvents    = 300;
  /** An ET system's default event size in bytes. */
  public static final int    defaultEventSize    = 1000;
  /* An ET system's default maximum number of stations. */
  public static final int    defaultStationsMax  = 20;
  /** An ET system's default maximum number of attachments. */
  public static final int    defaultAttsMax      = 50;

  // station stuff

  /** A station object's status meaning it has not been fully created yet. */
  public static final int    stationUnused          = 0;
  /** A station's status used in C implementations meaning the station is
   *  currently being created. */
  public static final int    stationCreating        = 1;
  /** A station's status meaning it exists but has no attachments. */
  public static final int    stationIdle            = 2;
  /** A station's status meaning it exists and has at least one attachment. */
  public static final int    stationActive          = 3;
  /** A station may have multiple attachments.
   *  @see StationConfig#setUserMode */
  public static final int    stationUserMulti       = 0;
  /** A station may only have one attachment.
   *  @see StationConfig#setUserMode */
  public static final int    stationUserSingle      = 1;
  /** A station will not block the flow of events. Once its cue is full, the
   *  station asks for no more events.
   *  @see StationConfig#setBlockMode */
  public static final int    stationNonBlocking     = 0;
  /** A station accepts every event into its cue and may block the flow of
   *  events.
   *  @see StationConfig#setBlockMode */
  public static final int    stationBlocking        = 1;
  /** All events are placed into a station's cue with no filtering applied
   *  (besides prescaling).
   *  @see StationConfig#setSelectMode */
  public static final int    stationSelectAll       = 1;
  /** Events are placed into a station's cue with a predefined filtering applied
   *  (besides prescaling).
   *  @see StationConfig#setSelectMode */
  public static final int    stationSelectMatch     = 2;
  /** Events are placed into a station's cue with a user defined filtering
   *  applied (besides prescaling).
   *  @see StationConfig#setSelectMode */
  public static final int    stationSelectUser      = 3;
  /** Events are placed into the cues of a single group of parallel stations
   *  with a round robin distribution algorithm.
   *  @see StationConfig#setSelectMode */
  public static final int    stationSelectRRobin    = 4;
  /** Events are placed into the cues of a single group of parallel stations
   *  in an algorithm designed to keep the cues equal in value.
   *  @see StationConfig#setSelectMode */
  public static final int    stationSelectEqualCue  = 5;
  /** Events owned by a crashed user process are restored to the ET system in
   *  the output list of the attachment's station.
   *  @see StationConfig#setRestoreMode */
  public static final int    stationRestoreOut      = 0;
  /** Events owned by a crashed user process are restored to the ET system in
   *  the input list of the attachment's station.
   *  @see StationConfig#setRestoreMode */
  public static final int    stationRestoreIn       = 1;
  /** Events owned by a crashed user process are restored to the ET system by
   *  putting them in GRAND_CENTRAL station (recycling them).
   *  @see StationConfig#setRestoreMode */
  public static final int    stationRestoreGC       = 2;
  /** Events owned by a crashed user process attached to a parallel station
   *  are restored to the ET system by redistributing them among that group
   *  of parallel stations (recycling them). Also, if the station has no more
   *  attachments, the events in its input list are also redistributed (unlike
   *  in stationRestoreGC mode where these events are put in the station's output
   *  list).
   *  @see StationConfig#setRestoreMode */
  public static final int    stationRestoreRedist   = 3;
  /** Events flow "normally" - that is serially - through a station.
   *  @see StationConfig#setFlowMode */
  public static final int    stationSerial          = 0;
  /** Events flow in parallel through stations in a single group.
   *  @see StationConfig#setFlowMode */
  public static final int    stationParallel        = 1;
  /** Events flow in parallel through stations in a single group with this station
   *  as the head of that group.
   *  @see StationConfig#setFlowMode */
  public static final int    stationParallelHead    = 2;

  // station defaults

  /** A default input list cue size for a nonblocking station.
   *  @see StationConfig#setCue */
  public static final int    defaultStationCue      = 10;
  /** A default prescale value for a station.
   *  @see StationConfig#setPrescale */
  public static final int    defaultStationPrescale = 1;

  // talk to C language ET systems with structures

  /** C structure state value for talking to C language ET systems. */
         static final int    structNew           = 0;
  /** C structure state value for talking to C language ET systems. */
         static final int    structOk            = 1;
	 
  /** Add station to end of linked list. */
  public static final int    end                 = -1;
  /** Make added station head of a new group of parallel stations. */
  public static final int    newHead             = -2;

  // Events

  /** Low event priority.
   *  @see Event#setPriority */
  public static final int    low                 = 0;
  /** High event priority.
   *  @see Event#setPriority */
  public static final int    high                = 1;
  /** Parse event priority information. */
         static final int    priorityMask        = 0x1;
  /** Event has been obtained with getEvents, not newEvents. */
         static final int    eventUsed           = 0;
  /** Event has been obtained with newEvents, not getEvents. */
         static final int    eventNew            = 1;
  /** System is event owner */
         static final int    system              = -1;

  /** User sleeps when waiting for events to fill a station's empty input list. */
  public static final int    sleep               = 0;
  /** User waits for a specified time when waiting for events to fill a
   *  station's empty input list. */
  public static final int    timed               = 1;
  /** User does not wait for events to fill station's empty input list, but
   *  returns immediately. */
  public static final int    async               = 2;
  /** User intends to modify the event's data. */
  public static final int    modify              = 4;
  /** User intends to modify only the event's header information. */
  public static final int    modifyHeader        = 8;
  /** User wants events automatically dumped (not put) if data is not modified. */
  public static final int    dump                = 16;
  /** Parse event waiting information. */
         static final int    waitMask            = 0x3;

  //public static final int    openNoWait          = 0;
  //public static final int    openWait            = 1;

  //public static final int    remote              = 0;
  //public static final int    local               = 1;
  //public static final int    localNoShare        = 2;

  //public static final String subnetDefault       = "default";
  //public static final String subnetAll           = "all";

  /** An event's data is OK. */
  public static final int    dataOk              = 0;
  /** An event's data is corrupted. */
  public static final int    dataCorrupt         = 1;
  /** An event's data may possibly be corrupted. */
  public static final int    dataPossiblyCorrupt = 2;
  /** Parse data status information. */
         static final int    dataMask            = 0x30;
  /** Parse data status information. */
         static final int    dataShift           = 4;

  /** An event's data is big endian. */
  public static final int    endianBig           = 0;
  /** An event's data is little endian. */
  public static final int    endianLittle        = 1;
  /** An event's data endian is the same as the local host's. */
  public static final int    endianLocal         = 2;
  /** An event's data endian is opposite of the local host's. */
  public static final int    endianNotLocal      = 3;
  /** An event's data endian is to be switched. */
  public static final int    endianSwitch        = 4;

  /** An event's data does not need to be swapped to be the same endian as the
   *  local host's. */
  public static final int    noSwap              = 0;
  /** An event's data needs to be swapped to be the same endian as the local
   *  host's. */
  public static final int    swap                = 1;

  /** Print out no status messages. */
  public static final int    debugNone           = 0;
  /** Print out only severe error messages. */
  public static final int    debugSevere         = 1;
  /** Print out severe and normal error messages. */
  public static final int    debugError          = 2;
  /** Print out all error and warning messages. */
  public static final int    debugWarn           = 3;
  /** Print out all error, warning, and informational messages. */
  public static final int    debugInfo           = 4;

  // C language ET system error codes

  /** No error. */
  public static final int    ok                  =  0;
  /** General error. */
  public static final int    error               = -1;
  /** Error specifying too many of something. */
  public static final int    errorTooMany        = -2;
  /** Error specifying that something already exists. */
  public static final int    errorExists         = -3;
  /** Error when a thread was told to wake up from a blocking read. */
  public static final int    errorWakeUp         = -4;
  /** Error specifying a time out. */
  public static final int    errorTimeout        = -5;
  /** Error specifying that a station has an empty input list. */
  public static final int    errorEmpty          = -6;
  /** Error specifying that a station's input list is being accessed by
   *  another thread or process. */
  public static final int    errorBusy           = -7;
  /** Error specifying that the ET system is dead. */
  public static final int    errorDead           = -8;
  /** Error specifying problems in a network read. */
  public static final int    errorRead           = -9;
  /** Error specifying problems in a network write. */
  public static final int    errorWrite          = -10;
  /** Error not used in java ET. */
  public static final int    errorRemote         = -11;
  /** Error not used in java ET. */
  public static final int    errorNoRemote       = -12;
  /** Error when memory asked for is too big. */
  public static final int    errorTooBig         = -13;


  // constants from private.h
/**
 * String length of dotted-decimal, ip address string
 * Some systems - but not all - define INET_ADDRSTRLEN
 * ("ddd.ddd.ddd.ddd\0" = 16)
 */
 static final int ipAddrStrLen = 16;

/**
 * MAXHOSTNAMELEN is defined to be 256 on Solaris and is the max length
 * of the host name so we add one for the terminator. On Linux the
 * situation is less clear but 257 appears to be the max (whether that
 * includes termination is not clear).
 * We need it to be uniform across all platforms since we transfer
 * this info across the network. Define it to be 256 for everyone.
 */
  static final int maxHostNameLen = 256;

  /** Java ET systems are 32 bit since arrays can only be of size Integer.MAX_VALUE. */
  static final int    bit64               = 0;
  /** Major ET version number. */
  static final int    version             = 11;
  /** Minor ET version number. */
  static final int    minorVersion        = 0;
  /** Maximum number of attachments to an ET system. */
  static final int    attachmentsMax      = 110;
  //static final int    ipAddrStrLen        = 16;
  //static final int    maxHostNameLen      = 256;

  /** ET system was implemented in the C language. */
  public static final int    langC               = 0;
  /** ET system was implemented in the C++ language. */
  public static final int    langCpp             = 1;
  /** ET system was implemented in the Java language. */
  public static final int    langJava            = 2;

  /** A mutex is not locked. Relevant only to C language ET systems. */
  public static final int    mutexUnlocked       = 0;
  /** A mutex is locked. Relevant only to C language ET systems. */
  public static final int    mutexLocked         = 1;
  /** The local UNIX operating system allows multiple processes to share POSIX
   *  mutexes. Relevant only to C language ET systems. */
  public static final int    mutexShare          = 0;
  /** The local UNIX operating system does not allow multiple processes to share
   *  POSIX mutexes. Relevant only to C language ET systems. */
  public static final int    mutexNoShare        = 1;

  //static final int    attUnused           = 0;
  //static final int    attActive           = 1;
  /** An attachment is not being told to wake up. */
  static final int    attContinue         = 0;
  /** An attachment is being told to wake up. */
  static final int    attQuit             = 1;
  /** An attachment is not blocked on a read statement. */
  static final int    attUnblocked        = 0;
  /** An attachment is blocked on a read statement. */
  static final int    attBlocked          = 1;

  // codes sent over the network to identify ET system routines to call
  static final int    netEvGetL        = 0;
  static final int    netEvsGetL       = 1;
  static final int    netEvPutL        = 2;
  static final int    netEvsPutL       = 3;
  static final int    netEvNewL        = 4;
  static final int    netEvsNewL       = 5;
  static final int    netEvDumpL       = 6;
  static final int    netEvsDumpL      = 7;
  static final int    netEvsNewGrpL    = 8;

  static final int    netEvGet         = 20;
  static final int    netEvsGet        = 21;
  static final int    netEvPut         = 22;
  static final int    netEvsPut        = 23;
  static final int    netEvNew         = 24;
  static final int    netEvsNew        = 25;
  static final int    netEvDump        = 26;
  static final int    netEvsDump       = 27;
  static final int    netEvsNewGrp     = 28;

  static final int    netAlive         = 40;
  static final int    netWait          = 41;
  static final int    netClose         = 42;
  static final int    netFClose        = 43;
  static final int    netWakeAtt       = 44;
  static final int    netWakeAll       = 45;

  static final int    netStatAtt       = 60;
  static final int    netStatDet       = 61;
  static final int    netStatCrAt      = 62;
  static final int    netStatRm        = 63;
  static final int    netStatSPos      = 64;
  static final int    netStatGPos      = 65;

  static final int    netStatIsAt      = 80;
  static final int    netStatEx        = 81;
  static final int    netStatSSw       = 82;
  static final int    netStatGSw       = 83;
  static final int    netStatLib       = 84;
  static final int    netStatFunc      = 85;
  static final int    netStatClass     = 86;

  static final int    netStatGAtts     = 100;
  static final int    netStatStatus    = 101;
  static final int    netStatInCnt     = 102;
  static final int    netStatOutCnt    = 103;
  static final int    netStatGBlock    = 104;
  static final int    netStatGUser     = 105;
  static final int    netStatGRestore  = 106;
  static final int    netStatGPre      = 107;
  static final int    netStatGCue      = 108;
  static final int    netStatGSelect   = 109;

  static final int    netStatSBlock    = 115;
  static final int    netStatSUser     = 116;
  static final int    netStatSRestore  = 117;
  static final int    netStatSPre      = 118;
  static final int    netStatSCue      = 119;

  static final int    netAttPut        = 130;
  static final int    netAttGet        = 131;
  static final int    netAttDump       = 132;
  static final int    netAttMake       = 133;

  static final int    netSysTmp        = 150;
  static final int    netSysTmpMax     = 151;
  static final int    netSysStat       = 152;
  static final int    netSysStatMax    = 153;
  static final int    netSysProc       = 154;
  static final int    netSysProcMax    = 155;
  static final int    netSysAtt        = 156;
  static final int    netSysAttMax     = 157;
  static final int    netSysHBeat      = 158;
  static final int    netSysPid        = 159;
  static final int    netSysGrp        = 160;

  static final int    netSysData       = 170;
  static final int    netSysHist       = 171;
}
