
/* bosdvcstrig.c - read DVCS data file and produce output to be used for
DVCS Trigger System simulation */
 
#include <stdio.h>
 
#include "bosio.h"
#define NBCS 700000
#include "bcs.h"

/* print calorimeter image: indexes from 1 */
static void
dvcs_pic1(int hits[24][24])
{
  int i, j;

  printf("\n\n");
  for(i=1; i<24; i++)
  {
    for(j=1; j<24; j++)
	{
      if(hits[i][j]==1) printf("x");
      else              printf("0");
    }
    printf("\n");
  }
  printf("\n\n");

  return;
}

int
main()
{
  FILE *fd;
  int ncol, nrow, nrow1, i, j, l, iev, ind, ind1, ind2, k, n, nw, iret, ix, iy;
  int tmp1 = 1, tmp2 = 2;
  int idl, idh, adc, tdc;
  /*char *str3 = "OPEN INPUT UNIT=1 FILE=\"/work/boiarino/dvcs/dvcs_047213.A00\" ";*/
  /*char *str3 = "OPEN INPUT UNIT=1 FILE=\"/work/boiarino/dvcs/dvcs_047213_pi0.bos\" ";*/
  /*char *str3 = "OPEN INPUT UNIT=1 FILE=\"/work/boiarino/dvcs/clas_047210.A01\" ";*/
  char *str3 = "OPEN INPUT UNIT=1 FILE=\"/work/boiarino/dvcs/egg_47210_pass1.a01.bos\" ";
  unsigned short *buf, *buf1;

  int pedestal[24][24];
  int i1, i2, i3, i4;
  float x5;

  int hits[24][24];

  printf(" bosplit reached !\n");

  bosInit(bcs_.iw,NBCS);


/*
Here is how we decode IC positions from indexes:
      Do ix=1,MaxX
         do iy=1,MaxY
            zpos(ix,iy)=z0
            xpos(ix,iy)=x0+float(12-ix)*xstep
            ypos(ix,iy)=y0+float(iy-12)*ystep
         enddo
      EndDo
here Z0=-11, x0=y0=0, xstep=1.346, ystep=1.36

Definitions of ix and iy from IC bos bank are:
            icid=i16(i)
            iy=int(icid/256)
            ix=icid-iy*256
*/



  /* get pedestals from ASCII file */
  for(i=0;i<24;i++) for(j=0;j<24;j++) {pedestal[i][j]=0; hits[i][j]=0;}
  fd = fopen("/work/boiarino/dvcs/pedmean.tab","r");
  if(fd==NULL)
  {
    printf("Cannot open pedestal file - exit\n");
    exit(0);
  }
  for(;;)
  {
    n = fscanf(fd,"%d%d%d%x%f",&i1,&i2,&i3,&i4,&x5);
    if(n!=5 || n==EOF) break;
    pedestal[i2][i3] = (int)x5;
	/*
    printf("ped-> %d %d %d 0x%04x %f -> pedestal[%2d][%2d]=%4d\n",
      i1,i2,i3,i4,x5,i2,i3,pedestal[i2][i3]);
	*/
  }
  fclose(fd);

  /* looking for IC bank */

  fparm_(str3, strlen(str3));
  printf("# DVCS data event-by-event in following format:\n");
  printf("#  x    y   tdc adc pedestal energy\n");
  for(i=0; i<10000; i++)
  {
    frbos_(bcs_.iw,&tmp1,"E",&iret,1);

    if( (ind2=bosNlink(bcs_.iw,"HEAD",0)) > 0)
	{
      iev = bcs_.iw[ind2+2];
      printf("Event %d\n",iev);
	}
    else
	{
      printf("Event -1 (no HEAD bank)\n");
      continue;
    }

    if( (ind=bosNlink(bcs_.iw,"IC  ",0)) > 0)
    {
      buf = (unsigned short *)&bcs_.iw[ind];

      ncol = bcs_.iw[ind-6];
      nrow = bcs_.iw[ind-5];
      for(l=0; l<nrow; l++)
      {


        /* looking for TDC partner */
        tdc = 0;
	    if( (ind1=bosNlink(bcs_.iw,"IC0 ",0)) > 0)
        { 
          buf1 = (unsigned short *)&bcs_.iw[ind1];
          nrow1 = bcs_.iw[ind1-5];
          for(j=0; j<nrow1; j++)
		  {
            /*printf("ids: 0x%04x 0x%04x\n",buf[0],buf1[0]);*/
            if(buf[0] == buf1[0])
		    {
              tdc = buf1[1];
              /*printf("FOUND MATCH: event %d id=0x%04x tdc=%d\n",i,buf[0],tdc);*/
              break;
		    }
            buf1 += 2;
		  }
        }



        idl = buf[0]&0xFF;
        idh = (buf[0]>>8)&0xFF;

        ix = 12 - idl;
        iy = idh - 12;

        adc = buf[2];
        if(adc>0 /*&& tdc>0*/)
		{
		  /*
          printf("x=%4d (xid=%4d) y=%4d (yid=%4d) -> tdc = %4d, adc = %4d, ped=%d, energy=%d\n",
            ix, idl, iy, idh, tdc, adc, pedestal[idl][idh],adc-pedestal[idl][idh]);
		  */
		  /*
          printf("x=%4d y=%4d -> tdc = %4d, adc = %4d, ped=%4d, energy=%4d\n",
            ix, iy, tdc, adc, pedestal[idl][idh],adc-pedestal[idl][idh]);
		  */
          printf("%4d %4d %4d %4d %4d %6d\n",
            ix, iy, tdc, adc, pedestal[idl][idh],adc-pedestal[idl][idh]);
          hits[idl][idh] = 1;
		}
        buf += 3;
      }


    }

    if(iret == -1 || iret > 0)
    {
      printf(" End-of-File flag, iret =%d\n",iret);
      goto a10;
    }
    else if(iret < 0)
    {
      printf(" Error1, iret =%d\n",iret);
      goto a10;
    }
    if(iret != 0)
    {
      printf(" Error2, iret =%d\n",iret);
    }
    bdrop_(bcs_.iw,"E",1);
    bgarb_(bcs_.iw);
  }
a10:

  fparm_("CLOSE",5);


  /* print calorimeter image 
  dvcs_pic1(hits);
*/

  exit(0);
}










