
/* sglib.c - main functions of the segment finding library */


#include <stdio.h>
#include <string.h>
#include <limits.h>
/*#include "bos.h"*/
#include "prlib.h"
#include "sglib.h"
#include "sdakeys.h"
SDAkeys sdakeys_;

#define DEBUG 0

static unsigned long nlayer1[6][3][2];
static unsigned long nwires1[6][3][2];

void
sginit()
{
  int i, j, sec, rgn, supl, numlay, numwire;

  /* number of layers/wires */
  for(sec=0; sec<6; sec++)
  {
    for(rgn=0; rgn<3; rgn++)
    {
      for(supl=0; supl<2; supl++)
      {
        if(rgn==0 && supl==0) numlay=4;
        else                  numlay=6;
        if(rgn==0)
        {
          if(supl==0) numwire=130;
          else        numwire=142;
        }
        else
        {
          if(supl==0) numwire=189;
          else        numwire=192;
        }

        nlayer1[sec][rgn][supl] = numlay;
        nwires1[sec][rgn][supl] = numwire;
      }
    }
  }

  /* create differential shift table and fill lookup tables */
  AxialPlusStereoInit();
  printf("sginit completed\n");fflush(stdout);

  return;
}


/* DO WE NEED THAT ??? */
void
sggen()
{
  int i, j;
  FILE *fd;

  printf("sggen reached\n");

  /* create shifts.h file */
  if((fd=fopen("shifts.h","w")) != NULL)
  {
    fprintf(fd,"#define SHIFTS(XXX) \\\n");
    fprintf(fd,"\\\n/* shifts.h - generated automaticaly */\\\n\\\n");
    fprintf(fd,
      "stat1|=SuperLayer##XXX(axwork,&mask1[ 0],tmp1[ 0]); \\\n");
    fprintf(fd,
      "stat2|=SuperLayer##XXX(stwork,&mask2[ 0],tmp2[ 0]); \\\n");
    fprintf(fd,"\\\n");
/*
    for(i=1; i<NSHIFT; i++)
    {
      for(j=0; j<5; j++)
      {
        if(dshift[i][j] > 0)
        {
          fprintf(fd,"LeftShiftWord##XXX##_%02d(&axwork[%1d]); \\\n",
            dshift[i][j],j+1);
          fprintf(fd,"LeftShiftWord##XXX##_%02d(&stwork[%1d]); \\\n",
            dshift[i][j],j+1);
        }
        else if(dshift[i][j] < 0)
        {
          fprintf(fd,"RightShiftWord##XXX##_%02d(&axwork[%1d]); \\\n",
            -dshift[i][j],j+1);
          fprintf(fd,"RightShiftWord##XXX##_%02d(&stwork[%1d]); \\\n",
            -dshift[i][j],j+1);
        }
      }
      fprintf(fd,
        "stat1|=SuperLayer##XXX(axwork,&mask1[%2d],tmp1[%2d]); \\\n",i,i);
      fprintf(fd,
        "stat2|=SuperLayer##XXX(stwork,&mask2[%2d],tmp2[%2d]); \\\n",i,i);
      fprintf(fd,"\\\n");
      printf("%3d >>> %2d %2d %2d %2d %2d\n",i,dshift[i][0],
                          dshift[i][1],dshift[i][2],dshift[i][3],dshift[i][4]);
    }
*/
    fclose(fd);
  }

  return;
}

/* NOT COMPLETED ... */
void
sginitfun(PRSEC sgm)
{
  int irg, iax, ist, itmp;

  for(irg=0; irg<3; irg++)
  {
    iax = sdakeys_.lanal[irg*2+2];
    ist = sdakeys_.lanal[irg*2+3];
    if(irg==0)
    {
      itmp = iax;
      iax = ist;
      ist = itmp;
    }
	/*
    printf("irg=%d\n",irg); fflush(stdout);
    printf("  adr=0x%08x\n",(int)&slmin4); fflush(stdout);
    printf("  0x%08x\n",(int)slmin4); fflush(stdout);
    printf("    adr=0x%08x\n",(int)&sgm.rg[irg].axmin); fflush(stdout);
    printf("    0x%08x\n",(int)sgm.rg[irg].axmin); fflush(stdout);
    */

/*
    sgm.rg[irg].axmin = slmin4;
    sgm.rg[irg].stmin = slmin4;
    sgm.rg[irg].rgmin = rgmin8;
*/

	/*
    if(iax<3)       sgm.rg[irg].axmin = slmin2;
    else if(iax==3) sgm.rg[irg].axmin = slmin3;
    else if(iax>3)  sgm.rg[irg].axmin = slmin4;
    if(ist<3)       sgm.rg[irg].stmin = slmin2;
    else if(ist==3) sgm.rg[irg].stmin = slmin3;
    else if(ist>3)  sgm.rg[irg].stmin = slmin4;
    */
  }

  return;
}






#define GETEV \
    /* copy data to bitmap arrays for segment finding */ \
    memset(data,0,36*sizeof(Word192)); \
    hits0 = bosDC0; \
    for(i=0; i<bosNumDC0; i++) \
    { \
      BREAKSHORT(hits0->id, wire, layer); \
      if(GOODHIT) \
      { \
        /* layer is 1..36 and must be converted */ \
        wire--; \
        layer--; \
        rgn = layer / 12; \
        supl = (layer % 12) / 6;  /* the 0..1 variety */ \
        lay = layer % 6; \
        /*printf("set: rgn=%1d supl=%1d lay=%2d wire=%3d -> 1\n", \
        rgn,supl,lay,wire);*/ \
        SetBitWord192(&data[rgn][supl][lay], wire); \
      } \
      hits0++; \
    }


/* return 0 if sector does not have required track information,
 otherwise returns the number of superlayers with segments */
int
sgtrigger(int *jw, int isec, int nsl, int layershifts[6])
{
  unsigned int tempDC[10000];
  Word192 data[3][2][6];  /* rgn, supl, layer */
  DC0DataPtr hits0, hits1, bosDC0;
  int bosNumDC0, i, slcount, ret, sec, ind, nd, rgn, supl, lay, nlay, nwire;
  int iax, ih;
  unsigned char layer, wire;

  /* check if bank exist and have a data */
  sec = isec-1;
  if((ind = etNlink(jw,"DC0 ",sec+1)) <= 0)
  {
	/*printf("slcount1=%d\n",slcount);fflush(stdout);*/
    return(0);
  }
  if((nd=etNdata(jw,ind)) <= 0)
  {
	/*printf("slcount2=%d\n",slcount);fflush(stdout);*/
    return(0);
  }

  /* set data pointers */
  bosNumDC0 = nd * sizeof(int) / sizeof(DC0Data);
  bosDC0 = (DC0DataPtr)&jw[ind];


  GETEV;

  /* remove noise */
  if(DEBUG) printf("1: isec %d\n",isec);
  slcount = 6;
  for(rgn=0; rgn<3; rgn++)
  {
    if(DEBUG) printf("rgn=%d\n",rgn);
    for(supl=0; supl<2; supl++)
    {
      nlay  = nlayer1[sec][rgn][supl];
      nwire = nwires1[sec][rgn][supl];

      /*
      printf("befor\n");
      for(lay=0; lay<6; lay++) PrintWord192(&data[rgn][supl][lay]);
      */

      ret = RemoveNoise(data[rgn][supl], nlay, nwire, layershifts);

      if(ret == 0) slcount --; /* bag SuperLayer */
      if(slcount < nsl) /* the number of SL required */
      {
        /*printf("bad sector %d\n",sec+1);*/
		/*printf("slcount3=%d\n",slcount);fflush(stdout);*/
        return(0);
      }
      /*
      printf("after ret=%d\n",ret);
      for(lay=0; lay<6; lay++) PrintWord192(&data[rgn][supl][lay]);
      */
    }
  }

  /*printf("slcount4=%d\n",slcount);fflush(stdout);*/
  return(slcount);
}


/* rewrites all DC banks removing single hits */
int
sgremovenoise(int *jw, int isec, int nsl, int layershifts[6])
{
  unsigned int tempDC[10000];
  Word192 data[3][2][6];  /* rgn, supl, layer */
  DC0DataPtr hits0, hits1, bosDC0;
  int bosNumDC0, i, slcount, ret, sec, ind, nd, rgn, supl, lay, nlay, nwire;
  int iax, ih;
  unsigned char layer, wire;

  if(DEBUG) printf("sgremovenoise() reached\n");

 /* check if bank exist and have a data */
  sec = isec-1;
  if((ind = etNlink(jw,"DC0 ",sec+1)) <= 0) return;
  if((nd=etNdata(jw,ind)) <= 0) return;

  /* set data pointers */
  bosNumDC0 = nd * sizeof(int) / sizeof(DC0Data);
  bosDC0 = (DC0DataPtr)&jw[ind];


  if(DEBUG) printf("sgremovenoise() befor GETEV\n");
  GETEV;
  if(DEBUG) printf("sgremovenoise() after GETEV\n");

  /* remove noise */
  if(DEBUG) printf("1: isec %d\n",isec);
  slcount = 6;
  for(rgn=0; rgn<3; rgn++)
  {
    if(DEBUG) printf("rgn=%d\n",rgn);
    for(supl=0; supl<2; supl++)
    {
      nlay  = nlayer1[sec][rgn][supl];
      nwire = nwires1[sec][rgn][supl];

      /*
      printf("befor\n");
      for(lay=0; lay<6; lay++) PrintWord192(&data[rgn][supl][lay]);
      */

      ret = RemoveNoise(data[rgn][supl], nlay, nwire, layershifts);

      if(ret == 0) slcount --; /* bag SuperLayer */
      if(slcount < nsl) /* the number of SL required */
      {
        /*printf("bad sector %d\n",sec+1);*/
        /*return(1);*/
        ;
      }
      /*
      printf("after ret=%d\n",ret);
      for(lay=0; lay<6; lay++) PrintWord192(&data[rgn][supl][lay]);
      */

    }
  }

  /* cleanup original data banks if noise removing */
  hits0 = bosDC0;
  for(i=0; i< bosNumDC0; i++)
  {
    BREAKSHORT(hits0->id, wire, layer);
    if(GOODHIT)
    {
      /* layer is 1..36 and must be converted */
      wire--;
      layer--;
      rgn = layer / 12;
      supl = (layer % 12) / 6;  /* the 0..1 variety */
      lay = layer % 6;

      /* returns 'False' if corresponding bit 'wire' unset */
      /* in that case will set id=0 - will be ignored by GETDCDATA */
      if(!CheckBitWord192(&data[rgn][supl][lay], wire)) hits0->id = 0;
    }
    hits0++;
  }



  /*********************/
  /* recreate DC0 bank */



  /*Sergey: CED does not like it ...........*/
  return(slcount);



  /* move all non-zero hits up in existing bank */
  hits0 = hits1 = bosDC0;
  for(i=0; i< bosNumDC0; i++)
  {
    if(hits0->id > 0)
    {
      *hits1++ = *hits0;
    }
    hits0++;
  }
  nd = ( (int)hits1-(int)&jw[ind]+1 ) / 4;
  memcpy((char *)tempDC, (char *)&jw[ind], nd * 4);
  etNdrop(jw,"DC0 ",isec);
  ind = etNcreate(jw,"DC0 ",isec,2,nd);
  memcpy((char *)&jw[ind], (char *)tempDC, nd * 4);

  return(slcount);
}



/* segment finding for one sector

      opt = 1 - noise removing only
          = 2 - segment finding only
          = 3 - noise removing and then segment finding
*/

void
sglib(int *jw, int isec, int opt, int *nsgm, PRSEC *sgm)
{
  /* get it from segment dictionary - take 'max' segment */
  static int layershifts[6] = {0, 2, 3, 3, 4, 4};
  /*static int layershifts[6] = {0, 5, 6, 7, 8, 9};*/

  Word192 data[3][2][6];  /* rgn, supl, layer */
  DC0DataPtr hits0, bosDC0;
  int bosNumDC0, i, slcount, ret, sec, ind, nd, rgn, supl, lay, nlay, nwire;
  int iax, ih;
  unsigned char layer, wire;

  *nsgm = 0;
  sec = isec-1;
/*if(isec!=2) return;*/
  sgm->rg[0].nax = sgm->rg[1].nax = sgm->rg[2].nax = 0;
  sgm->rg[0].nst = sgm->rg[1].nst = sgm->rg[2].nst = -1;

  /* check if bank exist and have a data */
  if((ind = etNlink(jw,"DC0 ",sec+1)) <= 0) return;
  if((nd=etNdata(jw,ind)) <= 0) return;

  /* set data pointers */
  bosNumDC0 = nd * sizeof(int) / sizeof(DC0Data);
  bosDC0 = (DC0DataPtr)&jw[ind];

  if(opt==1 || opt==3)
  {
    GETEV;

    /* remove noise */
    if(DEBUG) printf("2-------------------------> isec %d <----------\n",isec);
    slcount = 6;
    for(rgn=0; rgn<3; rgn++)
    {
      if(DEBUG) printf("rgn=%d\n",rgn);
      for(supl=0; supl<2; supl++)
      {
        nlay  = nlayer1[sec][rgn][supl];
        nwire = nwires1[sec][rgn][supl];

        /*
        printf("befor\n");
        for(lay=0; lay<6; lay++) PrintWord192(&data[rgn][supl][lay]);
        */

        ret = RemoveNoise(data[rgn][supl], nlay, nwire, layershifts);

        if(ret == 0) slcount --; /* bag SuperLayer */
        if(slcount < (int)sdakeys_.zcut[2])
        {
          /*printf("bad sector %d\n",sec+1);*/
          return;
        }
        /*
        printf("after ret=%d\n",ret);
        for(lay=0; lay<6; lay++) PrintWord192(&data[rgn][supl][lay]);
        */

      }
    }

    /* cleanup original data banks if noise removing */
    hits0 = bosDC0;
    for(i=0; i< bosNumDC0; i++)
    {
      BREAKSHORT(hits0->id, wire, layer);
      if(GOODHIT)
      {
        /* layer is 1..36 and must be converted */
        wire--;
        layer--;
        rgn = layer / 12;
        supl = (layer % 12) / 6;  /* the 0..1 variety */
        lay = layer % 6;

        /* returns 'False' if corresponding bit 'wire' unset */
        /* in that case will set id=0 - will be ignored by GETDCDATA */
        if(!CheckBitWord192(&data[rgn][supl][lay], wire)) hits0->id = 0;
      }
      hits0++;
    }
  }


  /* segment finding */
  if(opt==2 || opt==3)
  {
    GETEV;

    /* segments finding */
    if(DEBUG) printf("3---------------------------> isec %d <--------\n",isec);
    for(rgn=2; rgn>=0; rgn--)
    {
      int nclust;
      if(DEBUG) {printf("rgn=%d\n",rgn);fflush(stdout);}

      if(rgn==0) /* just to enforce 2-hit segments in R1 stereo ... */
        SegmentSearch128(data[rgn][0], data[rgn][1], &nclust, &sgm->rg[rgn]);
      else
        SegmentSearch192(data[rgn][0], data[rgn][1], &nclust, &sgm->rg[rgn]);

	  /*
printf(">>>>>> isec=%d rgn=%d nclust=%d\n",isec,rgn,nclust);fflush(stdout);
	  */

      /*if(nclust<=0) return;*/
      *nsgm = 1;
    }

    /* add TDC info to the found segments (14micros) */
    hits0 = bosDC0;
    for(i=0; i< bosNumDC0; i++)
    {
      BREAKSHORT(hits0->id, wire, layer);
      if(GOODHIT)
      {
        wire--;
        layer--;
        rgn = layer / 12;
        supl = (layer % 12) / 6;
        lay = layer % 6;

        if(supl==0)
        {
          for(iax=0; iax<sgm->rg[rgn].nax; iax++)
          {
            for(ih=0; ih<sgm->rg[rgn].ax[iax].la[lay].nhit; ih++)
            {
              if(wire==sgm->rg[rgn].ax[iax].la[lay].iw[ih])
              {
                sgm->rg[rgn].ax[iax].la[lay].tdc[ih] = hits0->tdc;
              }
            }
          }
        }
        else
        {
          for(iax=0; iax<sgm->rg[rgn].nax; iax++)
          {
            for(ih=0; ih<sgm->rg[rgn].st[iax].la[lay].nhit; ih++)
            {
              if(wire==sgm->rg[rgn].st[iax].la[lay].iw[ih])
              {
                sgm->rg[rgn].st[iax].la[lay].tdc[ih] = hits0->tdc;
              }
            }
          }
        }
      }
      hits0++;
    }

  }

  return;
}



/* print results for the one sector */

void
sgprint(PRSEC *sgm)
{
  int rgn, iax, il, ih;

  if(!DEBUG) return;

  printf("sgprint =============== wire from 1 here !!! ==============\n");
  for(rgn=0; rgn<3; rgn++)
  {
    printf(" region %d\n",rgn);
    for(iax=0; iax<sgm->rg[rgn].nax; iax++)
    {
      printf("   stereo: road %d\n",sgm->rg[rgn].st[iax].ird+1);
      for(il=NLAY-1; il>=0; il--)
      {
        printf("     la=%1d, nh=%1d:",il,sgm->rg[rgn].st[iax].la[il].nhit);
        for(ih=0; ih<sgm->rg[rgn].st[iax].la[il].nhit; ih++)
        {
          printf(" %3d-%4d",sgm->rg[rgn].st[iax].la[il].iw[ih]+1,
                            sgm->rg[rgn].st[iax].la[il].tdc[ih]);
        }
        printf("\n");
      }
      printf("   axial: road %d\n",sgm->rg[rgn].ax[iax].ird+1);
      for(il=NLAY-1; il>=0; il--)
      {
        printf("     la=%1d, nh=%1d:",il,sgm->rg[rgn].ax[iax].la[il].nhit);
        for(ih=0; ih<sgm->rg[rgn].ax[iax].la[il].nhit; ih++)
        {
          printf(" %3d-%4d",sgm->rg[rgn].ax[iax].la[il].iw[ih]+1,
                            sgm->rg[rgn].ax[iax].la[il].tdc[ih]);
        }
        printf("\n");
      }
    }

  }

  return;
}


/* fill road finding structures, SWAPING region 1 */

void
sgroad(PRSEC *sgm, int ncl[3], unsigned char ird[3][2][nclmx])
{
  int rgn, iax, il, ih, itmp;

  if(DEBUG) printf("sgroad ======= wire from 1 here !!! ==============\n");
  for(rgn=0; rgn<3; rgn++)
  {
    ncl[rgn] = sgm->rg[rgn].nax;
    if(DEBUG) printf(" region number = %d,   #clusters = %d\n",rgn,ncl[rgn]);
    for(iax=0; iax<ncl[rgn]; iax++)
    {
      ird[rgn][0][iax] = sgm->rg[rgn].ax[iax].ird+1;
      ird[rgn][1][iax] = sgm->rg[rgn].st[iax].ird+1;
      if(rgn==0)
      {
        itmp = ird[rgn][0][iax];
        ird[rgn][0][iax] = ird[rgn][1][iax];
        ird[rgn][1][iax] = itmp;
      }
      if(DEBUG) printf("  %2d: (%3d %3d)\n",
                       iax,ird[rgn][0][iax],ird[rgn][1][iax]);
    }
  }

  return;
}




