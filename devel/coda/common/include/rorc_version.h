/* rorc_version.h */

/* software driver version number */ 
#define RORC_LIBRARY_VERSION "v5.3.8"
#define RORC_DRIVER_MAJOR 5
#define RORC_DRIVER_MINOR 3
#define RORC_DRIVER_RELEASE 8

/* allowed firmware version numbers */
#define PRORC_VERSION_MIN 174
#define PRORC_VERSION_MAX 0
#define DRORC_CH1_VERSION_MIN 111
#define DRORC_CH1_VERSION_MAX 0
#define DRORC_CH2_VERSION_MIN 111
#define DRORC_CH2_VERSION_MAX 0

#ifdef SOURCE_FILE
/*****************************************************************************/
/* The following is used to identify the module (see "man what" for details) */
/* variables SOURCE_FILE and                                                 */
/*           DESCRIPTION must be defined before include this file            */
char rorcIdent[]="@(#)""" SOURCE_FILE """: """ DESCRIPTION \
     """ """ RORC_LIBRARY_VERSION """ compiled """ __DATE__ """ """ __TIME__;
/*****************************************************************************/
#endif

/*
 * New in version 5.3.8 (19-Feb_2009)
 *    shorter loop in rorcMapCannel() for PCI speed check
 *
 * New in version 5.3.7 (12-Feb_2009)
 *    new option in rorc_receive: -x -1, meaning not to stop at DTSTW error
 *    rorc_receive writes to file after every page read      
 *    us sleeps in rorc_recieve, before each readyFIFO loadings
 *    new tool getIdSIU.c
 *
 * New in version 5.3.6 (02-Nov-2008)
 *    ddlRead/WriteDataBlock() use one timeout value in us 
 *                   both for memory and PCI loops
 *    using pci_loop_per_usec for time-out
 *    adding exit code to siu_reset.c              
 *
 * New in version 5.3.5 (24-Jun-2008)
 *    fixing bug at block read in rorc_receive.c
 *    fixing bugs in scripts
 *
 * New in version 5.3.4 (16-May-2008)
 *    change of pciff to diuff in rorc_traffic_monitor
 *    rorc_receive can produce binary output file
 *
 * New in version 5.3.3 (26-Nov-2007)
 *    rorc_receive can identify RORC device using revision and serial numbers
 *
 * New in version 5.3.2 (19-Oct-2007)
 *    check PCI command/status word in a less strict way as before
 *                                  in RORC open routines()
 *
 * New in version 5.3.1 (09-Sep-2007)
 *    FeC2 returns FeC2_0 or FeC2_1 exit status
 *    wait the specified timeout us to check if the transfer
 *                          has been finished in ddlWriteDataBlock()
 *
 * New in version 5.3.0 (07-Sep-2007)
 *    fec2rorc library excluded
 *
 * New in version 5.2.4 (10-Aug-2007)
 *    correct bug in rorcFind()
 *
 * New in version 5.2.3 (26-Aug-2007)
 *    GNUmakefile makes rorc_lib_l.o and rorc_ddl_l.o libraries for DATE
 *    change SMILOPTS in GNUmakefile
 *    rorc_status and rorc_check_pci print PCI command/status register 
 *    check PCI command/status word in rorc driver and open routines
 *    change sprintf and strcat to snprintf and strncat in rorc_date.h,
 *           rorc_send_command.c, rorc_set_femu_.c
 *
 * New in version 5.2.2 (25-Jul-2007)
 *    simplification of infoLogger call in rorc_date.h and GNUmakefile
 *    shell scripts ddg, FeC2 and rorc_receive can be called form any directory
 *    drorc_flash_programmer can configure RORC device from flash memory
 *    new programs rorc_measure_pci and rorc_traffic_monitor
 *    new routine rorcReadFifoCounters()
 *    new routine rorcMeasurePciFreq()
 *    new routine rorcOpenForMonitor()
 *    FeC2 command CALL_FILE modified
 *    new FeC2 commands: LOOP and LOOP_OFF
 *    adding "@mkdir -p ${BINS_DIR}/" to Makefile and GNUmakefile
 *    modifying printouts in rorcFind() and FeC2
 *    -fPIC option added to flags POPTS in Makefile and CFLAGS in GNUmakefile
 *
 * New in version 5.2.1 (23-May-2007)
 *    new version 0.5.c of fec2rorc library:
 *       Some minor bugs fixed
 *       Corrected physmem unmapping
 *    changing rorc_qfind
 *    minor bug in rorcArmDDL() fixed
 *
 * New in version 5.2.0 (26-Apr-2007)
 *    new test script rorc_monitor
 *    new test program rorc_qfind 
 *    new routine: rorcQuickFind()
 *    new version of rorcFind()
 *    constant RORC_DRIVER_VERSION renamed to RORC_LIBRARY_VERSION
 *    new rorcFind() routine working with /proc/rorc_map
 *    rorc_driver creates /proc/rorc_map
 *    rorc_driver reads serial number
 *
 * New in version 5.1.7 (19-Apr-2007)
 *    minor bug in ddlSetSiuLoopBack()
 *    printing error text, not codes in rorc_receive program
 *    new version 0.5.c of fec2rorc library:
 *       Altro functionality completely re-written
 *       Slight API modifications
 *       Internatlisation of fec_chan_addr calculation
 *       Modification of fec_* functions to follow standard
 *       Unification of style in line with documentation
 *
 * New in version 5.1.6 (24-Mar-2007)
 *    less printout in ddlSendData() and in FeC2 program
 *
 * New in version 5.1.5 (15-Mar-2007)
 *    change LOG_TO call to LOG in rorc_date.h
 *    rorc driver reads PCI bus mode and speed
 *    new test program rorc_empty_datafifos
 *    modifying rorcArmDDL() in rorc_ddl.c
 *    new routine rorcEmptyDataFifos() in rorc_lib.c
 *    including test script tstmonitor in scripts sub directory 
 *    fec2rorc library included
 *    changing wait counters type from int (31 bits) to long long (63 bits)
 *      and changing its range from (0-0x0FFFFFF) to (0-0x7FFFFFFFFFFFFFFF)
 *    modifying rorcPrintStatus() and ddlPrintStatus() routines
 *    new program: rorc_check_pci
 *    print PCI bus mode, speed and RORC serial in drorc_flash_programmer
 *    print PCI bus ID in hex format in drorc_flash_programmer
 *    changing l1_trigger mask from 0x3FF to 0xFF in ddg.c
 *
 * New in version 5.1.4 (17-Nov-2006)
 *    new script sub-directory containing functional test scripts
 *    new "quiet" libraries: rorc_lib_q.o and rorc_ddl_q.o
 *    allow long file name (255 chars) in drorc_flash_programmer
 *    new routine in rorc_ddl.c: ddlPrintStatus()
 *    new routine in rorc_lib.c: rorcPrintStatus()
 *    check if month greater than 12 when interprets fw id
 *    change LOG_NORMAL_TH to LOG_DETAILED_TH in rorc_date.h
 *    printing sw version at the beginning of rorc_send run
 *    flushing stdout before looping in rorc_receive and rorc_send
 *
 * New in version 5.1.3 (11-Aug-2006)
 *    modifications for similar SLC3/SLC4 installation procedure
 *
 * New in version 5.1.2 (11-Aug-2006)
 *    adding license information to rorc_driver
 *    creating install_rorc_driver, load_rorc_driver and 
 *                                         reload_rorc_driver scripts
 *    modifying GNUmakefile and Makefile by calling the above scripts
 *
 * New in version 5.1.1 (27-Jul-2006) 
 *    new Makefile and GNUmakefile working under both SLC3 and SLC4
 *    modified driver for SLC4
 *    fixing minor error in reset sequence of rorc_receive
 *
 * New in version 4.4.6 (26-Jun-2006)
 *    rorc_receive prints block number on output file
 *    New program: drorc_flash_programmer for DRORC remote configuration
 *    FeC2 can identify the RORC device using revision and serial numbers
 *    FeC2 accepts environment variables in the script
 *
 * New in version 4.4.5 (02-May-2006)
 *    correcting the printout of the stop time in ddg.c
 *    accept revision number 4 (DRORC2)
 *    sleep after loop-back settings in rorc_receive program
 *    check time-out option in stand-alone programs
 *    delete lines not in use from GNUmakefile and Makefile 
 *    changing diagnostic printouts in ddlSendData() ddlReadDataBlock() 
 *    additional prints of RORC Hw and Fw id in rorc_status program
 *
 * New in version 4.4.4 (05-Jan-2006)
 *    new FeC2 command: write_block_multiple
 *    clear SIU/DIU status after reset on rorc_receive program
 *    print sw version at the beginning of FeC2 and rorc_receive runs
 *    changing DTSTW register check to data transmit ending check  
 *    fixing minor code problem in rorc_aux.c 
 *    fixing minor code problem in rorc_set_femu.c 
 *    removing -static option from the compilation of ddg program
 *
 * New in version 4.4.3 (05-Sep-2005)
 *    Allow DDG to use non-integrated D-RORC in case of sw generation
 *    Add new routines checking DTSTW register emptyness
 *    Add read_ddl_status command to FeC2
 *    Add examples
 *    Checking Rx DMA count in ddlReadDataBlock()
 *    Checking Tx DMA count in ddlSendData()
 *    Infinite wait time for rorc_send
 *    New DDL commands for test mode
 *    New commands for SIU RX loopback
 *    Add text "Error" to each error messages
 *    Change variable name diu_version to siu_version in siu_status.c
 *
 * New in version 4.4.2 (16-Apr-2005) 
 *    Fix a bug in GNUmakefile
 *    Ommit "nobody" from makefiles
 *
 * New in version 4.4.1 (06-Apr-2005) 
 *    Include ddg in rorc sw package
 *    Use of physmem package version 3.1
 *    rorc_driver.o and KOPT entries removed form the OBJS list of 
 *                                                      the GNUmakefile
 *    FeC2' physmem offset depends on minor and channel number
 *
 * New in version 4.3.14 (18-Mar-2005) 
 *    fix time-out bug in ddlReadDataBlock()
 *    FeC2 stops if protocol error occurs
 *
 * New in version 4.3.13 (10-Mar-2005) 
 *    FeC2 sets failed code if too much/less data arrive
 *    for the READ_AND_CHECK_BLOCK command
 *
 * New in version 4.3.13 (03-Mar-2005) 
 *    FeC2 prints a warning if FEE command or address is too large
 *
 * New in version 4.3.13 (22-Feb-2005)
 *    introduce Rx and Tx DMA count and DMA address registers
 *    modifying rorc_reg program accordingly
 *
 * New in version 4.3.12 (08-Feb-2005)
 *    fix a bug in ddlWriteDataBlock() routine 
 *
 * New in version 4.3.11 (31-Jan-2005)
 *    no use of strtok in token browse routine 
 *    new options -w and -y for rorc_receive program
 *    change variable VERSION to RORC_DRIVER_VERSION
 *    do not reset RORC in ddlFindDiuVersion
 *                   routine in case of INTEG RORC
 *    use usec for working indicator (spinning '\') and 
 *                   print page or GB moved together with the indicator
 *
 * New in version 4.3.10: (13-Dec-2004)
 *    -I${DATE_BANKS_MANAGER_DIR}  added to COPTS in GNUmakefile
 *
 * New in version 4.3.9: (10-Dec-2004)
 *    module identification string for what command is not static any more
 *    rorc_id prints the compiled sw version
 *    rorc_find, rorcMapChannel and rorcCheckOpen returns with error
 *                      if /sbin/lsmod does not shows rorc_driver
 *
 * New in version 4.3.8: (20-Oct-2004)
 *    Fixing bug in the HW id check 
 *
 * New in version 4.3.7: (19-Oct-2004)
 *    Correcting GNUMakefile 
 *
 * New in version 4.3.6: (19-Oct-2004)
 *    Use upper case letters for HW id check
 *
 * New in version 4.3.5: (06-Aug-2004)
 *    In GNUMakefile -I/date/physmem replaced by -I${DATE_PHYSMEM_DIR}
 *    Wait 10 ms after SIU reset in ddlResetSiu() both for pRorc and D-RORC
 *    Calibrate loop time in rorcMapChannel 
 *    rorc_GUI.tcl added to the sw package
 *
 * New in version 4.3.4:
 *    Status interpreting texts to rorc_lib.h
 *
 * New in version 4.3.3:
 *    Adding HLT flow-control on/off command
 *
 * New in version 4.3.2:
 *    Changing path pRorc to rorc 
 * 
 * New in version 4.3.1:
 *    Checking RORC firmware version
 *    Changing pRorcFind
 *    Changing some routine names: instead of "pRorc",
 *                             they should start with "rorc"
 *    Allow in-line command in FeC2 script
 *    New FeC2 command: WAIT <usecs>
 *    Fix bug in siu_reset program
 *    Changing D-RORC Hw id format
 *    Adding HLT splitter on/off command
 */
