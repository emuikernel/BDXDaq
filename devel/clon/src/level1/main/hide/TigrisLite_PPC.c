
/* tigrisLite.c */

#ifndef VXWORKS

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "Hv.h"
#include "ced.h"
#include "tigris.h"

#define FILENAME "/usr/local/clas/parms/ts/Current.Config"

extern struct SLsecdef Sector[7]; /* defined in fileio.c */
extern struct phase3_def phase3; /* defined in fileio.c */
extern char InFileName[512]; /* defined in fileio.c */
extern char dlhn[1024]; /* defined in fileio.c */

static void
usage()
{
  printf ("usage: tigrisLite [-c] <target host> <trigger filename>\n");
  printf ("\n\twhere:\n\t\t-c = optional: answer the last downloaded trigger file\n");
  printf ("\t\ttarget host = the host name to download to\n");
  printf ("\t\ttrigger filename = name of the trigger to download\n");

  exit(-1);
}

void
main(int argc, char *argv[])
{
  int i, phase, r;
  int mxse, sctr, j, k, sector;
  FILE *fp;

  j=1;
  if(argc < 2) usage();

  if(*argv[j] == '-')
  {
    if((strncmp (argv[j], "-c", 2)) == 0)
    {
      system("more /usr/local/clas/parms/ts/Current.Config");
      exit(0);
    }
    else
    {
      usage();
    }
  }

  if(argc != 3) usage();

  strcpy(dlhn, argv[1]);
  strcpy(InFileName, argv[2]);
  printf("download host >%s<\nfile to download >%s<\n",dlhn,InFileName);

  for(sector=0; sector<7; sector++) 
    for(mxse=0; mxse<12; mxse++) 
      for(i=0; i<32; i++) 
        for(j=0; j<16; j++) 
          Sector[sector].SE[mxse][i][j] = 2;

  /* Phase3 data variables set to 2 (X)  */
  for(i=0; i<MaxTrig; i++)
  {
    for(j=0; j<MaxTrigProducts; j++)
    {
      for (k=0; k<19; k++)
      {
        phase3.Trig[i][j][k] = 2;
      }
    }
    phase3.Trig_Delay[i] = 1;
    phase3.Enable_Async[i] = 0;
  }

  if((r=ReadValues ()) != 0)
  {
    printf ("file read failed: can't open file: %s\n", InFileName);
    exit (-1);
  }

  printf ("... done reading...\n");

  /* cleanup trigger data array */
  bigdata_cleanup();

  /* down load phase 1 (sector 1 - 6, phase 0, 1 the scintillator bits) */
  for(phase=0; phase<2; phase++) 
  {
    for(i=1; i<7; i++)
    {
      download_phase1(i, phase);
      printf("done downloading sector %d\n", i);
    }
  }

  /* download phase 2 (the se equations) */
  for(i=1; i<7; i++)
  {
    download_phase2(i);
    printf("phase 2 sector %d\n", i);
  }

  /* last download the event processor bits */
  download_phase3();

  printf("finished download...\n");
  if((fp=fopen(FILENAME, "w")) == 0)
  {
    printf ("failed to open the file %s\n", FILENAME);
    printf ("will exit now - the hardware config has changed BUT the file\n");
    printf ("that maintains this info has some problem FIX IT NOW!! \n");
    exit (-2);
  }	  

  fprintf (fp, "%s\n", InFileName);
  fprintf (fp, "(only ~trigger/tigris and ~trigger/tigrisLite update this info)");
  fclose (fp);
  printf ("done ...\n");

  exit(0);
} 

#else

void
TigrisLite_PPC_dummy()
{
  return;
}

#endif




