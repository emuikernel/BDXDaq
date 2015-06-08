
/* iclib.c - inner calorimeter; Stepan Stepanyan's and some other's
             code incorporated from offline to online reconstruction
             by Sergey and Ivan */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "iclib.h"
#include "coda.h"
#include "tt.h"

#define DEBUG
/* static variables: filled up in 'icinit()', read-only by others */

static float pedestal[MaxY][MaxX], gain[MaxY][MaxX];
static float ic_status[MaxY][MaxX];
static float tchan[MaxY][MaxX];
static float xpos[MaxY][MaxX], ypos[MaxY][MaxX], zpos[MaxY][MaxX];
static TTSPtr ttp = NULL;    


int ic_read_map_float_(int *runno_ic, int *longueur, float *result_float_ic,
                       char *subsystem_ic, char *item_ic);
int ic_read_map_int_(int *runno_ic, int *longueur, int *result_float_ic,
                     char *subsystem_ic, char *item_ic);

int
ic_read_map_float_(int *runno_ic, int *longueur, float *result_float_ic,
                   char *subsystem_ic, char *item_ic)
{
  int time, i;
  float *values_float_ic;
  char *dir_ic, def_map_ic[100];

  if( (values_float_ic = (float *) malloc(longueur[0]*sizeof(float)))==NULL)
  {
    printf("Error allocating memory in ic_read_map_float...");
    return(1);
   }
   
  if(dir_ic = getenv("CLON_PARMS") )
  {
    sprintf(def_map_ic,"%s/Maps/IC.map",dir_ic);

    time=-99;
    if(map_get_float(def_map_ic,subsystem_ic,item_ic,longueur[0],
                     values_float_ic,*runno_ic,&time) != 0)
    {
      printf("Error in map_get_float...");
      printf("path Map=%s\n",def_map_ic);
      return(1);
    }

    for(i=0; i<length_ic; i++) result_float_ic[i] = values_float_ic[i];
    free(values_float_ic);
    return(0);
  } 
  else
  {
    printf(" Error trying to read the Map : %s \n",dir_ic);
    return(1);
  }
}


int
ic_read_map_int_(int *runno_ic, int *longueur, int *result_int_ic,
                 char *subsystem_ic, char *item_ic)
{
  int time, i;
  int *values_int_ic;
  char *dir_ic, def_map_ic[100];

  if( (values_int_ic = (int *) malloc(longueur[0]*sizeof(int)))==NULL)
  {
    printf("Error allocating memory in ic_read_map_int...");
    return(1);
  }
   
  
  if(dir_ic = getenv("CLON_PARMS") )
  {
    sprintf(def_map_ic,"%s/Maps/IC.map",dir_ic);
    time=-99;

    if(map_get_int(def_map_ic,subsystem_ic,item_ic,longueur[0],values_int_ic,
                   *runno_ic,&time) != 0)
    {
      printf("Error in map_get_int...");
      printf("path Map=%s\n",def_map_ic);
      return(1);
    }
    for(i=0; i<length_ic; i++) result_int_ic[i] = values_int_ic[i];
    free(values_int_ic);
    return(0);
  } 
  else
  {
    printf(" Error trying to read the Map : %s \n",dir_ic);
    return(1);
  }
}


/* main functions */

int
icinit(int runnum)
{
  FILE *fd;
  int pedmans[NUM_SLOTS][MAXCHANNEL], pedgains[NUM_SLOTS][MAXCHANNEL];
  float x0,y0,z0;
  int slot, channel, ped, gains, offset;
  float sigma;
  int i, j, jj, ix, iy, res, itmp;
  int result_ent[length_ic];
  float result_reel[length_ic];
  float xstep, ystep;
  int longueur;
  char subsystem[10], item[10];
  int result;

  printf("icinit reached\n");

  /*ic_brun_(&runnum);*/    

  strcpy(subsystem,"Energy");
  strcpy(item,"Gain0");
  longueur = length_ic;

  result= ic_read_map_float_(&runnum,&longueur,result_reel,subsystem,item);
  if(result) printf("Gain map not found. Using defaults.\n"); 

 for(ix=0; ix<MaxX; ix++)
    for(iy=0; iy<MaxY; iy++)
      if (!result) gain[iy][ix] = result_reel[ix+iy*MaxY];
      else gain[iy][ix]=1.0;
 /*

every number corresponds to 3.8Gev
----------------------------------

1	0
2	5100
3	4500
4	4200
5	5050
6	3900
7	2000
8	4200
9	4800
10	4250
11	5000
12	4050
13	4800
14	4700
15	4500
16	4400
17	3650
19	3700
21	6900
22	8000
23	4850
24	5950
25	5250
26	4800
27	4700
28	7000
29	4600
30	4850
31	7000
32	4800
35      0
36	6050
37	4900
38	5100
39	5250
40	6000
41	7000
42	5000
43	7200
44	6000
45	4300
46	3800
47	5150
48	4200
49	1800
50	4200
51	2400
52	4300
53	5400
54	4200
55	3000
56	5000
57	5000
58	4500
65	5000
66	4750
67	4500
68	5050
69	4000
70	5300
71	5350
72	5500
73	0
74	5700
75	4950
76	5500
77	5000
78	6050
79	4400
80      0
83	4100
84	5300
85	5100
86	6000
87	4800
88	3000
89	4500
90	5200
91	5350
92	6000
93	5250
94	5200
95	5300
96	0
97	5400
98	5100
99	5800
100	5900
101	4700
102	5800
103	5700
104	8192
105	5400
106	4500
107	5000
108	5000
109	4500
110	5500
111	0
112	5300

  */

  strcpy(subsystem,"Energy");
  strcpy(item,"Pedestals");


  result=ic_read_map_float_(&runnum,&longueur,result_reel,subsystem,item);
  if (result) printf("Pedestal map not found. Using defaults.\n"); 
  for(ix=0; ix<MaxX; ix++)
    for(iy=0; iy<MaxY; iy++)
      if (!result) pedestal[iy][ix] = result_reel[ix+iy*MaxY];
      else pedestal[iy][ix]=600; // Pedestal default value




  /* get pedestals from online file */
  if(result)
  {
    printf("reading pedestal file ...\n");

    if(fd = fopen("/usr/local/clas/parms/pedman/archive/cc1.ped_cmon","r"))
    {
      for(i=0; i<NUM_SLOTS; i++)
      {
        for(j=0; j<MAXCHANNEL; j++)
        {
          pedmans[i][j]=0;
        }
      }
	  
      do
      {
        res = fscanf(fd, "%i %i %i %f %i",
	  				  &slot,&channel,&ped,&sigma,&offset);
        if(res > 0)
        {
          printf("slot=%d channel=%d -> ped=%d\n",slot,channel,ped);
          pedmans[slot][channel] = ped;
        }
      } while(res > 0);
	  
      printf(" done.\n");
    }
    else
    {
      printf(" cannot open a file - exit.\n");
      exit(0);
    }

    printf("reading gain file ...\n");

    if(fd = fopen("/usr/local/clas/parms/pedman/archive/cc1.gain","r"))
    {
      for(i=0; i<NUM_SLOTS; i++)
      {
        for(j=0; j<MAXCHANNEL; j++)
        {
          pedgains[i][j]=0;
        }
      }
	  
      do
      {
        res = fscanf(fd, "%i %i %i %i",
	  				  &slot,&channel,&itmp,&gains);
        if(res > 0)
        {
          printf("slot=%d channel=%d -> gain=%d\n",slot,channel,gains);
          pedgains[slot][channel] = gains;
        }
      } while(res > 0);
	  
      printf(" done.\n");
    }
    else
    {
      printf(" cannot open a file - exit.\n");
      exit(0);
    }

    printf("downloading DDL table ...\n");
    clonbanks();
    printf(" done.\n");

    printf("icinit: downloading translation table (ttp=0x%08x) ...\n",ttp);
    ttp = TT_LoadROCTT(12, ttp);
    printf(" done.\n");

    for(i=0; i<NUM_SLOTS; i++)
    {
      for(j=0; j<64; j++)
      {
        jj = j << 1;
        if(!strncmp((char *)&ttp->name[i][jj],"IC  ",4))
        {
          printf("tt: name>%4.4s< id->0x%04x sl=%d ch=%d ped=%d gain=%d\n",
           &ttp->name[i][jj],ttp->id[i][jj],i,j,pedmans[i][j],pedgains[i][j]);
          ix = ttp->id[i][jj] & 0xFF;
          iy = (ttp->id[i][jj] >> 8) & 0xFF;
          pedestal[iy][ix] = (float)pedmans[i][j];
          gain[iy][ix] = 3800.0/((float)pedgains[i][j] - pedestal[iy][ix]);
if(gain[iy][ix] < 0.) gain[iy][ix] = 0.;
          printf("pedestal[0x%04x][0x%04x]=%f\n",iy,ix,pedestal[iy][ix]);
          printf("gain[0x%04x][0x%04x]=%f\n",iy,ix,gain[iy][ix]);
        }
      }
    }

  }



  /* **************************************************************** */
  strcpy(subsystem,"Timing");
  strcpy(item,"Tch");
  if (!ic_read_map_float_(&runnum,&longueur,result_reel,subsystem,item))
  for(ix=0; ix<MaxX; ix++)
    for(iy=0; iy<MaxY; iy++)
      tchan[iy][ix] = result_reel[ix+iy*MaxY];
  else {printf("Error reading Timimg from file, using default.\n");
  for(ix=0; ix<MaxX; ix++)
    for(iy=0; iy<MaxY; iy++)
      tchan[iy][ix] = 1.0;}
  

  strcpy(subsystem,"status");
  strcpy(item,"status");
  if(!ic_read_map_int_(&runnum,&longueur,result_ent,subsystem,item))
  for(ix=0; ix<MaxX; ix++)
    for(iy=0; iy<MaxY; iy++)
      ic_status[iy][ix] = result_ent[ix+23*iy];
  else {
  printf("Error reading status from file, using default.\n");
  for(ix=0; ix<MaxX; ix++)
    for(iy=0; iy<MaxY; iy++)
      ic_status[iy][ix] = 0;
  }

  /* ***************************************************************** */
  strcpy(subsystem,"Geometry");
  strcpy(item,"stepx");
  longueur = 1;
  result=ic_read_map_float_(&runnum,&longueur,result_reel,subsystem,item);
  if (result) printf("Geometry file not found. Using defaults.\n");
  if (!result) xstep = result_reel[0];
  else xstep=0.133; // Its our Geometry step x default

  strcpy(item,"stepy");
  if(!ic_read_map_float_(&runnum,&longueur,result_reel,subsystem,item))
  ystep = result_reel[0];
  else 
  {ystep=0.133;
	 printf("Geometry file not found. Using defaults.\n");}

  strcpy(item,"x0");
  if(!ic_read_map_float_(&runnum,&longueur,result_reel,subsystem,item))
  x0 = result_reel[0];
  else x0=0;


  /* ****************************************************************** */
  strcpy(item,"y0");
  if(!ic_read_map_float_(&runnum,&longueur,result_reel,subsystem,item))
  y0 = result_reel[0];
  else y0=0;


  /* ****************************************************************** */
  strcpy(item,"z0");
  if (!ic_read_map_float_(&runnum,&longueur,result_reel,subsystem,item))
  z0 = result_reel[0];
  else z0=0;

  printf("->ehitmin=%f, egrplmin=%f, egrpmin=%f, ihalf=%d, MaxX=%d, MaxY=%d\n",
    ehitmin,egrplmin,egrpmin,ihalf,MaxX,MaxY);

  for(ix=0; ix<MaxX; ix++)
  {
    for(iy=0; iy<MaxY; iy++)
	{
      zpos[iy][ix] = z0;
      xpos[iy][ix] = x0 + ((float)(ix-11))*xstep;
      ypos[iy][ix] = y0 + ((float)(iy-11))*ystep;
      if(ix==(ihalf-1) && iy==(ihalf-1))
      {
        printf("X0=%f, Y0=%f\n",xpos[iy][ix],ypos[iy][ix]);
      }
    }
  }
  printf("Going...1\n");
  return(0);
}


int
iclib(int *jw)
{
#define MAXCLUST 100
  int i, j, k, l, m,  ncol, nrow, ind, ix, iy, ierr;
  int nmod, istat[MaxRaw], adc[MaxRaw], tdc[MaxRaw], ixm[MaxRaw], iym[MaxRaw];
  unsigned short *iw16;
  int nhit, itmp, new_hit_added_to_group;
  float ehit, tmp;
  int ngroup=0, nmember[MaxGroup], groupmember[MaxHits][MaxGroup];
  int ixhit[MaxHits], iyhit[MaxHits];
  float eicm[MaxHits], ticm[MaxHits], xhp[MaxHits], yhp[MaxHits];
  float eicg[MaxGroup], ticg[MaxGroup];
  float xhg[MaxGroup], yhg[MaxGroup], zhp[MaxHits];
  int our_map_adc[MaxX+1][MaxY+1];
  int bufx[MaxX*MaxY];
  int bufy[MaxX*MaxY];
  int counter,counter2;  
  int sum,itmp2;
  unsigned short *pnt;
  float *pt;
  float eclust[MAXCLUST],eclmax[MAXCLUST],tclust[MAXCLUST],xclust[MAXCLUST],yclust[MAXCLUST];
  float zclust[MAXCLUST],xcmax[MAXCLUST],ycmax[MAXCLUST],zcmax[MAXCLUST];
  float res1[MAXCLUST],res2[MAXCLUST],res3[MAXCLUST];
  int ncryst[MAXCLUST], ixclust[MAXCLUST], iyclust[MAXCLUST];
 
  /* typedef struct{short id;
          short tdc;
          short adc;}icn0;*/
  /*
  printf("iclib reached\n");
  */
  ierr = 0;
  etNdrop(jw,"ICHB",0);



  /*ic_read_bos(IW,ierr)*/;
  
  ind = etNlink(jw,"IC  ",0);
  if(ind > 0)
  {
    ncol = etNcol(jw,ind);
    nrow = etNrow(jw,ind);
    pnt = (unsigned short *)&jw[ind];
    if(nrow>0 && ncol>0) 
	{
	  /*Lets first clean 2-dimension map */
      for (i=0;i<MaxX+1;i++)
        for (j=0;j<MaxY+1;j++)
        {
          our_map_adc[i][j]=0;
	    }
	  /*       for (i=0;i<MaxX;i++)
        {for (j=0;j<MaxY;j++)
          printf("%05d ",our_map_adc[i][j]);
		  printf("\n");}*/
	  /* Lets fill our_map with data */
#ifdef DEBUG
       for (i=0;i<nrow;i++)
         printf("id=%0x %d %d tdc=%d adc=%d\n",pnt[i*3],pnt[i*3]/256,pnt[i*3]-(pnt[i*3]/256)*256,pnt[i*3+1],pnt[i*3+2]);
#endif
       for (i=0;i<nrow;i++)
       {
         j=(pnt[i*3]>>8)&0xFF;
         k=pnt[i*3]&0xFF;
         if(j>MaxY || k>MaxX)
         {
           printf("%0x %d %d %d Error while reading IC %d or %d greater than MAXSIZE \n",
            pnt[i*3],i,nrow,ncol,j,k);exit(-1);}
            our_map_adc[j][k]=pnt[i*3+2];
            itmp2=pedestal[j][k];
            our_map_adc[j][k]-=itmp2;
            if (our_map_adc[j][k]<0) our_map_adc[j][k]=0;
         }
#ifdef DEBUG
       for (i=MaxY-1; i>=0; i--)
       { printf("%02d|",i);
         for (j=0; j<MaxX; j++)
		 { if (!our_map_adc[i][j]) printf("   0 ");
           else printf("%04d ",our_map_adc[i][j]);
	     }
	     printf("\n");
       }
       printf("   ------------------------------------------------------------------------------------------------------------------\n");
       printf("   ");
       for (j=0; j<MaxX; j++)
       printf("  %02d ",j);
       printf("\n");
	   /* Now we try to find clasters */
     printf("Looking for clusters\n");
#endif
	 do{ k=0;
           l=0;

		   // Here we look for largest value ADC chanel not used before.
           // Chanels we use we mark with negative values. That is our_map_adc[x][y]=-our_map_adc[x][y]

           for(i=0;i<MaxY;i++)
            for(j=0;j<MaxX;j++)
			  if (our_map_adc[i][j]>our_map_adc[k][l]) {k=i;l=j;}      

		   // If one found
		   //          printf("k=%d l=%d\n",k,l);
           if (k!=0 && l!=0) {
#ifdef DEBUG
printf("Starting build cluster info.\n");
#endif
/*
  How it SHOULD work:
  We take point(peak) and look if neighboring point qualify to be part of the same cluster.
  If it qualify we add to chain. We look in four directions.
  Conditions to be met:
  1)Secondary point have to be smaller than peak.
  2)It have not be used yet(if used we convert it to negative)
  3)It have to be within bounds.
  If all conditions are met then we put we point into cluster and later we try
  to find OTHER free points around it.
 */           
           counter=0;            
           counter2=0;
           bufy[counter]=k;
           bufx[counter++]=l;
           while(counter2<counter)
			 {// printf("Begining: counter=%d counter2=%d\n",counter,counter2);
             i=bufy[counter2];
             j=bufx[counter2];
             // Look to the left
			 if (i>1 && 
                 our_map_adc[i][j]>=our_map_adc[i-1][j] && our_map_adc[i-1][j]>0) {
                 for (m=0;m<counter;m++) 
                  if (bufy[m]==i-1 && bufx[m]==j) goto a0010;
                 bufy[counter]=i-1;
                 bufx[counter++]=j;}
             // Look down
a0010:			 if (j>1 && 
                 our_map_adc[i][j]>=our_map_adc[i][j-1] && our_map_adc[i][j-1]>0) {
                 for (m=0;m<counter;m++) 
                  if (bufy[m]==i && bufx[m]==j-1) goto a0011;
                 bufy[counter]=i;
                 bufx[counter++]=j-1;}
             // Look to the right
a0011:         
               if (i<MaxX-1)
              if (our_map_adc[i][j]>=our_map_adc[i+1][j] && our_map_adc[i+1][j]>0) {
                 for (m=0;m<counter;m++) 
                  if (bufy[m]==i+1 && bufx[m]==j) goto a0012;
                 bufy[counter]=i+1;
                 bufx[counter++]=j;}
			  // look up
a0012:            
              if (j<MaxY-1)
				{
              if (our_map_adc[i][j]>=our_map_adc[i][j+1] && our_map_adc[i][j+1]>0) {
			   
				for (m=0;m<counter;m++){ 
                  if (bufy[m]==i && bufx[m]==j+1) goto a013;}
                
                 bufy[counter]=i;
                 bufx[counter++]=j+1;}}
         

           a013: if (j<MaxY-1 && i<MaxX-1)
				{
              if (our_map_adc[i][j]>=our_map_adc[i+1][j+1] && our_map_adc[i+1][j+1]>0) {
			   
				for (m=0;m<counter;m++){ 
                  if (bufy[m]==i+1 && bufx[m]==j+1) goto a014;}
                
                 bufy[counter]=i+1;
                 bufx[counter++]=j+1;}}

           a014: if (j<MaxY-1 && i>1 )
				{
              if (our_map_adc[i][j]>=our_map_adc[i-1][j+1] && our_map_adc[i-1][j+1]>0) {
			   
				for (m=0;m<counter;m++){ 
                  if (bufy[m]==i-1 && bufx[m]==j+1) goto a015;}
                
                 bufy[counter]=i-1;
                 bufx[counter++]=j+1;}}

a015: if (j>1 && i>1 )
				{
              if (our_map_adc[i][j]>=our_map_adc[i-1][j-1] && our_map_adc[i-1][j-1]>0) {
			   
				for (m=0;m<counter;m++){ 
                  if (bufy[m]==i-1 && bufx[m]==j-1) goto a016;}
                
                 bufy[counter]=i-1;
                 bufx[counter++]=j-1;}}
a016: if (j>1 && i<MaxX-1 )
				{
              if (our_map_adc[i][j]>=our_map_adc[i+1][j-1] && our_map_adc[i+1][j-1]>0) {
			   
				for (m=0;m<counter;m++){ 
                  if (bufy[m]==i+1 && bufx[m]==j-1) goto a000;}
                
                 bufy[counter]=i+1;
                 bufx[counter++]=j-1;}}



          a000:  our_map_adc[i][j]=-our_map_adc[i][j];
                 counter2++;
				
		  
				 // printf("counter=%d counter2=%d\n",counter,counter2);
		   }
#ifdef DEBUG
         printf("Found cluster.Number entries: %d Peak position(%d,%d)\n",counter,bufx[0],bufy[0]);
		   // Now we have cluster and have to do something with it.
#endif	      
          eclmax[ngroup]=-our_map_adc[bufy[0]][bufx[0]];
          xcmax[ngroup]=-xpos[bufy[0]][bufx[0]];
          ycmax[ngroup]=ypos[bufy[0]][bufx[0]];
          printf("Peak x=%f y=%f\n",xcmax[ngroup],ycmax[ngroup]);
          zcmax[ngroup]=zpos[bufy[0]][bufx[0]];
          ncryst[ngroup]=counter;
          eclust[ngroup]=0;
          xclust[ngroup]=0;
          yclust[ngroup]=0;
          zclust[ngroup]=0;
          ixclust[ngroup]=bufx[0];
          iyclust[ngroup]=bufy[0];
      
          for(i=0; i<counter; i++) 
          {
            k=bufy[i];
            l=bufx[i];
		    tmp=-our_map_adc[k][l]*gain[k][l];
			//            printf("1: eclust= %f tmp=%f ngroup=%d\n",eclust[ngroup],tmp,ngroup);
            eclust[ngroup]+=tmp;
			//   printf("2: eclust= %f tmp=%f ngroup=%d\n",eclust[ngroup],tmp,ngroup);

            xclust[ngroup]-=tmp*xpos[k][l]*gain[k][l];
            yclust[ngroup]+=tmp*ypos[k][l]*gain[k][l];
            zclust[ngroup]+=tmp*zpos[k][l]*gain[k][l];
		  }
          xclust[ngroup]=xclust[ngroup]/eclust[ngroup];
          yclust[ngroup]=yclust[ngroup]/eclust[ngroup];
          zclust[ngroup]=zclust[ngroup]/eclust[ngroup];
          printf("--> Energy=%f\n",eclust[ngroup]);

          ngroup++;

          //End of cluster processing
        }

}while((k*l)!=0);
#ifdef DEBUG
printf("Writing %d found clusters to bank...",ngroup);
#endif
	 // Now we got all possible clusters, lets write it to ICHB bank
     ind=etNcreate(jw,"ICHB",0,14,ngroup);
     if (ind>0)
	   {for (i=0;i<ngroup;i++)
		 {pt=(float *)&jw[ind];
		 pt[i*14]=eclust[i];         // 0
		 pt[i*14+1]=eclmax[i];       // 1
         pt[i*14+4]=xclust[i];       // 4
         pt[i*14+5]=yclust[i];       // 5
         pt[i*14+6]=zclust[i];       // 6
         pt[i*14+7]=xcmax[i];        // 7
         pt[i*14+8]=ycmax[i];        // 8
         pt[i*14+9]=zcmax[i];        // 9
         jw[ind+i*14+10]=ixclust[i];  // 10 temporary
         jw[ind+i*14+11]=iyclust[i];  // 11 temporary
         jw[ind+i*14+13]=ncryst[i];  // 13

		 }
	   }

	   

	}
  }
#ifdef DEBUG
printf("Written.\n");
#endif

  /* read 'IC  ' bank and fill corresponding arrays */


return(0);



  nmod = 0;
  for(i=0; i<MaxRaw; i++) istat[i] = 0;
  ind = etNlink(jw,"IC  ",0);
  if(ind > 0)
  {
    ncol = etNcol(jw,ind);
    nrow = etNrow(jw,ind);
    iw16 = (unsigned short *)&jw[ind];
    k = 0;
    for(i=0; i<nrow; i++)
    {
      istat[nmod] = -101;
      tdc[nmod] = iw16[k+1];
      adc[nmod] = iw16[k+2];
      ixm[nmod] = iw16[k] & 0xFF;
      iym[nmod] = (iw16[k]>>8) & 0xFF;

      nmod ++;
      if(nmod >= MaxRaw) break;

      k += ncol;
    }
  }

  /* continue if have some data */

  if(nmod > 0)
  {

	/*
uthfill(ana->histf, 997, (float)adc[0], 0., 1.);
	*/

    /*ic_e_t(ierr)*/;
    nhit = 0;
    for(i=0; i<nmod; i++)
    {
      if(istat[i] == -101)
      {
        ix = ixm[i];
        iy = iym[i];
        ehit = (adc[i]-pedestal[iy][ix])*gain[iy][ix];
        if(ehit > ehitmin && nhit < MaxHits)
        {
          eicm[nhit] = ehit;
          ticm[nhit] = tdc[i]*tchan[iy][ix];
          xhp[nhit] = xpos[iy][ix];
          yhp[nhit] = ypos[iy][ix];
          zhp[nhit] = zpos[iy][ix];
          ixhit[nhit] = ixm[i];
          iyhit[nhit] = iym[i];
          istat[nhit] = -nhit;

          nhit ++;
        }
        else
        {
          istat[i] = 0;
        }
      }
    }

    /* continue if have some hits */

    if(nhit > 0)
    {

      /*ic_group(ierr)*/;

      /* Lets first sort hits */
      for(i=0; i<nhit; i++)
      {
        for(j=i+1; j<nhit; j++)
        {
          if(eicm[j] > eicm[i])
          {
            tmp=eicm[j]; eicm[j]=eicm[i]; eicm[i]=tmp;
            tmp=ticm[j]; ticm[j]=ticm[i]; ticm[i]=tmp;
            tmp=xhp[j]; xhp[j]=xhp[i]; xhp[i]=tmp;
            tmp=yhp[j]; yhp[j]=yhp[i]; yhp[i]=tmp;
            tmp=zhp[j]; zhp[j]=zhp[i]; zhp[i]=tmp;
            itmp=ixhit[j]; ixhit[j]=ixhit[i]; ixhit[i]=itmp;
            itmp=iyhit[j]; iyhit[j]=iyhit[i]; iyhit[i]=itmp;
            itmp=istat[j]; istat[j]=istat[i]; istat[i]=itmp;
          }
        }
      }

      ngroup=0;
      for(i=0; i<nhit; i++)
      {
        if(istat[i]<0)
        {
          /* If current hit free lets put it into a new group */
          nmember[ngroup]++;
          groupmember[0][ngroup]=i; //New group founded
          istat[i]=-ngroup-i-1;        
          do
          {
            new_hit_added_to_group=0;
            for(j=i+1; j<nhit; j++)
            {
              if(istat[j]<0)   /* its free */
              {
                for(k=0; k<nmember[ngroup]; k++)
                {
                  if(eicm[groupmember[k][ngroup]]>eicm[j])
                  {
                    if((abs(xhp[groupmember[k][ngroup]]-xhp[j])
                       +abs(yhp[groupmember[k][ngroup] ]-yhp[j]))<0.3)
                    {
                      groupmember[nmember[ngroup]++][ngroup]=j;
                      istat[i]=-ngroup-j-1;
                      new_hit_added_to_group=1;
                      goto a01;
                    }
                  }
                }
              }
a01:
              continue;
            }
             
          } while(new_hit_added_to_group==1);
          ngroup++;
        }
      }



      if(ierr > 0)
      {


        /*ichb(IW,IW,ierr)*/;


      }
    }
  }

  return(0);
}
