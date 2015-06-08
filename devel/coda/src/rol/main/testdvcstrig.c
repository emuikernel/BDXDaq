/* UNIX version only */

#ifndef VXWORKS

/* testdvcstrig.c
 *
 * Program to set VME Discriminators CAEN v895
 *
 * written by: Puneet Khetarpal
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <libtcp.h>

#define F_NAME    100       /* length of config. file name */
#define TEXT_STR  20000     /* size of some text strings */
#define false     0
#define true      1
#define bool      int

/* get main env variables */
char *getenv();
char *clonparms;

/* constants */
const unsigned int ADDRESS_START = 4194304000;
const unsigned int ADDRESS_FACTOR = 65536;

/* function prototypes */

unsigned int GetDiscAddress(int number);

/*************************************************************************/
/*************************************************************************/
/*  main program                                                         */
/*************************************************************************/
int
main(int argc, char *argv[])
{
  char fma[F_NAME] = { "" };  /* config file name */
  /* char *det = "pretrig3";  */
  char *mod, *roc;
  char *ident; 
  int i, j, ii, kk, stt = 0;
  char result[TEXT_STR];      /* string for messages from tcpClientCmd */
  char exename[200];          /* VME discr vxworks exec name */
  bool isHod = false;
  int processAll = 0;
  int switchVal = 0;
  char *str = "Enable";


/* get the environment variables */
  clonparms = getenv("CLON_PARMS");

  // setup variables
  roc = "pretrig3";
  mod = "mon";

  // initialize variables
  int IC_disc[23][23];
  int IC_chan[23][23];
  int IC_HOD_disc[4][18];
  int IC_HOD_chan[4][18];

  for (i = 0; i < 23; i++) {
    for (j = 0; j < 23; j++) {
      IC_disc[i][j] = 0;
      IC_chan[i][j] = 0;
      if (i < 4 && j < 18) {
	IC_HOD_disc[i][j] = 0;
	IC_HOD_chan[i][j] = 0;
      }
    }
  }

  // read the input files
  char *filename1 = "/usr/local/clas/parms/ic/IC_out.txt";
  FILE *file1 = fopen(filename1, "r");
  if (file1 == NULL) {
    printf("\nCan't open \n%s\n\n", filename1);
    return 0;
  }
  
  int x = 0;
  int y = 0;
  int disc = 0;
  int chan = 0;

  while (fscanf(file1, "%d %d %d %d", &x, &y, &disc, &chan) != EOF) {
    x--; y--;
    IC_disc[x][y] = disc;
    IC_chan[x][y] = chan;
  }

  fclose(file1);

  // read the IC hodoscope file
  char *filename2 = "/usr/local/clas/parms/ic/IC_HOD_out.txt";
  FILE *file2 = fopen(filename2, "r");
  if (file2 == NULL) {
    printf("\nCan't open \n%s\n\n", filename2);
    return 0;
  }

  while (fscanf(file2, "%d %d %d %d", &x, &y, &disc, &chan) != EOF) {
    x--;y--;
    IC_HOD_disc[x][y] = disc;
    IC_HOD_chan[x][y] = chan;
  }

  fclose(file2);

  // check input parameters
  if (argc < 5) {
    stt = 1;
  } else {
    // check first string for ic or hod
    ident = argv[1];
    if ((strcmp(ident, "ic") != 0) && (strcmp(ident, "hod") != 0)) {
      stt = 2;
    } else if (strcmp(ident, "hod") == 0) {
      isHod = true;
    } 
    // check x and y
    x = atoi(argv[2]);
    y = atoi(argv[3]);
    // check if both x and y are 0
    if (stt == 0 && x == 0 && y == 0) {
      processAll = 1;
    } else {
      x--;
      y--;
      if (stt == 0 && isHod == 1) {
	if ((IC_HOD_disc[x][y] == 0) || (x < 0 || x > 3 || y < 0 || y > 17)) {
	  printf("Error: incorrect x y coordinates specified\n");
	  stt = 3;
	}
      } else if (stt == 0) {
	if ((IC_disc[x][y] == 0) || (x < 0 || x > 22 || y < 0 || y > 22)) {
	  printf("Error: incorrect x y coordinates specified\n");
	  stt = 3;
	}
      }
    }

    // check if last argument is 0 or 1
    switchVal = atoi(argv[4]);
    if (stt == 0 && (switchVal != 0 && switchVal != 1)) {
      printf("Error: incorrect option for channel\n");
      stt = 4;
    } else if (stt == 0) {
      str = (switchVal == 1) ? "Enable" : "Disable";
    }
  }

  if (stt != 0) {
    printf("  usage: testdvcstrig [ic | hod] x y [0 | 1]\n");
    printf("    ic|hod : specify whether want ic or hodoscope \n");
    printf("    x: x position (1-23) in ic or quadrant in hodoscope (1-4)\n");
    printf("    y: y position (1-23) in ic or index in hodoscope (1-18)\n");
    printf("    0/1: disable or enable channel\n");     
    printf("  To enable/disable all discriminator channels for IC or Hodoscope, use:\n");
    printf("    testdvcstrig [ic | hod] 0 0 [0 | 1]\n");
    exit(stt);
  }


  // if input is correct then enable or disable channel as requested
  unsigned int discAdd = GetDiscAddress(1);  
  int channel = 0;

  // if want to process all discriminators, then run a for loop
  if (processAll == 1) {
    char *detstr = (isHod == 1) ? "hodoscope" : "IC";
    int startDisc = (isHod == 1) ? 29 : 1;
    int endDisc = (isHod == 1) ? 33 : 28;
    printf("%s all discriminators and channels for %s\n", str, detstr);
    for (i = startDisc; i <= endDisc; i++) {
      sprintf(exename, "v895%sAllChan(%u)", str, GetDiscAddress(i));
      memset(result,0,TEXT_STR);
      tcpClientCmd(roc, exename, result);
    }
  } else {  // process a single channel in a discriminator
    printf("Processing channel request: %s\n", str);
    if (isHod == 1) {
      discAdd = GetDiscAddress(IC_HOD_disc[x][y]);
      channel = IC_HOD_chan[x][y];
    } else {
      discAdd = GetDiscAddress(IC_disc[x][y]);
      channel = IC_chan[x][y];
    }
    printf("discriminator: 0x%08X channel: %d\n", discAdd, channel); 

    sprintf(exename, "v895%sChannel(%u, %d)", str, discAdd, channel);
    memset(result,0,TEXT_STR);
    tcpClientCmd(roc, exename, result);
  }

  exit(stt);   /* end of main() */
}

// level 1 ////////////////////////////////////////////////////////////////////

unsigned int GetDiscAddress(int number) {
  // local variables
  unsigned int address = number;
  
  // statements
  address = ADDRESS_START + number*ADDRESS_FACTOR;

  return address;
}

#endif

