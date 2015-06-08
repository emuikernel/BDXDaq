#ifndef bostypes_h
#define bostypes_h




/*
  Philosophy: 
  
  To provide a C wrapper for BOS, by providing structures
  for each BOS bank and routines to communicate with BOS using these 
  structures. Ideally, one would have to know nothing about BOS
  calls to write C code using the structures and subroutines prototyped
  in this header file.

  Implementation:

  In general, each BOS bank has an associated structure denoted
  by clas<BANKNAME>_t ( the _t dennotes that it is a type). 
  For example: the DC0 bos bank has a structure of type clasDC0_t
  associated with it. The type clasDC0_t consists of a bankHeader_t
  structure containing bank header information common to all BOS banks
  and an array of structures of type dc0_t containing the DC0 bank
  information. Typically, one might write some C code like:

  clasDC0_t *DC_bank;
  char *filname = "fpack_file";

  initBOS();
  read_ddl();
  open_fpack(filename);
  
  if (GetData()){
    DC_bank = (clasDC0_t *)getBank(&bcs_, "DC0 ");
  }

  from the pointer returned from the call to getBank you can now access
  the BOS common directly through a structure.
  */


 /* FPACK structures */
typedef struct {
  char name[8];
  int numa;
  int numb;
  int count;
  int RecClass;
} recordHeader_t;

typedef struct{
  char name[8];
  int numb;
  int ncol;
  int nrow;
  char form[4];
} recordBlock_t;

typedef enum {Sequential,Directory,Keyed,Special,Ordered} FileAccess_t;
typedef enum {Binary,Text} FileForm_t;
typedef enum {Read,Write,ReadWrite,Modify} FileAction_t;

typedef struct {
  int UnitNo;
  FileAccess_t Access;
  FileForm_t Form;
  FileAction_t Action;
  int PhysicalRecLength;
  int nPhysRecRead;
  int nPhysRecWritten;
  int nLogicalRecRead;
  int nLogicalRecWritten;
  int nKbytesRead;
  int nKbytesWritten;
  int dummy1;
  int dummy2;
  int AveLogRecLen;
  int AveLogRecLenWritten;
  int nBlocksRead;
  int nBlocksWritten;
  int nBlocksWrttenNoFormat;
  int dummy3[12];
} FileParameters_t;

  

/*BOS bank stuctures to access the BOS common*/

/*Every BOS bank starts with a header followed by the contents of the
  bank most structures follow the form 

  BOS BANK
  ---------------------   
  bankHeader_t                   
  ---------------------          \
  bank structure example: dc0_t   | 
  ---------------------           | number of bank structures equal
  bank structure                  | to bank->nrow
  ---------------------           |
  etc..                          /
  --------------------

  all these definitions are manufactured by 
  the perl scrpt ddl2h and are located in the header file bosdll.h
  */

#ifndef bosBankHeader_DEFINED
#define bosBankHeader_DEFINED
typedef struct {
  int ncol;
  int nrow;
  char name[4];
  int nr;
  int nextIndex;
  int nwords;
} bankHeader_t;
#endif

/*----------------BANK DEFINITIONS-------------*/
/* ALL BANKS DEFINED IN the ddl file are included here*/
#include <bosddl.h>

/* Special bank definition for the ROC data. */

typedef struct {
  int data;
}rc_t;

typedef struct {
  bankHeader_t bank;
  rc_t         rc[1];
}clasRC_t;
/*-------------------END OF BANK DEFINITIONS---------*/

#define PRESTART_EVENT 17
#define GO_EVENT 18
#define PHYSICS_1_EVENT 1

#define NDIM 700000

typedef struct bosbank {
  int ipad[5];
  int iw[NDIM];
} BOSbank;

extern BOSbank bcs_;
extern BOSbank wcs_; 


/*------------Prototypes: functions defined in c_bos_io--------------*/

/*call initbos() to set up BOS fundamental and work areas
  i.e. sets up the bcs_ and wcs_ arrays */
void initbos();

/* read ddl file of bank prototypes - wrapper for call to txt2bos 
   - needs rewrite */
void read_ddl();

/*standard fparm_ call in C return value 1 if no error 0 if error*/
int fparm_c(char *mess);

/* wrapper for frbos_ call return 0 if success -1 if EOF*/
int getBOS(BOSbank *bcs, int lun, char *list);

/*wrapper for frbos_ call return 1 if success a 0 if error*/
int putBOS(BOSbank *bcs, int lun, char *list);

/* get the first of a  particular bank*/
/* the first argument is a pointer to the bos bank, the second
   is a charachter string (4 long) indicating the name of the bank
   example:
   clasCC_t *CC = NULL;
   CC = (clasCC_t *)getBank(&bcs_,"CC  "));
   */
void *getBank(BOSbank *,const char *);

/* given a bank return a pointer to the next bank or NULL if no next bank*/
void *getNextBank(BOSbank *bcs, bankHeader_t *bank);

/*get a specific bank*/
void *getGroup(BOSbank *, const char *name, int j);

/*make a new bank*/
void *makeBank(BOSbank *bcs, char *bankname, int banknum, int ncol, int nrow);

/*Drop a bank*/
void *dropBank(BOSbank *bcs, char *bankname, int banknum);

/*Drop a list of banks*/
void dropAllBanks(BOSbank *bcs, char *list);

/* clean up a BOS common */
void cleanBanks(BOSbank *bcs);

/*format a new bank*/
void formatBank(char *bankname, char *bankformat);

/*wrapper for call to blist_ */
void bankList(BOSbank *bcs, char *list, char *banklist);

/* open a fpack format file for reading*/
void open_fpack(char *filename);

/* get a BOS type event returns 1 if success returns 0 if fail (eof)*/
int GetData();

int fileLength(char *datafile);

FileParameters_t fileQuery(char *datafile);


#endif


