/*----------------------------------------------------------------------------*
 *  Copyright (c) 1991, 1992  Southeastern Universities Research Association, *
 *                            Continuous Electron Beam Accelerator Facility   *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 * CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606 *
 *      heyes@cebaf.gov   Tel: (804) 249-7030    Fax: (804) 249-7363          *
 *----------------------------------------------------------------------------*
 * Discription: follows this header.
 *
 * Author:
 *	Graham Heyes
 *	CEBAF Data Acquisition Group
 *
 * Revision History:
 *      $Log: CODA_format.c,v $
 *      Revision 2.7  1998/10/13 15:40:53  abbottd
 *      fixed event type mask in decode routines to look only at 1 byte
 *
 *      Revision 2.6  1998/05/27 13:44:57  heyes
 *      add message Q to ROC, improve EB stability on EBD transition
 *
 *      Revision 2.4  1997/04/23 17:33:49  heyes
 *      cmlog added and libmsg removed
 *
 *      Revision 2.3  1996/10/29 19:03:09  heyes
 *      new rcServer
 *
 *      Revision 2.2  1996/09/19 17:16:06  heyes
 *      Support for CODA file format
 *
 *      Revision 2.1  1996/09/19 14:14:08  heyes
 *      Initial release
 *
 *      Revision 1.2  1996/08/29 17:03:58  heyes
 *----------------------------------------------------------------------------*/
#define ERROR -1
#define OK 0
#define NOT_TYPE 1

#include "bosio.h"
#include "etbosio.h"
#include "CODA_format.h"

int 
CODA_reserv_frag(unsigned long **datap, evDesc desc)
{
  /* CODA headers are two long words */

  desc->frag_start = *datap;
  *datap +=2;
  desc->head_length = 0; /* for compartibility with BOS */
}

int 
CODA_reserv_head(unsigned long **datap, evDesc desc)
{
  /* CODA headers are two long words */

  desc->head_start = *datap;
  *datap +=2;
}

int 
CODA_reserv_desc(unsigned long **datap, evDesc desc)
{
  /* CODA ID banks are five long words */

  desc->desc_start = *datap;
  *datap +=5;
}

int 
CODA_decode_frag(unsigned long **datap, evDesc desc)
{
  desc->length = ((*datap)[0] - 1) << 2;
  desc->evnb   = (*datap)[1] & 0xff;
  desc->type   = ((*datap)[1] >> 16) & 0x00ff;
  desc->fragments[0] = &(*datap)[2];
  desc->bankTag[0] = desc->rocid;
  desc->fragLen[0] = desc->length;
  desc->bankCount = 1;
  desc->soe = *datap;

  /* CODA headers are two long words */
  *datap += 2;

  /* fragment headers have a variable format and are hard to 
     check, we assume here that if the length is resonable 
     then so is the rest of the event. NOTE: limit 0.5MBy.
     */
  if (desc->length < 500000) {
    return OK;
  } else {
    return ERROR;
  }
}

int 
CODA_encode_frag(unsigned long **datap, evDesc desc)
{
  /* CODA event lengths are "non inclusive" i.e. don't include
     the length itself */

  desc->frag_start[0] = *datap - desc->frag_start - 1;
  desc->frag_start[1] = ((desc->rocid & 0xffff) << 16) | (desc->evnb & 0xff) | 0x0100;
}

int 
CODA_decode_head(unsigned long **datap, evDesc desc)
{
  unsigned long marker;

  /* The only usefull data here are the total event length and type */

  desc->type   = ((*datap)[1] >> 16 ) & 0x00ff;

  desc->length = ((*datap)[0] - 1) << 2;
  marker = (*datap)[1] & 0xffff;

  /* CODA headers are two long words */

  *datap += 2;

  /* CODA event header must have 0x10CC as low 16-bits */

  if ( marker == 0x10CC ) {
    return OK;
  } else {
    return ERROR;
  }
}

int 
CODA_encode_head(unsigned long **datap, evDesc desc)
{
  /* CODA event lengths are "non inclusive" i.e. don't include
     the length itself */

  desc->head_start[0] = *datap - desc->head_start - 1;
  desc->head_start[1] = 0x10CC | ((desc->type & 0x00ff) << 16);
  desc->length = (*datap - desc->head_start)<<2;
}

int 
CODA_encode_desc(unsigned long **datap, evDesc desc)
{
  /* CODA event lengths are "non inclusive" i.e. don't include
     the length itself */

  desc->desc_start[0] = 4;
  desc->desc_start[1] = 0xC0000100;
  desc->desc_start[2] = desc->evnb;
  desc->desc_start[3] = desc->type;
  desc->desc_start[4] = desc->err[1];
}

int 
CODA_decode_desc(unsigned long **datap, evDesc desc)
{
  unsigned long marker;

  /* CODA event lengths are "non inclusive" i.e. don't include
     the length itself */

  marker = (*datap)[1];
  desc->evnb = (*datap)[2];
 
  desc->type = (*datap)[3];
  desc->err[1] = (*datap)[4];

  *datap += 5;

  if ( marker == 0xC0000100 ) {
    return OK;
  } else {
    return ERROR;
  }  
}

int 
CODA_decode_spec(unsigned long **datap, evDesc desc)
{
  unsigned long marker;

  desc->length = ((*datap)[0] - 1) << 2;

  desc->type  = ((*datap)[1] >> 16) & 0x00ff;

  marker = (*datap)[1] & 0xffff;

  desc->time = (*datap)[2];

  switch(desc->type) {
  case 16:
    {
      desc->syncev = (*datap)[3];
      desc->evnb = (*datap)[4];
      desc->err[1] = (*datap)[5];
      break;
    } 
  case 17: 
    {
      desc->runnb = (*datap)[3];
      desc->runty = (*datap)[4];
      break;
    }
  default: 
    {
      desc->evnb = (*datap)[4];
      break;
    }
  }
  *datap += ((*datap)[0] + 1);

  if ( marker == 0x01CC ) {
    return OK;
  } else {
    return ERROR;
  }  
}

int 
CODA_encode_spec(unsigned long **datap, evDesc desc)
{
  (*datap)[1] = 0x01CC | (desc->type << 16);

  (*datap)[2] = desc->time;

  switch(desc->type) {
  case 16:
    {
      (*datap)[0] = 5;
      (*datap)[3] = desc->syncev;
      (*datap)[4] = desc->evnb;
      (*datap)[5] = desc->err[1];
      break;
    } 
  case 17: 
    {
      (*datap)[0] = 4;
      (*datap)[3] = desc->runnb;
      (*datap)[4] = desc->runty;
      break;
    }
  default: 
    {
      (*datap)[0] = 4;
      (*datap)[3] = 0;
      (*datap)[4] = desc->evnb;
      break;
    }
  }

  desc->length = ((*datap)[0] + 1)<<2;
  *datap += ((*datap)[0] + 1);
}
