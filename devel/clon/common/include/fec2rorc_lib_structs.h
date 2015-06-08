/* fec2rorc_lib_structs.h */

#ifndef __fec2rorc_structs__
#define __fec2rorc_structs__

#define R_OPEN          -2049
#define R_CLOSED        -2050
#define R_NOT_OPEN_ERR  -2051

#define ERR_SMALL_BUFFER   -3000
#define KEY_UNINITIALIZED  -3001
#define BAD_PARAMETER      -3002
#define FEC_OFF            -3003

/* 8 MB to be allocated per channel */
#define SEGMENT_SIZE (1024*1024*8)
#define MAX_CHAN 12

typedef unsigned long u_long; 

typedef struct{
  int minor;
  int channel;
  int physmem_fd;
  int status;
    
  rorc_pci_dev_t rorc;
  rorcReadyFifo_t *rf;
  int err;
  int n_reply;
  stword_t stw[DDL_MAX_REPLY];
  unsigned long ustimeout;
    
  /* offset values */    
  volatile unsigned long *addr_user;
  unsigned long *return_addr;
  unsigned long *buffer_addr;

  /* original values - used for unmapping */
  unsigned long *o_addr_user;
  unsigned long o_phys_addr;
  unsigned long o_phys_size_physmem;

  unsigned long phys_offset;
  unsigned long phys_addr;
  unsigned long phys_size_physmem;
  unsigned long buffer_phys;
  unsigned long return_phys;

} chan_key;

#endif
