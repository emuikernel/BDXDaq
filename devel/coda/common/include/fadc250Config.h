/****************************************************************************
 *
 *  fadc250Config.h  -  configuration library header file for fADC250 board 
 *
 *  SP, 07-Nov-2013
 *
 */


#define FNLEN     128       /* length of config. file name */
#define STRLEN    250       /* length of str_tmp */
#define ROCLEN     80       /* length of ROC_name */
#define NBOARD     21
#define NCHAN      16


/** FADC250 configuration parameters **/
typedef struct {
  int f_rev;
  int b_rev;
  int b_ID;

  int          mode;
  unsigned int winOffset;
  unsigned int winWidth;
  unsigned int nsb;
  unsigned int nsa;
  unsigned int npeak;

  unsigned int chDisMask;
  unsigned int trigMask;
  unsigned int thr[NCHAN];
  unsigned int dac[NCHAN];
  float        ped[NCHAN];
  unsigned int thrIgnoreMask;
  float gain[NCHAN];

} FADC250_CONF;


/* functions */

void fadc250InitGlobals();
int fadc250ReadConfigFile(char *filename);
int fadc250DownloadAll();
int fadc250Config(char *fname);
void fadc250Mon(int slot);
