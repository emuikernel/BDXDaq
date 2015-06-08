#ifdef Linux

/* rorcGetEvent.c - rorc_receive.c modified by Sergey Boyarinov for CODA DAQ */

#define DESCRIPTION "RORC's Data Receiver for CODA DAQ System"
#define SOURCE_FILE "rorcGetEvent.c"

#include <signal.h>
#include <getopt.h>
#include "rorc_ddl.h"

#include "rcu.h" /*sergey: for rorc_node*/

#define printf(message...) (fprintf (stdout, message), fflush(stdout))

#define MAX_MAPPING   (1024*1024*30)

#define MAX_LEN       (1024*512-1) /* 2^19-1 */
#define PAGE_LEN      (1024*4)
#define MIN_BLOCK_LEN (64*4)
#define MAX_BLOCK     (MAX_MAPPING/MIN_BLOCK_LEN + 1)

#define RF_GAP          1    // every entry used in RF
#define MAX_RF_INDEX    (128 * RF_GAP - 1) 
#define MAX_MAPPING_RF  ((MAX_RF_INDEX + 1) * 8) /*1024*/

#define MAX_FIFO 128
#define MIN_FIFO (MAX_FIFO-1)

/*
#define debug printf
*/

typedef struct
{
   int                     rf;
   volatile unsigned long *addr;
} memoryBlock_t;


/* global variables */ 

static struct sigaction sigac;
static long long int timeout; 
static unsigned long long resp_cycle;
static int rorc_serial;


/*******************************/
/* memory management variables */

/* whole memory vars, set once during memory map process */
static int physmem_fd; /* memory file descriptor */
volatile unsigned long *addr_user; /* user space startup address */
unsigned long phys_addr; /* phys space startup address */
static unsigned long phys_size_physmem; /* memory size available */
static unsigned long max_mapping; /* memory size we will use (sergey: for one channel) */

typedef struct
{
  int               id;
  int               channel;
  unsigned long     mem_offset;
  unsigned long     phys_addr; /* phys space startup address */
  unsigned long    *addr_user; /* user space startup address */
  unsigned long     max_buffer_size;
  int               max_block;
  unsigned long     phys_addr_rf;
  unsigned long     phys_addr_data;
  unsigned long     phys_addr_end;
  rorcReadyFifo_t  *rf;

  volatile unsigned long *next_free_block; /* user space fifo pointer */
  unsigned long next_free_block_phys; /* phys space fifo pointer */
  memoryBlock_t block[MAX_BLOCK]; /* current fifo pointers: rf and addr */

  volatile unsigned long *buffer_address;
  int               rf_free_index;
  int               next_free_index;
  int               free_block;
  int               next_index;
  int               n_read;
  int               rf_index;

  unsigned long     dtsw; 
  int               stopped;
  int               event;
  int               continuation;
  int               open_transaction;
  int               last_data_len; 
  int               sum_data_len;

  rorcDescriptor_t  prorc;

  int active;

} rorcStruct_t;

static rorcStruct_t rorc[2]; /* for one rorc with 2 channels */
static int rorcActive = 0;

/********************************************************
*          signal handler                               *
********************************************************/
void
sig_hand(int signum)
{

/* signum: 1(SIGHUP)  2(SIGINT)  3(SIGQUIT) */
  if(signum < 4)
  {
    interrupt_arrived = 1;
  }
}

/********************************************************
*          signal handling                              *
********************************************************/
void
set_sig_hand(void)
{
  sigac.sa_flags = 0;
  sigac.sa_handler =  sig_hand;
  if (sigaction(SIGHUP, &sigac, NULL) < 0 )
  {
     perror(" sigaction SIGHUP ");
     exit(1);
  }
  if (sigaction(SIGINT, &sigac, NULL) < 0 )
  {
     perror(" sigaction SIGINT ");
     exit(1);
  }
  if (sigaction(SIGQUIT, &sigac, NULL) < 0 )
  {
     perror(" sigaction SIGQUIT ");
     exit(1);
  }
}




/********************************************************
*         next_push                                    *
********************************************************/
void
next_push(rorcDescriptor_t *prorc_dev, rorcStruct_t *rr)
{
  rr->rf[rr->rf_free_index].length = -1;
  rr->rf[rr->rf_free_index].status = -1;
  rorcPushFreeFifo(prorc_dev,
                   rr->next_free_block_phys,
                   (PAGE_LEN/4),
                   rr->rf_free_index);

  debug("PUSH: phys=0x%08lx, user=%8p, index=%d\n",
                   rr->next_free_block_phys,
                   rr->next_free_block,
                   rr->rf_free_index);

  rr->free_block--;
  rr->next_free_index++;
  if(rr->next_free_index >= rr->max_block) rr->next_free_index = 0;
  rr->rf_free_index += RF_GAP;
  if(rr->rf_free_index > MAX_RF_INDEX) rr->rf_free_index = 0;
  rr->next_free_block_phys += PAGE_LEN;
  rr->next_free_block += PAGE_LEN/4;
  if(((rr->next_free_block_phys + PAGE_LEN - rr->phys_addr - 1)
        > rr->max_buffer_size))
  {
    rr->next_free_block_phys = rr->phys_addr;
    rr->next_free_block = rr->addr_user;
  }
  rr->block[rr->next_free_index].rf = rr->rf_free_index;
  rr->block[rr->next_free_index].addr = rr->next_free_block;

  debug("PUSH: next phys=0x%08lx, user=%8p, index=%d into block[%d]\n", 
         rr->next_free_block_phys,
         rr->next_free_block,
         rr->rf_free_index,
         rr->next_free_index);
}


/*********************************************************************/
/*********************************************************************/
/*********************************************************************/

int
rorcPrestart(int serial, int channel, int nfecs, int fecs[14], int threshold, int fpd)
{
  int ret = 0;
  int opt[20];

  /* currently we assume one rorc with 2 channels, can be changed if necessary */
  rorc[channel].channel = channel;
  rorc[channel].id = 0;

  /* connect to RCU */
  opt[0] = channel;
  opt[1] = serial;
  altroInit(1,opt);

  rorc_serial = serial;

  /* switch off fecs */
  opt[0] = 0;
  altroInit(3,opt);

  /* switch on fecs */
  opt[0] = nfecs;
  opt[1] = fecs[0];
  opt[2] = fecs[1];
  opt[3] = fecs[2];
  opt[4] = fecs[3];
  opt[5] = fecs[4];
  opt[6] = fecs[5];
  opt[7] = fecs[6];
  opt[8] = fecs[7];
  opt[9] = fecs[8]; 
  opt[10] = fecs[9];
  opt[11] = fecs[10];
  opt[12] = fecs[11];
  opt[13] = fecs[12];
  opt[14] = fecs[13];
  altroInit(3,opt); 

  /* setting readout */
  opt[0] = 100;       /*the number of samples per channel*/
  opt[1] = 15;        /*the number of pre-trigger samples*/
  opt[2] = 1;         /*0-no zero suppression, 1-use zero suppression*/
  opt[3] = threshold; /*zero suppression threshold*/
  opt[4] = 3;         /*zero suppression nr. of samples in cluster*/
  opt[5] = 3;         /*zero suppression nr. pre samples*/
  opt[6] = 3;         /*zero suppression nr. post samples*/
  opt[7] = 1;         /*0-no baseline subtraction, 1-use baseline subtraction*/
  opt[8] = 4;         /*baseline subtraction mode 0-off, 4-on: see pg. 37 of ALTRO Manual*/
  opt[9] = fpd;       /*fixed pedestal for baseline subtraction*/
  opt[10] = 6;        /*external trigger*/
  altroInit(10,opt);

  /* disconnect from RCU */
  altroInit(2,opt);

  return(ret);
}


/*



*/

int
rorcDownload()
{
  int ret;
  int i, he;
  int channel;
  volatile unsigned long *addr_user_rf;
  unsigned long *addr_user_data;
  rorcStruct_t *rr;

  printf("\nCompiled RORC sw version: %s.\n", RORC_LIBRARY_VERSION);


  /***********************************/
  /* physmem (memory initialization) */

  /* returns:
       addr_user - user space address
       phys_addr - phys space address
       phys_size_physmem - memory size
   */
  if(physmemOpen(&physmem_fd,&addr_user,&phys_addr,&phys_size_physmem))
  {
    return(-1);
  }

  rorcActive = 1;

  printf(" Allocation of the physmem memory.\n");
  printf(" Physical address: 0x%08lx.\n", phys_addr);
  printf(" Size of the physmem memory: %ld (0x%08lx) bytes, %ld (0x%08lx) words.\n",
           phys_size_physmem, phys_size_physmem, 
           phys_size_physmem >> 2, phys_size_physmem >> 2);

  /* set 'max_mapping' to the minimum 'MAX_MAPPING' or 'phys_size_physmem';
     it will be actual size of our memory */
  max_mapping = MAX_MAPPING;
  if(phys_size_physmem < MAX_MAPPING) max_mapping = phys_size_physmem;




  /************************************************/
  /* loop over 2 channels setting memory pointers */

  for(channel=0; channel<2; channel++)
  {
    rr = &rorc[channel];
    rr->mem_offset = channel * MAX_MAPPING; /* must step by max space required for one channel */
    if((rr->mem_offset + max_mapping) > phys_size_physmem)
    {
      printf("Error: useable memory offset (%ld) + size (%ld) must be "
           "less than the physmem area physical size (%ld).\n",
                    rr->mem_offset, max_mapping, phys_size_physmem);
      return(-2);
    }
 
    /* actual user space and phys space addresses */
    rr->phys_addr = phys_addr + rr->mem_offset;
    rr->addr_user = addr_user + rr->mem_offset/4;
    printf("[%d] memory starts at 0x%08lx (%p), offset: 0x%08lx.\n",channel,
             rr->phys_addr, rr->addr_user, rr->mem_offset);
    printf("Size of the useable memory: %ld (0x%08lx) bytes, %ld (0x%08lx) words.\n",
           max_mapping, max_mapping, max_mapping>>2, max_mapping>>2);

    /* actual buffer size to store event(s) from one rorc channel */
    rr->max_buffer_size = max_mapping - MAX_MAPPING_RF - MAX_LEN*4 - 2*PAGE_LEN;

    /* align it with PAGE_LEN */
    he = rr->max_buffer_size % PAGE_LEN;
    if(he) rr->max_buffer_size += PAGE_LEN - he;

    /* the number of blocks in that buffer */
    rr->max_block = rr->max_buffer_size / PAGE_LEN;
    if(MAX_FIFO > rr->max_block)
    {
      printf("Error: max_fifo (%d) times PAGE_LEN (%d) must be "
           "less than reserved physmem area buffer size (%ld).\n", 
                    MAX_FIFO, PAGE_LEN, rr->max_buffer_size);
      return(-3);
    }

    rr->phys_addr_rf = rr->phys_addr + rr->max_buffer_size;
    addr_user_rf = rr->addr_user + (rr->max_buffer_size >> 2);

    rr->phys_addr_data = rr->phys_addr_rf + MAX_MAPPING_RF;
    addr_user_data = (unsigned long *)addr_user_rf + (MAX_MAPPING_RF >> 2);

    /* align it to PAGE_LEN */
    he = rr->phys_addr_data % PAGE_LEN;
    if(he)
    {
      rr->phys_addr_data += PAGE_LEN - he;
      addr_user_data += (PAGE_LEN - he) >> 2;
    }

    rr->phys_addr_end = rr->phys_addr_data + (MAX_LEN << 2);
    rr->rf = (rorcReadyFifo_t *)addr_user_rf;


    printf(" Event buffer address: 0x%08lx (%8p), size: 0x%08lx (%ld) bytes\n",
           phys_addr,
           addr_user,
           rr->max_buffer_size,
           rr->max_buffer_size);
    printf(" Memory block (page) size: 0x%08x (%d) bytes, number of blocks: %d\n",
           PAGE_LEN, PAGE_LEN, rr->max_block);
    printf(" Ready FIFO address: 0x%08lx (%8p), offset: 0x%08lx, size: %d bytes\n", 
           rr->phys_addr_rf,
           addr_user_rf,
           rr->phys_addr_rf - phys_addr + rr->mem_offset,
           MAX_MAPPING_RF);
    printf(" Expected event address: 0x%08lx (%8p), offset: 0x%08lx,\n",
           rr->phys_addr_data,
           addr_user_data,
           rr->phys_addr_data - phys_addr + rr->mem_offset);
    printf("          size: 0x%08lx (%ld) bytes (%ld words)\n",
           rr->phys_addr_end - rr->phys_addr_data,
           rr->phys_addr_end - rr->phys_addr_data,
           (rr->phys_addr_end - rr->phys_addr_data) >> 2);
    printf(" Start of free memory: 0x%08lx, offset: 0x%08lx\n",
           rr->phys_addr_end,
           rr->phys_addr_end - phys_addr + rr->mem_offset);
    printf(" Free memory size: 0x%08lx (%ld) bytes (%ld words)\n",
           max_mapping - (rr->phys_addr_end + 4 - phys_addr),
           max_mapping - (rr->phys_addr_end + 4 - phys_addr),
           (max_mapping - (rr->phys_addr_end + 4 - phys_addr)) >> 2);
    printf(" max fifo: %d, min fifo: %d.\n", MAX_FIFO, MIN_FIFO);
    printf(" Timeout values used: %lld us (%lld cycles),\n",DDL_RESPONSE_TIME, timeout);
    printf(" Resp_wait: %lld us (%lld cycles).\n",DDL_RESPONSE_TIME,resp_cycle);
  }

  return(ret);
}

int
rorcGo(int channel)
{
  rorcStruct_t *rr = &rorc[channel];
  int ret = 0;
  int p_minor = 0;
  int reset_level = 3;
  int i;
  __u32 fw;
  unsigned long status; 
  stword_t stw;


  /*******************************/
  /* open RORC and map mailboxes */

  printf(" Asking RORC %d/%d to recieve data of %d word length\n",
          p_minor, channel, MAX_LEN);
  printf("channel=%d\n",channel);
  ret = rorcOpenChannel(&rr->prorc, p_minor, channel);
  if(ret && (ret != RORC_PCI_ERROR)) goto exit;
  timeout = DDL_RESPONSE_TIME * rr->prorc.pci_loop_per_usec;
  resp_cycle = DDL_RESPONSE_TIME * rr->prorc.pci_loop_per_usec;
  printf(" Number of wait loops per usec: %.2f.\n", rr->prorc.pci_loop_per_usec);
  printf(" Maximum allowed response time: %lld.\n", rr->prorc.max_resp_time);
  printf(" Maximum allowed number of loops: %lld.\n", DDL_MAX_WAIT_CYCLE);
  if(resp_cycle > DDL_MAX_WAIT_CYCLE)
  {
    printf("Error: time-out value %lld us is too large.\n",DDL_RESPONSE_TIME);
    printf("Please choose a value less than or equal to %lld us.\n", 
                                                      rr->prorc.max_resp_time);
    goto exit;
  }

  /* PCI bus */
  if(rorcCheckDriver(&rr->prorc, 1, 5, 1, 5))
    printf(" PCI bus mode: %s, %s\n", rorcInterpretBusMode(rr->prorc.bus_speed_mode),
                                   rorcInterpretBusSpeed(rr->prorc.bus_speed_mode));

  /* check max_fifo */
  fw = rorcReadFw(&rr->prorc);
  if(rorcFFSize(fw) && (rorcFFSize(fw) < MAX_FIFO))
  {
    printf("Error: max_fifo (%d) too big.\n", MAX_FIFO);
    rorcInterpretFw(fw);
    rorcClose(&rr->prorc);
    goto exit;
  }

  if(reset_level)
  {
    rorcArmDDL(&rr->prorc, RORC_RESET_RORC);
    printf(" RORC reset\n");
  }

  if(reset_level > 1)
  {
    rorcArmDDL(&rr->prorc, RORC_RESET_DIU);
    printf(" DIU reset\n");

    if(reset_level > 2)
    {    
      /* wait a little before SIU reset */
      usleep(200000);
      /* reset SIU */
      if(rorcArmDDL(&rr->prorc, RORC_RESET_SIU) != RORC_STATUS_OK)
      {
        printf("Error: Could not reset SIU. I stop.\n");
        rorcClose(&rr->prorc);
        goto exit;
      }
      printf(" SIU reset\n");

      rorcArmDDL(&rr->prorc, RORC_RESET_DIU);
      printf(" DIU reset\n");
    }

    rorcArmDDL(&rr->prorc, RORC_RESET_RORC);
    printf(" RORC reset\n");
  }

  if(reset_level)
  {
    /* wait a little after reset */
    usleep(100000);
  }

  /* clear SIU/DIU status */
  if(rorcCheckLink(&rr->prorc) != RORC_STATUS_OK)
  {
    printf(" SIU not seen. Can not clear SIU status. I stop\n");
    rorcClose(&rr->prorc);
    goto exit;
  }
  else
  {
    status = ddlReadSiu(&rr->prorc, 0, timeout);
    if(status != -1)
      printf(" SIU status cleared. Status word: 0x%08lx\n", status);
  }

  status = ddlReadDiu(&rr->prorc, 0, timeout);
  if(status != -1)
    printf(" DIU status cleared. Status word: 0x%08lx\n", status);


  /* signal handling */
  interrupt_arrived = 0;
  set_sig_hand();

  /*********************/
  /* start data cycles */

  rr->event = 0;
  rr->continuation = 0;
  rr->open_transaction = 0;

  /* push free fifo for the first MAX_FIFO events */
  rorcReset(&rr->prorc, RORC_RESET_FF);
  if(rorcCheckFreeFifo(&rr->prorc) == RORC_FF_EMPTY)
  {
    printf(" Rx address FIFO emptied\n");
  }
  else
  {
    printf("Error: I can not empty the Rx address FIFO\n");
    goto exit;
  }
  rr->free_block = rr->max_block;
  rr->next_free_index = 0;
  rr->rf_free_index = 0;
  rr->next_free_block_phys = rr->phys_addr;
  rr->next_free_block = rr->addr_user;
  rr->block[rr->next_free_index].rf = rr->rf_free_index;
  rr->block[rr->next_free_index].addr = rr->next_free_block;;
  debug("First phys=0x%08lx, user=%8p, index=%d into block[%d]\n", 
      rr->next_free_block_phys,
      rr->next_free_block,
      rr->rf_free_index,
      rr->next_free_index);
  debug("PUSH %d entries\n", MAX_FIFO);
  for(i=0; i<MAX_FIFO; i++) next_push(&rr->prorc,&rorc[channel]);

  /* send start data collecting command */
  rorcStartDataReceiver(&rr->prorc, rr->phys_addr_rf);
  printf(" Data Receiver started\n");

  /* clear SIU/DIU status */
  if(rorcCheckLink(&rr->prorc) != RORC_STATUS_OK)
  {
    printf(" SIU not seen. Can not clear SIU status. I stop\n");
    rorcClose(&rr->prorc);
    goto exit;
  }
  else
  {
    status = ddlReadSiu(&rr->prorc, 0, timeout);
    if(status != -1)
      printf(" SIU status cleared. Status word: 0x%08lx\n", status);
  }

  status = ddlReadDiu(&rr->prorc, 0, timeout);
  if(status != -1)
  {
    printf(" DIU status cleared. Status word: 0x%08lx\n", status);
  }

  /* RDYRX command to FEE */
  ret = rorcStartTrigger(&rr->prorc, resp_cycle, &stw);
  if(ret == RORC_LINK_NOT_ON)
  {
    printf("Error: LINK IS DOWN, RDYRX command can not be sent. I stop\n");
    goto exit;
  }
  else if(ret == RORC_STATUS_ERROR)
  {
    printf("Error: RDYRX command can not be sent. I stop\n");
    goto exit;
  }
  else if(ret == RORC_NOT_ACCEPTED)
  {
    printf(" No reply arrived for RDYRX in timeout %lld usec\n",
      DDL_RESPONSE_TIME); 
  }
  else
  {
    printf(" FEE accepted the RDYDX command. Its reply: 0x%08lx\n",stw.stw);
  }
  rr->open_transaction = 1;
  fflush (stdout);

  interrupt_arrived = 0;

  /* start looping */
  rr->stopped = 0;
  rr->last_data_len = 0;
  rr->sum_data_len = 0;
  rr->n_read = 0;
  rr->next_index = 0;
  rr->rf_index = rr->block[rr->next_index].rf;
  rr->buffer_address = rr->block[rr->next_index].addr;
  debug("For the first cycle rf_index: %d, buff_addr: %p from block[%d]\n", 
              rr->rf_index, rr->buffer_address, rr->next_index);
  rr->dtsw = 1; /* last event is finished */
  printf("\n\n");

exit:

  ret = 1;

  rr->active = 1;

  return(ret);
}

#define MIN(x,y) ((x) < (y) ? (x) : (y))

/* returns event length in words, or negative error */
int
rorcGetEvent(int channel, int NOUTPUTBUF, int *outputbuf)
{
  rorcStruct_t *rr = &rorc[channel];
  int data_len, comp_err = 0;
  int i, j, ret;
  int data_arrived;
  int exit_loop = 0, copied_length = 0;
  int dtsw_len;
  stword_t stw;
  int ncycles;

  while(1)
  {

    /***********************/
    /* loop for DMA ending */

    data_arrived = 1;
    ncycles = 0;
    while(!rorcHasData(rr->rf, rr->rf_index))
    {
	  /* will stay in the loop forever if there is no data - be careful !!! */
      ncycles ++;
      if(rr->stopped)
      {
        if(ddlCheckStatus(&rr->prorc))
        {
          printf(" Status arrived\n");
          stw = ddlReadStatus(&rr->prorc);
          printf(" Status word after stop: 0x%08lx\n", stw.stw);
          goto exit;
        }
      }
      if(interrupt_arrived && !rr->stopped)
      {
        data_arrived = 0;
        goto brr;
      }

	  if(0) /* do it occationally ??? */
	  {
        /* check if link is on */
        if(rorcCheckLink(&rr->prorc) != RORC_STATUS_OK)
        {
          printf("\nError: The DDL link is down.\n"); 
          goto exit;
        }

        if(rr->stopped)
        {
          printf("\nNo data arrived in timeout after the stop\n");
          goto exit;
        }
      }

      /*sergey */
      if(ncycles==100000)
	  {
		/*
        printf("channel=%d: forced exit of !rorcHasData loop on ncycles=%d\n",
          channel,ncycles);
		*/
        goto nodata;
	  }

    } /* !rorcHasData(rf, rf_index) */


    /* */
    if(data_arrived)
    {
      rr->n_read++;
      data_len = rr->rf[rr->rf_index].length;
      rr->dtsw = rr->rf[rr->rf_index].status;
      if(rr->dtsw)
      {
        if(!rr->continuation)
        {
          rr->event++;
        }
      }

      dtsw_len = (rr->dtsw & 0x7ffff000) >> 12;
      debug("DATA: event=%d rf_index=%d dtstw=0x%08lx buffer=%8p\n", 
                    rr->event, rf_index, rr->dtsw, rr->buffer_address);

      /* check the length and DTSTW */
      comp_err = 0;
      rr->sum_data_len += data_len;
      if(rr->dtsw)
      {
        if(((rr->dtsw & 0xff) != 0x82) || (rr->sum_data_len != dtsw_len))
        {
          printf("\nDTSTW ERROR: DTSTW: 0x%08lx\n",rr->dtsw);
          printf(" data len: 0x%08x (%d), sum_data_len: 0x%08x (%d), dtsw_len: 0x%08x (%d)\n",
               data_len, data_len, rr->sum_data_len, rr->sum_data_len, dtsw_len, dtsw_len);
          comp_err = 1; /* force exit on this error */
        }
        if(rr->dtsw & 0x80000000)
        {
          printf("\nERROR bit set in DTSTW: DTSTW: 0x%08lx\n",rr->dtsw);
          comp_err = 1; /* force exit on this error */
        }
      }


	  /* copy fragment to the assembly buffer */
      for(i=0, j=rr->last_data_len; i<data_len; i++, j++)
	  {
        if(j<NOUTPUTBUF) outputbuf[j] = *(rr->buffer_address + i);
        else {printf("ERROR: ch %d outputbuf index j=%d\n",channel,j);fflush(stdout);}
	  }
	  

      if(rr->dtsw)
      {
        /* copy completed event to the output buffer */
        if(NOUTPUTBUF < rr->sum_data_len)
        {
          printf("WARN: ch %d, sum_data_len=%d\n",channel,rr->sum_data_len);
		  /*
		  if(channel==0)
		  {
            FILE *fd;
            fd = fopen("/home/clasrun/tpcbig.out","w");
            for(j=0; j<rr->sum_data_len; j++)
              fprintf(fd,"%6d 0x%08x\n",j,outputbuf[j]);
            fclose(fd);
		  }
		  */
		}


		/* print trailer words containing possible errors
        if(outputbuf[rr->sum_data_len-8]!=0x84000000||
           outputbuf[rr->sum_data_len-7]!=0x88000000||
           outputbuf[rr->sum_data_len-6]!=0x8c000000)
        {
          printf("\nchannel %d trailer:\n",channel);
          printf("trailer[last  ]=0x%08x\n",outputbuf[rr->sum_data_len-1]);
          printf("trailer[last-1]=0x%08x\n",outputbuf[rr->sum_data_len-2]);
          printf("trailer[last-2]=0x%08x\n",outputbuf[rr->sum_data_len-3]);
          printf("trailer[last-3]=0x%08x\n",outputbuf[rr->sum_data_len-4]);
          printf("trailer[last-4]=0x%08x\n",outputbuf[rr->sum_data_len-5]);
          printf("trailer[last-5]=0x%08x\n",outputbuf[rr->sum_data_len-6]);
          printf("trailer[last-6]=0x%08x\n",outputbuf[rr->sum_data_len-7]);
          printf("trailer[last-7]=0x%08x\n",outputbuf[rr->sum_data_len-8]);
          printf("trailer[last-8]=0x%08x\n\n",outputbuf[rr->sum_data_len-9]);
		}
*/


        copied_length = MIN(NOUTPUTBUF,rr->sum_data_len);
        exit_loop = 1;

        rr->last_data_len = 0;
        rr->sum_data_len = 0;

        /* prepare for the next DDL block */
        rr->continuation = rr->dtsw & CONTINUATION_BIT;
      }  
      else
      {
        rr->last_data_len += data_len;
      }
    } /* data arrived */

	debug("40\n");

	/* */
    if(!rr->stopped)
    {
      if(comp_err || (interrupt_arrived && !rr->continuation))
      {
        /* send EOBTR to FEE */
        ret = rorcStopTrigger(&rr->prorc, timeout, &stw);
        if(ret == RORC_LINK_NOT_ON)
        {
          printf("Error: LINK IS DOWN, EOBTR command can not be sent. I stop\n");
          goto brr;
        }
        else if(ret == RORC_STATUS_ERROR)
        {
          printf("Error: EOBTR command can not be sent. I stop\n");
          goto brr;
        }
        printf(" EOBTR command sent to the FEE\n");
        rr->open_transaction = 0;
        if(ret != RORC_NOT_ACCEPTED)
        {
          printf(" FEE accepted the EOBTR command. Its reply: 0x%08lx\n", 
                    stw.stw);
          goto brr;
        }

        if(comp_err)
        {
          goto brr;
        }
        rr->stopped=1;
      }
    } /* !rr->stopped */

	debug("50\n");

    /* */
    if(data_arrived)
    {
      /* prepare for next block (page) */
      rr->free_block++;
      rr->next_index++;
      if (rr->next_index >= rr->max_block) rr->next_index = 0;
      rr->rf_index = rr->block[rr->next_index].rf;
      rr->buffer_address = rr->block[rr->next_index].addr;
      debug("For the next cycle rf_index: %d, buff_addr: %p from block[%d]\n", 
                rr->rf_index, rr->buffer_address, rr->next_index);

      if(rr->dtsw)
      {
        if((MAX_FIFO - rr->n_read) <= MIN_FIFO) /* push free fifo for the next n_read events */
        {
          debug("PUSH %d entrie(s)\n", n_read);
          while(rr->n_read)
          { 
            next_push(&rr->prorc,&rorc[channel]);
            rr->n_read--;
          } /* while (n_read) */
        } /* if (MAX_FIFO - n_read <= MIN_FIFO) */
      }  /* if (dtstw) */
      else
      {
        if((MAX_FIFO - rr->n_read) <= 1)
        {
          debug("PUSH 1 entry\n");
          next_push(&rr->prorc,&rorc[channel]);
          rr->n_read--;
        }
      }
    } /* if data_arrived */

	debug("90\n");

    if(exit_loop) break;

  } /* while(1) */

  return(copied_length);

nodata:




  return(0);


brr:
  return(-1);

exit:

  return(-2);
}


int
rorcEnd(int channel)
{
  rorcStruct_t *rr = &rorc[channel];
  int ret = 0;

  if(rr->active)
  {
    rr->active = 0;

    /* send stop data collecting command */
    printf("11: 0x%08x\n",rr->prorc);fflush(stdout);
    rorcStopDataReceiver(&rr->prorc);

    /* unmap and close RORC */
    printf("12\n");fflush(stdout);
    rorcClose(&rr->prorc);

    /* unmap and close physmem memory */
    printf("13\n");fflush(stdout);
    if (physmemClose(physmem_fd,
                   (unsigned long *)(addr_user),phys_size_physmem))
    {
      return(-1);
    }
    printf("14\n");fflush(stdout);
  }

  return(ret);
}

int
rorcExit()
{
  int ret = 0;

  if(rorcActive)
  {
    rorcActive = 0;

    /* unmap and close physmem memory */
    if (physmemClose(physmem_fd,
                   (unsigned long *)(addr_user),phys_size_physmem))
    {
      return(-1);
    }
  }

  return(ret);
}

#endif
