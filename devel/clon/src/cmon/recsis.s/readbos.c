
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "ntypes.h"
#include "bostypes.h"


void
initbos()
{
  int nbcs = 700000 , nwbcs = NDIM;
  
  bos_(bcs_.iw, &nbcs);
  bos_(wcs_.iw, &nwbcs);
}

/* this should be the standard open and close routine
 returns 0 if failure 1 if success */
int
fparm_c(char *mess)
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
int
getBOS(BOSbank *bcs, int lun, char *list)
{
  int ierr = 0;
  frbos_(bcs->iw, &lun, list, &ierr, strlen(list));
  return(!ierr);
}

/* A write routine with less problems than fseqr but reqires a unit number*/
int
putBOS(BOSbank *bcs, int lun, char *list)
{
  int ierr = 0;
  fwbos_(bcs->iw, &lun, list, &ierr, strlen(list));
  return(!ierr);
}

void *
getBank(BOSbank *bcs,const char *name)
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

void *
getNextBank(BOSbank *bcs, bankHeader_t *bank)
{
  bankHeader_t *NextBank;

  if(bank->nextIndex){
    NextBank = (bankHeader_t *) &(bcs->iw[bank->nextIndex - 6]);
  } else {
    NextBank = NULL;
  }
  return((void *)NextBank);
}

void *
getGroup(BOSbank *bcs, const char *name, int j)
{
  int ind;
/*printf("+1\n");fflush(stdout);*/

  /*ind = mlink_(bcs->iw, name, &j, strlen(name));*/
  ind = etNlink(bcs->iw, name, j);

/*printf("+2\n");fflush(stdout);*/
  /*fprintf(stderr, "\nj = %d, ind = %d\n", j, ind);*/
  return( ind ? (void *) &(bcs->iw[ind - 6]) : NULL);

}

void *
makeBank(BOSbank *bcs, char *bankname, int banknum, int ncol, int nrow)
{
  int ind = 0;

  /*ind = mbank_(bcs->iw, bankname, &banknum, &ncol, &nrow, strlen(bankname));*/
  ind = etNcreate(bcs->iw, bankname, banknum, ncol, nrow);

  return( ind ? (void *) &bcs->iw[ind - 6] : NULL);
}

void *
dropBank(BOSbank *bcs, char *bankname, int banknum)
{
  int ind = 0;
  ind = mdrop_(bcs->iw, bankname, &banknum, strlen(bankname));
  return( ind ? (void *) bcs->iw[ind - 6] : NULL);
}

void
bankList(BOSbank *bcs, char *list, char *banklist)
{
  blist_(bcs->iw, list, banklist, strlen(list), strlen(banklist));
}

void
dropAllBanks(BOSbank *bcs, char *list)
{
  bdrop_(bcs->iw, list, strlen(list));
}

void
cleanBanks(BOSbank *bcs)
{
  bgarb_(bcs->iw);
}

void
formatBank(char *bankname, char *bankformat)
{
  mkfmt_(bcs_.iw, bankname, bankformat, strlen(bankname), strlen(bankformat));
}

void
open_fpack(char *filename)
{
  char mess[1024];

  sprintf(mess," OPEN BOSINPUT FILE=\"%s\"  READ",filename);
  fparm_(mess,strlen(mess));

}

int
GetData()
{
  int iret = 0;
  char *logical = "BOSINPUT";
  fseqr_(logical,&iret,strlen(logical));
  return(iret >= 0);
}

void
open_fpack_unit(char *filename,char *dataname,int unitnum)
{
  char mess[1024];

    sprintf(mess," OPEN %s UNIT=%3d FILE=\"%s\" READ STATUS=OLD", dataname, unitnum, filename);
    fparm_(mess,strlen(mess));
}

void
close_fpack_unit(char *dataname)
{
  char mess[128];
  
  sprintf(mess," CLOSE %s",dataname);
  fparm_(mess,strlen(mess));
}

void
rewind_fpack_unit(char *dataname)
{
  char mess[128];
  
  sprintf(mess," REWIND %s",dataname);
  fparm_(mess,strlen(mess));
}

int
GetData_unit(char *dataname)
{
  int iret = 0;
  fseqr_(dataname,&iret,strlen(dataname));
  return(iret >= 0);
}

int
fileLength(char *datafile)
{

#define NAMELEN 100

  char filename[NAMELEN],hostname[NAMELEN];
  FileParameters_t fileParameters;
  int ierr;

  fqfile_(datafile,filename,hostname,&fileParameters,&ierr,strlen(datafile),NAMELEN,NAMELEN);

  return(fileParameters.nKbytesWritten);

}

FileParameters_t
fileQuery(char *datafile)
{
 char filename[NAMELEN],hostname[NAMELEN];
 FileParameters_t fileParameters;
 int ierr;

 fqfile_(datafile,filename,hostname,&fileParameters,&ierr,strlen(datafile),NAMELEN,NAMELEN);
 return(fileParameters);
}
