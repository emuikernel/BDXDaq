h30337
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/copen.c
e
s 00219/00000/00000
d D 1.1 00/08/10 11:10:10 boiarino 1 0
c date and time created 00/08/10 11:10:10 by boiarino
e
u
U
f e 0
t
T
I 1

/* copen.c - lowest level open routine */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "ioincl.h"
#include "biofun.h"

/* #define COND_DEBUG */
/* #define DEBUG */ 
#include "dbgpr.h"
 
 
/* open a local file */

/*  Defined in the ioincl.h as external */ 
FILE *filepointer[NUNITS]; 
 
int
copen(int *stream, char *filename, char *options, int medium, int blksize,
          int *bufptr, int reopen)
{
  FILE *file;
  char *sp, *bufio;
  long  buflen;
#ifdef APOLLO
  int   count;
#endif
#ifdef IRIX
  struct mtop top;
#endif
 
  *bufptr = 0;
 
  DPR7("copen : stream=%i fn='%s' options='%s' medium=%i blksize=%i bufptr=%X reopen=%i\n"
       ,stream,filename,options,medium,blksize,bufptr,reopen); 
  if(medium == 2) 
  {
    int opt;
    DPR("copen : medium = 2 -> LowLevel System IO\n"); 

/*
    int opt,r,w,a;
    r=w=a=0;
    if ( strchr(options,'r') != NULL ) r = 1;
    if ( strchr(options,'w') != NULL ) w = 1;
    if ( strchr(options,'a') != NULL ) a = 1;
    if (w==1)
    {
      if (r==1) opt = O_RDWR;
      else      opt = O_WRONLY | O_TRUNC;
      opt = opt | O_CREAT ;
    }
    else
    {
      opt = O_RDONLY;
    }
    if (a == 1) opt = opt | O_APPEND | O_CREAT ;
*/
    if(!strncmp(options,"rb",2))       /* open file for reading */
	{
      opt = O_RDONLY;
	}
    else if(!strncmp(options,"wb",2))  /* truncate to zero length */
	{                                  /* or create file for writing */
      opt = O_WRONLY | O_TRUNC | O_CREAT;
	}
    else if(!strncmp(options,"a+b",3)) /* open or create file for update, */
	{                                  /* writing at end-of-file */
      opt = O_RDWR | O_APPEND | O_CREAT;
	}
    else if(!strncmp(options,"r+b",3)) /* open file for update */
    {                                  /* (reading and writing) */
      opt = O_RDWR;
	}
    else
    {
      fprintf(stderr,"unknown options >%s<\n",options);
      perror("copen ");
      fflush(stderr);
      return(-7);
    }


/*
opt = opt | O_NONBLOCK;
printf("NDELAY ...\n");
*/

    if( (int)(file = (FILE *)open(filename, opt)) == -1)
    {
      if (!reopen)
      {
        fprintf(stderr,"copen: failed to open(\"%s\", \"%s\")\n",filename,options);
        perror("open");
        fflush(stderr);
        return(errno);
      }
      else
      {
        fprintf(stderr,"copen: INFO: Splitted files are expired -> end-of-run.\n");
        perror("open");
        fflush(stderr);
        return(-1);
      }
    }

    /* set permission bits:

      S_ISUID   04000   Set user ID on execution.
      S_ISGID   020#0   Set group ID on execution
                        if # is 7, 5, 3, or 1.
                        Enable mandatory file/record locking
                        if # is 6, 4, 2, or 0.
      S_ISVTX   01000   Save text image  after execution.
      S_IRWXU   00700   Read, write, execute by owner.
      S_IRUSR   00400   Read by owner.
      S_IWUSR   00200   Write by owner.
      S_IXUSR   00100   Execute (search if a directory) by owner.
      S_IRWXG   00070   Read, write, execute by group.
      S_IRGRP   00040   Read by group.
      S_IWGRP   00020   Write by group.
      S_IXGRP   00010   Execute by group.
      S_IRWXO   00007   Read, write, execute (search) by others.
      S_IROTH   00004   Read by others.
      S_IWOTH   00002   Write by others.
      S_IXOTH   00001   Execute by others.

    */

    fchmod ((int)file, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
    DPR1("copen : handler -> %x\n",file); 
  }
  else if(medium == 0)
  {
    if((file = fopen(filename, options)) == NULL)
    {
      if (!reopen)
      {
        fprintf(stderr,"copen: failed to fopen(\"%s\", \"%s\")\n",filename,options);
        perror("copen ");
        fflush(stderr);
        return(errno);
      }
      else
      {
        fprintf(stderr,"copen: INFO: Splitted files are expired -> end-of-run.\n");
        perror("copen ");
        fflush(stderr);
        return(-1);
      }
    }
  }
  else if(medium == 1)	/* allocate the I/O buffer for tapes */
  {
#ifdef APOLLO
    fprintf(stderr," Exabyte I/O not yet implemented \n");
    fflush(stderr);
    fclose(file);
    return(EBIO_EOF);
#endif
#ifdef IRIX
    buflen = blksize + 8;
    /* on IRIX one needs a buffer longer by exactly 8 bytes then blksize !!! */
#else
    buflen = blksize;
#endif
    if((bufio = (char *) MALLOC(buflen)) == (char *)NULL)
    {
      fprintf(stderr,"copen: Cannot allocate IO buffer.\n");
      perror("copen ");
      fflush(stderr);
      fclose(file);
      return(EBIO_EOF);
    }
    *bufptr = (long)bufio;
#ifdef IRIX
    top.mt_op =  MTSCSI_SETFIXED;
    top.mt_count = blksize;
    ioctl(fileno(file), MTSPECOP, &top);
    /* On IRIX the tape file must be closed and reopened after
         changing the blocksize. Otherwise, the first I/O fails!   */
    fclose(file);
 
    if((file = fopen(filename, options)) == NULL) return(errno);
#endif
#ifdef ALLIANT
    setbuffer(file,bufio,buflen);
#else
    setvbuf(file,bufio,_IOFBF,buflen);
#endif
  }

/*****************************************************************
 * Now we have a file pointer to store in the array filepointer. *
 * Look for an empty slot.                                       *
 *****************************************************************/

  if (!reopen)
  {
    for( *stream = 0; filepointer[*stream] != (FILE *) NULL; (*stream)++ ) ;
    if( *stream >= NUNITS )
    {
      *stream = -99;
      fprintf(stderr,"copen: no empty slot for filepointer found.\n");
      perror("copen ");
      fflush(stderr);
      return(EBIO_EOF);
    }
  }

  filepointer[*stream] = file;

  return(0);
}
 

E 1
