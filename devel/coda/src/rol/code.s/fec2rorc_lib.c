/* fec2rorc_lib.c */

#ifdef Linux

#include "rorc_ddl.h"
#include "rorc_lib.h"

#include "fec2rorc_version.h"
#include "fec2rorc_lib.h"

#define RORC_DELAY    200
#define TIMEOUT      3000

#define SIZE_CHECK(a,b) if(a>b) return BAD_PARAMETER;

chan_key rorc_array[MAX_CHAN];
chan_key error_key;

chan_key* get_channel (int revision, int serial, int chan) {
  int ret;
  int cursor;
  int minor;
  int channel;
  u_long phys_offset;
  chan_key *ptr;

  error_key.err = 0;
  ptr = &error_key;
  ptr->status = R_NOT_OPEN_ERR;
 
  if (revision == USE_MINOR) {  // -1
    minor = serial;
    channel = chan;
  } else {
    ret = rorcFind (revision, serial, &minor);
    if (ret != RORC_STATUS_OK) {
      error_key.err = ret; // ERROR
      return &error_key;   
    } else {
      channel = chan;
    }
  }

  /* make an entry in the rorc_array table at position cursor */
  cursor = (minor * 2) + channel;
  ptr = &(rorc_array[cursor]);

  /* store the RORC details */
  ptr->minor = minor;
  ptr->channel = channel;
  phys_offset = (ptr->minor * (2*SEGMENT_SIZE) + (channel * SEGMENT_SIZE));
  
  /* open the RORC channel */
  ret = rorcOpenChannel (&(ptr->rorc), ptr->minor, ptr->channel);
  if (ret != RORC_STATUS_OK) {
    error_key.err = ret; // ERROR
    return &error_key;
  } 
    
  /* reset the RORC channel */
  rorcReset (&(ptr->rorc), RORC_RESET_RORC);
  
  /* store further RORC details */
  ptr->ustimeout = TIMEOUT * ptr->rorc.loop_per_usec;
  
  /* open phsymem with a RORC function */
  ret = physmemOpen (&(ptr->physmem_fd), 
		     &(ptr->addr_user), 
		     &(ptr->phys_addr), 
		     &(ptr->phys_size_physmem));
  if (ret != EXIT_SUCCESS) {
    error_key.err = ret;
    return &error_key; // ERROR
  }
    
  /* store the original values */
  ptr->o_addr_user = (unsigned long *)ptr->addr_user;  
  ptr->o_phys_addr = ptr->phys_addr;
  ptr->o_phys_size_physmem = ptr->phys_size_physmem;    
    
  /* store the offsets */
  ptr->phys_addr += phys_offset;
  ptr->addr_user += phys_offset / sizeof(long); 
  ptr->return_phys = ptr->phys_addr; 
  ptr->return_addr = (unsigned long *)ptr->addr_user; 
  ptr->buffer_phys = ptr->return_phys + 8;
  ptr->buffer_addr = (u_long *) ptr->return_addr + 2;
  ptr->rf = (rorcReadyFifo_t *) ptr->return_addr;
  
  /* setup key for use */
  ptr->status = R_OPEN;
  ptr->err = 0;
    
  return ptr;
}

int release_channel (chan_key *ptr) {
  int ret = 0;

  if (ptr->status == R_CLOSED) 
    return KEY_UNINITIALIZED; // ERROR

  if (ptr->status != R_OPEN) 
    return KEY_UNINITIALIZED; // ERROR
 
  physmemClose (ptr->physmem_fd, ptr->o_addr_user, ptr->o_phys_size_physmem);
  // ERROR

  ret = rorcClose (&(ptr->rorc));
  if (ret != EXIT_SUCCESS) {
    return ret; // ERROR
  }
    
  return 0;
}

int ddl_sendCommand (chan_key *ptr, u_long fee_address) {
  int ret = 0;
  int num_replies = 1; 
    
  ret = ddlSendCommandAndWaitReply (&(ptr->rorc),
				    FECTRL,
				    fee_address,
				    ptr->ustimeout,
				    ptr->stw,
				    num_replies,
				    &(ptr->n_reply));
  
  return ret;
}

int ddl_statusReadout (chan_key *ptr, u_long fee_address) {
  int ret = 0;
  int num_replies = 2; 

  ret = ddlSendCommandAndWaitReply (&(ptr->rorc),
				    FESTRD,
				    fee_address,
				    ptr->ustimeout,
				    ptr->stw,
				    num_replies,
				    &(ptr->n_reply));
  if (ret != RORC_STATUS_OK) {
    return ret;
  } else {
    return ptr->stw[0].part.param;
  } 
}

int ddl_writeBlock (chan_key *ptr, u_long *array_ptr, int num_words, u_long fee_address) {
  int ret = 0;
  int n_reply;
  int step;
    
  if ((ptr->status) != R_OPEN) {
    return R_NOT_OPEN_ERR;
  } 
  
  memcpy (ptr->buffer_addr, array_ptr, num_words*sizeof(int));

  ret = ddlWriteDataBlock(&(ptr->rorc), 
			  ptr->buffer_phys,
			  num_words,
			  ptr->return_phys,
			  ptr->return_addr,
			  fee_address,
			  ptr->ustimeout,
			  ptr->stw,
			  &n_reply,
			  &step);
  return ret;
}

int ddl_readBlock (chan_key *ptr, u_long fee_address, u_long *buffer, u_long buffer_size) {
  int num_words;
  int n_reply;
  int step;
  int ret = 0;
    
  if (ptr->status != R_OPEN) {
    return R_NOT_OPEN_ERR;
  } 
  
  ret = ddlReadDataBlock( &(ptr->rorc),
			  ptr->buffer_phys,
			  ptr->return_phys,
			  ptr->rf,
			  fee_address,
			  ptr->ustimeout,
			  ptr->stw,
			  &n_reply,
			  &step);
  
  if (ret == 0) {
    num_words = ptr->rf[n_reply-1].length;
    if (num_words > buffer_size) 
      return ERR_SMALL_BUFFER;
    memset (buffer, 0x0, buffer_size);
    memcpy (buffer, ptr->buffer_addr, num_words*sizeof(u_long));     
    return num_words;  // good return
  } else {
    return ret;
  }
}


int rcu_resetFEC (chan_key *ptr){
  int ret = ddl_sendCommand (ptr, RCU_FEC_RESET);
  usleep (RORC_DELAY);
  return ret;
}

int rcu_resetRCU (chan_key *ptr){
  int ret = ddl_sendCommand (ptr, RCU_RESET);
  usleep (RORC_DELAY);
  return ret;
}

int rcu_resetALL (chan_key *ptr){
  int ret = ddl_sendCommand (ptr, RCU_GLB_RESET);
  usleep (RORC_DELAY);
  return ret;
}

int rcu_powerOnFEC (chan_key *ptr, int branch, int fec){
  u_long write = 0x1;
  u_long afl = 0x0;
  int ret;

  if (branch != 0 && branch != 1) 
    return BAD_PARAMETER;
  if (branch == 1){
    write = 0x00010000;
  }
    
  /* instruct RCU that we wish to read one word */
  ret = ddl_sendCommand (ptr,(RCU_WRD_RD | 1));
  if (ret != 0) {
    return ret;
  }

  /* read current AFL */
  ret = ddl_readBlock (ptr, RCU_AFL, &afl, RCU_AFL_SZ);
  if (ret != RCU_AFL_SZ) {
    return ret; // ERROR
  }
  
  write = ((write << fec) | afl);    
  ret = ddl_writeBlock (ptr, &write, RCU_AFL_SZ, RCU_AFL);
  if (ret < 0) { 
    return ret; // ERROR
  }
  return 0;
}

int rcu_powerOffFEC (chan_key *ptr, int branch, int fec){
  u_long write = 0x1;
  u_long afl = 0x0;
  int ret;
  
  if (branch != 0 && branch != 1) 
    return -9; // ERROR
  if (branch == 1){
    write = 0x00010000;
  }
  
  /* instruct RCU that we wish to read one word */
  ret = ddl_sendCommand (ptr, (RCU_WRD_RD | 1));
  if (ret != 0) {
    return ret;
  }
  
  /* read current AFL status */
  ret = ddl_readBlock (ptr, RCU_AFL, &afl, RCU_AFL_SZ);
  if (ret != RCU_AFL_SZ) {
    return ret;
  }
  
  write = ~(write<<fec) & afl; 
  ret = ddl_writeBlock (ptr, &write, RCU_AFL_SZ, RCU_AFL);
  if (ret < 0) {
    return ret;
  }
  return 0;
}

int fec_writeAltroReg (chan_key *ptr, int bcast, int branch, int fec, 
		       int chip, int channel, int reg, int data) {
  int word;
  u_long imem[3];
  u_long afl;
  u_long afl_fec;
  int ret;

  /* check paramters */
  SIZE_CHECK (bcast,AL_BCAST);
  SIZE_CHECK (branch,AL_BRANCH);
  SIZE_CHECK (fec,AL_FEC);
  SIZE_CHECK (chip,AL_CHIP);
  SIZE_CHECK (channel,AL_CHAN);
  SIZE_CHECK (reg,AL_REG);
  SIZE_CHECK (data,AL_DATA);
 
  /* instruct RCU that we wish to read one word */
  ret = ddl_sendCommand (ptr,(RCU_WRD_RD | 1));
  if (ret != 0) {
    return ret;
  }

  /* read current AFL */
  ret = ddl_readBlock (ptr, RCU_AFL, &afl, RCU_AFL_SZ);
  if (ret != RCU_AFL_SZ) {
    return ret;
  }
  
  /* check if this FEC is power on */
  afl_fec = (branch) ? 0x00010000 : 0x1;
  afl_fec <<= fec;
  if (afl_fec & afl == 0) {
    return FEC_OFF;
  }

  /* assemble the instructions */
  word = bcast  << 18;
  word |= branch << 16;
  word |= fec << 12;
  word |= chip << 9;
  word |= channel << 5;
  
  imem[0] = (0x00600000 | word) | reg;
  imem[1] =  0x00700000 | data;
  imem[2] =  0x00390000;

  ret = ddl_writeBlock (ptr, imem, 3, RCU_INSTRUCTION_MEM);
  if (ret != 0) {
    return ret; 
  }
  
  ret = ddl_sendCommand (ptr, C_EXEC); 
  if (ret != 0) {
    return ret; 
  }

  return 0;
}

int fec_readAltroReg (chan_key *ptr, int branch, int fec, int chip,
		      int channel, int reg, u_long *buffer) {
  
  u_long imem[2];
  u_long afl;
  u_long afl_fec;
  int ret;
  int word;

  /* check paramters */
  SIZE_CHECK (branch,AL_BRANCH);
  SIZE_CHECK (fec,AL_FEC);
  SIZE_CHECK (chip,AL_CHIP);
  SIZE_CHECK (channel,AL_CHAN);
  SIZE_CHECK (reg,AL_REG);
  
  /* instruct RCU that we wish to read one word */
  ret = ddl_sendCommand (ptr,(RCU_WRD_RD | 1));
  if (ret != 0) {
    return ret;
  }

  /* read current AFL */
  ret = ddl_readBlock (ptr, RCU_AFL, &afl, RCU_AFL_SZ);
  if (ret != RCU_AFL_SZ) {
    return ret;
  }
  
  /* check if this FEC is power on */
  afl_fec = (branch) ? 0x00010000 : 0x1;
  afl_fec <<= fec;
  if (afl_fec & afl == 0) {
    return FEC_OFF;
  }

  /* assemble the instructions */
  word  = branch << 16;
  word |= fec << 12;
  word |= chip << 9;
  word |= channel << 5;
  
  imem[0] = (0x00500000 | word) | reg;
  imem[1] = 0x00390000;

  ret = ddl_writeBlock (ptr, imem, 2, RCU_INSTRUCTION_MEM);
  if (ret != 0) {
    return ret;
  }

  ret = ddl_sendCommand (ptr, C_EXEC);   
  if (ret != 0) {
    return ret; 
  }

  /* read first word of the RCU result memory */
  ret = ddl_readBlock (ptr, RCU_RESULT_MEM, buffer, 1);
  if (ret != 1) {
    return ret; 
  }

  return 0;
}

int fec_writeBcReg (chan_key *ptr, int branch, int fec, int reg, 
		    int data) {
  
  int word = 0;
  u_long imem[3];
  int ret;
 
  SIZE_CHECK (branch,AL_BRANCH);
  SIZE_CHECK (fec,AL_FEC);
  SIZE_CHECK (reg,AL_REG);
  SIZE_CHECK (data,AL_DATA);

  /* assemble the instructions */
  word |= branch << 16;
  word |= fec << 12;
  
  imem[0] = (0x00620000 | word) | reg;
  imem[1] =  0x00700000 | data;
  imem[2] =  0x00390000;
  
  ret = ddl_writeBlock (ptr, imem, 3, RCU_INSTRUCTION_MEM);
  if (ret != 0) {
    return ret;
  }
  
  ret = ddl_sendCommand (ptr, C_EXEC); 
  if (ret != 0) {
    return ret; 
  }

  return 0;
}

int fec_readBcReg (chan_key *ptr, int branch, int fec, int reg, 
		   u_long *reg_data) {
    
  u_long imem[2];
  u_long afl;
  u_long afl_fec;
  int ret;
  int word;

  /* check paramters */
  SIZE_CHECK (branch,AL_BRANCH);
  SIZE_CHECK (fec,AL_FEC);
  SIZE_CHECK (reg,AL_REG);

  /* instruct RCU that we wish to read one word */
  ret = ddl_sendCommand (ptr,(RCU_WRD_RD | 1));
  if (ret != 0) {
    return ret;
  }

  /* read current AFL */
  ret = ddl_readBlock (ptr, RCU_AFL, &afl, RCU_AFL_SZ);
  if (ret != RCU_AFL_SZ) {
    return ret;
  }

  /* check if this FEC is power on */
  afl_fec = (branch) ? 0x00010000 : 0x1;
  afl_fec <<= fec;
  if (afl_fec & afl == 0) {
    return FEC_OFF;
  }

  /* assemble the instructions */
  word  = branch << 16;
  word |= fec << 12;
  
  imem[0] = (0x00520000 | word) | reg;
  imem[1] =  0x00390000;
  
  ret = ddl_writeBlock (ptr, imem, 2, RCU_INSTRUCTION_MEM);
  if (ret != 0) {
    return ret;
  }
  
  ret = ddl_sendCommand (ptr, C_EXEC);   
  if (ret != 0) {
    return ret; 
  }

  /* read first word of the RCU result memory */
  ret = ddl_readBlock (ptr, RCU_RESULT_MEM, reg_data, 1);
  if (ret != 1) {
    return ret;
  }

  return 0;
}

#endif
