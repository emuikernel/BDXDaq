/* fec2rorc_lib.h */

#ifndef __fec2rorc_h__
#define __fec2rorc_h__

#include "rorc_ddl.h"
#include "rorc_lib.h"

#include "fec2rorc_version.h"
#include "fec2rorc_lib_structs.h"
#include "fec2rorc_RCU.h"


/*  return a channel key pointer that can be used to access a RORC channel. 
 *  revision   - rorc revision (-1 to use serial as minor)
 *  serial     - rorc serial
 *  chan       - rorc channel
 *  return     - this function will always return a chan_key*, 
 *               the user must check that chan_key->status is R_OPEN
 */
chan_key* get_channel (int revision, int serial, int chan);

/*  close down RORC channel and free physmem
 *  return    - 0 on OK, <0 on RORC error, >0 on other error
 */
int release_channel (chan_key *ptr);

/*  write a data block to an area of memory using the RORC
 *  ptr          - chan_key returned by get_channel()
 *  array_ptr    - points to the data to send
 *  num_words    - is the size of the array
 *  fee_address  - is the memory address to write to
 *  return       - 0 on OK, <0 on RORC error, >0 on other error
 */
int ddl_writeBlock (chan_key *ptr, u_long *array_ptr, int num_words, u_long fee_address);

/*  read back a data block from the RORC
 *  ptr          - chan_key returned by get_channel()
 *  fee_address  - is the address at which you would like to start reading
 *  buffer       - is a pointer to some memory to write to, 
 *                 this must be big enough to take the entire read
 *  buffer_size - is the size of the buffer
 *  return      - number of words read on OK, <0 on RORC error 
 */
int ddl_readBlock (chan_key *ptr, u_long fee_address, u_long *buffer, u_long buffer_size);

/*  send command to FEE - FECTRL transaction
 *  ptr          - chan_key returned by get_channel()
 *  fee_address  - is what part of the fee to send to
 *  return       - 0 on OK, <0 on RORC error, >0 on other error
 */
int ddl_sendCommand (chan_key *ptr, u_long fee_address);

/*  send command to FEE - FESTRD transaction
 *  ptr          - chan_key returned by get_channel()
 *  fee_address  - is what part of the fee to send to
 *  return       - >=0 the returned 19bit word, <0 on RORC error  
 */
int ddl_statusReadout (chan_key *ptr, u_long fee_address);

/*  reset ALL FEC's on a RCU
 *  ptr        - chan_key returned by get_channel()
 *  return     - 0 on OK, <0 on RORC error
 */
int rcu_resetFEC (chan_key *ptr);

/*  reset the RCU only, not the attached FECs
 *  ptr        - chan_key returned by get_channel()
 *  return     - 0 on OK, <0 on RORC error
 */
int rcu_resetRCU (chan_key *ptr);

/*  reset the RCU and the FECs.
 *  ptr        - chan_key returned by get_channel()
 *  return     - 0 on OK, <0 on RORC error
 */
int rcu_resetALL (chan_key *ptr); 

/* modify the AFL to turn on a particular FEC attached 
 * to the RCU, the status of other cards is uneffected
 * ptr        - chan_key returned by get_channel()
 * return     - 0 on OK, <0 on RORC error
 */
int rcu_powerOnFEC (chan_key* ptr, int branch, int fec);

/*  modify the AFL to turn off a particular FEC attached 
 *  to the RCU, the status of other cards is uneffected
 *  ptr        - chan_key returned by get_channel()
 *  return     - 0 on OK, <0 on RORC error, >0 on other error
 */
int rcu_powerOffFEC (chan_key* ptr, int branch, int fec);

/*  write data to an address within one of the altro chips onboard the FEC
 *  ptr        - chan_key returned by get_channel()
 *  ch_addr    - FEC channel address
 *  reg_addr   - ALTRO register address
 *  reg_data   - data word to write
 *  return     - 0 on OK, <0 on RORC error
 */
int fec_writeAltroReg (chan_key *ptr, int bcast, int branch, int fec, int chip, 
		       int channel, int reg, int data);

/*  read data to from address within one of the altro chips onboard the FEC
 *  ptr         - chan_key returned by get_channel()
 *  ch_addr     - FEC channel addresss
 *  reg_addr    - ALTRO Register Address
 *  *buffer     - pointer to storage buffer
 *  return      - 0 on OK, <0 on RORC error
 */
int fec_readAltroReg (chan_key *ptr, int branch, int fec, int chip, int channel, 
		      int reg, u_long *buffer);

/*  write data to an address within the board controller onboard the FEC
 *  ptr         - chan_key returned by get_channel()
 *  reg_addr    - board controller register address
 *  return      - 0 on OK, <0 on RORC error
 */
int fec_writeBcReg (chan_key *ptr, int branch, int fec, int reg, int data);

/*  read data to from address within the board controller onboard the FEC
 *  ptr         - chan_key returned by get_channel()
 *  reg_addr    - board controller register address
 *  *reg_data   - pointer to storage buffer
 *  return      - 0 on OK, <0 on RORC error
 */
int fec_readBcReg (chan_key *ptr, int branch, int fec, int reg, u_long *reg_data);

#endif
