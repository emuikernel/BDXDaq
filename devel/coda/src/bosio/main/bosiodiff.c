/* bosiodiff.c - compare two FPACK files */

#include <stdio.h>

#define MAXDATA 100000

main(int argc, char **argv)
{
  int i,status,ierr,eof,eor;
  int key1,numdb1,ncol1,nrow1,nch1,numra1,numrb1,icl1,nwd1;
  int key2,numdb2,ncol2,nrow2,nch2,numra2,numrb2,icl2,nwd2;
  char answer, tmp[1000];
  int max_data = MAXDATA;
  int data1[MAXDATA], data2[MAXDATA];
  char *kname1="        ", *hname1="        ", *format1="               ";
  char *kname2="        ", *hname2="        ", *format2="               ";

  printf("argc=%d\n",argc);
  for(i=0; i<argc; i++) printf("argv[%d]=%s\n",i,argv[i]);

  if(argc != 3) { printf("\n   Usage: bosiodiff file1.ext file2.ext\n\n"); exit(0); }

  sprintf(tmp,"OPEN BOSIN11 UNIT=11 FILE='%s' READ OLD BINARY",argv[1]);
  printf("Opening file ->%s<-\n",tmp);
  status = fparm_(tmp,strlen(tmp));

  sprintf(tmp,"OPEN BOSIN12 UNIT=12 FILE='%s' READ OLD BINARY",argv[2]);
  printf("Opening file ->%s<-\n",tmp);
  status = fparm_(tmp,strlen(tmp));





  key1 = 0;
  key2 = 0;
  eof = 0;
  while(!eof)  /* loop over all input records */
  {

    /* FIRST FILE */

    frname_("BOSIN11",7);

    for(i=0; i<strlen(kname1); i++) kname1[i] = ' ';
    frkey_(kname1,&numra1,&numrb1,&icl1,&ierr,8);
    if(ierr == -1) eof = 1;

    if(!eof)  /* dump each record key */
    {
      key1++;
      if(ierr != 0) printf("?bosiodiff...error on frkey: %d\n",ierr);
      printf(" key = %d  name = %s  numra = %d  numrb = %d  icl = %d\n",
               key1,kname1,numra1,numrb1,icl1);
      /* dump all headers in this record */
      printf(" hname       numdb   ncol   nrow    nwd    format\n");
      printf(" ---------   -----   ----   ----    ---    ---------------\n");
      eor = 0;
      while(!eor)
      {
        for(i=0; i<strlen(hname1); i++) hname1[i] = ' ';
        for(i=0; i<strlen(format1); i++) format1[i] = ' ';
        frhdr_(hname1,&numdb1,&ncol1,&nrow1,format1,&nch1,&ierr,8,15);
        if(ierr == -1) eor = 1;
        if(!eor)
        {
          if(ierr != 0)
          {
            printf("?bosiodiff...error on frhdr: %d\n",ierr);
          }
          else
          {
            frdat_(&nwd1,data1,&max_data);
            printf(" %s %8d %6d %6d %6d    %s\n",hname1,numdb1,ncol1,nrow1,nwd1,format1);
          }
        }
      }
      printf("\n\n");
    }

    /* SECOND FILE */

    frname_("BOSIN12",7);

    for(i=0; i<strlen(kname2); i++) kname2[i] = ' ';
    frkey_(kname2,&numra2,&numrb2,&icl2,&ierr,8);
    if(ierr == -1) eof = 1;

    if(!eof)  /* dump each record key */
    {
      key2++;
      if(ierr != 0) printf("?bosiodiff...error on frkey: %d\n",ierr);
      printf(" key = %d  name = %s  numra = %d  numrb = %d  icl = %d\n",
               key2,kname2,numra2,numrb2,icl2);
      /* dump all headers in this record */
      printf(" hname       numdb   ncol   nrow    nwd    format\n");
      printf(" ---------   -----   ----   ----    ---    ---------------\n");
      eor = 0;
      while(!eor)
      {
        for(i=0; i<strlen(hname2); i++) hname2[i] = ' ';
        for(i=0; i<strlen(format2); i++) format2[i] = ' ';
        frhdr_(hname2,&numdb2,&ncol2,&nrow2,format2,&nch2,&ierr,8,15);
        if(ierr == -1) eor = 1;
        if(!eor)
        {
          if(ierr != 0)
          {
            printf("?bosiodiff...error on frhdr: %d\n",ierr);
          }
          else
          {
            frdat_(&nwd2,data2,&max_data);
            printf(" %s %8d %6d %6d %6d    %s\n",hname2,numdb2,ncol2,nrow2,nwd2,format2);
          }
        }
      }
      printf("\n\n");
    }

    /* COMPARISION */

    if(strcmp(kname1,kname2))   printf("diff  1 !!!\n");
    if(numra1 != numra2)        printf("diff  2 !!!\n");
    if(numrb1 != numrb2)        printf("diff  3 !!!\n");
    if(icl1 != icl2)            printf("diff  4 !!!\n");
    if(strcmp(hname1,hname2))   printf("diff  5 !!!\n");
    if(numdb1 != numdb2)        printf("diff  6 !!!\n");
    if(ncol1 != ncol2)          printf("diff  7 !!!\n");
    if(nrow1 != nrow2)          printf("diff  8 !!!\n");
    if(strcmp(format1,format2)) printf("diff  9 !!!\n");
    if(nch1 != nch2)            printf("diff 10 !!!\n");
    if(nwd1 != nwd2)            printf("diff 11 !!!\n");
    for(i=0; i<nwd1; i++) if(data1[i] != data2[i]) {printf("diff 12 !!!\n"); break;}

    printf(" enter return to continue, q to quit: ");
    answer = getchar();
    if(answer=='q' || answer=='Q') eof = 1;
  }

  /* close file */
  status = fparm_("CLOSE UNIT=11",13);

  exit(0);
}









