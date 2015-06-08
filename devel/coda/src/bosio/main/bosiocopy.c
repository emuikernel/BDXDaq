/* bosiocopy.c - copy one FPACK file to another */

#include <stdio.h>

/*#define PRINT 1*/
#define MAXDATA 10000000

main(int argc, char **argv)
{
  static int data[MAXDATA];
  int i,status,ikey,ierr,eof,eor,numdb,ncol,nrow,nch,numra,numrb,icl,nwd;
  int max_data = MAXDATA;
  char answer, tmp1[1000], tmp2[1000];

  char kname[9], hname[9],  format[1000];
  kname[8] = '\0';
  hname[8] = '\0';
  format[999] = '\0';

  printf("argc=%d\n",argc);
  for(i=0; i<argc; i++) printf("argv[%d]=%s\n",i,argv[i]);

  if(argc != 3)
  {
    printf("\n   Usage: bosiocopy <infile> <outfile>\n\n");
    exit(0);
  }

  sprintf(tmp1,"OPEN BOSINPUT UNIT=11 FILE='%s' READ OLD BINARY",argv[1]);
  printf("Opening file ->%s<-\n",tmp1);
  status = fparm_(tmp1,strlen(tmp1));
  frname_("BOSINPUT",8);

  sprintf(tmp2,"OPEN BOSOUTPU UNIT=12 FILE='%s'  WRITE RECL=32768 NEW",argv[2]);
  printf("Opening file ->%s<-\n",tmp2);
  status = fparm_(tmp2,strlen(tmp2));
  fwname_("BOSOUTPU",8);

  ikey=0;
  eof=0;
  while(!eof)  /* loop over all input records */
  {
    for(i=0; i<strlen(kname); i++) kname[i] = ' ';
    frkey_(kname,&numra,&numrb,&icl,&ierr,8);
    if(ierr == -1) eof = 1;
    else if(ierr != 0) printf("?bosiocopy...error on frkey: %d\n",ierr);

    if(!eof)  /* dump each record key */
    {
      ikey++;
      fwkey_(kname,&numra,&numrb,&icl,strlen(kname));
      if(ierr != 0) printf("?bosiocopy...error on fwkey: %d\n",ierr);
#ifdef PRINT
      printf(" key = %d  name = %s  numra = %d  numrb = %d  icl = %d\n",
               ikey,kname,numra,numrb,icl);
      /* dump all headers in this record */
      printf(" hname       numdb   ncol   nrow    nwd    format\n");
      printf(" ---------   -----   ----   ----    ---    ---------------\n");
#endif
      eor = 0;
      while(!eor)
      {
        for(i=0; i<strlen(hname); i++) hname[i] = ' ';
        for(i=0; i<strlen(format); i++) format[i] = ' ';

        frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,
               8,999);
        if(ierr == -1) eor = 1;
        else if(ierr != 0) printf("?bosiocopy...error on frhdr: %d\n",ierr);

        if(!eor)
        {
          frdat_(&nwd,data,&max_data); 
#ifdef PRINT
          printf(" %s %8d %6d %6d %6d    %s\n",hname,numdb,ncol,nrow,nwd,format);
#endif
          fwhdr_(hname,&numdb,&ncol,&nrow,format,
                 strlen(hname),strlen(format));
          fwdat_(&nwd,data);
        }
      }
#ifdef PRINT
      printf("\n\n");
#endif
      fwend_(&ierr);
    }
#ifdef PRINT
    printf(" enter return to continue, q to quit: ");
    answer = getchar();
    if(answer=='q' || answer=='Q') eof = 1;
#endif
  }

  /* close input file */
  status = fparm_("CLOSE UNIT=11",13);

  /* close output file */
  fweod_();
  status = fparm_("CLOSE UNIT=12",13);

  exit(0);
}
