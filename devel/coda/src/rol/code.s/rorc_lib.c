#ifdef Linux

/* rorc_lib.c
 *
 * Library routines for ALICE RORC programs 
 *
 * last updated: 19/02/2009
 * written by: Peter Csato and Ervin Denes
 *    
 * history: 
 *
 * v.5.3.8  19-Feb-2009: shorter loop in rorcMapCannel() for PCI speed check
 * v 5.3.6  02-Nov-2008: measuring pci_loop_per_usec in rorcMapchannel()
 * v.5.3.2  19-Oct-2007: modifying PCI command/status check
 * v.5.2.4  10-Sep-2007: correct bug in rorcFind()
 * v.5.2.3  11-Aug-2007: check PCI command/status word in rorc open routines
 * v.5.2.2  11-Jul-2007: new routine rorcOpenForMonitor()
 *          10-Jul-2007: new routine rorcReadFifoCounters()
 *          01-Jun-2007: modifying printouts in rorcFind()
 * v.5.2.0  24-Apr-2007: new routine: rorcQuickFind()
 *          23-Apr-2007: new rorcFind() routine
 *          20-Apr-2007: read RORC's s/n from the driver at open time
 * v.5.1.5  14-Mar-2007: new routine rorcInterpretBusMode() and ... BusSpeed()
 *          02-Mar-2007: new routine rorcEmptyDataFifos()
 *          18-Jan-2007: modifying rorcPrintStatus() 
 * v.5.1.4  08-Nov-2006: new routine: rorcPrintStatus()
 *          21-Oct-2006: check if month greater than 12 when interprets fw id 
 * v.4.4.5  17-Mar-2006: allow revision number 4
 * v.4.4.5  17-Feb-2006: calculate the possible maximum respond time value
 * v.4.4.3  02-Sep-2005: new routines checking DTSTW register emptyness
 * v.4.4.3  11-Jul-2005: new routines rorcReadRxDmaCount and rorcReadTxDmaCount
 * v.4.4.1  04-Apr-2005: changing PHYSMEM_DEV to /dev/physmem0
 * v.4.3.8  19-Oct-2004: fixing bug in HW id check
 * v.4.3.6  19-Oct-2004: use upper case letters for HW id check
 * v.4.3.5  06-Aug-2004: calibrate loop time in rorcMapChannel()
 * v.4.3.4  08-Jul-2004: status interpreting texts to rorc_lib.h
 * v.4.3.3  07-Jul-2004: adding HLT flow-control on/off command
 * v.4.3.2  05-Jul-2004: fixing small bugs                  
 * v.4.3.1  02-Jul-2004: adding HLT splitter on/off command
 * v.4.3.1  14-Apr-2004: changing D-RORC Hw id format
 * v.4.3    19-Nov-2003: changing the locking mechanisme
 * v.4.2.3  26-Jan-2004: new version of RORC hw id routines
 * v.4.2.3  15-Jan-2004: change pRorcFind to rorcFind
 * v.4.2.3  05-Dec-2003: check RORC fw and sw version compability  
 * v.4.2    30-Oct-2003: Hw id for D-RORC
 * v.4.2    26-Aug-2003: locking a file if channel is busy
 * v.4.2    22-Aug-2003: rorcMapChannel added for 2 channels
 *   
 ****************************************************/
#define LIB_SOURCE_FILE "rorc_lib.c"
#define LIB_DESCRIPTION "ALICE RORC library"

#include "rorc_ddl.h"
#include "physmem.h"
char libIdent[]="@(#)""" LIB_SOURCE_FILE """: """ LIB_DESCRIPTION \
     """ """RORC_LIBRARY_VERSION """ compiled """ __DATE__ """ """ __TIME__;

/*
 * Global variables ------------------------------------------------
 */

rorcId_t rorc_id[RORC_MAX_REVISION + 1] = 
  { // id_text, sn_pos, ch_pos, ver_pos, ld_pos
    { ""      ,    0,     0,       0,      0    },
    { "pRORC" ,   15,     0,       6,      0    },
    { "D-RORC",   33,    11,       7,     20    },
    { "D-RORC",   33,    11,       7,     20    },
    { "DRORC2",   33,    11,       7,     20    }
  };

char *bus_speed_txt[] = {" 33 MHz", " 66 MHz", "100 MHz", "133 MHz"};
char *bus_mode_txt[] = {"PCI  ", "PCI-X"};

rorcReturn_t rorc_return[MAX_RETURN_CODE] =
  {
    { RORC_STATUS_OK,                   "STATUS_OK"                   },
    { RORC_STATUS_ERROR,                "STATUS_ERROR"                },
    { RORC_INVALID_PARAM,               "INVALID_PARAM"               } ,

    { RORC_LINK_NOT_ON,                 "LINK_NOT_ON"                 },
    { RORC_CMD_NOT_ALLOWED,             "CMD_NOT_ALLOWED"             },
    { RORC_NOT_ACCEPTED,                "NOT_ACCEPTED"                },
    { RORC_NOT_ABLE,                    "NOT_ABLE"                    },
    { RORC_TIMEOUT,                     "TIMEOUT"                     },

    { RORC_FF_FULL,                     "FF_FULL"                     },
    { RORC_FF_EMPTY,                    "FF_EMPTY"                    },

    { RORC_NOT_ENOUGH_REPLY,            "NOT_ENOUGH_REPLY"            },
    { RORC_TOO_MANY_REPLY,              "TOO_MANY_REPLY"              },

    { RORC_DATA_BLOCK_NOT_ARRIVED,      "DATA_BLOCK_NOT_ARRIVED"      },
    { RORC_NOT_END_OF_EVENT_ARRIVED,    "NOT_END_OF_EVENT_ARRIVED"    },
    { RORC_LAST_BLOCK_OF_EVENT_ARRIVED, "LAST_BLOCK_OF_EVENT_ARRIVED" },

    { RORC_PCI_ERROR,                   "PCI_COMMAND_STATUS_ERROR"    }
  };

char return_text[MAX_RETURN_TEXT];  //maximum return text length: 32
 
int self_lock[MAX_DEVICE][MAX_CHANNEL] = 
 { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
   {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} };

/*****************************************************************************/
void sprom_load_address_mem(volatile char *ptr_to_pci_mem, __u8 address)
{

  while (NVRAM_BUSY_MEM(ptr_to_pci_mem));
  *(ptr_to_pci_mem + 0x3F) = 0x80;           /* open low address latch */
  *(ptr_to_pci_mem + 0x3E) = address;        /* load low address byte */
  *(ptr_to_pci_mem + 0x3F) = 0xA0;           /* open high address latch */
  *(ptr_to_pci_mem + 0x3E) = 0x00;           /* load high address byte */
  *(ptr_to_pci_mem + 0x3F) = 0x00;           /* close the latch */

}

/*******************************************************/
__u8 sprom_readB_mem(volatile char *ptr_to_pci_mem, __u8 address)
{

  sprom_load_address_mem(ptr_to_pci_mem, address);
  *(ptr_to_pci_mem + 0x3F) = 0xE0;              /* start read */
  while (NVRAM_BUSY_MEM(ptr_to_pci_mem));
  return (*(ptr_to_pci_mem + 0x3E));

}

/*****************************************************************************/
int i2c_write_a_byte(rorcHandle_t dev, __u8 address, __u8 data, int timeout)
{
  __u32 rhid;
  int i = 0;
  rhid = I2C_WRITE | (address << 8) | data;
  dRorcWriteReg(dev, RHID, rhid);
  while (I2C_ACTIVE(dev))
  {
    if (timeout && (++i >= timeout))
      return (RORC_TIMEOUT);
  }
  return (RORC_STATUS_OK);
}

/*****************************************************************************/
int i2c_read_a_byte(rorcHandle_t dev, __u8 address, __u8 *data, int timeout)
{
  __u32 rhid;
  int i = 0;
  dRorcWriteReg(dev, RHID, (address << 8));
  while (1)
  {
    rhid = dRorcReadReg(dev, RHID);
    if (rhid & I2C_DATA_VALID)
      break;
    if (timeout && (++i >= timeout))
      return (RORC_TIMEOUT);
  }
  *data = rhid & I2C_DATA;
  return (RORC_STATUS_OK);
}

/*****************************************************************************/
rorcHwSerial_t rorcSerial(rorcHandle_t prorc)
{
  int i, j, rorc_rev, ret, txtlen;
  __u8 address;
  volatile char *ptr_to_pci_mem;
  rorcHwSerial_t hw;
  char txt[20], checktxt[20];

  if (pRorc(prorc))
  {
    ptr_to_pci_mem = (volatile char *)prorc->p2pci;
    address  = 0x80;

    for (i = 0; i < 21; i++, address++)
    {
      hw.data[i] = sprom_readB_mem(ptr_to_pci_mem, address);
      for (j=0; j < MAX_WAIT; j++);
    }
  }
  else
  {
    address  = 0x0;
    for (i = 0; i < DDL_MAX_HW_ID - 1; i++, address++)
    {
      ret = i2c_read_a_byte(prorc, address, &hw.data[i], MAX_WAIT);
      if (ret == RORC_TIMEOUT)
      {
        hw.data[0] = '\0';
        break;
      }
      if (hw.data[i] == '\0')
        break;
    }
  }

  rorc_rev = prorc->rorc_revision;
  txtlen = strlen(rorc_id[rorc_rev].id_text);
  strcpy (checktxt, rorc_id[rorc_rev].id_text);
  upper (checktxt);

  strncpy (txt, hw.data, txtlen);
  txt[txtlen] = '\0';
  upper (txt);

  if (strcmp(txt, checktxt) != 0)
  {
    hw.version = -1;
    hw.subversion = -1;
    hw.serial = -1;
    return(hw);
  }

  hw.version = hw.data[rorc_id[rorc_rev].ver_pos] - '0';
  hw.subversion = hw.data[rorc_id[rorc_rev].ver_pos + 2] - '0';
  hw.serial = atoi(&hw.data[rorc_id[rorc_rev].sn_pos]);

  return(hw);

}

/*****************************************************************************/
void rorcBuildHwSerial(__u8 data[], int rorc_rev, int version_major,
                       int version_minor, __u8 c_pld[], int numb_chan,
                       int serial)
{
  int i;

  data[0] = '\0';
  if (rorc_rev == PRORC)
    return;

  /* fill data with spaces */
  for (i = 0; i < DDL_MAX_HW_ID-1; i++)
    data[i] = ' ';

  /* add hw id text */
  sprintf(data, "%s", rorc_id[rorc_rev].id_text);
  data[strlen(rorc_id[rorc_rev].id_text)] = ' ';

  /* add hw version */
  sprintf(&data[rorc_id[rorc_rev].ver_pos], "%1dv%1d", version_major,
		                                       version_minor);
  data[rorc_id[rorc_rev].ver_pos + 3] = ' ';

  /* add device type */
  if (rorc_id[rorc_rev].ld_pos)
  {
    sprintf(&data[rorc_id[rorc_rev].ld_pos - 4],"LD: %s", c_pld);
    data[rorc_id[rorc_rev].ld_pos + strlen(c_pld)] = ' ';
  }

  /* add number of channels */
  if (rorc_id[rorc_rev].ch_pos)
  {
    sprintf(&data[rorc_id[rorc_rev].ch_pos], "%s", numb_chan == 1 ? "PLG." :
		                                                    "INT.");
    data[rorc_id[rorc_rev].ch_pos + 4] = ' ';
  }

  /* add serial number */
  sprintf(&data[rorc_id[rorc_rev].sn_pos - 5],"S/N: %05d", serial);
  data[rorc_id[rorc_rev].sn_pos + 5] = ' ';

  data[DDL_MAX_HW_ID-1] = '\0';
  trim(data);
}

/*****************************************************************************/
int rorcCheckPCI(int revision, unsigned long pci_com_stat)
{
  unsigned long pci_mask = 0xc0000007;

  if (revision < 4)
  {
    if ((pci_com_stat & pci_mask) != (EXP_PCI_COM_STAT_REV23 & pci_mask))
      return RORC_PCI_ERROR;
  }
  else
  {
    if ((pci_com_stat & pci_mask) != (EXP_PCI_COM_STAT_REV4 & pci_mask))
      return RORC_PCI_ERROR;
  }

  return RORC_STATUS_OK;

}

/*****************************************************************************/
int rorcCheckOpen(int minor, unsigned int channel)
{
  int fd, fd_ch, ret, l_pid;
  unsigned int pid;
  char prorc_name[16], channel_lock[64];

  if (system("/sbin/lsmod | grep rorc_driver > /dev/null"))
    return (-1);

  sprintf(prorc_name,"%s%d", DEV_RORC, minor);
  fd = open(prorc_name, O_RDWR);
  if (fd < 0)
    return fd;

  ret = fd;  

  pid = channel;
  if(ioctl(fd, RORC_R_CHAN, &pid) == -1)
  {
    perror("ioctl");
    ret = -1;
    goto vege;
  }
  if (pid  == 0xffffffff)
  { 
    ret = -1;
    goto vege;
  }
  
  if (self_lock[minor][channel])
  {
    date_log("RORC %d, channel %d is already locked by this process.", 
                   minor, channel);
    ret = -1;
    goto vege;
  }

  sprintf(channel_lock, "%sprorc_%d_channel_%d", LOCK_DIR, minor, channel);
  fd_ch = open (channel_lock, O_RDWR);
  if(fd_ch < 0 )
  {
    perror("open");
    ret = -1;
    goto vege;
  }

  l_pid = lock_test(fd_ch);
  close (fd_ch);
  if (l_pid)
  {
    if (l_pid < 0)
      perror("fcntl");
    else
//      date_log("RORC %d, channel %d is locked by process %d.",
//                     minor, channel, l_pid);
    ret = -1;
    goto vege;
  }

vege:
  close (fd);
  return (ret);

}

/*****************************************************************************/
int rorcFind(int revision, int serial, int *minor)
{
  int fd, i, j;
  int find_revision, find_serial;
  rorc_pci_dev_t prorc;
  rorcHwSerial_t hw;
  FILE *procf = NULL;
  char *values[9];

  procf = fopen("/proc/rorc_map", "r");
  if (procf != NULL)
  {
    while (1)
    {
      i = getToken(values, 9, procf);
      if (i == 0)
        break;
      find_revision = atoi(values[1]);
      find_serial = atoi(values[4]);

      if ( (find_revision == revision) && (find_serial == serial) )
      {
        *minor = atoi(values[0]);
//        date_log ("Device found. Minor = %d.", *minor);
        fclose (procf);
        return (RORC_STATUS_OK);
      }
    }
    fclose (procf);

    date_log ("Device with revision %d and serial %d not found!", 
                                                       revision, serial);
    return (RORC_STATUS_ERROR);
  }
  
  for (i = 0; i < MAX_DEVICE; i++)
  {
    for (j = 0; j < MAX_CHANNEL; j++)
    {
      fd = rorcCheckOpen(i, j);
      if (fd < 0)
        continue;

      rorcMapChannel(&prorc, i, j);
      hw = rorcSerial(&prorc);
      find_revision = prorc.rorc_revision;
      rorcClose(&prorc);

      if ( (find_revision == revision) && (serial == hw.serial) )
      {
        *minor = i;
        return (RORC_STATUS_OK);
      }
    }
  }
  return (RORC_STATUS_ERROR);
}

/*****************************************************************************/
int rorcQuickFind(int *rorc_minor, int *rorc_revision, unsigned long *com_stat,
                  int *pci_speed, 
                  int *rorc_serial, int *rorc_fw_maj,int *rorc_fw_min, 
                  int *max_chan, int *ch_pid0, int *ch_pid1, int max_dev)
{
  int i, found;
  FILE *procf = NULL;
  char *values[9];

  procf = fopen("/proc/rorc_map", "r");
  if (procf == NULL)
    return (-1);

  found = 0;
  while (1)
  {
    i = getToken(values, 9, procf);
    if (i == 0)
      break;
    ch_pid0[found]       = atoi(values[7]);
    ch_pid1[found]       = atoi(values[8]);
    max_chan[found]      = ((ch_pid1[found] == -1) ? 1 : 2);
    rorc_minor[found]    = atoi(values[0]);
    rorc_revision[found] = atoi(values[1]);
    com_stat[found]      = strtoul(values[2], (char**)NULL, 16);
    pci_speed[found]     = atoi(values[3]);
    rorc_serial[found]   = atoi(values[4]);
    rorc_fw_maj[found]   = atoi(values[5]); 
    rorc_fw_min[found]   = atoi(values[6]); 

    found++;
    if (found == max_dev)
      break;
  }

  fclose (procf);
  return (found);
}

/*****************************************************************************/
int rorcFindAll(rorcHwSerial_t *hw, rorcHwSerial_t *diu_hw,
                rorcChannelId_t *channel, int *rorc_revision, 
                                     int *diu_vers, int max_dev)
{
  int i, j, fd, found;
  rorc_pci_dev_t prorc;

  found = 0;
  for (i = 0; i < MAX_DEVICE; i++) 
  {
    for  (j = 0; j < MAX_CHANNEL; j++)
    {
      fd = rorcCheckOpen(i, j);
      if (fd < 0)
        continue;
      channel[found].minor = i;
      channel[found].channel = j;

      rorcOpenChannel(&prorc, i, j);
      hw[found] = rorcSerial(&prorc);
      rorc_revision[found] = prorc.rorc_revision;

      diu_vers[found] = prorc.diu_version;
      if (prorc.diu_version == NEW)
        diu_hw[found] = ddlSerial(&prorc, DIU, 
			         DDL_RESPONSE_TIME * prorc.pci_loop_per_usec);
      else
        diu_hw[found].data[0] = '\0';
      rorcClose(&prorc);
      found++;
      if (found == max_dev)
        break;
    }
    if (found == max_dev)
      break;
  }
  return (found);
}

/*****************************************************************************/
/* Special open routine for traffic monitoring                               */
/*****************************************************************************/
int rorcOpenForMonitor(rorcDescriptor_t *prorc_dev, int minor, int channel)
{
  unsigned int ddl_channel, pid;
  int fd, fd_ch, i;
  unsigned offset, shift, bar;
  volatile unsigned long *ptr_to_pci_mem;
  char prorc_name[16];
  struct timeval tv1, tv2;
  int dsec, dusec;
  double dtime, max_loop;

  if (system("/sbin/lsmod | grep rorc_driver > /dev/null"))
  {
    date_log("RORC card not plugged or driver not loaded");
    return RORC_STATUS_ERROR;
  }

  sprintf(prorc_name,"%s%d", DEV_RORC, minor);
  fd = open ( prorc_name, O_RDWR);
  if(fd < 0 )
  {
    perror("open");
    date_log("Can not open device %s\n", prorc_name);
    return RORC_STATUS_ERROR;
  }

  ddl_channel = channel;
  pid = ddl_channel;
  if(ioctl(fd, RORC_R_CHAN, &pid) == -1)
  {
    perror("ioctl");
    return RORC_STATUS_ERROR;
  }
  if (pid  == 0xffffffff)
  {
    date_log("Can not open device %s, channel %d, channel does not exist", 
                              prorc_name, ddl_channel);
    close (fd);
    return RORC_STATUS_ERROR;
  }

  if (self_lock[minor][ddl_channel])
  {
    date_log("Can not open device %s, channel %d, channel is already open", 
                              prorc_name, ddl_channel);
    close (fd);
    return RORC_STATUS_ERROR;
  }

  self_lock[minor][ddl_channel] = 1;
  
  prorc_dev->fd = fd;
  prorc_dev->fd_ch = fd_ch;
  prorc_dev->minor = minor;
  prorc_dev->ddl_channel = ddl_channel;

  if (ioctl(fd, RORC_SET_CHAN, &ddl_channel) == 1)
  {
    perror("ioctl");
    return RORC_STATUS_ERROR;
  }

  prorc_dev->rorc_revision = 0;
  if(ioctl(fd, RORC_R_REVISION, &prorc_dev->rorc_revision) == -1)
  {
    perror("ioctl");
    return RORC_STATUS_ERROR;
  }
  if (prorc_dev->rorc_revision > RORC_MAX_REVISION)
  {
    date_log("RORC revision number (%d) is not allowed.\n", 
                                           prorc_dev->rorc_revision);
    return RORC_STATUS_ERROR;
  }
  if (prorc_dev->rorc_revision < DRORC2)
  {
    date_log("RORC revision number (%d) is not allowed for traffic monitoring.\n", 
                                           prorc_dev->rorc_revision);
    return RORC_STATUS_ERROR;
  }

  prorc_dev->driver_major = 0;
  if(ioctl(fd, RORC_R_DR_MAJ, &prorc_dev->driver_major) == -1)
  {
    perror("ioctl");
    return RORC_STATUS_ERROR;
  }

  prorc_dev->driver_minor = 0;
  if(ioctl(fd, RORC_R_DR_MIN, &prorc_dev->driver_minor) == -1)
  {
    perror("ioctl");
    return RORC_STATUS_ERROR;
  }

  prorc_dev->driver_release = 0;
  if(ioctl(fd, RORC_R_DR_REL, &prorc_dev->driver_release) == -1)
  {
    perror("ioctl");
    return RORC_STATUS_ERROR;
  }

  if(ioctl(fd, RORC_R_BAR0, &prorc_dev->base_address[0]) == -1)
  {
    perror("ioctl");
    return RORC_STATUS_ERROR;
  }

  if(ioctl(fd, RORC_R_BAR1, &prorc_dev->base_address[1]) == -1)
  {
    perror("ioctl");
    return RORC_STATUS_ERROR;
  }

  if (rorcCheckDriver(prorc_dev, 1, 5, 1, 5))
  {
    if(ioctl(fd, RORC_R_PCI_CFG, &prorc_dev->bus_speed_mode) == -1)
    {
      perror("ioctl");
      return RORC_STATUS_ERROR;
    }
  }

  if (rorcCheckDriver(prorc_dev, 0, 5, 2, 0))
  {
    if(ioctl(fd, RORC_R_SERIAL, &prorc_dev->rorc_serial) == -1)
    {
      perror("ioctl");
      return RORC_STATUS_ERROR;
    }
  }

  if (rorcCheckDriver(prorc_dev, 1, 5, 2, 3))
  {
    if(ioctl(fd, RORC_R_COM_STAT, &prorc_dev->command_status) == -1)
    {
      perror("ioctl");
      return RORC_STATUS_ERROR;
    }
  }

  bar = ddl_channel;
  offset = prorc_dev->base_address[bar] & PAGE_MASK;
  shift = (prorc_dev->base_address[bar] & ~PAGE_MASK) >> 2;
  prorc_dev->shift = shift;

  ptr_to_pci_mem = (unsigned long *) mmap(NULL, PAGE_SIZE, 
                    PROT_READ|PROT_WRITE, MAP_SHARED, fd, offset);

  if((int)ptr_to_pci_mem == -1)
  {
     perror("mmap");
     return RORC_STATUS_ERROR;
  }
  ptr_to_pci_mem += shift;
  prorc_dev->p2pci = ptr_to_pci_mem;

  for (i = 0; i < DRORC_REG_NUM; i++)
    prorc_dev->reg[i] = ptr_to_pci_mem + i;

  prorc_dev->diu_version  = -1;

  /* calibrate memory loop time for time-outs */
  max_loop = 1000000;
  gettimeofday(&tv1, NULL);

  for (i = 0; i < max_loop; i++);

  gettimeofday(&tv2, NULL);
  elapsed(&tv2, &tv1, &dsec, &dusec);
  dtime = (double)dsec * MEGA + (double)dusec;
  prorc_dev->loop_per_usec = (double)max_loop/dtime;
  if (prorc_dev->loop_per_usec < 1)
    prorc_dev->loop_per_usec = 1;

  /* calibrate PCI loop time for time-outs */
  max_loop = 1000000;
  gettimeofday(&tv1, NULL);

  for (i = 0; i < max_loop; i++)
    rorcCheckStatus(prorc_dev);

  gettimeofday(&tv2, NULL);
  elapsed(&tv2, &tv1, &dsec, &dusec);
  dtime = (double)dsec * MEGA + (double)dusec;
  prorc_dev->pci_loop_per_usec = (double)max_loop/dtime;
// printf("pci_loop_per_usec:    %.2f\n", prorc_dev->pci_loop_per_usec);
// printf("PCI wait cycles for %d usecs: %.2f\n", DDL_RESPONSE_TIME,
//                     DDL_RESPONSE_TIME * prorc_dev->pci_loop_per_usec);

  prorc_dev->max_resp_time = DDL_MAX_WAIT_CYCLE/prorc_dev->loop_per_usec;
  if (prorc_dev->max_resp_time > 100000)
    prorc_dev->max_resp_time = (prorc_dev->max_resp_time / 100000) * 100000;
// printf("Max. allowed timeout value: %lld us\n", prorc_dev->max_resp_time);

  return RORC_STATUS_OK;
}

/*****************************************************************************/
int rorcMapChannel(rorcDescriptor_t *prorc_dev, int minor, int channel)
{
  unsigned int ddl_channel, pid;
  int fd, fd_ch, i, ret;
  unsigned offset, shift, bar;
  volatile unsigned long *ptr_to_pci_mem;
  char prorc_name[16], channel_lock[64];
  struct timeval tv1, tv2;
  int dsec, dusec;
  double dtime, max_loop;

  if (system("/sbin/lsmod | grep rorc_driver > /dev/null"))
  {
    date_log("RORC card not plugged or driver not loaded");
    return RORC_STATUS_ERROR;
  }

  sprintf(prorc_name,"%s%d", DEV_RORC, minor);
  fd = open ( prorc_name, O_RDWR);
  if(fd < 0 )
  {
    perror("open");
    date_log("Can not open device %s\n", prorc_name);
    return RORC_STATUS_ERROR;
  }

  ddl_channel = channel;
  pid = ddl_channel;
  if(ioctl(fd, RORC_R_CHAN, &pid) == -1)
  {
    perror("ioctl");
    return RORC_STATUS_ERROR;
  }
  if (pid  == 0xffffffff)
  {
    date_log("Can not open device %s, channel %d, channel does not exist", 
                              prorc_name, ddl_channel);
    close (fd);
    return RORC_STATUS_ERROR;
  }

  if (self_lock[minor][ddl_channel])
  {
    date_log("Can not open device %s, channel %d, channel is already open", 
                              prorc_name, ddl_channel);
    close (fd);
    return RORC_STATUS_ERROR;
  }

  sprintf(channel_lock, "%sprorc_%d_channel_%d", LOCK_DIR, minor, channel);
  fd_ch = open (channel_lock, O_RDWR);
  if(fd_ch < 0 )
  {
    date_log("Can not open file %s\n", channel_lock);
    perror("open");
    close (fd);
    return RORC_STATUS_ERROR;
  }

  ret = lock_test(fd_ch);
  if (ret)
  {
    date_log("Can not open device %s, channel %d, channel is busy", 
                              prorc_name, ddl_channel);
    if (ret < 0)
      date_log("fcntl error");
    close (fd);
    return RORC_STATUS_ERROR;
  }

  if (lock_file(fd_ch) < 0)
  {
    date_log("Can not lock file  %s", channel_lock);
    perror("fcntl");
    close (fd);
    return RORC_STATUS_ERROR;
  }
  self_lock[minor][ddl_channel] = 1;
  
  prorc_dev->fd = fd;
  prorc_dev->fd_ch = fd_ch;
  prorc_dev->minor = minor;
  prorc_dev->ddl_channel = ddl_channel;

  if (ioctl(fd, RORC_SET_CHAN, &ddl_channel) == 1)
  {
    perror("ioctl");
    return RORC_STATUS_ERROR;
  }

  prorc_dev->rorc_revision = 0;
  if(ioctl(fd, RORC_R_REVISION, &prorc_dev->rorc_revision) == -1)
  {
    perror("ioctl");
    return RORC_STATUS_ERROR;
  }
  if (prorc_dev->rorc_revision > RORC_MAX_REVISION)
    date_log("RORC revision number (%d) is not allowed.\n", 
                                           prorc_dev->rorc_revision);

  prorc_dev->driver_major = 0;
  if(ioctl(fd, RORC_R_DR_MAJ, &prorc_dev->driver_major) == -1)
  {
    perror("ioctl");
    return RORC_STATUS_ERROR;
  }

  prorc_dev->driver_minor = 0;
  if(ioctl(fd, RORC_R_DR_MIN, &prorc_dev->driver_minor) == -1)
  {
    perror("ioctl");
    return RORC_STATUS_ERROR;
  }

  prorc_dev->driver_release = 0;
  if(ioctl(fd, RORC_R_DR_REL, &prorc_dev->driver_release) == -1)
  {
    perror("ioctl");
    return RORC_STATUS_ERROR;
  }

  if(ioctl(fd, RORC_R_BAR0, &prorc_dev->base_address[0]) == -1)
  {
    perror("ioctl");
    return RORC_STATUS_ERROR;
  }

  if(ioctl(fd, RORC_R_BAR1, &prorc_dev->base_address[1]) == -1)
  {
    perror("ioctl");
    return RORC_STATUS_ERROR;
  }

  if (rorcCheckDriver(prorc_dev, 1, 5, 1, 5))
  {
    if(ioctl(fd, RORC_R_PCI_CFG, &prorc_dev->bus_speed_mode) == -1)
    {
      perror("ioctl");
      return RORC_STATUS_ERROR;
    }
  }

  if (rorcCheckDriver(prorc_dev, 0, 5, 2, 0))
  {
    if(ioctl(fd, RORC_R_SERIAL, &prorc_dev->rorc_serial) == -1)
    {
      perror("ioctl");
      return RORC_STATUS_ERROR;
    }
  }

  if (rorcCheckDriver(prorc_dev, 1, 5, 2, 3))
  {
    if(ioctl(fd, RORC_R_COM_STAT, &prorc_dev->command_status) == -1)
    {
      perror("ioctl");
      return RORC_STATUS_ERROR;
    }
  }

  bar = ddl_channel;
  offset = prorc_dev->base_address[bar] & PAGE_MASK;
  shift = (prorc_dev->base_address[bar] & ~PAGE_MASK) >> 2;
  prorc_dev->shift = shift;

  ptr_to_pci_mem = (unsigned long *) mmap(NULL, PAGE_SIZE, 
                    PROT_READ|PROT_WRITE, MAP_SHARED, fd, offset);

  if((int)ptr_to_pci_mem == -1)
  {
     perror("mmap");
     return RORC_STATUS_ERROR;
  }
  ptr_to_pci_mem += shift;
  prorc_dev->p2pci = ptr_to_pci_mem;

  if (pRorc(prorc_dev))
  {
    for (i = 0; i < 4; i++)
      prorc_dev->omb[i+1] = ptr_to_pci_mem + i;

    for (i = 4; i < 8; i++)
      prorc_dev->imb[i-3] = ptr_to_pci_mem + i;

    prorc_dev->mwar   = ptr_to_pci_mem + 9;
    prorc_dev->mwtc   = ptr_to_pci_mem + 10;
    prorc_dev->mrar   = ptr_to_pci_mem + 11;
    prorc_dev->mrtc   = ptr_to_pci_mem + 12;
    prorc_dev->mbef   = ptr_to_pci_mem + 13;
    prorc_dev->intcsr = ptr_to_pci_mem + 14;
    prorc_dev->mcsr   = ptr_to_pci_mem + 15;
  }
  else
  {
    for (i = 0; i < DRORC_REG_NUM; i++)
      prorc_dev->reg[i] = ptr_to_pci_mem + i;
  }

  prorc_dev->diu_version  = -1;

  /* calibrate memory loop time for time-outs */
  max_loop = 1000000;
  gettimeofday(&tv1, NULL);

  for (i = 0; i < max_loop; i++);

  gettimeofday(&tv2, NULL);
  elapsed(&tv2, &tv1, &dsec, &dusec);
  dtime = (double)dsec * MEGA + (double)dusec;
  prorc_dev->loop_per_usec = (double)max_loop/dtime;
  if (prorc_dev->loop_per_usec < 1)
    prorc_dev->loop_per_usec = 1;
// printf("memory loop_per_usec: %lld\n", prorc_dev->loop_per_usec);

  /* calibrate PCI loop time for time-outs */
  max_loop = 1000;
  gettimeofday(&tv1, NULL);

  for (i = 0; i < max_loop; i++)
    rorcCheckStatus(prorc_dev);

  gettimeofday(&tv2, NULL);
  elapsed(&tv2, &tv1, &dsec, &dusec);
  dtime = (double)dsec * MEGA + (double)dusec;
  prorc_dev->pci_loop_per_usec = (double)max_loop/dtime;
// printf("pci_loop_per_usec:    %.2f\n", prorc_dev->pci_loop_per_usec);
// printf("PCI wait cycles for %d usecs: %.2f\n", DDL_RESPONSE_TIME, 
//                     DDL_RESPONSE_TIME * prorc_dev->pci_loop_per_usec);

  prorc_dev->max_resp_time = DDL_MAX_WAIT_CYCLE/prorc_dev->loop_per_usec;
  if (prorc_dev->max_resp_time > 100000)
    prorc_dev->max_resp_time = (prorc_dev->max_resp_time / 100000) * 100000;
// printf("Max. allowed timeout value: %lld us\n", prorc_dev->max_resp_time);

  if (prorc_dev->rorc_revision > RORC_MAX_REVISION)
  {
    fprintf(stderr, "Incorrect RORC revision number: %d\n", 
                                               prorc_dev->rorc_revision);
    return RORC_STATUS_ERROR;
  }

  if (rorcCheckDriver(prorc_dev, 1, 5, 2, 3))
  {
    if (rorcCheckPCI(prorc_dev->rorc_revision, prorc_dev->command_status))
    {
      fprintf(stderr, "Incorrect PCI command/status word: 0x%08lx\n", 
                                               prorc_dev->command_status);
      return RORC_PCI_ERROR;
    }
  }

  return RORC_STATUS_OK;
}

/*****************************************************************************/
int rorcMap(rorcDescriptor_t *prorc_dev, int minor)
{
  return (rorcMapChannel(prorc_dev, minor, 0));
}

/*****************************************************************************/
int rorcOpenChannel(rorcDescriptor_t *prorc_dev, int minor, int channel)
{ 
  int ret;

  ret = rorcMapChannel(prorc_dev, minor, channel);
  if ((ret == RORC_STATUS_OK) || (ret == RORC_PCI_ERROR))
    ddlFindDiuVersion(prorc_dev);
 
  return ret;
}

/*****************************************************************************/
int rorcOpen(rorcDescriptor_t *prorc_dev, int minor)
{ 
  return(rorcOpenChannel(prorc_dev, minor, 0));
}

/*****************************************************************************/
int rorcClose(rorcDescriptor_t *prorc)
{

  if (ioctl(prorc->fd, RORC_RES_CHAN, &(prorc->ddl_channel)) == 1)
  {
    date_log("Can not release channel");
    perror("ioctl");
    return RORC_STATUS_ERROR;
  }

  if (unlock_file(prorc->fd_ch) < 0)
  {
    date_log("Can not unlock channel lock file");
    perror("fcntl");
    return RORC_STATUS_ERROR;
  }
  close(prorc->fd_ch);
  self_lock[prorc->minor][prorc->ddl_channel] = 0;

  if(munmap ((caddr_t) (prorc->p2pci - prorc->shift), PAGE_SIZE))
  {
    perror("munmap");
    date_log("%08x\n", (unsigned int)(prorc->p2pci - prorc->shift));
    return RORC_STATUS_ERROR ;
  }

  close (prorc->fd);
  return RORC_STATUS_OK;
}

/*****************************************************************************/
int physmemOpen (int *fd, 
                 volatile unsigned long **user_addr,
                 unsigned long *phys_addr,
                 unsigned long *size_physmem)
{

#define DEV_PHYSMEM "/dev/physmem0"

  int physmem_fd;                   // file desciptor of physmem
  void *addr_user_physmem;          // pointer in user space to physmem
  unsigned long phys_addr_physmem;
  unsigned long phys_size_physmem;


  if ((physmem_fd = open(DEV_PHYSMEM,O_RDWR)) == -1)
  {
    printf("Cannot open device %s \n", DEV_PHYSMEM);
    return (EXIT_FAILURE);
  }

  /* get physical address of physmem */
  if (ioctl (physmem_fd, PHYSMEM_GETADDR, &phys_addr_physmem) < 0)
  {
    printf("Cannot ioctl device %s to get address\n", DEV_PHYSMEM);
    close(physmem_fd);
    return (EXIT_FAILURE);
  }

  /* get size of the physmem */
  if (ioctl (physmem_fd, PHYSMEM_GETSIZE, &phys_size_physmem) < 0)
  {
    printf("Cannot ioctl device %s to get size \n", DEV_PHYSMEM);
    close(physmem_fd);
    return (EXIT_FAILURE);
  }

  /* map the whole physmem to user space */
  if ((addr_user_physmem = mmap(0,phys_size_physmem,PROT_READ|PROT_WRITE,
                                MAP_SHARED,physmem_fd,0)) == MAP_FAILED)
  {
    perror("mmap failed");
    close(physmem_fd);
    return (EXIT_FAILURE);
  }

  *fd = physmem_fd;
  *user_addr = addr_user_physmem;
  *phys_addr = phys_addr_physmem;
  *size_physmem = phys_size_physmem;

  return 0;
}


/*****************************************************************************/
int physmemClose (int physmem_fd, 
                  unsigned long *addr_user_physmem, 
                  unsigned long phys_size_physmem)
{

  /* remove the mapping to physmem */
  if (munmap(addr_user_physmem, phys_size_physmem) < 0) {
    perror("munmap failed");
    close(physmem_fd);
    return (EXIT_FAILURE);
  }

  /* close the device for physmem */
  if (close (physmem_fd) == -1) {
    printf("Cannot close device %s \n", DEV_PHYSMEM);
    return (EXIT_FAILURE);
  }

  return 0;
}

/*****************************************************************************/
int rorcCheckVersion(rorc_pci_dev_t *dev)
{
  unsigned int pid;
  unsigned long fw;
  __u32 command; 
  __u32 mb1, mb2, mb3; 
  int n, fw_version, version_min, version_max;
  
  if (pRorc(dev))
  {
    /* empty incoming mb1, 2, and 3 if necessary */
    n = pRorcEmptyMb(dev, 1, 0);
    n = pRorcEmptyMb(dev, 2, 0);
    n = pRorcEmptyMb(dev, 3, 0);

    /* send "Get pRORC Id command */
    command = PRORC_CMD_GET_ID;
    pRorcWriteMb (dev, 1, command);

    /* read inMB 1, 2 and 3 */
    pRorcWaitMbNotEmpty(dev, PRORC_NE_IMB1 | PRORC_NE_IMB2 | PRORC_NE_IMB3);
    mb2 = pRorcReadMb (dev, 2);
    mb3 = pRorcReadMb (dev, 3);  //fw
    mb1 = pRorcReadMb (dev, 1);

    fw_version = rorcFWVersMajor(mb3) * 100 + rorcFWVersMinor(mb3);
    version_min = PRORC_VERSION_MIN;
    version_max = PRORC_VERSION_MAX;
  }
  else
  {
    pid = 1;
    if(ioctl(dev->fd, RORC_R_CHAN, &pid) == -1)
    {
      perror("ioctl");
      return RORC_STATUS_ERROR;
    } 
    /* read fw id */
    fw = dRorcReadReg (dev, RFID);    
    fw_version = rorcFWVersMajor(fw) * 100 + rorcFWVersMinor(fw);
    
    if (pid  == 0xffffffff)
    { 
    version_min = DRORC_CH1_VERSION_MIN;
    version_max = DRORC_CH1_VERSION_MAX;
    }
    else
    {
    version_min = DRORC_CH2_VERSION_MIN;
    version_max = DRORC_CH2_VERSION_MAX;
    }
  }

  if ( (version_min && (fw_version < version_min)) ||
       (version_max && (fw_version > version_max)) )
    return RORC_STATUS_ERROR;

  return(RORC_STATUS_OK);
}


/*****************************************************************************/
void rorcReset (rorc_pci_dev_t *dev, int option)
{
  int ret, prorc_cmd;
  long long int longret, timeout;

  timeout = DDL_RESPONSE_TIME * dev->pci_loop_per_usec;
  
  if (pRorc(dev))
  {
    if (option & RORC_RESET_DIU)
    {
      pRorcInitCmdProc (dev);
      pRorcWriteMb(dev, 1, PRORC_CMD_RESET_DIU); 
      pRorcWaitCmdProc (dev);
    }
    if (option & RORC_RESET_SIU)
    {
      pRorcInitCmdProc (dev);
      pRorcWriteMb(dev, 1, PRORC_CMD_RESET_SIU); 
      pRorcWaitCmdProc (dev);
      pRorcEmptyMb(dev, 1, 0);  // clear DIU reply
    }
    if (option & RORC_RESET_FF)
    {
      pRorcInitCmdProc (dev);
      pRorcWriteMb(dev, 1, PRORC_CMD_CLEAR_FF); 
      pRorcWaitCmdProc (dev);
    }
    if (option & RORC_RESET_FIFOS)
    {
      pRorcInitCmdProc (dev);
      pRorcWriteMb(dev, 1, PRORC_CMD_CLEAR_FIFOS); 
      pRorcWaitCmdProc (dev);
    }
    if (option & RORC_RESET_ERROR)
    {
      pRorcInitCmdProc (dev);
      pRorcWriteMb(dev, 1, PRORC_CMD_CLEAR_ERROR); 
      pRorcWaitCmdProc (dev);
    }
    if (option & RORC_RESET_COUNTERS)
    {
      pRorcInitCmdProc (dev);
      pRorcWriteMb(dev, 1, PRORC_CMD_CLEAR_COUNTERS); 
      pRorcWaitCmdProc (dev);
    }
    if (!(option & RORC_RESET_ALL) || (option & RORC_RESET_RORC))
    { 
      ioctl(dev->fd, PRORC_RESET, &ret);  //add-on-reset

      /* initialize the DMA */
      ret = 1;
      ioctl(dev->fd, PRORC_SET_RVW, &ret);   /* read versus write priority = 1 */
      ioctl(dev->fd, PRORC_SET_WVR, &ret);   /* write versus read priority = 1 */
      ret = 0;
      ioctl(dev->fd, PRORC_SET_WMS, &ret);   /* write management scheme = 0 */
    }
  }
  else
  {
    prorc_cmd = 0;
    if (option & RORC_RESET_DIU)
      prorc_cmd |= DRORC_CMD_RESET_DIU;
    if (option & RORC_RESET_FF)
      prorc_cmd |= DRORC_CMD_CLEAR_RXFF | DRORC_CMD_CLEAR_TXFF;
    if (option & RORC_RESET_FIFOS)
      prorc_cmd |= DRORC_CMD_CLEAR_FIFOS;
    if (option & RORC_RESET_ERROR)
      prorc_cmd |= DRORC_CMD_CLEAR_ERROR;
    if (option & RORC_RESET_COUNTERS)
      prorc_cmd |= DRORC_CMD_CLEAR_COUNTERS;
    if (prorc_cmd)   // any reset
    {
      dRorcWriteReg(dev, C_CSR, (__u32) prorc_cmd); 
    }
    if (option & RORC_RESET_SIU)
    {
      rorcPutCommandRegister(dev, PRORC_CMD_RESET_SIU);
      longret = ddlWaitStatus(dev, timeout);
      if (longret < timeout)
        ddlReadStatus(dev);
    }
    if (!option || (option & RORC_RESET_RORC))
    {
      dRorcWriteReg(dev, RCSR, DRORC_CMD_RESET_CHAN);  //channel reset
    }
  }
}


/*****************************************************************************/
int rorcEmptyDataFifos(rorc_pci_dev_t *dev, int empty_time)

/* try to empty D-RORC's data FIFOs                            
               empty_time:  time-out value in usecs
 * Returns:    RORC_STATUS_OK or RORC_TIMEOUT 
 */

{
  struct timeval start_tv, now_tv;
  int dsec, dusec;
  double dtime;
  rorcStatus_t status;

  gettimeofday(&start_tv, NULL);
  dtime = 0;
  while (dtime < empty_time)
  {
    if (pRorc(dev))
    {
      rorcReadRorcStatus(dev, &status);
      if (!(status.ccsr & PRORC_STAT_DC_NOT_EMPTY))
        return (RORC_STATUS_OK);

      pRorcInitCmdProc (dev);
      pRorcWriteMb(dev, 1, PRORC_CMD_CLEAR_FIFOS); 
      pRorcWaitCmdProc (dev);
    }
    else
    {
      if (!dRorcCheckRxData(dev))
        return (RORC_STATUS_OK);

      dRorcWriteReg(dev, C_CSR, (__u32) DRORC_CMD_CLEAR_FIFOS);
    } 
    gettimeofday(&now_tv, NULL);
    elapsed(&now_tv, &start_tv, &dsec, &dusec);
    dtime = (double)dsec * MEGA + (double)dusec;
  }

  if (dRorcCheckRxData(dev))        // last check
    return (RORC_TIMEOUT);
  else
    return (RORC_STATUS_OK);

}

/*****************************************************************************/
int dRorcWaitRxStatusNotEmpty(rorc_pci_dev_t *dev)
{
  int i;

  i = 0;
  do
  {
    i++;
  } while (!rorcCheckStatus(dev));

  return i;

}

/*****************************************************************************/
int dRorcCheckRxFreeFifo(rorc_pci_dev_t *dev)  //RX Address FIFO
{
   __u32 st;

  st = dRorcReadReg(dev, C_CSR);
  if (st & DRORC_STAT_RXAFF_FULL)  
    return (RORC_FF_FULL);
  else if (st & DRORC_STAT_RXAFF_EMPTY)
    return (RORC_FF_EMPTY);
  else
    return (RORC_STATUS_OK);  //Not Empty
  
}

/*****************************************************************************/
int dRorcCheckTxFreeFifo(rorc_pci_dev_t *dev)  //TX Address FIFO
{
  __u32 st;

  st = dRorcReadReg(dev, C_CSR);
  if (st & DRORC_STAT_TXAFF_FULL)  
    return (RORC_FF_FULL);
  else if (st & DRORC_STAT_TXAFF_EMPTY)
    return (RORC_FF_EMPTY);
  else
    return (RORC_STATUS_OK);  //Not Empty
}

/*****************************************************************************/
int rorcCheckTxNotFinished(rorc_pci_dev_t *dev)
{
  rorcStatus_t status;

  if (pRorc(dev))
  {
    rorcReadRorcStatus(dev, &status);
    return ((status.ccsr & PRORC_STAT_FED_NOT_EMPTY) ||
            (status.ccsr & PRORC_STAT_DTSTW_NOT_EMPTY)); 
  }
  else
    return (dRorcCheckTxData(dev)); 
}

/*****************************************************************************/
int rorcCheckFreeFifo(rorc_pci_dev_t *dev)
{
  rorcStatus_t status;
  __u32 st;

  if (pRorc(dev))
  {
    rorcReadRorcStatus(dev, &status);
    st = status.ccsr;                    // pRORC: .ocsr
    if (st & PRORC_STAT_FF_FULL)
      return (RORC_FF_FULL);
    else if (st & PRORC_STAT_FF_EMPTY)
      return (RORC_FF_EMPTY);
    else
      return (st & PRORC_STAT_FF_USEDW);
  }
  else
    return dRorcCheckRxFreeFifo(dev);
  
}

/*****************************************************************************/
int rorcPopFreeFifo (rorc_pci_dev_t *dev,
		      int             tx,
                      __u32           *blockAddress,
                      int             *blockLength,
                      int             *readyFifoIndex)

{   
  int status;
    __u32 ret;

  if (pRorc(dev))
  {
    ioctl(dev->fd, PRORC_MB_RESET, &ret);
    ret = *dev->imb[4];
    if (ret & PRORC_BIT_FF_EMPTY)
      return (RORC_FF_EMPTY);

    *dev->omb[1] = PRORC_CMD_POP_FF;
    pRorcWaitMbNotEmpty(dev, PRORC_NE_INMB);

    *blockAddress   = *dev->imb[2];
    *blockLength    = *dev->imb[3];
    ret             = *dev->imb[4];
    *readyFifoIndex = ret & 0x3ff;
    ret             = *dev->imb[1];
    status          = RORC_STATUS_OK;
  }
  else
  {
    if (tx)
       status = dRorcCheckTxFreeFifo(dev);
    else
       status = dRorcCheckRxFreeFifo(dev);
    if (status == RORC_FF_EMPTY)
      return (status);

    if (tx)
    {    
      *blockAddress   = dRorcReadReg(dev, C_TAFH);
      ret             = dRorcReadReg(dev, C_TAFL);
    }
    else
    {
      *blockAddress   = dRorcReadReg(dev, C_RAFH);
      ret             = dRorcReadReg(dev, C_RAFL);
    }
    *blockLength    = ret >> 8;
    *readyFifoIndex = ret & 0xff;
  }

  return (status);
}

/*****************************************************************************/
/*
 *  * wait for ending the command processing
 *
 */
int pRorcWaitCmdProc(rorc_pci_dev_t *dev)
{
  int i;
  __u32 ret, bit1, bit2;

  i = 0;
  while(1)
  {
    i++;
    ret = pRorcReadMb(dev,4);
    bit1 = ret & PRORC_BIT_CMD_RDY1;
    bit2 = (ret & PRORC_BIT_CMD_RDY2) << 1;
    if (bit1 != bit2)
      continue;

   if (bit1 != prorc_cmd_rdy)
     break;
  }
  return (i);
}

/*****************************************************************************/
int pRorcWaitMbNotEmpty(rorc_pci_dev_t *dev, __u32 mbMask)
{
  int i;
  __u32 ret;

  i = 0;
  do
  {
    i++;
    ret = *dev->mbef;
  } while ((ret & mbMask) != mbMask);

  return i;

}

/***************************************************************/
int pRorcEmptyMb(rorc_pci_dev_t *prorc, int mb_number, short print)

/* try to read out all data from the pRORC Incoming Mailbox 
 * Parameters: prorc     pointer to a pRORC device,
               mb_number mailbox number (1 or 4)
               print     if != 0 prints what the mailbox contains
 * Returns:    number of words found in the mailbox 
 */

{
  int n = 0;
  __u32 status, mb_mask;
  
  switch (mb_number)
  { 
    case 2:  mb_mask = PRORC_NE_IMB2; break;
    case 3:  mb_mask = PRORC_NE_IMB3; break;
    case 4:  mb_mask = PRORC_NE_IMB4; break;
    default: mb_mask = PRORC_NE_IMB1; break;
  }

  /* read the mailbox while not empty */
  while (pRorcCheckMb(prorc, mb_mask))
  {
    status = pRorcReadMb(prorc, mb_number) ;
    n++;
    if (print)
      printf("%5d:\t0x%08x\n", n, status);
  }
  if (print)
  {
    if (n)
      printf("The mailbox %d contained %d status word(s).\n\n", mb_number, n);
    else
      printf("The mailbox %d is empty.\n\n", mb_number);
  }
  return(n);
}

/***************************************************************/
int dRorcEmptyRxStatus(rorc_pci_dev_t *prorc, short print)

/* try to read out all data from the D-RORC's DDL Status FIFO 
               print     if != 0 prints what the FIFO contains
 * Returns:    number of words found in the mailbox 
 */

{
  int n = 0;
  __u32 status;

  /* read the FIFO while not empty */
  while (rorcCheckStatus(prorc))
  {
    status = dRorcReadReg(prorc, C_DSR);
    n++;
    if (print)
      printf("%5d:\t0x%08x\n", n, status);
  }
  if (print)
  {
    if (n)
      printf("The DDL status FIFO contained %d status word(s).\n\n", n);
    else
      printf("The DDL status FIFO is empty.\n\n");
  }
  return(n);
}

/*****************************************************************************/
int rorcReadRorcStatus(rorcHandle_t dev, rorcStatus_t *status)
{
  __u32 mb1;

  if (pRorc(dev))
  {
    /* send "Get pRORC Status" command */
    pRorcInitCmdProc (dev);
    pRorcWriteMb(dev, 1, PRORC_CMD_GET_STAT);
    pRorcWaitCmdProc (dev);

    /* read inMB 1, 2, 3 (4 is already read by pRorcWaitCmdProc) */
    pRorcWaitMbNotEmpty(dev, PRORC_NE_IMB1
                          |  PRORC_NE_IMB2
                          |  PRORC_NE_IMB3
//                        |  PRORC_NE_IMB4
                       );
    status->ccsr = pRorcReadMb (dev, 2);                 //.ocsr
    status->cerr = pRorcReadMb (dev, 3);                 //.rser
    status->cdgs = (pRorcReadMb (dev, 4) & 0xFFFFFF);    //.ecnt
    mb1 = pRorcReadMb (dev, 1);
  }
  else
  {
    status->ccsr = dRorcReadReg (dev, C_CSR);
    status->cerr = dRorcReadReg (dev, C_ERR);
    status->cdgs = dRorcReadReg (dev, C_DGS);
  }

  return (RORC_STATUS_OK);
}


/*****************************************************************************/
char *rorcPrintStatus(rorcHandle_t dev, int prefix)
{
  rorcStatus_t status;
  int ff, ff1, ff2;
  char pref[14], suff[2];

  pref[0] = '\0';
  suff[0] = '\n';
  suff[1] = '\0';
  if (prefix)
    suff[0] = '\0';

  rorcReadRorcStatus(dev, &status);

  statInfo[0] = '\0';
  if (!prefix)
    stat_log("\nRORC status:\n");

  if (pRorc(dev))
    stat_log("%sOCSR: 0x%08x%s", pref, status.ccsr, suff);
  else
    stat_log("%sC_CSR: 0x%08x%s", pref, status.ccsr, suff);
 
  if (prefix)
    strcpy(pref, ",");

  rorcInterpretStatus(dev, status.ccsr, pref, suff);

  if (pRorc(dev))
  {
    stat_log("%sRSER: 0x%08x%s", pref, status.cerr, suff);
    rorcInterpretErrors(status.cerr, pref, suff);
    rorcReset (dev, PRORC_CMD_CLEAR_ERROR);
  }
  else
  {
    stat_log("%sC_ERR: 0x%08x%s", pref, status.cerr, suff);
    rorcReset (dev, DRORC_CMD_CLEAR_ERROR);
  }

  if (pRorc(dev))
    stat_log("%sECNT: 0x%06x (%d)%s", pref, status.cdgs, status.cdgs, suff);
  else
    stat_log("%sC_DGS: 0x%06x (%d)%s", pref, status.cdgs, status.cdgs, suff);

  if (pRorc(dev))
  {
    ff = rorcCheckFreeFifo(dev);
    stat_log("%sFree FIFO status: ", pref);
    if (ff == RORC_FF_FULL)
      stat_log("full%s", suff);
    else if (ff == RORC_FF_EMPTY)
      stat_log("empty%s", suff);
    else
    {
      ff2 = (ff + 1) * 8;
      ff1 = ff2 - 7;
      stat_log("%snumber of entries loaded is between %d and %d%s", 
                                                      pref, ff1, ff2, suff);
    }
  }

  if (!prefix)
    stat_log("\n");

  return (statInfo);
}


/*****************************************************************************/
int rorcCheckDriver(rorcHandle_t dev, int crevision, 
                                         int cmajor, int cminor, int crelease)
{
  return ((dev->rorc_revision > crevision) &&
         ( (dev->driver_major > cmajor) ||
          ((dev->driver_major == cmajor) && (dev->driver_minor > cminor)) ||
          ((dev->driver_major == cmajor) && (dev->driver_minor == cminor) && 
                                        (dev->driver_release >= crelease)) ));
}


/*****************************************************************************/
char* rorcInterpretBusMode(int bus_speed_mode)
{
  int bus_mode;

  bus_mode = ((bus_speed_mode >> 28) & 0x4) >> 2;
  return (bus_mode_txt[bus_mode]);
}


/*****************************************************************************/
char* rorcInterpretBusSpeed(int bus_speed_mode)
{
  int bus_speed;

  bus_speed = (bus_speed_mode >> 28) & 0x3;
  return (bus_speed_txt[bus_speed]);
}


/*****************************************************************************/
char* rorcInterpretReturnCode(int return_code)
{
  int i;

  for (i = 0; i < MAX_RETURN_CODE; i++)
  {
    if (return_code == rorc_return[i].code)
    {
      sprintf(return_text, "%s (%d)", rorc_return[i].text, return_code);
      return (return_text);
    }
  }

  sprintf(return_text, "%d", return_code);
  return(return_text);
}


/*****************************************************************************/
void rorcInterpretStatus(rorcHandle_t dev, __u32 status, char* pref, char* suff)
{
  int i, stat_bit, n_text;
  char full_text[1024];
  char *status_text[32];

  if (status == 0)
    return;

  if (pRorc(dev))
  {
    if (status & PRORC_STAT_ANY)
    {
      if (strlen(pref) == 0)
        stat_log("%sThe following OCSR bits are set:%s", pref, suff);

      strcpy(full_text, PRORC_STAT_TEXT);
      n_text = browse(full_text, status_text, 32, ",");

      for (i = 0; i < 31; i++)
      {
        stat_bit = status & 0x1;
        status = status >> 1;
	if (stat_bit)
	{
          if (status_text[i][0] != '0')
            stat_log("%s %s%s", pref, status_text[i], suff);
          else
            stat_log("%s %s (%d)%s", pref, PRORC_STAT_DEFAULT_TEXT, i, suff);
	}
      }
    }
  }
  else
  {
    if (status & DRORC_STAT_ANY)
    {
      if (strlen(pref) == 0)
        stat_log("%sThe following status bits are set:%s", pref, suff);

      strcpy(full_text, DRORC_STAT_TEXT);
      n_text = browse(full_text, status_text, 32, ",");

      for (i = 0; i < 31; i++)
      {
        stat_bit = status & 0x1;
        status = status >> 1;
	if (stat_bit)
	{
          if (status_text[i][0] != '0')
            stat_log("%s %s%s", pref, status_text[i], suff);
          else
            stat_log("%s %s (%d)%s", pref, DRORC_STAT_DEFAULT_TEXT, i, suff);
	}
      }
    }
  }
    
  if (strlen(pref) == 0)
    stat_log("\n");

}

/*****************************************************************************/
void rorcInterpretErrors(__u32 errors, char* pref, char* suff)
{

  if (errors == 0)
    return;

  if (strlen(pref) == 0)
    stat_log("%sThe following RSER bits are set:%s", pref, suff);

  if (errors & PRORC_ERROR)
    stat_log ("%sError attention bit%s", pref, suff);
  if (errors & PRORC_ERR_INV_COMM)
    stat_log ("%sInvalid pRORC command%s", pref, suff);
  if (errors & PRORC_ERR_MISS_PARM)
    stat_log ("%sLess or more command parameters%s", pref, suff);
  if (errors & PRORC_ERR_FF_OVWR)
    stat_log ("%sFree FIFO overwrite attempt%s", pref, suff);
  if (errors & PRORC_ERR_DTSTW_OVWR)
    stat_log ("%sDTSTW register overwrite attempt%s", pref, suff);
  if (errors & PRORC_ERR_DIU_OVWR)
    stat_log ("%sDIU Command Register overwrite (warning)%s", pref, suff);
  if (errors & PRORC_ERR_LINK_DOWN)
    stat_log ("%sLink went down during Data Collection%s", pref, suff); 
  if (errors & PRORC_ERR_STAT_FF_FULL)
    stat_log ("%sStatus FIFO is full%s", pref, suff);
  if (errors & PRORC_ERR_FED_FULL)
    stat_log ("%sFED FIFO is full%s", pref, suff);
  if (errors & PRORC_ERR_DC_FULL)
    stat_log ("%sDC FIFO full%s", pref, suff);
  if (errors & PRORC_ERR_JTAG_FULL)
    stat_log ("%sJTAG FIFO is full%s", pref, suff);

  if (strlen(pref) == 0)
    stat_log("\n");

}
/*****************************************************************************/
__u32 rorcReadFw(rorcHandle_t dev)
{
  __u32 mb1, hw, fw;

  if (pRorc(dev))
  {
    /* send "Get pRORC Id command */
    pRorcInitCmdProc (dev);
    pRorcWriteMb(dev, 1, PRORC_CMD_GET_ID);
    pRorcWaitCmdProc (dev);

    /* read inMB 1, 2 and 3 */
    pRorcWaitMbNotEmpty(dev, PRORC_NE_IMB1 |
                             PRORC_NE_IMB2 |
                             PRORC_NE_IMB3);
    hw = pRorcReadMb (dev, 2);
    fw = pRorcReadMb (dev, 3);
    mb1 = pRorcReadMb (dev, 1);
  }
  else
  {
    fw = dRorcReadReg (dev, RFID);
  }

  return (fw);
}


/*****************************************************************************/
void rorcInterpretVersion(__u32 x)

/*
 * Interpret hw/fw version
 */

{
  int major, minor, month, day, year;

  char* monthName[] = {"unknown month", "January", "February", "March",
                       "April", "May", "June", "July", "August",
                       "September", "October", "November", "December",
                       "unknown month", "unknown month", "unknown month"};

  major   = rorcFWVersMajor(x);
  minor   = rorcFWVersMinor(x);
  year    = (x >>  9) & 0xf;
  month   = (x >>  5) & 0xf;
  day     =  x        & 0x1f;

  printf(" Version: %d.%d\n Release date : %s %d 20%02d\n",
           major, minor, monthName[month], day, year);

}

/*****************************************************************************/
void rorcInterpretSerial (rorcHwSerial_t hw)

/*
 * Interpret version and serial number
 */

{
  printf("Hardware id word of the RORC: %s, i.e.\n", hw.data);

  if (hw.version == -1)
  {
    printf(" No version and serial number found.\n");
    return;
  }

  printf(" Version: %d.%d, S/N: %05d.\n", hw.version, hw.subversion, hw.serial);

}

/*****************************************************************************/
void rorcInterpretFw(__u32 fw)

/*
 * Interpret fw version
 */

{
  printf("Firmware identity word of the RORC: 0x%08x, i.e.\n", fw);
  rorcInterpretVersion(fw);
  printf(" Free FIFO size: %d entries\n", rorcFFSize(fw));
}

/*****************************************************************************/
int rorcMeasurePciFreq(rorcHandle_t dev, double *bus_speed, double *time_tick)
{
  int fw_major, fw_minor;
  unsigned long fw;
  rorcFifoCounter_t counter;

  if (dev->rorc_revision < DRORC2)
    return (RORC_CMD_NOT_ALLOWED);

  /* read fw id */
  fw = rorcReadFw(dev);
  fw_major = rorcFWVersMajor(fw);
  fw_minor = rorcFWVersMinor(fw);
  if ((fw_major < 2) || ((fw_major == 2) && (fw_minor < 11)))
    return (RORC_CMD_NOT_ALLOWED);

  /* measure PCI speed */
  dRorcWriteReg(dev, C_TMCS, 0);  // start time counter
  sleep(1);
  dRorcWriteReg(dev, C_TMCS, 0);  // read and restart time counter
  counter.tmcs  = dRorcReadReg(dev, C_TMCS);
  *bus_speed = (double)counter.tmcs / MEGA;
  if      (*bus_speed > (double)150.0)
    *bus_speed = 166.667;
  else if (*bus_speed > (double)116.667)
    *bus_speed = 133.333;
  else if (*bus_speed > (double)83.333)  
    *bus_speed = 100.0;
  else if (*bus_speed > (double)50.0)
    *bus_speed = 66.667;
  else
    *bus_speed = 33.333;
  *time_tick = (double)1.0/(*bus_speed * MEGA);

  return (RORC_STATUS_OK);
}

/*****************************************************************************/
int rorcReadFifoCounters(rorcHandle_t dev, rorcFifoCounter_t *counter)
{
  int regnum = C_TMCS;  

  if (dev->rorc_revision < DRORC2)
    return (RORC_CMD_NOT_ALLOWED);

  dRorcWriteReg(dev, regnum, 0);
  counter->tmcs  = dRorcReadReg(dev, regnum++);
  counter->ddlfe = dRorcReadReg(dev, regnum++);
  counter->ddlff = dRorcReadReg(dev, regnum++);
  counter->pcife = dRorcReadReg(dev, regnum++);
  counter->diuff = dRorcReadReg(dev, regnum++);
  counter->ddlfr = dRorcReadReg(dev, regnum++);
  counter->ddlfw = dRorcReadReg(dev, regnum++);
  counter->pcifr = dRorcReadReg(dev, regnum++);
  counter->pcifw = dRorcReadReg(dev, regnum) * 2;  // pci write is double word

  return (RORC_STATUS_OK);
}

/*****************************************************************************/
int rorcReadCounters(rorcHandle_t dev, rorcCounter_t *counter, int inOrOut)
{
  __u32 mb1;

  if (pRorc(dev))
  {
    /* send "Get pRORC counters" command */
    pRorcInitCmdProc (dev);
    if (inOrOut)
      pRorcWriteMb(dev, 1, PRORC_CMD_GET_IN_BYTES);
    else
      pRorcWriteMb(dev, 1, PRORC_CMD_GET_OUT_BYTES);
    pRorcWaitCmdProc (dev);
  
    /* read inMB 1, 2, 3 (4 is already read by pRorcWaitCmdProc) */
    pRorcWaitMbNotEmpty(dev, PRORC_NE_IMB1
                           | PRORC_NE_IMB2
                           | PRORC_NE_IMB3
//                         | PRORC_NE_IMB4
                     );
    counter->mbc = pRorcReadMb (dev, 2);
    counter->gtc = pRorcReadMb (dev, 3);
    counter->lbc = pRorcReadMb (dev, 4) & 0x00FFFFFF;
    mb1 = pRorcReadMb (dev, 1);
    counter->gbc = (mb1 & 0x7FFFFF00) >> 8;
  
    counter->lspeed = (double)counter->lbc / RORC_CLOCK_18;
    counter->bytes = (double)counter->mbc * TWO_TO_THE_20 + (double)counter->gbc;
    counter->time = (double)counter->gtc * RORC_CLOCK_18;
    counter->gspeed = counter->bytes / counter->time;

    return (RORC_STATUS_OK);
  }
  else
    return (RORC_CMD_NOT_ALLOWED);
}

/****************************************************************************/
int rorcReadRxDmaCount(rorcHandle_t dev)
{
  if (pRorc(dev))
    return (pRorcReadRxDmaCount(dev));
  else
    return (dRorcReadRxDmaCount(dev));
}

/****************************************************************************/
int rorcReadTxDmaCount(rorcHandle_t dev)
{
  if (pRorc(dev))
    return (pRorcReadTxDmaCount(dev));
  else
    return (dRorcReadTxDmaCount(dev));
}

/****************************************************************************/
int rorcParamOn(rorc_pci_dev_t *dev, int param)
{
  __u32 cmd;

  if (pRorc(dev))
  {
    switch (param)
    {
      case PRORC_PARAM_LOOPB:
        cmd = PRORC_CMD_LOOPB_ON ;
        break;
      case PRORC_PARAM_STOP_ERR:
        cmd = PRORC_CMD_STOP_ERR_ON ;
        break;
      case (PRORC_PARAM_LOOPB | PRORC_PARAM_STOP_ERR) :
        cmd = PRORC_CMD_LOOPB_ON | PRORC_CMD_STOP_ERR_ON;
        break;
      default :
        return (RORC_INVALID_PARAM);
    }
    pRorcInitCmdProc (dev);
    pRorcWriteMb(dev, 1, cmd);
    pRorcWaitCmdProc (dev);
  }
  else
  {
    if (param != PRORC_PARAM_LOOPB)
      return (RORC_INVALID_PARAM);
    if (!dRorcCheckLoopBack(dev))
      dRorcChangeLoopBack(dev);
  }

  return (RORC_STATUS_OK);
}

/****************************************************************************/
int rorcParamOff(rorc_pci_dev_t *dev)
// switches off both loopback and stop_on_error
{

  if (pRorc(dev))
  {
    pRorcInitCmdProc (dev);
    pRorcWriteMb(dev, 1, PRORC_CMD_PARAM_RESET);
    pRorcWaitCmdProc (dev);
  }
  else
  {
    if (dRorcCheckLoopBack(dev))
      dRorcChangeLoopBack(dev);
  }
  
  return (RORC_STATUS_OK);
}

/****************************************************************************/
int rorcHltFlctlOn(rorc_pci_dev_t *dev)
{

  if (pRorc(dev))
    return (RORC_CMD_NOT_ALLOWED);

  if (dev->rorc_revision < INTEG)
    return (RORC_CMD_NOT_ALLOWED);

  if (!dRorcCheckHltFlctl(dev))
    dRorcChangeHltFlctl(dev);

  return (RORC_STATUS_OK);
}

/****************************************************************************/
int rorcHltFlctlOff(rorc_pci_dev_t *dev)
{

  if (pRorc(dev))
    return (RORC_CMD_NOT_ALLOWED);

  if (dev->rorc_revision < INTEG)
    return (RORC_CMD_NOT_ALLOWED);

  if (dRorcCheckHltFlctl(dev))
    dRorcChangeHltFlctl(dev);

  return (RORC_STATUS_OK);
}

/****************************************************************************/
int rorcHltSplitOn(rorc_pci_dev_t *dev)
{

  if (pRorc(dev))
    return (RORC_CMD_NOT_ALLOWED);

  if (dev->rorc_revision < INTEG)
    return (RORC_CMD_NOT_ALLOWED);

  if (!dRorcCheckHltSplit(dev))
    dRorcChangeHltSplit(dev);

  return (RORC_STATUS_OK);
}

/****************************************************************************/
int rorcHltSplitOff(rorc_pci_dev_t *dev)
{

  if (pRorc(dev))
    return (RORC_CMD_NOT_ALLOWED);

  if (dev->rorc_revision < INTEG)
    return (RORC_CMD_NOT_ALLOWED);

  if (dRorcCheckHltSplit(dev))
    dRorcChangeHltSplit(dev);

  return (RORC_STATUS_OK);
}

/****************************************************************************/
int rorcArmDataGenerator(rorc_pci_dev_t *dev,
                         __u32           initEventNumber,
                         __u32           initDataWord,
                         int             dataPattern,
                         int             eventLen,
                         int             seed,
                         int             *rounded_len)
{
  unsigned long blockLen;

  if ((eventLen < 1) || (eventLen >= 0x00080000))
    return (RORC_INVALID_PARAM);

  *rounded_len = eventLen;

  if (pRorc(dev))
  {
    if (seed)
    {
      /* round to the nearest lower power of two */
      *rounded_len = roundPowerOf2(eventLen);
      blockLen = (*rounded_len - 1) | 0x80000000;
    }
    else
      blockLen = eventLen - 1;

    pRorcInitCmdProc (dev);
    pRorcWriteMb(dev, 2, initDataWord);
    pRorcWriteMb(dev, 3, blockLen);
    pRorcWriteMb(dev, 4, seed);
    pRorcWriteMb(dev, 1, PRORC_CMD_DG_PARAM1);
    pRorcWaitCmdProc (dev);
  
    pRorcInitCmdProc (dev);
    pRorcWriteMb(dev, 2, dataPattern);
    pRorcWriteMb(dev, 3, initEventNumber);
    pRorcWriteMb(dev, 1, PRORC_CMD_DG_PARAM2);
    pRorcWaitCmdProc (dev);
  }
  else
  {
    if (seed)
    {
      /* round to the nearest lower power of two */
      *rounded_len = roundPowerOf2(eventLen);
      blockLen = ((*rounded_len - 1) << 4) | dataPattern;
      blockLen |= 0x80000000;
      dRorcWriteReg(dev, C_DG2, seed);
    }
    else
    {
      blockLen = ((eventLen - 1) << 4) | dataPattern;
      dRorcWriteReg(dev, C_DG2, initDataWord);
    }

    dRorcWriteReg(dev, C_DG1, blockLen);
    dRorcWriteReg(dev, C_DG3, initEventNumber);
  }

  return (RORC_STATUS_OK);
}

/****************************************************************************/
int rorcStartDataGenerator(rorc_pci_dev_t *dev, __u32 maxLoop)
{
  __u32 cycle;

  if (maxLoop)
    cycle = (maxLoop - 1) & 0x7fffffff;
  else
    cycle = 0x80000000;

  if (pRorc(dev))
  {
    pRorcInitCmdProc (dev);
    pRorcWriteMb(dev, 2, cycle);
    pRorcWriteMb(dev, 1, PRORC_CMD_START_DG);
    pRorcWaitCmdProc (dev);
  }
  else
  {
    dRorcWriteReg(dev, C_DG4, cycle);
    dRorcWriteReg(dev, C_CSR, DRORC_CMD_START_DG);
  }

  return (RORC_STATUS_OK);
}

/****************************************************************************/
int rorcStopDataGenerator(rorc_pci_dev_t *dev)
{ 
  if (pRorc(dev))
  {
    pRorcInitCmdProc (dev);
    pRorcWriteMb(dev, 2, (__u32) 0); // dummy parameter
    pRorcWriteMb(dev, 1, PRORC_CMD_STOP_DG);
    pRorcWaitCmdProc (dev);
  }
  else
    dRorcWriteReg(dev, C_CSR, DRORC_CMD_STOP_DG);
  
  return (RORC_STATUS_OK);
}

/****************************************************************************/
int rorcStartDataReceiver(rorc_pci_dev_t *dev,
                          unsigned long   readyFifoBaseAdress)
{
  unsigned long dmaWaitCycles;

  if (pRorc(dev))
  {
    /* dev->phys_addr_rf = readyFifoBaseAdress; */
    /* dev->usr_addr_rf  = readyFifoUserAddress; */

    pRorcInitCmdProc (dev);
    pRorcWriteMb(dev, 2, readyFifoBaseAdress);
    pRorcWriteMb(dev, 1, PRORC_CMD_PUT_READY_BASE);
    pRorcWaitCmdProc (dev);

    dmaWaitCycles = PRORC_DMA_WAIT;
    pRorcInitCmdProc (dev);
    pRorcWriteMb(dev, 2, dmaWaitCycles);
    pRorcWriteMb(dev, 1, PRORC_CMD_START_W_DMA);
    pRorcWaitCmdProc (dev);
  }
  else
  {
    dRorcWriteReg(dev, C_RRBAR, readyFifoBaseAdress);
    if (!(dRorcReadReg(dev, C_CSR) & DRORC_CMD_DATA_RX_ON_OFF))
      dRorcWriteReg(dev, C_CSR, DRORC_CMD_DATA_RX_ON_OFF);
  }

  return (RORC_STATUS_OK);
}

/****************************************************************************/
int
rorcStopDataReceiver(rorc_pci_dev_t *dev)
{
  printf("111\n");
  if(pRorc(dev))
  {
  printf("112\n");
    pRorcInitCmdProc(dev);
    pRorcWriteMb(dev, 2, (__u32) 0); // dummy parameter
    pRorcWriteMb(dev, 1, PRORC_CMD_STOP_W_DMA);
    pRorcWaitCmdProc(dev);
  }
  else
  {
  printf("113: 0x%08x\n",dev);
  /*sergey: segfault here*/
    if(dRorcReadReg(dev, C_CSR) & DRORC_CMD_DATA_RX_ON_OFF)
	{
  printf("114\n");
      dRorcWriteReg(dev, C_CSR, DRORC_CMD_DATA_RX_ON_OFF);
	}
  
  }
  printf("115\n");

  return(RORC_STATUS_OK);
}

/****************************************************************************/
int rorcStartDownload(rorc_pci_dev_t *dev,
                       unsigned long   bufferPhysAddress,
                       unsigned long   bufferWordLength,
                       unsigned long   returnPhysAddress)
{
  
  if (pRorc(dev))
  {
    pRorcInitCmdProc (dev);
    pRorcWriteMb(dev, 2, bufferPhysAddress);
    pRorcWriteMb(dev, 3, bufferWordLength);
    pRorcWriteMb(dev, 4, returnPhysAddress);
    pRorcWriteMb(dev, 1, PRORC_CMD_DOWNL_DATA);
    pRorcWaitCmdProc (dev);
  }
  else
  {
    dRorcWriteReg(dev, C_TRBAR, returnPhysAddress);
    dRorcPushTxFreeFifo(dev, bufferPhysAddress, bufferWordLength, 0);
    if (!(dRorcReadReg(dev, C_CSR) & DRORC_CMD_DATA_TX_ON_OFF))
      dRorcWriteReg(dev, C_CSR, DRORC_CMD_DATA_TX_ON_OFF);
  }

  return (RORC_STATUS_OK);
}

/****************************************************************************/
int rorcStopDownload(rorc_pci_dev_t *dev)

{
  if (!pRorc(dev))
  {
    if ((dRorcReadReg(dev, C_CSR) & DRORC_CMD_DATA_TX_ON_OFF))
      dRorcWriteReg(dev, C_CSR, DRORC_CMD_DATA_TX_ON_OFF);
  }

  return (RORC_STATUS_OK);
}

/****************************************************************************/
int rorcStartJtag(rorc_pci_dev_t        *dev,
                   unsigned long          bufferPhysAddress,
                   unsigned long          bufferWordLength,
                   unsigned long          returnPhysAddress)
{
  if (pRorc(dev))
  {
    pRorcInitCmdProc (dev);
    pRorcWriteMb(dev, 2, bufferPhysAddress);
    pRorcWriteMb(dev, 3, bufferWordLength);
    pRorcWriteMb(dev, 4, returnPhysAddress);
    pRorcWriteMb(dev, 1, PRORC_CMD_DOWNL_JTAG);
    pRorcWaitCmdProc (dev);
  }
  else
  {
/*
    dRorcWriteReg(dev, C_TAFH, bufferPhysAddress);
    dRorcWriteReg(dev, C_TAFL, bufferWordLength);
    dRorcWriteReg(dev, C_TRBAR, returnPhysAddress); 
*/
    dRorcWriteReg(dev, C_CSR, DRORC_CMD_JTAG_DOWN_ON);
  }

  return (RORC_STATUS_OK);
}

/****************************************************************************/
int rorcStopJtag(rorc_pci_dev_t        *dev)

{
  if (!pRorc(dev))
    dRorcWriteReg(dev, C_CSR, DRORC_CMD_JTAG_DOWN_OFF);

  return (RORC_STATUS_OK);
}

/***************************************************************************/

#endif
