
/* bosdump.c - dump BOS file */

#include <stdio.h>

#include "bosio.h"

#define MAXDATA 4000000

static int iw[MAXDATA];

int
main(int argc, char **argv)
{
  static int data[MAXDATA];
  int i,status,ikey,ierr,eof,eor,numdb,ncol,nrow,nch,numra,numrb,icl,nwd;
  int max_data = MAXDATA;
  char answer, tmp[1000];
  char *kname="        ", *hname="        ", *format="               ";
  int ind;
  BOSIO *handle;

  printf("argc=%d\n",argc);
  for(i=0; i<argc; i++) printf("argv[%d]=%s\n",i,argv[i]);

  if(argc != 2) { printf("\n   Usage: bosdump 'filename'\n\n"); exit(0); }




  bosInit(iw,MAXDATA);

  status = bosOpen(argv[1],"r",&handle);
  printf("after bosOpen() status=%d\n",status);


  eof=0;
  while(!eof)  /* loop over all input records */
  {

    /*ind = bosNdrop(iw,"+KYE",0);*/

    status=bosRead(handle,iw,"E");
    if(status != 0) eof=1;
    if(eof == 1) break;

    bosLprint(iw, "E");

    if((i=bosLdrop(iw,"E")) < 0)
    {
      printf("Error in bosLdrop, number %d\n",i);
      exit(1);
    }
    if((i=bosNgarbage(iw)) < 0)
    {
      printf("Error in bosNgarbage, number %d\n",i);
      exit(1);
    }

    printf(" enter return to continue, q to quit: ");
    answer = getchar();
    if(answer=='q' || answer=='Q') eof = 1;
  }
  status = bosClose(handle);

 




exit(0);




  sprintf(tmp,"OPEN BOSINPUT UNIT=11 FILE='%s' READ OLD BINARY",argv[1]);
  printf("Opening file ->%s<-\n",tmp);
  status = fparm_(tmp,strlen(tmp));
  frname_("BOSINPUT",8);

  ikey=0;
  eof=0;
  while(!eof)  /* loop over all input records */
  {
    for(i=0; i<strlen(kname); i++) kname[i] = ' ';

    frkey_(kname,&numra,&numrb,&icl,&ierr,8);

    if(ierr == -1) eof = 1;

    if(!eof)  /* dump each record key */
    {
      ikey++;
      if(ierr != 0) printf("?bosiodump...error on frkey: %d\n",ierr);
      printf(" key = %d  name = %s  numra = %d  numrb = %d  icl = %d\n",
               ikey,kname,numra,numrb,icl);
      /* dump all headers in this record */
      printf(" hname       numdb   ncol   nrow    nwd    format\n");
      printf(" ---------   -----   ----   ----    ---    ---------------\n");
      eor = 0;
      while(!eor)
      {
        for(i=0; i<strlen(hname); i++) hname[i] = ' ';
        for(i=0; i<strlen(format); i++) format[i] = ' ';

        frhdr_(hname,&numdb,&ncol,&nrow,format,&nch,&ierr,8,15);

        if(ierr == -1) eor = 1;
        if(!eor)
        {
          if(ierr != 0)
          {
            printf("?bosiodump...error on frhdr: %d\n",ierr);
          }
          else
          {
			frdat_(&nwd,data,&max_data);

            printf(" %s %8d %6d %6d %6d    %s\n",hname,numdb,ncol,nrow,nwd,format);
          }
        }
      }
      printf("\n\n");
    }
    printf(" enter return to continue, q to quit: ");
    answer = getchar();
    if(answer=='q' || answer=='Q') eof = 1;
  }

  /* close file */
  status = fparm_("CLOSE UNIT=11",13);

  exit(0);
}







