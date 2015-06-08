/* UNIX version only */

#ifndef VXWORKS

/* setdvcsthresh.c
 *
 * Program to set VME Discriminators CAEN v895 thresholds
 *
 * written by Puneet Khetarpal
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
//void GetQuadrantIndex(int x, int y, int & quad, int & index);


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
  int  i, j, ii, kk, stt = 0;
  char result[TEXT_STR];      /* string for messages from tcpClientCmd */
  char exename[200];          /* VME discr vxworks exec name */
  bool isHod = false;
  int processAll = 0;
  int threshVal = 0;
  char *str = "SetThreshold";


/* get the environment variables */
  clonparms = getenv("CLON_PARMS");

  // setup variables
  roc = "pretrig3";
  mod = "mon";

  // initialize variables
  int IC_disc[23][23];
  int IC_chan[23][23];
  int IC_HOD_quad[10][8];
  int IC_HOD_index[10][8];
  int IC_HOD_disc[10][8];   // 4 and 18 orig
  int IC_HOD_chan[10][8];

  for (i = 0; i < 23; i++) {
    for (j = 0; j < 23; j++) {
      IC_disc[i][j] = 0;
      IC_chan[i][j] = 0;
      if (i < 4 && j < 18) {
	IC_HOD_disc[i][j] = 0;
	IC_HOD_chan[i][j] = 0;
      }
      if (i < 10 && j < 8) {
	IC_HOD_quad[i][j] = -1;
	IC_HOD_index[i][j] = -1;
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
  int quad = 0;
  int index = 0;
  int disc = 0;
  int chan = 0;

  while (fscanf(file1, "%d %d %d %d", &x, &y, &disc, &chan) != EOF) {
    x--; y--;
    IC_disc[x][y] = disc;
    IC_chan[x][y] = chan;
  }

  fclose(file1);

  // read the IC hodoscope file
  char *filename2 = "/usr/local/clas/parms/ic/hod_xyqi.txt";
  FILE *file2 = fopen(filename2, "r");
  if (file2 == NULL) {
    printf("\nCan't open \n%s\n\n", filename2);
    return 0;
  }

  while (fscanf(file2, "%d %d %d %d %d %d", &x, &y, &quad, &index, &disc, &chan) != EOF) {
    quad--;index--;
    IC_HOD_quad[x][y] = quad;
    IC_HOD_index[x][y] = index;
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
      if (stt == 0 && isHod == 1) {
	// check if x and y coordinates are within range
	if ((x < 0 || x > 9 || y < 1 || y > 8) || (IC_HOD_quad[x][y] == -1)) {
  	  printf("Error: incorrect x y coordinates specified\n");
	  stt = 3;
	} else {
	  quad = IC_HOD_quad[x][y];
	  index = IC_HOD_index[x][y];
	  disc = IC_HOD_disc[x][y];
	  chan = IC_HOD_chan[x][y];
	}
      } else if (stt == 0) {
        x--;
        y--;
	if ((IC_disc[x][y] == 0) || (x < 0 || x > 22 || y < 0 || y > 22)) {
	  printf("Error: incorrect x y coordinates specified\n");
	  stt = 3;
	}
      }
    }

    // check if last argument is 0 or 1
    threshVal = atoi(argv[4]);
    if (stt == 0 && (threshVal < 1  || threshVal > 255)) {
      printf("Error: incorrect threshold for channel\n");
      stt = 4;
    } 
  }

  if (stt != 0) {
    printf("  usage: setdvcsthresh [ic | hod] x y [ thresh ]\n");
    printf("    ic|hod : specify whether want ic or hodoscope \n");
    printf("    x: x position (1-23) in IC or (0-9) in hodoscope\n");
    printf("    y: y position (1-23) in IC or (1-8) in hodoscope\n");
    printf("    thresh: threshold for the channel (1-255 mV)\n");
    printf("  NOTE: the x axis coordinates 0 and 9 are reserved for \n");
    printf("    channels connected to those between 1 and 8.\n");
    printf("  To set all discriminator channels to same threshold for IC or hodoscope, use:\n");
    printf("    setdvcsthresh [ic | hod] 0 0 [ thresh ]\n");
    exit(stt);
  }


  // if input is correct then enable or disable channel as requested
  unsigned int discAdd = GetDiscAddress(1);  
  int channel = 0;

  // if want to process all discriminators, then run a for loop
  if (processAll == 1) {
    char *detstr = (isHod == 1) ? "hodoscope" : "IC";
    int startDisc = (isHod == 1) ? 29 : 1;
    int endDisc = (isHod == 1) ? 33: 28;
    printf("Setting all discriminators and channels for %s to: %d\n", detstr, 
	   threshVal);
    for (i = startDisc; i <= endDisc; i++) {
      discAdd = GetDiscAddress(i);
      printf(" Setting for discriminator: %d\n", i);
      for (j = 0; j <= 15; j++) {
        sprintf(exename, "v895SetThreshold(%u, %d, %d)", discAdd, j,threshVal);
        memset(result,0,TEXT_STR);
        tcpClientCmd(roc, exename, result);
      }
    }
  } else {  // process a single channel in a discriminator
    if (isHod == 1) {
      discAdd = GetDiscAddress(IC_HOD_disc[x][y]);
      channel = IC_HOD_chan[x][y];
      printf("Setting discriminator %d channel %d threshold: %d\n", 
	     IC_HOD_disc[x][y], IC_HOD_chan[x][y], threshVal);
    } else {
      discAdd = GetDiscAddress(IC_disc[x][y]);
      channel = IC_chan[x][y];
      printf("Setting discriminator %d channel %d threshold: %d\n", 
	     IC_disc[x][y], IC_chan[x][y], threshVal);
    }

    sprintf(exename, "v895SetThreshold(%u, %d, %d)",discAdd,channel,threshVal);
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

