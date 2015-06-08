
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "uttime.h"
#include "prlib.h"


#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))


main(int argc,char **argv)
{
  char str1[1000];
  char str2[1000];
  char *str3="OPEN OUTPU1 UNIT=21 FILE='/tmp/tmp.bos' WRITE RECL=32768 NEW";
  char *str33="OPEN OUTPU1 UNIT=21 FILE='/tmp/tmp.bos' ";
  char *str4="OPEN OUTPU2 UNIT=22 FILE='./prlink.bos' WRITE RECL=32768 NEW";

  char *kname1="        ", *hname1="        ", *format1="               ";
  char *kname2="        ", *hname2="        ", *format2="               ";
  char *kname3="LTORM   ", *hname3="        ", *format3="               ";
  int status,i,j,k,l,is,in,numra,numrb,icl,ierr,numdb,ncol1,ncol,nrow,nch,max_npat;
  int max_lnk1,max_lnk2,max_lnk3;
  int nw11, nw12, nw13, nw14, nw15;
  int nw21, nw22, nw23, nw24, nw25;
  int nw31, nw32, nw33, nw34, nw35;
  int md1,npat1,npat2,npat3,ipat[MAX_NPAT][6],dtyp1,utime1,dtyp2,utime2,tmp1,tmp2,skip,nwd;
  int *buf10, *buf20, *buf30;
  int *buf13, *buf23, *buf33;
  short *buf13s, *buf23s, *buf33s;
  int lnk1[NS2], lnk2[NS2], lnk3[NS2];
  int *link11, *link21, *link31;
  int *link12, *link22, *link32;
  short *nlink13[2], *nlink23[2], *nlink33[2];
  static LINKsegm *nlink14, *nlink24, *nlink34;
  static char buf14[20000000], buf24[20000000], buf34[40000000]; /* temporary */
  int *link15, *link25, *link35;
  static int grlookup[MAX_NPAT];


  if(argc != 3) {
    printf("Usage: mergedict <dictfilename1> <dictfilename2>\n");
    printf("Output will go to the prlink.bos file in the current directory.\n");
    exit(1);
  }

  strcpy(str1,"OPEN INPUT1 UNIT=11 FILE=\"");
  strcat(str1,argv[1]);
  strcat(str1,"\"");

  strcpy(str2,"OPEN INPUT2 UNIT=12 FILE=\"");
  strcat(str2,argv[2]);
  strcat(str2,"\"");


  status = fparm_(str1,strlen(str1));
  if(status) {printf("1: status=%d\n",status); exit(1);}
  status = fparm_(str2,strlen(str2));
  if(status) {printf("2: status=%d\n",status); exit(1);}
  status = fparm_(str3,strlen(str3));
  if(status) {printf("3: status=%d\n",status); exit(1);}


  /* activate temporary output file */

  fwname_("OUTPU1",6);
  numra = numrb = icl = 0;
  fwkey_(kname3,&numra,&numrb,&icl,strlen(kname3));


  /* activate 1st input file */

  frname_("INPUT1",6);
  for(i=0; i<strlen(kname1); i++) kname1[i] = ' ';
  frkey_(kname1,&numra,&numrb,&icl,&ierr,strlen(kname1));
  printf("open 1st dictionary file: >%s< %d %d %d %d\n",kname1,numra,numrb,icl,ierr);


  /* activate 2nd input file */

  frname_("INPUT2",6);
  for(i=0; i<strlen(kname2); i++) kname2[i] = ' ';
  frkey_(kname2,&numra,&numrb,&icl,&ierr,strlen(kname2));
  printf("open 2nd dictionary file: >%s< %d %d %d %d\n",kname2,numra,numrb,icl,ierr);


  /* read first record from 1st file */

  frname_("INPUT1",6);
  for(i=0; i<strlen(hname1); i++) hname1[i] = ' ';
  for(i=0; i<strlen(format1); i++) format1[i] = ' ';
  frhdr_(hname1,&numdb,&ncol,&nrow,format1,&nch,&ierr,strlen(hname1),strlen(format1));
  buf10 = (int *) malloc(ncol*sizeof(int));
  frdat_(&md1,buf10,&ncol);
  max_npat = (md1-1-2-NS2)/6;
  printf("1st file: >%s< %8d %6d %6d %6d    >%s< ---> max_npat=%d\n",
                    hname1,numdb,ncol,nrow,md1,format1,max_npat);
  max_lnk1 = buf10[ncol-1];
  k = 0;
  npat1 = npat3 = buf10[k++];
  for(j=0; j<npat1; j++)
  {
    for(i=0; i<6; i++)
    {
      ipat[j][i] = buf10[k++];
    }
  }
  for(j=npat1; j<max_npat; j++) for(i=0; i<6; i++) k++; /* skip the rest of record */
  dtyp1 = buf10[k++];
  utime1 = buf10[k++];
  for(i=0; i<NS2; i++) lnk1[i] = buf10[k++];
  printf("1st file: max_lnk1=%d npat1=%d max_npat=%d dtyp1=%d utime1=%d\n",
               max_lnk1,npat1,max_npat,dtyp1,utime1); fflush(stdout);


  /* read first record from 2nd file */

  frname_("INPUT2",6);
  for(i=0; i<strlen(hname2); i++) hname2[i] = ' ';
  for(i=0; i<strlen(format2); i++) format2[i] = ' ';
  frhdr_(hname2,&numdb,&ncol,&nrow,format2,&nch,&ierr,strlen(hname2),strlen(format2));
  buf20 = (int *) malloc(ncol*sizeof(int));
  frdat_(&md1,buf20,&ncol);
  max_npat = (md1-1-2-NS2)/6;
  printf("2nd file: >%s< %8d %6d %6d %6d    >%s< ---> max_npat=%d\n",
                    hname2,numdb,ncol,nrow,md1,format2,max_npat);
  max_lnk2 = buf20[ncol-1];
  k = 0;
  npat2 = buf20[k++];
  for(j=0; j<npat2; j++)
  {
    skip = 0;
    /*printf("== %3d == %3d %3d %3d %3d %3d %3d\n",j,buf20[k],buf20[k+1],buf20[k+2],buf20[k+3],buf20[k+4],buf20[k+5]);*/
    for(l=0; l<npat1; l++)
    {
      if(ipat[l][0]==buf20[k]&&ipat[l][1]==buf20[k+1]&&ipat[l][3]==buf20[k+3]&&
         ipat[l][4]==buf20[k+4]&&ipat[l][5]==buf20[k+5])
      {
        grlookup[j] = l;
printf("=grlookup[%3d]=%d\n",j,grlookup[j]);
        skip=1;
        break;
      }
    }
    if(!skip)
    {
      for(i=0; i<6; i++)
      {
        ipat[npat3][i] = buf20[k++];
      }
      grlookup[j] = npat3;
printf("-grlookup[%3d]=%d\n",j,grlookup[j]);
      npat3++;
      if(npat3 > MAX_NPAT) {printf("MAX_NPAT=%d exeeded - ERROR exit.\n",MAX_NPAT); exit(0);}
    }
    else
    {
      k+=6;
    }
  }
  for(j=npat2; j<max_npat; j++) for(i=0; i<6; i++) k++; /* skip the rest of record */
  dtyp2 = buf20[k++];
  utime2 = buf20[k++];
  for(i=0; i<NS2; i++) lnk2[i] = buf20[k++];
  printf("2nd file: max_lnk2=%d npat2=%d max_npat=%d dtyp2=%d utime2=%d\n",
               max_lnk2,npat2,max_npat,dtyp2,utime2); fflush(stdout);


  /* allocate buffers for 1st file */

  link11 = (int *) malloc(max_lnk1*sizeof(int));
  if(!link11) printf("prlib: ERROR: malloc problem (link11)\n"); fflush(stdout);

  link12 = (int *) malloc(max_lnk1*sizeof(int));
  if(!link12) printf("prlib: ERROR: malloc problem (link12)\n"); fflush(stdout);

  nlink13[0] = (short *) malloc(max_lnk1*sizeof(short));
  if(!nlink13[0]) printf("prlib: ERROR: malloc problem (nlink13[0])\n"); fflush(stdout);
  nlink13[1] = (short *) malloc(max_lnk1*sizeof(short));
  if(!nlink13[1]) printf("prlib: ERROR: malloc problem (nlink13[1])\n"); fflush(stdout);
  buf13 = (int *) malloc(max_lnk1*sizeof(int));
  buf13s = (short *) buf13;

  nlink14 = (LINKsegm *) malloc(max_lnk1*sizeof(LINKsegm));
  if(!nlink14) printf("prlib: ERROR: malloc problem (nlink14)\n"); fflush(stdout);

  link15 = (int *) malloc(max_lnk1*NVEC*sizeof(int));
  if(!link15) printf("prlib: ERROR: malloc problem (link15)\n"); fflush(stdout);

for(i=0; i<strlen(hname1); i++) hname1[i] = ' ';
for(i=0; i<strlen(format1); i++) format1[i] = ' ';


  /* allocate buffers for 2nd file */

  link21 = (int *) malloc(max_lnk2*sizeof(int));
  if(!link21) printf("prlib: ERROR: malloc problem (link21)\n"); fflush(stdout);

  link22 = (int *) malloc(max_lnk2*sizeof(int));
  if(!link22) printf("prlib: ERROR: malloc problem (link22)\n"); fflush(stdout);

  nlink23[0] = (short *) malloc(max_lnk2*sizeof(short));
  if(!nlink23[0]) printf("prlib: ERROR: malloc problem (nlink23[0])\n"); fflush(stdout);
  nlink23[1] = (short *) malloc(max_lnk2*sizeof(short));
  if(!nlink23[1]) printf("prlib: ERROR: malloc problem (nlink23[1])\n"); fflush(stdout);
  buf23 = (int *) malloc(max_lnk2*sizeof(int));
  buf23s = (short *) buf23;

  nlink24 = (LINKsegm *) malloc(max_lnk2*sizeof(LINKsegm));
  if(!nlink24) printf("prlib: ERROR: malloc problem (nlink24)\n"); fflush(stdout);

  link25 = (int *) malloc(max_lnk2*NVEC*sizeof(int));
  if(!link25) printf("prlib: ERROR: malloc problem (link25)\n"); fflush(stdout);

for(i=0; i<strlen(hname2); i++) hname2[i] = ' ';
for(i=0; i<strlen(format2); i++) format2[i] = ' ';


  /* allocate buffers for merging */

  max_lnk3 = max_lnk1 + max_lnk2;
  link31 = (int *) malloc(max_lnk3*sizeof(int));

  if(!link31) printf("prlib: ERROR: malloc problem (link31)\n"); fflush(stdout);
  link32 = (int *) malloc(max_lnk3*sizeof(int));
  if(!link32) printf("prlib: ERROR: malloc problem (link32)\n"); fflush(stdout);

  nlink33[0] = (short *) malloc(max_lnk3*sizeof(short));
  if(!nlink33[0]) printf("prlib: ERROR: malloc problem (nlink33[0])\n"); fflush(stdout);
  nlink33[1] = (short *) malloc(max_lnk3*sizeof(short));
  if(!nlink33[1]) printf("prlib: ERROR: malloc problem (nlink33[1])\n"); fflush(stdout);
  buf33 = (int *) malloc(max_lnk3*sizeof(int));
  buf33s = (short *) buf33;

  nlink34 = (LINKsegm *) malloc(max_lnk3*sizeof(LINKsegm));
  if(!nlink34) printf("prlib: ERROR: malloc problem (nlink34)\n"); fflush(stdout);

  link35 = (int *) malloc(max_lnk3*NVEC*sizeof(int));
  if(!link35) printf("prlib: ERROR: malloc problem (link35)\n"); fflush(stdout);

for(i=0; i<strlen(hname3); i++) hname3[i] = ' ';
for(i=0; i<strlen(format3); i++) format3[i] = ' ';


  /* read files record by record, merge them and write result to temporary file */

  for(i=0; i<NS2-0; i++)
  {
    /* read 1st file record */

    frname_("INPUT1",6);

    frhdr_(hname1,&numdb,&ncol,&nrow,format1,&nch,&ierr,strlen(hname1),strlen(format1));
    frdat_(&nw11,link11,&ncol);
    printf(" %s %8d %6d %6d %6d    %s\n",hname1,numdb,ncol,nrow,nw11,format1); fflush(stdout);

    frhdr_(hname1,&numdb,&ncol,&nrow,format1,&nch,&ierr,strlen(hname1),strlen(format1));
    frdat_(&nw12,link12,&ncol);
    printf(" %s %8d %6d %6d %6d    %s\n",hname1,numdb,ncol,nrow,nw12,format1); fflush(stdout);

    frhdr_(hname1,&numdb,&ncol,&nrow,format1,&nch,&ierr,strlen(hname1),strlen(format1));
    frdat_(&nw13,buf13,&ncol);
    printf(" %s %8d %6d %6d %6d    %s\n",hname1,numdb,ncol,nrow,nw13,format1); fflush(stdout);
    for(j=0; j<nw13; j++)
      for(l=0; l<2; l++)
        nlink13[l][j] = buf13s[j*2+l];

    frhdr_(hname1,&numdb,&ncol,&nrow,format1,&nch,&ierr,strlen(hname1),strlen(format1));
    frdat_(&nw14,buf14,&ncol);
    printf(" %s %8d %6d %6d %6d    %s\n",hname1,numdb,ncol,nrow,nw14,format1); fflush(stdout);

    frhdr_(hname1,&numdb,&ncol,&nrow,format1,&nch,&ierr,strlen(hname1),strlen(format1));
    /*printf("1: >%s< >%s< ncol=%d nrow=%d\n",hname1,format1,ncol,nrow); fflush(stdout);*/
    nwd = ncol * nrow;
    frdat_(&nw15,link15,&nwd);
    printf(" %s %8d %6d %6d %6d    %s\n",hname1,numdb,ncol,nrow,nw15,format1); fflush(stdout);


    /* read 2nd file record */

    frname_("INPUT2",6);

    frhdr_(hname2,&numdb,&ncol,&nrow,format2,&nch,&ierr,strlen(hname2),strlen(format2));
    frdat_(&nw21,link21,&ncol);
    printf(" %s %8d %6d %6d %6d    %s\n",hname2,numdb,ncol,nrow,nw21,format2); fflush(stdout);
    frhdr_(hname2,&numdb,&ncol,&nrow,format2,&nch,&ierr,strlen(hname2),strlen(format2));
    frdat_(&nw22,link22,&ncol);
    printf(" %s %8d %6d %6d %6d    %s\n",hname2,numdb,ncol,nrow,nw22,format2); fflush(stdout);
    frhdr_(hname2,&numdb,&ncol,&nrow,format2,&nch,&ierr,strlen(hname2),strlen(format2));
    frdat_(&nw23,buf23,&ncol);
    printf(" %s %8d %6d %6d %6d    %s\n",hname2,numdb,ncol,nrow,nw23,format2); fflush(stdout);
    for(j=0; j<nw23; j++)
      for(l=0; l<2; l++)
        nlink23[l][j] = buf23s[j*2+l];

    frhdr_(hname2,&numdb,&ncol,&nrow,format2,&nch,&ierr,strlen(hname2),strlen(format2));
    frdat_(&nw24,buf24,&ncol);
    printf(" %s %8d %6d %6d %6d    %s\n",hname2,numdb,ncol,nrow,nw24,format2); fflush(stdout);
    l = 0;
    for(j=0; j<max_lnk2; j++)
    {
      for(is=0; is<6; is++)
      {
        nlink24[j].npat[is] = buf24[l++];
        for(in=0; in<nlink24[j].npat[is]; in++)
        {
          nlink24[j].ipat[is][in] = buf24[l++];
        }
      }
    }

    frhdr_(hname2,&numdb,&ncol,&nrow,format2,&nch,&ierr,strlen(hname2),strlen(format2));
    /*printf("1: >%s< >%s< ncol=%d nrow=%d\n",hname2,format2,ncol,nrow); fflush(stdout);*/
    nwd = ncol * nrow;
    frdat_(&nw25,link25,&nwd);
    printf(" %s %8d %6d %6d %6d    %s\n",hname2,numdb,ncol,nrow,nw25,format2); fflush(stdout);

    printf("\n"); fflush(stdout);

    /*********/
    /* merge */
    /*********/

    lnk3[i] = nw11;

    bcopy((char *)link11, (char *)link31, nw11*sizeof(int));
    bcopy((char *)link12, (char *)link32, nw12*sizeof(int));

    for(j=0; j<max_lnk1; j++)
      for(l=0; l<2; l++)
        nlink33[l][j] = buf13s[j*2+l];

    l = 0;
    for(j=0; j<max_lnk1; j++)
    {
      for(is=0; is<6; is++)
      {
        nlink34[j].npat[is] = buf14[l++];
        for(in=0; in<nlink34[j].npat[is]; in++)
        {
          nlink34[j].ipat[is][in] = buf14[l++];
        }
      }
    }

    bcopy((char *)link15, (char *)link35, nw15*sizeof(int));

    for(j=0; j<nw21; j++)
    {
      tmp1 = link21[j];
      tmp2 = link22[j];
      skip = 0;
      for(k=0; k<nw11; k++)
      {
        if(link11[k] == tmp1 && link12[k] == tmp2) {skip=1;break;}
      }
      if(!skip)
      {
        l = lnk3[i];
        link31[l] = link21[j];

        link32[l] = link22[j];

        nlink33[0][l] = nlink23[0][j];
        nlink33[1][l] = nlink23[1][j];


        for(is=0; is<6; is++)
        {
          nlink34[l].npat[is] = nlink24[j].npat[is];
          for(in=0; in<nlink24[j].npat[is]; in++)
          {
            nlink34[l].ipat[is][in] = grlookup[nlink24[j].ipat[is][in]];
          }
        }

        for(in=0; in<NVEC; in++) link35[l*NVEC+in] = link25[j*NVEC+in];

        lnk3[i] ++;
      }
    }


    /* write temporary file */

    hname3 = "DAXI";
    format3 = "B08";
    ncol = lnk3[i];
    nrow = 4;
    numdb = i;
    fwhdr_(hname3,&numdb,&ncol,&nrow,format3,strlen(hname3),strlen(format3));
    fwdat_(&ncol,link31);
    printf(" %s %8d %6d %6d %6d    %s\n",hname3,numdb,ncol,nrow,max_lnk3,format3); fflush(stdout);

    hname3 = "DSTE";
    numdb = i;
    fwhdr_(hname3,&numdb,&ncol,&nrow,format3,strlen(hname3),strlen(format3));
    fwdat_(&ncol,link32);
    printf(" %s %8d %6d %6d %6d    %s\n",hname3,numdb,ncol,nrow,max_lnk3,format3); fflush(stdout);

    hname3 = "DMOM";
    format3 = "B16";
    nrow = 2;
    for(j=0; j<ncol; j++)
    {
      for(l=0; l<2; l++)
      {
        buf33s[j*2+l] = nlink33[l][j];
      }
    }
    numdb = i;
    fwhdr_(hname3,&numdb,&ncol,&nrow,format3,strlen(hname3),strlen(format3));
    fwdat_(&ncol,buf33);
    printf(" %s %8d %6d %6d %6d    %s\n",hname3,numdb,ncol,nrow,max_lnk3,format3); fflush(stdout);

    hname3 = "DSLP";
    format3 = "B08";
    ncol1 = ncol;
    nrow = 1;
    ncol = 0;
    for(j=0; j<ncol1; j++)
    {
      for(is=0; is<6; is++)
      {
        buf34[ncol++] = nlink34[j].npat[is];
        for(in=0; in<nlink34[j].npat[is]; in++)
        {
          buf34[ncol++] = nlink34[j].ipat[is][in];
        }
      }
    }
    numdb = i;
    fwhdr_(hname3,&numdb,&ncol,&nrow,format3,strlen(hname3),strlen(format3));
    ncol = (ncol + 3)/4;
    fwdat_(&ncol,buf34);
    printf(" %s %8d %6d %6d %6d    %s\n",hname3,numdb,ncol,nrow,max_lnk3,format3); fflush(stdout);

    hname3 = "DVEC";
    format3 = "I";
    ncol = ncol1;
    nrow = NVEC;
    numdb = i;
    fwhdr_(hname3,&numdb,&ncol,&nrow,format3,strlen(hname3),strlen(format3));
    j = ncol*nrow;
    fwdat_(&j,link35);
    printf(" %s %8d %6d %6d %6d    %s\n",hname3,numdb,ncol,nrow,max_lnk3,format3); fflush(stdout);

  }

  fwend_(&ierr);
  if(ierr != 0) printf(" *** error in fwend \n");
  fweod_();
  fparm_("CLOSE OUTPU1",12);

  fparm_("CLOSE UNIT=11",13);
  fparm_("CLOSE UNIT=12",13);

  /***************************************************************/
  /* write segment record to final file and copy temp file there */
  /***************************************************************/

  fparm_(str4,strlen(str4));
  fwname_("OUTPU2",6);
  numra = numrb = icl = 0;
  fwkey_(kname3,&numra,&numrb,&icl,strlen(kname3));
  hname3 = "DSEG";
  format3 = "I";
  numdb = 1;
  max_npat = MAX_NPAT;
  ncol = 1 + 6 * max_npat + 2 + NS2 + 1;
  nrow = 1;
  fwhdr_(hname3,&numdb,&ncol,&nrow,format3,strlen(hname3),strlen(format3));
  k = 0;
  buf30 = (int *) malloc(ncol*sizeof(int));
  buf30[k++] = npat3;
printf("new npat = %d\n",npat3);  fflush(stdout);
  for(j=0; j<npat3; j++)
  {
    printf("== %3d == %3d %3d %3d %3d %3d %3d\n",j,ipat[j][0],ipat[j][1],ipat[j][2],ipat[j][3],ipat[j][4],ipat[j][5]);
    for(i=0; i<6; i++)
    {
      buf30[k++] = ipat[j][i];
    }
  }
  for(j=npat3; j<max_npat; j++)
    for(i=0; i<6; i++)
      buf30[k++] = 0;
  /*if(strncmp(knam,"LTORM",5) == 0)*/ buf30[k++] = 3;
  getunixtime_(&buf30[k++]); /* unix time */
  for(i=0; i<NS2; i++) buf30[k++] = lnk3[i];
  max_lnk3 = 0;
  for(i=0; i<NS2; i++) max_lnk3 = MAX(max_lnk3,lnk3[i]);
  buf30[k++] = max_lnk3;
  fwdat_(&ncol,buf30);
  printf(" %s %8d %6d %6d %6d    %s\n",hname3,numdb,ncol,nrow,ncol,format3); fflush(stdout);



  kname3="LTORM   ";
  hname3="        ";
  format3="               ";

  fparm_(str33,strlen(str33));
  frname_("OUTPU1",6);
  frkey_(kname3,&numra,&numrb,&icl,&ierr,strlen(kname3));
  printf("open temporary file: >%s< %d %d %d %d\n",kname3,numra,numrb,icl,ierr);

  for(i=0; i<NS2-0; i++)
  {
    frhdr_(hname3,&numdb,&ncol,&nrow,format3,&nch,&ierr,strlen(hname3),strlen(format3));
    frdat_(&nw31,link31,&ncol);
    printf(" %s %8d %6d %6d %6d    %s\n",hname3,numdb,ncol,nrow,nw31,format3); fflush(stdout);
    fwhdr_(hname3,&numdb,&ncol,&nrow,format3,strlen(hname3),strlen(format3));
    fwdat_(&ncol,link31);


    frhdr_(hname3,&numdb,&ncol,&nrow,format3,&nch,&ierr,strlen(hname3),strlen(format3));
    frdat_(&nw32,link32,&ncol);
    printf(" %s %8d %6d %6d %6d    %s\n",hname3,numdb,ncol,nrow,nw32,format3); fflush(stdout);
    fwhdr_(hname3,&numdb,&ncol,&nrow,format3,strlen(hname3),strlen(format3));
    fwdat_(&ncol,link32);


    frhdr_(hname3,&numdb,&ncol,&nrow,format3,&nch,&ierr,strlen(hname3),strlen(format3));
    frdat_(&nw33,buf33,&ncol);
    printf(" %s %8d %6d %6d %6d    %s\n",hname3,numdb,ncol,nrow,nw33,format3); fflush(stdout);
    fwhdr_(hname3,&numdb,&ncol,&nrow,format3,strlen(hname3),strlen(format3));
    fwdat_(&ncol,buf33);


    frhdr_(hname3,&numdb,&ncol,&nrow,format3,&nch,&ierr,strlen(hname3),strlen(format3));
    ncol = (ncol + 3)/4;
    frdat_(&nw34,buf34,&ncol);
    printf(" %s %8d %6d %6d %6d    %s\n",hname3,numdb,ncol,nrow,nw34,format3); fflush(stdout);
    fwhdr_(hname3,&numdb,&ncol,&nrow,format3,strlen(hname3),strlen(format3));
    fwdat_(&ncol,buf34);


    frhdr_(hname3,&numdb,&ncol,&nrow,format3,&nch,&ierr,strlen(hname3),strlen(format3));
    nwd = ncol * nrow;
    frdat_(&nw35,link35,&nwd);
    printf(" %s %8d %6d %6d %6d    %s\n",hname3,numdb,ncol,nrow,nw35,format3); fflush(stdout);
    fwhdr_(hname3,&numdb,&ncol,&nrow,format3,strlen(hname3),strlen(format3));
    fwdat_(&nw35,link35);


  }

  fwend_(&ierr);
  if(ierr != 0) printf(" *** error in fwend \n");
  fweod_();
  fparm_("CLOSE OUTPU2",12);

  fparm_("CLOSE OUTPU1",12);

  printf("removing temporary file /tmp/tmp.bos ...\n"); fflush(stdout);
  system("rm -rf /tmp/tmp.bos");

  exit(0);
}










