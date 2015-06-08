/*-----------------------------------------------------------------------------
 * Copyright (c) 1991,1992 Southeastern Universities Research Association,
 *                         Continuous Electron Beam Accelerator Facility
 *
 * This software was developed under a United States Government license
 * described in the NOTICE file included as part of this distribution.
 *
 * CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
 * Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
 *-----------------------------------------------------------------------------
 * 
 * Description:
 *	Utility for CAMAC RPC calls (client program)
 *	
 * Author:  Chip Watson, CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: cnaf.c,v $
 *   Revision 1.1.1.1  1996/08/21 19:18:46  heyes
 *   Imported sources
 *
 *	  Revision 1.1  94/03/16  07:53:26  07:53:26  heyes (Graham Heyes)
 *	  Initial revision
 *	  
 *	  Revision 1.1  94/03/15  11:47:33  11:47:33  heyes (Graham Heyes)
 *	  Initial revision
 *	  
 *	  Revision 1.2  1992/07/14  17:55:02  watson
 *	  Added reporting of X/noX response
 *
 *	  Revision 1.1  1992/06/05  20:26:58  watson
 *	  Initial revision
 *
 * 03-jun-92 cw  replaced atoi calls with sscanf / strtoul
 * 15-nov-91 cw  original coding
 */

#include <stdio.h>
#include <stdlib.h>
#include <ca.h>

int
main(int argc, char *argv[])
{
  int ext,b,c,n,a,f,d,q,lstat,x;
  int stat,nfields;
  char line[80];

  if (argc<2) {
    printf("Usage: cnaf target [c n a f [d]]\n");
    return(0);
  }

  caopen(argv[1],&stat);
  if (stat!=1) {
    printf("failed to establish connection to target machine %s\n",argv[1]);
    printf("Usage: cnaf target [c n a f [d]]\n");
    return(0);
  }

  b=0;

  if ((argc==6)||(argc==7)) {
    sprintf(line,"%s %s %s %s\0",argv[2],argv[3],argv[4],argv[5]);
    if (sscanf(line,"%i%i%i%i",&c,&n,&a,&f) != 4) {
      printf("bad arguments: %s\n",line);
      printf("Usage: cnaf target [c n a f [d]]\n");
      return(0);
    }
    if ((f>=16) && (f<24)) {	/* write function -- data required */
      if (argc!=7) {
	printf("cnaf data missing, 0 assumed\n");
	d = 0;
      } else
	d = strtoul(argv[6],NULL,0);
    } else if (argc!=6) 
      printf("cnaf data argument ignored (non-write function)\n");
    cdreg(&ext,b,c,n,a);
    cfsa(f,ext,&d,&q);
    ctstat(&lstat);
    x = (~lstat>>1)&1;
    if (f<=7)
      printf("lstat=0x%08x -> dec:%d    hex:%x    q:%d  x:%d\n",
        lstat,d,d,q,x);
    else
      printf("q:%d  x:%d\n",q,x);

  } else if (argc==2) {
    printf("successful connection established to %s\n",argv[1]);
    printf("c n a f [d]:");
    while (gets(line)!=NULL) {
      nfields = sscanf(line,"%i%i%i%i%i",&c,&n,&a,&f,&d);
      if ((nfields!=4)&&(nfields!=5))
	printf("bad arguments\n");
      else {
	if ((f>=16) && (f<24)) {	/* write function -- data required */
	  if (nfields!=5) {
	    printf("cnaf data missing, 0 assumed\n");
	    d = 0;
	  }
	} else if (nfields!=4) 
	  printf("cnaf data argument ignored (non-write function)\n");
	cdreg(&ext,b,c,n,a);
	cfsa(f,ext,&d,&q);
	ctstat(&lstat);
	x = (~lstat>>1)&1;
	if (f<=7)
      printf("lstat=0x%08x -> dec:%d    hex:%x    q:%d  x:%d\n",
        lstat,d,d,q,x);
	else
	  printf("q:%d  x:%d\n",q,x);
      }
      printf("c n a f [d]:");
    }
    printf("\n");
  } else {
    printf("Invalid number of arguments\n");
    printf("Usage: cnaf target [c n a f [d]]\n");
  }

}

