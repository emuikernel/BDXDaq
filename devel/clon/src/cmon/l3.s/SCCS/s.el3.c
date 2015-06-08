h31667
s 00160/00100/00393
d D 1.3 02/03/26 15:15:47 boiarino 4 3
c *** empty log message ***
e
s 00006/00004/00487
d D 1.2 01/11/29 12:36:55 boiarino 3 1
c minor changes
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 18:54:35 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/l3.s/el3.c
e
s 00491/00000/00000
d D 1.1 01/11/19 18:54:34 boiarino 1 0
c date and time created 01/11/19 18:54:34 by boiarino
e
u
U
f e 0
t
T
I 1

/* el3.c - level3 from Alex Vlassov */

#include <stdio.h> 
#include <string.h>
#include <math.h> 
#include "sdakeys.h"
SDAkeys keys;

#define ROW  4
#define COL  5
#define MAXCC 10
#define MAXEC 30
#define MAXECL 10
#define MAX_AMP 9000.
#define HALF 0.5
#define MAXSC 20
#define MAXTDC 4094
#define MAX_SC_PAD 23
#define MAXSECT 6
#define MAX_CC_SEGM 36
#define MAX_EC_SEGM 36
#define MAX_EC_LAYER 6
#define MIN_EC_ENERGY 400.

I 4
#define mask_no_l1 0x80
#define mask_no_cc 0x81
#define mask_no_sc 0x82
#define mask_no_ec 0x84
#define mask_ccec  0x88
#define mask_ccsc  0x90

E 4
/* Global variables */
static float ccped[MAXSECT][MAX_CC_SEGM];
static float ecped[MAXSECT][MAX_EC_SEGM][MAX_EC_LAYER];
static int sect_mask[MAXSECT];
D 4
static int mask_no_cc,mask_no_sc,mask_no_ec,mask_ccec,mask_ccsc;
E 4
static float torus_current[1];
static float cc_to_sc[3];
static float ec_to_sc[3];

/* Function prototypes */
int l3_init2(int runnumb);  
int l3_event2(int *iw);  
int read_cc_ped(void);
int read_ec_ped(void);
int ecsort(int nechits, int *ecpdn, int *ec_sort_pdn);
/*double get_epics(char *channel, char *attrib);*/

/*  ----------------- */

/* Body */
int
l3_init2(int runnumb)
{
  /* Purpose and methods : Initializations for level 3 trigger

     Inputs :
              Run number (int)
         
     Outputs:
              Returns 0 : Initialization is done without errors

                     -1 : Error during initialization */
 
  int fd, status, ind_epic, i;
  /*double val;*/

  /*  Reading pedestals */
  if(read_cc_ped())
  {
    printf(" ***  Can not read cc.tped file  *** \n");
    return(-1);
  }
  if(read_ec_ped())
  {
    printf(" ***  Can not read ec.tped file  *** \n");
	return(-1);
  }

  /* Sector mask settings */
  sect_mask[0] = 0x1; sect_mask[1] = 0x2;  sect_mask[2] = 0x4; 
  sect_mask[3] = 0x8; sect_mask[4] = 0x10; sect_mask[5] = 0x20;
D 4
  mask_no_cc = 0x81; mask_no_sc = 0x82; mask_no_ec = 0x83;
  mask_ccec = 0x84; mask_ccsc = 0x85;
E 4

  /* check torus current */
  /*val = get_epics("torus_current","VAL"); 
  printf("torus_current : %f\n",val); fflush(stdout);
  torus_current[0] = (float)val;*/

  trgetconfig(&keys);
  torus_current[0] = keys.zmagn[1];

  if(torus_current[0] < 0.)    /* OUTBENDING ELECTRONS */
  {
    printf("l3_init2: OUTBENDING\n");
    cc_to_sc[0] =  1.1900   ;
    cc_to_sc[1] =  0.64683  ;
    cc_to_sc[2] =  0.019717 ;
    ec_to_sc[0] = -0.81413  ;
    ec_to_sc[1] =  0.62782  ;
    ec_to_sc[2] =  0.0      ;
  }
  else                         /* INBENDING ELECTRONS */
  {
    printf("l3_init2: INBENDING\n");
    cc_to_sc[0] =  1.0404   ;
    cc_to_sc[1] =  0.75679  ;
    cc_to_sc[2] =  0.018668 ;
    ec_to_sc[0] = -0.13902  ;
    ec_to_sc[1] =  0.60180  ;
    ec_to_sc[2] =  0.0      ;
  }

  printf("l3_init2: CC_SC: %f %f %f\n",cc_to_sc[0],cc_to_sc[1],cc_to_sc[2]);
  printf("l3_init2: EC_SC: %f %f %f\n",ec_to_sc[0],ec_to_sc[1],ec_to_sc[2]);

  return(0);
}


int
l3_event2(int *iw)
{
	/*
  Purpose and methods :
       Checks if there was an electron candidate in the event.

  Inputs :
       TGBI bank for sectors with level1 trigger.

  Outputs: Returns MASK :
                        8 LOW bits containe information of 
                        electron candidate in a Sector:
                      -  If Yes,
                        bit 7 is 0;
                        bits 0-5 containes 1
                        in case of electron candidate in sector 1-6 
                      -  If NO, then bit 7 = 1, bits 0-6 containes number:
                        the reason for rejection :
D 4
                        1 : No CC data
                        2 : No SC data
                        3 : No EC data
                        4 : NO match CC with EC 
                        5 : NO match CC with SC
E 4
I 4
                           all 0 : No Level 1
                           bit 0 : No CC data
                           bit 1 : No SC data
                           bit 2 : No EC data
                           bit 3 : NO match CC with EC 
                           bit 4 : NO match CC with SC
E 4
	*/

  int i,j,k,ires,ncol,nrow,ncclust,ind,nsec,kc,jold,nsclust,neclust;
  int l_ec,s_ec,nechits,runnumb, ind_rgbi, ind_cc, ind_ec, ind_sc;
  int latch1, sect, nhis, mpdn;
  short *iw16;
  float ccsegm[MAXCC],ccadc[MAXCC],adc,tdc,ecadc[MAXEC],ecadc_s[MAXEC];
  float esegm[MAXECL],eadc[MAXECL];
  float s, a1, a2, a3, ccsg, aec, unit, aa, ab;
  int scpdn[MAXSC],ecpdn[MAXEC],ec_sort_pdn[MAXEC],ecpdn_s[MAXEC];
  int ifind, res, nres;

  /* Body */
D 4
  ifind = 0; nres = 0; res = 0x0;
E 4
I 4
  ifind = 0; nres = 0; res = 0;
E 4

  /* TGBI bank data */
  ind_rgbi = etNlink(iw,"TGBI",0); latch1 = iw[ind_rgbi];

  for (nsec = 0; nsec < MAXSECT; nsec++)    /* Cycle on sectors */
  {
I 4
    sect = nsec + 1;

E 4
    /* Skip Sectors without signal in latch1 */ 
D 4
    if ((latch1 & sect_mask[nsec]) == 0 ) continue;
E 4
I 4
    if((latch1 & sect_mask[nsec]) == 0)
    {
      nres |= mask_no_l1;
      /*printf("el3: no_l1 0x%02x\n",nres);*/
      continue;
	}
E 4

D 4
    /* Skip Sectors without signal in latch1 */
E 4

D 4
    sect = nsec + 1;

E 4
I 4
    /*************/
E 4
    /*  CC stuff */
D 4
    ind_cc = etNlink(iw,"CC  ",sect);
    if(ind_cc <= 0) continue;  /* No CC in the Sector - look for new sector */
E 4
I 4
    /*************/
E 4

D 4
    ncol = etNcol(iw,ind_cc);
	nrow = etNrow(iw,ind_cc);

    iw16 = (short *) &iw[ind_cc];
    k = 0; ncclust = 0;
    for(i=0;i<nrow;i++)         /* CC loop */
E 4
I 4
    ncclust = 0;
    if((ind_cc = etNlink(iw,"CC  ",sect)) > 0)
E 4
    {
D 4
      kc = (int)(HALF*(iw16[k] + 1));
      tdc = (float)(iw16[k+1]); 
      adc = (float)(iw16[k+2]) - ccped[nsec][(iw16[k]-1)];
E 4
I 4
      ncol = etNcol(iw,ind_cc);
      nrow = etNrow(iw,ind_cc);
E 4

D 4
      if(adc > MAX_AMP) continue;
      if(adc > 5. && tdc < MAXTDC)
E 4
I 4
      iw16 = (short *) &iw[ind_cc];
      k = 0;
      for(i=0;i<nrow;i++)         /* CC loop */
E 4
      {
D 4
        if(ncclust==0) 
E 4
I 4
        kc = (int)(HALF*(iw16[k] + 1));
        tdc = (float)(iw16[k+1]); 
        adc = (float)(iw16[k+2]) - ccped[nsec][(iw16[k]-1)];

        if(adc > MAX_AMP) continue;
        if(adc > 5. && tdc < MAXTDC)
E 4
        {
D 4
          ncclust = 1;
          ind = ncclust - 1; 
          ccsegm[ind] = kc;
          ccadc [ind] = adc;
          jold = kc;
        }
        else
  	    {
          if(abs(kc-jold) <= 2)   /* Same cluster */
E 4
I 4
          if(ncclust==0) 
E 4
          {
D 4
            ccsegm[ind] = ( ccadc[ind] * ccsegm[ind] + kc * adc )
                        / ( ccadc[ind] + adc );
            ccadc[ind] += adc;
            jold = kc;          
          }
          else                    /* New cluster */
          {
            ncclust += 1;
            ind += 1;
E 4
I 4
            ncclust = 1;
            ind = ncclust - 1; 
E 4
            ccsegm[ind] = kc;
            ccadc [ind] = adc;
            jold = kc;
          }
D 4
        }
	  }
      k += ncol;
    }                           /* CC loop */
E 4
I 4
          else
  	      {
            if(abs(kc-jold) <= 2)   /* Same cluster */
            {
              ccsegm[ind] = ( ccadc[ind] * ccsegm[ind] + kc * adc )
                        / ( ccadc[ind] + adc );
              ccadc[ind] += adc;
              jold = kc;          
            }
            else                    /* New cluster */
            {
              ncclust += 1;
              ind += 1;
              ccsegm[ind] = kc;
              ccadc [ind] = adc;
              jold = kc;
            }
          }
	    }
        k += ncol;
      }                           /* CC loop */
    }
E 4
    if(ncclust < 1)
D 4
      {if ( !ifind) nres = mask_no_cc ; continue;}
E 4
I 4
    {
      if(!ifind)
      {
        nres |= mask_no_cc;
        /*printf("el3: no_cc 0x%02x\n",nres);*/
      }
      continue;
    }
E 4

D 4
  /*  ----------------- */
E 4

D 4
  /*  SC stuff */
    ind_sc = etNlink(iw,"SC  ",sect);
    if(ind_sc <= 0) continue;  /* No SC in the Sector - look for new sector */
E 4
I 4
    /*************/
    /*  SC stuff */
    /*************/
E 4

D 4
    ncol = etNcol(iw,ind_sc);
	nrow = etNrow(iw,ind_sc);
E 4
I 4
    nsclust = 0;
    if((ind_sc = etNlink(iw,"SC  ",sect)) > 0)
    {
      ncol = etNcol(iw,ind_sc);
      nrow = etNrow(iw,ind_sc);
E 4

D 4
    iw16 = (short *)&iw[ind_sc];
    k = 0; nsclust = 0;
    for(i=0;i<nrow;i++)             /* SC loop */
E 4
I 4
      iw16 = (short *)&iw[ind_sc];
      k = 0;
      for(i=0;i<nrow;i++)             /* SC loop */
      {
        if(iw16[k]<= MAX_SC_PAD &&
          ( (iw16[k+1] > 0 && iw16[k+1] < MAXTDC) ||
            (iw16[k+3] > 0 && iw16[k+3] < MAXTDC)) )
        {
          nsclust += 1;
          scpdn[nsclust-1] = iw16[k]; 
        }
        k += ncol;
      }                            /* SC loop */
    }
    if(nsclust < 1)
E 4
    {
D 4
      if(iw16[k]<= MAX_SC_PAD &&
      ( (iw16[k+1] > 0 && iw16[k+1] < MAXTDC) ||
      (iw16[k+3] > 0 && iw16[k+3] < MAXTDC)))
E 4
I 4
      if(!ifind)
E 4
      {
D 4
        nsclust += 1;
        scpdn[nsclust-1] = iw16[k]; 
E 4
I 4
        nres |= mask_no_sc;
        /*printf("el3: no_sc 0x%02x\n",nres);*/
E 4
      }
D 4
      k += ncol;
    }                            /* SC loop */
    if(nsclust < 1)
      {if ( !ifind) nres = mask_no_sc ; continue;}
E 4
I 4
      continue;
    }
E 4

D 4
  /*  ----------------- */
E 4

D 4
  /*  EC stuff */
    ind_ec = etNlink(iw,"EC  ",sect);
    if(ind_ec <= 0) continue;  /* No EC in the Sector - look for new sector */
E 4
I 4
    /*************/
    /*  EC stuff */
    /*************/
E 4

D 4
    ncol = etNcol(iw,ind_ec);
	nrow = etNrow(iw,ind_ec);
E 4
I 4
    nechits = 0;
    if((ind_ec = etNlink(iw,"EC  ",sect)) > 0)
    {
      ncol = etNcol(iw,ind_ec);
      nrow = etNrow(iw,ind_ec);
E 4

D 4
    iw16 = (short *)&iw[ind_ec];
    k = 0; nechits = 0;
    for(i=0;i<nrow;i++)            /* EC loop */
E 4
I 4
      iw16 = (short *)&iw[ind_ec];
      k = 0;
      for(i=0;i<nrow;i++)            /* EC loop */
      {
        s_ec = iw16[k] & 0xFF;
        l_ec = iw16[k] >> 8 ;
        tdc = (float)(iw16[k+1]);
        adc = (float)(iw16[k+2]) - ecped[nsec][s_ec - 1][l_ec - 1];
        if(adc > 0. && (nechits < MAXEC ) && (l_ec == 1 || l_ec == 4))
        {
          ecpdn[nechits] = s_ec; 
          ecadc[nechits] = adc;
          nechits += 1;      
        }
        k += ncol;
      }                           /* EC loop */
    }
    if(nechits < 1)
E 4
    {
D 4
      s_ec = iw16[k] & 0xFF;
      l_ec = iw16[k] >> 8 ;
      tdc = (float)(iw16[k+1]);
      adc = (float)(iw16[k+2]) - ecped[nsec][s_ec - 1][l_ec - 1];
      if(adc > 0. && (nechits < MAXEC ) && (l_ec == 1 || l_ec == 4))
E 4
I 4
      if(!ifind)
E 4
      {
D 4
        ecpdn[nechits] = s_ec; 
        ecadc[nechits] = adc;
        nechits += 1;      
E 4
I 4
        nres |= mask_no_ec;
        /*printf("el3: no_ec 0x%02x\n",nres);*/
E 4
      }
D 4
      k += ncol;
    }                           /* EC loop */
E 4
I 4
      continue;
    }
E 4

D 4
    if(nechits < 1)
      {if ( !ifind ) nres = mask_no_ec ; continue;}
E 4

D 4
	/*  Sorting */
E 4
I 4
    /************************/
	/*  EC clusters sorting */
    /************************/
E 4

    i = ecsort(nechits,ecpdn,ec_sort_pdn);
    for(i=0;i<nechits;i++)
    {
      ecpdn_s[i] = ecpdn[ec_sort_pdn[i]];
      ecadc_s[i] = ecadc[ec_sort_pdn[i]];
    } 

    neclust = 0; aec = 0;

    for(i=0;i<nechits && neclust < MAXECL ;i++)
    {
      if(neclust == 0)
      {
        esegm[neclust] = jold = ecpdn_s[i];
        eadc[neclust] = ecadc_s[i];
        aec +=ecadc_s[i];
        neclust = 1;
      }
      else 
      {
        if((ecpdn_s[i] - jold) <= 2) /* Same cluster */
        {
          esegm[neclust-1] = ( eadc[neclust-1]*esegm[neclust-1]
             + ecpdn_s[i]*ecadc_s[i] ) / (eadc[neclust-1] + ecadc_s[i]);
          eadc[neclust-1] += ecadc_s[i];
          aec +=ecadc_s[i]; 
          jold = ecpdn_s[i];
        }
        else                         /* New cluster */
        {
          esegm[neclust] = jold = ecpdn_s[i];
          eadc[neclust] = ecadc_s[i];
          aec +=ecadc_s[i];
          neclust += 1;
        }
      }
    }

D 4
   /*  ----------------- */
E 4

D 4
  /*  Comparing */
E 4
I 4
    /*************/
    /* Comparing */
    /*************/
E 4

D 4
    a1 = 1000.; a2 = 1000.; a3 = 1000.; mpdn = 0;
	/*   CC-SC        CC-EC     SC-EC    */
E 4
I 4
    a1 = 1000.; /* CC-SC */
    a2 = 1000.; /* CC-EC */
    a3 = 1000.; /* SC-EC */
E 4

    for(i=0;i<ncclust;i++)                     /*  CC  */
    {
      ccsg = ccsegm[i];
      for(j=0;j<neclust;j++)                   /*  EC  */
      {
        s = fabs( 2.3677 + 1.135*ccsg +0.06692*ccsg*ccsg -
        0.00223*ccsg*ccsg*ccsg - esegm[j]);
        if(s < a2 ) a2 = s;
	  }

      for(k=0;k<nsclust;k++)                   /*  SC  */
      {
        s = fabs( (float)scpdn[k] - 
          (cc_to_sc[0] + cc_to_sc[1]*ccsg + cc_to_sc[2]*ccsg*ccsg));
        if(s < a1 )
        {
          a1 = s;
D 4
          mpdn = scpdn[k];
E 4
        }
	  }
    }

    for(j=0;j<neclust;j++)                     /*  EC  */
    {
I 3
      aa = ec_to_sc[0] + ec_to_sc[1]*esegm[j];
      if(aa < 1.5 ) aa = 1.5;
E 3
      for(k=0;k<nsclust;k++)                   /*  SC  */
      {
D 3
        aa = esegm[j];
		s = fabs( (float)scpdn[k] - (ec_to_sc[0] + ec_to_sc[1]*esegm[j] ));
        if(s < a3 ) a3 = s;
E 3
I 3
        s = fabs( (float)scpdn[k] - aa);
	    if(s < a3 ) a3 = s;
E 3
	  }
D 3
    }
E 3
I 3
	}
E 3

I 3

E 3
D 4
  /* switches */
E 4
I 4
    /************/
    /* switches */
    /************/
E 4

    if(a2 > 4.0)
    {
D 4
      nres = mask_ccec;
E 4
I 4
      nres |= mask_ccec;
      /*printf("el3: no_ccec 0x%02x\n",nres);*/
E 4
      if( a3 < 2. && aec > MIN_EC_ENERGY)
      {
        ifind = 1;
        res =  res | sect_mask[nsec];
        continue;
      }
    }
    else
    {
      if(a1 > 3.5) 
      {
D 4
        nres = mask_ccsc;
E 4
I 4
        nres |= mask_ccsc;
        /*printf("el3: no_ccsc 0x%02x\n",nres);*/
E 4
        if( a3 < 2. && aec > MIN_EC_ENERGY)
        {
          ifind = 1;
          res =  res | sect_mask[nsec];
          continue;
        }
      }
      else 
      {
        ifind = 1;
        res =  res | sect_mask[nsec];
        continue;
      }
	}

  }  /* Cycle on sectors */

D 4
  if (ifind) return(res);
  else return(nres);
E 4

I 4
  /********/
  /* exit */
  /********/

  if(ifind)
  {
    /*printf("el3: accept: res=0x%02x\n",res);*/
    return(res);
  }
  else
  {
    /*printf("el3: reject: nres=0x%02x\n",nres);*/
    return(nres);
  }

E 4
}


int
read_cc_ped(void)
{
  FILE *ptr;
  char st[128],filenm[128];
 /* Name of the file  with pedestal data data */
  char *filen_cc = "pedman/Tfiles/cc.tped";
  int j,n,sect,segm,iped;
  float *pm, tmp;

  sprintf(filenm,"%s/%s",getenv("CLON_PARMS"),filen_cc);
  if((ptr=fopen(filenm,"r")) == NULL)
  {
    printf(" *** ERROR in file OPENING %s \n",filenm);
    return(-1);
  }
  printf(" *** File %s is opened succesfully \n",filenm);

  pm =  &ccped[0][0];
  while(n=fgets(st,128,ptr) != NULL)
  {
    if((strlen(st) < 10) || (st[0] == '#') || (st[0] == '*') ||
    (st[0] == '1')) continue;
    n=sscanf(st,"%d%d%d%f", &segm,&sect,&iped,&tmp);
    ccped[sect][segm-1] = (float)iped;
    /* printf(" result : segment %d sector %d ped %d temp %f \n",
	   segm,      sect,     iped,   tmp); */
  }
  fclose(ptr);
  printf(" *** File %s is closed succesfully \n",filenm);
  return(0);

  /* -------  data are stored ----- */
}

int
read_ec_ped(void)
{
  FILE *ptr;
  char st[128],filenm[128];
 /* Name of the file  with pedestal data data */
  char *filen_ec = "pedman/Tfiles/ec.tped";
  int j,n,sect,num,segm,layer,iped;
  float *pm, tmp;

  sprintf(filenm,"%s/%s",getenv("CLON_PARMS"),filen_ec);
  if((ptr=fopen(filenm,"r")) == NULL)
  {
    printf(" *** ERROR in file OPENING %s \n",filenm);
    return(-1);
  }
  printf(" *** File %s is opened succesfully \n",filenm);

  pm =  &ecped[0][0][0];
  while(n=fgets(st,128,ptr) != NULL)
  {
    if((strlen(st) < 10) || (st[0] == '#') || (st[0] == '*') ||
    (st[0] == '1')) continue;
    n=sscanf(st,"%d%d%d%f", &num,&sect,&iped,&tmp);
    segm = num & 0xFF; layer = (num >> 8);
    ecped[sect][segm-1][layer-1] = iped;
  }
  fclose(ptr);
  printf(" *** File %s is closed succesfully \n",filenm);
  return(0);

  /* -------  data are stored ----- */
}


int
ecsort(int n, int *list, int *ind)
{
  int i, order, i1, k;

  if(n <= 0) return(0);
  if(n == 1)
  {
    ind[0] = 1;
    return(0);
  }
  for(i=0;i<n;i++) ind[i] = i;
 
  order = 0;
  while(!order)
  {
    order = 1;
    for(i=0;i<n-1;i++)
    {
      i1 = i+1;
      if(list[(ind[i])] > list[(ind[i1])])
      {
        k=ind[i];
        ind[i]=ind[i1];
        ind[i1]=k;
        order=0;
      }
    }
  }

  return(0);
}
E 1
