/* UNIX version only */

#ifndef VXWORKS

#define SP_DEBUG_INHx
#define SP_DEBUG_RCFx
#define SP_DEBUG_Px

/* dimanc.c
 *
 * Program to set and monitor discriminators for diman GUI
 *   written  by vhg  7-Jul-1997
 *   modified by SP  11-Jan-2005
 *
 * Program sets (set) or monitors (mon) thresholds, widths and masks
 * of pretrigger (p) or channel (c) or all (all) discriminators
 * for all CLAS detectors (ec,cc,sc,lac)
 * basing on default config files:
 *   $CLON_PARMS/pretrigger/conf/l1_detname.conf
 *   $CLON_PARMS/discr/conf/detname_thresh.conf
 * or (cfn) - correct config file with correct full path.
 *
 * Working at (gui) mode program gets inputs from prompt.
 *
 * Usage:
 * ------
 *   dimanc set/mon/gui p/c/all ec/cc/sc/lac/all [cfn] [btn1 [btn2...]]
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <ca.h>
#include <libtcp.h>

extern void get_run_config();


/* open monitored output file */
#define OPEN_MON(Xmon,monFN,ses_det,run_pc) \
   sprintf(mon_file_name, (monFN), \
	   clonparms, (ses_det), (run_pc)); \
   if (((Xmon) = fopen(mon_file_name,"w")) == NULL) { \
     printf("\n Error: Can't open file %s \n\n", mon_file_name); \
     exit(101); \
   }


/* put header to common monitored output file */
#define HEAD_MON(Xmon,pret_chan) \
  fprintf((Xmon),"#  This file contains info of %s discriminators \n", \
	  (pret_chan)); \
  fprintf((Xmon),"#  created by vhg on %.2d/%.2d %.2d:%.2d:%.2d \n", \
	  zjam[5], zjam[4], zjam[3], zjam[2], zjam[1]); \
  fprintf((Xmon),"#  blank lines and lines starting with # are ignored \n\n"); \
  fprintf((Xmon),"*TIME* \n"); \
  fprintf((Xmon),"%11d           #  this is the unix time \n", t);


/* put header to single monitored output file */
#define HEAD_MSn(Xmon) \
  fprintf((Xmon),"file was created by 'dimanc'\n"); \
  fprintf((Xmon)," %.2d/%.2d %.2d:%.2d:%.2d \n", \
	  zjam[5], zjam[4], zjam[3], zjam[2], zjam[1]);


/* main chooser */
#define PrCh_MAIN(Xmon,PC,CFN,FuncName) \
  if ( !strcmp(pc,"all")   )      pc_tmp = (PC); \
  if ( !strcmp(pc_tmp,(PC)) ) { \
    OPEN_MON((Xmon),"%s/diman/mondat/mon_%s_%s.txt",det_tmp,(PC)); \
    HEAD_MSn((Xmon)); \
    if ( strlen(fma) == 0 ) \
      sprintf(fma, (CFN), clonparms, det_tmp); \
    printf(" ii=%d,  pc_tmp=%s,  cfn=\"%s\" \n", ii, pc_tmp, fma); \
    stt = (FuncName)(fma,mod,det_tmp); \
    sprintf(fma,""); \
    fclose((Xmon)); \
    printf(" ii=%d,  pc_tmp=%s,  stt=%d \n", ii, pc_tmp, stt); \
    if (stt != 0)     exit(stt); \
  }


/* calibration constants for 2313 discr. width :  BEST fit  */
#define PW1 12268
#define PW2 -7287
#define PW3 1529.3
#define PW4 -112.53

#define PR1 1.6114
#define PR2 -0.60865E-03
#define PR3 0.14891E-06
#define PR4 -0.16617E-10

/* calibration constants for 2313 discr. width :  SIMPLE fit  */
#define P1 33406
#define P2 209

/* dimension constants */
#define N_DET  4      /* number of CLAS detectors (ec,cc,sc,lac) */
#define SC_PR  18     /* number of SC pretrig discriminators */
#define T_STR  12     /* size of some text strings */
#define T_TCP  8192   /* size of string for response from tcpClientCmd */
#define F_NAME 100    /* length of config.(mon.) file name */
#define D_MAX  600    /* maximum number of discriminators */


/* arrays for pretrig/channel discr. parameters */
int db[D_MAX], dc[D_MAX], dn[D_MAX], da[D_MAX];
int dm[D_MAX], dt[D_MAX], dw[D_MAX];

/* variables to set discr. from GUI or from prompt */
int btn[6] = { -1, -1, -1, -1, -1, -1 };
/*-----------------------------------------------------------------
int   ec_p_i_h, ec_p_i_l,   cc_p_h1, cc_p_h2.             lac_p_t;
int   ec_p_o_h, ec_p_o_l,   cc_p_l1, cc_p_l2,   sc_p_t;
int   ec_p_t_h, ec_p_t_l,                       sc_p_w;
int   ec_c_t,               cc_c_t,             sc_c_t,   lac_c_t;
int   ec_c_w,               cc_c_w,             sc_c_w,   lac_c_w;
------------------------------------------------------------------*/

/* for cfsa(...) */
static int zero = 0;

/* Branch & Controller numbers for FC CAMAC crates:
     int FC_branches [] = {  1, 1, 0, 1, 2, 2, 2, 2,   0  };
     int FC_crates   [] = {  0, 1, 2, 3, 4, 5, 6, 7,   1  };

  Branch & Controller numbers for LAC CAMAC crates:
     int LAC_branches [] = {  0, 0  };
     int LAC_crates   [] = {  0, 1  };    */

/* branch number & controller number for CAMAC crate in FC Rack C2-6 */
int b_FC_C2_6 = 0;
int c_FC_C2_6 = 2;


/* rocnames of crates with detectors discriminators */
char *roc       = "camac1";
char *lac_roc   = "camac2";
/* next line was used just for PRIMEX II run period
char *lac_roc   = "dccntrl";
*/

char *sc_roc_P1 = "pretrig1";
char *sc_roc_P2 = "pretrig2";
char sc_roc_Pi[SC_PR][T_STR];

/* monitored file */
static FILE *pmon    = NULL;
static FILE *cmon    = NULL;
static FILE *pALLmon = NULL;
static FILE *cALLmon = NULL;

/* get main env variables */
char *getenv();
char *clonparms;

/* run parameters */
char *mysql_database = "clasrun";
char *session;                   /* CODA session */
char *config;                    /* CODA config  */
int   run = 0;                   /* RUN number   */


/* function prototypes */
int ReadConfigFile (char *fname,
		    int *bb,
		    int *cc,
		    int *nn,
		    int *ma,
		    int *th,
		    int *a_w);

int ReadConfigFile_l1_cc (char *fname, int *th);
int ReadConfigFile_l1_sc (char *fname, int *nn, int *th, int *ww, int *ma);

int PrDiscr_setmon (char *fname, char *mod, char *det);
int ChDiscr_setmon (char *fname, char *mod, char *det);
int takeOFFinhibit (int *bb, int *cc, int kk);


/*************************************************************************/
/*************************************************************************/
/*  main program                                                         */
/*************************************************************************/
int
main(int argc, char *argv[])
{
  char fma[F_NAME] = { "" };   /* config file name */
  char mon_file_name[F_NAME];  /* monitored file name */
  char *mod, *pc, *det;        /* Inputs: mode, discr.type, det.name */
  int  ii, stt = 0;
  char *det_names[] = { "ec", "cc", "sc", "lac" };
  char *pc_tmp  = "all";
  char *det_tmp = "all";

  /* for date and time */
  struct tm *mt;
  time_t    t;
  int       zjam[5];


/* get the environment variables and run number */
  clonparms = getenv("CLON_PARMS");
  session   = getenv("DD_NAME");
  if (session == NULL)   session = "clasprod";
  get_run_config(mysql_database, session, &run, &config);


/* check input parameters */
  if ((argc < 4) ||  (argc > 10))                     stt = 1;
  else {
    mod = argv[1];
    pc  = argv[2];
    det = argv[3];

    if (argc == 5)  strcpy(fma,argv[4]);

    for (ii=4; ii<argc; ii++) {
      btn[ii-4] = atoi(argv[ii]);
      if ( btn[ii-4] < 0 )                            stt = 2;
    }
    if      ( (strcmp(mod, "set") != 0) &&
	      (strcmp(mod, "mon") != 0) &&
	      (strcmp(mod, "gui") != 0) )             stt = 3;
    else if ( (strcmp(pc,  "p")   != 0) &&
	      (strcmp(pc,  "c")   != 0) &&
	      (strcmp(pc,  "all") != 0) )             stt = 4;
    else if ( (strcmp(det, "ec")  != 0) &&
	      (strcmp(det, "cc")  != 0) &&
	      (strcmp(det, "sc")  != 0) &&
	      (strcmp(det, "lac") != 0) &&
	      (strcmp(det, "all") != 0) )             stt = 5;

    if (stt != 0) {}
    else if ( !strcmp(mod,"gui") && !strcmp(pc,"p") ) {
      if      ( !strcmp(det,"ec")  && (argc != 10) )  stt = 6;
      else if ( !strcmp(det,"cc")  && (argc !=  8) )  stt = 7;
      else if ( !strcmp(det,"sc")  && (argc !=  6) )  stt = 8;
      else if ( !strcmp(det,"lac") && (argc !=  5) )  stt = 9;
      else if ( !strcmp(det,"all") )                  stt = 10;
    }
    else if ( !strcmp(mod,"gui") && !strcmp(pc,"c") ) {
      if      ( argc !=  6 )                          stt = 11;
      else if ( !strcmp(det,"all") )                  stt = 12;
    }
    else if ( !strcmp(mod,"gui") && !strcmp(pc,"all") ) {
                                                      stt = 13;
    }
  }

  if (stt != 0) {
    printf("\n Usage: dimanc");
    printf(" set/mon/gui p/c/all ec/cc/sc/lac/all [cfn] [btn1 [btn2...]]\n\n");
    printf("  \"set\" - to set or monitor pretrigger/channel discriminators \n");
    printf("    /     thresholds, widths & masks basing on default config files\n");
    printf("  \"mon\"   \"$CLON_PARMS/pretrigger/conf/l1_detname.conf\" and \n");
    printf("          \"$CLON_PARMS/discr/conf/detname_thresh.conf\"        \n");
    printf("          or \"cfn\" - correct config file with correct full path\n\n");
    printf("  \"gui\" - to set pr/ch discriminators from GUI or from prompt \n");
    printf("          input correct values and numbers of btn1 ... btn6 > 0 \n");
    printf("          in accordance with the following mnemonics:           \n");
    printf("    ----------------------------------------------------------- \n");
    printf("    dimanc gui p  ec in_hi out_hi tot_hi in_low out_low tot_low \n");
    printf("    dimanc gui p  cc cc_hi_1 cc_low_1 cc_hi_2 cc_low_2 \n");
    printf("    dimanc gui p  sc sc_p_thresh sc_p_width \n");
    printf("    dimanc gui p lac lac_p_thresh \n");
    printf("    dimanc gui c  ec  ec_c_thresh  ec_c_width \n");
    printf("    dimanc gui c  cc  cc_c_thresh  cc_c_width \n");
    printf("    dimanc gui c  sc  sc_c_thresh  sc_c_width \n");
    printf("    dimanc gui c lac lac_c_thresh lac_c_width \n\n");
    exit(stt);
  }


/* get time and date */
  t = time(NULL);
  mt = localtime(&t);
  zjam[1] = mt->tm_sec;
  zjam[2] = mt->tm_min;
  zjam[3] = mt->tm_hour;
  zjam[4] = mt->tm_mday;
  zjam[5] = mt->tm_mon+1;


/* printout start-time and some globals for current run */
  printf("\n Start time = %s \n", ctime(&t));
  printf(" clonparms=%s;\n   session=%s;\n"
	 "    config=%s;\n       run=%06d;\n",
	 clonparms, session, config, run);


/* common monitored output files: check, open and put header */
  if ( !strcmp(mod,"mon") && !strcmp(det,"all") ) {
    if ( !strcmp(pc,"p") || !strcmp(pc,"all") ) {
      OPEN_MON(pALLmon,"%s/pretrigger/archive/pretrig_%s_%06d.txt",session,run);
      HEAD_MON(pALLmon,"pretrigger");
    }
    if ( !strcmp(pc,"c") || !strcmp(pc,"all") ) {
      OPEN_MON(cALLmon,"%s/discr/archive/discr_%s_%06d.txt",session,run);
      HEAD_MON(cALLmon,"channel");
    }
  }


/* check existence of the config file inputed from command line */
  if ( strlen(fma) > 0 ) {
    if      ( !strcmp(mod,"gui") )                      stt = 401;
    else if ( !strcmp(pc, "all") )                      stt = 402;
    else if ( !strcmp(det,"all") )                      stt = 403;
    else if ( fopen(fma,"r") == NULL )                  stt = 404;
  }
  if (stt != 0) {
    switch (stt) {
    case 401:  printf("\n\n Working at \"gui\" mode");              break;
    case 402:  printf("\n\n Working with \"all\" discriminators");  break;
    case 403:  printf("\n\n Working with \"all\" detectors");       break;
    case 404:  printf("\n\n Can't open config file  \"%s\"", fma);  break;
    }
    printf("\n Program will work with default config files. \n\n");
    sprintf(fma,"");
  }


/* start process of monitoring or setting */
  for (ii=0; ii<N_DET; ii++) {
    if ( !strcmp(det,"all") ) {
      det_tmp = det_names[ii];
    }
    else {
      det_tmp = det;
      ii = N_DET;
    }
    pc_tmp = pc;
    printf("\n ii=%d,  det=%s,   det_tmp=%s \n", ii, det, det_tmp);

    PrCh_MAIN(pmon,"p","%s/pretrigger/conf/l1_%s.conf",PrDiscr_setmon);
    PrCh_MAIN(cmon,"c","%s/discr/conf/%s_thresh.conf",ChDiscr_setmon);
  }


/* close of common monitored output file */
  if (pALLmon)   fclose(pALLmon);
  if (cALLmon)   fclose(cALLmon);


/* printout end-time */
  t = time(NULL);
  printf("\n End time = %s \n", ctime(&t));

  exit(stt);   /* end of main() */
}


/*************************************************************************/
/*  read almost all config files (except: l1_cc.conf & l1_sc.conf)       */
/*************************************************************************/
int
ReadConfigFile ( char *fname,
		 int *bb,
		 int *cc,
		 int *nn,
		 int *ma,
		 int *th,
		 int *a_w)
{
  FILE *ff;
  int  k, ch;
  char tt[T_STR];

  if ( (ff = fopen(fname,"r")) == NULL) {
    printf("\n Error: Can't open config.file %s \n\n", fname);
    return(-1); }

  k = 0;
  while ((ch = getc(ff)) != EOF) {
    if     ( ch == '#' || ch == ' ' || ch == '\t' ) {
      while (getc(ff) != '\n') {}
    }
    else if( ch == '\n' ) {}
    else {
      ungetc(ch,ff);

      if( (strstr(fname, "ec_thresh.conf") != NULL) ||
	  (strstr(fname, "cc_thresh.conf") != NULL) ||
	  (strstr(fname, "sc_thresh.conf") != NULL) ) {

	fscanf(ff,"%s %s %d %s %d %s %d %s %x %s %d %s %d \n",
	       tt, tt, &bb[k], tt, &cc[k], tt,  &nn[k],
	       tt,     &ma[k], tt, &th[k], tt, &a_w[k]);
      }
      else {
	fscanf(ff,"%s %s %d %s %d %s %d %s %d %s %x %s %d \n",
	       tt, tt, &bb[k], tt, &cc[k], tt, &nn[k],
	       tt,    &a_w[k], tt, &ma[k], tt, &th[k]);
      }
#ifdef SP_DEBUG_RCF
      printf("sssRCF ::  k=%.3d,  last_str=\"%s\",  b=%d, c=%d, n=%.2d, "
	     "mask=0x%.4x, thresh=%d, a_w=%d \n",
	     k, tt,  bb[k], cc[k], nn[k], ma[k], th[k], a_w[k]);
#endif
      k++;
    }
  }
  fclose(ff);

  return(k);
}


/*************************************************************************/
/*  read only CC config files: l1_cc.conf                                */
/*************************************************************************/
int
ReadConfigFile_l1_cc (char *fname, int *th)
{
  FILE *ff;
  int  k, ch;

  if ( (ff = fopen(fname,"r")) == NULL) {
    printf("\n Error: Can't open config.file %s \n\n", fname);
    return(-1); }

  k = 0;
  while ((ch = getc(ff)) != EOF) {
    if     ( ch == '#' || ch == ' ' || ch == '\t' ) {
      while (getc(ff) != '\n') {}
    }
    else if( ch == '\n' ) {}
    else {
      ungetc(ch,ff);
      fscanf(ff, "%d %d %d %d \n", &th[0], &th[1], &th[2], &th[3]);
#ifdef SP_DEBUG_RCF
      printf("sssRCFcc ::  k=%d,  th0=%d, th1=%d, th2=%d, th3=%d \n",
	     k, th[0], th[1], th[2], th[3]);
#endif
      k++;
    }
  }
  fclose(ff);

  return(k);
}


/*************************************************************************/
/*  read only SC config files: l1_sc.conf                                */
/*************************************************************************/
int
ReadConfigFile_l1_sc (char *fname, int *nn, int *th, int *ww, int *ma)
{
  FILE *ff;
  int  k, ch;

  if ( (ff = fopen(fname,"r")) == NULL) {
    printf("\n Error: Can't open config.file %s \n\n", fname);
    return(-1); }

  k = 0;
  while ((ch = getc(ff)) != EOF) {
    if     ( ch == '#' || ch == ' ' || ch == '\t' ) {
      while (getc(ff) != '\n') {}
    }
    else if( ch == '\n' ) {}
    else {
      ungetc(ch,ff);
      fscanf(ff, "%s %d %d %d %d \n", sc_roc_Pi[k], 
	     &nn[k], &th[k], &ww[k], &ma[k]);
#ifdef SP_DEBUG_RCF
      printf("sssRCFsc ::  k=%.2d,  sc_roc_Pi=\"%s\",  n=%d, "
	     "thresh=%d, width=%d, mask=%d \n",
	     k, sc_roc_Pi[k], nn[k], th[k], ww[k], ma[k]);
#endif
      k++;
    }
  }
  fclose(ff);

  return(k);
}


#define L1ECKOEF 4 /* Sergey 23-mar-2005 */

/*************************************************************************/
/*  function for level 1 pretrigger discriminator control                */
/*************************************************************************/
int
PrDiscr_setmon (char *fname, char *mod, char *det)
{
  int  stat, ext, data, q;
  int  kk, ii, nj;
  int  mean_tw[6] = { 0, 0, 0, 0, 0, 0 };
  int  t_tmp, w_tmp;
  char result[T_TCP];   /* string for messages from tcpClientCmd */
  char exename[80];     /* vpi board discr vxworks exec name */


  /* open the connection with ROC */
  caopen(roc, &stat);
  if (stat!=1) {
    printf("\n Error: Failed to establish connection with ROC = %s\n\n", roc);
    return (-1); }

  /* take off inhibit */
  cdreg(&ext,b_FC_C2_6,c_FC_C2_6,30,0);
  cfsa(17,ext,&zero,&q);


/************  EC pretrig discriminators  ************/

  if( !strcmp(det,"ec") ) {

    kk = ReadConfigFile(fname,db,dc,dn,dm,dt,da);
    if ( kk <= 0 )   return(-2);

/* EC monitoring mode */
    if( !strcmp(mod,"mon") ) {

      /* EC level1 thresholds read */
      for (ii=0; ii<kk; ii++) {
	q = 0;
	cdreg(&ext,db[ii],dc[ii],dn[ii],da[ii]);
	cfsa(0,ext,&data,&q);
	if (!q) {
	  printf("b=%d, c=%d, n=%d, a=%d, camac read error: data=%d\n",
		 db[ii], dc[ii], dn[ii], da[ii], data);
	  dt[ii] = -1;
	}
	else
	  dt[ii] = (data & 0xFF)*L1ECKOEF;
      }

      /* EC level1 masks read */
      ii=0;
      for (nj=11; nj<=16; nj++) {
	q=0;
	cdreg(&ext,b_FC_C2_6,c_FC_C2_6,nj,0);
	cfsa(1,ext,&data,&q);
	if (!q) {
	  printf("b=%d, c=%d, n=%d, a=0, camac read error: data=0x%.4x\n",
		 b_FC_C2_6, c_FC_C2_6, nj, data);
	  dm[ii] = -1;
	}
	else
	  dm[ii] = data;
	ii++;
      }

      /* fill up of common monitored file */
      if (pALLmon) {
	fprintf(pALLmon,"\n\n*EC_L1_T*  level1 thresholds data for EC\n\n");
	fprintf(pALLmon,"#  sec1  sec2  sec3  sec4  sec5  sec6\n");
	nj=0;
	for (ii=0; ii<kk; ii++) {
	  fprintf(pALLmon,"%6d", dt[ii]);
	  mean_tw[nj] = mean_tw[nj] + dt[ii];
	  if ( ((ii+1) % 6 == 0) && ii ) {
	    fprintf(pALLmon,"\n");
	    nj++;
	  }
	}
	fprintf(pALLmon,"\n*EC_L1_M*  level1 masks data for EC\n\n");
	fprintf(pALLmon,"# sec1   sec2   sec3   sec4   sec5   sec6\n");
	for (ii=0; ii<6; ii++)
	  fprintf(pALLmon," %6d", dm[ii]);
	fprintf(pALLmon,"\n\n*EC_Mean*  inner_outer_total/high ,"
                                      " inner_outer_total/low \n");
	for (ii=0; ii<6; ii++)
	  fprintf(pALLmon," %6d", mean_tw[ii]/6);
      }

      /* fill up of single monitored file */
      fprintf(pmon,"\n*EC_L1_T*  level1 thresholds data for EC\n\n");
      fprintf(pmon,"#  sec1  sec2  sec3  sec4  sec5  sec6\n");
      for (ii=0; ii<kk; ii++) {
	fprintf(pmon,"%6d", dt[ii]);
	if ( ((ii+1) % 6 == 0) && ii )
	  fprintf(pmon,"\n");
      }
      fprintf(pmon,"\n\n*EC_L1_M*  level1 masks data for EC\n\n");
      fprintf(pmon,"# sec1   sec2   sec3   sec4   sec5   sec6\n");
      for (ii=0; ii<6; ii++)
	fprintf(pmon," 0x%.4x", dm[ii]);
      fprintf(pmon,"\n\n");

    } /* end of EC monitoring mode */


/* EC setting mode */
    else {

      /* EC level1 thresholds set */
      for (ii=0; ii<kk; ii++) {

	if ( !strcmp(mod,"gui") ) {    /* check GUI set */
	  switch (da[ii]) {
	  case 0:
	    dt[ii] = btn[0];        /* redefine EC inner high */
	    break;
	  case 1:
	    dt[ii] = btn[1];        /* redefine EC outer high */
	    break;
	  case 2:
	    dt[ii] = btn[2];        /* redefine EC total high */
	    break;
	  case 3:
	    dt[ii] = btn[3];        /* redefine EC inner low */
	    break;
	  case 4:
	    dt[ii] = btn[4];        /* redefine EC outer low */
	    break;
	  case 5:
	    dt[ii] = btn[5];        /* redefine EC total low */
	    break;
	  }
	}

/* Sergey B. on Vardan's advice: was 2, must be 4 !!! (23-mar-2005) */
	dt[ii] = dt[ii]/L1ECKOEF;     /* step is 2mv */

	if ( dt[ii] <   0 )  dt[ii] =   0;
	if ( dt[ii] > 255 )  dt[ii] = 255;

	q = 0;
	cdreg(&ext,db[ii],dc[ii],dn[ii],da[ii]);
	cfsa(16,ext,&dt[ii],&q);
	if (!q)
	  printf("b=%d, c=%d, n=%d, a=%d, camac set error of thresh=%d\n",
		 db[ii], dc[ii], dn[ii], da[ii], dt[ii]);
      }

      /* EC level1 masks set */
      ii=0;
      for (nj=11; nj<=16; nj++) {
	q=0;
	cdreg(&ext,b_FC_C2_6,c_FC_C2_6,nj,0);
	cfsa(17,ext,&dm[ii],&q);
	if (!q)
	  printf("b=%d, c=%d, n=%d, a=0, camac set error of mask=0x%.4x\n",
		 b_FC_C2_6, c_FC_C2_6, nj, dm[ii]);
	ii++;
      }
    } /* end of EC setting mode */

  } /* end of EC pretrig discriminators */


/************  CC pretrig discriminators  ************/

  else if( !strcmp(det,"cc") ) {

/* CC monitoring mode */
    if( !strcmp(mod,"mon") ) {

      /* CC level1 thresholds read */
      for (ii=0; ii<4; ii++) {
	q = 0;
	cdreg(&ext,b_FC_C2_6,c_FC_C2_6,1,ii);
	cfsa(0,ext,&data,&q);
	if (!q) {
	  printf("b=%d, c=%d, n=1, a=%d, camac read error: data=%d\n",
		 b_FC_C2_6, c_FC_C2_6, ii, data);
	  dt[ii] = -1;
	}
	else           /* converting DAC step to mv */
	  dt[ii] = (2047 - (data & 0x7FF)) / 4;
      }

      /* fill up of common monitored file */
      if (pALLmon) {
	fprintf(pALLmon,"\n\n\n*CC_L1_T*  level1 thresholds data for CC\n");
	for (ii=0; ii<4; ii++)
	  fprintf(pALLmon," %6d", dt[ii]);
	fprintf(pALLmon,"\n\n*CC_Mean*  high/low \n");
        fprintf(pALLmon," %6d %6d \n\n\n",
		(dt[0] + dt[2])/2,
		(dt[1] + dt[3])/2);
      }

      /* fill up of single monitored file */
      fprintf(pmon,"\n*CC_L1_T*  level1 thresholds data for CC\n\n");
      for (ii=0; ii<4; ii++)
	fprintf(pmon," %6d", dt[ii]);
      fprintf(pmon,"\n\n");

    } /* end of CC monitoring mode */


/* CC setting mode */
    else {

      kk = ReadConfigFile_l1_cc(fname,dt);
      if ( kk <= 0 )   return(-3);

      if ( !strcmp(mod,"gui") ) {      /* check GUI set */
	dt[0] = btn[0];             /* redefine CC high-1 thresh. */
	dt[1] = btn[1];             /* redefine CC low-1  thresh. */
	dt[2] = btn[2];             /* redefine CC high-2 thresh. */
	dt[3] = btn[3];             /* redefine CC low-2  thresh. */
      }

      /* thresholds calibration */
      for (ii=0; ii<4; ii++) {
	dt[ii] = 2047 - 4*dt[ii];
	if ( dt[ii] <    0 )  dt[ii] =    0;
	if ( dt[ii] > 2047 )  dt[ii] = 2047;
      }

      /* CC level1 thresholds set */
      for (ii=0; ii<4; ii++) {
	q = 0;
	cdreg(&ext,b_FC_C2_6,c_FC_C2_6,1,ii);
	cfsa(16,ext,&dt[ii],&q);
	if (!q)
	  printf("b=%d, c=%d, n=1, a=%d, camac set error of thresh=%d\n",
		 b_FC_C2_6, c_FC_C2_6, ii, dt[ii]);
      }
    } /* end of CC setting mode */

  } /* end of CC pretrig discriminators */


/************  SC pretrig discriminators  ************/

  else if( !strcmp(det,"sc") )
  {

    /* SC monitoring mode */
    if( !strcmp(mod,"mon") )
    {
      /* SC level1 thresholds read and fill up of monitored file */

      fprintf(pmon,"\n*SC_L1_T*  level1 thresholds data for SC\n\n");
      if (pALLmon)
      {
        fprintf(pALLmon,"*SC_L1_T*  level1 thresholds data for SC\n");
      }

      memset(result,0,T_TCP);
      tcpClientCmd(sc_roc_P1, "mon_vpi_all2", result);

      fprintf(pmon,"%s \n", result);
      if (pALLmon)
      {
        fprintf(pALLmon,"%s \n", result);
        for (ii=0; ii<(SC_PR/2); ii++)
        {
          sscanf(result,"%d %d %d \n", &t_tmp, &w_tmp, &data);
          mean_tw[0] = mean_tw[0] + t_tmp;
          mean_tw[1] = mean_tw[1] + w_tmp;
        }
      }

      memset(result,0,T_TCP);
      tcpClientCmd(sc_roc_P2, "mon_vpi_all2", result);

      fprintf(pmon,"%s \n\n", result);
      if (pALLmon)
      {
        fprintf(pALLmon,"%s \n\n", result);
        for (ii=0; ii<(SC_PR/2); ii++)
        {
          sscanf(result,"%d %d %d \n", &t_tmp, &w_tmp, &data);
          mean_tw[2] = mean_tw[2] + t_tmp;
          mean_tw[3] = mean_tw[3] + w_tmp;
        }
      }

      if (pALLmon)
      {
        fprintf(pALLmon,"*SC_MeanThr* \n");
        fprintf(pALLmon," %6d \n\n", (mean_tw[0] + mean_tw[2])/SC_PR);
	    fprintf(pALLmon,"*SC_MeanWid* \n");
        fprintf(pALLmon," %6d \n\n", (mean_tw[1] + mean_tw[3])/SC_PR);
      }

    } /* end of SC monitoring mode */


    /* SC setting mode */
#define USE_FAST_SC_LOAD
    else
    {
#ifdef USE_FAST_SC_LOAD
      unsigned int twm[2][9];
#endif
      kk = ReadConfigFile_l1_sc(fname,dn,dt,dw,dm);
      if ( kk <= 0 ) return(-4);

      for (ii=0; ii<kk; ii++)
      {
        if ( !strcmp(mod,"gui") )
        {    /* check GUI set */
          dt[ii] = btn[0];          /* redefine SC pretrig thresh. */
          dw[ii] = btn[1];          /* redefine SC pretrig width   */
        }
        if ( dt[ii] <  30 )  dt[ii] =  30;
        if ( dt[ii] > 787 )  dt[ii] = 787;
        if ( dw[ii] <  30 )  dw[ii] =  30;
        if ( dw[ii] > 183 )  dw[ii] = 183;



#ifdef USE_FAST_SC_LOAD

        /* pack threshold, width and mask in one 'int' word */
        twm[ii/9][ii%9] = (dm[ii]<<20) | (dw[ii]<<10) | dt[ii];

#else

        sprintf(exename, "set_vpi_thresh(%d,%d)", dn[ii], dt[ii]);
        memset(result,0,T_TCP);
        tcpClientCmd(sc_roc_Pi[ii], exename, result);
#ifdef SP_DEBUG_P
        printf("sssPDsc1 ::  \"%s\"  ii=%.2d,  result=%s;; \n",
	       sc_roc_Pi[ii], ii, result);
#endif
        sprintf(exename, "set_vpi_width(%d,%d)",  dn[ii], dw[ii]);
        memset(result,0,T_TCP);
        tcpClientCmd(sc_roc_Pi[ii], exename, result);
#ifdef SP_DEBUG_P
        printf("sssPDsc2 ::  \"%s\"  ii=%.2d,  result=%s;; \n",
	       sc_roc_Pi[ii], ii, result);
#endif
        sprintf(exename, "set_vpi_mask(%d,%d)",   dn[ii], dm[ii]);
        memset(result,0,T_TCP);
        tcpClientCmd(sc_roc_Pi[ii], exename, result);
#ifdef SP_DEBUG_P
        printf("sssPDsc3 ::  \"%s\"  ii=%.2d,  result=%s;; \n\n",
	       sc_roc_Pi[ii], ii, result);
#endif

#endif /*USE_FAST_SC_LOAD*/

      }


#ifdef USE_FAST_SC_LOAD
printf("1--------------\n");
printf("set_vpi_all_pack(%d,%d,%d,%d,%d,%d,%d,%d,%d)\n",
              twm[0][0],twm[0][1],twm[0][2],
              twm[0][3],twm[0][4],twm[0][5],
              twm[0][6],twm[0][7],twm[0][8]);
printf("2--------------\n");

      sprintf(exename,
              "set_vpi_all_pack(%d,%d,%d,%d,%d,%d,%d,%d,%d)",
              twm[0][0],twm[0][1],twm[0][2],
              twm[0][3],twm[0][4],twm[0][5],
              twm[0][6],twm[0][7],twm[0][8]);
      memset(result,0,T_TCP);

printf("exename >%s<\n",exename);
printf("3--------------\n");
      tcpClientCmd(sc_roc_P1, exename, result);
	  
      sprintf(exename,
              "set_vpi_all_pack(%d,%d,%d,%d,%d,%d,%d,%d,%d)",
              twm[1][0],twm[1][1],twm[1][2],
              twm[1][3],twm[1][4],twm[1][5],
              twm[1][6],twm[1][7],twm[1][8]);
      memset(result,0,T_TCP);
      tcpClientCmd(sc_roc_P2, exename, result);
#endif


    } /* end of SC setting mode */

  } /* end of SC pretrig discriminators */


/************  LAC pretrig discriminators  ************/

  else {

    kk = ReadConfigFile(fname,db,dc,dn,dm,dt,da);
    if ( kk <= 0 )   return(-5);

/* LAC monitoring mode */
    if( !strcmp(mod,"mon") ) {

      /* LAC level1 thresholds read */
      for (ii=0; ii<kk; ii++) {
	q = 0;
	cdreg(&ext,db[ii],dc[ii],dn[ii],da[ii]);
	cfsa(0,ext,&data,&q);
	if (!q) {
	  printf("b=%d, c=%d, n=%d, a=%d, camac read error: data=%d\n",
		 db[ii], dc[ii], dn[ii], da[ii], data);
	  dt[ii] = -1;
	}
	else
	  dt[ii] = (data & 0xFF)*2;
      }

      /* LAC level1 masks read */
      q=0;
      cdreg(&ext,db[0],dc[0],dn[0],0);
      cfsa(1,ext,&data,&q);
      if (!q) {
	printf("b=%d, c=%d, n=%d, a=0, camac read error: data=0x%.4x\n",
	       db[0], dc[0], dn[0], data);
	dm[0] = -1;
      }
      else
	dm[0] = data;

      /* fill up of monitored file */
      fprintf(pmon,"\n*LAC_L1_T*  level1 thresholds data for LAC\n\n");
      for (ii=0; ii<kk; ii++)    fprintf(pmon,"%6d", dt[ii]);
      fprintf(pmon,"\n\n*LAC_L1_M*  level1 masks data for LAC\n\n");
      fprintf(pmon," 0x%.4x\n\n", dm[0]);

    } /* end of LAC monitoring mode */


/* LAC setting mode */
    else {

      /* LAC level1 thresholds set */
      for (ii=0; ii<kk; ii++) {

	if ( !strcmp(mod,"gui") ) {    /* check GUI set */
	  dt[ii] = btn[0];          /* redefine LAC pretrig thresh. */
	}
	dt[ii] = dt[ii]/2;     /* step is 2mv */

	if ( dt[ii] <   0 )  dt[ii] =   0;
	if ( dt[ii] > 255 )  dt[ii] = 255;

	q = 0;
	cdreg(&ext,db[ii],dc[ii],dn[ii],da[ii]);
	cfsa(16,ext,&dt[ii],&q);
	if (!q)
	  printf("b=%d, c=%d, n=%d, a=%d, camac set error of thresh=%d\n",
		 db[ii], dc[ii], dn[ii], da[ii], dt[ii]);
      }

      /* LAC level1 masks set */
      q=0;
      cdreg(&ext,db[0],dc[0],dn[0],0);
      cfsa(17,ext,&dm[0],&q);
      if (!q)
	printf("b=%d, c=%d, n=%d, a=0, camac set error of mask=0x%.4x\n",
	       db[0], dc[0], dn[0], dm[0]);

    } /* end of LAC setting mode */

  } /* end of LAC pretrig discriminators */

  return(0);   /* end of "PrDiscr_setmon()" */
}


/*************************************************************************/
/*  function for channel discriminator control                           */
/*************************************************************************/
int
ChDiscr_setmon (char *fname, char *mod, char *det)
{
  int   stat, ext, data, q;
  int   kk, ii, nj;
  char  roc_ch[T_STR];
  float tempf;


/* channel discriminators for LAC are CAEN C207 */
  if( !strcmp(det,"lac") ) {
    kk = ReadConfigFile(fname,db,dc,dn,dm,dt,da);
    sprintf(roc_ch, "%s", lac_roc);
  }
/* channel discriminators for EC,CC,SC are LeCroy 2313 */
  else {
    kk = ReadConfigFile(fname,db,dc,dn,dm,dt,dw);
    sprintf(roc_ch, "%s", roc);
  }
  if ( kk <= 0 )  return(-1);

  /* open the connection with ROC */
  caopen(roc_ch, &stat);
  if (stat!=1) {
    printf("\n Error: Failed to establish connection with ROC = %s\n\n", roc_ch);
    return (-2); }

  /* take off inhibit */
  takeOFFinhibit(db,dc,kk);


/************  LAC channel discriminators  ************/

  if( !strcmp(det,"lac") ) {

/* LAC ch.discr. monitoring mode */
    if( !strcmp(mod,"mon") ) {

      /* LAC ch.discr. thresholds read */
      for (ii=0; ii<kk; ii++) {
	q = 0;
	cdreg(&ext,db[ii],dc[ii],dn[ii],da[ii]);
	cfsa(0,ext,&data,&q);
	if (!q) {
	  printf("b=%d, c=%d, n=%d, a=%d, camac read error: data=%d\n",
		 db[ii], dc[ii], dn[ii], da[ii], data);
	  dt[ii] = -1;
	}
	else
	  dt[ii] = (data & 0xFF)*2;
      }

      /* fill up of common monitored file */
      if (cALLmon) {
	fprintf(cALLmon,"\n\n*LAC_T*  channel discriminator data \n\n");
	for (ii=0; ii<kk; ii++)
	  fprintf(cALLmon,"%6d %6d %6d %6d \n",
		  dc[ii], dn[ii], da[ii], dt[ii]);
	fprintf(cALLmon,"\n*LAC_M*  masks for the LAC \n\n");
	fprintf(cALLmon,"%x common mask is set \n\n\n", dm[3]);
      }

      /* fill up of single monitored file */
      fprintf(cmon,"*%s* channel discriminator data \n\n", det);
      for (ii=0; ii<kk; ii++)
	fprintf(cmon,"%6d %6d %6d %6d \n",
		dc[ii], dn[ii], da[ii], dt[ii]);

    } /* end of LAC ch.discr. monitoring mode */


/* LAC ch.discr. setting mode */
    else {

      /* LAC ch.discr. thresholds and masks set */
      for (ii=0; ii<kk; ii++) {

	if ( !strcmp(mod,"gui") ) {    /* check GUI set */
	  dt[ii] = btn[0];          /* redefine LAC channel thresh. */
	}
	dt[ii] = dt[ii]/2;     /* step is 2mv */

	if ( dt[ii] <   0 )  dt[ii] =   0;
	if ( dt[ii] > 255 )  dt[ii] = 255;

	q = 0;
	cdreg(&ext,db[ii],dc[ii],dn[ii],da[ii]);
	cfsa(16,ext,&dt[ii],&q);        /* set thresholds */
	if (!q)
	  printf("b=%d, c=%d, n=%d, a=%d, camac set error of thresh=%d\n",
		 db[ii], dc[ii], dn[ii], da[ii], dt[ii]);

	if ( da[ii] == 0 ) {
	  q=0;
	  cfsa(17,ext,&dm[ii],&q);      /* set masks      */
	  if (!q)
	    printf("b=%d, c=%d, n=%d, a=0, camac set error of mask=0x%.4x\n",
		   db[ii], dc[ii], dn[ii], dm[ii]);
	}
      }
    } /* end of LAC ch.discr. setting mode */

  } /* end of LAC channel discriminators */


/************  EC,CC,SC channel discriminators  ************/
  else {

/* EC,CC,SC ch.discr. monitoring mode */
    if( !strcmp(mod,"mon") ) {

      for (ii=0; ii<kk; ii++) {
	cdreg(&ext,db[ii],dc[ii],dn[ii],0);

	q = 0;
	cfsa(26,ext,&data,&q);       /* set discr. to remote mode */
	if (!q)
	  printf("b=%d, c=%d, n=%d, a=0, camac remote mode set error\n",
		 db[ii], dc[ii], dn[ii]);

	q = 0;
	cfsa(0,ext,&data,&q);        /* reading the masks */
	data &= 0xFFFF;
	if (!q) {
	  printf("b=%d, c=%d, n=%d, a=0, camac read mask error\n",
		 db[ii], dc[ii], dn[ii]);
	  dm[ii] = -1;
	}
	else
	  dm[ii] = data;

	q = 0;
	cdreg(&ext,db[ii],dc[ii],dn[ii],1);
	cfsa(0,ext,&data,&q);        /* reading the thresholds */
	data &= 0xFFF;
	if (!q) {
	  printf("b=%d, c=%d, n=%d, a=1, camac read threshold error\n",
		 db[ii], dc[ii], dn[ii]);
	  dt[ii] = -1;
	}
	else
	  dt[ii] = data*1024/4095 + 8;

	q = 0;
	cdreg(&ext,db[ii],dc[ii],dn[ii],2);
	cfsa(0,ext,&data,&q);        /* reading the widths */
	data &= 0xFFF;
	if (!q) {
	  printf("b=%d, c=%d, n=%d, a=2, camac read width error\n",
		 db[ii], dc[ii], dn[ii]);
	  dw[ii] = -1;
	}
	else {
	  tempf  = (float) (data);
	  tempf  = ((PR4*tempf+PR3)*tempf+PR2)*tempf+PR1;
	  dw[ii] = (int) floor(exp(exp(tempf)));
	  /*
	  dw[ii] = P1/(data + P2);
	  */
	}
      }

      /* fill up of common monitored file */
      if (cALLmon) {
	fprintf(cALLmon,"\n\n*%s*  channel discriminator data \n\n", det);
	for (ii=0; ii<kk; ii++)
	  fprintf(cALLmon,"%6d %6d %6x %6d %6d \n",
		  dc[ii], dn[ii], dm[ii], dt[ii], dw[ii]);
      }

      /* fill up of single monitored file */
      fprintf(cmon,"*%s* channel discriminator data \n\n", det);
      for (ii=0; ii<kk; ii++)
	fprintf(cmon,"%6d %6d %6x %6d %6d \n",
		dc[ii], dn[ii], dm[ii], dt[ii], dw[ii]);

    } /* end of EC,CC,SC ch.discr. monitoring mode */


/* EC,CC,SC ch.discr. setting mode */
    else {

      for (ii=0; ii<kk; ii++) {
	cdreg(&ext,db[ii],dc[ii],dn[ii],0);

	q = 0;
	cfsa(26,ext,&data,&q);       /* set discr. to remote mode */
	if (!q)
	  printf("b=%d, c=%d, n=%d, a=0, camac remote mode set error\n",
		 db[ii], dc[ii], dn[ii]);

	q = 0;
	cfsa(16,ext,&dm[ii],&q);     /* setting the masks */
	if (!q)
	  printf("b=%d, c=%d, n=%d, a=0, camac set mask error\n",
		 db[ii], dc[ii], dn[ii]);


	if ( !strcmp(mod,"gui") ) {    /* check GUI set */
	  dt[ii] = btn[0];          /* redefine EC,CC,SC channel thresh. */
	  dw[ii] = btn[1];          /* redefine EC,CC,SC channel width   */
	}

	dt[ii] = (dt[ii]-8)*4095/1024;
	if ( dt[ii] <    0 )  dt[ii] =    0;
	if ( dt[ii] > 4095 )  dt[ii] = 4095;


	if ( dw[ii] <   5 )   dw[ii] =   5;
	if ( dw[ii] > 145 )   dw[ii] = 145;
	tempf  = log((float) (dw[ii]));
	tempf  = ((PW4*tempf+PW3)*tempf+PW2)*tempf+PW1;
	dw[ii] = (int) floor(tempf);
	if ( dw[ii] <    0 )  dw[ii] =    0;
	if ( dw[ii] > 4095 )  dw[ii] = 4095;
	/*
	dw[ii] = P1/dw[ii] - P2;
	*/


	q = 0;
	cdreg(&ext,db[ii],dc[ii],dn[ii],1);
	cfsa(16,ext,&dt[ii],&q);     /* setting the thresholds */
	if (!q)
	  printf("b=%d, c=%d, n=%d, a=1, camac set threshold error\n",
		 db[ii], dc[ii], dn[ii]);

	q = 0;
	cdreg(&ext,db[ii],dc[ii],dn[ii],2);
	cfsa(16,ext,&dw[ii],&q);     /* setting the widths */
	if (!q)
	  printf("b=%d, c=%d, n=%d, a=2, camac set width error\n",
		 db[ii], dc[ii], dn[ii]);
      }
    } /* end of EC,CC,SC ch.discr. setting mode */

  } /* end of EC,CC,SC channel discriminators */

  return(0);   /* end of "ChDiscr_setmon()" */
}


int
takeOFFinhibit(int *bb, int *cc, int kk)
{
  unsigned int bc = 0x0;
  int ii, bj, cj, ext, q;

  for (ii=0; ii<kk; ii++)  bc |= (1 << (8*bb[ii] + cc[ii]));

#ifdef SP_DEBUG_INH
  printf(" kk=%d,  ii=%d,  bc=0x%08x \n", kk, ii, bc);
#endif

  for (ii=0; ii<32; ii++) {
    if(bc&1) {
      bj = ii / 8;
      cj = ii % 8;
#ifdef SP_DEBUG_INH
      printf(" ii=%2d,  bj=%d,  cj=%d \n", ii, bj, cj);
#endif
      cdreg(&ext,bj,cj,30,0);
      cfsa(17,ext,&zero,&q);
    }
    bc >>= 1;  if(bc == 0)  break;
  }
}


#else

int
dimanc_dummy()
{
  exit(0);
}

#endif
