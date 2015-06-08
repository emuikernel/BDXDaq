
/* nethist.c - network performance histogramming (Sergey Boyarinov) */

#include <stdio.h>
#include <math.h>
 
#define NWPAWC 1000000 /* Length of the PAWC common block. */
#define LREC 1024      /* Record length in machine words. */

struct {
  float hmemor[NWPAWC];
} pawc_;


main(int argc, char **argv)
{
  FILE *fd;
  char *HBOOKfile = "net.his";
  int nwpawc,lun,lrec,istat,icycle,idn,nbins,nbins1,igood,offset;
  int itmp, ibins, icount;
  float tmp, tmpx, ww, x1, x2;
  int nr,nl,ncol,nrow,i,j,l,l1,l2,ichan,nn,iev,ind,ind1,ind2;
  int status,status1,handle,handle1,k,m;
  int scal,nw,scaler,scaler_old;
  unsigned int hel,str[133],strob,helicity,strob_old,helicity_old;
  int tmp1 = 1, tmp2 = 2, iret, bit1, bit2;
  float *bcsfl, rndm_();

  if(argc != 2)
  {
    printf("Usage: nethist <blaster_filename>\n");
    exit(1);
  }

  nwpawc = NWPAWC;
  hlimit_(&nwpawc);
  lun = 11;
  lrec = LREC;
  hropen_(&lun,"NTUPEL",HBOOKfile,"N",&lrec,&istat,
     strlen("NTUPEL"),strlen(HBOOKfile),1);
  if(istat)
  {
    printf("ERROR: cannot open RZ file %s for writing.\n", HBOOKfile);
    exit(0);
  }
  else
  {
    printf("RZ file >%s< opened for writing, istat = %d\n\n", HBOOKfile, istat);
  }


  nbins=360;
  x1 = 0.;
  x2 = 360.;
  ww = 0.;
  idn = 10;
  
  hbook1_(&idn,"datarate MBYTES PER SECOND",&nbins,&x1,&x2,&ww,26);
  
  /* open blaster file */
  if(fd = fopen(argv[1],"r"))
  {
printf("bla1\n");fflush(stdout);
tmpx = 5.0;
printf("bla2\n");fflush(stdout);

    ibins = 0;
    icount = 0;
    while( (fgets(str,132,fd)) != NULL )
    {
      /*printf("str->%s<\n",str);*/
      sscanf(str,"%i",&itmp);
      tmp = ((float)itmp)/*1000000.*/;
      /*printf("itmp=%d -> tmp=%f\n",itmp,tmp);*/

      if(ibins < nbins)
      {
        idn=10;
        ww = tmp;
		printf("x=%f, w=%f\n",tmpx,ww);
        hf1_(&idn,&tmpx,&ww);
        tmpx = tmpx + 1.0;
        ibins ++;
      }
      icount ++;
    }

    printf("ibins=%d icount=%d -> closing HBOOK file ..\n",
      ibins,icount);fflush(stdout);
    idn = 0;
    hrout_(&idn,&icycle," ",1);
    /*hprint_(&idn);*/
    hrend_("NTUPEL", 6);

    close(fd);
  }
  else
  {
    printf("cannot open file >%s< - exit\n",argv[1]);
  }

  exit(0);
}

