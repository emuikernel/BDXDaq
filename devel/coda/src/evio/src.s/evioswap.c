/*
 *  evioswap.c
 *
 *   evioswap() swaps one evio version 2 event
 *       - in place if dest is NULL
 *       - copy to dest if not NULL
 *
 *   swap_int2_t_val() swaps one int32_t, call by val
 *   swap_int32_t() swaps an array of uint32_t's
 *
 *   thread safe
 *
 *
 *   Author: Elliott Wolin, JLab, 21-nov-2003
 *
 *   Notes:
 *     simple loop in swap_xxx takes 50% longer than pointers and unrolled loops
 *     -O is over twice as fast as -g
 *
 *   To do:
 *     use in evio.c, replace swap_util.c
 *
 *   Author: Carl Timmer, JLab, jan-2012
 *      - add doxygen documentation
 *      - add comments & beautify
 *      - simplify swapping routines
 *      - make compatible with evio version 4 (padding info in data type)
 *
 */


/**
 * ################################
 * COMPOSITE DATA:
 * ################################
 *   This is a new type of data (value = 0xf) which originated with Hall B.
 *   It is a composite type and allows for possible expansion in the future
 *   if there is a demand. Basically it allows the user to specify a custom
 *   format by means of a string - stored in a tagsegment. The data in that
 *   format follows in a bank. The routine to swap this data must be provided
 *   by the definer of the composite type - in this case Hall B. The swapping
 *   function is plugged into this evio library's swapping routine.
 *   Here's what it looks like.
 *
 * MSB(31)                          LSB(0)
 * <---  32 bits ------------------------>
 * _______________________________________
 * |  tag    | type |    length          | --> tagsegment header
 * |_________|______|____________________|
 * |        Data Format String           |
 * |                                     |
 * |_____________________________________|
 * |              length                 | \
 * |_____________________________________|  \  bank header
 * |       tag      |  type   |   num    |  /
 * |________________|_________|__________| /
 * |               Data                  |
 * |                                     |
 * |_____________________________________|
 *
 *   The beginning tagsegment is a normal evio tagsegment containing a string
 *   (type = 0x3). Currently its type and tag are not used - at least not for
 *   data formatting.
 *   The bank is a normal evio bank header with data following.
 *   The format string is used to read/write this data so that takes care of any
 *   padding that may exist. As with the tagsegment, the tag, type, & num are ignored.
 */


/* include files */
#include <evio.h>
#include <stdlib.h>
#include <stdio.h>


// from Sergey's composite swap library
int eviofmt(char *fmt, unsigned char *ifmt, int ifmtLen);
int eviofmtswap(int *iarr, int nwrd, unsigned char *ifmt, int nfmt, int tolocal, int padding);



/* entry points */
void evioswap(uint32_t *buffer, int tolocal, uint32_t*dest);
int32_t swap_int32_t_value(int32_t val);
uint32_t *swap_int32_t(uint32_t *data, unsigned int length, uint32_t *dest);


/* internal prototypes */
/*static*/ void swap_bank(uint32_t *buf, int tolocal, uint32_t *dest);
/*static*/ void swap_segment(uint32_t *buf, int tolocal, uint32_t *dest);
/*static*/ void swap_tagsegment(uint32_t *buf, int tolocal, uint32_t *dest);
/*static*/ void swap_data(uint32_t *data, int type, int length, int tolocal, uint32_t *dest);
/*static*/ void swap_int64_t(uint64_t *data, int length, uint64_t *dest);
/*static*/ void swap_short(uint16_t *data, int length, uint16_t *dest);
/*static*/ void copy_data(uint32_t *data, int length, uint32_t *dest);
/*static*/ void swap_composite_t(uint32_t *data, int tolocal, uint32_t *dest);


/*--------------------------------------------------------------------------*/

/**
 * Routine to swap the endianness of an evio event (bank).
 *
 * @param buf     buffer of evio event data to be swapped
 * @param tolocal if 0 buf contains data of same endian as local host,
 *                else buf has data of opposite endian
 * @param dest    buffer to place swapped data into.
 *                If this is NULL, then dest = buf.
 */
void
evioswap(uint32_t *buf, int tolocal, uint32_t *dest)
{

  swap_bank(buf, tolocal, dest);

  return;
}



/**
 * Routine to swap the endianness of an evio bank.
 *
 * @param buf     buffer of evio bank data to be swapped
 * @param tolocal if 0 buf contains data of same endian as local host,
 *                else buf has data of opposite endian
 * @param dest    buffer to place swapped data into.
 *                If this is NULL, then dest = buf.
 */
/*static*/ void
swap_bank(uint32_t *buf, int tolocal, uint32_t *dest) {

    uint32_t data_length, data_type;
    uint32_t *p=buf;

    /* Swap header to get length and contained type if buf NOT local endian */
    if (tolocal) {
        p = swap_int32_t(buf, 2, dest);
    }
    
    data_length  =  p[0]-1;
    data_type    = (p[1] >> 8) & 0x3f; /* padding info in top 2 bits of type byte */

    /* Swap header if buf is local endian */
    if (!tolocal) {
        swap_int32_t(buf, 2, dest);
    }
    
    /* Swap non-header bank data */
    swap_data(&buf[2], data_type, data_length, tolocal, ((dest==NULL) ? NULL: &dest[2]));

    return;
}



/**
 * Routine to swap the endianness of an evio segment.
 *
 * @param buf     buffer of evio segment data to be swapped
 * @param tolocal if 0 buf contains data of same endian as local host,
 *                else buf has data of opposite endian
 * @param dest    buffer to place swapped data into.
 *                If this is NULL, then dest = buf.
 */
/*static*/ void
swap_segment(uint32_t *buf, int tolocal, uint32_t *dest)
{

    uint32_t data_length,data_type;
    uint32_t *p=buf;

    /* Swap header to get length and contained type if buf NOT local endian */
    if (tolocal) {
        p = swap_int32_t(buf, 1, dest);
    }
  
    data_length  =  p[0] & 0xffff;
    data_type    = (p[0] >> 16) & 0x3f; /* padding info in top 2 bits of type byte */

    /* Swap header if buf is local endian */
    if (!tolocal) {
        swap_int32_t(buf, 1, dest);
    }
  
    /* Swap non-header segment data */
    swap_data(&buf[1], data_type, data_length, tolocal, ((dest==NULL) ? NULL : &dest[1]));
  
    return;
}



/**
 * Routine to swap the endianness of an evio tagsegment.
 *
 * @param buf     buffer of evio tagsegment data to be swapped
 * @param tolocal if 0 buf contains data of same endian as local host,
 *                else buf has data of opposite endian
 * @param dest    buffer to place swapped data into.
 *                If this is NULL, then dest = buf.
 */
/*static*/ void
swap_tagsegment(uint32_t *buf, int tolocal, uint32_t *dest)
{
    uint32_t data_length,data_type;
    uint32_t *p=buf;

    /* Swap header to get length and contained type if buf NOT local endian */
    if (tolocal) {
        p = swap_int32_t(buf,1,dest);
    }
    
    data_length  =  p[0] & 0xffff;
    data_type    = (p[0] >> 16) & 0xf; /* no padding info in tagsegments */

    /* Swap header if buf is local endian */
    if (!tolocal) {
        swap_int32_t(buf,1,dest);
    }
    
    /* Swap non-header tagsegment data */
    swap_data(&buf[1], data_type, data_length, tolocal, ((dest==NULL)? NULL : &dest[1]));
  
    return;
}



/**
 * Routine to swap any type of evio data.
 *
 * @param data    buffer of evio data to be swapped
 * @param type    type of evio data
 * @param length  length of evio data in 32 bit words
 * @param tolocal if 0 data is of same endian as local host,
 *                else data is of opposite endian
 * @param dest    buffer to place swapped data into.
 *                If this is NULL, then dest = data.
 */
/*static*/ void
swap_data(uint32_t *data, int type, int length, int tolocal, uint32_t *dest)
{
    uint32_t fraglen;
    uint32_t l=0;


    /* Swap the data or call swap_fragment */
    switch (type) {

        /* unknown type ... no swap */
        case 0x0:
            copy_data(data, length, dest);
            break;


        /* 32-bit types: uint, float, or int */
        case 0x1:
        case 0x2:
        case 0xb:
            swap_int32_t(data, length, dest);
            break;


        /* 8-bit types: string array, char, or uchar ... no swap */
        case 0x3:
        case 0x6:
        case 0x7:
            copy_data(data, length, dest);
            break;


        /* 16-bit types: short or ushort */
        case 0x4:
        case 0x5:
            swap_short((uint16_t*)data, length*2, (uint16_t*)dest);
            break;


        /* 64-bit types: double, int, or uint */
        case 0x8:
        case 0x9:
        case 0xa:
            swap_int64_t((uint64_t*)data, length/2, (uint64_t*)dest);
            break;


        /* Composite type */
        case 0xf:
            swap_composite_t((uint32_t*)data, tolocal, (uint32_t*)dest);
            break;
            

        /* bank */
        case 0xe:
        case 0x10:
            while (l < length) {
                /* data is opposite local endian */
                if (tolocal) {
                    /* swap bank */
                    swap_bank(&data[l], tolocal, (dest==NULL) ? NULL : &dest[l]);
                    /* bank was this long (32 bit words) including header */
                    fraglen = (dest==NULL) ? data[l]+1: dest[l]+1;
                } else {
                    fraglen = data[l] + 1;
                    swap_bank(&data[l], tolocal, (dest==NULL) ? NULL : &dest[l]);
                }
                l += fraglen;
            }
            break;


        /* segment */
        case 0xd:
        case 0x20:
            while (l < length) {
                if (tolocal) {
                    swap_segment(&data[l], tolocal, (dest==NULL) ? NULL : &dest[l]);
                    fraglen = (dest==NULL) ? (data[l]&0xffff)+1: (dest[l]&0xffff)+1;
                } else {
                    fraglen = (data[l] & 0xffff) + 1;
                    swap_segment(&data[l], tolocal, (dest==NULL) ? NULL : &dest[l]);
                }
                l += fraglen;
            }
            break;


        /* tagsegment, NOTE: val of 0x40 is no longer used for tagsegment - needed for padding */
        case 0xc:
            while (l < length) {
                if (tolocal) {
                    swap_tagsegment(&data[l], tolocal, (dest==NULL) ? NULL : &dest[l]);
                    fraglen = (dest==NULL)?(data[l]&0xffff)+1:(dest[l]&0xffff)+1;
                } else {
                    fraglen = (data[l] & 0xffff) + 1;
                    swap_tagsegment(&data[l], tolocal, (dest==NULL) ? NULL : &dest[l]);
                }
                l += fraglen;
            }
            break;


        /* unknown type, just copy */
        default:
            copy_data(data, length, dest);
            break;
    }

    return;
}



/**
 * This routine swaps the bytes of a 32 bit integer and
 * returns the swapped value. Rewritten to be a wrapper
 * for EVIO_SWAP32(x) macro.
 *
 * @param val int val to be swapped
 * @return swapped value
 * @deprecated Use the EVIO_SWAP32(x) macro instead of this routine.
 */
int32_t
swap_int32_t_value(int32_t val)
{
  return EVIO_SWAP32(val);
}


/**
 * This routine swaps a single buffer of composite type.
 *
 * @param data    pointer to data to be swapped
 * @param tolocal if 0 data is of same endian as local host,
 *                else data is of opposite endian
 * @param dddd    pointer to where swapped data is to be copied to.
 *                If NULL, the data is swapped in place.
 */

#undef DEBUG

/*static*/ void
swap_composite_t(uint32_t *data, int tolocal, uint32_t *dddd)
{
    int formatLen, dataLen;
    char *formatString;
    uint32_t *pData, *dest;
    int nfmt, padding;
    unsigned char ifmt[1024];

    /* swap in place or copy ? */
    dest = (dddd == NULL) ? data : dddd;
    
    /* location of incoming data */
    pData = data;
    
    /* swap format tagsegment header word */
    if (tolocal) {
        pData = swap_int32_t(data, 1, dest);
    }
#ifdef DEBUG
printf("swap: 1: tagsegment header 0x%08x\n",pData[0]);fflush(stdout);
#endif

    /* get length of format string (in 32 bit words) */
    formatLen = pData[0] & 0xffff;

    if (!tolocal) {
        swap_int32_t(data, 1, dest);
    }

    /* copy if needed */
    if (dddd != NULL) {
        copy_data(&data[1], formatLen, &dest[1]);
    }
            
    /* set start of format string */
    formatString = (char*)(&pData[1]);

    /* swap data bank header words */
    pData = &data[formatLen+1];
    if (tolocal) {
        pData = swap_int32_t(&data[formatLen+1], 2, &dest[formatLen+1]);
    }
                              
    /* get length of composite data (bank's len - 1)*/
    dataLen = pData[0] - 1;
    padding = (pData[1]>>14)&0x3;
#ifdef DEBUG
printf("swap: 6: dataLen=%d, padding=%d\n",dataLen,padding);fflush(stdout);
#endif

    if (!tolocal) {
        swap_int32_t(&data[formatLen+1], 2, &dest[formatLen+1]);
    }

    /* copy if needed */
    if (dddd != NULL) {
        copy_data(&data[formatLen+3], dataLen, &dest[formatLen+3]);
    }
  
    /* get start of composite data, will be swapped in place */
    pData = &(dest[formatLen+3]);

#ifdef DEBUG
printf("swap: 91 >%s<\n",formatString);fflush(stdout);
#endif

    /* swap composite data: convert format string to internal format, then call formatted swap routine */
    if ((nfmt = eviofmt(formatString, ifmt, 1024)) > 0 ) {
#ifdef DEBUG
printf("swap: 10: dataLen=%d nfmt=%d pData[dataLen] -> 0x%08x 0x%08x 0x%08x 0x%08x\n",dataLen,nfmt,pData[dataLen-2],pData[dataLen-1],pData[dataLen],pData[dataLen+1]);fflush(stdout);
#endif
        if (eviofmtswap(pData, dataLen, ifmt, nfmt, tolocal, padding)) {
#ifdef DEBUG
printf("swap: 11\n");fflush(stdout);
#endif
            printf("swap_composite_t: eviofmtswap returned error, bad arg(s)\n");
        }
    }
    else {
        printf("swap_composite_t: error %d in eviofmt\n", nfmt);
    }
#ifdef DEBUG
printf("swap: 99: dataLen=%d nfmt=%d pData[dataLen] -> 0x%08x 0x%08x 0x%08x 0x%08x\n",dataLen,nfmt,pData[dataLen-2],pData[dataLen-1],pData[dataLen],pData[dataLen+1]);fflush(stdout);
#endif
}



/**
 * This routine swaps a buffer of 32 bit integers.
 *
 * @param data   pointer to data to be swapped
 * @param length number of 32 bit ints to be swapped
 * @param dest   pointer to where swapped data is to be copied to.
 *               If NULL, the data is swapped in place.
 * @return pointer to beginning of swapped data
 */
uint32_t *
swap_int32_t(uint32_t *data, unsigned int length, uint32_t *dest)
{
    int i;

    if (dest == NULL) {
        dest = data;
    }

#ifdef DEBUG
printf("swap: 431: length=%d\n",length);fflush(stdout);
#endif
    for (i=0; i < length; i++) {
#ifdef DEBUG
printf("swap: 432: [%d] 0x%08x\n",i,data[i]);fflush(stdout);
#endif
        dest[i] = EVIO_SWAP32(data[i]);
#ifdef DEBUG
printf("swap: 433: [%d] 0x%08x\n",i,dest[i]);fflush(stdout);
#endif
    }
    
    return(dest);
}



/**
 * This routine swaps a buffer of 64 bit integers.
 *
 * @param data   pointer to data to be swapped
 * @param length number of 64 bit ints to be swapped
 * @param dest   pointer to where swapped data is to be copied to.
 *               If NULL, the data is swapped in place.
 */
/*static*/ void
swap_int64_t(uint64_t *data, int length, uint64_t *dest)
{
    int i;

    if (dest == NULL) {
        dest = data;
    }

    for (i=0; i < length; i++) {
        dest[i] = EVIO_SWAP64(data[i]);
    }
}

 
 
/**
 * This routine swaps a buffer of 16 bit integers.
 *
 * @param data   pointer to data to be swapped
 * @param length number of 16 bit ints to be swapped
 * @param dest   pointer to where swapped data is to be copied to.
 *               If NULL, the data is swapped in place.
 */
/*static*/ void
swap_short(uint16_t *data, int length, uint16_t *dest)
{
    int i;

    if (dest == NULL) {
        dest = data;
    }

    for (i=0; i < length; i++) {
        dest[i] = EVIO_SWAP16(data[i]);
    }
}



/**
 * This routine swaps nothing, it just copies the given number of 32 bit ints.
 *
 * @param data   pointer to data to be copied
 * @param length number of 32 bit ints to be copied
 * @param dest   pointer to where data is to be copied to.
 *               If NULL, nothing is done.
 */
/*static*/ void
copy_data(uint32_t *data, int length, uint32_t *dest)
{   
  int i;
  
  if (dest == NULL) {
      return;
  }
  
  for (i=0; i<length; i++) {
      dest[i] = data[i];
  }
}
 
