h29416
s 00056/00026/00172
d D 1.2 03/04/17 16:51:16 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 01/11/19 18:58:49 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/recsis.s/readbos.c
e
s 00198/00000/00000
d D 1.1 01/11/19 18:58:48 boiarino 1 0
c date and time created 01/11/19 18:58:48 by boiarino
e
u
U
f e 0
t
T
I 1
D 3
#include <string.h>
E 3
I 3

E 3
#include <stdio.h>
I 3
#include <string.h>
E 3
#include <stdlib.h>
#include <errno.h>
D 3
#include <ntypes.h>
#include <bostypes.h>
E 3

I 3
#include "ntypes.h"
#include "bostypes.h"
E 3

D 3
void initbos()
E 3
I 3

void
initbos()
E 3
{
  int nbcs = 700000 , nwbcs = NDIM;
  
  bos_(bcs_.iw, &nbcs);
  bos_(wcs_.iw, &nwbcs);
}

/* this should be the standard open and close routine
 returns 0 if failure 1 if success */
D 3
int fparm_c(char *mess)
E 3
I 3
int
fparm_c(char *mess)
E 3
{
  char msg[73];
  int one = 1,i;

  fparm_(mess,strlen(mess));

  /*  for(i=1; i<1000; i++) msg[i] = ' '; */
  fermes_(msg,&one,sizeof(msg));
  msg[sizeof(msg) - 1] = '\0';
  if(msg[0] != ' ') {
    fprintf(stderr, "%s\n",msg);
    return(0);
  }
  return(1);
}

/* A read routine with less problems than fseqr but reqires a unit number*/
D 3
int getBOS(BOSbank *bcs, int lun, char *list)
E 3
I 3
int
getBOS(BOSbank *bcs, int lun, char *list)
E 3
{
  int ierr = 0;
  frbos_(bcs->iw, &lun, list, &ierr, strlen(list));
  return(!ierr);
}

/* A write routine with less problems than fseqr but reqires a unit number*/
D 3
int putBOS(BOSbank *bcs, int lun, char *list)
E 3
I 3
int
putBOS(BOSbank *bcs, int lun, char *list)
E 3
{
  int ierr = 0;
  fwbos_(bcs->iw, &lun, list, &ierr, strlen(list));
  return(!ierr);
}

D 3
void *getBank(BOSbank *bcs,const char *name)
E 3
I 3
void *
getBank(BOSbank *bcs,const char *name)
E 3
{
#define BOSNAMELENGTH 4
  int name_index; 
  bankHeader_t *hdr;
  char BankName[5];
  char *Bptr = BankName;
  strcpy(BankName,"    ");

  while (*name)
    *Bptr++ = *name++;

  name_index = mamind_(bcs->iw,BankName,strlen(BankName));

  hdr = (bankHeader_t *) &(bcs->iw[bcs->iw[name_index - 1] - 6]);
  return(hdr->nwords ? (void *) hdr : 0);

}

D 3
void *getNextBank(BOSbank *bcs, bankHeader_t *bank)
E 3
I 3
void *
getNextBank(BOSbank *bcs, bankHeader_t *bank)
E 3
{
  bankHeader_t *NextBank;

  if(bank->nextIndex){
    NextBank = (bankHeader_t *) &(bcs->iw[bank->nextIndex - 6]);
  } else {
    NextBank = NULL;
  }
  return((void *)NextBank);
}

D 3
void *getGroup(BOSbank *bcs, const char *name, int j)
E 3
I 3
void *
getGroup(BOSbank *bcs, const char *name, int j)
E 3
{
I 3
  int ind;
/*printf("+1\n");fflush(stdout);*/
E 3

D 3
  int ind =  mlink_(bcs->iw, name,&j,strlen(name));
E 3
I 3
  /*ind = mlink_(bcs->iw, name, &j, strlen(name));*/
  ind = etNlink(bcs->iw, name, j);

/*printf("+2\n");fflush(stdout);*/
E 3
  /*fprintf(stderr, "\nj = %d, ind = %d\n", j, ind);*/
  return( ind ? (void *) &(bcs->iw[ind - 6]) : NULL);

}

D 3
void *makeBank(BOSbank *bcs, char *bankname, int banknum, int ncol, int nrow)
E 3
I 3
void *
makeBank(BOSbank *bcs, char *bankname, int banknum, int ncol, int nrow)
E 3
{
  int ind = 0;

D 3
  ind = mbank_(bcs->iw, bankname, &banknum, &ncol, &nrow, strlen(bankname));
E 3
I 3
  /*ind = mbank_(bcs->iw, bankname, &banknum, &ncol, &nrow, strlen(bankname));*/
  ind = etNcreate(bcs->iw, bankname, banknum, ncol, nrow);

E 3
  return( ind ? (void *) &bcs->iw[ind - 6] : NULL);
}

D 3
void *dropBank(BOSbank *bcs, char *bankname, int banknum)
E 3
I 3
void *
dropBank(BOSbank *bcs, char *bankname, int banknum)
E 3
{
  int ind = 0;
  ind = mdrop_(bcs->iw, bankname, &banknum, strlen(bankname));
  return( ind ? (void *) bcs->iw[ind - 6] : NULL);
}

D 3
void bankList(BOSbank *bcs, char *list, char *banklist)
E 3
I 3
void
bankList(BOSbank *bcs, char *list, char *banklist)
E 3
{
  blist_(bcs->iw, list, banklist, strlen(list), strlen(banklist));
}

D 3
void dropAllBanks(BOSbank *bcs, char *list)
E 3
I 3
void
dropAllBanks(BOSbank *bcs, char *list)
E 3
{
  bdrop_(bcs->iw, list, strlen(list));
}

D 3
void cleanBanks(BOSbank *bcs)
E 3
I 3
void
cleanBanks(BOSbank *bcs)
E 3
{
  bgarb_(bcs->iw);
}

D 3
void formatBank(char *bankname, char *bankformat)
E 3
I 3
void
formatBank(char *bankname, char *bankformat)
E 3
{
  mkfmt_(bcs_.iw, bankname, bankformat, strlen(bankname), strlen(bankformat));
}

D 3
void open_fpack(char *filename)
E 3
I 3
void
open_fpack(char *filename)
E 3
{
  char mess[1024];

  sprintf(mess," OPEN BOSINPUT FILE=\"%s\"  READ",filename);
  fparm_(mess,strlen(mess));

}

D 3
int GetData()
E 3
I 3
int
GetData()
E 3
{
  int iret = 0;
  char *logical = "BOSINPUT";
  fseqr_(logical,&iret,strlen(logical));
  return(iret >= 0);
}

D 3
void open_fpack_unit(char *filename,char *dataname,int unitnum)
E 3
I 3
void
open_fpack_unit(char *filename,char *dataname,int unitnum)
E 3
{
  char mess[1024];

    sprintf(mess," OPEN %s UNIT=%3d FILE=\"%s\" READ STATUS=OLD", dataname, unitnum, filename);
    fparm_(mess,strlen(mess));
}

D 3
void close_fpack_unit(char *dataname)
E 3
I 3
void
close_fpack_unit(char *dataname)
E 3
{
  char mess[128];
  
  sprintf(mess," CLOSE %s",dataname);
  fparm_(mess,strlen(mess));
}

D 3
void rewind_fpack_unit(char *dataname)
E 3
I 3
void
rewind_fpack_unit(char *dataname)
E 3
{
  char mess[128];
  
  sprintf(mess," REWIND %s",dataname);
  fparm_(mess,strlen(mess));
}

D 3
int GetData_unit(char *dataname)
E 3
I 3
int
GetData_unit(char *dataname)
E 3
{
  int iret = 0;
  fseqr_(dataname,&iret,strlen(dataname));
  return(iret >= 0);
}

D 3
int fileLength(char *datafile)
E 3
I 3
int
fileLength(char *datafile)
E 3
{

#define NAMELEN 100

  char filename[NAMELEN],hostname[NAMELEN];
  FileParameters_t fileParameters;
  int ierr;

  fqfile_(datafile,filename,hostname,&fileParameters,&ierr,strlen(datafile),NAMELEN,NAMELEN);

  return(fileParameters.nKbytesWritten);

}

D 3
FileParameters_t fileQuery(char *datafile)
E 3
I 3
FileParameters_t
fileQuery(char *datafile)
E 3
{
 char filename[NAMELEN],hostname[NAMELEN];
 FileParameters_t fileParameters;
 int ierr;

 fqfile_(datafile,filename,hostname,&fileParameters,&ierr,strlen(datafile),NAMELEN,NAMELEN);
 return(fileParameters);
}
E 1
