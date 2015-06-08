
/*  bdump.c - from Wolin's bosdump.cc by Sergey */


#include <stdio.h>
#include <stdlib.h>
#include <strings.h>



static int done    	= 0;
static int nopause 	= 0;
static int skip   	= 0;	   
static int nskip   	= 0;	   
static int extract     = 0;
static int evtype       = -9999;
static int evclass      = -9999;
static int nextract    = 0;
static int hex_dump    = 0;
static int nbankdump   = 0;
static char *outfile    = (char *)"out.evt";
static char *filename;
static char *bankname[100];
static int nbankname    = 0;
static int bank_printed = 0;
static int found_one    = 0;

/* size of bos aray */
#define NBCS 10000000


#include <bosio.h>
#include <bos.h>
#include <bcs.h>


void decode_command_line(int argc, char **argv);

#define MIN(a,b)          ( (a) < (b) ? (a) : (b) )

//----------------------------------------------------------------------------

int
main(int argc, char **argv)
{
  BOSIO *descriptor, *outdescriptor;
  int status, i, j, k, l, nlong, ind, name, nr, nbank, head;
  int nevent     = 0;
  int nbank_tot  = 0;
  int *ptr, *fmtptr;
  char answer;


  /* get file name, etc. */
  decode_command_line(argc,argv);


  /* initialize bos */
  status = bosInit(bcs_.iw,NBCS);


  /* open file */
  status = bosOpen(filename,(char*)"r",&descriptor);


  /* open output file if requested */
  if(extract>0) status = bosOpen(outfile,(char*)"w",&outdescriptor);


  /* read, process, and optionally extract events */
  while((done==0)&&((extract<=0)||(extract-nextract>0))&&
	(status=bosRead(descriptor,bcs_.iw,(char*)"E"))==0)
  {
    nevent++;
    bank_printed=0;
    if(nevent>skip)
    {
      if(extract<=0)
      {
		/* check for event type or class */
	    head=bosNlink(bcs_.iw,(char*)"HEAD",0);
	    if(head>0)
        {
	      if((evtype!=-9999)&&(bcs_.iw[head+4]!=evtype))continue;    
	      if((evclass!=-9999)&&(bcs_.iw[head+6]!=evclass))continue;
	    }
	
        nbank = bosLdump(bcs_.iw,(char*)"E",&ptr,&fmtptr);

	    if(nbankname==0)
        {
	      printf("\n\nStarting dump of event number %d ",nevent); 
	      printf(" containing %d total banks\n",nbank);
	    }
	
		/* loop over all banks in event */
	    for(i=0; i<nbank; i++)
        {
	      ind=ptr[i]-1;
	      name=bcs_.iw[ind-INAM];
	      nr=bcs_.iw[ind-INR];
	      nbank_tot++;

	      /* print if bank matches any in list */
	      found_one=0;
	      for(j=0; j<nbankname; j++)
            if(strncmp(bankname[j],(char *)&name,4)==0) found_one=1;

	      if((nbankname==0)||(found_one==1))
          {
	        nbankdump++;
	        bank_printed=1;
	        printf("\n\n  - - - - bank %d event %d",i,nevent); 
		    printf(" ( %d banks dumped, ",nbankdump); 
		    printf(" %d banks read so far) - - - -\n",nbank_tot);

	        if(hex_dump==0)
            {
	          bosNprint(bcs_.iw,(char *)&name,nr);
	        }
            else /*hex dump */
            {
              nlong = bcs_.iw[ind];
	          printf("\n  Bank name: %4.4s  Bank number: %d  Length: %d\n",(char *)&name,nr,nlong);
	          for(k=0; k<nlong; k+=8)
              {
                printf("%4d:",k);
		        for(l=k; l<MIN(k+8,nlong); l++)
                {
                  printf("  0x%08x",bcs_.iw[ind+l+1]);
		        }
                printf("\n");
	          }
	        }

	      }
	    }
	
	    if(nbankname==0)
        {
	      printf("\n\n=============== end of event %d containing %d banks ===============\n",nevent,nbank);
	    }
	
	    if((nopause==0)&&((nbankname==0)||(bank_printed==1)))
        {
	      printf("\n\n hit return for next event, q to quit: ");
          answer = getchar();
          if(answer=='q' || answer=='Q') done = 1;
	    }
	    fflush(stdout);
	
      }
      else
      {
	    bosWrite(outdescriptor,bcs_.iw,(char*)"E");
	    nextract++;
      }

    }
    else /* skip loop */
	{
      nskip++;
    }

    bosLdrop(bcs_.iw,(char*)"E");
    bosNgarbage(bcs_.iw);

  } /* read loop */


  /* finished reading/writing events...close files */
  bosClose(descriptor);
  if(extract>0)
  {
    bosWrite(outdescriptor,bcs_.iw,(char*)"0");
    bosClose(outdescriptor);
  }

  /* report read error */
  if((status!=-1)&&(status!=0))
  {
    printf("\n?read error event %d, status is %d\n",nevent,status);
  }

  /* done */
  printf("\n\nbosdump read %d events, skipped %d events, extracted %d events, and printed %d banks from %d events\n",
    nevent,nskip,nextract,nbankdump,nevent-nskip);
  printf("\n  *** bosdump done ***\n");

  exit(EXIT_SUCCESS);
}


/*---------------------------------------------------------------------------*/


void
decode_command_line(int argc, char **argv)
{

  const char *help = "\nusage:\n\n   bosdump [-skip skip] [-nopause] [-extract filename] "
    "[-bank bankname] [-hex] [-type evtype] [-class evclass] file.ext\n";


  /* error if no command line args */
  if(argc<2)
  {
    printf("%s",help);;
    exit(EXIT_SUCCESS);
  }
  

  int i=1;
  while(i<argc)
  {
    if (strncasecmp(argv[i],"-nopause",8)==0)
    {
      nopause=1;
      i=i+1;

    }
    else if (strncasecmp(argv[i],"-skip",5)==0)
    {
      skip=atoi(argv[i+1]);
      i=i+2;

    }
    else if (strncasecmp(argv[i],"-type",5)==0)
    {
      evtype=atoi(argv[i+1]);
      i=i+2;

    }
    else if (strncasecmp(argv[i],"-class",6)==0)
    {
      evclass=atoi(argv[i+1]);
      i=i+2;

    }
    else if (strncasecmp(argv[i],"-hex",4)==0)
    {
      hex_dump=1;
      i=i+1;

    }
    else if (strncasecmp(argv[i],"-bank",5)==0)
    {
      if(nbankname<100)
      {
	    nbankname++;
	    bankname[nbankname-1]=strdup(argv[i+1]);
      }
      i=i+2;

    }
    else if (strncasecmp(argv[i],"-extract",8)==0)
    {
      extract=atoi(argv[i+1]);
      i=i+2;

    }
    else if (strncasecmp(argv[i],"-", 1) != 0)
    {
      break;  /* reached msg field */
    }
    else
    {
      printf("%s",help); 
      exit(EXIT_SUCCESS);
    } 
  }
  

  /* get file name */
  filename=argv[i];


  return;
}



