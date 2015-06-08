/* fb_diag.c   ( for loading into FASTBUS CPU )
 *
 *  Set of FASTBUS utility functions to diagnose hardware status
 *     (for LeCroy's  ADC1881,  TDC1872,  TDC1875,  TDC1877,
 *                    ADC1881M, TDC1872A, TDC1875A, TDC1877S )
 *
 *  Functions:
 *  ----------
 *    fb_busy_stats (rocname,prn)              get info for all boards
 *    rocid (rocname)                          get ROC_id for ROC_name
 *    fb_map (rocname)                         create hardmap_this
 *    fb_map_from_CLON_PARMS(rocname)          restore hardmap_CLON_PARMS
 *    fb_map_compare ()                        compare two hardmaps
 *    csr_conf_settings (rocname,prn)          restore CSRs default settings
 *
 *    fb_board_stats   (slot,prn)              get board status
 *    fb_72_75_bl_read (slot)                  make one BlockRead for 1872 or 1875
 *    fb_72_75_stats (slot,csr0,b_type,prn)    board check for TDC 1872 & 1875
 *    fb_77_stats    (slot,csr0,b_type,prn)    board check for TDC 1877 & 1877S
 *    fb_81_stats    (slot,csr0,b_type,prn)    board check for ADC 1881
 *
 *    fb_board_csr_read ()                     get CSRs for all boards in crate
 *
 *
 *     by Serguei P.
 *     Last update 01/07/2002
 */

#ifdef VXWORKS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sysLib.h>
#include <taskLib.h>

/*
#include "fb.h"
*/

#define SFI_VME_ADDR (char*)0xe00000

#define BOARD_ID_MASK  0xffff0000
#define B1872          0x10480000
#define B1872A         0x10360000
#define B1875          0x10490000
#define B1875A         0x10370000
#define B1877          0x103c0000
#define B1877S         0x103d0000
#define B1881          0x104b0000
#define B1881M         0x104f0000

/*
extern STATUS sysBusToLocalAdrs();

extern void fb_init_1(int addr);
extern void sfi_error_decode(int flag);

extern unsigned long fpac(int pa, int sa);
extern unsigned long fprel();
extern unsigned long fprc(int pa, int sa, int *data);
extern unsigned long fpwc(int pa, int sa, int data);
*/

typedef struct csr_conf
{
  unsigned int csr0_1877;
  unsigned int csr1_1877;
  unsigned int csr18_1877;
  unsigned int csr0_1877S;
  unsigned int csr1_1877S;
  unsigned int csr18_1877S;
  unsigned int csr0_1881;
  unsigned int csr1_1881;
  unsigned int csr0_1872;
} CSR_CONF;
CSR_CONF csrdef;

/* input board registers info */
char *dir_csr      = "$CLON_PARMS/feconf/csrconf/clasrun";

/* input ROC maps */
/* copy 'right' files to the following directory for references */
char *dir_mapparms = "$CLON_PARMS/feconf/rocmap/clasrun/previous";

/* working directory */
char *dir_mapthis  = "$CLON_PARMS/fb_boards/clasrun";


char map_this[26][10], map_parms[26][10], res[120];

int  csr_adc[]    = {5, 0, 1, 3, 7, 16};
int  csr_tdc72[]  = {2, 0, 1}; /*
int  csr_tdc75[]  = {2, 0, 1}; */
int  csr_tdc77[]  = {6, 0, 1, 3, 7, 16, 18}; /*
int  csr_tdc77S[] = {7, 0, 1, 3, 5, 7, 16, 18}; */


/****** Function prototypes ******/
extern unsigned long fb_frdb_1();
int  fb_busy_stats (char *rocname, int prn);
int  rocid (char *rocname);
int  fb_map (char *rocname);
int  fb_map_from_CLON_PARMS (char *rocname);
int  fb_map_compare ();
int  csr_conf_settings (char rocname[10], int prn);
char *fb_board_stats (int slot, int prn);
int   fb_72_75_bl_read (int slot);
char *fb_72_75_stats (int slot, int csr0, char b_type[10], int prn);
char *fb_77_stats    (int slot, int csr0, char b_type[10], int prn);
char *fb_81_stats    (int slot, int csr0, char b_type[10], int prn);

int  fb_board_csr_read ();




/*****************************************************************
 * Main function getting info about all FASTBUS boards in crate:
 *   prn=2: print Status Table for all slots, status could be
 *          "OK", "Busy ..." or "Error: ..." with short info
 *           what kind of error was occurred;
 *   prn=1: print Full Board Status for all nonempty slots;
 *   prn=0: print only Error messages, if some errors occurred,
 *          or simply return (0).
 */

int  fb_busy_stats (char *rocname, int prn)
{
  int  slot;
  char *stats;

  if ( fb_map(rocname) == -1 ) {
    printf("\n Error: Can't create hardware map for ROC \"%s\" \n\n", rocname);
    return (-1);
  }

  if ( fb_map_from_CLON_PARMS(rocname) == -1 )
    printf("\n Error: Can't get hardmap from %s \n\n", dir_mapparms);
  else if ( fb_map_compare() != 0 )
    printf("\n Warning: Present hardmap is different to %s \n\n", dir_mapparms);

  csr_conf_settings(rocname,prn);

  if (prn == 2) {
    printf("\n  ROC \"%s\" \n\n", rocname);
    printf("  Slot     Board type     Status \n");
    printf(" --------------------------------\n");
  }

  for ( slot=0; slot<26; slot++) {
    if    ( (strcmp(map_this[slot],"Empty")    == 0) || 
	    (strcmp(map_this[slot],"Unknown")  == 0) )
      stats = " -";
    else
      stats = fb_board_stats(slot,prn);

    if (prn == 2)
      printf("   %2d      %9s       %s \n", slot, map_this[slot], stats);
  }

  return(0);
}



/***************************************************************
 * Return numerical ROC_id corresponding to input ROC_name and
 * return (-1) if ROC_name is not in the list.
 */

int rocid (char *rocname)
{
  int i;
  char *roc="0123456789";
  char *ROC_names[32]={"","dc1","dc2","dc3","dc4","dc5","dc6","dc7",
		       "dc8","dc9","dc10","dc11","cc1","sc1","ec1","ec2",
		       "lac1","tage","scaler2","scaler4","tlv1","","","scaler3",
		       "","polar","","","","","scaler1","croctest1"};
  
  strcpy(roc,rocname);
  for(i=0; i<strlen(rocname); i++) {
    roc[i]=tolower(rocname[i]);
  }

  for (i=0;i<32;i++) {
    if ( strcmp(roc,ROC_names[i]) == 0) return (i);
  }
  
  return (-1);
}



/****************************************************************
 * Create FASTBUS hardware map, looking for:
 *           ADC1881,  TDC1872,  TDC1875,  TDC1877,
 *           ADC1881M, TDC1872A, TDC1875A, TDC1877S;
 * Store map in dir_mapthis/ROC??.hardmap & in map_this[26][10];
 *   return ( 0) if hardmap correctly defined,
 *   return (-1) and print Error message if "rocname" 
 *   is not in the list or problems with output file.
 */

int fb_map (char *rocname)
{
  FILE *fpout;
  char filename_out[130];
  int  slot, i, csr;
  int  val;
  unsigned long ladr;
  int map_adc[26], map_tdc72[26], map_tdc75[26];
  int map_tdc77[26], map_tdc77S[26];
  int map_unknown[26], map_empty[26];
  int  id;
  

/****** Find ROC_id for input ROC_name ******/
  if ( (id=rocid(rocname)) == -1) {
    printf("\n Error: ROC name \"%s\" is not in the list.\n\n", rocname);
    return (-1);
  }

/****** Initialization of SFI/fastbus ******/
  ladr = 0; /*
  sysBusToLocalAdrs (0x39, (char *)SFI_VME_ADDR, (char**)&ladr);
  printf("SFI_BaseAddress_local  = %8lX \n", ladr);
  fb_init_1 (ladr); */

/****** Initialization and filling map_this[26][10] ******/
  for (i=0;i<26;i++) strcpy(map_this[i],"");

/****** Loop over all the slots of the Fastbus crate ******/
  for ( slot=0; slot<26; slot++) {
      
/****** Reset the map array elements ******/
      map_adc[slot]     = 0;
      map_tdc72[slot]   = 0;
      map_tdc75[slot]   = 0;
      map_tdc77[slot]   = 0;
      map_tdc77S[slot]  = 0;
      map_unknown[slot] = 0;
      map_empty[slot]   = 0;
      
      if(fpac (slot, 0) != 0 ) {
	map_empty[slot] = 1;
	strcpy(map_this[slot],"Empty");
      }
      fprel();
      
      if(fprc (slot, 0, &csr) == 0) {
	val = csr&BOARD_ID_MASK;
	if ( val == B1881M || val == B1881 ) {
	  map_adc[slot] = 1;
	  strcpy(map_this[slot],"ADC1881");
	} else if ( val == B1872 || val == B1872A ) {
	  map_tdc72[slot] = 1;
	  strcpy(map_this[slot],"TDC1872");
	} else if ( val == B1875 || val == B1875A ) {
	  map_tdc75[slot] = 1;
	  strcpy(map_this[slot],"TDC1875");
	} else if ( val == B1877  ) {
	  map_tdc77[slot] = 1;
	  strcpy(map_this[slot],"TDC1877");
	} else if ( val == B1877S ) {
	  map_tdc77S[slot] = 1;
	  strcpy(map_this[slot],"TDC1877S");
	} else if ( map_empty[slot] == 0 ) {
	  map_unknown[slot] = 1;
	  strcpy(map_this[slot],"Unknown");
	}
      } 
      else {
	sfi_error_decode(3);
	map_empty[slot] = 1;
	strcpy(map_this[slot],"Empty");
      }
      fprel();
    }
  
  sprintf(filename_out, "%s/ROC%d.hardmap", dir_mapthis, id);
  if ( (fpout = fopen(filename_out,"w") ) == NULL) {
    printf("\n Error: Can't open output mapfile %s \n\n", filename_out);
    return (-1);
  }
 
  fprintf(fpout, \
          "Slot    ADC1881  TDC1872  TDC1875  TDC1877  TDC1877S  Unknown  Empty \n");
  fprintf(fpout, \
	  "-------------------------------------------------------------------- \n");
  for (i=0;i<26;i++) {
    fprintf(fpout,"%3d %8d %8d %8d %8d %8d %8d %8d \n", i, \
	    map_adc[i], map_tdc72[i], map_tdc75[i], map_tdc77[i], \
	    map_tdc77S[i], map_unknown[i], map_empty[i]);
  }
  
  if (fclose(fpout) !=0) {
    printf("\n Error: Can't close output mapfile %s \n\n", filename_out);
    return (-1);
  }

/****** End of creating hardware map ******/
  return(0);
}



/**********************************************************
 * Fill map_parms[26][10] reading hardmap-file from
 *      dir_mapparms/ROC??.hardmap;
 *   return ( 0) if hardmap-file exists and is correct,
 *   return (-1) and print Error message if hardmap-files
 *   does not exist or is unreadable, empty or corrupted.
 */

int fb_map_from_CLON_PARMS (char *rocname)
{
  FILE *mapfile;
  char filename[130];
  int  id, ch;
  char str_tmp[200];
  int  sl, a81, t72, t75, t77, t77S, un, empt;
  int  i, n1, n2, n3;


/****** Find ROC_id for input ROC_name ******/
  if ( (id=rocid(rocname)) == -1) {
    printf("\n Error: ROC name \"%s\" is not in the list.\n\n", rocname);
    return (-1);
  }

/****** Set hardmap-file name and open it ******/
  sprintf(filename, "%s/ROC%d.hardmap", dir_mapparms, id);
  if ( (mapfile = fopen(filename,"r") ) == NULL) {
    printf("\n Error: Can't open mapfile %s \n\n", filename);
    return (-1);
  }
  
/****** Initialization and filling map_parms[26][10] ******/
  for (i=0;i<26;i++) strcpy(map_parms[i],"");
  n1 = n2 = n3 = 0;

  while ((ch = getc(mapfile)) != EOF) {
    ungetc(ch,mapfile);
    fgets(str_tmp, 198, mapfile);
    n1++;
    if((strstr(str_tmp, "Slot") == NULL) && \
       (strstr(str_tmp, "----") == NULL) && \
       (strcmp(str_tmp, "\n") != 0)) {
      n2++;
      sscanf (str_tmp, "%d %d %d %d %d %d %d %d",\
	      &sl, &a81, &t72, &t75, &t77, &t77S, &un, &empt);
      if      ( a81  == 1 ) strcpy(map_parms[sl],"ADC1881");
      else if ( t72  == 1 ) strcpy(map_parms[sl],"TDC1872");
      else if ( t75  == 1 ) strcpy(map_parms[sl],"TDC1875");
      else if ( t77  == 1 ) strcpy(map_parms[sl],"TDC1877");
      else if ( t77S == 1 ) strcpy(map_parms[sl],"TDC1877S");
      else if ( un   == 1 ) strcpy(map_parms[sl],"Unknown");
      else if ( empt == 1 ) strcpy(map_parms[sl],"Empty");
      else {
	printf("\n Error: File %s is corrupted.\n\n", filename);
	fclose(mapfile);
	return (-1);
      }
    }
    else if (strcmp(str_tmp,"\n") == 0) n3++;
  }
  fclose(mapfile);
  /*  printf("sl=%d; n1=%d; n2=%d; n3=%d \n", sl, n1,n2,n3); */

  if ( n1 == n3 ) {
    printf("\n Error: File %s is empty.\n\n", filename);
    return (-1);
  } else if ( n2 != 26 ) {
    printf("\n Error: File %s is corrupted.\n\n", filename);
    return (-1);
  }
  
  /*  for (id=0;id<26;id++)
      printf("map_parms[%d]=%s;; \n", id, map_parms[id]); */
  
  return(0);
}



/**************************************************************
 * Compare two hardmaps from "dir_mapthis" and "dir_mapparms"
 *   return (0) if they are identical, or 
 *   return (int value) and print what types are not identical,
 *       value is the number of slots with not identical info. 
 */

int  fb_map_compare ()
{
  int ii, inf;

  inf = 0;
  for (ii=0;ii<26;ii++) {
    if ( strcmp(map_this[ii],map_parms[ii]) != 0) {
      if ( inf == 0 )
	printf("\n  Information is not identical for next slots: \n\n");
      printf("map_this[%2d]=%9s, <-> map_parms[%2d]=%9s; \n", \
	     ii, map_this[ii], ii, map_parms[ii]);
      inf++;
    } /* else
      printf("map_this[%2d]=%9s, <-> map_parms[%2d]=%9s; (these are identical)\n",\
      ii, map_this[ii], ii, map_parms[ii]); */
  }
  
  return(inf);
}



/**********************************************************
 * Define CSRs default settings and fill structure csrdef
 *   for ROC "rocname" reading csrconf-file from
 *   dir_csr/ROC??.hardcnf;
 * Print full info if prn=1;
 *   return (0) if csrconf-file exists and csrdef defined,
 *   return (1) if csrconf file is unreadable,
 *   return (-1) if "rocname" is not in the list.
 */

int csr_conf_settings (char rocname[10], int prn)
{
  FILE *fp;
  char filename_in[111];
  char ss[333];
  unsigned int bit;
  unsigned int *pt;
  int  id;

/****** CSRs initialization ******/
  csrdef.csr0_1877  = csrdef.csr1_1877  = csrdef.csr18_1877  = 0x0;
  csrdef.csr0_1877S = csrdef.csr1_1877S = csrdef.csr18_1877S = 0x0;
  csrdef.csr0_1881  = csrdef.csr1_1881  = 0x0;
  csrdef.csr0_1872  = 0x0;
  pt = &csrdef.csr0_1872;

/****** Find ROC_id for input ROC_name ******/
  if ( (id=rocid(rocname)) == -1) {
    printf("\n Error: ROC name \"%s\" is not in the list.\n\n", rocname);
    return (-1);
  }

/****** Define CSRs default settings from conf-file ******/
  sprintf(filename_in, "%s/ROC%d.hardcnf", dir_csr, id);
  if ((fp = fopen(filename_in, "r")) == NULL) {
    printf("\n Warning: Can't read csrconffile %s \n\n", filename_in);
    return (1);
  } else {
    while ( fscanf(fp, "%s", ss) != EOF) {
      if(strcmp(ss,"1877_CSR0") == 0) pt = &csrdef.csr0_1877;
      else if(strcmp(ss,"1877_CSR1") == 0) pt = &csrdef.csr1_1877;
      else if (strcmp(ss,"1877_CSR18") == 0) pt = &csrdef.csr18_1877;      
      else if(strcmp(ss,"1877S_CSR0") == 0) pt = &csrdef.csr0_1877S;
      else if(strcmp(ss,"1877S_CSR1") == 0) pt = &csrdef.csr1_1877S;
      else if (strcmp(ss,"1877S_CSR18") == 0) pt = &csrdef.csr18_1877S;      
      else if(strcmp(ss,"1881_CSR0") == 0) pt = &csrdef.csr0_1881;
      else if (strcmp(ss,"1881_CSR1") == 0) pt = &csrdef.csr1_1881;      
      else if(strcmp(ss,"1872_CSR0") == 0) pt = &csrdef.csr0_1872;
      
      while (strcmp(ss,"!") == 0) 
	{
	  fscanf(fp, "%d %s ", &bit, ss);
	  *pt = *pt << 1;
      	  if( bit==0 || bit==1 ) *pt = *pt | bit;
	  bit = 0;
	}
    }
    fclose(fp);
  }

/****** Print restored CSRs default settings (if prn=1) ******/
  if (prn == 1) {
    printf("\n ROC name is \"%s\" \n *********** \n", rocname);
    printf(" CSRs default settings restored from %s \n", filename_in);
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
    printf(" CSR0_77  = 0x%8x,  CSR1_77  = 0x%8x,  CSR18_77  = 0x%8x \n", \
	   csrdef.csr0_1877, csrdef.csr1_1877,csrdef. csr18_1877);
    printf(" CSR0_77S = 0x%8x,  CSR1_77S = 0x%8x,  CSR18_77S = 0x%8x \n", \
	   csrdef.csr0_1877S, csrdef.csr1_1877S,csrdef. csr18_1877S);
    printf(" CSR0_81  = 0x%8x,  CSR1_81  = 0x%8x \n", \
	   csrdef.csr0_1881, csrdef.csr1_1881);
    printf(" CSR0_72  = 0x%8x \n\n", \
	   csrdef.csr0_1872);
  }
  
  printf("\n");
  return(0);
}



/***************************************************************
 * Get board status for FASTBUS module in inputed "slot";
 * Print full info if prn=1;
 *   return "OK" if board is OK and has default CSRs settings,
 *          "Busy ..." or "Error: ..." with short explanation 
 *           what kind of error was occurred.
 */

char *fb_board_stats (int slot, int prn)
{
  int  csr0, id;
  char b_type[10];

  strcpy(res,"");  /*** return initialization ***/

/****** Check existence of the board ******/
  if (fpac(slot,0) != 0) {
    sfi_error_decode(3);
    sprintf(res, "Error: Slot#%d is empty.", slot);
    if (prn != 2) printf("\n %s \n\n", res);
    return (res);
  }
  fprel();

/****** Check Board ID ******/
  if (fprc(slot,0,&csr0) != 0) {
    sprintf(res, "Error: Slot#%d, can't read CSR0.", slot);
    if (prn != 2) printf("\n %s \n\n", res);
    return (res);
  }

  id  = csr0&BOARD_ID_MASK;
  if ( id == B1881 || id == B1881M ) {
    strcpy(b_type,"ADC1881");
    sprintf(res, "%s", fb_81_stats(slot,csr0,b_type,prn));
  } else if ( id == B1872 || id == B1872A ) {
    strcpy(b_type,"TDC1872");
    sprintf(res, "%s", fb_72_75_stats(slot,csr0,b_type,prn));
  } else if ( id == B1875 || id == B1875A ) {
    strcpy(b_type,"TDC1875");
    sprintf(res, "%s", fb_72_75_stats(slot,csr0,b_type,prn));
  } else if ( id == B1877  ) {
    strcpy(b_type,"TDC1877");
    sprintf(res, "%s", fb_77_stats(slot,csr0,b_type,prn));
  } else if ( id == B1877S ) {
    strcpy(b_type,"TDC1877S");
    sprintf(res, "%s", fb_77_stats(slot,csr0,b_type,prn));
  } else {
    sprintf(res, "Error: Slot#%d, can't define board type.", slot);
    if (prn != 2) printf("\n %s \n\n", res);
    return (res);
  }

  return (res);
}



/*****************************************************************
 * Make one Block read for TDC1872 or TDC1875:
 *   return  (0) if block read done, or
 *   return (-1) and print Error message, if some error occurred.
 */

int fb_72_75_bl_read (int slot)
{
  unsigned long tdc[65];
  unsigned long rslt, dmaptr, lenb, rb, rlen, len=65;
  int ii;
  
  ii=rlen=0;
  if ((rslt = sysLocalToBusAdrs(0x09, (char*)tdc, (char**)&dmaptr)) != 0) {
    printf("\n Error: sysLocalToBusAdrs return = %d \n\n", (int)rslt);
    return(-1);
  }

  lenb = len<<2;
  rslt = fb_frdb_1(slot,0,dmaptr,lenb,&rb,1,0,1,0,0x0a,0,0,1);
  if ((rb > (lenb+4))||(rslt != 0)) {
    printf("\n Error: Block Read rslt=0x%x maxBytes=%d returnBytes=%d \n\n", \
	   (int)rslt, (int)lenb, (int)rb);
    return(-1);
  } /* else {
    rlen = rb>>2;
    printf("Block Read   rslt=0x%x, maxbytes=%d, returnBytes=%d; \n", \
	   (int)rslt, (int)lenb, (int)rb);
    printf(" %d words ", (int)rlen);
    for(ii=0;ii<rlen;ii++) {
      if ((ii % 5) == 0) printf("\n    ");
      printf("  0x%08x", (int)tdc[ii]);
    }
    printf("\n");
  }
    */
  rslt = fpwc(slot,0,0x800000);    /*** increment Event Counter ***/
  if (rslt != 0){ 
    printf("\n Error: Increment Event Counter \n\n");
    return(-1);
  }
  
  return(0);
}



/***************************************************************
 * Get board status for FASTBUS TDC1872 or TDC1875:
 *   prn=2: NO PRINTS, all possible errors collect in return,
 *          Check only BUSY status,
 *          return "OK", "Busy ..." or "Error: ..." with short
 *          info what kind of error was occurred;
 *   prn=1: Check and print Full Board Status with compare
 *          settings of CSR0 to CSR0-default;
 *   prn=0: Board Check without compare CSR0 settings,
 *          print only Error messages if some errors occurred,
 *          return (check result).
 */

char *fb_72_75_stats (int slot, int csr0, char b_type[10], int prn)
{
  int  csr0_back, csr1, csr1_II;
  int  n_cec, n_rec, n_cec2, n_rec2;
  char info[6][10], *inff;

  strcpy(res,"");  /*** return initialization ***/

/****** Read & Store CSR1 state ******/
  if (fprc(slot,1,&csr1) != 0) {
    sprintf(res, "Error: %s, slot#%d, can't read CSR1.", b_type, slot);
    if (prn != 2) printf("\n %s \n\n", res);
    return (res);
  }

/****** Check buffer condition, BUSY or NOT ******/
  n_rec = csr1 & 0xf;
  n_cec = (csr1 & 0xf00) >> 8;

  if (fpwc(slot,0,0x31000640) != 0) {   /*** reprogram TDC ***/
    sprintf(res, "Error: %s, slot#%d, can't write to CSR0.", b_type, slot);
    if (prn != 2) printf("\n %s \n\n", res);
    return (res);
  }
  fpwc(slot,0,0x800);    /*** internal COM, one more event -> buffer ***/
  taskDelay(3);
  fprc(slot,1,&csr1_II);
  n_rec2 = csr1_II & 0xf;
  n_cec2 = (csr1_II & 0xf00) >> 8;

  if (n_cec == n_cec2) sprintf(res, "Data buffer is full -> Board is BUSY");
  else if (fb_72_75_bl_read(slot) == -1) sprintf(res, "Error: Block read failed.");
  else sprintf(res, "OK");
  
/****** Back design of CSR0 settings ******/
  csr0_back = 0;
  if ( csr0 & 0x0040 ) {
    csr0_back = csr0_back | (1<<22);
    strcpy(info[0],"Enabled");
  } else {
    csr0_back = csr0_back | (1<< 6);
    strcpy(info[0],"Disabled");
  }
  if ( csr0 & 0x0100 ) {
    csr0_back = csr0_back | (1<< 8);
    strcpy(info[1],"Enabled");
  } else {
    csr0_back = csr0_back | (1<<24);
    strcpy(info[1],"Disabled");
  }
  if ( csr0 & 0x0200 ) {
    csr0_back = csr0_back | (1<< 9);
    strcpy(info[2],"Internal");
  } else {
    csr0_back = csr0_back | (1<<25);
    strcpy(info[2],"External");
  }
  if ( csr0 & 0x0400 ) {
    csr0_back = csr0_back | (1<<10);
    strcpy(info[3],"Enabled");
  } else {
    csr0_back = csr0_back | (1<<26);
    strcpy(info[3],"Disabled");
  }
  if ( csr0 & 0x1000 ) {
    csr0_back = csr0_back | (1<<12);
    strcpy(info[4],"Enabled");
  } else {
    csr0_back = csr0_back | (1<<28);
    strcpy(info[4],"Disabled");
  }
  if ( csr0 & 0x2000 ) {
    csr0_back = csr0_back | (1<<13);
    strcpy(info[5],"High");
  } else {
    csr0_back = csr0_back | (1<<29);
    strcpy(info[5],"Low");
  }

/****** Restore CSR0 settings ******/
  fpwc(slot,0,csr0_back);

/****** Print full decoded status (if prn=1) ******/
  if (prn == 1) {
    printf("\n Slot = %2d,  Board type = %s \n", slot, b_type);
    printf("------------------------------------ \n");
    printf(" CSR0 = 0x%8x \n", csr0);
    printf("    Wait Status          = %s \n", info[0]);
    printf("    CAT COM Status (TR5) = %s \n", info[1]);
    printf("    MPI Status           = %s \n", info[2]);
    printf("    Test Pulser Status   = %s \n", info[3]);
    printf("    Autorange Status     = %s \n", info[4]);
    printf("    Range Status         = %s \n\n", info[5]);
    printf(" CSR1 = 0x%8x \n", csr1);
    printf("    Readout    Event Counter (REC) = %d \n", n_rec);
    printf("    Conversion Event Counter (CEC) = %d \n\n", n_cec);
    printf(" Busy status check gives: %s \n", res);
    printf(" ======================== \n\n");
    inff = "different to";
    if (csr0_back == csrdef.csr0_1872) inff = "consistent with";
    printf(" Defined CSRs settings are %s default \n", inff);
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
    printf("  CSR0_defined  = 0x%8x  <->  0x%8x = CSR0_default \n\n\n", \
	   csr0_back, csrdef.csr0_1872);
  }

  return (res);
}



/***************************************************************
 * Get board status for FASTBUS TDC1877 or TDC1877S:
 *   prn=2: NO PRINTS, all possible errors collect in return,
 *          Check only BUSY status,
 *          return "OK", "Busy ..." or "Error: ..." with short
 *          info what kind of error was occurred;
 *   prn=1: Check and print Full Board Status with compare
 *          settings of CSRs to CSRs-default;
 *   prn=0: Board Check without compare CSRs settings,
 *          print only Error messages if some errors occurred,
 *          return (check result).
 */

char *fb_77_stats (int slot, int csr0, char b_type[10], int prn)
{
  int  csr1, csr18, csr0_t, csr1_t, csr18_t;
  int  csr16, wbuf, rbuf;
  int  bit;
  char *info;

  strcpy(res,"");  /*** return initialization ***/

/****** Read & Store CSR16 state ******/
  if (fprc(slot,16,&csr16) != 0) {
    sprintf(res, "Error: %s, slot#%d, can't read CSR16.", b_type, slot);
    if (prn != 2) printf("\n %s \n\n", res);
    return (res);
  }

/****** Check buffer pointers, BUSY or NOT ******/
  wbuf  =  csr16 & 0xf;
  rbuf  = (csr16 & 0xf00) >> 8;
  if (wbuf == rbuf) sprintf(res, "Pointer WB=RB=%d -> Board is BUSY", wbuf);
  else sprintf(res, "OK");

/****** Print full decoded status (if prn=1) ******/
  if (prn == 1) {
    printf("\n Slot = %2d,  Board type = %s \n", slot, b_type);
    printf("------------------------------------ \n");
    printf(" CSR0  = 0x%8x \n", csr0);

    if ( csr0 & 0x2 ) info = "Enabled";
    else info = "Disabled";
    printf("    Logical Address       = %s \n", info);

    if (strcmp(b_type,"TDC1877S") == 0) {
      if ( csr0 & 0x4 ) info = "Enabled";
      else info = "Disabled";
      printf("    Register Commons      = %s \n", info);
    }

    if ( csr0 & 0x40 ) info = "Enabled";
    else info = "Disabled";
    printf("    Memory Test Mode      = %s \n", info);

    if ( csr0 & 0x100 ) info = "Enabled";
    else info = "Disabled";
    printf("    Priming during LNE    = %s \n", info);

    if (strcmp(b_type,"TDC1877S") == 0) {
      if ( csr0 & 0x200 ) info = "Selected";
      else info = "Not selected";
      printf("    Simple Sparsification = %s \n", info);
    }

    bit = ( csr0 & 0x1800 ) >> 11;
    if      ( bit == 0 ) info = "Bypass";
    else if ( bit == 1 ) info = "Primary Link";
    else if ( bit == 2 ) info = "End Link";
    else if ( bit == 3 ) info = "Middle Link";
    printf("    Multi-Block Configur. = %s \n\n", info);

    if (fprc(slot,1,&csr1) != 0) {      /*** read & store CSR1 state ***/
      printf("\n Error: Can't read CSR1.\n\n");
      return ("Error: Can't read CSR1");
    } printf(" CSR1  = 0x%8x \n", csr1);

    if ( csr1 & 0x1 ) info = "Enabled";
    else info = "Disabled";
    printf("    BIP to TR7       = %s \n", info);

    if ( csr1 & 0x2 ) info = "Enabled";
    else info = "Disabled";
    printf("    COM from TR6     = %s \n", info);

    if ( csr1 & 0x8 ) info = "Enabled";
    else info = "Disabled";
    printf("    FC from TR5      = %s \n", info);
    
    bit = ( csr1 & 0xf000000 ) >> 24;
    if      (bit  <   3) bit = 1024*(bit+1);
    else if (bit == 0xe) bit = 262144;
    else if (bit == 0xf) bit = 524288;
    else                 bit = 1024*(4-(bit-1)%2) << (bit-2)/2;
    printf("    FCW set to       = %d ns \n", bit);

    if ( csr1 & 0x4 ) info = "Enabled";
    else info = "Disabled";
    printf("    FCW from TR5     = %s \n", info);

    if (strcmp(b_type,"TDC1877S") == 0) {
      if ( csr1 & 0x100000 ) info = "Enabled";
      else info = "Disabled";
      printf("    Sparsification   = %s \n", info);
    }

    if ( csr1 & 0x800000 ) {
      info = "Enabled";
      bit = 1 << ((csr1 & 0x30000) >> 16);
      printf("    Test Burst Count = %d \n", bit);
      bit = ( csr1 & 0xc0000 ) >> 18;
      if (bit < 2) bit =  125*(bit+1);
      else         bit = 1000*(bit-1);
      printf("    Test Pulse       = %d ns \n", bit);
    }
    else info = "Disabled";
    printf("    Internal Test    = %s \n", info);

    if ( csr1 & 0x20000000 ) info = "Enabled";
    else info = "Disabled";
    printf("    Trailing Edge    = %s \n", info);

    if ( csr1 & 0x40000000 ) info = "Enabled";
    else info = "Disabled";
    printf("    Leading Edge     = %s \n", info);

    if ( csr1 & 0x80000000 ) {
      info = "Start";
      bit = ( csr1 & 0xf0 ) >> 4;
      if      (bit == 0) printf("    Com.Start Timeout= Front Pnl \n");
      else if (bit  > 9) bit = 32768;
      else               bit = 32 << bit;
      if (bit != 0)
	printf("    Com.Start Timeout= %d ns \n", bit);
    }
    else info = "Stop";
    printf("    Common mode      = %s \n\n", info);

    printf(" CSR16 = 0x%8x \n", csr16);
    printf("    Read  Buffer Pointer = %d \n", rbuf);
    printf("    Write Buffer Pointer = %d \n\n", wbuf);

    if (fprc(slot,18,&csr18) != 0) {     /*** read & store CSR18 state ***/
      printf("\n Error: Can't read CSR18.\n\n");
      return ("Error: Can't read CSR18");
    } printf(" CSR18 = 0x%8x \n", csr18);

    bit = csr18 & 0xf;
    if (bit == 0) bit = 16;
    printf("    LIFO Depth                  = %d \n", bit);

    bit = 8*((csr18 & 0xfff0) >> 4);
    printf("    Full Scale Time Measurement = %d ns \n\n", bit);

    printf(" Busy status check gives: %s \n", res);
    printf(" ======================== \n\n");

    info = "different to";
    if (strcmp(b_type,"TDC1877S") == 0) {
      csr0_t  = csrdef.csr0_1877S;
      csr1_t  = csrdef.csr1_1877S;
      csr18_t = csrdef.csr18_1877S;
      if ( ((csr0&0xffff) == csrdef.csr0_1877S) && \
	   ( csr1         == csrdef.csr1_1877S) && \
	   ( csr18        == csrdef.csr18_1877S) ) info = "consistent with";
    }
    else {
      csr0_t  = csrdef.csr0_1877;
      csr1_t  = csrdef.csr1_1877;
      csr18_t = csrdef.csr18_1877;
      if ( ((csr0&0xffff) == csrdef.csr0_1877) && \
	   ( csr1         == csrdef.csr1_1877) && \
	   ( csr18        == csrdef.csr18_1877) ) info = "consistent with";
    }
    printf(" Defined CSRs settings are %s default \n", info);
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
    printf("  CSR0_defined  = 0x%8x  <->  0x%8x = CSR0_default \n", \
	   (csr0&0xffff), csr0_t);
    printf("  CSR1_defined  = 0x%8x  <->  0x%8x = CSR1_default \n", \
	   csr1, csr1_t);
    printf("  CSR18_defined = 0x%8x  <->  0x%8x = CSR18_default \n\n\n", \
	   csr18, csr18_t);
  }

  return (res);
}



/***************************************************************
 * Get board status for FASTBUS ADC1881:
 *   prn=2: NO PRINTS, all possible errors collect in return,
 *          Check only BUSY status,
 *          return "OK", "Busy ..." or "Error: ..." with short
 *          info what kind of error was occurred;
 *   prn=1: Check and print Full Board Status with compare
 *          settings of CSRs to CSRs-default;
 *   prn=0: Board Check without compare CSRs settings,
 *          print only Error messages if some errors occurred,
 *          return (check result).
 */

char *fb_81_stats (int slot, int csr0, char b_type[10], int prn)
{
  int  csr1, csr16, wbuf, rbuf;
  int  bit;
  char *info;

  strcpy(res,"");  /*** return initialization ***/

/****** Read & Store CSR16 state ******/
  if (fprc(slot,16,&csr16) != 0) {
    sprintf(res, "Error: %s, slot#%d, can't read CSR16.", b_type, slot);
    if (prn != 2) printf("\n %s \n\n", res);
    return (res);
  }

/****** Check buffer pointers, BUSY or NOT ******/
  wbuf  =  csr16 & 0x3f;
  rbuf  = (csr16 & 0x3f00) >> 8;
  if (wbuf == rbuf) sprintf(res, "Pointer WB=RB=%d -> Board is BUSY", wbuf);
  else sprintf(res, "OK");

/****** Print full decoded status (if prn=1) ******/
  if (prn == 1) {
    printf("\n Slot = %2d,  Board type = %s \n", slot, b_type);
    printf("------------------------------------ \n");
    printf(" CSR0  = 0x%8x \n", csr0);

    if ( csr0 & 0x2 ) info = "Enabled";
    else info = "Disabled";
    printf("    Logical Address       = %s \n", info);

    if ( csr0 & 0x4 ) info = "Enabled";
    else info = "Disabled";
    printf("    Gate                  = %s \n", info);

    if ( csr0 & 0x40 ) info = "Enabled";
    else info = "Disabled";
    printf("    Memory Test Mode      = %s \n", info);

    if ( csr0 & 0x100 ) info = "Enabled";
    else info = "Disabled";
    printf("    Priming on LNE        = %s \n", info);

    bit = ( csr0 & 0x1800 ) >> 11;
    if      ( bit == 0 ) info = "Bypass";
    else if ( bit == 1 ) info = "Primary Link";
    else if ( bit == 2 ) info = "End Link";
    else if ( bit == 3 ) info = "Middle Link";
    printf("    Multi-Block Configur. = %s \n\n", info);

    if (fprc(slot,1,&csr1) != 0) {      /*** read & store CSR1 state ***/
      printf("\n Error: Can't read CSR1.\n\n");
      return ("Error: Can't read CSR1");
    } printf(" CSR1  = 0x%8x \n", csr1);

    if ( csr1 & 0x1 ) info = "Enabled";
    else info = "Disabled";
    printf("    CIP to TR7      = %s \n", info);

    if ( csr1 & 0x2 ) info = "Enabled";
    else info = "Disabled";
    printf("    Gate from TR6   = %s \n", info);

    if ( csr1 & 0x8 ) info = "Enabled";
    else info = "Disabled";
    printf("    FC from TR5     = %s \n", info);

    bit = 2048*(((csr1 & 0xf000000) >> 24) +1);
    printf("    FCW set to      = %d ns \n", bit);

    if ( csr1 & 0x4 ) info = "Enabled";
    else info = "Disabled";
    printf("    FCW from TR5    = %s \n", info);

    if ( csr1 & 0x10 ) info = "Enabled";
    else info = "Disabled";
    printf("    Overrun Detect  = %s \n", info);

    bit = ( csr1 & 0xc0 ) >> 6;
    if      ( bit == 1 ) info = "13 bit";
    else if ( bit == 2 ) info = "12 bit";
    else info = "Unpredictable";
    printf("    Conversion Mode = %s \n", info);

    if ( csr1 & 0x20000000 ) info = "Enabled";
    else info = "Disabled";
    printf("    Internal Tester = %s \n", info);

    if ( csr1 & 0x40000000 ) info = "Enabled";
    else info = "Disabled";
    printf("    Sparsification  = %s \n\n", info);

    printf(" CSR16 = 0x%8x \n", csr16);
    printf("    Read  Buffer Pointer = %d \n", rbuf);
    printf("    Write Buffer Pointer = %d \n\n", wbuf);

    printf(" Busy status check gives: %s \n", res);
    printf(" ======================== \n\n");

    info = "different to";
    if ( ((csr0&0xffff) == csrdef.csr0_1881) && \
	 ( csr1         == csrdef.csr1_1881) ) info = "consistent with";
    printf(" Defined CSRs settings are %s default \n", info);
    printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
    printf("  CSR0_defined  = 0x%8x  <->  0x%8x = CSR0_default \n", \
	   (csr0&0xffff), csrdef.csr0_1881);
    printf("  CSR1_defined  = 0x%8x  <->  0x%8x = CSR1_default \n\n\n", \
	   csr1, csrdef.csr1_1881);
  }

  return (res);
}



/*************************************************************
 * Read and print all essential CSRs for all boards in crate,
 *      printout is very fast and very useful,
 *      at the same time short and informative.
 * Preset hardmap are using to determined nonempty slots,
 * NOTE: to use this routine hardmap should be PRESET !!!
 *       so before self-start better to start fb_map().
 */

int  fb_board_csr_read ()
{
  int  slot, csr_tmp;
  int  i_csr, *csr;
  
  for ( slot=0; slot<26; slot++) {
    if ( ( strcmp(map_this[slot],"Empty")   != 0) && 
	 ( strcmp(map_this[slot],"Unknown") != 0) ) { 
      if        (strcmp(map_this[slot],"ADC1881")  == 0)   csr=csr_adc;
      else if ( (strcmp(map_this[slot],"TDC1872")  == 0) ||
		(strcmp(map_this[slot],"TDC1875")  == 0) ) csr=csr_tdc72;
      else if ( (strcmp(map_this[slot],"TDC1877")  == 0) ||
		(strcmp(map_this[slot],"TDC1877S") == 0) ) csr=csr_tdc77;
      else  {
	printf("\n Error: Slot type is corrupted.\n\n");
	return (-1);
      }

      printf("\n Slot = %2d,  Board type = %s \n", slot, map_this[slot]);
      for ( i_csr=1; i_csr<=csr[0]; i_csr++) {
	if ( fprc(slot, csr[i_csr], &csr_tmp) != 0) {
	  printf("\n Error: Can't read CSR%d for %s in slot %d.\n\n", \
		 csr[i_csr], map_this[slot], slot);
	  return (-1);
	}
	printf("  CSR%2d = 0x%8x\n", csr[i_csr], csr_tmp);
      }
    }
  }
  return(0);
}

#else /* no UNIX version */

void
fb_diag_dummy()
{
  return;
}

#endif
