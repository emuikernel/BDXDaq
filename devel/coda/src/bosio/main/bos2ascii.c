/* bos2ascii.c - dump FPACK file to ASCII file */

#include <stdio.h>

#define MAXDATA 4000000

main(int argc, char **argv)
{
  FILE *fd;
  static int data[MAXDATA];
  int i,status,ikey,ierr,eof,eor,numdb,ncol,nrow,nch,numra,numrb,icl,nwd;
  int max_data = MAXDATA;
  char answer, tmp[1000];
  char *kname="        ", *hname="        ", *format="               ";

  printf("argc=%d\n",argc);
  for(i=0; i<argc; i++) printf("argv[%d]=%s\n",i,argv[i]);

  if(argc != 2) { printf("\n   Usage: bos2ascii file.ext\n\n"); exit(0); }

  sprintf(tmp,"OPEN BOSINPUT UNIT=11 FILE='%s' READ OLD BINARY",argv[1]);
  printf("Opening file ->%s<-\n",tmp);
  status = fparm_(tmp,strlen(tmp));
  frname_("BOSINPUT",8);


  /* open output ascii file */
  if( (fd = fopen("./ascii.out","w")) == NULL )
  {
    printf("bos2ascii: can not open output file - exit.\n");
    exit(0);
  }

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
      if(ierr != 0) printf("?bos2ascii...error on frkey: %d\n",ierr);
      /*
      printf(" key = %d  name = %s  numra = %d  numrb = %d  icl = %d\n",
               ikey,kname,numra,numrb,icl);
      */
      /* dump all headers in this record */
      /*
      printf(" hname       numdb   ncol   nrow    nwd    format\n");
      printf(" ---------   -----   ----   ----    ---    ---------------\n");
      */
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
            printf("?bos2ascii...error on frhdr: %d\n",ierr);
          }
          else
          {
			frdat_(&nwd,data,&max_data);
            /*
            printf(" %s %8d %6d %6d %6d    %s\n",hname,numdb,ncol,nrow,nwd,format);
            */
/********************************/
/* write whatever you want here */
/********************************/
if(!strncmp("RC29",hname,4))
{
  if(numdb == 0)
  {
    fprintf(fd,"%6d",data[1]&0xFFF);
    fprintf(fd,"%6d",data[5]&0xFFF);
    fprintf(fd,"%6d",data[9]&0xFFF);
    fprintf(fd,"%6d",data[13]&0xFFF);
  }

  if(numdb == 1)
  {
    fprintf(fd,"%6d",data[1]&0xFFF);
    fprintf(fd,"%6d",data[5]&0xFFF);
    fprintf(fd,"%6d",data[9]&0xFFF);
    fprintf(fd,"%6d",data[13]&0xFFF);
    fprintf(fd,"\n");
  }
}
/********************************/
/********************************/
/********************************/

          }
        }
      }
      /*
      printf("\n\n");
      */
    }
/*
    printf(" enter return to continue, q to quit: ");
    answer = getchar();
    if(answer=='q' || answer=='Q') eof = 1;
*/
  }

  /* close files */
  status = fparm_("CLOSE UNIT=11",13);
  fclose(fd);

  exit(0);
}

