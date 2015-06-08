
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <errno.h>

#define NBCS 10000000
#include "bosio.h"
#include "bos.h"
#include "bcs.h"

#include "pedo.h"

extern FILE *pedlog; /* defined in pedo.c */

/* slightly modified demux_roc.c */
/*sergey: following routine from c_bos_io/demux_roc.c,
with some modifications*/
ROC_DMUX_t *
alloc_decode_roc(int mode)  /* Mode:  >0 Allocate a demuxing table. */
                                        /* Mode:  <0 Allocate a muxing table. NOT IMPLEMENTED. */
{
  /* This routine allocates memory for the ROC_DMUX_t structure. */ 
     
  ROC_DMUX_t *a;
  int i;
  
  if(mode==0)mode=1;
  if(mode>0){
    a = (ROC_DMUX_t *)malloc(sizeof(ROC_DMUX_t)*mode);
    for(i=0;i<26;i++)
      a->mod[i]=NULL;
  }
  else
    a=NULL;      /* Not yet implemented */
  
  return(a);
}
char *
quoted_strtok(char *instr,const char *tokenstring,const char *sptokstring)
{
  static char *save;
  char *p,*q;
  char *nexttok;

  char tokenstr[MAX_STR_LEN],sptokstr[MAX_STR_LEN];

  strcpy(tokenstr,tokenstring);
  strcpy(sptokstr,sptokstring);

  if(instr!=NULL)save = instr;

  while(1)
    {
      if(save[0]=='\0')      /* No token found. */
	{
	  return(NULL);
	}
    if(strchr(sptokstr,save[0])!=NULL) /* first "" */
	{
	  nexttok = sptokstr;
	  if(save[1]!=0)save++;
	  else return(NULL);
	  break;                  /* Special char. */
	}
      if(strchr(tokenstr,save[0])==NULL) /* first char not in token str */
	{
	  nexttok = tokenstr;
	  break;
	}
      save++;
    }
  
  p = save;

  while(2)
    {
      if(p[0]=='\0')      /* No second token found. */
	{
	  q = save;
	  save = p;
	  return(q);
	}
      if(strchr(nexttok,p[0])!=NULL) /* First char in token string */
	{
	  p[0]=0;
	  q = save;
	  save = p+1;
	  return(q);
	}
      p++;
    }

}      
char *
squeeze_space(char *string)
{
  int len,i,j;

  len = strlen(string);

  for(i=0;i<len;i++)
    {
      if(string[i]==' '||string[i]=='\t'||string[i]=='\n')
	{
	  for(j=i+1;j<=len;j++)string[j-1]=string[j];
	  i--;
	}
    }

  return(string);
}
int
kill_left_space(char *string)
{
  int loc,len,i;

  len =(int)strlen(string);           /* lenght of string */
  loc =(int)strspn(string," \t\12\n\f\v"); /* Find first char that is not space or tab,LF,NL etc */

  for(i=0;i<=(len-loc);i++) string[i]=string[i+loc];  /* move all chars up */
  
  return(len-loc);                    /* return new length */
}  
char *
fgets_clean(char *s, int size, FILE *pt)
{
  char *sx;
  int ch;
  int count=0;

  sx=s;
  while( (ch=fgetc(pt))!=EOF){
    if(ch=='#' || ch=='!'){                        /* Comment char found. */
      while( ((ch=fgetc(pt))!=EOF) && ch!='\n' );  /* Wind to end line. */
    }                                              /* if(ch=='\n') will finish up. */
    if(ch=='/')
      if( (ch=fgetc(pt))!=EOF && ch=='*'){         /* Real comment ? */
	while( (ch=fgetc(pt)) != EOF)              /* YES, Wind to next "*" */
	  if(ch=='*')                              /* End comment ? */
	    if( ((ch=fgetc(pt))!=EOF) && ch=='/')  /* YES ! */
	      continue;                            /* continue with next character. */
      }	                                            
      else{                                         /* No, just a 'loose' "/" */
	*sx='/'; sx++; count++;                     /* Recover the "/" by pushing it onto s */
	if(count>=size-1){                          /* OOPS, string full. */
	  *sx=0;
	  return(s);
	}
      }
                                                      /* Not a Comment. */
    if(ch=='\n'){                                   /* We are done with the line. */
      *sx=ch; sx++; count++;
      *sx=0;
      return(s);
    }
    *sx=ch; sx++; count++;
    if(count>=size-1){                          /* OOPS, string full. */
      *sx=0;
      return(s);
    }    
  }
}
int
read_n_tokens(int num_tokens,char **token,FILE *infp)
{
  char string[MAX_STR_LEN];
  char *tmptok;
  int i;
  int indx,loc,len;
  int continue_count;

  continue_count = 0;
  indx = 0;

  for(i=0;i<num_tokens;i++)
    {
      if(token[i]==NULL)token[i]=malloc(TOKEN_DEPTH);
    }

  while(!feof(infp)&&indx<num_tokens)
    {
      fgets_clean(string,MAX_STR_LEN,infp); /* Get a line after comments are removed. */
      if(feof(infp))
	{
	  return(0);
	}
      if(ferror(infp))
	{
	  fprintf(pedlog,"\nERROR- occurred while reading parameters.(read-n-tokens)\n\n");
	  exit(1);
	}

      len = kill_left_space(string);   /* Kill all white space on left */
      if(len<=0)continue;              /* line is empty. */
      
      tmptok = quoted_strtok(string,TOKEN_SEP,QUOTE_SEP);
      strncpy(token[continue_count],tmptok,TOKEN_DEPTH-1);
      squeeze_space(token[continue_count]);

      indx = continue_count+1;
      
      while(indx<num_tokens)
	{
	  tmptok =  quoted_strtok(NULL,TOKEN_SEP,QUOTE_SEP);
	  if(tmptok == NULL)          /* Continue on next line */
	    {
	      continue_count=indx;
	      break;
	    }	  
	  strncpy(token[indx],tmptok,TOKEN_DEPTH-1);
	  squeeze_space(token[indx]);
	  indx++;
	}
    }

  return(indx-1);
}
void
init_decode_roc(char * rocfile,    /* The name (+path) of the ROCxx.tab file. */
                ROC_DMUX_t *rtab,  /* Output demuxing table, or NULL          */
                ROC_MUX_t *dtab)   /* NULL (NOT IMPLEMENTED= always NULL )    */
{
  /* Initialize the decoding tables by reading the ROCxx.tab file. 
   * 
   * Search algorithm for this file is:
   *      A) Current directory.
   *      B) $CLON_PARMS/TT 
   *      C) $CLAS_PARMS/TT
   *
   * Note that on the CLON cluster there is a lot of confusion as of which directory
   * the ROCxx.tab files belong. It is safest to specify a full path name.
   *
   * Muxing (i.e. creating a RAW bank from a coocked one, is not implemented, and won't be
   *         for a while. If you REALLY REALLY need it, contact me.... )
   */

  FILE *fpt;
  char *envdir;
  char *envdir2;
  char line[256];
  char line2[256];
  char *tokens[MAX_NUM_TOK];
  int isector,ilayer,islab,iplace,icrate,islot,itype,ichan;
  int idummy;
  int i;

  for(i=0;i<MAX_NUM_TOK;i++)tokens[i]=malloc(sizeof(char)*TOKEN_DEPTH);
  memset(line,0,sizeof(line));
  memset(line2,0,sizeof(line2));

/* Find the file with the decoding information. */

printf("init_decode_roc reached: access to >%s< = %d\n",rocfile,access(rocfile,4));
if(access(rocfile,4)==0) printf("access permitted\n");
else                     printf("access denied\n");

  if( strchr(rocfile,'/') == NULL && access(rocfile,4)==EOF){  
                                      /* Is rocfile a path + filename ? */
                                      /* No, search . $CLON_PARMS and $CLAS_PARMS */
    envdir=getenv("CLON_PARMS");      /* Not in . , check environments next. */
    envdir2=getenv("CLAS_PARMS");
      
    if(envdir!=NULL){        /* We have a $CLON_PARMS so look in TT subdir */
      strcpy(line,envdir);
      strcat(line,"/TT/");
      strcat(line,rocfile);
    }
    if(envdir2!=NULL){       /* We have a $CLAS_PARMS so look in TT subdir */
      strcpy(line2,envdir2);
      strcat(line2,"/TT/");
      strcat(line2,rocfile);
    }
    else if(envdir==NULL){   /* envdir==NULL && envdir2==NULL */
      fprintf(pedlog,"Neither CLAS_PARMS nor CLON_PARMS is defined, I can not find the file %s\n",rocfile);
      exit(1);
    }
    
    if(access(line,4)!=EOF){
      fprintf(pedlog,"Retreiving %s from %s/TT \n",rocfile,envdir);
    }
    else if(access(line2,4)!=EOF){
      fprintf(pedlog,"Retreiving %s from %s/TT \n",rocfile,envdir2);
      strcpy(line,line2);
    }
    else {
      fprintf(pedlog,"Roc table file %s not was not found in ./ , CLON_PARMS/TT or CLAS_PARMS/TT\n",rocfile);
      exit(1);
    }
  }
  else{
    strcpy(line,rocfile);   /* rocfile contains full path + filename */
  }

/* Found the file, now open it and read in the information. */
  
  if( (fpt = fopen(line,"r"))==NULL){
    fprintf(pedlog,"Could not open file: %s \n",line);
    exit(1);
  }    
  while(!feof(fpt)){
    if(read_n_tokens(MAX_NUM_TOK,tokens,fpt)<10){   /* Read one line and parse tokens on the line.*/
      if(feof(fpt))break;
      else{
	fprintf(pedlog,"Error reading ROC file. \n");
	exit(1);
      }
    }
    if(rtab!=NULL){  /* Fill demuxing table */
      sscanf(tokens[1 ],"%d",&isector);
      sscanf(tokens[2 ],"%d",&ilayer);
      sscanf(tokens[3 ],"%d",&islab);
      sscanf(tokens[4 ],"%d",&iplace);
      sscanf(tokens[5 ],"%d",&idummy);
      sscanf(tokens[6 ],"%d",&idummy);
      sscanf(tokens[7 ],"%d",&icrate);
      sscanf(tokens[8 ],"%d",&islot);
      sscanf(tokens[9 ],"%d",&itype);
      sscanf(tokens[10],"%d",&ichan);  
      if(rtab->mod[islot] == NULL){                           /* First encounter of this module. */
	rtab->mod[islot]= malloc(sizeof(ROC_DMUX_MODULE_t));  /* Need to allocate space for it.  */
	rtab->mod[islot]->nummod=islot;     /* Redundant, but safe. */
	rtab->crate     = icrate;           /* Keep track of crate. */
                                            /* NOTE: at this point crates can not be mixed. */

	for(i=0;i<64;i++){                    /* INITIALIZE ! */
	  rtab->mod[islot]->ch[i].sector= 0;  /* Set all channels to undefined. */
	  rtab->mod[islot]->ch[i].layer = 0;  /* Set all channels to undefined. */
	  rtab->mod[islot]->ch[i].slab  = 0;  /* Set all channels to undefined. */
	  rtab->mod[islot]->ch[i].place=  -1; /* Set all channels to undefined. */
	}

	switch(itype){
	case 1872:             /* TDC */
	  idummy = 1;
	  break;
	case 1877:             /* Wire chamber TDC */
	  idummy = 2;
	  break;
	case 1881:
	  idummy = 0;           /* ADC */
	  break;
	default:
	  idummy = 255;
	}
	rtab->mod[islot]->type = (unsigned short)idummy;
      }
      else
	if(rtab->crate != icrate){
	    fprintf(pedlog,"Sorry, but you can not mix crates in a ROC definition file.\n");
	  exit(1);
	}

      strncpy(rtab->mod[islot]->ch[ichan].bankname,tokens[0],4);
      rtab->mod[islot]->ch[ichan].sector= (unsigned short)isector;
      rtab->mod[islot]->ch[ichan].layer  = (unsigned short)ilayer;
      rtab->mod[islot]->ch[ichan].slab  = (unsigned short)islab;
      rtab->mod[islot]->ch[ichan].place = (unsigned short)iplace;
    }
    if(dtab!=NULL){ /* Muxing table, for going the other direction. */
	  fprintf(pedlog,"Very sorry, but the translation from bos->raw is not finished. \n");
      exit(1);
    }
  }
}

int
decode_raw_data(clasRC_t *RC, int n_call, ROC_DMUX_t *RC_look,
		        int tdc_slot_low, int tdc_slot_high, RAW_DATA_t *R)
{
/*
 * This routine decodes the RAW data from a TDC or ADC. ( As of now, the 
 * Multi-Hit TDC use in DC are not supported.)
 * This works different for each module type, so there is specific code for 
 * each module type. 
 * It is important to note that because the ADC's have header words (which are
 * quite useless to us) we need to keep count of adc entries, and the RC bank
 * must be called in sequence for the ADCs.
 *
 * Thus: THIS ROUTINE MUST BE CALLED WITH THE ENTIRE BANK IN SEQUENCE.
 *       RUNNING n_call FROM 0 to RC->bank.nrow-1  (for ADC's) !!!
 *
 * Arguments:
 *
 *    *RC            = Raw data pointer as defined in bostypes.h
 *    n_call         = The row entry in RC, must be called in order for ADC's
 *    *RC_look       = Pointer to Structure containing all the crate info. 
 *                     Defined in demux_roc.h
 *    tdc_slot_low   = If RC_look==NULL this determines the lowest slot number 
 *                     with a TDC.
 *    tdc_slot_high  = If RC_look==NULL this determines the higher slot number 
 *                     with a TDC.
 *    *R             = Pointer to output Raw structure.
 *
 */

  static int is_channel_flag; /* Static, to save from call to call. */
  int dat;
  int iwc;

  if(n_call==0) /* initialization code for new RC bank. */
  {
    is_channel_flag=0;
  }
  
  dat = RC->rc[n_call].data;         /* Get the raw data (an ADC or TDC word. */
		
  R->islot= (dat>>27)&0x1F;      /*  However, it looks way confusing... */
  iwc  = (dat)&0x007F;
	    
/* Determine what type of module is in this slot. 
 * This is needed because the modules decode
 * differently. We are mostly interested in ADC's but I'll retain code for TDC also.
 * If the Translation Table is not availabe, we can't determine what type a slot is !
 * In that case, use the simple rule that ADCs are on the outside ( slot<8 or slot>17)
 */		

  if(R->islot<1 || R->islot>25)
  {
    printf("\nDATA ERROR: Data decodes to non existing slot # %d (decode_raw_data)\n",
	    R->islot);
    return(3);          /* Serious Problem, probably should quit here. */
  }

  if(RC_look != NULL)            /* Find what module it is from the TT */
  {
    if(RC_look->mod[R->islot] != NULL)
    {
      R->itype = RC_look->mod[R->islot]->type;
    }
    else
    {
      printf("\nTT Table ERROR: Slot not in tables: %d (decode_raw_data)\n\n",
	      R->islot);
      return(3);          /* Serious Problem, probably should quit here. */
    }
  }
  else  /* Don't have TT, so assume crate info is ok. */
  {
    if(R->islot>= tdc_slot_low && R->islot<= tdc_slot_high)
    {
	  R->itype =1;		      
    }
    else
    {
	  R->itype =0;
    }
  }

  if(R->itype==1)           /* It's a TDC */
  {
    R->ichan= (dat>>16)&0x3F;
    R->idat = (dat)&0x0FFF;  /* Only use 12 bits */
  }
  else                    /* Is ADC */
  {
      
  /* The first part of this code identifies the header words, which are ignored. 
   * Each header word contains the number of real words that follow. 
   * We just count down.
   */
    R->ichan= (dat>>17)&0x3F;		
    R->idat = (dat)&0x3FFF;	
    if(is_channel_flag==0 )  /* Is an ADC HEADER */
    {
	  if(iwc>0)  /* Next entry will be a channel. */
	  {
	    is_channel_flag = iwc-1;
	  }
 	  R->idat = (int)iwc-1;
	  /*printf("\nSkip histogramming\n");*/
	  return(1);   /* Skip the histogramming ! */
    }
    else /* Not a header, so use the data. */
    {
	  is_channel_flag--;       /* Count down to next header or TDC. */
	
	  if(R->islot<1 || R->islot >25)
      {
	    printf("\nIllegal Slot: %d\n",R->islot);
	    return(2);   /* Minor error. Recoverable. */
	  }
	  if(R->ichan<0 || R->ichan >=64)   /* Check for errors in data. */
	  {
	    printf("\nIllegal Channel %d, for Slot %d \n",R->ichan,R->islot);
	    return(2); /* Minor error, recoverable. */
	  }
    }
  }

  return(0);
}

